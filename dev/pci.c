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

struct _PCIDEVCTRL_ {
	u16 class;
	u16 sub_class;
	HANDLE (*open)(LPPCIDEV);
	const char * name;
};

HANDLE ahci_open(LPPCIDEV);

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
spin_optr_def_bit(Port,&port_busy,0);
static struct _PCIDEVCTRL_ pci_dev_ctrl[] = {
	{0x0000,0x0000,NULL,"Non-VGA-Compatible devices"},
	{0x0000,0x0001,NULL,"VGA-Compatible Device"},
	{0x0001,0x0000,NULL,"SCSI Bus Controller"},
	{0x0001,0x0001,NULL,"ISA Compatibility mode-only controller"},
	{0x0001,0x0002,NULL,"Floppy Disk Controller"},
	{0x0001,0x0003,NULL,"IPI Bus Controller"},
	{0x0001,0x0004,NULL,"RAID Controller"},
	{0x0001,0x0005,NULL,"ATA Controller"},
	{0x0001,0x0006,ahci_open,"Serial ATA"},
	{0x0001,0x0007,NULL,"Serial Attached SCSI"},
	{0x0001,0x0008,NULL,"Non-Volatile Memory Controller"},
	{0x0001,0x0080,NULL,"Unknow Mass Storage Controller"},
	{0x0002,0x0000,NULL,"Ethernet Controller"},
	{0x0002,0x0001,NULL,"Token Ring Controller"},
	{0x0002,0x0002,NULL,"FDDI Controller"},
	{0x0002,0x0003,NULL,"ATM Controller"},
	{0x0002,0x0004,NULL,"ISDN Controller"},
	{0x0002,0x0005,NULL,"WorldFip Controller"},
	{0x0002,0x0006,NULL,"PICMG 2.14 Multi Computing"},
	{0x0002,0x0007,NULL,"Infiniband Controller"},
	{0x0002,0x0008,NULL,"Fabric Controller"},
	{0x0002,0x0080,NULL,"Unknow Network Controller"},
	{0x0003,0x0000,NULL,"VGA Compatible Controller"},
	{0x0003,0x0001,NULL,"XGA Controller"},
	{0x0003,0x0002,NULL,"3D Controller (Not VGA-Compatible)"},
	{0x0003,0x0080,NULL,"Unknow Display Controller"},
	{0x0004,0x0000,NULL,"Multimedia Video Controller"},
	{0x0004,0x0001,NULL,"Multimedia Audio Controller"},
	{0x0004,0x0002,NULL,"Computer Telephony Device"},
	{0x0004,0x0003,NULL,"Audio Device"},
	{0x0004,0x0080,NULL,"Unknow Multimedia Controller"},
	{0x0005,0x0000,NULL,"RAM Controller"},
	{0x0005,0x0001,NULL,"Flash Controller"},
	{0x0005,0x0080,NULL,"Unknow Memory Controller"},
	{0x0006,0x0000,NULL,"Host Bridge"},
	{0x0006,0x0001,NULL,"ISA Bridge"},
	{0x0006,0x0002,NULL,"EISA Bridge"},
	{0x0006,0x0003,NULL,"MCA Bridge"},
	{0x0006,0x0004,NULL,"PCI-to-PCI Bridge"},
	{0x0006,0x0005,NULL,"PCMCIA Bridge"},
	{0x0006,0x0006,NULL,"NuBus Bridge"},
	{0x0006,0x0007,NULL,"CardBus Bridge"},
	{0x0006,0x0008,NULL,"RACEway Bridge"},
	{0x0006,0x0009,NULL,"PCI-to-PCI Bridge"},
	{0x0006,0x000a,NULL,"InfiniBand-to-PCI Host Bridge"},
	{0x0006,0x0080,NULL,"Unknow Bridge Device"},
	{0x0007,0x0000,NULL,"Serial Controller"},
	{0x0007,0x0001,NULL,"Parallel Controller"},
	{0x0007,0x0002,NULL,"Multiport Serial Controller"},
	{0x0007,0x0003,NULL,"Modem"},
	{0x0007,0x0004,NULL,"IEEE 488.1/2 (GPIB) Controller"},
	{0x0007,0x0005,NULL,"Smart Card"},
	{0x0007,0x0080,NULL,"Unknow Simple Communication Controller"},
	{0x0008,0x0000,NULL,"PIC"},
	{0x0008,0x0001,NULL,"DMA Controller"},
	{0x0008,0x0002,NULL,"Timer"},
	{0x0008,0x0003,NULL,"RTC Controller"},
	{0x0008,0x0004,NULL,"PCI Hot-Plug Controller"},
	{0x0008,0x0005,NULL,"SD Host controller"},
	{0x0008,0x0006,NULL,"IOMMU"},
	{0x0008,0x0080,NULL,"Unknow Base System Peripheral"},
	{0x0009,0x0000,NULL,"Keyboard Controller"},
	{0x0009,0x0001,NULL,"Digitizer Pen"},
	{0x0009,0x0002,NULL,"Mouse Controller"},
	{0x0009,0x0003,NULL,"Scanner Controller"},
	{0x0009,0x0004,NULL,"Gameport Controller"},
	{0x0009,0x0080,NULL,"Unknow Input Device Controller"},
	{0x000a,0x0000,NULL,"Generic Docking Station"},
	{0x000a,0x0080,NULL,"Unknoe Docking Station"},
	{0x000b,0x0000,NULL,"i386 Procrssor"},
	{0x000b,0x0001,NULL,"i486 Processor"},
	{0x000b,0x0002,NULL,"Pentium"},
	{0x000b,0x0003,NULL,"Pentium Pro"},
	{0x000b,0x0010,NULL,"Alpha"},
	{0x000b,0x0020,NULL,"PowerPC"},
	{0x000b,0x0030,NULL,"MIPS"},
	{0x000b,0x0040,NULL,"Co-Processor"},
	{0x000b,0x0080,NULL,"Unknow Processor"},
	{0x000c,0x0000,NULL,"FireWire (IEEE 1394) Controller"},
	{0x000c,0x0001,NULL,"ACCESS Bus"},
	{0x000c,0x0002,NULL,"SSA"},
	{0x000c,0x0003,NULL,"USB Controller"},
	{0x000c,0x0004,NULL,"Fibre Channel"},
	{0x000c,0x0005,NULL,"SMBus"},
	{0x000c,0x0006,NULL,"InfiniBand"},
	{0x000c,0x0007,NULL,"IPMI Interface"},
	{0x000c,0x0008,NULL,"SERCOS Interface (IEC 61491)"},
	{0x000c,0x0080,NULL,"Unknow Serial Bus Controller"},
	{0x000d,0x0000,NULL,"iRDA Compatible Controller"},
	{0x000d,0x0001,NULL,"Consumer IR Controller"},
	{0x000d,0x0010,NULL,"RF Controller"},
	{0x000d,0x0011,NULL,"Bluetooth Controller"},
	{0x000d,0x0012,NULL,"Broadband Controller"},
	{0x000d,0x0020,NULL,"Ethernet Controller (802.1a)"},
	{0x000d,0x0021,NULL,"Ethernet Controller (802.1b)"},
	{0x000d,0x0080,NULL,"Unknow Wireless Controller"},
	{0x000e,0x0000,NULL,"I20"},
	{0x000f,0x0001,NULL,"Satellite TV Controller"},
	{0x000f,0x0002,NULL,"Satellite Audio Controller"},
	{0x000f,0x0003,NULL,"Satellite Voice Controller"},
	{0x000f,0x0004,NULL,"Satellite Data Controller"},
	{0x0010,0x0000,NULL,"Network and Computing Encrpytion/Decryption"},
	{0x0010,0x0010,NULL,"Entertainment Encryption/Decryption"},
	{0x0010,0x0080,NULL,"Other Encryption/Decryption"},
	{0x0011,0x0000,NULL,"DPIO Modules"},
	{0x0011,0x0001,NULL,"Performance Counters"},
	{0x0011,0x0010,NULL,"Communication Synchronizer"},
	{0x0011,0x0020,NULL,"Signal Processing Management"},
	{0x0011,0x0080,NULL,"Unknow Signal Processing Controller"},
	{0x0012,0xffff,NULL,"Processing Accelerator"},
	{0x0013,0xffff,NULL,"Non-Essential Instrumentation"},
	{0x0040,0xffff,NULL,"Co-Processor"},
	{0x00FF,0xffff,NULL,"Unassigned Class (Vendor specific)"}
};
static u8 pci_read_byte_m1(u8 bus,u8 dev,u8 fun,u8 reg){
	u32 addr = 0x80000000;
	int _o;
	u32 data;
	u64 rf;
	
	_o = reg & 0x03;
	addr |= ((u32)reg) & 0xfc;
	addr |= ((u32)fun) << 8;
	addr |= ((u32)dev) << 11;
	addr |= ((u32)bus) << 16;
	SFI(rf); LockPort();
	outd(PCI_C1_ADDR,addr);
	data = ind(PCI_C1_DATA);
	UnlockPort(); LF(rf);
	return (data >> (_o << 3)) & 0x0ff;
}
static u16 pci_read_word_m1(u8 bus,u8 dev,u8 fun,u8 reg){
	u32 addr = 0x80000000;
	int _o;
	u32 data;
	u64 rf;

	_o = reg & 0x03;
	addr |= ((u32)reg) & 0xfc;
	addr |= ((u32)fun) << 8;
	addr |= ((u32)dev) << 11;
	addr |= ((u32)bus) << 16;
	SFI(rf); LockPort();
	outd(PCI_C1_ADDR,addr);
	outd(PCI_C1_ADDR,addr);
	data = ind(PCI_C1_DATA);
	UnlockPort(); LF(rf);
	return (data >> (_o << 3)) & 0x0ffff;
}
static u32 pci_read_dword_m1(u8 bus,u8 dev,u8 fun,u8 reg){
	u32 addr = 0x80000000;
	int _o;
	u32 data;
	u64 rf;
	
	_o = reg & 0x03;
	addr |= ((u32)reg) & 0xfc;
	addr |= ((u32)fun) << 8;
	addr |= ((u32)dev) << 11;
	addr |= ((u32)bus) << 16;
	SFI(rf); LockPort();
	outd(PCI_C1_ADDR,addr);
	data = ind(PCI_C1_DATA);
	UnlockPort(); LF(rf);
	return data;
}
static void pci_write_byte_m1(u8 bus,u8 dev,u8 fun,u8 reg,u8 v){
	u32 addr = 0x80000000;
	int _o;
	u32 data;
	u64 rf;

	_o = reg & 0x03;
	addr |= ((u32)reg) & 0xfc;
	addr |= ((u32)fun) << 8;
	addr |= ((u32)dev) << 11;
	addr |= ((u32)bus) << 16;
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
	u32 addr = 0x80000000;
	int _o;
	u32 data;
	u64 rf;
	
	_o = reg & 0x03;
	addr |= ((u32)reg) & 0xfc;
	addr |= ((u32)fun) << 8;
	addr |= ((u32)dev) << 11;
	addr |= ((u32)bus) << 16;
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
	u32 addr = 0x80000000;
	int _o;
	u64 rf;

	_o = reg & 0x03;
	addr |= ((u32)reg) & 0xfc;
	addr |= ((u32)fun) << 8;
	addr |= ((u32)dev) << 11;
	addr |= ((u32)bus) << 16;
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
	u16 status;
	
	for(i = 0;i < 4;i++){
		if(int_line[i] == irq){
			for(dev = int_list[i];dev;dev = dev->int_next){
				status = pci_read_word(dev,PCIStatus);
				if(dev->int_handle) dev->int_handle(dev,dev->ctrl,status);
			}
		}
	}
	return -1;
}
static int pci_dev_init_thread(void* _cur_dev){
	int i;
	LPPCIDEV cur_dev = _cur_dev;
	
	if(cur_dev->next) create_thread(NULL,pci_dev_init_thread,cur_dev->next);
	for(i = 0;i < sizeof(pci_dev_ctrl)/sizeof(struct _PCIDEVCTRL_);i++){
		if(cur_dev->class != pci_dev_ctrl[i].class) continue;
		if(pci_dev_ctrl[i].sub_class == 0xffff || cur_dev->sub_class == pci_dev_ctrl[i].sub_class){
			//printk("PCI:%d,%d,%d:%s,Vendor %04X,Device:%04X.\n",
			//	cur_dev->bus,cur_dev->dev,cur_dev->fun,pci_dev_ctrl[i].name,cur_dev->vendor,cur_dev->device);
			if(pci_dev_ctrl[i].open) cur_dev->ctrl = pci_dev_ctrl[i].open(cur_dev);
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
