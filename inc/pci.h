/*
	inc/pci.h
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

#ifndef _PCI_H_
#define _PCI_H_

#include <stddef.h>

#define PCI_CONFIG_ADDRESS	0xcf8
#define PCI_CONFIG_DATA		0xcfc

#define PCIVndID			0x00
#define PCIDevID			0x02
#define PCICmd				0x04
#define PCIStatus			0x06
#define PCIRevID			0x08
#define PCIProgIF			0x09
#define PCISubClass			0x0a
#define PCIClass			0x0b
#define PCICLSize			0x0c
#define PCILtcTimer			0x0d
#define PCIHT				0x0e
#define PCIBIST				0x0f
#define PCIIntLine			0x3c
#define PCIIntPin			0x3d

#define PCI0BAR(x)			(0x10+(x)*4)
#define PCI0CISPtr			0x28
#define PCI0SubSysVndID		0x2c
#define PCI0SubSysID		0x2e
#define PCI0ExpROMBAR		0x30
#define PCI0CapPtr			0x34
#define PCI0MinGrant		0x3e
#define PCI0MaxLtc			0x3f

#define PCI1BAR(x)			(0x10+(x)*4)
#define PCI1PrmBusNum		0x18
#define PCI1SecBusNum		0x19
#define PCI1SubBusNum		0x1a
#define PCI1SecLtcTimer		0x1b
#define PCI1IOBR			0x1c
#define PCI1IOLR			0x1d
#define PCI1SecStatus		0x1e
#define PCI1MBAR			0x20
#define PCI1MLR				0x22
#define PCI1PreMBAR			0x24
#define PCI1PreMLR			0x26
#define PCI1PreMBARU32		0x28
#define PCI1PreMLRU32		0x2c
#define PCI1IOBARU16		0x30
#define PCI1IOLRU16			0x32
#define PCI1CapPtr			0x34
#define PCI1ExpROMBAR		0x38
#define PCI1BrgCtrl			0x3e

#define PCI2Socket_ExCapBAR	0x10
#define PCI2OffOfCapList	0x14
#define PCI2SecStatus		0x16
#define PCI2PCIBusNum		0x18
#define PCI2BusNum			0x19
#define PCI2SubBusNum		0x1a
#define PCI2LatencyTimer	0x1b
#define PCI2MBAR(x)			(0x1c+(x)*8)
#define PCI2MLR(x)			(0x20+(x)*8)
#define PCI2IOBAR(x)		(0x2c+(x)*8)
#define PCI2IOLR(x)			(0x30+(x)*8)
#define PCI2BrgCtrl			0x3e
#define PCI2SubsysDevID		0x40
#define PCI2SubsysVndID		0x42
#define PCI216BitBAR		0x44

#define PCI_CMD_IOSE	0x0001//IOSE	RW/RO	I	I/O Space Enable
#define PCI_CMD_MSE		0x0002//MSE	RW		0	Memory Space Enable
#define PCI_CMD_MBE		0x0004//BME	RW		0	Bus Master Enable
#define PCI_CMD_SCE		0x0008//SCE	RO		0	Special Cycle Enable
#define PCI_CMD_MWIE	0x0010//MWIE	RW/RO	I	Memory Write and Invaildate Enable
#define PCI_CMD_VGA		0x0020//VGA	RO		0	VGA Palette Snooping Enable
#define PCI_CMD_PEE		0x0040//PEE	RW/RO	0	Parity Error Response Enable
#define PCI_CMD_WCC		0x0080//WCC	RO		0	Wait Cycle Enable
#define PCI_CMD_SEE		0x0100//SEE	RW/RO	0	SERR# Enable
#define PCI_CMD_ID		0x0200//ID	RW		0	Interrupt Disable

#define PCI_STATUS_IS	0x0008//IS	RO		0	Interrupt Status
#define PCI_STATUS_CL	0x0010//CL	RO		1	Capabilities List
#define PCI_STATUS_C66	0x0020//C66	RO		I	66 MHz Capable
#define PCI_STATUS_FBC	0x0080//FBC	RO		I	Fast Back-to-Back Capable
#define PCI_STATUS_DPD	0x0100//DPD	RWC		0	Master Data Pariy Error Detected
#define PCI_STATUS_DEVT	0x0600//DEVT	RO		I	DEVSEL# Timing
#define PCI_STATUS_STA	0x0800//STA	RWC		0	Signaled Target-Abort
#define PCI_STATUS_RTA	0x1000//RTA	RWC		0	Received Target Abort
#define PCI_STATUS_RMA	0x2000//RMA	RWC		0	Received Master-Abort
#define PCI_STATUS_SSE	0x4000//SSE	RWC		0	Single System Error
#define PCI_STATUS_DPE	0x8000//DPE	RWC		0	Detected Parity Error

#define PCI_BIST_CC		0x0f
#define PCI_BIST_SB		0x40
#define PCI_BIST_BC		0x80

#define PCI_C1_ADDR		0xcf8
#define PCI_C1_DATA		0xcfc

#define PCI_C2_FUN		0xcf8
#define PCI_C2_BUS		0xcfa
#define PCI_C2_REGB		0xc000

typedef struct _PCIDEV_ {
	u8 bus;
	u8 dev;
	u8 fun;
	u8 head;
	u8 class;
	u8 sub_class;
	u8 program_if;
	u8 rev;
	u16 vendor;
	u16 device;
	u8 lock;
	void * ctrl;
	int (*int_handle)(struct _PCIDEV_ *,void * ctrl,u16);
	int (*restart)(struct _PCIDEV_ *,void * ctrl);
	int (*close)(struct _PCIDEV_ *,void * ctrl);
	int (*power_off)(struct _PCIDEV_ *,void * ctrl);
	int (*power_on)(struct _PCIDEV_ *,void * ctrl);
	struct _PCIDEV_ * next;
	struct _PCIDEV_ * int_next;
} PCIDEV,*LPPCIDEV;

u8 pci_read_byte(LPPCIDEV dev,u8 reg);
u16 pci_read_word(LPPCIDEV dev,u8 reg);
u32 pci_read_dword(LPPCIDEV dev,u8 reg);
void pci_write_byte(LPPCIDEV dev,u8 reg,u8 data);
void pci_write_word(LPPCIDEV dev,u8 reg,u16 data);
void pci_write_dword(LPPCIDEV dev,u8 reg,u32 data);

LPPCIDEV pci_find_class(LPPCIDEV dev,u32 class,u32 sub_class,u32 program_if);

#endif