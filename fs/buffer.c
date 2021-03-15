/*
	fs/buffer.c
	Copyright (C) 2020  Quasimodo

    This program is kfree software: you can redistribute it and/or modify
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
#include <kernel.h>
#include <config.h>
#include <buffer.h>
#include <spinlock.h>
#include <mm.h>
#include <asm.h>
#include <string.h>
#include <lava.h>

#define map_table   ((struct _BUFFER_HEAD_ **)BHLAB)
#define map_mutex   ((char*)BUF_MUTEX_BASE)
#define map_table2  ((struct _BUFFER_HEAD_ **)BHLAB2)
spin_optr_def_arg_bit(Table,map_mutex,0);
spin_optr_struct_member_bit(Head,struct _BUFFER_HEAD_,status,0);
struct bh_stamp_ref{
    struct _BUFFER_HEAD_ * bh;
    u64 stamp;
};
static char map_mutex2[32];
spin_optr_def_arg_bit(Table2,map_mutex2,0);
static struct bh_stamp_ref first_free_head = {NULL,0};

static int inline mapping(HANDLE dev,u64 iblock){
    iblock ^= (u64)dev;
    iblock ^= iblock >> 32;
    iblock ^= iblock >> 16;
    return iblock & 0x0ffff;
}
static int inline mapping2(HANDLE dev,u64 iblock){
    iblock ^= (u64)dev;
    iblock ^= iblock >> 32;
    iblock ^= iblock >> 16;
    iblock ^= iblock >> 8;
    return iblock & 0x000ff;
}
static int inline map22(int map) {return (map ^(map >> 8)) & 0x00ff;}
static inline struct _BUFFER_HEAD_ * alloc_head(){
    struct bh_stamp_ref ref,ref2;
    while(first_free_head.bh){
        ref.bh = first_free_head.bh;
        ref.stamp = first_free_head.stamp;
        ref2.bh = ref.bh->next;
        ref2.stamp = ref.stamp + 1;
        if(!cmpxchg16b(&first_free_head,&ref,&ref2,NULL)) return ref.bh;
    }
    return kmalloc(sizeof(struct _BUFFER_HEAD_),0);
}
static inline void free_head(struct _BUFFER_HEAD_ * bh){
    struct bh_stamp_ref ref,ref2;
    ref2.bh = bh;
    do{
		bh->next = ref.bh = first_free_head.bh;
        ref.stamp = first_free_head.stamp;
        ref2.stamp = ref.stamp + 1;
    } while(cmpxchg16b(&first_free_head,&ref,&ref2,NULL));
}
static inline struct _BUFFER_HEAD_ * search_head(struct _BUFFER_HEAD_ * bh,struct _LL_BLOCK_DEV_ * dev,u64 iblock,u32 byte_off){
    if(!byte_off){
        for(;bh;bh = bh->next){
            if (bh->dev != dev || bh->iblock != iblock) continue;
            if(bh->byte_off) bh = bh->parent;
            bh->ref_count++;
            return bh;
        }
        return NULL;
    }
    else{
        for(;bh;bh = bh->next){
            if (bh->dev != dev || bh->iblock != iblock || bh->byte_off != byte_off) continue;
            bh->ref_count++;
            return bh;
        }
        return NULL;
    }
}
static inline void insert_list(struct _BUFFER_HEAD_ ** head,struct _BUFFER_HEAD_ * bh){
    bh->prev = NULL;
    bh->next = *head;
    if(*head) (*head)->prev = bh;
    *head = bh;
}
static inline void remove_list(struct _BUFFER_HEAD_ ** head,struct _BUFFER_HEAD_ * bh){
    if(bh->prev) bh->prev->next = bh->next;
    else *head = bh->next;
    if(bh->next) bh->next->prev = bh->prev;
}
static void load_block(struct _BUFFER_HEAD_ * bh,struct _LL_BLOCK_DEV_ * dev){
    LPTHREAD thread;
    int ie,err;

    ie = IE();
    cli();
    LockHead(bh);
    if(!(bh->status & BH_VAILD)){
        if(bh->status & BH_UPDATEING){
            thread = bh->wait;
            bh->wait = GetCurThread();
            bh->wait->flag = TF_BLOCK;
            UnlockHead(bh);
            schedule_imm();
            if(ie) sti();
            if(thread) thread->flag = TF_ACTIVE;
        }
        else{
            bh->status |= BH_UPDATEING;
            UnlockHead(bh);
            if(ie) sti();
            err = dev->read_block(bh);
            cli();
            LockHead(bh);
            if(!err) bh->status |= BH_VAILD;
            bh->status &= ~BH_UPDATEING;
            if(bh->wait) {
                bh->wait->flag = TF_ACTIVE;
                bh->wait = NULL;
            }
            UnlockHead(bh);
            if(ie) sti();
        }
    }
    else{
        UnlockHead(bh);
        if(ie) sti();
    }
}
static struct _BUFFER_HEAD_ * raw_buf_alloc(struct _LL_BLOCK_DEV_ * dev,u64 iblock){
    int map,map2,ie;
	struct _BUFFER_HEAD_* bh;

	map = mapping(dev, iblock);
    ie = IE();
    cli();
    LockTable(map);
    bh = search_head(map_table[map],dev,iblock,0);
    if(bh){
        UnlockTable(map);
        if(ie) sti();
		return bh;
    }
    map2 = map22(map);
    LockTable2(map2);
    bh = search_head(map_table2[map2],dev,iblock,0);
    if(bh){
        remove_list(map_table2 + map2,bh);
        UnlockTable2(map2);
        insert_list(map_table + map,bh);
        UnlockTable(map);
        if(ie) sti();
		return bh;
    }
    UnlockTable2(map2);
    bh = alloc_head();
	memset(bh, 0, sizeof(struct _BUFFER_HEAD_));
	bh->dev = dev;
	bh->ref_count = 1;
	bh->addr = PADDR2V(get_free_page(0,0,dev->bits));
	bh->iblock = iblock;
	bh->buf_size = dev->cache_size;
	//bh->byte_off = 0;
    insert_list(map_table + map,bh);
    UnlockTable(map);
    if(ie) sti();
	return bh;
}
struct _BUFFER_HEAD_* ll_bread(struct _LL_BLOCK_DEV_* dev, u64 iblock) {
	struct _BUFFER_HEAD_* bh;

	if (!dev) return NULL;
	bh = raw_buf_alloc(dev, iblock);
	if (!(bh->status & BH_VAILD)) load_block(bh,dev);
	return bh;
}
struct _BUFFER_HEAD_* ll_balloc(struct _LL_BLOCK_DEV_* dev, u64 iblock) {
	struct _BUFFER_HEAD_* bh;
    int ie;

	if (!dev) return NULL;
	bh = raw_buf_alloc(dev, iblock);
    ie = IE();
    cli();
	LockHead(bh);
    bh->status |= BH_VAILD;
    UnlockHead(bh);
    if(ie) sti();
	return bh;
}
struct _BUFFER_HEAD_* ml_blink(struct _LL_BLOCK_DEV_* dev, u64 iblock, uint32_t byte_off,uint32_t size) {
	struct _BUFFER_HEAD_* bh,*_bh;
	unsigned int map;
    LPTHREAD thread;
    struct bh_stamp_ref ref,ref2;
    int ie,err;

    //printk("ml_blink:%P,%lld,%d,%d.\n",dev,iblock,byte_off,size);
	if (!dev) return NULL;
	map = mapping(dev, iblock);
    ie = IE();
    cli();
    LockTable(map);
    bh = search_head(map_table[map],dev,iblock,byte_off);
    if(bh){
        UnlockTable(map);
        if(ie) sti();
		return bh;
    }
    UnlockTable(map);
    if(ie) sti();
	_bh = raw_buf_alloc(dev, iblock);
	if (!(_bh->status & BH_VAILD)) load_block(_bh,dev);
	if (!byte_off) return _bh;
	bh = alloc_head();
	memset(bh, 0, sizeof(struct _BUFFER_HEAD_));
	bh->parent = _bh;
	bh->iblock = iblock;
	bh->ref_count = 1;
	bh->addr = byte_off + (char*)(_bh->addr);
	bh->buf_size = size;
	bh->byte_off = byte_off;
	bh->dev = dev;
    cli();
	LockTable(map);
    insert_list(map_table + map,bh);
    UnlockTable(map);
	bh->status |= BH_VAILD;
    if(ie) sti();
	return bh;
}
struct _BUFFER_HEAD_* hl_blink(struct _BUFFER_HEAD_* _bh, uint32_t byte_off, uint32_t size) {
	struct _BUFFER_HEAD_* bh,*unvaild = NULL;
	unsigned int map;
    LPTHREAD thread;
    struct bh_stamp_ref ref,ref2;
    int ie;

	if (!_bh) return NULL;
	map = mapping(_bh->dev, _bh->iblock);
    ie = IE();
    cli();
    LockTable(map);
	if (!byte_off) {
		_bh->ref_count++;
        UnlockTable(map);
         if(ie) sti();
		return _bh;
	}
    if(_bh->parent){
        byte_off += _bh->byte_off;
	    _bh = _bh->parent;
    }
    bh = search_head(map_table[map],_bh->dev,_bh->iblock,byte_off);
    if(bh){
        UnlockTable(map);
        if(ie) sti();
		return bh;
    }
    _bh->ref_count++;
    UnlockTable(map);
    bh = alloc_head();
	memset(bh, 0, sizeof(struct _BUFFER_HEAD_));
	bh->parent = _bh;
	bh->iblock = _bh->iblock;
	bh->ref_count = 1;
	bh->addr = byte_off + (char*)(_bh->addr);
	bh->buf_size = size;
	bh->byte_off = byte_off;
	bh->dev = _bh->dev;
	LockTable(map);
    insert_list(map_table + map,bh);
    UnlockTable(map);
    if(ie) sti();
	bh->status |= BH_VAILD;
	return bh;
}
struct _BUFFER_HEAD_* syn_ml_blink(struct _LL_BLOCK_DEV_ * dev,u64 iblock,uint32_t byte_off,uint32_t size){
    struct _BUFFER_HEAD_* bh,*_bh;
	unsigned int map,map2;
    LPTHREAD thread;
    struct bh_stamp_ref ref,ref2;
    int ie,err;

    //printk("ml_blink:%P,%lld,%d,%d.\n",dev,iblock,byte_off,size);
	if (!dev) return NULL;
	map = mapping(dev, iblock);
    ie = IE();
    cli();
    LockTable(map);
    bh = search_head(map_table[map],dev,iblock,byte_off);
    if(bh){
        UnlockTable(map);
        if(ie) sti();
		return bh;
    }
    _bh = search_head(map_table[map],dev,iblock,0);
    if(!_bh){
        map2 = map22(map);
        LockTable2(map2);
        _bh = search_head(map_table2 + map2,dev,iblock,0);
        if(_bh){
            remove_list(map_table2 + map2,_bh);
            UnlockTable2(map2);
        }
        else{
            _bh = alloc_head();
            memset(_bh,0,sizeof(struct _BUFFER_HEAD_));
            _bh->dev = dev;
	        _bh->ref_count = 1;
	        _bh->addr = PADDR2V(get_free_page(0,0,dev->bits));
	        _bh->iblock = iblock;
	        _bh->buf_size = dev->cache_size;
        }
        insert_list(map_table + map,_bh);
    }
    if(byte_off){
        bh = alloc_head();
	    memset(bh, 0, sizeof(struct _BUFFER_HEAD_));
	    bh->parent = _bh;
	    bh->iblock = iblock;
	    bh->ref_count = 1;
	    bh->addr = byte_off + (char*)(_bh->addr);
	    bh->buf_size = size;
	    bh->byte_off = byte_off;
	    bh->dev = dev;
        bh->status = BH_VAILD;
        insert_list(map_table + map,bh);
    }
    UnlockTable(map);
    if(ie) sti();
    if (!(_bh->status & BH_VAILD)) load_block(_bh,dev);
    if(byte_off) return bh;
    else return _bh;
}
struct _BUFFER_HEAD_* syn_hl_blink(struct _BUFFER_HEAD_* _bh, uint32_t byte_off, uint32_t size){
    struct _BUFFER_HEAD_* bh,*unvaild = NULL;
	unsigned int map;
    LPTHREAD thread;
    struct bh_stamp_ref ref,ref2;
    int ie;

	if (!_bh) return NULL;
	map = mapping(_bh->dev, _bh->iblock);
    ie = IE();
    cli();
    LockTable(map);
	if (!byte_off) {
		_bh->ref_count++;
        UnlockTable(map);
		return _bh;
	}
    if(_bh->parent){
        byte_off += _bh->byte_off;
	    _bh = _bh->parent;
    }
    bh = search_head(map_table[map],_bh->dev,_bh->iblock,byte_off);
    if(bh){
        UnlockTable(map);
        if(ie) sti();
		return bh;
    }
    bh = alloc_head();
	memset(bh, 0, sizeof(struct _BUFFER_HEAD_));
    _bh->ref_count++;
	bh->parent = _bh;
	bh->iblock = _bh->iblock;
	bh->ref_count = 1;
	bh->addr = byte_off + (char*)(_bh->addr);
	bh->buf_size = size;
	bh->byte_off = byte_off;
	bh->dev = _bh->dev;
    insert_list(map_table + map,bh);
    UnlockTable(map);
    if(ie) sti();
	bh->status |= BH_VAILD;
	return bh;
}
void * bealloc(struct _BUFFER_HEAD_ * bh,size_t size,unsigned int align){
    void * res;
    int ie;

    if(!bh) return NULL;
    while(1){
        if(bh->alloc_ptr){
            if(bh->alloc_ptr != (void*)1) return bh->alloc_ptr;
            while(bh->alloc_ptr == (void*)1){
                wait(0);
            }
            return bh->alloc_ptr;
        }
        if(!cmpxchg8b(&bh->alloc_ptr,NULL,(void*)1,NULL)){
            res = kmalloc(size,align);
            memset(res,0,size);
            ie = IE();
            cli();
            LockHead(bh);
            bh->status |= BH_EALLOC;
            bh->alloc_ptr = res;
            UnlockHead(bh);
            if(ie) sti();
            return res;
        }
    }
}
int bfree(struct _BUFFER_HEAD_* bh) {
	int map,map2,ie;
    struct _BUFFER_HEAD_ * _bh;

	if (!bh) return -1;
    if(bh->ref_count <= 0){
        printk("BUG:bfree():Try to free free head.\n");
        stop();

    }
    ie = IE();
	map = mapping(bh->dev, bh->iblock);
    cli();
    LockTable(map);
	bh->ref_count--;
	if (bh->ref_count) {
        UnlockTable(map);
        if(ie) sti();
        return 0;
    }
    remove_list(map_table + map,bh);
	if (bh->parent) {
        _bh = bh->parent;
        free_head(bh);
		bh = _bh;
		bh->ref_count--;
		if (bh->ref_count) {
            UnlockTable(map);
            if(ie) sti();
            return 0;
        }
        remove_list(map_table + map,bh);
	}
    if(bh->status & BH_EALLOC) kfree(bh->alloc_ptr);
	map2 = map22(map);
    LockTable2(map2);
    insert_list(map_table2 + map2,bh);
    UnlockTable2(map2);
    UnlockTable(map);
    if(ie) sti();
	return 0;
}
static int bsync_thread(void * _bh){
    struct _BUFFER_HEAD_* bh;

    bh = _bh;
    printk("write block:%d.\n",bh->iblock);
    bh->dev->write_block(bh);
    free_page(ADDRV2P(NULL,bh->addr));
    kfree(bh);
    return 0;
}
void close_all_disk();
int bsync(struct _LL_BLOCK_DEV_* dev) {
	struct _BUFFER_HEAD_* bh;
	int i;
    int ie;

    ie = IE();
    if(!dev){
        close_all_disk();
        for (i = 0;i < 256;i++) {
            while(map_table2[i]){
                cli();
		        LockTable2(i);
                bh = map_table2[i];
                map_table2[i] = bh->next;
                if(bh->next) bh->next->prev = NULL;
                UnlockTable2(i);
                if(ie) sti();
                bh->next = NULL;
                if(bh->status & BH_DIRTY) {
                    create_thread(NULL,bsync_thread,bh);   
                }
                else{
                    free_page(ADDRV2P(NULL,bh->addr));
                    kfree(bh);
                }
            }
	    }
        for(i = 0;i < 65536;i++)
            for(bh = map_table[i];bh && !bh->byte_off;bh = bh->next)
                printk("Occupyed head:%d.",bh->iblock);
        return 0;
    }
	for (i = 0;i < 256;i++) {
        while(map_table2[i]){
            cli();
		    LockTable2(i);
            bh = map_table2[i];
            map_table2[i] = bh->next;
            UnlockTable2(i);
            if(ie) sti();
            if(bh->status & BH_DIRTY) bh->dev->write_block(bh);
            free_page(ADDRV2P(NULL,bh->addr));
            kfree(bh);
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
    LockHead(bh);
	lock_list = bh->lock_list;
	while (lock_list) {
		mask = 1;
		_mask = lock_list->mask;
		for (i = 0;i < BH_LOCK_LIST_SIZE;i++) {
			if (mask & _mask) {
				if ((lock_list->start[i] >= start && lock_list->end[i] <= start) ||
					lock_list->start[i] <= end && lock_list->end[i] >= end) {
					if (unvaild) unvaild->attr &= 1 << iunvaild;
                    UnlockHead(bh);
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
		lock_list = kmalloc(sizeof(struct _BUFFER_LOCK_ARRAY_),0);
		if (!lock_list) return -1;
		lock_list->mask = 1;
		lock_list->attr = 0;
		lock_list->start[0] = start;
		lock_list->end[0] = end;
		lock_list->next = bh->lock_list;
		bh->lock_list = lock_list;
	}
    UnlockHead(bh);
	return 0;
}
int buf_unlock(struct _BUFFER_HEAD_* bh, uint32_t start) {
	struct _BUFFER_LOCK_ARRAY_* lock_list,*prev = NULL;
	int i, mask, _mask;

	if (!bh) return -1;
	if (bh->parent) {
		start += bh->byte_off;
		bh = bh->parent;
	}
    LockHead(bh);
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
						kfree(lock_list);
					}
                    UnlockHead(bh);
					return 0;
				}
			}
			mask <<= 1;
		}
		prev = lock_list;
	}
    UnlockHead(bh);
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
			if (_mask & mask && lock_list->start[i] <= pos && lock_list->end[i] > pos) 
				return 1;
		prev = lock_list;
	}
	return 0;
}
u64 bclean() {
	struct _BUFFER_HEAD_* bh,*_bh,*__bh;
	u64 count = 0;
    int i;
    int ie;

    ie = IE();
    for(i = 0;i < 256;i++){
        cli();
        LockTable2(i);
        while(map_table2[i]){
            if(map_table2[i]->status & BH_DIRTY) break;
            free_page(ADDRV2P(NULL,map_table2[i]->addr));
            map_table2[i] = map_table2[i]->next;
            kfree(map_table2[i]);
            count += 4096 + sizeof(struct _BUFFER_HEAD_);
        }
        if(!map_table2[i]) continue;
        _bh = map_table2[i];
        bh = _bh->next;
        while(bh){
            if(bh->status & BH_DIRTY) {
                _bh = bh;
                bh = bh->next;
                continue;
            }
            _bh->next = bh->next;
            free_page(ADDRV2P(NULL,bh->addr));
            kfree(map_table2[i]);
            count += 4096 + sizeof(struct _BUFFER_HEAD_);
            bh = _bh->next;
        }
        UnlockTable2(i);
        if(ie) sti();
    }
    bh = xchgq(&first_free_head.bh,(u64)NULL);
    while(bh){
        _bh = bh->next;
        kfree(bh);
        count += sizeof(struct _BUFFER_HEAD_);
        bh = _bh;
    }
	return count;
}
void buf_init(){
    memset(map_table,0,sizeof(void*) * 65536);
    memset(map_mutex,0,65536/8);
    memset(map_table2,0,sizeof(void*) * 256);
    memset(map_mutex2,0,32);
}