/*
	dev/pci.c
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

#include <lava.h>
#include <stddef.h>
#include <asm.h>
#include <pci.h>
#include <mm.h>
#include <int.h>
#include <stdio.h>
#include <string.h>
#include <spinlock.h>
#include <pe.h>

static u8 (*__pci_read_byte)(u8,u8,u8,u8);
static u16 (*__pci_read_word)(u8,u8,u8,u8);
static u32 (*__pci_read_dword)(u8,u8,u8,u8);
static void (*__pci_write_byte)(u8,u8,u8,u8,u8);
static void (*__pci_write_word)(u8,u8,u8,u8,u16);
static void (*__pci_write_dword)(u8,u8,u8,u8,u32);
static u32 dev_count;
static u8 int_line[4];
static LPPCIDEV dev_list;
static LPPCIDEV int_list[4];
static int port_busy;
static struct _PCI_DEV_MODULE_ * pci_dev_ctrl;
static int pci_dev_ctrl_count;
spin_optr_def_bit(Port,&port_busy,0);
#define CM1_ADDR(bus,dev,fun,reg) \
	(0x80000000 | (((u32)(reg)) & 0xfc) | (((u32)(fun)) << 8) | (((u32)(dev)) << 11) | (((u32)(bus)) << 16))

static u8 pci_read_byte_m1(u8 bus,u8 dev,u8 fun,u8 reg){
	u32 addr = CM1_ADDR(bus,dev,fun,reg);
	int _o;
	u32 data;
	u64 rf;
	
	_o = reg & 0x03;
	SFI(rf); LockPort();
	outd(PCI_C1_ADDR,addr);
	data = ind(PCI_C1_DATA);
	UnlockPort(); LF(rf);
	return (data >> (_o << 3)) & 0x0ff;
}
static u16 pci_read_word_m1(u8 bus,u8 dev,u8 fun,u8 reg){
	u32 addr = CM1_ADDR(bus,dev,fun,reg);
	int _o;
	u32 data;
	u64 rf;

	_o = reg & 0x03;
	SFI(rf); LockPort();
	outd(PCI_C1_ADDR,addr);
	data = ind(PCI_C1_DATA);
	UnlockPort(); LF(rf);
	return (data >> (_o << 3)) & 0x0ffff;
}
static u32 pci_read_dword_m1(u8 bus,u8 dev,u8 fun,u8 reg){
	u32 addr = CM1_ADDR(bus,dev,fun,reg);
	int _o;
	u32 data;
	u64 rf;
	
	_o = reg & 0x03;
	SFI(rf); LockPort();
	outd(PCI_C1_ADDR,addr);
	data = ind(PCI_C1_DATA);
	UnlockPort(); LF(rf);
	return data;
}
static void pci_write_byte_m1(u8 bus,u8 dev,u8 fun,u8 reg,u8 v){
	u32 addr = CM1_ADDR(bus,dev,fun,reg);
	int _o;
	u32 data;
	u64 rf;

	_o = reg & 0x03;
	SFI(rf); LockPort();
	outd(PCI_C1_ADDR,addr);
	data = ind(PCI_C1_DATA);
	data &= ~(0x0ff << (_o << 3));
	data |= v << (_o << 3);
	outd(PCI_C1_ADDR,addr);
	outd(PCI_C1_DATA,data);
	UnlockPort(); LF(rf);
}
static void pci_write_word_m1(u8 bus,u8 dev,u8 fun,u8 reg,u16 v){
	u32 addr = CM1_ADDR(bus,dev,fun,reg);
	int _o;
	u32 data;
	u64 rf;
	
	_o = reg & 0x03;
	SFI(rf); LockPort();
	outd(PCI_C1_ADDR,addr);
	data = ind(PCI_C1_DATA);
	data &= ~(0x0ffff << (_o << 3));
	data |= v << (_o << 3);
	outd(PCI_C1_ADDR,addr);
	outd(PCI_C1_DATA,data);
	UnlockPort(); LF(rf);
}
static void pci_write_dword_m1(u8 bus,u8 dev,u8 fun,u8 reg,u32 v){
	u32 addr = CM1_ADDR(bus,dev,fun,reg);
	int _o;
	u64 rf;

	_o = reg & 0x03;
	SFI(rf); LockPort();
	outd(PCI_C1_ADDR,addr);
	outd(PCI_C1_DATA,v);
	UnlockPort(); LF(rf);
}
static u8 pci_read_byte_m2(u8 bus,u8 dev,u8 fun,u8 reg){
	u16 Port = PCI_C2_REGB;
	u8 addr = 0xf0;
	u32 data;
	int _o;
	u64 rf;
	
	_o = reg & 0x03;
	addr |= fun << 1;
	Port |= ((u16)dev) << 8;
	Port |= ((u16)reg) & 0xfc;
	SFI(rf); LockPort();
	outb(PCI_C2_FUN,addr);
	outb(PCI_C2_BUS,bus);
	data = ind(Port);
	UnlockPort(); LF(rf);
	return (data >> (_o << 3)) & 0x0ff;
}
static u16 pci_read_word_m2(u8 bus,u8 dev,u8 fun,u8 reg){
	u16 Port = PCI_C2_REGB;
	u8 addr = 0xf0;
	u32 data;
	int _o;
	u64 rf;
	
	_o = reg & 0x03;
	addr |= fun << 1;
	Port |= ((u16)dev) << 8;
	Port |= ((u16)reg) & 0xfc;
	SFI(rf); LockPort();
	outb(PCI_C2_FUN,addr);
	outb(PCI_C2_BUS,bus);
	data = ind(Port);
	UnlockPort(); LF(rf);
	return (data >> (_o << 3)) & 0x0ffff;
}
static u32 pci_read_dword_m2(u8 bus,u8 dev,u8 fun,u8 reg){
	u16 Port = PCI_C2_REGB;
	u8 addr = 0xf0;
	u32 data;
	int _o;
	u64 rf;
	
	_o = reg & 0x03;
	addr |= fun << 1;
	Port |= ((u16)dev) << 8;
	Port |= ((u16)reg) & 0xfc;
	SFI(rf); LockPort();
	outb(PCI_C2_FUN,addr);
	outb(PCI_C2_BUS,bus);
	data = ind(Port);
	UnlockPort(); LF(rf);
	return data;
}
static void pci_write_byte_m2(u8 bus,u8 dev,u8 fun,u8 reg,u8 v){
	u16 Port = PCI_C2_REGB;
	u8 addr = 0xf0;
	u32 data;
	int _o;
	u64 rf;

	_o = reg & 0x03;
	addr |= fun << 1;
	Port |= ((u16)dev) << 8;
	Port |= ((u16)reg) & 0xfc;
	SFI(rf); LockPort();
	outb(PCI_C2_FUN,addr);
	outb(PCI_C2_BUS,bus);
	data = ind(Port);
	data &= ~(0x0ff << (_o << 3));
	data |= v << (_o << 3);
	outb(PCI_C2_FUN,addr);
	outb(PCI_C2_BUS,bus);
	outd(Port,data);
	UnlockPort(); LF(rf);
}
static void pci_write_word_m2(u8 bus,u8 dev,u8 fun,u8 reg,u16 v){
	u16 Port = PCI_C2_REGB;
	u8 addr = 0xf0;
	u32 data;
	int _o;
	u64 rf;
	
	_o = reg & 0x03;
	addr |= fun << 1;
	Port |= ((u16)dev) << 8;
	Port |= ((u16)reg) & 0xfc;
	SFI(rf); LockPort();
	outb(PCI_C2_FUN,addr);
	outb(PCI_C2_BUS,bus);
	data = ind(Port);
	data &= ~(0x0ffff << (_o << 3));
	data |= v << (_o << 3);
	outb(PCI_C2_FUN,addr);
	outb(PCI_C2_BUS,bus);
	outd(Port,data);
	UnlockPort(); LF(rf);
}
static void pci_write_dword_m2(u8 bus,u8 dev,u8 fun,u8 reg,u32 v){
	u16 Port = PCI_C2_REGB;
	u8 addr = 0xf0;
	int _o;
	u64 rf;

	_o = reg & 0x03;
	addr |= fun << 1;
	Port |= ((u16)dev) << 8;
	Port |= ((u16)reg) & 0xfc;
	SFI(rf); LockPort();
	outb(PCI_C2_FUN,addr);
	outb(PCI_C2_BUS,bus);
	outd(Port,v);
	UnlockPort(); LF(rf);
}
u8 pci_read_byte(LPPCIDEV dev,u8 reg){
	return __pci_read_byte(dev->bus,dev->dev,dev->fun,reg);
}
u16 pci_read_word(LPPCIDEV dev,u8 reg){
	return __pci_read_word(dev->bus,dev->dev,dev->fun,reg);
}
u32 pci_read_dword(LPPCIDEV dev,u8 reg){
	return __pci_read_dword(dev->bus,dev->dev,dev->fun,reg);
}
void pci_write_byte(LPPCIDEV dev,u8 reg,u8 data){
	return __pci_write_byte(dev->bus,dev->dev,dev->fun,reg,data);
}
void pci_write_word(LPPCIDEV dev,u8 reg,u16 data){
	return __pci_write_word(dev->bus,dev->dev,dev->fun,reg,data);
}
void pci_write_dword(LPPCIDEV dev,u8 reg,u32 data){
	return __pci_write_dword(dev->bus,dev->dev,dev->fun,reg,data);
}
LPPCIDEV pci_find_class(LPPCIDEV dev,u32 class,u32 sub_class,u32 program_if){
	
	if(!dev) dev = dev_list;
	else dev = dev->next;
	if(class >= 0x100) return NULL;
	for(;dev;dev = dev->next){
		if(dev->class != class) continue;
		if(sub_class >= 0x100) return dev;
		if(dev->sub_class != sub_class) continue;
		if(program_if >= 0x100) return dev;
		if(dev->program_if == program_if) return dev;
	}
	return NULL;
}
static int pci_irq_handle(int irq){
	int i;
	LPPCIDEV dev;
	
	for(i = 0;i < 4;i++){
		if(int_line[i] == irq)
			for(dev = int_list[i];dev;dev = dev->int_next)
				if(dev->int_handle) dev->int_handle(dev,dev->ctrl,pci_read_word(dev,PCIStatus));
		
	}
	return 0;
}
static int pci_dev_init_thread(void* _cur_dev){
	int i;
	LPPCIDEV cur_dev = _cur_dev;
	
	if(cur_dev->next) create_thread(NULL,pci_dev_init_thread,cur_dev->next);
	for(i = 0;i < pci_dev_ctrl_count;i++){
		if(cur_dev->class != pci_dev_ctrl[i].class_code) continue;
		if(pci_dev_ctrl[i].subclass_code == 0xffff || cur_dev->sub_class == pci_dev_ctrl[i].subclass_code){
			//printk("PCI:%d,%d,%d:%s,Vendor %04X,Device:%04X.\n",
			//	cur_dev->bus,cur_dev->dev,cur_dev->fun,pci_dev_ctrl[i].name,cur_dev->vendor,cur_dev->device);
			if(pci_dev_ctrl[i].construct) cur_dev->ctrl = pci_dev_ctrl[i].construct(cur_dev);
			break;
		}
	}
	return 0;
}
void pci_init(){
	u32 bus,dev,fun,dev_per_bus;
	u8 _int_line;
	u32 i;
	LPPCIDEV _dev;
	struct _PE_SECTION_ * pci_section;
	
	if(init_msg.PCIax & 0x01){
		__pci_read_byte = pci_read_byte_m1;
		__pci_read_word = pci_read_word_m1;
		__pci_read_dword = pci_read_dword_m1;
		__pci_write_byte = pci_write_byte_m1;
		__pci_write_word = pci_write_word_m1;
		__pci_write_dword = pci_write_dword_m1;
		dev_per_bus = 32;
	}
	else{
		__pci_read_byte = pci_read_byte_m2;
		__pci_read_word = pci_read_word_m2;
		__pci_read_dword = pci_read_dword_m2;
		__pci_write_byte = pci_write_byte_m2;
		__pci_write_word = pci_write_word_m2;
		__pci_write_dword = pci_write_dword_m2;
		dev_per_bus = 16;
	}
	init_msg.PCIcx &= 0xff;
	dev_list = NULL;
	int_list[0] = int_list[1] = int_list[2] = int_list[3] = NULL;
	int_line[0] = int_line[1] = int_line[2] = int_line[3] = 0xff;
	dev_count = 0;
	port_busy = 0;
	pci_section = get_section_addr(PCI_DEV_MODULE_SECTION_NAME);
	if(!pci_section){
		printk("Can not find section.\n");
		stop();
	}
	pci_dev_ctrl = ADDRP2V(pci_section->VirtualAddress + init_msg.KernelImagePBase);
	pci_dev_ctrl_count = pci_section->VirtualSize / sizeof(struct _PCI_DEV_MODULE_);
	for(bus = 0;bus <= init_msg.PCIcx;bus++){
		for(dev = 0;dev < dev_per_bus;dev++){
			for(fun = 0;fun < 8;fun++){
				if(__pci_read_word(bus,dev,fun,PCIVndID) == 0xffff) {
					if(!fun) break;
					continue;
				}
				if(dev_list){
					_dev->next = kmalloc(sizeof(PCIDEV),0);
					_dev = _dev->next;
				}
				else dev_list = _dev = kmalloc(sizeof(PCIDEV),0);
				memset(_dev,0,sizeof(PCIDEV));
				_dev->bus = bus;
				_dev->fun = fun;
				_dev->dev = dev;
				_dev->head = __pci_read_byte(bus,dev,fun,PCIHT);
				_dev->class = __pci_read_byte(bus,dev,fun,PCIClass);
				_dev->sub_class = __pci_read_byte(bus,dev,fun,PCISubClass);
				_dev->program_if = __pci_read_byte(bus,dev,fun,PCIProgIF);
				_dev->rev = __pci_read_byte(bus,dev,fun,PCIRevID);
				_dev->vendor = __pci_read_word(bus,dev,fun,PCIVndID);
				_dev->device = __pci_read_word(bus,dev,fun,PCIDevID);
				_int_line = __pci_read_byte(bus,dev,fun,PCIIntLine);
				dev_count++;
				if(_int_line) 
					for(i = 0;i < 4;i++){
						if(int_line[i] == 0xff){
							int_line[i] = _int_line;
							_dev->int_next = NULL;
							int_list[i] = _dev;
							request_irq(int_line[i],pci_irq_handle);
							irq_enable(int_line[i]);
							break;
						}
						if(_int_line == int_line[i]){
							_dev->int_next = int_list[i];
							int_list[i] = _dev;
							break;
						}
					}
				if(!fun && !(_dev->head & 0x80)) break;
				else _dev->head &= 0x7f;
			}
		}
	}
	create_thread(NULL,pci_dev_init_thread,dev_list);
}
