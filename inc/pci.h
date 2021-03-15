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

/*
0x0000,0x0000,"Non-VGA-Compatible devices"
0x0000,0x0001,"VGA-Compatible Device"
0x0001,0x0000,"SCSI Bus Controller"
0x0001,0x0001,"ISA Compatibility mode-only controller"
0x0001,0x0002,"Floppy Disk Controller"
0x0001,0x0003,"IPI Bus Controller"
0x0001,0x0004,"RAID Controller"
0x0001,0x0005,"ATA Controller"
0x0001,0x0006,open,"Serial ATA"
0x0001,0x0007,"Serial Attached SCSI"
0x0001,0x0008,"Non-Volatile Memory Controller"
0x0001,0x0080,"Unknow Mass Storage Controller"
0x0002,0x0000,"Ethernet Controller"
0x0002,0x0001,"Token Ring Controller"
0x0002,0x0002,"FDDI Controller"
0x0002,0x0003,"ATM Controller"
0x0002,0x0004,"ISDN Controller"
0x0002,0x0005,"WorldFip Controller"
0x0002,0x0006,"PICMG 2.14 Multi Computing"
0x0002,0x0007,"Infiniband Controller"
0x0002,0x0008,"Fabric Controller"
0x0002,0x0080,"Unknow Network Controller"
0x0003,0x0000,"VGA Compatible Controller"
0x0003,0x0001,"XGA Controller"
0x0003,0x0002,"3D Controller (Not VGA-Compatible)"
0x0003,0x0080,"Unknow Display Controller"
0x0004,0x0000,"Multimedia Video Controller"
0x0004,0x0001,"Multimedia Audio Controller"
0x0004,0x0002,"Computer Telephony Device"
0x0004,0x0003,"Audio Device"
0x0004,0x0080,"Unknow Multimedia Controller"
0x0005,0x0000,"RAM Controller"
0x0005,0x0001,"Flash Controller"
0x0005,0x0080,"Unknow Memory Controller"
0x0006,0x0000,"Host Bridge"
0x0006,0x0001,"ISA Bridge"
0x0006,0x0002,"EISA Bridge"
0x0006,0x0003,"MCA Bridge"
0x0006,0x0004,"PCI-to-PCI Bridge"
0x0006,0x0005,"PCMCIA Bridge"
0x0006,0x0006,"NuBus Bridge"
0x0006,0x0007,"CardBus Bridge"
0x0006,0x0008,"RACEway Bridge"
0x0006,0x0009,"PCI-to-PCI Bridge"
0x0006,0x000a,"InfiniBand-to-PCI Host Bridge"
0x0006,0x0080,"Unknow Bridge Device"
0x0007,0x0000,"Serial Controller"
0x0007,0x0001,"Parallel Controller"
0x0007,0x0002,"Multiport Serial Controller"
0x0007,0x0003,"Modem"
0x0007,0x0004,"IEEE 488.1/2 (GPIB) Controller"
0x0007,0x0005,"Smart Card"
0x0007,0x0080,"Unknow Simple Communication Controller"
0x0008,0x0000,"PIC"
0x0008,0x0001,"DMA Controller"
0x0008,0x0002,"Timer"
0x0008,0x0003,"RTC Controller"
0x0008,0x0004,"PCI Hot-Plug Controller"
0x0008,0x0005,"SD Host controller"
0x0008,0x0006,"IOMMU"
0x0008,0x0080,"Unknow Base System Peripheral"
0x0009,0x0000,"Keyboard Controller"
0x0009,0x0001,"Digitizer Pen"
0x0009,0x0002,"Mouse Controller"
0x0009,0x0003,"Scanner Controller"
0x0009,0x0004,"Gameport Controller"
0x0009,0x0080,"Unknow Input Device Controller"
0x000a,0x0000,"Generic Docking Station"
0x000a,0x0080,"Unknoe Docking Station"
0x000b,0x0000,"i386 Procrssor"
0x000b,0x0001,"i486 Processor"
0x000b,0x0002,"Pentium"
0x000b,0x0003,"Pentium Pro"
0x000b,0x0010,"Alpha"
0x000b,0x0020,"PowerPC"
0x000b,0x0030,"MIPS"
0x000b,0x0040,"Co-Processor"
0x000b,0x0080,"Unknow Processor"
0x000c,0x0000,"FireWire (IEEE 1394) Controller"
0x000c,0x0001,"ACCESS Bus"
0x000c,0x0002,"SSA"
0x000c,0x0003,"USB Controller"
0x000c,0x0004,"Fibre Channel"
0x000c,0x0005,"SMBus"
0x000c,0x0006,"InfiniBand"
0x000c,0x0007,"IPMI Interface"
0x000c,0x0008,"SERCOS Interface (IEC 61491)"
0x000c,0x0080,"Unknow Serial Bus Controller"
0x000d,0x0000,"iRDA Compatible Controller"
0x000d,0x0001,"Consumer IR Controller"
0x000d,0x0010,"RF Controller"
0x000d,0x0011,"Bluetooth Controller"
0x000d,0x0012,"Broadband Controller"
0x000d,0x0020,"Ethernet Controller (802.1a)"
0x000d,0x0021,"Ethernet Controller (802.1b)"
0x000d,0x0080,"Unknow Wireless Controller"
0x000e,0x0000,"I20"
0x000f,0x0001,"Satellite TV Controller"
0x000f,0x0002,"Satellite Audio Controller"
0x000f,0x0003,"Satellite Voice Controller"
0x000f,0x0004,"Satellite Data Controller"
0x0010,0x0000,"Network and Computing Encrpytion/Decryption"
0x0010,0x0010,"Entertainment Encryption/Decryption"
0x0010,0x0080,"Other Encryption/Decryption"
0x0011,0x0000,"DPIO Modules"
0x0011,0x0001,"Performance Counters"
0x0011,0x0010,"Communication Synchronizer"
0x0011,0x0020,"Signal Processing Management"
0x0011,0x0080,"Unknow Signal Processing Controller"
0x0012,0xffff,"Processing Accelerator"
0x0013,0xffff,"Non-Essential Instrumentation"
0x0040,0xffff,"Co-Processor"
0x00FF,0xffff,"Unassigned Class (Vendor specific)"
*/


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

struct _PCI_DEV_MODULE_ {
	HANDLE (*construct)(struct _PCIDEV_ * dev);
	void (*destruct)(struct _PCIDEV_ * dev,HANDLE ctrl);
	short class_code;
	short subclass_code;
	short program_flag; 
	const char * name;
};

#define PCI_DEV_MODULE_SECTION_NAME		".pcidev"

#define PCI_DEV_MODULE(cons,des,_class,sub,progif,name)	\
static struct _PCI_DEV_MODULE_ __attribute__((__used__,__section__(PCI_DEV_MODULE_SECTION_NAME))) pci_dev_module_##name = {\
	cons,des,_class,sub,progif,#name\
};


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
	wchar_t name[24];// L"/.dev/pci/bbb-dd-f.dev"

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