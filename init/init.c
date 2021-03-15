/*
	init/init.c
	Copyright (C) 2020  Quasimodo

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
#include <stddef.h>
#include <stdio.h>
#include <lava.h>
#include <kernel.h>
#include <arch.h>
#include <keyboard.h>
#include <fs.h>
#include <mm.h>
#include <int.h>
#include <graph.h>
#include <fctrl.h>
#include <buffer.h>
#include <pe.h>

struct _TASK_{
	int lock;
	int need;//多个项目的need字段组成一个邻接矩阵，构成网，网中不能有回路
	void (*construction)();
	//void (*destruction)();
	char * cstr;
	//char * dstr;
};

void paging_init_ap();
int paging_init_bp(u64 memory_start,u64 memory_size);
void sdt_init_bp();
void irq_init();
void sdt_init_ap();
void lapic_init();
void apic_enable();
void mp_init();
void timer_init();
void ker_heap_init();
void schedule_init(int (*entry)(void*));
void schedule_init_ap(int (*entry)(void*));
void fs_init();
void arch_init_ap();
void private_data_init();
void syscall_init();
void buf_init();
void file_table_init();


void gui_init();
void pit_init();
void int86_init();
void pci_init();
void ps2_kbd_init();
void vbe_init();
void time_init();
void serial_init();

void back_to_user_mode(u64 rip,u64 rflags,u64 rsp,u64 cs);

static struct _TASK_ task[] = {
	{0,0,gui_init,"gui_init().\n"},
	{0,1,pit_init,"pit_init().\n"},
	{0,1,int86_init,"int86_init().\n"},
	{0,1,pci_init,"pci_init().\n"},
	{0,1,ps2_kbd_init,"ps2_kbd_init().\n"},
	{0,5,vbe_init,"vbe_init().\n"},
	{0,1,time_init,"time_init().\n"},
	{0,1,serial_init,"serial_init().\n"}
};
void __attribute__((noreturn)) shut_down(int type){
	bsync(NULL);
	while(1) halt();
}

static struct _PE_SECTION_ * section_base;
static int section_count;
struct _PE_SECTION_ * get_section_addr(const char * sname){
	char name_tmp[8];
	int i;

	if(!sname || !sname[0]) return NULL;
	i = 0;
	do{
		if(!sname[i]) break;
		name_tmp[i] = sname[i];
		i++;
	} while(i <8);
	if(i < 8){
		do{
			name_tmp[i] = 0;
			i++;
		} while(i < 8);
	}
	for(i = 0;i < section_count;i++){
		if(!memcmp(section_base[i].Name,name_tmp,8)) return section_base + i;
	}
	return NULL;
}

int test_thread(void * none){
	LPSTREAM test_file;
	char tmp[256];
	wprintk(L"测试线程执行成功了。\n");
	wait(1000);
	wprintk(L"等待了1秒钟。\n");
	test_file = open(L"/system/lava.sys",FS_OPEN_WRITE | FS_OPEN_READ,NULL,NULL);
	if(test_file) {
		wprintk(L"打开文件成功了。\n");
		wprintk(L"%P.\n",test_file);
		read(test_file,256,tmp);
		//ShowHexs(tmp,256/16);
		close(test_file);
	}
	else wprintk(L"打开文件失败了。\n");
	test_file = open(L"/system/test1.txt",FS_OPEN_READ | FS_OPEN_WRITE,NULL,NULL);
	if(test_file) {
		if(write(test_file,256,tmp)) printk("write file fail.\n");
		close(test_file);
	}
	test_file = open(L"/system/test2.txt",FS_OPEN_READ | FS_OPEN_WRITE,NULL,NULL);
	if(test_file) {
		if(write(test_file,256,tmp)) printk("write file fail.\n");
		close(test_file);
	}
	test_file = open(L"/system/test3.txt",FS_OPEN_READ | FS_OPEN_WRITE,NULL,NULL);
	if(test_file) {
		if(write(test_file,256,tmp)) printk("write file fail.\n");
		close(test_file);
	}
	shut_down(0);
	while(1) halt();
}
int test_thread2(void * none){
	int i = 0;
	LPSTREAM hd;
	struct _BUFFER_HEAD_ * bh1,*bh2;

	cli();
	hd = open(L"/.dev/ata0.dev",0,NULL,NULL);
	sti();
	if(hd){
		wprintk(L"Thread 2 open hd success.%P.\n",GetCurThread());
		cli();
		bh1 = bread(hd,0);
		sti();
		printk("thread 2,%P.\n",bh1);
		cli();
		bh2 = bread(hd,65536);
		sti();
		printk("thread 2,%P,%P.\n",bh1,bh2);	
	}
	while(1){
		//printk("On CPU %d,%d.\n",GetCPUId(),GetCurThread()->id);
		halt();
	}
	//while(1){
	//	wait(1000);
	//	i++;
	//	printk("tick:%d.i:%d.\n",ticks,i);
	//}
}
int test_thread3(void * none){
	int i = 0;
	LPSTREAM hd;
	struct _BUFFER_HEAD_ * bh1,*bh2;

	cli();
	hd = open(L"/.dev/ata0.dev",0,NULL,NULL);
	sti();
	if(hd){
		wprintk(L"Thread 3 open hd success.%P.\n",GetCurThread());
		cli();
		bh1 = bread(hd,65536);
		sti();
		printk("thread 3,%P.\n",bh1);
		cli();
		bh2 = bread(hd,0);
		sti();
		printk("thread 3,%P,%P.\n",bh1,bh2);
	}
	while(1){
		//printk("On CPU %d,%d.\n",GetCPUId(),GetCurThread()->id);
		halt();
	}
}
int init_thread_entry(void*none){
	int i,j;
	u64 * double_fault_stack;
	static volatile int finished = 0;
	static volatile int init = 0;

	sti();
	spin_unlock_bit(ADDRP2V(PROCESS_INIT_MUTEX),0);
	double_fault_stack = kmalloc(PAGE_SIZE,PAGE_SIZE);
	double_fault_stack += PAGE_SIZE/sizeof(u64);
	double_fault_stack--;
	write_private_dword(TSS.reg[ISTL(1)],(u64)double_fault_stack);
	write_private_dword(TSS.reg[ISTH(1)],(u64)double_fault_stack >> 32);

	*(u64*)PADDR2V(get_cr3()) = 0;
	for(i = 0;i < sizeof(task)/sizeof(struct _TASK_);i++){
		if(task[i].need){
			for(j = 0;j < 32;j++){
				if(bt(&task[i].need,j)) while(!bt(&(task[j].lock),1)) pause();
			}
		}
		if(!spin_try_lock_bit(&(task[i].lock),0)){
			task[i].construction();
			print(task[i].cstr);
			bts(&(task[i].lock),1);
			xaddd(&finished,1);
		}
	}
	if(!spin_try_lock_bit(&init,0)){
		while(finished < sizeof(task)/sizeof(struct _TASK_)) pause();
		wprintk(L"欢迎使用Lava OS。\n");
		create_thread(NULL,test_thread,NULL);
		//create_thread(NULL,test_thread2,NULL);
		//create_thread(NULL,test_thread3,NULL);
	}
	while(1) halt();
}
void __attribute__((noreturn)) INIT_CODE ap_entry_point(){//AP start at here
	paging_init_ap();
	sdt_init_ap();
	private_data_init();
	lapic_init();
	apic_enable();
	put_cr3(create_paging());
	//stop();
	schedule_init_ap(init_thread_entry);
}
void  __attribute__((noreturn)) INIT_CODE  entry_point(struct _MSG_ * msg){//BSP start at here
	struct _MZ_HEAD_ * head;
	struct _PEP_HEAD_ * pe_head;

	processor_count = 0;
	memcpy(&init_msg,msg,sizeof(struct _MSG_));
	head = ADDRP2V(init_msg.KernelImagePBase);
	pe_head = ADDRP2V(init_msg.KernelImagePBase + head->PEHead);
	section_base = (void*)(sizeof(struct _PEP_HEAD_) + (char*)(pe_head));
	section_count = pe_head->NumberOfSections;
	paging_init_bp(init_msg.MemoryStart + init_msg.ATACount * AHCI_PORT_SPACE_SIZE,init_msg.MemorySize);
	bts((void*)PROCESS_INIT_MUTEX,0);
	ker_heap_init();
	sdt_init_bp();
	private_data_init();
	syscall_init();
	mp_init();
	timer_init();
	buf_init();
	file_table_init();
	schedule_init(init_thread_entry);
}

void ShowHexs(void * _p,int Line){
	uint8_t * p = _p;
	int i;
	
	while(Line){
		for(i = 0;i < 16;i++){
			printk("%02X",p[i]);
			if(i == 7) putchar('-');
			else putchar(' ');
		}
		putchar('\n');
		Line--;
		p += 16;
	}
}