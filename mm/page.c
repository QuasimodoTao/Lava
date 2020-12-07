/*
	mm/page.c
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

#include <stddef.h>
#include <asm.h>
#include <lava.h>
#include <string.h>
#include <arch.h>
#include <int.h>
#include <kernel.h>
#include <mm.h>
#include <spinlock.h>

spin_optr_array_bit(PDE,u64,PAGE_BUSY_BIT);

/*
Page Exist
bit0:P if 1
bit1:R/W
bit2:U/S
bit3:PWT
bit4:PCD
bit5:A
bit6:D
bit7:PAT/PS
bit8:G
bit9-11:
bit12-45:Physical page address
bit46-47:0
bit48-61:rvd
bit62:busy
bit63:XD

Page is Swaped out,Only Bottom Page
bit0:P if 0
bit1:R/W
bit2:U/S
bit3-11:rvd
bit12-43:swap id
bit44-60:rvd
bit61:extend page exist bit if 0
bit62:busy
bit63:XD

Address is Allocated but not allocated physical page,For Page table(PML4,PDPT,PD)
bit0:P if 0
bit1:R/W
bit2:U/S
bit3-5:Rvd
bit6-bit47:Bit-map pointer//only PML4E,PDPTE,PDE
bit48-60:rvd
bit61:extend page exist bit if 1
bit62:busy
bit63:XD

P	SD	CD
0	x	x	页面于交换区或不存在
1	0	0	页面存在
1	0	1	无效
1	1	0	可缓冲的mmio或dma区域或各级页表
1	1	1	不可缓冲mmio或dma区域

if(bit0) //Address Vaild
else
	if(bit61)
		if(bit6 - bit47) //bit map exist,not all page allocated
		else //all page is allocated
	else //all page are not allocated
	
	
if(bit0 || bit61) address vaild
else address unvaild
*/

static u64 syspdbe;
static u64 page_count;
static u64 cur_page;
static u64 free_page_base;
#if (MAX_THREAD <= 255)
static unsigned char * mem_map;
#define MM_CMPXCHG(i,c,d,r)	cmpxchg1b(mem_map + i,c,d,r)
#define MM_XADD(i,v)		xaddb(mem_map + i,v)		
#elif (MAX_THREAD <= 65535)
static unsigned short * mem_map;
#define MM_CMPXCHG(i,c,d,r)	cmpxchg2b(mem_map + i,c,d,r)		
#define MM_XADD(i,v)		xaddw(mem_map + i,v)		
#else
static unsigned int * mem_map;
#define MM_CMPXCHG(i,c,d,r)	cmpxchg4b(mem_map + i,c,d,r)
#define MM_XADD(i,v)		xaddd(mem_map + i,v)		
#endif

//获取空闲的页,将分配实际的物理页
static PAGE __get_free_page(int bits){//lock-free
	u64 i, first_free;
	u64 my_last;

	if(bits == 24) my_last = (1LL << 12) - (free_page_base >> 12);
	else if(bits == 32) my_last = (1LL << 20) - (free_page_base >> 12);
	else my_last = page_count;
	if(my_last > page_count) my_last = page_count;
	first_free = cur_page;
	for(i = first_free;i;i--) 
		if(!mem_map[i] && !MM_CMPXCHG(i,0,1,NULL)){
			cmpxchg8b(&cur_page,first_free,i - 1,NULL);
			return (i << 12) + free_page_base;
		}
	for(i = my_last - 1;i;i--) 
		if(!mem_map[i] && !MM_CMPXCHG(i,0,1,NULL)){
			cmpxchg8b(&cur_page,first_free,i - 1,NULL);
			return (i << 12) + free_page_base;
		}
	//TODO:swap out
	print("No more page can be use.\n");
	asm("cli\nhlt"::"a"(page_count),"b"(cur_page));
	stop();
}
//获取页
PAGEE get_free_page(int swapable,int leve, int bits){
	PAGEE page;
	if(swapable){
		if(leve) return PAGE_ALLOCATED_MASK | PAGE_WRITE | PAGE_USER;
		else return PAGE_ALLOCATED_MASK | PAGE_WRITE;
	}
	else{
		page = __get_free_page(bits);
		if(leve) return page | PAGE_WRITE | PAGE_USER | PAGE_EXIST;
		else return page | PAGE_WRITE | PAGE_EXIST;
	}
}
//释放页
int free_page(PAGEE page){//lock-free
	if(page & PAGE_EXIST) {
		page -= free_page_base;
		page >>= PAGE_SHIFT_1;
		if(page >=  page_count) return -1;
		MM_XADD(page,-1);
		return 0;
	}
	else{
		if(!page) return -1;
		if(!(page & PAGE_ALLOCATED_MASK)) {
			//BUG:释放未分配的页面
			return -1;
		}
		page &= SWAP_ID_MASK;
		page >>= PAGE_SHIFT_1;
		swap_clear(page);//******************************************
		return 0;
	}
}
//测试并解压位图
//先测试，有效则解压位图，无效则不解压
static int pg_ext_test_and_depress(u64 * pde,int index,u64 user_mode){
	void * bit_map;
	u64 * p;
	u32 i;
	
	if(*pde & PAGE_ALLOCATED_MASK){
		if(*pde & PAGE_BIT_MAP_MASK){
			bit_map = PAGE2MAP(*pde);
			if(!bt(bit_map,index)) return -1;
			*pde = get_free_page(0,0,0) | PAGE_BUSY_MASK | user_mode;
			p = PADDR2V(*pde);
			for(i = 0;i < PAGE_ENT_MASK + 1;i++){
				if(bt(bit_map,i)) p[i] = PAGE_ALLOCATED_MASK;
				else p[i] = 0;
			}
			kfree(bit_map);
		}
		else{
			*pde = get_free_page(0,0,0) | PAGE_BUSY_MASK | user_mode;
			p = PADDR2V(*pde);
			for(i = 0;i < PAGE_ENT_MASK + 1;i++) p[i] = PAGE_ALLOCATED_MASK;
		}
	}
	else return -1;
}
//测试位图，但不解压
static int pg_ext_test(u64 * pde,int index){
	if(!(*pde & PAGE_ALLOCATED_MASK) || 
		(*pde & PAGE_BIT_MAP_MASK && !bt(PAGE2MAP(*pde),index))) return 0;
	else return 1;
}
//测试索引对应是否存在，存在则返回-1，否则解压位图或获取新物理页，返回0
static int pg_ext_test_and_depress_new(u64 * pde,int index,u64 user_mode){
	void * bit_map;
	u64 * pl;
	u32 i;
	
	if(*pde & PAGE_ALLOCATED_MASK){
		if(*pde & PAGE_BIT_MAP_MASK){
			bit_map = PAGE2MAP(*pde);
			if(bt(bit_map,index)) return -1;
			*pde = get_free_page(0,0,0) | PAGE_BUSY_MASK | user_mode;
			pl = PADDR2V(*pde);
			for(i = 0;i <= PAGE_ENT_MASK;i++){
				if(bt(bit_map,i)) pl[i] = PAGE_ALLOCATED_MASK;
				else pl[i] = 0;
			}
			kfree(bit_map);
		}
		else return -1;
	}
	else{
		*pde = get_free_page(0,0,0) | PAGE_BUSY_MASK | user_mode;
		memset(PADDR2V(*pde),0,PAGE_SIZE);
	}
	return 0;
}
//解压位图当前指针有效且已压缩则解压，否则则申请新页面
static void pg_ext_depress_new(u64 * pde,u64 user_mode){
	void * bit_map;
	u64 * pl;
	PAGEE page;
	u32 i;
	
	if(*pde & PAGE_ALLOCATED_MASK) {
		bit_map = PAGE2MAP(*pde);
		page = *pde;
		*pde = get_free_page(0,0,0) | PAGE_BUSY_MASK | user_mode;
		pl = PADDR2V(*pde);
		if(page & PAGE_BIT_MAP_MASK){
			for(i = 0;i <= PAGE_ENT_MASK;i++){
				if(bt(bit_map,i)) pl[i] = PAGE_ALLOCATED_MASK;
				else pl[i] = 0;
			}
			kfree(bit_map);
		}
		else for(i = 0;i <= PAGE_ENT_MASK;i++) pl[i] = PAGE_ALLOCATED_MASK;
	}
	else{
		*pde = get_free_page(0,0,0) | PAGE_BUSY_MASK | user_mode;
		memset(PADDR2V(*pde),0,PAGE_SIZE);
	}
}

