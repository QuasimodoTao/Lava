/*
	inc/mm.h
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

#ifndef _MM_H_
#define _MM_H_

#include <stddef.h>
#include <kernel.h>

typedef u64 PAGE,PAGEE,PADDR;
typedef void * VADDR;
//PAGE:不包含属性位，值包含物理页地址或也较好慌21特
//PAGEE:包含属性位和物理页地址

PAGEE get_free_page(int swapable,int leve,int bits);
int free_page(PAGEE page);
int addr_vaild(LPTHREAD thread,VADDR vaddr);
PADDR ADDRV2P(LPTHREAD thread,VADDR vaddr);
int put_page(PAGEE page,LPTHREAD thread,VADDR vaddr);
PAGEE get_page(LPTHREAD thread,VADDR vaddr);
int page_unswapable(LPTHREAD thread,VADDR vaddr,size_t size);
int page_swapable(LPTHREAD thread,VADDR vaddr,size_t size);
int page_allocatable(PAGE page);
int page_disallocatable(PAGE page);
int page_uncacheable(LPTHREAD thread,VADDR vaddr,size_t size);
int page_cacheable(LPTHREAD thread,VADDR vaddr,size_t size);
int free_vaddr(LPTHREAD thread,VADDR vaddr);
VADDR kmalloc(size_t size,unsigned int align);
void kfree(VADDR addr);
void * vmalloc(size_t size);
void vfree(VADDR addr);
u64 ker_heap_clean();
u64 create_paging();
int allocate_area(LPTHREAD thread,VADDR vaddr,size_t size,int attr);
int free_area(LPTHREAD thread,VADDR vaddr,size_t size);
int free_page_table(LPPROCESS process);

#ifdef SPEARATE_STACK
void * alloc_stack();
void free_stack(void*);
#else
#define alloc_stack()       kmalloc(STACK_SIZE,PAGE_SIZE)
#define free_stack(stack)   kfree(stack)
#endif


#define PAGE2PAGEE(page,p,w,u)	((page) | (p?0:1) | ((w?0:1) << 1) | ((u?0:1) << 2))
#define PAGEE2PAGE(page)		((page) & PAGE_PADDR_MASK_4K)
#define P4E(vaddr)	((((u64)(vaddr)) >> PAGE_SHIFT_4) & PAGE_ENT_MASK)
#define P3E(vaddr)	((((u64)(vaddr)) >> PAGE_SHIFT_3) & PAGE_ENT_MASK)
#define P2E(vaddr)	((((u64)(vaddr)) >> PAGE_SHIFT_2) & PAGE_ENT_MASK)
#define P1E(vaddr)	((((u64)(vaddr)) >> PAGE_SHIFT_1) & PAGE_ENT_MASK)
#define PAGE2MAP(page)	((void*)(((page) & PAGE_BIT_MAP_MASK) | 0xffff000000000000))

#endif