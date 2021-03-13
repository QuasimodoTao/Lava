/*
	tools/dop/buffer.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "dop.h"
#include "error.h"
#include "fs.h"

#define InsertFirstList(_head,_this,_prev,_next)	\
{\
	_this->_prev = NULL;\
	_this->_next = _head;\
	_head->_prev = _this;\
	_head = _this;\
}
#define RemoveFirstList(_head,_this,_prev,_next) \
{\
	if(_this->_prev) _this->_prev->_next = _this->_next;\
	else _head = _this->_next;\
	_this->_next->_prev = _this->_prev;\
}


static struct _BUFFER_HEAD_* free_head_list = NULL;

static struct _BUFFER_HEAD_ const_node = {
	.dev = (void*)1,
	.iblock = 0,
	.byte_off = 0,
	.addr = NULL,
	.prev = NULL,
	.next = NULL
};
static struct _BUFFER_HEAD_* first_free = &const_node;
static struct _BUFFER_HEAD_* last_free = &const_node;
static struct _BUFFER_HEAD_* map_table[256];
static struct _LL_BLOCK_DEV_* dev_array[LL_BLOCK_DEV_MAX] = {NULL,};
/*
allocate buffer
bits:0,24,32
*/

/*
BH_ADDR_VAILD = 0;������ʱ���еģ���������δӳ��
BH_DIRTY = 1;�������Ѿ����޸ģ���Ҫд�ش���
BH_BUSY = 1;������ͷ���ڱ�����
BH_UPDATAING = 1;���������ڱ����£������ڷ��ʴ���
*/

static int inline mapping(HANDLE dev, iblock_t iblock, uint32_t byte_off) {
	iblock ^= (u64)dev;
	iblock ^= byte_off;
	iblock ^= iblock >> 32;
	iblock ^= iblock >> 16;
	iblock ^= iblock >> 8;
	return iblock & 0x0ff;
}
static struct _BUFFER_HEAD_* raw_buf_alloc(struct _LL_BLOCK_DEV_* dev, iblock_t iblock,uint32_t byte_off) {
	int map;
	struct _BUFFER_HEAD_* bh;

	map = mapping(dev, iblock, byte_off);
	bh = map_table[map];
	while (bh) {
		if (bh->dev == dev && 
			bh->iblock == iblock && 
			bh->byte_off == byte_off) {

			bh->ref_count++;
			return bh;
		}
		bh = bh->next;
	}
	bh = first_free;
	while (bh != &const_node) {
		if (bh->dev == dev && 
			bh->iblock == iblock && 
			bh->byte_off == byte_off) {
			bh->ref_count = 1;
			RemoveFirstList(first_free, bh, prev, next);
			InsertList(map_table[map], bh, prev, next);
			return bh;
		}
		bh = bh->next;
	}
	if (free_head_list) {
		bh = free_head_list;
		free_head_list = free_head_list->next;
		if (free_head_list) free_head_list->prev = NULL;
	}
	else {
		bh = malloc(sizeof(struct _BUFFER_HEAD_));
		if (!bh) return NULL;
	}
	memset(bh, 0, sizeof(struct _BUFFER_HEAD_));
	bh->dev = dev;
	bh->ref_count = 1;
	bh->addr = malloc(dev->cache_size);
	bh->iblock = iblock;
	bh->buf_size = dev->cache_size;
	bh->byte_off = byte_off;
	InsertList(map_table[map], bh, prev, next);
	return bh;
}

struct _BUFFER_HEAD_* ll_bread(struct _LL_BLOCK_DEV_* dev, iblock_t iblock) {
	struct _BUFFER_HEAD_* bh;

	if (!dev) return NULL;
	bh = raw_buf_alloc(dev, iblock, 0);
	if (!(bh->status & BH_VAILD)) {
		if (dev->read_block(bh)) return NULL;
		bh->status |= BH_VAILD;
	}
	return bh;
}
struct _BUFFER_HEAD_* ll_balloc(struct _LL_BLOCK_DEV_* dev, iblock_t iblock) {
	struct _BUFFER_HEAD_* bh;

	if (!dev) return NULL;
	bh = raw_buf_alloc(dev, iblock, 0);
	bh->status |= BH_VAILD;
	return bh;
}
struct _BUFFER_HEAD_* ml_blink(struct _LL_BLOCK_DEV_* dev, iblock_t iblock, uint32_t byte_off,uint32_t size) {
	struct _BUFFER_HEAD_* bh,*_bh;
	unsigned int map;

