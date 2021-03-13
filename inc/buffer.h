/*
	inc/buffer.h
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

#ifndef _BUFFER_H_
#define _BUFFER_H_
#include <stddef.h>
#include <fs.h>
#include <stdio.h>

#define BH_LOCKED		1
#define BH_UPDATEING	2
#define BH_DIRTY		4
#define BH_VAILD		8
#define BH_24BITS		0x00
#define BH_32BITS		0x10
#define BH_64BITS		0x20
#define BH_EALLOC		0x100
#define BH_LOCK_LIST_SIZE	14

struct _BUFFER_LOCK_ARRAY_ {
	int mask;//4
	int attr;//4
	unsigned int start[BH_LOCK_LIST_SIZE];//24
	unsigned int end[BH_LOCK_LIST_SIZE];//24
	struct _BUFFER_LOCK_ARRAY_* next;//8
};

struct _LL_BLOCK_DEV_ {
	void* data;
	int32_t cache_size;
    unsigned char bits;
	int(*read_block)(struct _BUFFER_HEAD_*);
	int(*write_block)(struct _BUFFER_HEAD_*);
};
struct _BUFFER_HEAD_ {
	volatile int status;
		//bit0:locked
		//bit1:updataing
		//bit2:dirty
		//bit3:vaild
		//bit7-4:address bits
		//bit8:extend alloc be used
	unsigned int ref_count;
	unsigned int buf_size;
	unsigned int byte_off;
	void* addr;
	u64 iblock;
	struct _LL_BLOCK_DEV_ * dev;
	LPTHREAD wait;
    struct _BUFFER_HEAD_* prev,* next;
	struct _BUFFER_HEAD_* parent;
	struct _BUFFER_LOCK_ARRAY_* lock_list;
	void * alloc_ptr;
	union{
		void * pdata[5];
		int64_t idata[5];
	} edata;
};

struct _BUFFER_HEAD_* ll_bread(struct _LL_BLOCK_DEV_* dev, u64 iblock);
struct _BUFFER_HEAD_* ll_balloc(struct _LL_BLOCK_DEV_* dev, u64 iblock);
struct _BUFFER_HEAD_* ml_blink(struct _LL_BLOCK_DEV_* dev, u64 iblock, uint32_t byte_off,uint32_t size);
struct _BUFFER_HEAD_* hl_blink(struct _BUFFER_HEAD_* _bh, uint32_t byte_off, uint32_t size);
struct _BUFFER_HEAD_* syn_ml_blink(struct _LL_BLOCK_DEV_ * dev,u64 iblock,uint32_t byte_off,uint32_t size);
struct _BUFFER_HEAD_* syn_hl_blink(struct _BUFFER_HEAD_* _bh, uint32_t byte_off, uint32_t size);
void * bealloc(struct _BUFFER_HEAD_ * bh,size_t size,unsigned int align);
int buf_lock(struct _BUFFER_HEAD_* bh, uint32_t start, uint32_t end);
int buf_unlock(struct _BUFFER_HEAD_* bh, uint32_t start);
int bfree(struct _BUFFER_HEAD_* bh);
int bsync(struct _LL_BLOCK_DEV_* dev);
u64 bclean();
static inline void bdirty(struct _BUFFER_HEAD_* bh) {
	if (bh->parent) bh = bh->parent;
	lock_ord(&(bh->status),BH_DIRTY);
}

#endif