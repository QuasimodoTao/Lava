/*
	inc/lava.h
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

#ifndef _LAVA_H_
#define _LAVA_H_
#include <stddef.h>
#include <config.h>
#include <vbe.h>

#define BUSF_CBUS	0x00000001
#define BUSF_CBUSII	0x00000002
#define BUSF_EISA	0x00000004
#define BUSF_FUTURE	0x00000008
#define BUSF_INTERN	0x00000010
#define BUSF_ISA	0x00000020
#define BUSF_MBI	0x00000040
#define BUSF_MBII	0x00000080
#define BUSF_MCA	0x00000100
#define BUSF_MPI	0x00000200
#define BUSF_MPSA	0x00000400
#define BUSF_NUBUS	0x00000800
#define BUSF_PCI	0x00001000
#define BUSF_PCMCIA	0x00002000
#define BUSF_TC		0x00004000
#define BUSF_VL		0x00008000
#define BUSF_VME	0x00010000
#define BUSF_XPRESS 0x00020000


struct _MSG_ {
	GUID DiskGUID;//
	GUID PartGUID;//
	u16 PCIax;//
	u16 PCIbx;//
	u16 PCIcx;//
	u8 SMAPCnt;//
	u64 PCMP;//
	u64 Feature;//
	u64 RSDP;//
	u64 SMAP;//
	u64 PCIBIOSEnt;//
	u64 KernelImagePBase;//
	u64 KernelImageSize;//
	u64 FontBase;//
	u64 FontSize;//
	u64 ConfigBase;//
	u64 ConfigSize;//
	u64 MemoryStart;
	u64 MemorySize;
	u64 CurModeInfo;
	u64 VESAInfo;
	u64 LAPIC;
	u64 IOAPIC[MAX_IOAPIC];
	int CPUCount;
	int IOAPICCount;
	int BusFlags;
	int ATACount;
	u32 pPageStart;
	u32 pPageCount;
};
struct _RSDP_ {
	int64_t Sign;
	char Check;
	char OEMID[6];
	char Revision;
	uint32_t RsdtAddr;
	uint32_t Length;//>=2.0
	uint64_t XsdtAddr;//>=2.0
	char ExtCheck;//>=2.0
	char Rvd[3];
};

struct _SMAP_ {
	u64 Start;
	u64 Size;
	u32 Attr;
	u32 EAttr;
};



void ShowHexs(void * _p,int Line);

struct _MSG_ init_msg;


#define DEBUG
#define CHECK
	
#define ADDRP2V(paddr)	((void*)((paddr) | PMEMSTART))

//heap manager



//page ent address to physical page address
#define PPADDR(s)		((s)&0x00003ffffffff000)
//page ent address to virtual page address
#define PADDR2V(s)		((void*)ADDRP2V(PPADDR(s)))

#define NATURALALIGNU(v)	(((v) + 0x0f) & 0xfffffffffffffff0)
#define NATURALALIGND(v)	((v) & 0xfffffffffffffff0)

#define _In
#define _Out
#define INIT	__attribute__((section(".init")))





#endif