	if (!dev) return NULL;
	map = mapping(dev, iblock, byte_off);
	bh = map_table[map];
	while (bh) {
		if (bh->dev == dev &&
			bh->iblock == iblock &&
			bh->byte_off == byte_off) {
			bh->ref_count++;
			return bh;
		}
		bh = bh->next;
	}
	bh = raw_buf_alloc(dev, iblock, 0);
	if (!(bh->status & BH_VAILD)) {
		if (dev->read_block(bh)) return NULL;
		bh->status |= BH_VAILD;
	}
	if (!byte_off) return bh;
	if (free_head_list) {
		_bh = free_head_list;
		free_head_list = free_head_list->next;
		if (free_head_list) free_head_list->prev = NULL;
	}
	else {
		_bh = malloc(sizeof(struct _BUFFER_HEAD_));
		if (!_bh) return NULL;
	}
	memset(_bh, 0, sizeof(struct _BUFFER_HEAD_));
	_bh->parent = bh;
	_bh->iblock = iblock;
	_bh->ref_count = 1;
	_bh->addr = byte_off + (char*)(bh->addr);
	_bh->buf_size = size;
	_bh->byte_off = byte_off;
	_bh->dev = dev;
	InsertList(map_table[map], _bh, prev, next);
	_bh->status |= BH_VAILD;
	return _bh;
}
struct _BUFFER_HEAD_* hl_blink(struct _BUFFER_HEAD_* _bh, uint32_t byte_off, uint32_t size) {
	struct _BUFFER_HEAD_* bh,*unvaild = NULL;
	unsigned int map;

	if (!_bh) return NULL;
	if (!byte_off) {
		_bh->ref_count++;
		return _bh;
	}
	byte_off += _bh->byte_off;
	map = mapping(_bh->dev, _bh->iblock, byte_off);
	bh = map_table[map];
	while (bh) {
		if (bh->dev == _bh->dev && 
			bh->iblock == _bh->iblock && 
			bh->byte_off == byte_off) {
			bh->ref_count++;
			return bh;
		}
		bh = bh->next;
	}
	bh = raw_buf_alloc(_bh->dev, _bh->iblock, 0);
	if (!(bh->status & BH_VAILD)) {
		if (bh->dev->read_block(bh)) return NULL;
		bh->status |= BH_VAILD;
	}

