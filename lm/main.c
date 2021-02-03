/*
	lm/main.c
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

#include "lm.h"
#include <pe.h>
#include <lava.h>
#include <stdio.h>
#include <mp.h>

struct _RELY_MSG_ {
	GUID DiskGUID;
	GUID PartGUID;
	u16 SMAPCount;
	u16 SMAPSegment;
	u16 ModeSeg;
	u16 FirstFreeSeg;
	u32 PCIBIOSEnt;
	u16 PCIax;
	u16 PCIbx;
	u16 PCIcx;
	u16 VESAInfoSeg;
};
uint64_t int_init(uint64_t MemoryStart);
void pic_init();
int64_t SATAInit(int64_t MemoryStart,GUID * ActiveDisk,int * ata_count);
int64_t LFSInit(int64_t MemoryStart,GUID * ActivePart);
int mem_init(u64 MemoryStart,u64 MemoryEnd);
void gui_init(u64 mode_info);


void __attribute__((noreturn)) EntryPoint(const struct _RELY_MSG_ * Msg){
	int64_t MemoryStart;
	struct _MSG_ * _Msg;
	struct _MPFT_ * MPH,* start,* end;
	struct _RSDP_ * RSDP;
	int i;
	uint8_t Sum;
	struct _NODE_ * File;
	struct _MZ_HEAD_ Head;
	struct _PEP_HEAD_ pep;
	struct _SECTION_ * Section;
	u32 Size;
	u64 KernelEntryPoint;
	u64 MemorySize;
	struct _SMAP_ * Map;
	u64 KerEnt;
	struct _MPCTH_ * MPCTH;
	struct _MPS_BUS_ * bus;
	struct _MPS_IOAPIC_ * _ioapic;
	u64 base;
	u8 * cur;
	int MapPageCount;
	u64 vaddr;
	u64 paddr;
	u32 * _PCMP_;
	
	gui_init(((u64)Msg->ModeSeg) << 4);
	MemoryStart = ((u64)Msg->FirstFreeSeg) << 4;
	_Msg = (struct _MSG_ *)0x09fb00;
	memcpy(&_Msg->DiskGUID,&Msg->DiskGUID,2*sizeof(GUID));
	MemoryStart = int_init(MemoryStart);
	_Msg->PCIax = Msg->PCIax;
	_Msg->PCIbx = Msg->PCIbx;
	_Msg->PCIcx = Msg->PCIcx;
	_Msg->SMAPCnt = Msg->SMAPCount;
	_Msg->SMAP = ((u64)Msg->SMAPSegment) << 4;
	_Msg->PCIBIOSEnt = Msg->PCIBIOSEnt;
	_Msg->CurModeInfo = ((u64)Msg->ModeSeg) << 4;
	_Msg->VESAInfo = ((u64)Msg->VESAInfoSeg) << 4;
	{//Search Mult-processors floating table
		_Msg->PCMP = _Msg->Feature = 0;
		start = (struct _MPFT_ *)0x7fc00;
		end = (struct _MPFT_ *)0x80000;
		while(1){
			for(;start < end;start++){
				if(start->Sign != 0x5f504d5f) continue;
				if(start->Length != 1) continue;
				Sum = 0;
				for(i = 0;i < 16;i++) Sum += ((uint8_t*)start)[i];
				if(Sum) continue;
				break;
			}
			if(start != end){
				MPCTH = (struct _MPCTH_ *)(u64)start->Addr;
				if(MPCTH->sign == 0x504d4350){
					cur = (u8*)start->Addr;
					Sum = 0;
					i = 0;
					for(;i < MPCTH->tab_len;i++)Sum += cur[i];
					if(!Sum) {
						MPH = start;
						break;
					}
				}
			}
			start++;
			if(start >= end){
				if(end == 0x80000){
					start = 0x9fc00;
					end = 0xa0000;
				}
				else if(end == 0xa0000){
					start = 0xf0000;
					end = 0x100000;
				}
				else {
					MPH = NULL;
					break;
				}
			}
		}
		if(!MPH){
			for(_PCMP_ = (u32*)0xe0000;_PCMP_ < (u32*)0x100000;_PCMP_ += 4){
				if(*_PCMP_ != 0x504d4350) continue;
				MPCTH = (struct _MPCTH_ *)_PCMP_;
				cur = (u8*)_PCMP_;
				Sum = 0;
				i = 0;
				for(;i < MPCTH->tab_len;i++)Sum += cur[i];
				if(!Sum) break;	
			}
			if(_PCMP_ != 0x100000) _Msg->PCMP = (u64)_PCMP_;
		}
		else{
			_Msg->PCMP = (int64_t)MPH->Addr;
			_Msg->Feature = MPH->Feature[0];
			_Msg->Feature |= ((u64)MPH->Feature[1]) << 8;
			_Msg->Feature |= ((u64)MPH->Feature[2]) << 16;
			_Msg->Feature |= ((u64)MPH->Feature[3]) << 24;
			_Msg->Feature |= ((u64)MPH->Feature[4]) << 32;
		}
		if(!_Msg->PCMP && !_Msg->Feature){
			printk("%P,%d.\n",_Msg->PCMP,_Msg->Feature & 0xff);
			print("Unsuppose muchine.\n");
			stop();
			
		}
		if(_Msg->Feature & 0xff){
			_Msg->LAPIC = 0xfee00000;
			_Msg->IOAPIC[0] = 0xfec00000;
			_Msg->IOAPICCount = 1;
			_Msg->CPUCount = 2;
			switch(MPH->Feature[0]){
			case 1: _Msg->BusFlags = BUSF_ISA; break;
			case 2: _Msg->BusFlags = BUSF_EISA; break;
			case 3: _Msg->BusFlags = BUSF_EISA; break;
			case 4: _Msg->BusFlags = BUSF_MCA; break;
			case 5: _Msg->BusFlags = BUSF_ISA | BUSF_PCI; break;
			case 6: _Msg->BusFlags = BUSF_EISA | BUSF_PCI; break;
			case 7: _Msg->BusFlags = BUSF_MCA | BUSF_PCI; break;
			}
		}
		else{
			_Msg->CPUCount = _Msg->IOAPICCount = _Msg->BusFlags = 0;
			MPCTH = (struct _MPCTH_ *)_Msg->PCMP;
			_Msg->LAPIC = MPCTH->lapic_mmio;
			base = (u64)MPCTH;
			cur = (u8*)(((u64)MPCTH) + sizeof(struct _MPCTH_));
			while(((u64)cur) - base < MPCTH->tab_len){
				switch(*cur){
				case 0:
					cur += sizeof(struct _MPS_CPU_);
					_Msg->CPUCount++;
					break;
				case 1:
					bus = (struct _MPS_BUS_ *)cur;
					cur += sizeof(struct _MPS_BUS_);
					if(!strncmp("CBUS\0\0",bus->str,6)) _Msg->BusFlags |= BUSF_CBUS;
					else if(!strncmp("CBUSII",bus->str,6)) _Msg->BusFlags |= BUSF_CBUSII;
					else if(!strncmp("EISA\0\0",bus->str,6)) _Msg->BusFlags |= BUSF_EISA;
					else if(!strncmp("FUTURE",bus->str,6)) _Msg->BusFlags |= BUSF_FUTURE;
					else if(!strncmp("INTERN",bus->str,6)) _Msg->BusFlags |= BUSF_INTERN;
					else if(!strncmp("ISA\0\0\0",bus->str,6)) _Msg->BusFlags |= BUSF_ISA;
					else if(!strncmp("MBI\0\0\0",bus->str,6)) _Msg->BusFlags |= BUSF_MBI;
					else if(!strncmp("MBII\0\0",bus->str,6)) _Msg->BusFlags |= BUSF_MBII;
					else if(!strncmp("MCA\0\0\0",bus->str,6)) _Msg->BusFlags |= BUSF_MCA;
					else if(!strncmp("MPI\0\0\0",bus->str,6)) _Msg->BusFlags |= BUSF_MPI;
					else if(!strncmp("MPSA\0\0",bus->str,6)) _Msg->BusFlags |= BUSF_MPSA;
					else if(!strncmp("NUBUS\0",bus->str,6)) _Msg->BusFlags |= BUSF_NUBUS;
					else if(!strncmp("PCI\0\0\0",bus->str,6)) _Msg->BusFlags |= BUSF_PCI;
					else if(!strncmp("PCMCIA",bus->str,6)) _Msg->BusFlags |= BUSF_PCMCIA;
					else if(!strncmp("TC\0\0\0\0",bus->str,6)) _Msg->BusFlags |= BUSF_TC;
					else if(!strncmp("VL\0\0\0\0",bus->str,6)) _Msg->BusFlags |= BUSF_VL;
					else if(!strncmp("VME\0\0\0",bus->str,6)) _Msg->BusFlags |= BUSF_VME;
					else if(!strncmp("XPRESS",bus->str,6)) _Msg->BusFlags |= BUSF_XPRESS;
					break;
				case 2:
					_ioapic = (struct _MPS_IOAPIC_ *)cur;
					cur += sizeof(struct _MPS_IOAPIC_);
					if(_Msg->IOAPICCount >= MAX_IOAPIC) break;
					_Msg->IOAPIC[_Msg->IOAPICCount] = _ioapic->mmio;
					_Msg->IOAPICCount++;
					break;
				case 3: cur += sizeof(struct _MPS_IO_INT_); break;
				case 4: cur += sizeof(struct _MPS_LINT_ASGN_); break;
				case 128: cur += sizeof(struct _MPS_MAP_); break;
				case 129: cur += sizeof(struct _MPS_BHD_); break;
				case 130: cur += sizeof(struct _MPS_CBASM_ ); break;
				}
			}
		}
	}
	{//Search ACPI Root System Descriptor Point
		_Msg->RSDP = 0;
		for(RSDP = (struct _RSDP_ *)0x7fc00;RSDP < (struct _RSDP_ *)0x80000;RSDP = (struct _RSDP_ *)(((int64_t)RSDP) + 16)){
			if(RSDP->Sign != 0x2052545020445352) continue;
			Sum = 0;
			if(RSDP->Revision >= 2) for(i = 0;i < RSDP->Length;i++) Sum += ((uint8_t*)RSDP)[i];
			else for(i = 0;i < 20;i++) Sum += ((uint8_t*)RSDP)[i];
			if(Sum) continue;
			_Msg->RSDP = (int64_t)RSDP;
		}
		if(!_Msg->RSDP){
			for(RSDP = (struct _RSDP_ *)0x9fc00;RSDP < (struct _RSDP_ *)0xa0000;RSDP = (struct _RSDP_ *)(((int64_t)RSDP) + 16)){
				if(RSDP->Sign != 0x2052545020445352) continue;
				Sum = 0;
				if(RSDP->Revision >= 2) for(i = 0;i < RSDP->Length;i++) Sum += ((uint8_t*)RSDP)[i];
				else for(i = 0;i < 20;i++) Sum += ((uint8_t*)RSDP)[i];
				if(Sum) continue;
				_Msg->RSDP = (int64_t)RSDP;
			}	
			if(!_Msg->RSDP){
				for(RSDP = (struct _RSDP_ *)0xe0000;RSDP < (struct _RSDP_ *)0x100000;RSDP = (struct _RSDP_ *)(((int64_t)RSDP) + 16)){
					if(RSDP->Sign != 0x2052545020445352) continue;
					Sum = 0;
					if(RSDP->Revision >= 2) for(i = 0;i < RSDP->Length;i++) Sum += ((uint8_t*)RSDP)[i];
					else for(i = 0;i < 20;i++) Sum += ((uint8_t*)RSDP)[i];
					if(Sum) continue;
					_Msg->RSDP = (int64_t)RSDP;
				}	
			}
		}
	}
	if(!(MemoryStart = lm_CfgPCI(MemoryStart,Msg->PCIBIOSEnt,Msg->PCIax,Msg->PCIbx,Msg->PCIcx))) {
		print("Unsuppose muchine.\n");
		stop();		
	}
	if(!(MemoryStart = SATAInit(MemoryStart, &(Msg->DiskGUID),&(_Msg->ATACount)))){
		print("Unsuppose environment.\n");
		stop();		
	}
	if(!(MemoryStart = LFSInit(MemoryStart,&(Msg->PartGUID)))){
		print("Unsuppose environment.\n");
		stop();		
	}
	mem_init(MemoryStart,0x90000);	
	{//Load Kernel
		File = Open(L"lava.sys");
		if(!File){
			print("Can not found kernel image.\n");
			stop();	
		}
		_Msg->KernelImagePBase = 0x100000;
		Read(File,&Head,sizeof(struct _MZ_HEAD_));
		if(Head.Magic != 0x5a4d){
			print("Unsuppose kernel image.\n");
			stop();
		}
		Seek(Head.PEHead,SEEK_SET,File);
		Read(File,&pep,sizeof(struct _PEP_HEAD_));
		if(pep.MagicPE != 0x00004550){
			print("Unsuppose kernel image.\n");
			stop();
		}
		Section = malloc(sizeof(struct _SECTION_) * pep.NumberOfSections);
		Read(File,Section,sizeof(struct _SECTION_) * pep.NumberOfSections);
		
		Size = Tell(File);
		Size += pep.FileAlignment - 1;
		Size &= ~(pep.FileAlignment - 1);
		Seek(0,SEEK_SET,File);
		Read(File,(void*)0x100000,Size);
		Size = 0;
		for(i = 0;i < pep.NumberOfSections;i++){
			Seek(Section[i].PointerToRAWData,SEEK_SET,File);
			Read(File,(void*)(0x100000 + Section[i].VirtualAddress),Section[i].SizeOfRawData);
			if(Size < Section[i].SizeOfRawData + Section[i].VirtualAddress)
				Size = Section[i].SizeOfRawData + Section[i].VirtualAddress;
		}
		Size += 0xfff;
		Size &= 0xfffff000;
		_Msg->KernelImageSize = Size;
		Close(File);
		vaddr = pep.ImageBase;
		paddr = 0x100000;
		MemoryStart = 0x100000 + Size;
		_Msg->FontBase = MemoryStart;
	}
	{//Load Font
		File = Open(L"font.fnt");
		if(File){
			Seek(0,SEEK_END,File);
			Size = Tell(File);
			Seek(0,SEEK_SET,File);
			Read(File,(void*)(_Msg->FontBase),Size);
			Size += 0x1ff;
			Size &= 0xfffffe00;
			_Msg->FontSize = Size;
			Close(File);
		}
		else {
			_Msg->FontSize = 0;
			print("Can not found font file.\n");
		}
	}
	{//Load Configuation
		File = Open(L"Config.ini");
		_Msg->ConfigBase = _Msg->FontBase + Size;
		if(File){
			Seek(0,SEEK_END,File);
			Size = Tell(File);
			Seek(0,SEEK_SET,File);
			Read(File,(void*)(_Msg->ConfigBase),Size);
			Size += 0x1ff;
			Size &= 0xfffffe00;
			_Msg->ConfigSize = Size;
			Close(File);
		}
		else {
			_Msg->ConfigSize = 0;
			print("Can not found config file.\n");
		}
	}
	_Msg->MemoryStart = _Msg->ConfigBase + _Msg->ConfigSize;
	MemorySize = 0;
	Map = _Msg->SMAP;
	for(i = 0;i < Msg->SMAPCount;i++){
		if(Map[i].Attr == 1 && MemorySize < Map[i].Start + Map[i].Size) 
			MemorySize = Map[i].Start + Map[i].Size;
	}
	_Msg->MemorySize = MemorySize;
	_Msg->MemoryStart += 0xfff;
	_Msg->MemoryStart &= 0xfffffffffffff000;
	KerEnt = pep.ImageBase + pep.AddressOfEntryPoint;
	printk("%P.\n",KerEnt);
	asm("movq %%cr3,%%rbx\n\tmovq %%rbx,%%cr3\n\tmovq $0xffff80000000fff8,%%rsp\n\tcall %%rax"::"a"(KerEnt),"c"(_Msg));;
	Stop();
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