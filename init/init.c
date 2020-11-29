//Lava OS
//PageFault
//20-03-03-14-56
#include <stddef.h>
#include <stdio.h>
#include <lava.h>
#include <kernel.h>
#include <arch.h>
#include <keyboard.h>
#include <fs.h>
#include <mm.h>
#include <int.h>

struct _TASK_{
	int lock;
	int need;//多个项目的need字段组成一个邻接矩阵，构成网，网中不能有回路
	void (*task)();
	char * str;
};

void paging_init_ap();
int paging_init_bp(u64 memory_start,u64 memory_size);
void ker_stack_init();
void sdt_init_bp();
void sdt_init_ap();
u64 mmio_init();

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
void serial_init();

void ps2_kbd_init();
void int86_init();
void vbe_init();
void gui_init();
void pci_init();
void pit_init();
void time_init();

int getchar();

void back_to_user_mode(u64 rip,u64 rflags,u64 rsp,u64 cs);
extern u8 user_code[];
extern int user_code_size;

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
int lock = 0;

int test_thread(void * none){
	LPSTREAM serial;
	char buf[32];
	LPSTREAM kbd;

	serial = open(L".dev/serial/COM1.dev",0);
	kbd = open(L".dev/ps2_kbd.dev",0);
	
	//while(read(serial,32,buf));
	//write(serial,sizeof("This is a test.\n"),"This is a test.\n");
	
	while(1){
		halt();
		//printk("On CPU %d,%P.\n",GetCPUId(),GetCurThread());
		
	}
}

int init_thread_entry(void*none){
	volatile int i;
	int j;

	spin_unlock_bit(ADDRP2V(PROCESS_INIT_MUTEX),0);
	*(u64*)PADDR2V(get_cr3()) = 0;
	for(i = 0;i < sizeof(task)/sizeof(struct _TASK_);i++){
		if(task[i].need){
			for(j = 0;j < 32;j++){
				if(bt(&task[i].need,j)) while(!bt(&(task[j].lock),1)) nop();
			}
		}
		if(!spin_try_lock_bit(&(task[i].lock),0)){
			task[i].task();
			print(task[i].str);
			bts(&(task[i].lock),1);
		}
	}
	printk("Process %d activing.\n",GetCPUId());
	while(1) halt();
}
void __attribute__((noreturn)) ap_entry_point(){//AP start at here
	paging_init_ap();
	sdt_init_ap();
	private_data_init();
	SD();
	lapic_init();
	apic_enable();
	put_cr3(create_paging());
	schedule_init_ap(init_thread_entry);
}
void  __attribute__((noreturn)) entry_point(struct _MSG_ * msg){//BSP start at here
	
	processor_count = 0;
	memcpy(&init_msg,msg,sizeof(struct _MSG_));
	paging_init_bp(msg->MemoryStart + msg->ATACount * AHCI_PORT_SPACE_SIZE,msg->MemorySize);
	bts((void*)PROCESS_INIT_MUTEX,0);
	ker_heap_init();
	sdt_init_bp();
	private_data_init();
	SD();
	mmio_init();
	ker_stack_init();
	syscall_init();
	fs_init();
	mp_init();
	timer_init();
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