	if (free_head_list) {
		_bh = free_head_list;
		free_head_list = free_head_list->next;
		if (free_head_list) free_head_list->prev = NULL;
	}
	else {
		_bh = malloc(sizeof(struct _BUFFER_HEAD_));
		if (!_bh) return NULL;
	}
	memset(_bh, 0, sizeof(struct _BUFFER_HEAD_));
	_bh->parent = bh;
	_bh->iblock = bh->iblock;
	_bh->ref_count = 1;
	_bh->addr = byte_off + (char*)(bh->addr);
	_bh->buf_size = size;
	_bh->byte_off = byte_off;
	_bh->dev = bh->dev;
	InsertList(map_table[map], _bh, prev, next);
	_bh->status |= BH_VAILD;
	return _bh;
}
int bfree(struct _BUFFER_HEAD_* bh) {
	int map;

	if (!bh) return -1;
	bh->ref_count--;
	if (bh->ref_count) return 0;
	map = mapping(bh->dev, bh->iblock, bh->byte_off);
	RemoveList(map_table[map], bh, prev, next);
	if (bh->parent) {
		InsertList(free_head_list, bh, prev, next);
		bh = bh->parent;
		bh->ref_count--;
		if (bh->ref_count) return 0;
		if (bh->lock_list) {
			wprintf(L"BUG:");
			return 0;
		}
		map = mapping(bh->dev, bh->iblock, bh->byte_off);
		RemoveList(map_table[map], bh, prev, next);
	}
	InsertFirstList(first_free, bh, prev, next);
	return 0;
}
int bsync(struct _LL_BLOCK_DEV_* dev) {
	struct _BUFFER_HEAD_* bh;
	int i;

	for (i = 0;i < 256;i++) {
		if (map_table[i]) {
			printf("BUG:buffer:map_table[%d] is no NULL,sector %lld,byte offset %d is not free.\n",
				i, map_table[i]->iblock, map_table[i]->byte_off);
		}
	}
	bh = first_free;
	if (dev) {
		while (bh != &const_node) {
			if (bh->dev == dev && bh->status & BH_DIRTY && bh->byte_off == 0) {
				bh->dev->write_block(bh);
			}
			bh = bh->next;
		}
	}
	else {
		while (bh) {
			if (bh->status & BH_DIRTY)
				bh->dev->write_block(bh);
			bh = bh->next;
		}
		for (i = 0;i < 256;i++) {
			bh = map_table[i];
			while (bh) {
				if (bh->status & BH_DIRTY)
					bh->dev->write_block(bh);
				bh = bh->next;
			}
		}
	}
	return 0;
}
int buf_lock(struct _BUFFER_HEAD_* bh, uint32_t start, uint32_t end) {
	struct _BUFFER_LOCK_ARRAY_* lock_list,*unvaild = NULL;
	int i,iunvaild;
	int mask,_mask;

	if (!bh) return -1;
	if (start > end) return -1;
	if (end > bh->buf_size) return -1;
	if (bh->parent) {
		start += bh->byte_off;
		end += bh->byte_off;
		bh = bh->parent;
	}
	lock_list = bh->lock_list;
	while (lock_list) {
		mask = 1;
		_mask = lock_list->mask;
		for (i = 0;i < BH_LOCK_LIST_SIZE;i++) {
			if (mask & _mask) {
				if ((lock_list->start[i] >= start && lock_list->end[i] <= start) ||
					lock_list->start[i] <= end && lock_list->end[i] >= end) {
					if (unvaild) unvaild->attr &= 1 << iunvaild;
					return 1;//fail
				}
			}
			else {
				if (!unvaild && !(lock_list->attr & mask)) {
					unvaild = lock_list;
					iunvaild = i;
					unvaild->attr |= mask;
				}
			}
			mask <<= 1;
		}
		lock_list = lock_list->next;
	}
	if (unvaild) {
		lock_list = unvaild;
		lock_list->start[iunvaild] = start;
		lock_list->end[iunvaild] = end;
		unvaild->attr &= 1 << iunvaild;
	}
	else {
		lock_list = malloc(sizeof(struct _BUFFER_LOCK_ARRAY_));
		if (!lock_list) return -1;
		lock_list->mask = 1;
		lock_list->attr = 0;
		lock_list->start[0] = start;
		lock_list->end[0] = end;
		lock_list->next = bh->lock_list;
		bh->lock_list = lock_list;
	}
	return 0;
}
int buf_try_lock(struct _BUFFER_HEAD_* bh, uint32_t start, uint32_t end) {
	return buf_lock(bh, start, end);
}
int buf_unlock(struct _BUFFER_HEAD_* bh, uint32_t start) {
	struct _BUFFER_LOCK_ARRAY_* lock_list,*prev = NULL;
	int i, mask, _mask;

	if (!bh) return -1;
	if (bh->parent) {
		start += bh->byte_off;
		bh = bh->parent;
	}
	lock_list = bh->lock_list;
	while (lock_list) {
		mask = 1;
		_mask = lock_list->mask;
		for (i = 0;i < BH_LOCK_LIST_SIZE;i++) {
			if (_mask & mask) {
				if (lock_list->start[i] == start) {
					lock_list->mask &= !mask;
					if (!lock_list->mask && !lock_list->attr) {
						if (prev) prev->next = lock_list->next;
						else bh->lock_list = lock_list->next;
						free(lock_list);
					}
					return 0;
				}
			}
			mask <<= 1;
		}
		prev = lock_list;
	}
	return -1;
}
int buf_islock(struct _BUFFER_HEAD_* bh, uint32_t pos) {
	struct _BUFFER_LOCK_ARRAY_* lock_list, * prev = NULL;
	int i, mask, _mask;

	if (!bh) return -1;
	if (bh->parent) {
		pos += bh->byte_off;
		bh = bh->parent;
	}
	lock_list = bh->lock_list;
	while (lock_list) {
		mask = 1;
		_mask = lock_list->mask;
		for (i = 0;i < BH_LOCK_LIST_SIZE;i++,mask <<= 1) 
			if (_mask & mask && lock_list->start[i] >= pos && lock_list->end[i] < pos) 
				return 1;
		prev = lock_list;
	}
	return 0;
}
u64 bclean() {
	struct _BUFFER_HEAD_* bh,*_bh;
	u64 count = 0;

	bh = last_free->prev;
	if (!bh) return 0;
	while (bh->prev) {
		if (bh->status & BH_DIRTY) continue;
		free(bh->addr);
		count += bh->dev->cache_size;
		bh->dev = NULL;
		_bh = bh;
		bh = bh->prev;
		_bh->next->prev = bh;
		bh->next = _bh->next;
		free(bh);
		count += sizeof(struct _BUFFER_HEAD_);
	}
	if (bh->status & BH_DIRTY) return count;
	first_free = bh->prev;
	bh->next->prev = NULL;
	free(bh->addr);
	count += bh->dev->cache_size;
	free(bh);
	count += sizeof(struct _BUFFER_HEAD_);
	return count;
}
