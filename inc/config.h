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

//adjustable
#define FIRST_SELECTOR		0x10
#define MAX_CPU_COUNT		32
#define IPI_START_IRQ		0x90
#define IPI_COUNT			0x10
#define IRQ_COUNT			96
//#define VM_DEBUG
#define IRQ_DE_CHECK
#define CPU_TIME_UPDATA_IPI	0
#define CPU_TIME			10


//const
#define KERNEL_CS			(FIRST_SELECTOR)
#define KERNEL_SS			(FIRST_SELECTOR + 0x08)
#define USER_CS_32			(FIRST_SELECTOR + 0x10)
#define USER_SS_32			(FIRST_SELECTOR + 0x18)
#define USER_CS_64			(FIRST_SELECTOR + 0x20)
#define USER_SS_64			(FIRST_SELECTOR + 0x28)
//All physical page mapping at PMEMSTART to PMEMEND.
//Thus,the system only suppose physical memory up to 64TB
#define PMEMSTART	 	0xffff800000000000LL
#define PMEMEND			0xffffbfffffffffffLL

//Kernel Stack:Unswapable stack
//1MB per stack,top 64KB exist,bottom 4KB not exist
//Totoal 16384 Stack
#define KSBA			0xffff800000001800LL
//to 0xffffc00000002fff
#define KSBV			0xffff800000001c00LL
//to 0xffffc00000003fff
#define KSB				0xffffc00800000000LL
#define KSS				0x0000000000100000LL
#define KST				8192

#define TLB				0xffffc00000020000LL
#define MAX_THREAD		16384

//Paging:Allocatable page map.(0 is allocatable)
#define PGADMB			0xffffc00080000000LL
//Paging:Swapable page map.(0 is swapable)
#define PGSDMB			0xffffc00100000000LL
//Kernel Heap:page control block base
#define MMHMCBB			0xffffc00001000000LL
//Kernel Heap:last page control block
#define MMHMCBL			0xffffc00001fffffcLL
//Kernel Heap Base
#define MMHSB			0xffffc00400000000LL
//MMIO:All MMIO space map at there and set uncacheable
//on suppose up to 512MB
#define MMIOBASE		0xffffc00008000000LL
#define MMIOSIZE		0x0000000010000000LL
//CPU Private data page map at here
#define SPEC_DATA_BASE	0xffffc00000100000LL
//MP start up IPI vector
#define AP_ENT_VECTOR			0x01
#define AP_INIT_BASE			0xfffffffffffff000
#define PROCESS_INIT_MUTEX		0xfffffffffffff0f0
#define SYSPDBE_PTR				0xfffffffffffff0f8
#define SYSCALL_ENTER			0xfffffffffffff100
#define SYSENTER_ENTER			0xfffffffffffff200
#define SYSCALL_INT_ENTER(x)	(0xfffffffffffff300 + 0x100 * ((x) - 0x80))

#define SYSCALL_ENTER_CODE_START	0x00007ffffffff000
#define SUPPOSE_SYSCALL_PTR	0x00007ffffffff0f0
#define SUPPOSE_SYSENTER_PTR	0x00007ffffffff0f8

#define VM_FREE_SPACE_START		0x10000
#define VM_FREE_SPACE_SIZE		0x8fc00


#define AHCI_PORT_SPACE_SIZE	512*1024

#define GDT_BASE		0xffff800000009000LL
//to 0xffffc0000000bfff
#define IDT_BASE		0xffff800000008000LL
//to 0xffffc00000004fff
#define INT_ENT_BASE	0xffffffffffff0000LL
//to 0xffffffffffffefff
#define LAPIC_MMIO		0xffffc00000000000LL
//to 0xffffc00000005fff
#define MAX_IOAPIC		4
#define IOAPIC_MMIO		0xffffc00000001000LL
//to 0xffffc00000009fff

#define USER_STACK_P4E_INDEX	255

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
#define PAGE_FIRST_MASK			0x01ff000000000000LL
#define PAGE_FIRST_SHIFT		48

#define VM_CALL_PROC_BASE		0x0000000000000600LL

#define USER_INIT_RSP			0x00007fffffffeff8LL

#define SYSTEM_CALL_PROC_BASE	0x00007ffffffff000LL

#endif