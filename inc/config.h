/*
	inc/config.h
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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define SPEARATE_STACK


//adjustable
#define MAX_CPU_COUNT		32
#define IRQ_START_INT       0x20       
#define IPI_START_INT		0x50
#define IPI_COUNT			0x10
#define IRQ_COUNT			48
//#define VM_DEBUG
#define IRQ_DE_CHECK
#define CPU_TIME_UPDATA_IPI	0
#define CPU_SCHEDULE_IPI    1
#define CPU_TIME			10


//const
#define FIRST_SELECTOR		0x10                    
#define KERNEL_CS			(FIRST_SELECTOR)        //GDT2
#define KERNEL_SS			(FIRST_SELECTOR + 0x08) //GDT3
#define USER_CS_32			(FIRST_SELECTOR + 0x10) //GDT4
#define USER_SS_32			(FIRST_SELECTOR + 0x18) //GDT5
#define USER_CS_64			(FIRST_SELECTOR + 0x20) //GDT6
#define USER_SS_64			(FIRST_SELECTOR + 0x28) //GDT7
//Physical address
//0x00000000_00000000 - 0x00000000_000003ff
//0x00000000_00000400 - 0x00000000_000004ff 256B BDA
//0x00000000_00000500 - 0x00000000_000005ff 256B Original data area
//0x00000000_00000600 - 0x00000000_000007ff 512B Int 0x?? * 256 For VM module
#define VM_CALL_PROC_BASE   0x0000000000000600
//0x00000000_00000800 - 0x00000000_000009ff 512B VESA information
//0x00000000_00000a00 - 0x00000000_00000aff 256B Current VESA mode information
//0x00000000_00000b00 - 0x00000000_00000bff 256B SMAP
//0x00000000_00000c00 - 0x00000000_00000fff 1KB Reserved
//0x00000000_00001000 - 0x00000000_000010ef 240B AP initlize code
//0x00000000_000010f0 - 0x00000000_000010f7 8B Processors initlize mutex
//0x00000000_000010f8 - 0x00000000_000010ff 8B SysPDBE addr
//0x00000000_00001100 - 0x00000000_000011ff 256B Syscall Enter
//0x00000000_00001200 - 0x00000000_000012ff 256B Sysenter Enter
//0x00000000_00001300 - 0x00000000_000013ff 256B Syscall Int80 Enter DPL3
//0x00000000_00001400 - 0x00000000_000014ff 256B Syscall Int81 Enter DPL3
//0x00000000_00001500 - 0x00000000_000015ff 256B Syscall Int82 Enter DPL3
//0x00000000_00001600 - 0x00000000_000016ff 256B Syscall Int83 Enter DPL3
//0x00000000_00001700 - 0x00000000_000017ff 256B Syscall Int84 Enter DPL0
//0x00000000_00001800 - 0x00000000_00001fff 2KB Reserved
//0x00000000_00002000 - 0x00000000_00002fff 4KB PML4
//0x00000000_00003000 - 0x00000000_00003fff 4KB PDPT
//0x00000000_00004000 - 0x00000000_00004fff 4KB PDE0
//0x00000000_00005000 - 0x00000000_00005fff 4KB PDE1
//0x00000000_00006000 - 0x00000000_00006fff 4KB PDE2
//0x00000000_00007000 - 0x00000000_00007fff 4KB PDE3
//0x00000000_00008000 - 0x00000000_00008fff 4KB IDT
#define PIDT_BASE           0x0000000000008000
//0x00000000_00009000 - 0x00000000_0000afff 8KB GDT
#define PGDT_BASE           0x0000000000009000
//0x00000000_0000b000 - 0x00000000_0000ffff 20KB Stack init code
//0x00000000_00010000 - 0x00000000_0008ffff 512KB For VM model
#define VM_FREE_SPACE_START		0x10000
#define VM_FREE_SPACE_SIZE		0x80000
//0x00000000_00090000 - 0x00000000_00095fff 24KB For ISA/EISA DMA
//0x00000000_00096000 - 0x00000000_0009fbff 39KB Reserved
//0x00000000_0009fc00 - 0x00000000_0009ffff 1KB EBDA
//0x00000000_000a0000 - 0x00000000_000bffff 128KB For Video
//0x00000000_000c0000 - 0x00000000_000fffff 256KB For BIOS
//0x00000000_00100000 - ?? Kernel image
//?? - ?? Font
//?? - ?? Config file

//Virtual Address
//0x00000000_00000000 - 0x00000000_3fffffff 1GB reserved
//0x00000000_40000000 - 0x00000000_ffffffff 3GB User program image
//0x00000001_00000000 - 0x000000ff_ffffffff 1020GB private dynamic library
//0x00000100_00000000 - 0x000001ff_ffffffff 1024GB public dynamic library
//0x00000200_00000000 - 0x000002ff_ffffffff 1024GB thread private heap
//0x00000300_00000000 - 0x000003ff_ffffffff 1024GB thread public heap
//0x00000400_00000000 - 0x00007eff_ffffffff ?? reserved
//0x00007f00_00000000 - 0x00007fff_ffffefff 512GB - 4KB thread stack
//0x00007fff_fffff000 - 0x00007fff_ffffffff 4KB For system and read only,mapping 0x1000
#define USER_INIT_RSP               0x00007fffffffeff8
#define USER_STACK_P4E_INDEX	    255
//0xffff8000_00000000 - 0xffff807f_ffffffff 256GB Mapping all physical memory
#define PMEMSTART	 	0xffff800000000000LL
#define PMEMEND			0xffff807fffffffffLL
#define IDT_BASE        (PIDT_BASE | PMEMSTART)
#define GDT_BASE        (PGDT_BASE | PMEMSTART)
//0xffff8040_00000000 - 0xffff8040_007fffff 8MB For thread list
#define TLB             0xffff804000000000
#define MAX_THREAD      1024*1024
//0xffff8040_01000000 - 0xffff8040_01ffffff 16MB Kernel heap control base
#define MMHMCBB			0xffff804001000000LL
#define MMHMCBL			0xffff804001fffffcLL
//0xffff8044_00000000 - 0xffff8047_ffffffff 16GB Kernel heap
#define MMHSB			0xffff804400000000LL
//0xffff8048_00000000 - 0xffff804b_ffffffff 16KB * 1024 * 1024 Kernel Stack.
//per enter is 16KB,but lower 4KB is NULL,to ensure we will know when stack overflow.
#ifdef SPEARATE_STACK
#define STACK_BASE      0xffff804800000000LL
#define STACK_SIZE      0x4000
#else
#define STACK_SIZE      0x1000
#endif

//MP start up IPI vector
#define AP_ENT_VECTOR			0x01
#define AP_INIT_BASE			((AP_ENT_VECTOR << 12) | PMEMSTART)
#define PROCESS_INIT_MUTEX		((AP_ENT_VECTOR << 12) | PMEMSTART + 0x0f0)
#define SYSCALL_ENTER			((AP_ENT_VECTOR << 12) | PMEMSTART + 0x100)
#define SYSENTER_ENTER			((AP_ENT_VECTOR << 12) | PMEMSTART + 0x200)
#define SYSCALL_80_ENTER        ((AP_ENT_VECTOR << 12) | PMEMSTART + 0x300)
#define SYSCALL_81_ENTER        ((AP_ENT_VECTOR << 12) | PMEMSTART + 0x400)
#define SYSCALL_82_ENTER        ((AP_ENT_VECTOR << 12) | PMEMSTART + 0x500)
#define SYSCALL_83_ENTER        ((AP_ENT_VECTOR << 12) | PMEMSTART + 0x600)
#define USER_SYSCALL_ENTER_CODE_START ((AP_ENT_VECTOR << 12) | PMEMSTART + 0x800)
#define SUPPOSE_SYSCALL_PTR	    ((AP_ENT_VECTOR << 12) | PMEMSTART + 0x8f0)
#define SUPPOSE_SYSENTER_PTR	((AP_ENT_VECTOR << 12) | PMEMSTART + 0x8f8)
#define USER_SYSCALL_80_ENTER   ((AP_ENT_VECTOR << 12) | PMEMSTART + 0x900)
#define USER_SYSCALL_81_ENTER   ((AP_ENT_VECTOR << 12) | PMEMSTART + 0x910)
#define USER_SYSCALL_82_ENTER   ((AP_ENT_VECTOR << 12) | PMEMSTART + 0x920)
#define USER_SYSCALL_83_ENTER   ((AP_ENT_VECTOR << 12) | PMEMSTART + 0x930)

#define AHCI_PORT_SPACE_SIZE	512*1024
#define MAX_IOAPIC 8

#define PAGE_SIZE_512G	0x0000008000000000LLU
#define PAGE_SIZE_1G	0x0000000040000000LLU
#define PAGE_SIZE_2M	0x0000000000200000LLU
#define PAGE_SIZE_4K	0x0000000000001000LLU
#define PAGE_SIZE		PAGE_SIZE_4K
#define PAGE_SHIFT_4	39
#define PAGE_SHIFT_3	30
#define PAGE_SHIFT_2	21
#define PAGE_SHIFT_1	12
#define PAGE_ENT_MASK	0x1ff
#define PAGE_BUSY_BIT	62
#define PAGE_ALLOCATED_MASK_BIT	61
#define PAGE_ALLOCATED_MASK	(1LL << PAGE_ALLOCATED_MASK_BIT)
#define PAGE_BUSY_MASK	(1LL << PAGE_BUSY_BIT)
#define PAGE_EXIST		0x0000000000000001LL
#define PAGE_WRITE		0x0000000000000002LL
#define PAGE_USER		0x0000000000000004LL
#define PAGE_PWT		0x0000000000000008LL
#define PAGE_PCD		0x0000000000000010LL
#define PAGE_ACCESSED	0x0000000000000020LL
#define PAGE_DIRTY		0x0000000000000040LL
#define PAGE_BIG		0x0000000000000080LL
#define PAGE_PADDR_MASK_1G	0x000fffffc0000000LL
#define PAGE_PADDR_MASK_2M	0x000fffffffe00000LL
#define PAGE_PADDR_MASK_4K	0x000ffffffffff000LL
#define PAGE_PADDR_MASK		PAGE_PADDR_MASK_4K
#define PAGE_IN_ADDR_MASK_1G	0x000000003fffffffLL
#define PAGE_IN_ADDR_MASK_2M	0x00000000001fffffLL
#define PAGE_IN_ADDR_MASK_4K	0x0000000000000fffLL
#define PAGE_IN_ADDR_MASK		PAGE_IN_ADDR_MASK_4K
#define SWAP_ID_MASK			0x00000ffffffff000LL
#define PAGE_BIT_MAP_MASK		0x0000ffffffffffc0LL

#define MAX_P4E                 256

#define PAGE_FIRST_MASK			0x01ff000000000000LL
#define PAGE_FIRST_SHIFT		48


#endif