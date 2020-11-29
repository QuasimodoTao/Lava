//Lava OS
//PageFault
//20-03-10-13-34

#ifndef _MM_H_
#define _MM_H_

#include <stddef.h>
#include <kernel.h>

typedef u64 PAGE,PAGEE,PADDR;
typedef void * VADDR;
//PAGE:不包含属性位，值包含物理页地址或也较好慌21特
//PAGEE:包含属性位和物理页地址

PAGEE get_free_page(int swapable,int leve);
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

void * alloc_stack();
void free_stack(VADDR _stack);

u64 create_paging();

void * mmio_map(u64 paddr,size_t size);

#define PAGE2PAGEE(page,p,w,u)	((page) | (p?0:1) | ((w?0:1) << 1) | ((u?0:1) << 2))
#define PAGEE2PAGE(page)		((page) & PAGE_PADDR_MASK_4K)
#define P4E(vaddr)	((((u64)(vaddr)) >> PAGE_SHIFT_4) & PAGE_ENT_MASK)
#define P3E(vaddr)	((((u64)(vaddr)) >> PAGE_SHIFT_3) & PAGE_ENT_MASK)
#define P2E(vaddr)	((((u64)(vaddr)) >> PAGE_SHIFT_2) & PAGE_ENT_MASK)
#define P1E(vaddr)	((((u64)(vaddr)) >> PAGE_SHIFT_1) & PAGE_ENT_MASK)
#define PAGE2MAP(page)	((void*)(((page) & PAGE_BIT_MAP_MASK) | 0xffff000000000000))

#endif