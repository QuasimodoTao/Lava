//Lava OS
//PageFault
//20-04-06-13-38

#include <vm.h>
#include <stddef.h>
#include <mm.h>
#include <string.h>
#include <stdio.h>
#include <vbe.h>

struct _MY_MODE_ {
	u16 code,xres;
	u16 yres,byte_per_line;
	u8 red_pos,green_pos,blue_pos,rvd_pos;
	u32 lfb;
};

static struct _MY_MODE_ * mode;
static int mode_count;
static u32 vga_memory_size;

void vbe_init(){
	u16 * mode_list;
	//u16 mode_list[] = {0x100,0x101,0x102,0x103,0x104,0x105,0xffff};
	u32 vm_addr1,vm_addr2;
	struct VESA_INFO * vesa_info;
	struct CPU cpu;
	int _mode_count;
	struct VESA_MODE * _mode_info;
	int i;

	cpu.gr[EAX] = 0x00004f00;
	vm_addr1 = vm_alloc_memory(512);
	vesa_info = vm_addr2v(vm_addr1);
	memset(vesa_info,0,512);
	cpu.sr[ES] = vm_addr1 >> 4;
	cpu.gr[EDI] = vm_addr1 & 0x0f;
	int86(0x10,&cpu);
	mode_list = vm_saddr2v(vesa_info->mode_seg,vesa_info->mode_base);
	//mode_list += 30;
	vga_memory_size = vesa_info->mem_size * 65536;
	//printk("vga size:%dMB\n",vga_memory_size >> 20);
	_mode_count = 0;
	while(mode_list[_mode_count] != 0xffff) _mode_count++;
	vm_addr2 = vm_alloc_memory(sizeof(struct VESA_MODE) * _mode_count);
	_mode_info = vm_addr2v(vm_addr2);
	_mode_count = 0;
	cpu.gr[EDI] = vm_addr2 & 0x0f;
	mode_count = 0;
	while(mode_list[_mode_count] != 0xffff){
		cpu.gr[EAX] = 0x00004f01;
		cpu.sr[ES] = (vm_addr2 >> 4) + _mode_count * (sizeof(struct VESA_MODE) >> 4);
		cpu.gr[ECX] = mode_list[_mode_count];
		int86(0x10,&cpu);
		if((_mode_info[_mode_count].bits_per_pixel == 32 ||
			_mode_info[_mode_count].bits_per_pixel == 24) &&
			_mode_info[_mode_count].mode_attr & 0x10) mode_count++;
		_mode_count++;
	}
	//printk("mode count %d.\n",mode_count);
	mode = kmalloc(sizeof(struct _MY_MODE_) * mode_count,0);
	mode_count = 0;
	for(i = 0;i < _mode_count;i++){
		if((_mode_info[i].bits_per_pixel != 32 && 
			_mode_info[i].bits_per_pixel != 24) ||
			!(_mode_info[i].mode_attr & 0x10)) continue;
		mode[mode_count].code = mode_list[i];
		mode[mode_count].xres = _mode_info[i].xres;
		mode[mode_count].yres = _mode_info[i].yres;
		mode[mode_count].byte_per_line = _mode_info[i].byte_per_scan_line;
		mode[mode_count].lfb = _mode_info[i].lfb;
		mode[mode_count].red_pos = _mode_info[i].red_pos;
		mode[mode_count].green_pos = _mode_info[i].green_pos;
		mode[mode_count].blue_pos = _mode_info[i].blue_pos;
		mode[mode_count].rvd_pos = _mode_info[i].rvd_pos;
		//printk("mode:%04X,%dx%d,%08X.\n",mode[mode_count].code,mode[mode_count].xres,
		//	mode[mode_count].yres,mode[mode_count].lfb);
		mode_count++;
	}
	vm_free_memory(vm_addr2);
	vm_free_memory(vm_addr1);
	
}

