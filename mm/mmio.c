/*
	mm/mmio.c
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
#include <mm.h>
#include <asm.h>
#include <stdio.h>
#include <string.h>
#include <int.h>
#include <spinlock.h>

static void * mmio_mask;
static volatile int lock;
static u32 first;

spin_optr_def_bit(Global,&lock,0);

void * mmio_map(u64 paddr,size_t size){
	u64 start,end;
	u32 count;
	u32 i, j;
	
	start = paddr;
	end = paddr + size;
	
	start &= 0xfffffffffffff000;
	end += 0xfff;
	end &= 0xfffffffffffff000;
	count = (end - start) >> 12;
	
	ID();
	LockGlobal();
	for(i = first;i < MMIOSIZE >> 15;i++){
		for(j = 0;j < count;j++)
			if(bt(mmio_mask,i+j)) break;
		if(j == count) break;
		i += j - 1;
	}
	for(j = 0;j < count;j++) bts(mmio_mask,i+j);
	first = i + j;
	UnlockGlobal();
	IE();
	for(j = 0;j < count;j++,start += 0x1000){
		put_page(start | 0x03,0,(void*)(MMIOBASE + ((i + j) << 12)));
		page_disallocatable(start);
	
	}
	page_uncacheable(NULL,(void*)(MMIOBASE + ((i + j) << 12)),count << 12);
	return (void*)(MMIOBASE + (i << 12));
}
void mmio_init(){
	mmio_mask = kmalloc(MMIOSIZE >> 15,0);
	memset(mmio_mask,0,MMIOSIZE >> 15);
	lock = 0;
	first = 0;
}