static int allocate_area_4K(LPTHREAD thread,VADDR vaddr,u32 count,int attr){
	u64 * p4,*p3,*p2,*p1;
	u32 p4e,p3e,p2e,p1e,i;
	void * bit_map = NULL;
	PAGEE page;
	LPPROCESS process;
	u64 first,rf,user_mode = 0;
	
	if(!count) return 0;
	p4e = P4E(vaddr);
	if(p4e >= 256){
		if(p4e > MAX_P4E){
			//error
		}
		else p4 = (&syspdbe) - 256;
	}
	else {
		if(!thread) thread = GetCurThread();
		if(!thread) return -1;
		user_mode = PAGE_USER;
		if(p4e == USER_STACK_P4E_INDEX)
			p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
		else{
			process = thread->father;
			if(!process) return -1;
			p4 = process->pdbe;
		}
	}
	SFI(rf);
	p3e = P3E(vaddr);
	LockPDE(p4,p4e);
	if(!(p4[p4e] & PAGE_EXIST) && pg_ext_test_and_depress_new(p4 + p4e,p3e,user_mode)){
		UnlockPDE(p4,p4e);
		LF(rf);
		return -1;
	}
	p3 = PADDR2V(p4[p4e]);
	LockPDE(p3,p3e);
	if(attr & PAGE_EXIST){
		UnlockPDE(p4,p4e);
		p2e = P2E(vaddr);
		if((!(p3[p3e] & PAGE_EXIST) && pg_ext_test_and_depress_new(p3 + p3e,p2e,user_mode)) || 
			p3[p3e] & PAGE_BIG){
			UnlockPDE(p3,p3e);
			LF(rf);
			return -1;
		}
		p2 = PADDR2V(p3[p3e]); 
		LockPDE(p2,p2e);
		UnlockPDE(p3,p3e);
		p1e = P1E(vaddr);
		if(!(p2[p2e] & PAGE_EXIST)){
			if(p2[p2e] & PAGE_ALLOCATED_MASK){
				if(!(p2[p2e] & PAGE_BIT_MAP_MASK)) {
					UnlockPDE(p2,p2e);
					LF(rf);
					return -1;
				}
				bit_map = PAGE2MAP(p2[p2e]);
				for(i = 0;i < count;i++,p1e++){
					if(bt(bit_map,p1e)){
						UnlockPDE(p2,p2e);
						LF(rf);
						return -1;
					}
				}
				p1e = P1E(vaddr);
				p2[p2e] = get_free_page(0,attr & PAGE_USER,0) | PAGE_BUSY_MASK;
				p1 = PADDR2V(p2[p2e]);
				for(i = 0;i <= PAGE_ENT_MASK;i++){
					if(bt(bit_map,i)) p1[i] = PAGE_ALLOCATED_MASK | (attr & PAGE_USER);
					else p1[i] = 0;
				}
				kfree(bit_map);
			}
			else{
				p2[p2e] = get_free_page(0,attr & PAGE_USER,0) | PAGE_BUSY_MASK;
				memset(PADDR2V(p2[p2e]),0,PAGE_SIZE);
			}
		}
		else if(p2[p2e] & PAGE_BIG){
			UnlockPDE(p2,p2e);
			LF(rf);
			return -1;
		}
		else{
			p1 = PADDR2V(p2[p2e]);
			for(i = 0;i < count;i++,p1e++){
				if(!(p1[p1e] & PAGE_EXIST)) continue;
				UnlockPDE(p2,p2e);
				LF(rf);
				return -1;
			}
			p1e = P1E(vaddr);
		}
		p1 = PADDR2V(p2[p2e]); 
		while(count){
			p1[p1e] = get_free_page(0,attr & PAGE_USER,0);
			count--;
			p1e++;
		}
		UnlockPDE(p2,p2e);
		LF(rf);
		return 0;
	}
	//可以使用扩展页结构记录
	p2e = P2E(vaddr);
	if(!(p3[p3e] & PAGE_EXIST) && pg_ext_test_and_depress_new(p3+p3e,p2e,user_mode)){
		//对应地址已有效，返回错误
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		LF(rf);
		return -1;
	}
	p2 = PADDR2V(p3[p3e]);
	LockPDE(p2,p2e);
	p1e = P1E(vaddr);
	if(p2[p2e] & PAGE_EXIST){
		p1 = PADDR2V(p2[p2e]);
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		if(p1[p1e] & (PAGE_EXIST | PAGE_ALLOCATED_MASK)){
			//对应地址已有效，返回错误
			UnlockPDE(p2,p2e);
			LF(rf);
			return -1;
		}
		while(count){
			p1[p1e] = PAGE_ALLOCATED_MASK | (attr & PAGE_USER);
			count--;
			p1e++;
		}
		UnlockPDE(p2,p2e);
		LF(rf);
		return 0;
	}
	if(!(p2[p2e] & PAGE_ALLOCATED_MASK)){
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memset(bit_map,0,(PAGE_ENT_MASK + 1)/8);
		first = p1e ? 0 : p1e;
		while(count){
			bts(bit_map,p1e);
			count--;
			p1e++;
		}
		p2[p2e] = (first << PAGE_FIRST_SHIFT) | PAGE_ALLOCATED_MASK | 
			((u64)bit_map & PAGE_BIT_MAP_MASK);
		//UnlockPDE(p2,p2e);
		LF(rf);
		return 0;
	}
	if(!(p2[p2e] & PAGE_BIT_MAP_MASK)){
		UnlockPDE(p2,p2e);
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		LF(rf);
		return -1;
	}
	bit_map = PAGE2MAP(p2[p2e]);
	for(i = 0;i < count;i++,p1e++){
		if(bt(bit_map,p1e)){
			UnlockPDE(p2,p2e);
			UnlockPDE(p3,p3e);
			UnlockPDE(p4,p4e);
			LF(rf);
			return -1;
		}
	}
	p1e = P1E(vaddr);
	for(i = 0;i < count;i++,p1e++) bts(bit_map,p1e);
	p1e = P1E(vaddr);
	first = (p2[p2e] & PAGE_FIRST_MASK) >> PAGE_FIRST_SHIFT;
	if(first > p1e){
		//BUG
		first = 0;
	}
	if(first < p1e){
		UnlockPDE(p2,p2e);
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		LF(rf);
		return 0;
	}
	while((first < PAGE_ENT_MASK + 1) && bt(bit_map,first)) first++;
	if(first <= PAGE_ENT_MASK){
		p2[p2e] &= ~PAGE_FIRST_MASK;
		p2[p2e] |= first << PAGE_FIRST_SHIFT;
		UnlockPDE(p2,p2e);
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		LF(rf);
		return 0;
	}
	p2[p2e] = PAGE_ALLOCATED_MASK | (attr & PAGE_USER);
	//UnlockPDE(p2,p2e);
	memset(bit_map,0,(PAGE_ENT_MASK + 1)/8);
	for(i = 0;i < PAGE_ENT_MASK + 1;i++){
		if(p2[i] == PAGE_ALLOCATED_MASK) bts(bit_map,i);
		else break;
	}
	if((i < PAGE_ENT_MASK + 1) && !p2[i]){
		for(;i < PAGE_ENT_MASK + 1;i++)
			if(p2[i] == PAGE_ALLOCATED_MASK) bts(bit_map,i);
			else if(p2[i]) break;
	}
	if(i <= PAGE_ENT_MASK){
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		LF(rf);
		kfree(bit_map);
		return 0;
	}
	free_page(p3[p3e]);
	if(first <= PAGE_ENT_MASK){
		p3[p3e] &= ~(PAGE_BIT_MAP_MASK | PAGE_FIRST_MASK);
		p3[p3e] |= ((u64)bit_map & PAGE_BIT_MAP_MASK) | (first << PAGE_FIRST_SHIFT);
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		LF(rf);
		return 0;
	}
	p3[p3e] = PAGE_ALLOCATED_MASK | (attr & PAGE_USER);
	//UnlockPDE(p3,p3e);
	memset(bit_map,0,(PAGE_ENT_MASK + 1)/8);
	for(i = 0;i < PAGE_ENT_MASK + 1;i++)
		if(p3[i] == PAGE_ALLOCATED_MASK) bts(bit_map,i);
		else break;
	first = i;
	if((i < PAGE_ENT_MASK + 1) && !p3[i]){
		for(;i < PAGE_ENT_MASK + 1;i++)
			if(p3[i] == PAGE_ALLOCATED_MASK) bts(bit_map,i);
			else if(p3[i]) break;
	}
	if(i <= PAGE_ENT_MASK){
		UnlockPDE(p4,p4e);
		LF(rf);
		kfree(bit_map);
		return 0;
	}
	free_page(p4[p4e]);
	if(first >= PAGE_ENT_MASK + 1) {
		p4[p4e] = PAGE_ALLOCATED_MASK | (attr & PAGE_USER);
		//UnlockPDE(p4,p4e);
		LF(rf);
		kfree(bit_map);
		return 0;
	}
	p4[p4e] &= ~(PAGE_BIT_MAP_MASK | PAGE_FIRST_MASK);
	p4[p4e] |= ((u64)bit_map & PAGE_BIT_MAP_MASK) | (first << PAGE_FIRST_SHIFT);
	UnlockPDE(p4,p4e);
	LF(rf);
	return 0;
}
static int allocate_area_2M(LPTHREAD thread,VADDR vaddr,u32 count,int attr){
	u64 * p4,*p3,*p2,*p1;
	u32 p4e,p3e,p2e;
	void * bit_map;
	PAGEE page;
	LPPROCESS process;
	u32 i,j;
	u64 first;
	u64 user_mode = 0;
	u64 rf;
	
	if(!count) return 0;
	p4e = P4E(vaddr);
	if(p4e >= 256){
		if(p4e > MAX_P4E){
			//error
		}
		else p4 = (&syspdbe) - 256;
	}
	else {
		if(!thread) thread = GetCurThread();
		if(!thread) return -1;
		user_mode = PAGE_USER;
		if(p4e == USER_STACK_P4E_INDEX)
			p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
		else{
			process = thread->father;
			if(!process) return -1;
			p4 = process->pdbe;
		}
	}
	SFI(rf);
	LockPDE(p4,p4e);
	p3e = P3E(vaddr);
	if(!(p4[p4e] & PAGE_EXIST) && pg_ext_test_and_depress_new(p4 + p4e,p3e,user_mode)){
		UnlockPDE(p4,p4e);
		LF(rf);
		return -1;
	}
	p3 = PADDR2V(p4[p4e]);
	LockPDE(p3,p3e);
	if(attr & PAGE_EXIST){
		UnlockPDE(p4,p4e);
		if(p3[p3e] & PAGE_EXIST){
			p2 = PADDR2V(p3[p3e]);
			p2e = P2E(vaddr);
			for(i = 0;i < count;i++,p2e++){
				if(p2[p2e] & PAGE_EXIST){
					p1 = PADDR2V(p2[p2e]);
					for(j = 0;j <= PAGE_ENT_MASK;j++){
						if(p1[j] & PAGE_EXIST || p1[j] & PAGE_ALLOCATED_MASK) {
							UnlockPDE(p3,p3e);
							LF(rf);
							return -1;
						}
					}
				}
				else if (p2[p2e] & PAGE_ALLOCATED_MASK) {
					UnlockPDE(p3,p3e);
					LF(rf);
					return -1;
				}
			}
			p2e = P2E(vaddr);
			while(count){
				if(!(p2[p2e] & PAGE_EXIST)) p2[p2e] = get_free_page(0,attr & PAGE_USER,0);
				p1 = PADDR2V(p2[p2e]);
				for(i = 0;i <= PAGE_ENT_MASK;i++) p1[i] = get_free_page(0,attr & PAGE_USER,0);
				count--;
				p2e++;
			}
			UnlockPDE(p3,p3e);
			LF(rf);
			return 0;
		}
		if(!(p3[p3e] & PAGE_ALLOCATED_MASK)){
			p3[p3e] = get_free_page(0,attr & PAGE_USER,0) | PAGE_BUSY_MASK;
			p2 = PADDR2V(p3[p3e]);
			p2e = P2E(vaddr);
			while(count){
				if(!(p2[p2e])) p2[p2e] = get_free_page(0,attr & PAGE_USER,0);
				p1 = PADDR2V(p2[p2e]);
				for(i = 0;i <= PAGE_ENT_MASK;i++) p1[i] = get_free_page(0,attr & PAGE_USER,0);
				count--;
				p2e++;
			}
			UnlockPDE(p3,p3e);
			LF(rf);
			return 0;
		}
		if(!(p3[p3e] & PAGE_BIT_MAP_MASK)){
			UnlockPDE(p3,p3e);
			LF(rf);
			return -1;
		}
		p2e = P2E(vaddr);
		bit_map = PAGE2MAP(p3[p3e]);
		for(i = 0;i < count;i++,p2e++){
			if(bt(bit_map,p2e)){
				UnlockPDE(p3,p3e);
				LF(rf);
				return -1;
			}
		}
		p3[p3e] = get_free_page(0,attr & PAGE_USER,0) | PAGE_BUSY_MASK;
		p2 = PADDR2V(p3[p3e]);
		for(i = 0;i <= PAGE_ENT_MASK;i++){
			if(bt(bit_map,i)) p2[i] = PAGE_ALLOCATED_MASK | (attr & PAGE_USER);
			else p2[i] = 0;
		}
		kfree(bit_map);
		p2e = P2E(vaddr);
		while(count){
			p2[p2e] = get_free_page(0,attr & PAGE_USER,0);
			p1 = PADDR2V(p2[p2e]);
			for(i = 0;i <= PAGE_ENT_MASK;i++) p1[i] = get_free_page(0,attr & PAGE_USER,0);
			count--;
			p2e++;
		}
		UnlockPDE(p3,p3e);
		LF(rf);
		return 0;
	}
	if(p3[p3e] & PAGE_EXIST){
		UnlockPDE(p4,p4e);
		p2e = P2E(vaddr);
		p2 = PADDR2V(p3[p3e]);
		for(i = 0;i < count;i++,p2e++){
			if(p2[p2e] & PAGE_EXIST){
				for(j = 0;j <= PAGE_ENT_MASK;j++){
					if(p1[j] & PAGE_EXIST || p1[j] & PAGE_ALLOCATED_MASK) {
						UnlockPDE(p3,p3e);
						LF(rf);
						return -1;
					}
				}
				free_page(p2[p2e]);
				p2[p2e] = 0;
			}
			else if(p2[p2e] & PAGE_ALLOCATED_MASK){
				UnlockPDE(p3,p3e);
				LF(rf);
				return -1;
			}
		}
		p2e = P2E(vaddr);
		while(count){
			p2[p2e] = PAGE_ALLOCATED_MASK | (attr & PAGE_USER);
			count--;
			p2e++;
		}
		UnlockPDE(p3,p3e);
		LF(rf);
		return 0;
	}
	if(!(p3[p3e] & PAGE_ALLOCATED_MASK)){
		UnlockPDE(p4,p4e);
		bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memset(bit_map,0,(PAGE_ENT_MASK + 1)/8);
		p2e = P2E(vaddr);
		while(count){
			bts(bit_map,p2e);
			p2e++;
			count--;
		}
		p3[p3e] = PAGE_ALLOCATED_MASK | ((u64)bit_map & PAGE_BIT_MAP_MASK) | (attr & (PAGE_WRITE | PAGE_USER));
		//UnlockPDE(p3,p3e);
		LF(rf);
		return 0;
	}
	if(!(p3[p3e] & PAGE_BIT_MAP_MASK)){
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		LF(rf);
		return -1;
	}
	p2e = P2E(vaddr);
	bit_map = PAGE2MAP(p3[p3e]);
	for(i = 0;i < count;i++,p2e++){
		if(bt(bit_map,p2e)){
			UnlockPDE(p3,p3e);
			UnlockPDE(p4,p4e);
			LF(rf);
			return -1;
		}
	}
	p2e = P2E(vaddr);
	for(i = 0;i < count;i++,p2e++) bts(bit_map,p2e);
	p2e = P2E(vaddr);
	first = (p3[p3e] & PAGE_FIRST_MASK) >> PAGE_FIRST_SHIFT;
	if(first > p2e){
		//BUG
		first = 0;
	}
	if(first < p2e){
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		LF(rf);
		return 0;
	}
	while((first < PAGE_ENT_MASK + 1) && bt(bit_map,first)) first++;
	if(first <= PAGE_ENT_MASK){
		p3[p3e] &= ~PAGE_FIRST_MASK;
		p3[p3e] |= first << PAGE_FIRST_SHIFT;
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		LF(rf);
		return 0;
	}
	p3[p3e] = PAGE_ALLOCATED_MASK | (attr & PAGE_USER);
	//UnlockPDE(p3,p3e);
	memset(bit_map,0,(PAGE_ENT_MASK + 1)/8);
	for(i = 0;i < PAGE_ENT_MASK + 1;i++){
		if(p3[i] == PAGE_ALLOCATED_MASK) bts(bit_map,i);
		else break;
	}
	first = i;
	if((i < PAGE_ENT_MASK + 1) && !p3[i]){
		for(;i < PAGE_ENT_MASK + 1;i++)
			if(p3[i] == PAGE_ALLOCATED_MASK) bts(bit_map,i);
			else if(p3[i]) break;
	}
	if(i <= PAGE_ENT_MASK){
		UnlockPDE(p4,p4e);
		LF(rf);
		kfree(bit_map);
		return 0;
	}
	free_page(p4[p4e]);
	if(first >= PAGE_ENT_MASK + 1) {
		p4[p4e] = PAGE_ALLOCATED_MASK | (attr & PAGE_USER);
		//UnlockPDE(p4,p4e);
		LF(rf);
		kfree(bit_map);
		return 0;
	}
	p4[p4e] &= ~(PAGE_BIT_MAP_MASK | PAGE_FIRST_MASK);
	p4[p4e] |= ((u64)bit_map & PAGE_BIT_MAP_MASK) | (first << PAGE_FIRST_SHIFT);
	UnlockPDE(p4,p4e);
	LF(rf);
	return 0;
}
static int allocate_area_1G(LPTHREAD thread,VADDR vaddr,u32 count,int attr){
	u64 * p4,*p3,*p2,*p1;
	u32 p4e,p3e;
	void * bit_map;
	PAGEE page;
	LPPROCESS process;
	u32 i,j,k;
	u64 first;
	u64 rf;
	
	if(!count) return 0;
	p4e = P4E(vaddr);
	if(p4e >= 256){
		if(p4e > MAX_P4E){
			//error
		}
		else p4 = (&syspdbe) - 256;
	}
	else {
		if(!thread) thread = GetCurThread();
		if(!thread) return -1;
		if(p4e == USER_STACK_P4E_INDEX)
			p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
		else{
			process = thread->father;
			if(!process) return -1;
			p4 = process->pdbe;
		}
	}
	SFI(rf);
	LockPDE(p4,p4e);
	p3e = P3E(vaddr);
	if(attr & PAGE_EXIST){
		if(p4[p4e] & PAGE_EXIST){
			p3 = PADDR2V(p4[p4e]);
			for(i = 0;i < count;i++,p3e++){
				if(p3[p3e] & PAGE_EXIST){
					p2 = PADDR2V(p3[p3e]);
					for(j = 0;j <= PAGE_ENT_MASK;j++){
						if(p2[j] & PAGE_EXIST){
							p1 = PADDR2V(p2[j]);
							for(k = 0;k <= PAGE_ENT_MASK;k++){
								if(p1[k] & PAGE_EXIST || p1[k] & PAGE_ALLOCATED_MASK){
									UnlockPDE(p4,p4e);
									LF(rf);
									return -1;
								}
							}
						}
						else if(p2[j] & PAGE_ALLOCATED_MASK){
							UnlockPDE(p4,p4e);
							LF(rf);
							return -1;
						}
					}
				}
				else if(p3[p3e] & PAGE_ALLOCATED_MASK){
					UnlockPDE(p4,p4e);
					LF(rf);
					return -1;
				}
			}
			p3e = P3E(vaddr);
			while(count){
				if(!(p3[p3e] & PAGE_EXIST)) {
					p3[p3e] = get_free_page(0,attr & PAGE_USER,0);
					memset(PADDR2V(p3[p3e]),0,PAGE_SIZE);
				}
				p2 = PADDR2V(p3[p3e]);
				for(j = 0;j <= PAGE_ENT_MASK;j++){
					if(!(p2[j] & PAGE_EXIST)){
						p2[j] = get_free_page(0,attr & PAGE_USER,0);
						memset(PADDR2V(p2[j]),0,PAGE_SIZE);
					}
					p1 = PADDR2V(p2[j]);
					for(k = 0;k < PAGE_ENT_MASK;k++) p1[k] = get_free_page(0,attr & PAGE_USER,0);
				}
				count--;
				p3e++;
			}
			UnlockPDE(p4,p4e);
			LF(rf);
			return 0;
		}
		if(!(p4[p4e] & PAGE_ALLOCATED_MASK)){
			p3 = PADDR2V(p4[p4e]);
			p3e = P3E(vaddr);
			while(count){
				if(!(p3[p3e] & PAGE_EXIST)) {
					p3[p3e] = get_free_page(0,attr & PAGE_USER,0);
					memset(PADDR2V(p3[p3e]),0,PAGE_SIZE);
				}
				p2 = PADDR2V(p3[p3e]);
				for(j = 0;j <= PAGE_ENT_MASK;j++){
					if(!(p2[j] & PAGE_EXIST)){
						p2[j] = get_free_page(0,attr & PAGE_USER,0);
						memset(PADDR2V(p2[j]),0,PAGE_SIZE);
					}
					p1 = PADDR2V(p2[j]);
					for(k = 0;k < PAGE_ENT_MASK;k++) p1[k] = get_free_page(0,attr & PAGE_USER,0);
				}
				count--;
				p3e++;
			}
			UnlockPDE(p4,p4e);
			LF(rf);
			return 0;
		}
		if(!(p4[p4e] & PAGE_BIT_MAP_MASK)){
			UnlockPDE(p4,p4e);
			LF(rf);
			return -1;
		}
		bit_map = PAGE2MAP(p4[p4e]);
		for(i = 0;i < count;i++,p3e++){
			if(!bt(bit_map,p3e)) continue;
			UnlockPDE(p4,p4e);
			LF(rf);
			return -1;
		}
		p4[p4e] = get_free_page(0,attr & PAGE_USER,0) | PAGE_BUSY_MASK;
		p3 = PADDR2V(p4[p4e]);
		for(i = 0;i <= PAGE_ENT_MASK;i++){
			if(bt(bit_map,i)) p3[i] = PAGE_ALLOCATED_MASK | (attr & PAGE_USER);
			else p3[i] = 0;
		}
		kfree(bit_map);
		p3e = P3E(vaddr);
		while(count){
			p3[p3e] = get_free_page(0,attr & PAGE_USER,0);
			p2 = PADDR2V(p3[p3e]);
			for(j = 0;j <= PAGE_ENT_MASK;j++){
				p2[j] = get_free_page(0,attr & PAGE_USER,0);
				p1 = PADDR2V(p2[j]);
				for(k = 0;k <= PAGE_ENT_MASK;k++) p1[k] = get_free_page(0,attr & PAGE_USER,0);
			}
			count--;
			p3e++;
		}
		UnlockPDE(p4,p4e);
		LF(rf);
		return 0;
	}
	if(p4[p4e] & PAGE_EXIST){
		p3 = PADDR2V(p4[p4e]);
		for(i = 0;i < count;i++,p3e++){
			if(p3[p3e] & PAGE_EXIST){
				p2 = PADDR2V(p3[p3e]);
				for(j = 0;j <= PAGE_ENT_MASK;j++){
					if(p2[j] & PAGE_EXIST){
						p1 = PADDR2V(p2[j]);
						for(k = 0;k <= PAGE_ENT_MASK;k++){
							if(p1[k] & PAGE_EXIST || p1[k] & PAGE_ALLOCATED_MASK){
								UnlockPDE(p4,p4e);
								LF(rf);
								return -1;
							}
						}
						free_page(p2[j]);
						p2[j] = 0;
					}
					else if(p2[j] & PAGE_ALLOCATED_MASK){
						UnlockPDE(p4,p4e);
						LF(rf);
						return -1;
					}
				}
				free_page(p3[p3e]);
				p3[p3e] = 0;
			}
			else if(p3[p3e] & PAGE_ALLOCATED_MASK){
				UnlockPDE(p4,p4e);
				LF(rf);
				return -1;
			}
		}
		p3e = P3E(vaddr);
		while(count){
			p3[p3e] = PAGE_ALLOCATED_MASK | (attr & PAGE_USER);
			count--;
			p3e++;
		}
		UnlockPDE(p4,p4e);
		LF(rf);
		return 0;
	}
	if(!(p4[p4e] & PAGE_ALLOCATED_MASK)){
		bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memset(bit_map,0,(PAGE_ENT_MASK + 1)/8);
		p3e = P3E(vaddr);
		while(count){
			bts(bit_map,p3e);
			p3e++;
			count--;
		}
		p4[p4e] = PAGE_ALLOCATED_MASK | ((u64)bit_map & PAGE_BIT_MAP_MASK) | (attr & (PAGE_WRITE | PAGE_USER));
		//UnlockPDE(p4,p4e);
		LF(rf);
		return 0;
	}
	bit_map = PAGE2MAP(p4[p4e]);
	for(i = 0;i < count;i++,p3e++){
		if(!bt(bit_map,p3e)) continue;
		UnlockPDE(p4,p4e);
		LF(rf);
		return -1;
	}
	p3e = P3E(vaddr);
	first = (p4[p4e] & PAGE_FIRST_MASK) >> PAGE_FIRST_SHIFT;
	if(first > p3e){
		//BUG
		first = 0;
	}
	if(first < p3e){
		UnlockPDE(p4,p4e);
		LF(rf);
		return 0;
	}
	while((first < PAGE_ENT_MASK + 1) && bt(bit_map,first)) first++;
	if(first <= PAGE_ENT_MASK){
		p4[p4e] &= ~PAGE_FIRST_MASK;
		p4[p4e] |= first << PAGE_FIRST_SHIFT;
		UnlockPDE(p4,p4e);
		LF(rf);
		return 0;
	}
	p4[p4e] = PAGE_ALLOCATED_MASK | (attr & PAGE_USER);
	//UnlockPDE(p3,p3e);
	kfree(bit_map);
	UnlockPDE(p4,p4e);
	LF(rf);
	return 0;
}
int allocate_area(LPTHREAD thread,VADDR vaddr,size_t size,int attr){
	//type:r/w,u/s,s/d
	//bit0:	0:Swapable/Unexist;1:Unswapable/Exist
	//bit1:	0:Write;1:Read
	//bit2:	0:Super;1:User
	u64 start,end,first,allocated;
	u64 * p4,*p3,*p2,*p1;
	u32 p4e,p3e,p2e,p1e,i;
	u64 count;
	void * bit_map;
	PAGEE page;
	LPPROCESS process;
	VADDR vaddr_start;
	u64 rf;
	
	if(!size) return 0;
	start = (u64)vaddr & PAGE_PADDR_MASK_4K;
	end = (u64)vaddr + size + PAGE_SIZE - 1;
	end &= PAGE_PADDR_MASK_4K;
	size = end - start;
	allocated = 0;
	vaddr_start = start;

	if(start & (PAGE_SIZE_2M - 1)){//2M对齐前余
		if(size <= PAGE_SIZE_2M - (start & (PAGE_SIZE_2M - 1))) //未跨越对齐界，即所有的地址存在于一个1级页表中
			return allocate_area_4K(thread,(VADDR)start,size >> PAGE_SHIFT_1,attr);
		//跨越对齐界
		count = ((start + PAGE_SIZE_2M - 1) & ~(PAGE_SIZE_2M - 1)) - start;
		count >>= PAGE_SHIFT_1;
		if(allocate_area_4K(thread,(VADDR)start,count,attr)){
			free_area(thread,vaddr_start,allocated);
			return -1;
		}
		size -= ((start + PAGE_SIZE_2M - 1) & ~(PAGE_SIZE_2M - 1)) - start;
		allocated += ((start + PAGE_SIZE_2M - 1) & ~(PAGE_SIZE_2M - 1)) - start;
		start = (start + PAGE_SIZE_2M - 1) & ~(PAGE_SIZE_2M - 1);	
	}
	if(start & (PAGE_SIZE_1G - 1)){//1G对齐前余
		if(size <= PAGE_SIZE_1G - (start & (PAGE_SIZE_1G - 1))){//未跨越对齐界，即所有的地址存在于一个2级页表中
			if(!allocate_area_2M(thread,(VADDR)start,size >> PAGE_SHIFT_2,attr)) return 0;
			free_area(thread,vaddr_start,allocated);
			return -1;
		}
		//跨越对齐界
		count = ((start + PAGE_SIZE_1G - 1) & ~(PAGE_SIZE_1G - 1)) - start;
		count >>= PAGE_SHIFT_2;
		if(allocate_area_2M(thread,(VADDR)start,count,attr)){
			free_area(thread,vaddr_start,allocated);
			return -1;
		}
		size -= ((start + PAGE_SIZE_1G - 1) & ~(PAGE_SIZE_1G - 1)) - start;
		allocated += ((start + PAGE_SIZE_1G - 1) & ~(PAGE_SIZE_1G - 1)) - start;
		start = (start + PAGE_SIZE_1G - 1) & ~(PAGE_SIZE_1G - 1);	
	}
	if(start & (PAGE_SIZE_512G - 1)){//512G对齐前余
		if(size <= PAGE_SIZE_512G - (start & (PAGE_SIZE_512G - 1))){//未跨越对齐界，即所有的地址存在于一个3级页表中
			if(!allocate_area_1G(thread,(VADDR)start,size >> PAGE_SHIFT_3,attr)) return 0;
			free_area(thread,vaddr_start,allocated);
			return -1;
		}
		//跨越对齐界
		count = ((start + PAGE_SIZE_512G - 1) & ~(PAGE_SIZE_512G - 1)) - start;
		count >>= PAGE_SHIFT_3;
		if(allocate_area_1G(thread,(VADDR)start,count,attr)){
			free_area(thread,vaddr_start,allocated);
			return -1;
		}
		size -= ((start + PAGE_SIZE_512G - 1) & ~(PAGE_SIZE_512G - 1)) - start;
		allocated += ((start + PAGE_SIZE_512G - 1) & ~(PAGE_SIZE_512G - 1)) - start;
		start = (start + PAGE_SIZE_512G - 1) & ~(PAGE_SIZE_512G - 1);	
	}
	while(size >= PAGE_SIZE_512G){
		p4e = P4E(vaddr);
		if(p4e >= 256){
			if(p4e > MAX_P4E){
				//error
			}
			else p4 = (&syspdbe) - 256;
		}
		else {
			if(!thread) thread = GetCurThread();
			if(!thread) return -1;
			if(p4e == USER_STACK_P4E_INDEX)
				p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
			else{
				process = thread->father;
				if(!process) return -1;
				p4 = process->pdbe;
			}
		}
		SFI(rf);
		LockPDE(p4,p4e);
		if(attr & PAGE_EXIST){
			if(p4[p4e] & PAGE_EXIST){
				p3 = PADDR2V(p4[p4e]);
				for(p3e = 0;p3e <= PAGE_ENT_MASK;p3e++){
					if(p3[p3e] & PAGE_EXIST){
						p2 = PADDR2V(p3[p3e]);
						for(p2e = 0;p2e <= PAGE_ENT_MASK;p2e++){
							if(p2[p2e] & PAGE_EXIST){
								for(p1e = 0;p1e <= PAGE_ENT_MASK;p1e++){
									if(p1[p1e] & PAGE_EXIST || p1[p1e] & PAGE_ALLOCATED_MASK){
										UnlockPDE(p4,p4e);
										LF(rf);
										free_area(thread,vaddr_start,allocated);
										return -1;
									}
								}
							}
							else if(p2[p2e] & PAGE_ALLOCATED_MASK){
								UnlockPDE(p4,p4e);
								LF(rf);
								free_area(thread,vaddr_start,allocated);
								return -1;
							}
						}
					}
					else if(p3[p3e] & PAGE_ALLOCATED_MASK){
						UnlockPDE(p4,p4e);
						LF(rf);
						free_area(thread,vaddr_start,allocated);
						return -1;
					}
				}
				p3 = PADDR2V(p4[p4e]);
				for(p3e = 0;p3e <= PAGE_ENT_MASK;p3e++){
					if(!(p3[p3e] & PAGE_EXIST)){
						p3[p3e] = get_free_page(0,attr & PAGE_USER,0);
						memset(PADDR2V(p3[p3e]),0,PAGE_SIZE);
					}
					p2 = PADDR2V(p3[p3e]);
					for(p2e = 0;p2e <= PAGE_ENT_MASK;p2e++){
						if(!(p2[p2e] & PAGE_EXIST)){
							p2[p2e] = get_free_page(0,attr & PAGE_USER,0);
							memset(PADDR2V(p2[p2e]),0,PAGE_SIZE);
						}
						p1 = PADDR2V(p2[p2e]);
						for(p1e = 0;p1e <= PAGE_ENT_MASK;p1e++) p1[p1e] = get_free_page(0,attr & PAGE_USER,0);
					}
				}
				UnlockPDE(p4,p4e);
				LF(rf);
				size -= PAGE_SIZE_512G;
				allocated += PAGE_SIZE_512G;
				start = PAGE_SIZE_512G;
				continue;
			}
			if(p4[p4e] & PAGE_ALLOCATED_MASK){
				UnlockPDE(p4,p4e);
				LF(rf);
				free_area(thread,vaddr_start,allocated);
				return -1;
			}
			p3 = PADDR2V(p4[p4e]);
			for(p3e = 0;p3e <= PAGE_ENT_MASK;p3e++){
				if(!(p3[p3e] & PAGE_EXIST)){
					p3[p3e] = get_free_page(0,attr & PAGE_USER,0);
					memset(PADDR2V(p3[p3e]),0,PAGE_SIZE);
				}
				p2 = PADDR2V(p3[p3e]);
				for(p2e = 0;p2e <= PAGE_ENT_MASK;p2e++){
					if(!(p2[p2e] & PAGE_EXIST)){
						p2[p2e] = get_free_page(0,attr & PAGE_USER,0);
						memset(PADDR2V(p2[p2e]),0,PAGE_SIZE);
					}
					p1 = PADDR2V(p2[p2e]);
					for(p1e = 0;p1e <= PAGE_ENT_MASK;p1e++) p1[p1e] = get_free_page(0,attr & PAGE_USER,0);
				}
			}
			UnlockPDE(p4,p4e);
			LF(rf);
			size -= PAGE_SIZE_512G;
			allocated += PAGE_SIZE_512G;
			start = PAGE_SIZE_512G;
			continue;
		}
		if(p4[p4e] & PAGE_EXIST){
			p3 = PADDR2V(p4[p4e]);
			for(p3e = 0;p3e <= PAGE_ENT_MASK;p3e++){
				if(p3[p3e] & PAGE_EXIST){
					p2 = PADDR2V(p3[p3e]);
					for(p2e = 0;p2e <= PAGE_ENT_MASK;p2e++){
						if(p2[p2e] & PAGE_EXIST){
							for(p1e = 0;p1e <= PAGE_ENT_MASK;p1e++){
								if(p1[p1e] & PAGE_EXIST || p1[p1e] & PAGE_ALLOCATED_MASK){
									UnlockPDE(p4,p4e);
									LF(rf);
									free_area(thread,vaddr_start,allocated);
									return -1;
								}
							}
							free_page(p2[p2e]);
							p2[p2e] = 0;
						}
						else if(p2[p2e] & PAGE_ALLOCATED_MASK){
							UnlockPDE(p4,p4e);
							LF(rf);
							free_area(thread,vaddr_start,allocated);
							return -1;
						}
					}
					free_page(p3[p3e]);
					p3[p3e] = 0;
				}
				else if(p3[p3e] & PAGE_ALLOCATED_MASK){
					UnlockPDE(p4,p4e);
					LF(rf);
					free_area(thread,vaddr_start,allocated);
					return -1;
				}
			}
			free_page(p4[p4e]);
			p4[p4e] = PAGE_ALLOCATED_MASK;
			//UnlockPDE(p4,p4e);
			LF(rf);
			size -= PAGE_SIZE_512G;
			allocated += PAGE_SIZE_512G;
			start = PAGE_SIZE_512G;
			continue;
		}
		UnlockPDE(p4,p4e);
		LF(rf);
		free_area(thread,vaddr_start,allocated);
		return -1;
	}
	if(size & (PAGE_ENT_MASK << PAGE_SHIFT_3)){//512G对齐后余
		if(!(size & (PAGE_SIZE_512G - 1))){
			if(!allocate_area_1G(thread,(VADDR)start,size >> PAGE_SHIFT_3,attr)) return 0;
			free_area(thread,vaddr_start,allocated);
			return -1;
		}
		count = size >> PAGE_SHIFT_3;
		if(allocate_area_1G(thread,(VADDR)start,count,attr)){
			free_area(thread,vaddr_start,allocated);
			return -1;
		}
		start += count << PAGE_SHIFT_3;
		allocated += count << PAGE_SHIFT_3;
		size -= count << PAGE_SHIFT_3;
	}
	if(size & (PAGE_ENT_MASK << PAGE_SHIFT_2)){//1G对齐后余
		if(!(size & (PAGE_SIZE_1G - 1))){
			if(!allocate_area_2M(thread,(VADDR)start,size >> PAGE_SHIFT_2,attr)) return 0;
			free_area(thread,vaddr_start,allocated);
			return -1;
		}
		count = size >> PAGE_SHIFT_2;
		if(allocate_area_2M(thread,(VADDR)start,count,attr)){
			free_area(thread,vaddr_start,allocated);
			return -1;
		}
		start += count << PAGE_SHIFT_2;
		allocated += count << PAGE_SHIFT_2;;
		size -= count << PAGE_SHIFT_2;
	}
	if(size & (PAGE_ENT_MASK << PAGE_SHIFT_1)){//2M对齐后余
		if(!allocate_area_4K(thread,(VADDR)start,size >> PAGE_SHIFT_1,attr)) return 0;
		free_area(thread,vaddr_start,allocated);
		return -1;
	}
	return 0;
}
static int free_area_4K(LPTHREAD thread,VADDR vaddr,u32 count){
	u64 first;
	u64 * p4,*p3,*p2,*p1;
	u32 p4e,p3e,p2e,p1e,i;
	void * bit_map = NULL;
	LPPROCESS process;
	int ret = 0;
	u8 t_bit_map[(PAGE_ENT_MASK + 1)/8];
	u64 rf;
	
	if(!count) return 0;
	p4e = P4E(vaddr);
	if(p4e >= 256){
		if(p4e > MAX_P4E){
			//error
		}
		else p4 = (&syspdbe) - 256;
	}
	else {
		if(!thread) thread = GetCurThread();
		if(!thread) return -1;
		if(p4e == USER_STACK_P4E_INDEX) p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
		else{
			if(!(process = thread->father)) return -1;
			p4 = process->pdbe;
		}
	}
	
	SFI(rf); LockPDE(p4,p4e);
	p3e = P3E(vaddr);
	if(!(p4[p4e] & PAGE_EXIST)){
		if(!(p4[p4e] & PAGE_ALLOCATED_MASK)){ 
			UnlockPDE(p4,p4e); 
			LF(rf); 
			return -1; 
		}
		if(p4[p4e] & PAGE_BIT_MAP_MASK){
			bit_map = PAGE2MAP(p4[p4e]);
			if(!bt(bit_map,p3e)) { 
				UnlockPDE(p4,p4e); 
				LF(rf); 
				return -1; 
			}
			p4[p4e] = get_free_page(0,0,0) | PAGE_BUSY_MASK;
			p3 = PADDR2V(p4[p4e]);
			for(i = 0;i <= PAGE_ENT_MASK;i++) p3[i] = bt(bit_map,i) ? PAGE_ALLOCATED_MASK : 0;
		}
		else{
			p4[p4e] = get_free_page(0,0,0) | PAGE_BUSY_MASK;
			p3 = PADDR2V(p4[p4e]);
			for(i = 0;i <= PAGE_ENT_MASK;i++) p3[i] = 0;
		}
	}
	else p3 = PADDR2V(p4[p4e]);
	LockPDE(p3,p3e);
	p2e = P2E(vaddr);
	if(p3[p3e] & PAGE_EXIST) p2 = PADDR2V(p3[p3e]);
	else{
		if(!(p3[p3e] & PAGE_ALLOCATED_MASK)){ 
			UnlockPDE(p4,p4e); 
			UnlockPDE(p3,p3e); 
			LF(rf); 
			return -1; 
		}
		if(p3[p3e] & PAGE_BIT_MAP_MASK){
			bit_map = PAGE2MAP(p3[p3e]);
			if(!bt(bit_map,p2e)) { 
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				LF(rf); 
				return -1; 
			}
			p3[p3e] = get_free_page(0,0,0) | PAGE_BUSY_MASK;
			p2 = PADDR2V(p3[p3e]);
			for(i = 0;i <= PAGE_ENT_MASK;i++) p2[i] = bt(bit_map,i) ? PAGE_ALLOCATED_MASK : 0;
		}
		else{
			p3[p3e] = get_free_page(0,0,0) | PAGE_BUSY_MASK;
			p2 = PADDR2V(p3[p3e]);
			for(i = 0;i <= PAGE_ENT_MASK;i++) p2[i] = 0;
		}
	}
	LockPDE(p2,p2e);
	p1e = P1E(vaddr);
	if(p2[p2e] & PAGE_EXIST){
		p1 = PADDR2V(p2[p2e]);
		while(count--){
			if(free_page(p1[p1e])) ret = -1;
			p1[p1e++] = 0;
		}
		//检查压缩
		memset(t_bit_map,0,(PAGE_ENT_MASK + 1)/8);
		for(p1e = 0;p1e <= PAGE_ENT_MASK;p1e++){
			if(p1[p1e] & PAGE_EXIST){
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				UnlockPDE(p2,p2e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret;
			}
			if(p1[p1e] & PAGE_ALLOCATED_MASK){
				if(p1[p1e] & SWAP_ID_MASK){
					UnlockPDE(p4,p4e); 
					UnlockPDE(p3,p3e); 
					UnlockPDE(p2,p2e); 
					LF(rf); 
					if(bit_map) kfree(bit_map); 
					return ret;
				}
				bts(t_bit_map,p1e);
			}
			else break;
		}
		first = p1e;
		for(;p1e <= PAGE_ENT_MASK;p1e++){
			if(p1[p1e] & PAGE_EXIST){
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				UnlockPDE(p2,p2e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret;
			}
			if(p1[p1e] & PAGE_ALLOCATED_MASK){
				if(p1[p1e] & SWAP_ID_MASK){ 
					UnlockPDE(p4,p4e); 
					UnlockPDE(p3,p3e); 
					UnlockPDE(p2,p2e); 
					LF(rf); 
					if(bit_map) kfree(bit_map); 
					return ret;
				}
				bts(t_bit_map,p1e);
			}
		}
		//可以压缩
		free_page(p2[p2e]);
		if(first){
			//不可以合并
			UnlockPDE(p4,p4e); 
			UnlockPDE(p3,p3e);
			if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
			memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
			p2[p2e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
			//UnlockPDE(p2,p2e);
			LF(rf); return ret;
		}
		//检测是否可以合并
		for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
			if(!t_bit_map[i]) continue;
			//不可以合并
			UnlockPDE(p4,p4e); 
			UnlockPDE(p3,p3e);
			if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
			memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
			p2[p2e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
			//UnlockPDE(p2,p2e);
			LF(rf); return ret;
		}
		//可合并
		p2[p2e] = 0;
		//UnlockPDE(p2,p2e);
		//需向上检查是否可合并
	}
	else if(p2[p2e] & PAGE_ALLOCATED_MASK){
		if(p2[p2e] & PAGE_BIT_MAP_MASK){
			bit_map = PAGE2MAP(p2[p2e]);
			first = (p2[p2e] & PAGE_FIRST_MASK) >> PAGE_FIRST_SHIFT;
			first = first > p1e ? p1e : first;
			while(count--) if(!btr(bit_map,p1e++)) ret = -1;
			if(first){
				//不可以合并
				UnlockPDE(p4,p4e); UnlockPDE(p3,p3e);
				p2[p2e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
				//UnlockPDE(p2,p2e);
				LF(rf); return ret;
			}
			//检测是否可以合并
			for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
				if(!((u8*)bit_map)[i]) continue;
				//不可以合并
				UnlockPDE(p4,p4e); UnlockPDE(p3,p3e);
				p2[p2e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
				//UnlockPDE(p2,p2e);
				LF(rf); return ret;
			}
			//可合并
			p2[p2e] = 0;
			//需向上检查是否可合并
		}
		else{
			UnlockPDE(p4,p4e); UnlockPDE(p3,p3e);
			if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
			memset(bit_map,0xff,(PAGE_ENT_MASK + 1)/8);
			first = p1e;
			while(count--) btr(bit_map,p1e++);
			p2[p2e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;
			//UnlockPDE(p2,p2e);
			LF(rf); return 0;
		}
	}
	else{ 
		UnlockPDE(p2,p2e); 
		UnlockPDE(p3,p3e); 
		UnlockPDE(p4,p4e); 
		LF(rf); 
		if(bit_map) kfree(bit_map); 
		return -1; 
	}
	//向上检查合并
	//p2[p2e] === 0;
	memset(t_bit_map,0,(PAGE_ENT_MASK + 1)/8);
	for(p2e = 0;p2e <= PAGE_ENT_MASK;p2e++){
		if(p2[p2e] & PAGE_EXIST){ 
			UnlockPDE(p4,p4e); 
			UnlockPDE(p3,p3e); 
			LF(rf); 
			if(bit_map) kfree(bit_map); 
			return ret;
		}
		if(p2[p2e] & PAGE_ALLOCATED_MASK){
			if(p2[p2e] & SWAP_ID_MASK){ 
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret; 
			}
			bts(t_bit_map,p2e);
		}
		else break;
	}
	first = p2e;
	for(;p2e <= PAGE_ENT_MASK;p2e++){
		if(p2[p2e] & PAGE_EXIST){ 
			UnlockPDE(p4,p4e); 
			UnlockPDE(p3,p3e); 
			LF(rf); 
			if(bit_map) kfree(bit_map); 
			return ret; 
		}
		if(p2[p2e] & PAGE_ALLOCATED_MASK){
			if(p2[p2e] & SWAP_ID_MASK){ 
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret; 
			}
			bts(t_bit_map,p2e);
		}
	}
	//可以压缩
	free_page(p3[p3e]);
	if(first){
		//不可以合并
		UnlockPDE(p4,p4e);
		if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
		p3[p3e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
		//UnlockPDE(p3,p3e);
		LF(rf);
		return ret;
	}
	//检测是否可以合并
	for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
		if(!t_bit_map[i]) continue;
		//不可以合并
		UnlockPDE(p4,p4e);
		if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
		p3[p3e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
		//UnlockPDE(p3,p3e);
		LF(rf);
		return ret;
	}
	//可合并
	p3[p3e] = 0;
	//UnlockPDE(p3,p3e);
	//需向上检查是否可合并
	//p3[p3e] === 0;
	memset(t_bit_map,0,(PAGE_ENT_MASK + 1)/8);
	for(p3e = 0;p3e <= PAGE_ENT_MASK;p3e++){
		if(p3[p3e] & PAGE_EXIST){ 
			UnlockPDE(p4,p4e); 
			LF(rf); 
			if(bit_map) kfree(bit_map); 
			return ret;
		}
		if(p3[p3e] & PAGE_ALLOCATED_MASK){
			if(p3[p3e] & SWAP_ID_MASK){ 
				UnlockPDE(p4,p4e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret; 
			}
			bts(t_bit_map,p3e);
		}
		else break;
	}
	first = p3e;
	for(;p3e <= PAGE_ENT_MASK;p3e++){
		if(p3[p3e] & PAGE_EXIST){ 
			UnlockPDE(p4,p4e); 
			LF(rf); 
			if(bit_map) kfree(bit_map); 
			return ret; 
		}
		if(p3[p3e] & PAGE_ALLOCATED_MASK){
			if(p3[p3e] & SWAP_ID_MASK){ 
				UnlockPDE(p4,p4e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret; 
			}
			bts(t_bit_map,p3e);
		}
	}
	//可以压缩
	free_page(p4[p4e]);
	if(first){
		//不可以合并
		if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
		p4[p4e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
		//UnlockPDE(p3,p3e);
		LF(rf);
		return ret;
	}
	//检测是否可以合并
	for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
		if(!t_bit_map[i]) continue;
		//不可以合并
		if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
		p4[p4e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
		//UnlockPDE(p3,p3e);
		LF(rf);
		return ret;
	}
	//可合并
	p4[p4e] = 0;
	//UnlockPDE(p4,p4e);
	LF(rf);
	if(bit_map) kfree(bit_map);
	return ret;
}
static int free_area_2M(LPTHREAD thread,VADDR vaddr,u32 count){
	u64 first;
	u64 * p4,*p3,*p2,*p1;
	u32 p4e,p3e,p2e,p1e,i;
	void * bit_map = NULL;
	LPPROCESS process;
	int ret = 0;
	u8 t_bit_map[(PAGE_ENT_MASK + 1)/8];
	u64 rf;
	
	if(!count) return 0;
	p4e = P4E(vaddr);
	if(p4e >= 256){
		if(p4e > MAX_P4E){
			//error
		}
		else p4 = (&syspdbe) - 256;
	}
	else {
		if(!thread) thread = GetCurThread();
		if(!thread) return -1;
		if(p4e == USER_STACK_P4E_INDEX) p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
		else{
			if(!(process = thread->father)) return -1;
			p4 = process->pdbe;
		}
	}
	
	SFI(rf);
	LockPDE(p4,p4e);
	p3e = P3E(vaddr);
	if(p4[p4e] & PAGE_EXIST) p3 = PADDR2V(p4[p4e]);
	else if(p4[p4e] & PAGE_ALLOCATED_MASK){
		if(p4[p4e] & PAGE_BIT_MAP_MASK){
			bit_map = PAGE2MAP(p4[p4e]);
			if(!bt(bit_map,p3e)){
				UnlockPDE(p4,p4e);
				LF(rf);
				return -1;
			}
			p4[p4e] = get_free_page(0,0,0) | PAGE_BUSY_MASK;
			p3 = PADDR2V(p4[p4e]);
			for(i = 0;i <= PAGE_ENT_MASK;i++) p3[i] = bt(bit_map,i) ? PAGE_ALLOCATED_MASK : 0;
		}
		else{
			p4[p4e] = get_free_page(0,0,0) | PAGE_BUSY_MASK;
			p3 = PADDR2V(p4[p4e]);
			for(i = 0;i <= PAGE_ENT_MASK;i++) p3[i] = PAGE_ALLOCATED_MASK;
		}
	}
	LockPDE(p3,p3e);
	p2e = P2E(vaddr);
	if(p3[p3e] & PAGE_EXIST){
		p2 = PADDR2V(p3[p3e]);
		while(count--){
			if(p2[p2e] & PAGE_EXIST) {
				p1 = PADDR2V(p2[p2e]);
				for(p1e = 0;p1e <= PAGE_ENT_MASK;p1e++) if(free_page(p1[p1e])) ret = -1;
				free_page(p2[p2e]);
			}
			else if(p2[p2e] & PAGE_ALLOCATED_MASK){
				if(p2[p2e] & PAGE_BIT_MAP_MASK){
					if(bit_map) kfree(PAGE2MAP(p2[p2e]));
					else bit_map = PAGE2MAP(p2[p2e]);
					ret = -1;
				}
			}
			else ret = -1;
			p2[p2e] = 0;
		}
		//检查压缩
		memset(t_bit_map,0,(PAGE_ENT_MASK + 1)/8);
		for(p2e = 0;p2e <= PAGE_ENT_MASK;p2e++){
			if(p2[p2e] & PAGE_EXIST){
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret;
			}
			if(p2[p2e] & PAGE_ALLOCATED_MASK){
				if(p2[p2e] & SWAP_ID_MASK){
					UnlockPDE(p4,p4e); 
					UnlockPDE(p3,p3e); 
					LF(rf); 
					if(bit_map) kfree(bit_map); 
					return ret;
				}
				bts(t_bit_map,p2e);
			}
			else break;
		}
		first = p2e;
		for(;p2e <= PAGE_ENT_MASK;p2e++){
			if(p2[p2e] & PAGE_EXIST){
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret;
			}
			if(p2[p2e] & PAGE_ALLOCATED_MASK){
				if(p2[p2e] & SWAP_ID_MASK){ 
					UnlockPDE(p4,p4e); 
					UnlockPDE(p3,p3e); 
					LF(rf); 
					if(bit_map) kfree(bit_map); 
					return ret;
				}
				bts(t_bit_map,p2e);
			}
		}
		//可以压缩
		free_page(p3[p3e]);
		if(first){
			//不可以合并
			UnlockPDE(p4,p4e); 
			if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
			memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
			p3[p3e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
			//UnlockPDE(p3,p3e);
			LF(rf); return ret;
		}
		//检测是否可以合并
		for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
			if(!t_bit_map[i]) continue;
			//不可以合并
			UnlockPDE(p4,p4e); 
			if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
			memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
			p3[p3e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
			//UnlockPDE(p3,p3e);
			LF(rf); return ret;
		}
		//可合并
		p3[p3e] = 0;
		//UnlockPDE(p3,p3e);
		//需向上检查是否可合并
	}
	else if(p3[p3e] & PAGE_ALLOCATED_MASK){
		if(p3[p3e] & PAGE_BIT_MAP_MASK){
			bit_map = PAGE2MAP(p3[p3e]);
			first = (p3[p3e] & PAGE_FIRST_MASK) >> PAGE_FIRST_SHIFT;
			first = first > p2e ? p2e : first;
			while(count--) if(!btr(bit_map,p2e++)) ret = -1;
			if(first){
				//不可以合并
				UnlockPDE(p4,p4e);
				p3[p3e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
				//UnlockPDE(p3,p3e);
				LF(rf); return ret;
			}
			//检测是否可以合并
			for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
				if(!((u8*)bit_map)[i]) continue;
				//不可以合并
				UnlockPDE(p4,p4e);
				p3[p3e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
				//UnlockPDE(p3,p3e);
				LF(rf); return ret;
			}
			//可合并
			p3[p3e] = 0;
			//需向上检查是否可合并
		}
		else{
			UnlockPDE(p4,p4e);
			if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
			memset(bit_map,0xff,(PAGE_ENT_MASK + 1)/8);
			first = p2e;
			while(count--) btr(bit_map,p2e++);
			p3[p3e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;
			//UnlockPDE(p3,p3e);
			LF(rf); return 0;
		}
	}
	else{
		UnlockPDE(p3,p3e); 
		UnlockPDE(p4,p4e); 
		LF(rf); 
		if(bit_map) kfree(bit_map); 
		return -1; 
	}
	//p3[p3e] === 0;
	memset(t_bit_map,0,(PAGE_ENT_MASK + 1)/8);
	for(p3e = 0;p3e <= PAGE_ENT_MASK;p3e++){
		if(p3[p3e] & PAGE_EXIST){ 
			UnlockPDE(p4,p4e); 
			LF(rf); 
			if(bit_map) kfree(bit_map); 
			return ret;
		}
		if(p3[p3e] & PAGE_ALLOCATED_MASK){
			if(p3[p3e] & SWAP_ID_MASK){ 
				UnlockPDE(p4,p4e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret; 
			}
			bts(t_bit_map,p3e);
		}
		else break;
	}
	first = p3e;
	for(;p3e <= PAGE_ENT_MASK;p3e++){
		if(p3[p3e] & PAGE_EXIST){ 
			UnlockPDE(p4,p4e); 
			LF(rf); 
			if(bit_map) kfree(bit_map); 
			return ret; 
		}
		if(p3[p3e] & PAGE_ALLOCATED_MASK){
			if(p3[p3e] & SWAP_ID_MASK){ 
				UnlockPDE(p4,p4e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret; 
			}
			bts(t_bit_map,p3e);
		}
	}
	//可以压缩
	free_page(p4[p4e]);
	if(first){
		//不可以合并
		if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
		p4[p4e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
		//UnlockPDE(p3,p3e);
		LF(rf);
		return ret;
	}
	//检测是否可以合并
	for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
		if(!t_bit_map[i]) continue;
		//不可以合并
		if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
		p4[p4e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
		//UnlockPDE(p3,p3e);
		LF(rf);
		return ret;
	}
	//可合并
	p4[p4e] = 0;
	//UnlockPDE(p4,p4e);
	LF(rf);
	if(bit_map) kfree(bit_map);
	return ret;
}
static int free_area_1G(LPTHREAD thread,VADDR vaddr,u32 count){
	u64 first;
	u64 * p4,*p3,*p2,*p1;
	u32 p4e,p3e,p2e,p1e,i;
	void * bit_map = NULL;
	LPPROCESS process;
	int ret = 0;
	u8 t_bit_map[(PAGE_ENT_MASK + 1)/8];
	u64 rf;
	
	if(!count) return 0;
	p4e = P4E(vaddr);
	if(p4e >= 256){
		if(p4e > MAX_P4E){
			//error
		}
		else p4 = (&syspdbe) - 256;
	}
	else {
		if(!thread) thread = GetCurThread();
		if(!thread) return -1;
		if(p4e == USER_STACK_P4E_INDEX) p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
		else{
			if(!(process = thread->father)) return -1;
			p4 = process->pdbe;
		}
	}
	
	SFI(rf);
	LockPDE(p4,p4e);  
	p3e = P3E(vaddr);
	if(p4[p4e] & PAGE_EXIST){
		p3 = PADDR2V(p4[p4e]);
		while(count--){
			if(p3[p3e] & PAGE_EXIST){
				p2 = PADDR2V(p3[p3e]);
				for(p2e = 0;p2e <= PAGE_ENT_MASK;p2e++){
					if(p2[p2e] & PAGE_EXIST){
						p1 = PADDR2V(p2[p2e]);
						for(p1e = 0;p1e <= PAGE_ENT_MASK;p1e++) if(free_page(p1[p1e])) ret = -1;
						free_page(p2[p2e]);
					}
					else if(p2[p2e] & PAGE_ALLOCATED_MASK){
						if(p2[p2e] & PAGE_BIT_MAP_MASK) {
							if(bit_map) kfree(PAGE2MAP(p2[p2e]));
							else bit_map = PAGE2MAP(p2[p2e]);
						}
					}
					else ret = -1;
				}
				free_page(p3[p3e]);
			}
			else if(p3[p3e] & PAGE_ALLOCATED_MASK){
				if(p3[p3e] & PAGE_BIT_MAP_MASK) {
					if(bit_map) kfree(PAGE2MAP(p3[p3e]));
					else bit_map = PAGE2MAP(p3[p3e]);
				}
			}
			else ret = -1;
			p3[p3e] = 0;
		}
		
		//检查压缩
		memset(t_bit_map,0,(PAGE_ENT_MASK + 1)/8);
		for(p3e = 0;p3e <= PAGE_ENT_MASK;p3e++){
			if(p3[p3e] & PAGE_EXIST){
				UnlockPDE(p4,p4e);  
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret;
			}
			if(p3[p3e] & PAGE_ALLOCATED_MASK){
				if(p3[p3e] & SWAP_ID_MASK){
					UnlockPDE(p4,p4e); 
					LF(rf); 
					if(bit_map) kfree(bit_map); 
					return ret;
				}
				bts(t_bit_map,p3e);
			}
			else break;
		}
		first = p3e;
		for(;p3e <= PAGE_ENT_MASK;p3e++){
			if(p3[p3e] & PAGE_EXIST){
				UnlockPDE(p4,p4e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret;
			}
			if(p3[p3e] & PAGE_ALLOCATED_MASK){
				if(p3[p3e] & SWAP_ID_MASK){ 
					UnlockPDE(p4,p4e); 
					LF(rf); 
					if(bit_map) kfree(bit_map); 
					return ret;
				}
				bts(t_bit_map,p3e);
			}
		}
		//可以压缩
		free_page(p4[p4e]);
		if(first){
			//不可以合并
			if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
			memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
			p4[p4e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
			//UnlockPDE(p4,p4e);
			LF(rf); return ret;
		}
		//检测是否可以合并
		for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
			if(!t_bit_map[i]) continue;
			//不可以合并 
			if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
			memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
			p4[p4e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
			//UnlockPDE(p4,p4e);
			LF(rf); return ret;
		}
		//可合并
		p4[p4e] = 0;
		//UnlockPDE(p4,p4e);
		LF(rf);
		return ret;
	}
	else if(p4[p4e] & PAGE_ALLOCATED_MASK){
		if(p4[p4e] & PAGE_BIT_MAP_MASK){
			bit_map = PAGE2MAP(p4[p4e]);
			first = (p4[p4e] & PAGE_FIRST_MASK) >> PAGE_FIRST_SHIFT;
			first = first > p3e ? p3e : first;
			while(count--) if(!btr(bit_map,p3e++)) ret = -1;
			if(first){
				//不可以合并
				p4[p4e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
				//UnlockPDE(p4,p4e);
				LF(rf); return ret;
			}
			//检测是否可以合并
			for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
				if(!((u8*)bit_map)[i]) continue;
				//不可以合并
				p4[p4e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
				//UnlockPDE(p4,p4e);
				LF(rf); return ret;
			}
			//可合并
			p4[p4e] = 0;
			//UnlockPDE(p4,p4e);
			LF(rf);
			return ret;
		}
		else{
			if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
			memset(bit_map,0xff,(PAGE_ENT_MASK + 1)/8);
			first = p3e;
			while(count--) btr(bit_map,p3e++);
			p4[p4e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;
			//UnlockPDE(p4,p4e);
			LF(rf); return 0;
		}
	}
	else{
		UnlockPDE(p4,p4e); 
		LF(rf); 
		return -1; 
	}
}
int free_area(LPTHREAD thread,VADDR vaddr,size_t size){
	u64 start,end;
	u64 * p4,*p3,*p2,*p1;
	u32 p4e,p3e,p2e,p1e;
	u64 count;
	LPPROCESS process;
	int ret = 0;
	u64 rf;
	
	if(!size) return 0;
	start = (u64)vaddr & PAGE_PADDR_MASK_4K;
	end = (u64)vaddr + size + PAGE_SIZE - 1;
	end &= PAGE_PADDR_MASK_4K;
	size = end - start;
	
	if(start & (PAGE_SIZE_2M - 1)){//2M对齐前余
		if(size <= PAGE_SIZE_2M - (start & (PAGE_SIZE_2M - 1))) //未跨越对齐界，即所有的地址存在于一个1级页表中
			return free_area_4K(thread,(VADDR)start,size >> PAGE_SHIFT_1);
		//跨越对齐界
		count = ((start + PAGE_SIZE_2M - 1) & ~(PAGE_SIZE_2M - 1)) - start;
		count >>= PAGE_SHIFT_1;
		ret = free_area_4K(thread,(VADDR)start,count);
		size -= ((start + PAGE_SIZE_2M - 1) & ~(PAGE_SIZE_2M - 1)) - start;
		start = (start + PAGE_SIZE_2M - 1) & ~(PAGE_SIZE_2M - 1);	
	}
	if(start & (PAGE_SIZE_1G - 1)){//1G对齐前余
		if(size <= PAGE_SIZE_1G - (start & (PAGE_SIZE_1G - 1)))//未跨越对齐界，即所有的地址存在于一个2级页表中
			return free_area_2M(thread,(VADDR)start,size >> PAGE_SHIFT_2) ? -1 : ret;
		//跨越对齐界
		count = ((start + PAGE_SIZE_1G - 1) & ~(PAGE_SIZE_1G - 1)) - start;
		count >>= PAGE_SHIFT_2;
		ret = free_area_2M(thread,(VADDR)start,count) ? -1 : ret;
		size -= ((start + PAGE_SIZE_1G - 1) & ~(PAGE_SIZE_1G - 1)) - start;
		start = (start + PAGE_SIZE_1G - 1) & ~(PAGE_SIZE_1G - 1);	
	}
	if(start & (PAGE_SIZE_512G - 1)){//512G对齐前余
		if(size <= PAGE_SIZE_512G - (start & (PAGE_SIZE_512G - 1)))//未跨越对齐界，即所有的地址存在于一个3级页表中
			return free_area_1G(thread,(VADDR)start,size >> PAGE_SHIFT_3) ? -1 : ret;
		//跨越对齐界
		count = ((start + PAGE_SIZE_512G - 1) & ~(PAGE_SIZE_512G - 1)) - start;
		count >>= PAGE_SHIFT_3;
		ret = free_area_1G(thread,(VADDR)start,count) ? -1 : ret;
		size -= ((start + PAGE_SIZE_512G - 1) & ~(PAGE_SIZE_512G - 1)) - start;
		start = (start + PAGE_SIZE_512G - 1) & ~(PAGE_SIZE_512G - 1);	
	}
	while(size >= PAGE_SIZE_512G){
		p4e = P4E(vaddr);
		if(p4e >= 256){
			if(p4e > MAX_P4E){
				//error
			}
			else p4 = (&syspdbe) - 256;
		}
		else {
			if(!thread) thread = GetCurThread();
			if(!thread) return -1;
			if(p4e == USER_STACK_P4E_INDEX)
				p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
			else{
				if(!(process = thread->father)) return -1;
				p4 = process->pdbe;
			}
		}
		SFI(rf);
		LockPDE(p4,p4e);
		if(p4[p4e] & PAGE_EXIST){
			p3 = PADDR2V(p4[p4e]);
			for(p3e = 0;p3e <= PAGE_ENT_MASK;p3e++){
				if(p3[p3e] & PAGE_EXIST){
					p2 = PADDR2V(p3[p3e]);
					for(p2e = 0;p2e <= PAGE_ENT_MASK;p2e++){
						if(p2[p2e] & PAGE_EXIST){
							for(p1e = 0;p1e <= PAGE_ENT_MASK;p1e++) ret = free_page(p1[p1e]) ? -1 : ret;
							free_page(p2[p2e]);
						}
						else if(p2[p2e] & PAGE_ALLOCATED_MASK){
							if(p2[p2e] & PAGE_BIT_MAP_MASK) kfree(PAGE2MAP(p2[p2e]));
						}
						else ret = -1;
					}
					free_page(p3[p3e]);
				}
				else if(p3[p3e] & PAGE_ALLOCATED_MASK){
					if(p3[p3e] & PAGE_BIT_MAP_MASK) kfree(PAGE2MAP(p3[p3e]));
				}
				else ret = -1;
			}
			free_page(p4[p4e]);
		}
		else if(p4[p4e] & PAGE_ALLOCATED_MASK){
			if(p4[p4e] & PAGE_BIT_MAP_MASK) kfree(PAGE2MAP(p4[p4e]));
		}
		else ret = -1;
		p4[p4e] = 0;
		//UnlockPDE(p4,p4e);
		LF(rf);
		size -= PAGE_SIZE_512G;
		start = PAGE_SIZE_512G;
	}
	if(size & (PAGE_ENT_MASK << PAGE_SHIFT_3)){//512G对齐后余
		if(!(size & (PAGE_SIZE_512G - 1)))
			return free_area_1G(thread,(VADDR)start,size >> PAGE_SHIFT_3) ? -1 : ret;
		count = size >> PAGE_SHIFT_3;
		ret = free_area_1G(thread,(VADDR)start,count) ? -1 : ret;
		start += count << PAGE_SHIFT_3;
		size -= count << PAGE_SHIFT_3;
	}
	if(size & (PAGE_ENT_MASK << PAGE_SHIFT_2)){//1G对齐后余
		if(!(size & (PAGE_SIZE_1G - 1)))
			return free_area_2M(thread,(VADDR)start,size >> PAGE_SHIFT_2) ? -1 : ret;
		count = size >> PAGE_SHIFT_2;
		ret = free_area_2M(thread,(VADDR)start,count) ? -1 : ret;
		start += count << PAGE_SHIFT_2;
		size -= count << PAGE_SHIFT_2;
	}
	if(size & (PAGE_ENT_MASK << PAGE_SHIFT_1))//2M对齐后余
		return free_area_4K(thread,(VADDR)start,size >> PAGE_SHIFT_1) ? -1 : ret;
	//printk("free_area:%P,%P,%P.\n",thread,vaddr,size);
	//stop();
	return ret;
}
int free_vaddr(LPTHREAD thread,VADDR vaddr){
	u64 first;
	u64 * p4,*p3,*p2,*p1;
	u32 p4e,p3e,p2e,p1e,i;
	void * bit_map = NULL;
	LPPROCESS process;
	int ret = 0;
	u8 t_bit_map[(PAGE_ENT_MASK + 1)/8];
	u64 rf;
	
	p4e = P4E(vaddr);
	if(p4e >= 256){
		if(p4e > MAX_P4E){
			//error
		}
		else p4 = (&syspdbe) - 256;
	}
	else {
		if(!thread) thread = GetCurThread();
		if(!thread) return -1;
		if(p4e == USER_STACK_P4E_INDEX) p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
		else{
			if(!(process = thread->father)) return -1;
			p4 = process->pdbe;
		}
	}
	SFI(rf);
	LockPDE(p4,p4e);
	p3e = P3E(vaddr);
	if(!(p4[p4e] & PAGE_EXIST)){
		if(!(p4[p4e] & PAGE_ALLOCATED_MASK)){ 
			UnlockPDE(p4,p4e); 
			LF(rf); 
			return -1; 
		}
		if(p4[p4e] & PAGE_BIT_MAP_MASK){
			bit_map = PAGE2MAP(p4[p4e]);
			if(!bt(bit_map,p3e)) { 
				UnlockPDE(p4,p4e); 
				LF(rf); 
				return -1; 
			}
			p4[p4e] = get_free_page(0,0,0) | PAGE_BUSY_MASK;
			p3 = PADDR2V(p4[p4e]);
			for(i = 0;i <= PAGE_ENT_MASK;i++) p3[i] = bt(bit_map,i) ? PAGE_ALLOCATED_MASK : 0;
		}
		else{
			p4[p4e] = get_free_page(0,0,0) | PAGE_BUSY_MASK;
			p3 = PADDR2V(p4[p4e]);
			for(i = 0;i <= PAGE_ENT_MASK;i++) p3[i] = 0;
		}
	}
	else p3 = PADDR2V(p4[p4e]);
	LockPDE(p3,p3e);
	p2e = P2E(vaddr);
	if(p3[p3e] & PAGE_EXIST) p2 = PADDR2V(p3[p3e]);
	else{
		if(!(p3[p3e] & PAGE_ALLOCATED_MASK)){ 
			UnlockPDE(p4,p4e); 
			UnlockPDE(p3,p3e); 
			LF(rf); 
			return -1; 
		}
		if(p3[p3e] & PAGE_BIT_MAP_MASK){
			bit_map = PAGE2MAP(p3[p3e]);
			if(!bt(bit_map,p2e)) { 
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				LF(rf); 
				return -1; 
			}
			p3[p3e] = get_free_page(0,0,0) | PAGE_BUSY_MASK;
			p2 = PADDR2V(p3[p3e]);
			for(i = 0;i <= PAGE_ENT_MASK;i++) p2[i] = bt(bit_map,i) ? PAGE_ALLOCATED_MASK : 0;
		}
		else{
			p3[p3e] = get_free_page(0,0,0) | PAGE_BUSY_MASK;
			p2 = PADDR2V(p3[p3e]);
			for(i = 0;i <= PAGE_ENT_MASK;i++) p2[i] = 0;
		}
	}
	LockPDE(p2,p2e);
	p1e = P1E(vaddr);
	if(p2[p2e] & PAGE_EXIST){
		p1 = PADDR2V(p2[p2e]);
		if(free_page(p1[p1e])) {
			UnlockPDE(p4,p4e); 
			UnlockPDE(p3,p3e); 
			UnlockPDE(p2,p2e); 
			LF(rf); 
			if(bit_map) kfree(bit_map); 
			return -1;
		}
		/****************************************
		刷新TLB
		
		
		
		****************************************/
		//检查压缩
		memset(t_bit_map,0,(PAGE_ENT_MASK + 1)/8);
		for(p1e = 0;p1e <= PAGE_ENT_MASK;p1e++){
			if(p1[p1e] & PAGE_EXIST){
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				UnlockPDE(p2,p2e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return 0;
			}
			if(p1[p1e] & PAGE_ALLOCATED_MASK){
				if(p1[p1e] & SWAP_ID_MASK){
					UnlockPDE(p4,p4e); 
					UnlockPDE(p3,p3e); 
					UnlockPDE(p2,p2e); 
					LF(rf); 
					if(bit_map) kfree(bit_map); 
					return 0;
				}
				bts(t_bit_map,p1e);
			}
			else break;
		}
		first = p1e;
		for(;p1e <= PAGE_ENT_MASK;p1e++){
			if(p1[p1e] & PAGE_EXIST){
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				UnlockPDE(p2,p2e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return 0;
			}
			if(p1[p1e] & PAGE_ALLOCATED_MASK){
				if(p1[p1e] & SWAP_ID_MASK){ 
					UnlockPDE(p4,p4e); 
					UnlockPDE(p3,p3e); 
					UnlockPDE(p2,p2e); 
					LF(rf); 
					if(bit_map) kfree(bit_map); 
					return 0;
				}
				bts(t_bit_map,p1e);
			}
		}
		//可以压缩
		free_page(p2[p2e]);
		if(first){
			//不可以合并
			UnlockPDE(p4,p4e); 
			UnlockPDE(p3,p3e);
			if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
			memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
			p2[p2e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
			//UnlockPDE(p2,p2e);
			LF(rf); return 0;
		}
		//检测是否可以合并
		for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
			if(!t_bit_map[i]) continue;
			//不可以合并
			UnlockPDE(p4,p4e); 
			UnlockPDE(p3,p3e);
			if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
			memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
			p2[p2e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
			//UnlockPDE(p2,p2e);
			LF(rf); return 0;
		}
		//可合并
		p2[p2e] = 0;
		//UnlockPDE(p2,p2e);
		//需向上检查是否可合并
	}
	else if(p2[p2e] & PAGE_ALLOCATED_MASK){
		if(p2[p2e] & PAGE_BIT_MAP_MASK){
			bit_map = PAGE2MAP(p2[p2e]);
			first = (p2[p2e] & PAGE_FIRST_MASK) >> PAGE_FIRST_SHIFT;
			first = first > p1e ? p1e : first;
			if(!btr(bit_map,p1e++)) {
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				UnlockPDE(p2,p2e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return -1;
			}
			if(first){
				//不可以合并
				UnlockPDE(p4,p4e); UnlockPDE(p3,p3e);
				p2[p2e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
				//UnlockPDE(p2,p2e);
				LF(rf); return ret;
			}
			//检测是否可以合并
			for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
				if(!((u8*)bit_map)[i]) continue;
				//不可以合并
				UnlockPDE(p4,p4e); UnlockPDE(p3,p3e);
				p2[p2e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
				//UnlockPDE(p2,p2e);
				LF(rf); return ret;
			}
			//可合并
			p2[p2e] = 0;
			//需向上检查是否可合并
		}
		else{
			UnlockPDE(p4,p4e); UnlockPDE(p3,p3e);
			if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
			memset(bit_map,0xff,(PAGE_ENT_MASK + 1)/8);
			first = p1e;
			btr(bit_map,p1e);
			p2[p2e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;
			//UnlockPDE(p2,p2e);
			LF(rf); return 0;
		}
	}
	else{ 
		UnlockPDE(p2,p2e); 
		UnlockPDE(p3,p3e); 
		UnlockPDE(p4,p4e); 
		LF(rf); 
		if(bit_map) kfree(bit_map); 
		return -1; 
	}
	//向上检查合并
	//p2[p2e] === 0;
	memset(t_bit_map,0,(PAGE_ENT_MASK + 1)/8);
	for(p2e = 0;p2e <= PAGE_ENT_MASK;p2e++){
		if(p2[p2e] & PAGE_EXIST){ 
			UnlockPDE(p4,p4e); 
			UnlockPDE(p3,p3e); 
			LF(rf); 
			if(bit_map) kfree(bit_map); 
			return ret;
		}
		if(p2[p2e] & PAGE_ALLOCATED_MASK){
			if(p2[p2e] & SWAP_ID_MASK){ 
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret; 
			}
			bts(t_bit_map,p2e);
		}
		else break;
	}
	first = p2e;
	for(;p2e <= PAGE_ENT_MASK;p2e++){
		if(p2[p2e] & PAGE_EXIST){ 
			UnlockPDE(p4,p4e); 
			UnlockPDE(p3,p3e); 
			LF(rf); 
			if(bit_map) kfree(bit_map); 
			return ret; 
		}
		if(p2[p2e] & PAGE_ALLOCATED_MASK){
			if(p2[p2e] & SWAP_ID_MASK){ 
				UnlockPDE(p4,p4e); 
				UnlockPDE(p3,p3e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret; 
			}
			bts(t_bit_map,p2e);
		}
	}
	//可以压缩
	free_page(p3[p3e]);
	if(first){
		//不可以合并
		UnlockPDE(p4,p4e);
		if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
		p3[p3e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
		//UnlockPDE(p3,p3e);
		LF(rf);
		return ret;
	}
	//检测是否可以合并
	for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
		if(!t_bit_map[i]) continue;
		//不可以合并
		UnlockPDE(p4,p4e);
		if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
		p3[p3e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
		//UnlockPDE(p3,p3e);
		LF(rf);
		return ret;
	}
	//可合并
	p3[p3e] = 0;
	//UnlockPDE(p3,p3e);
	//需向上检查是否可合并
	//p3[p3e] === 0;
	memset(t_bit_map,0,(PAGE_ENT_MASK + 1)/8);
	for(p3e = 0;p3e <= PAGE_ENT_MASK;p3e++){
		if(p3[p3e] & PAGE_EXIST){ 
			UnlockPDE(p4,p4e); 
			LF(rf); 
			if(bit_map) kfree(bit_map); 
			return ret;
		}
		if(p3[p3e] & PAGE_ALLOCATED_MASK){
			if(p3[p3e] & SWAP_ID_MASK){ 
				UnlockPDE(p4,p4e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret; 
			}
			bts(t_bit_map,p3e);
		}
		else break;
	}
	first = p3e;
	for(;p3e <= PAGE_ENT_MASK;p3e++){
		if(p3[p3e] & PAGE_EXIST){ 
			UnlockPDE(p4,p4e); 
			LF(rf); 
			if(bit_map) kfree(bit_map); 
			return ret; 
		}
		if(p3[p3e] & PAGE_ALLOCATED_MASK){
			if(p3[p3e] & SWAP_ID_MASK){ 
				UnlockPDE(p4,p4e); 
				LF(rf); 
				if(bit_map) kfree(bit_map); 
				return ret; 
			}
			bts(t_bit_map,p3e);
		}
	}
	//可以压缩
	free_page(p4[p4e]);
	if(first){
		//不可以合并
		if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
		p4[p4e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
		//UnlockPDE(p3,p3e);
		LF(rf);
		return ret;
	}
	//检测是否可以合并
	for(i = 0;i < (PAGE_ENT_MASK + 1)/8;i++){
		if(!t_bit_map[i]) continue;
		//不可以合并
		if(!bit_map) bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memcpy(bit_map,t_bit_map,(PAGE_ENT_MASK + 1)/8);
		p4[p4e] = (first << PAGE_FIRST_SHIFT) | ((u64)bit_map & PAGE_BIT_MAP_MASK) | PAGE_ALLOCATED_MASK;	
		//UnlockPDE(p3,p3e);
		LF(rf);
		return ret;
	}
	//可合并
	p4[p4e] = 0;
	//UnlockPDE(p4,p4e);
	LF(rf);
	if(bit_map) kfree(bit_map);
	return ret;
}
//测试地址是否有效
int addr_vaild(LPTHREAD thread,VADDR addr){
	u64 * p4,* p3,* p2,* p1;
	u32 p4e,p3e,p2e,p1e;
	LPPROCESS process = NULL;
	u64 rf;

	p4e = P4E(addr);
	if(p4e >= 256){
		if(p4e > MAX_P4E){
			//error
		}
		else p4 = (&syspdbe) - 256;
	}
	else {
		if(!thread) thread = GetCurThread();
		if(!thread) return -1;
		if(p4e == USER_STACK_P4E_INDEX)
			p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
		else{
			process = thread->father;
			if(!process) return -1;
			p4 = process->pdbe;
		}
	}
	SFI(rf);
	LockPDE(p4,p4e);
	p3e = P3E(addr);
	if(!(p4[p4e] & PAGE_EXIST)){
		if(pg_ext_test(p4 + p4e,p3e)) {
			UnlockPDE(p4,p4e);
			LF(rf);
			return 1;
		}
		UnlockPDE(p4,p4e);
		LF(rf);
		return 0;
	}
	if(!(p4[p4e] & PAGE_EXIST)){
		UnlockPDE(p4,p4e);
		LF(rf);
		return 0;
	}
	p3 = PADDR2V(p4[p4e]);
	p2e = P2E(addr);
	LockPDE(p3,p3e); 
	UnlockPDE(p4,p4e);
	if(!(p3[p3e] & PAGE_EXIST)) {
		if(pg_ext_test(p3 + p3e,p2e)) {
			UnlockPDE(p3,p3e);
			LF(rf);
			return 1;
		}
		UnlockPDE(p3,p3e);
		LF(rf);
		return 0;
	}
	if(p3[p3e] & PAGE_BIG) {
		UnlockPDE(p3,p3e);
		LF(rf);
		return 1;
	}
	p2 = PADDR2V(p3[p3e]);
	p1e = P1E(addr);
	LockPDE(p2,p2e);
	UnlockPDE(p3,p3e);
	if(!(p2[p2e] & PAGE_EXIST)){
		if(pg_ext_test(p2 + p2e,p1e)) {
			UnlockPDE(p2,p2e);
			LF(rf);
			return 1;
		}
		UnlockPDE(p2,p2e);
		LF(rf);
		return 0;
	}
	if(p2[p2e] & PAGE_BIG) {
		UnlockPDE(p2,p2e);
		LF(rf);
		return 1;
	}
	p1 = PADDR2V(p2[p2e]); p1e = P1E(addr);
	LockPDE(p1,p1e); UnlockPDE(p2,p2e);
	if(p1[p1e] & PAGE_EXIST || p1[p1e] & PAGE_ALLOCATED_MASK){
		UnlockPDE(p1,p1e);
		LF(rf);
		return 1;
	}
	UnlockPDE(p1,p1e);
	LF(rf);
	return 0;
}
//虚拟地址转换为物理地址
PADDR ADDRV2P(LPTHREAD thread,VADDR addr){
	u64 * p4,* p3,* p2,* p1;
	u32 p4e,p3e,p2e,p1e;
	LPPROCESS process = NULL;
	PADDR paddr;
	u64 rf;
	
	p4e = P4E(addr);
	if(p4e >= 256){
		if(p4e > MAX_P4E){
			//error
		}
		else p4 = (&syspdbe) - 256;
	}
	else {
		if(!thread) thread = GetCurThread();
		if(!thread) return -1;
		if(p4e == USER_STACK_P4E_INDEX)
			p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
		else{
			process = thread->father;
			if(!process) return -1;
			p4 = process->pdbe;
		}
	}
	SFI(rf);
	LockPDE(p4,p4e);
	if(!(p4[p4e] & PAGE_EXIST)){
		UnlockPDE(p4,p4e); LF(rf); return 0;
	}
	p3 = PADDR2V(p4[p4e]); p3e = P3E(addr);
	LockPDE(p3,p3e); UnlockPDE(p4,p4e);
	if(!(p3[p3e] & PAGE_EXIST)) {
		UnlockPDE(p3,p3e); LF(rf); return 0;
	}
	if(p3[p3e] & PAGE_BIG) {
		paddr = p3[p3e] & PAGE_PADDR_MASK_1G;
		UnlockPDE(p3,p3e); LF(rf);
		paddr |= ((PADDR)addr) & PAGE_IN_ADDR_MASK_1G;
		return paddr;
	}
	p2 = PADDR2V(p3[p3e]); p2e = P2E(addr);
	LockPDE(p2,p2e); UnlockPDE(p3,p3e);
	if(!(p2[p2e] & PAGE_EXIST)){
		UnlockPDE(p2,p2e); LF(rf); return 0;
	}
	if(p2[p2e] & PAGE_BIG) {
		paddr = p2[p2e] & PAGE_PADDR_MASK_2M;
		UnlockPDE(p2,p2e); LF(rf);
		paddr |= ((PADDR)addr) & PAGE_IN_ADDR_MASK_2M;
		return paddr;
	}
	p1 = PADDR2V(p2[p2e]); p1e = P1E(addr);
	LockPDE(p1,p1e); UnlockPDE(p2,p2e);
	if(p1[p1e] & PAGE_EXIST) {
		paddr = p1[p1e] & PAGE_PADDR_MASK_4K;
		UnlockPDE(p1,p1e); LF(rf);
		paddr |= ((PADDR)addr) & PAGE_IN_ADDR_MASK_4K;
		return paddr;
	}
	UnlockPDE(p1,p1e);
	LF(rf);
	return 0;
}
//映射指定页（包含属性位）到虚拟地址
int put_page(PAGEE page, LPTHREAD thread, VADDR addr){
	u64 * p4,* p3,* p2,* p1;
	u32 p4e,p3e,p2e,p1e;
	LPPROCESS process = NULL;
	PAGEE _page;
	u64 * pdb;
	u64 flags;
	void * bit_map;
	u64 first;
	u32 i;
	u64 user_mode = 0;
	
	p4e = P4E(addr);
	if(p4e >= 256){
		if(p4e > MAX_P4E){
			printk("Try mapping page at unsupport address %P.\n",addr);
			stop();
		}
		else p4 = (&syspdbe) - 256;
	}
	else {
		if(!thread) thread = GetCurThread();
		if(!thread) return -1;
			user_mode = PAGE_USER;
		if(p4e == USER_STACK_P4E_INDEX)
			p4 = ((u64*)&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
		else{
			process = thread->father;
			if(!process) return -1;
			p4 = process->pdbe;
		}
	}
	flags = sflags();
	cli();
	LockPDE(p4,p4e);
	p3e = P3E(addr);
	if(!(p4[p4e] & PAGE_EXIST) && pg_ext_test_and_depress_new(p4 + p4e,p3e,user_mode)){
		UnlockPDE(p4,p4e);
		lflags(flags);
		return -1;
	}
	p3 = PADDR2V(p4[p4e]); 
	LockPDE(p3,p3e);
	if(page & PAGE_EXIST || ((page & PAGE_ALLOCATED_MASK) && (page & SWAP_ID_MASK))){
		//页面为物理页或已分配页且不为空页,即必须使用原始页结构记录
		UnlockPDE(p4,p4e);
		p2e = P2E(addr);
		if((!(p3[p3e] & PAGE_EXIST) && pg_ext_test_and_depress_new(p3 + p3e,p2e,user_mode)) || 
			p3[p3e] & PAGE_BIG){
			UnlockPDE(p3,p3e);
			lflags(flags);
			return -1;
		}
		p2 = PADDR2V(p3[p3e]); 
		LockPDE(p2,p2e);
		UnlockPDE(p3,p3e);
		p1e = P1E(addr);
		if((!(p2[p2e] & PAGE_EXIST) && pg_ext_test_and_depress_new(p2 + p2e,p1e,user_mode)) || 
			p2[p2e] & PAGE_BIG){
			UnlockPDE(p2,p2e);
			lflags(flags);
			return -1;
		}
		p1 = PADDR2V(p2[p2e]); 
		LockPDE(p1,p1e);
		UnlockPDE(p2,p2e);
		if(p1[p1e] & (PAGE_EXIST | PAGE_ALLOCATED_MASK)){
			UnlockPDE(p1,p1e);
			lflags(flags);
			return -1;
		}
		p1[p1e] = page;
		/****************************************
		刷新TLB
		
		
		
		****************************************/
		//UnlockPDE(p1,p1e);
		lflags(flags);
		return 0;
	}
	//可以使用扩展页结构记录
	p2e = P2E(addr);
	if(!(p3[p3e] & PAGE_EXIST) && pg_ext_test_and_depress_new(p3+p3e,p2,user_mode)){
		//对应地址已有效，返回错误
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		lflags(flags);
		return -1;
	}
	p2 = PADDR2V(p3[p3e]);
	LockPDE(p2,p2e);
	p1e = P1E(addr);
	if(p2[p2e] & PAGE_EXIST){
		p1 = PADDR2V(p2[p2e]);
		LockPDE(p1,p1e);
		UnlockPDE(p2,p2e);
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		if(p1[p1e] & (PAGE_EXIST | PAGE_ALLOCATED_MASK)){
			//对应地址已有效，返回错误
			UnlockPDE(p1,p1e);
			lflags(flags);
			return -1;
		}
		p1[p1e] = page;
		//UnlockPDE(p1,p1e);
		lflags(flags);
		return 0;
	}
	if(!(p2[p2e] & PAGE_ALLOCATED_MASK)){
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		bit_map = kmalloc((PAGE_ENT_MASK + 1)/8,(PAGE_ENT_MASK + 1)/8);
		memset(bit_map,0,(PAGE_ENT_MASK + 1)/8);
		bts(bit_map,p1e);
		first = p1e ? 0 : 1;
		p2[p2e] = (first << PAGE_FIRST_SHIFT) | PAGE_ALLOCATED_MASK | 
			((u64)bit_map & PAGE_BIT_MAP_MASK);
		//UnlockPDE(p2,p2e);
		lflags(flags);
		return 0;
	}
	if(!(p2[p2e] & PAGE_BIT_MAP_MASK)){
		UnlockPDE(p2,p2e);
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		lflags(flags);
		return -1;
	}
	bit_map = PAGE2MAP(p2[p2e]);
	if(bts(bit_map,p1e)){
		UnlockPDE(p2,p2e);
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		lflags(flags);
		return -1;
	}
	first = (p2[p2e] & PAGE_FIRST_MASK) >> PAGE_FIRST_SHIFT;
	if(first > p1e){
		//BUG
		first = 0;
	}
	if(first != PAGE_ENT_MASK && first == p1e){
		while((first < PAGE_ENT_MASK + 1) && bt(bit_map,first)) first++;
		if(first < PAGE_ENT_MASK + 1){
			p2[p2e] &= ~PAGE_FIRST_MASK;
			p2[p2e] |= first << PAGE_FIRST_SHIFT;
			UnlockPDE(p2,p2e);
			UnlockPDE(p3,p3e);
			UnlockPDE(p4,p4e);
			lflags(flags);
			return 0;
		}
	}
	p2[p2e] = PAGE_ALLOCATED_MASK;
	//UnlockPDE(p2,p2e);
	memset(bit_map,0,(PAGE_ENT_MASK + 1)/8);
	for(i = 0;i < PAGE_ENT_MASK + 1;i++){
		if(p2[i] == PAGE_ALLOCATED_MASK) bts(bit_map,i);
		else break;
	}
	if((i < PAGE_ENT_MASK + 1) && !p2[i]){
		for(;i < PAGE_ENT_MASK + 1;i++)
			if(p2[i] == PAGE_ALLOCATED_MASK) bts(bit_map,i);
			else if(p2[i]) break;
	}
	if(i <= PAGE_ENT_MASK){
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		lflags(flags);
		kfree(bit_map);
		return 0;
	}
	free_page(p3[p3e]);
	if(first <= PAGE_ENT_MASK){
		p3[p3e] &= ~(PAGE_BIT_MAP_MASK | PAGE_FIRST_MASK);
		p3[p3e] |= ((u64)bit_map & PAGE_BIT_MAP_MASK) | (first << PAGE_FIRST_SHIFT);
		UnlockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		lflags(flags);
		return 0;
	}
	p3[p3e] = PAGE_ALLOCATED_MASK;
	//UnlockPDE(p3,p3e);
	memset(bit_map,0,(PAGE_ENT_MASK + 1)/8);
	for(i = 0;i < PAGE_ENT_MASK + 1;i++)
		if(p3[i] == PAGE_ALLOCATED_MASK) bts(bit_map,i);
		else break;
	first = i;
	if((i < PAGE_ENT_MASK + 1) && !p3[i]){
		for(;i < PAGE_ENT_MASK + 1;i++)
			if(p3[i] == PAGE_ALLOCATED_MASK) bts(bit_map,i);
			else if(p3[i]) break;
	}
	if(i <= PAGE_ENT_MASK){
		UnlockPDE(p4,p4e);
		lflags(flags);
		kfree(bit_map);
		return 0;
	}
	free_page(p4[p4e]);
	if(first >= PAGE_ENT_MASK + 1) {
		p4[p4e] = PAGE_ALLOCATED_MASK;
		//UnlockPDE(p4,p4e);
		lflags(flags);
		kfree(bit_map);
		return 0;
	}
	else{
		p4[p4e] &= ~(PAGE_BIT_MAP_MASK | PAGE_FIRST_MASK);
		p4[p4e] |= ((u64)bit_map & PAGE_BIT_MAP_MASK) | (first << PAGE_FIRST_SHIFT);
		UnlockPDE(p4,p4e);
		lflags(flags);
		return 0;
	}
}
//获取虚拟地址对应的页,包含属性位
PAGEE get_page(LPTHREAD thread,VADDR addr){
	u64 * p4,* p3,* p2,* p1;
	u32 p4e,p3e,p2e,p1e;
	LPPROCESS process = NULL;
	PAGEE page;
	u64 rf;
	
	p4e = P4E(addr);
	if(p4e >= 256){
		if(p4e > MAX_P4E){
			//error
		}
		else p4 = (&syspdbe) - 256;
	}
	else {
		if(!thread) thread = GetCurThread();
		if(!thread) return -1;
		if(p4e == USER_STACK_P4E_INDEX)
			p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
		else{
			process = thread->father;
			if(!process) return -1;
			p4 = process->pdbe;
		}
	}
	SFI(rf);
	LockPDE(p4,p4e);
	if(!(p4[p4e] & PAGE_EXIST)){
		if(p4[p4e] & PAGE_ALLOCATED_MASK){
			UnlockPDE(p4,p4e);
			LF(rf);
			return PAGE_ALLOCATED_MASK;
		}
		UnlockPDE(p4,p4e);
		LF(rf);
		return -1;
	}
	p3 = PADDR2V(p4[p4e]); p3e = P3E(addr);
	LockPDE(p3,p3e); UnlockPDE(p4,p4e);
	if(!(p3[p3e] & PAGE_EXIST)){
		if(p3[p3e] & PAGE_ALLOCATED_MASK){
			UnlockPDE(p3,p3e);
			LF(rf);
			return PAGE_ALLOCATED_MASK;
		}
		UnlockPDE(p3,p3e);
		LF(rf);
		return -1;
	}
	else if(p3[p3e] & PAGE_BIG) {
		page = p3[p3e];
		UnlockPDE(p3,p3e);
		LF(rf);
		return page;
	}
	p2 = PADDR2V(p3[p3e]); p2e = P2E(addr);
	LockPDE(p2,p2e); UnlockPDE(p3,p3e);
	if(!(p2[p2e] & PAGE_EXIST)){
		if(p2[p2e] & PAGE_ALLOCATED_MASK){
			UnlockPDE(p2,p2e);
			LF(rf);
			return PAGE_ALLOCATED_MASK;
		}
		UnlockPDE(p2,p2e);
		LF(rf);
		return -1;
	}
	else if(p2[p2e] & PAGE_BIG) {
		page = p2[p2e];
		UnlockPDE(p2,p2e);
		LF(rf);
		return page;
	}
	p1 = PADDR2V(p2[p2e]); p1e = P1E(addr);
	UnlockPDE(p2,p2e);
	return p1[p1e];
}
//设置虚拟地址为不可交换,虚拟页
int page_unswapable(LPTHREAD thread,VADDR vaddr,size_t size){
	//已分配的虚拟地址才可设置为不可交换
	u64 * p4,* p3,* p2,* p1;
	u32 p4e,p3e,p2e,p1e;
	VADDR start,end;
	u64 swapid;
	LPPROCESS process = NULL;
	PAGEE page;
	u64 user_mode = 0;
	u64 rf;
	
	start = (u64)vaddr & PAGE_PADDR_MASK;
	end = (VADDR)(((u64)vaddr + size + PAGE_SIZE - 1) & PAGE_PADDR_MASK);
	
	if(!thread) thread = GetCurThread();
	if(!process) process = thread->father;
	while(start < end){
		user_mode = 0;
		p4e = P4E(start);
		if(p4e >= 256){
			if(p4e > MAX_P4E){
				//error
			}
			else p4 = (&syspdbe) - 256;
		}
		else{
			user_mode = PAGE_USER;
			if(p4e == USER_STACK_P4E_INDEX) p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
			else p4 = process->pdbe;
		}
		SFI(rf);
		LockPDE(p4,p4e);
		p3e = P3E(start);
		if(!(p4[p4e] & PAGE_EXIST) && pg_ext_test_and_depress(p4+p4e,p3e,user_mode)){
			UnlockPDE(p4,p4e);
			LF(rf);
			return -1;
		}
		p3 = PADDR2V(p4[p4e]); p2e = P2E(start);
		LockPDE(p3,p3e); UnlockPDE(p4,p4e);
		if(!(p3[p3e] & PAGE_EXIST) && pg_ext_test_and_depress(p3+p2e,p2e,user_mode)){
			UnlockPDE(p3,p3e);
			LF(rf);
			return -1;
		}
		if(p3[p3e] & PAGE_BIG) {
			p3[p3e] |= PAGE_PWT | PAGE_PCD;
			UnlockPDE(p3,p3e);
			LF(rf);
			start = (VADDR)((u64)start + PAGE_SIZE_1G);
			continue;
		}
		p2 = PADDR2V(p3[p3e]); p1e = P1E(start);
		LockPDE(p2,p2e); UnlockPDE(p3,p3e);
		if(!(p2[p2e] & PAGE_EXIST) && pg_ext_test_and_depress(p2+p2e,p1e,user_mode)){
			UnlockPDE(p2,p2e);
			LF(rf);
			return -1;
		}
		if(p2[p2e] & PAGE_BIG) {
			p2[p2e] |= PAGE_PWT | PAGE_PCD;
			UnlockPDE(p2,p2e);
			LF(rf);
			start = (VADDR)((u64)start + PAGE_SIZE_2M);
			continue;
		}
		p1 = PADDR2V(p2[p2e]); p1e = P1E(start);
		LockPDE(p1,p1e);
		UnlockPDE(p2,p2e);
		if(p1[p1e] & PAGE_EXIST){
			p1[p1e] |= PAGE_PWT | PAGE_PCD;
			UnlockPDE(p1,p1e);
			LF(rf);
			start = (VADDR)((u64)start + PAGE_SIZE_4K);
			continue;
		}
		else{
			if(p1[p1e] & PAGE_ALLOCATED_MASK){
				if(p1[p1e] & SWAP_ID_MASK){
					swapid = p1[p1e];
					page = get_free_page(0,0,0);
					page &= PAGE_PADDR_MASK;
					page |= p1[p1e] & PAGE_IN_ADDR_MASK;
					page |= PAGE_EXIST;
					p1[p1e] = page;
					//UnlockPDE(p1,p1e);
					LF(rf);
					swap_in((swapid & SWAP_ID_MASK) >>  PAGE_SHIFT_1,page);
				}
				else{
					page = get_free_page(0,0,0);
					page &= PAGE_PADDR_MASK;
					page |= p1[p1e] & PAGE_IN_ADDR_MASK;
					page |= PAGE_EXIST;
					p1[p1e] = page;
					//UnlockPDE(p1,p1e);
					LF(rf);
				}
				start = (VADDR)((u64)start + PAGE_SIZE_4K);
				continue;
			}
			else{
				UnlockPDE(p1,p1e);
				LF(rf);
				return -1;
			}
		}
	}
	return 0;
}
//设置页为可交换
int page_swapable(LPTHREAD thread,VADDR vaddr,size_t size){
	u64 * p4,* p3,* p2,* p1;
	u32 p4e,p3e,p2e,p1e;
	VADDR start,end;
	u64 swapid;
	LPPROCESS process = NULL;
	PAGEE page;
	u64 user_mode;
	u64 rf;
	
	start = (u64)vaddr & PAGE_PADDR_MASK;
	end = (VADDR)(((u64)vaddr + size + PAGE_SIZE - 1) & PAGE_PADDR_MASK);
	
	if(!thread) thread = GetCurThread();
	if(!process) process = thread->father;
	while(start < end){
		user_mode = 0;
		p4e = P4E(start);
		if(p4e >= 256){
			if(p4e > MAX_P4E){
				//error
			}
			else p4 = (&syspdbe) - 256;
		}
		else{
			user_mode = PAGE_USER;
			if(p4e == USER_STACK_P4E_INDEX) p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
			else p4 = process->pdbe;
		}
		SFI(rf);
		LockPDE(p4,p4e);p3e = P3E(start);
		if(!(p4[p4e] & PAGE_EXIST) && pg_ext_test_and_depress(p4+p4e,p3e,user_mode)){
			UnlockPDE(p4,p4e);
			LF(rf);
			return -1;
		}
		p3 = PADDR2V(p4[p4e]); p2e = P2E(start);
		LockPDE(p3,p3e); UnlockPDE(p4,p4e);
		if((!(p3[p3e] & PAGE_EXIST) && pg_ext_test_and_depress(p3+p3e,p2e,user_mode)) || p3[p3e] & PAGE_BIG) {
			UnlockPDE(p3,p3e);
			LF(rf);
			return -1;
		}
		p2 = PADDR2V(p3[p3e]); 
		LockPDE(p2,p2e); UnlockPDE(p3,p3e);p1e = P1E(start);
		if((!(p2[p2e] & PAGE_EXIST) && pg_ext_test_and_depress(p2+p2e,p1e,user_mode)) || p2[p2e] & PAGE_BIG) {
			UnlockPDE(p2,p2e);
			LF(rf);
			return -1;
		}
		p1 = PADDR2V(p2[p2e]); 
		LockPDE(p1,p1e);
		UnlockPDE(p2,p2e);
		if(p1[p1e] & PAGE_EXIST){
			p1[p1e] |= PAGE_PWT | PAGE_PCD;
			UnlockPDE(p1,p1e);
			LF(rf);
			start = (VADDR)((u64)start + PAGE_SIZE_4K);
			continue;
		}
		else{
			UnlockPDE(p1,p1e);
			LF(rf);
			return -1;
		}
	}
	return 0;
}
//释放页
int page_allocatable(u64 page){
	if(!page) return -1;
	page -= free_page_base;
	page >>= 12;
	if(page >= page_count) return -1;
	mem_map[page] = 0;
	return 0;
}
//强制占用页
int page_disallocatable(u64 page){
	if(!page) return -1;
	page -= free_page_base;
	page >>= 12;
	if(page >= page_count) return -1;
	mem_map[page] = 1;
	return 0;
}
//设置页为不可缓冲，页必须存在
int page_uncacheable(LPTHREAD thread,VADDR vaddr,size_t size){
	u64 * p4,* p3,* p2,* p1;
	u32 p4e,p3e,p2e,p1e;
	VADDR start,end;
	u64 swap_id;
	LPPROCESS process = NULL;
	PAGEE page;
	u64 user_mode;
	u64 rf;
	
	start = (u64)vaddr & PAGE_PADDR_MASK;
	end = (VADDR)(((u64)vaddr + size + PAGE_SIZE - 1) & PAGE_PADDR_MASK);
	
	while(start < end){
		p4e = P4E(start);
		user_mode = 0;
		if(p4e >= 256){
			if(p4e > MAX_P4E){
				//error
			}
			else p4 = (&syspdbe) - 256;
		}
		else {
			user_mode = 1;
			if(!thread) thread = GetCurThread();
			if(!thread) return -1;
			if(p4e == USER_STACK_P4E_INDEX) p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
			else {
				process = thread->father;
				if(!process) return -1;
				p4 = process->pdbe;
			}
		}
		SFI(rf);
		//asm("cli\n\thlt"::"a"(p4),"c"(p4e));
		LockPDE(p4,p4e); p3e = P3E(start);
		if(!(p4[p4e] & PAGE_EXIST) && pg_ext_test_and_depress(p4+p4e,p3e,user_mode)){
			UnlockPDE(p4,p4e);
			LF(rf);
			return -1;
		}
		p3 = PADDR2V(p4[p4e]); p2e = P2E(start);
		LockPDE(p3,p3e); UnlockPDE(p4,p4e);
		if(!(p3[p3e] & PAGE_EXIST) && pg_ext_test_and_depress(p3+p3e,p2e,user_mode)){
			UnlockPDE(p3,p3e);
			LF(rf);
			return -1;
		}
		if(p3[p3e] & PAGE_BIG) {
			p3[p3e] |= PAGE_PWT | PAGE_PCD;
			UnlockPDE(p3,p3e);
			LF(rf);
			start = (VADDR)((u64)start + PAGE_SIZE_1G);
			continue;
		}
		p2 = PADDR2V(p3[p3e]); p1e = P1E(start);
		LockPDE(p2,p2e); UnlockPDE(p3,p3e);
		if(!(p2[p2e] & PAGE_EXIST) && pg_ext_test_and_depress(p2+p2e,p1e,user_mode)){
			UnlockPDE(p2,p2e);
			LF(rf);
			return -1;
		}
		if(p2[p2e] & PAGE_BIG) {
			p2[p2e] |= PAGE_PWT | PAGE_PCD;
			UnlockPDE(p2,p2e);
			LF(rf);
			start = (VADDR)((u64)start + PAGE_SIZE_2M);
			continue;
		}
		p1 = PADDR2V(p2[p2e]);
		LockPDE(p1,p1e);
		UnlockPDE(p2,p2e);
		if(p1[p1e] & PAGE_EXIST){
			p1[p1e] |= PAGE_PWT | PAGE_PCD;
			UnlockPDE(p1,p1e);
			LF(rf);
			start = (VADDR)((u64)start + PAGE_SIZE_4K);
			continue;
		}
		else{
			page = get_free_page(0,0,0);
			page &= PAGE_PADDR_MASK;
			page |= p1[p1e] & PAGE_IN_ADDR_MASK;
			page |= PAGE_EXIST | PAGE_PWT | PAGE_PCD;
			if(p1[p1e] & PAGE_ALLOCATED_MASK){
				swap_id = p1[p1e];
				p1[p1e] = page;
				LF(rf);
				if(swap_id & SWAP_ID_MASK){
					swap_id &= SWAP_ID_MASK;
					swap_id >>= PAGE_SHIFT_1;
					swap_in(swap_id,page);
				}
				if(p4e >= 256 || 
					(p4e == USER_STACK_P4E_INDEX && thread == GetCurThread()) ||
					(process && process == GetCurProcess())) invtlb(start);
				start = (VADDR)((u64)start + PAGE_SIZE_4K);
				continue;
			}
			else{
				UnlockPDE(p1,p1e);
				LF(rf);
				return -1;
			}
		}
	}
	return 0;
}
//设置页为可缓冲，必须存在
int page_cacheable(LPTHREAD thread,VADDR vaddr,size_t size){
	u64 * p4,* p3,* p2,* p1;
	u32 p4e,p3e,p2e,p1e;
	VADDR start,end;
	u64 swapid;
	LPPROCESS process = NULL;
	PAGEE page;
	u64 user_mode = 0;
	u64 rf;
	
	start = (u64)vaddr & PAGE_PADDR_MASK;
	end = (VADDR)(((u64)vaddr + size + PAGE_SIZE - 1) & PAGE_PADDR_MASK);
	
	if(!thread) thread = GetCurThread();
	if(!process) process = thread->father;
	while(start < end){
		p4e = P4E(start);
		if(p4e >= 256){
			if(p4e > MAX_P4E){
				//error
			}
			else p4 = (&syspdbe) - 256;
		}
		else{
			user_mode = PAGE_USER;
			if(p4e == USER_STACK_P4E_INDEX) p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
			else p4 = process->pdbe;
		}
		SFI(rf);
		LockPDE(p4,p4e);p3e = P3E(start);
		if(!(p4[p4e] & PAGE_EXIST) && pg_ext_test_and_depress(p4+p4e,p3e,user_mode)){
			UnlockPDE(p4,p4e);
			LF(rf);
			return -1;
		}
		p3 = PADDR2V(p4[p4e]); p2e = P2E(start);
		LockPDE(p3,p3e); UnlockPDE(p4,p4e);
		if(!(p3[p3e] & PAGE_EXIST) && pg_ext_test_and_depress(p3+p3e,p2e,user_mode)){
			UnlockPDE(p3,p3e);
			LF(rf);
			return -1;
		}
		if(p3[p3e] & PAGE_BIG) {
			p3[p3e] &= ~(PAGE_PWT | PAGE_PCD);
			UnlockPDE(p3,p3e);
			LF(rf);
			start = (VADDR)((u64)start + PAGE_SIZE_1G);
			continue;
		}
		p2 = PADDR2V(p3[p3e]); 
		LockPDE(p2,p2e); UnlockPDE(p3,p3e); p1e = P1E(start);
		if(!(p2[p2e] & PAGE_EXIST) && pg_ext_test_and_depress(p2+p2e,p1e,user_mode)){
			UnlockPDE(p2,p2e);
			LF(rf);
			return -1;
		}
		if(p2[p2e] & PAGE_BIG) {
			p2[p2e] &= ~(PAGE_PWT | PAGE_PCD);
			UnlockPDE(p2,p2e);
			LF(rf);
			start = (VADDR)((u64)start + PAGE_SIZE_2M);
			continue;
		}
		p1 = PADDR2V(p2[p2e]);
		LockPDE(p1,p1e);
		UnlockPDE(p2,p2e);
		if(p1[p1e] & PAGE_EXIST){
			p1[p1e] &= ~(PAGE_PWT | PAGE_PCD | PAGE_BUSY_MASK);
			//UnlockPDE(p1,p1e);
			LF(rf);
			if(p4e >= 256 || 
				(p4e == USER_STACK_P4E_INDEX && thread == GetCurThread()) ||
				(process && process == GetCurProcess())) invtlb(start);
			start = (VADDR)((u64)start + PAGE_SIZE_4K);
			continue;
		}
		else{
			UnlockPDE(p1,p1e);
			LF(rf);
			return -1;
		}
	}
	return 0;
}
int free_page_table(LPPROCESS process){
	u64 * p4,*p3,*p2,*p1;
	u32 p4e,p3e,p2e,p1e;
	void * bit_map;

	if(!process) return ERR_INVAILD_PTR;
	p4 = process->pdbe;
	for(p4e = 0;p4e < 256;p4e++){
		if(!p4[p4e]) continue;
		if(!(p4[p4e] & PAGE_EXIST)){
			if(p4[p4e] & PAGE_ALLOCATED_MASK && 
				p4[p4e] & PAGE_BIT_MAP_MASK)
				kfree(PAGE2MAP(p4[p4e]));
			continue;
		}
		p3 = PADDR2V(p4[p4e]);
		for(p3e = 0;p3e <= PAGE_ENT_MASK;p3e++){
			if(!p3[p3e]) continue;
			if(!(p3[p3e] & PAGE_EXIST)){
				if(p3[p3e] & PAGE_ALLOCATED_MASK && 
					p3[p3e] & PAGE_BIT_MAP_MASK)
					kfree(PAGE2MAP(p3[p3e]));
				continue;
			}
			p2 = PADDR2V(p3[p3e]);
			for(p2e = 0;p2e <= PAGE_ENT_MASK;p2e++){
				if(!p2[p2e]) continue;
				if(!(p2[p2e] & PAGE_EXIST)){
					if(p2[p2e] & PAGE_ALLOCATED_MASK && 
						p2[p2e] & PAGE_BIT_MAP_MASK)
						kfree(PAGE2MAP(p2[p2e]));
					continue;
				}
				p1 = PADDR2V(p2[p2e]);
				for(p1e = 0;p1e <= PAGE_ENT_MASK;p1e++)
					if(p1[p1e]) free_page(p1[p1e]);
				free_page(p1[p1e]);
			}
			free_page(p2[p2e]);
		}
		free_page(p3[p3e]);
		p3[p3e] = NULL;
	}
	return 0;
}
u64 create_paging(){
	u64 pdb;
	u64 * p4;
	
	pdb = __get_free_page(0);
	p4 = PADDR2V(pdb);
	memset(p4,0,PAGE_SIZE);
	p4[256] = syspdbe;
	return pdb;
}
void paging_init_ap(){
	u64 * p4;
	
	p4 = (u64*)(get_cr3() & PAGE_PADDR_MASK);
	p4[256] = syspdbe;
	set_cr3(get_cr3());
}
static void paging_panic(struct _REG_STATUS_ * reg,u64 addr){
	printk("rax:%016llX rcx:%016llX rdx:%016llX\n",reg->rax,reg->rcx,reg->rdx);
	printk("rbx:%016llX rsp:%016llX rbp:%016llX\n",reg->rbx,reg->rsp,reg->rbp);
	printk("rsi:%016llX rdi:%016llX  r8:%016llX\n",reg->rsi,reg->rdi,reg->r8);
	printk(" r9:%016llX r10:%016llX r11:%016llX\n",reg->r9,reg->r10,reg->r11);
	printk("r12:%016llX r13:%016llX r14:%016llX\n",reg->r12,reg->r13,reg->r14);
	printk("r15:%016llX rip:%016llX rflgas:%016llX\n",reg->r15,reg->rip,reg->rflags);
	printk("err:%016llX cs:%04X ss:%04X\n",reg->err_code,reg->cs,reg->ss);
	printk("access address %P.\n",addr);
	stop();
}
int paging_fault(struct _REG_STATUS_ * reg){
	VADDR addr;
	u64 * p4,*p3,*p2,*p1;
	u64 * _p;
	u32 p4e,p3e,p2e,p1e;
	PAGEE page,_page;
	LPPROCESS process;
	LPTHREAD thread;
	void * bit_map;
	u32 i;
	u64 swap_id;
	u64 user_mode = 0;
	u64 rf;
	
/*
bit0:P:		0:non-present page
			1:page-level protection violation
bit1:W/R:	0:read
			1:write
bit2:U/S:	0:supervisor-mode
			1:user-mode
bit3:RSVD:	0:not caused by reserved bit violation
			1:caused by a reserved bit set to 1 in some paging-structure entry
bit4:I/D:	0:not instruction
			1:instruction
*/	
	//paging_panic(reg,addr);
	addr = (VADDR)get_cr2();
	//asm("cli\n\thlt"::"a"(addr));
	//if(addr == 0x7fffffffeff0) stop();
	if(!(reg->err_code & 0x01)){
		p4e = P4E(addr);
		if(p4e >= 256){
			if(p4e > MAX_P4E){
				//error
			}
			else p4 = (&syspdbe) - 256;
		}
		else {
			thread = GetCurThread();
			user_mode = PAGE_USER;
			if(p4e == USER_STACK_P4E_INDEX) p4 = (&(thread->user_stack_p4e)) - USER_STACK_P4E_INDEX;
			else{
				process = thread->father;
				if(!process) paging_panic(reg,(u64)addr);
				p4 = process->pdbe;
			}
			
		}
		if(p4[p4e] & PAGE_EXIST){
			_p = PADDR2V(get_cr3());
			if(!(_p[p4e] & PAGE_EXIST)){
				//PML4E not updata
				_p[p4e] = p4[p4e];
				invtlb(addr);
				return 0;
			}
		}
		SFI(rf);
		LockPDE(p4,p4e);
		p3e = P3E(addr);
		if(!(p4[p4e] & PAGE_EXIST) && pg_ext_test_and_depress(p4+p4e,p3e,user_mode)){
			UnlockPDE(p4,p4e);
			LF(rf);
			paging_panic(reg,addr);
			return 0;
		}
		p3 = PADDR2V(p4[p4e]);
		LockPDE(p3,p3e);
		UnlockPDE(p4,p4e);
		p2e = P2E(addr);
		if(!(p3[p3e] & PAGE_EXIST) && pg_ext_test_and_depress(p3+p3e,p2e,user_mode)){
			UnlockPDE(p3,p3e);
			LF(rf);
			paging_panic(reg,addr);
			return 0;
		}
		p2 = PADDR2V(p3[p3e]);
		LockPDE(p2,p2e);
		UnlockPDE(p3,p3e);
		p1e = P1E(addr);
		if(!(p2[p2e] & PAGE_EXIST) && pg_ext_test_and_depress(p2+p2e,p1e,user_mode)){
			UnlockPDE(p2,p2e);
			LF(rf);
			paging_panic(reg,addr);
			return 0;
		}
		p1 = PADDR2V(p2[p2e]);
		LockPDE(p1,p1e);
		UnlockPDE(p2,p2e);
		if(!(p1[p1e] & PAGE_EXIST)){
			if(p1[p1e] & PAGE_ALLOCATED_MASK){
				swap_id = p1[p1e];
				page = __get_free_page(0) | PAGE_EXIST | PAGE_WRITE | user_mode;
				p1[p1e] = page;
				//UnlockPDE(p1,p1e);
				LF(rf);
				swap_id &= SWAP_ID_MASK;
				swap_id >>= PAGE_SHIFT_1;
				if(swap_id){
					swap_in(swap_id,page);
				}
				return 0;
			}
			else{
				UnlockPDE(p1,p1e);
				LF(rf);
				paging_panic(reg,addr);
				return 0;
			}
		}
		else{
			UnlockPDE(p1,p1e);
			LF(rf);
			paging_panic(reg,addr);
			return 0;
		}
	}
	paging_panic(reg,addr);
	return 0;
}
int INIT paging_init_bp(u64 memory_start,u64 memory_size){
	u64 * p4,* p3,* p2,* p1;
	u32 p3e,p2e,p1e;
	u32 i,j,mask_size;
	int _cpuid[4];
	u64 addr;
	u64 r_size;
	
	r_size = memory_size;
	if(memory_size < 4LL * 1024 * 1024 * 1024) memory_size = 4LL * 1024 * 1024 * 1024;
	memory_start += PAGE_SIZE - 1;
	memory_start &= ~(PAGE_SIZE - 1);
	p4 = (VADDR)memory_start;
	memset(p4,0,PAGE_SIZE);
	memory_start += PAGE_SIZE;
	cpuid(0x80000001,0,_cpuid);
	if(memory_size > 256LL * 1024 * 1024 * 1024) memory_size = 256LL * 1024 * 1024 * 1024;
	p3e = (memory_size + (1LL << PAGE_SHIFT_3) - 1) >> PAGE_SHIFT_3;
	p2e = (memory_size + (1LL << PAGE_SHIFT_2) - 1) >> PAGE_SHIFT_2;
	{//map up to 256GB physiacl memory space at 0xffff800000000000
		p3 = (VADDR)memory_start;
		syspdbe = p4[256] = memory_start | PAGE_WRITE | PAGE_EXIST;
		memory_start += PAGE_SIZE;
		if(_cpuid[3] & 0x04000000){
			//Suppose 1G page
			for(i = 0;i < p3e;i++) p3[i] = i * PAGE_SIZE_1G | PAGE_BIG | PAGE_WRITE | PAGE_EXIST; 
			for(;i < 512;i++) p3[i] = 0;
		}
		else {
			//Unsuppose 1G page
			p2 = (VADDR)memory_start;
			for(i = 0;i < p3e;i++) p3[i] = memory_start + i * PAGE_SIZE | PAGE_WRITE | PAGE_EXIST;
			for(;i < 512;i++) p3[i] = 0;
			memory_start += p3e * PAGE_SIZE;
			for(i = 0;i < p2e;i++) p2[i] = i * PAGE_SIZE_2M | PAGE_BIG | PAGE_WRITE | PAGE_EXIST; 
			for(;i < p3e * 512;i++) p2[i] = 0;
		}
	}
	mem_map = ADDRP2V(memory_start);
	memory_size = r_size;
	page_count = (memory_size - memory_start)/(4096 + sizeof(mem_map[0]));
	page_count--;
	memset(mem_map,0,page_count * sizeof(mem_map[0]));
	memory_start += page_count * sizeof(mem_map[0]);
	memory_start += PAGE_SIZE - 1;
	memory_start &= ~(PAGE_SIZE - 1);
	free_page_base = memory_start;
	cur_page = page_count - 1;
	set_cr3(p4);
	return 0;
}

