/*
	fs/lfs.c
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
#include <lfs.h>
#include <fs.h>
#include <config.h>
#include <mm.h>
#include <buffer.h>
#include <time.h>
#include <lava.h>

#define UNVAILD_INODE		1
#define UNVAILD_IBLOCK		1
#define EATTR_MONOPOLIZE	1

struct LFS_FLIST_T{
	char mask[64];
	struct LFS_FLIST_T * list[512-8];
};
struct LFS_FLIST_L{
	char mask[64];
	struct _STREAM_ * flist[512-8];
};

struct LFS_DATA {
	LPSTREAM part;
	u8 vaild;
    u8 sec_per_block_shift;
	u8 map_updating;
	struct _BUFFER_HEAD_ * node;//0
	struct _BUFFER_HEAD_ * root;//1
	struct _BUFFER_HEAD_ * bad;//2
	struct _BUFFER_HEAD_ * map;//3
	struct _BUFFER_HEAD_* head;
	LPSTREAM * file_table;
	wchar_t * map_path;
	wchar_t _map_path[24];
	struct _SIMPLE_PATH_ my_path;
	struct _PART_INFO_ info;
	struct _SIMPLE_PATH_ f_path;
	FCPEB fc;
	FSCTRL fs;
};

#define BH_NEXT(_bh)	((struct _BUFFER_HEAD_ *)((_bh)->edata1))
#define BH_SON(_bh)		((struct _BUFFER_HEAD_ *)((_bh)->edata2))

static uint32_t lfs_inter_blockv2r(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* _node, uint32_t vblock);
static int lfs_inter_list(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* _node, uint32_t vblock, uint32_t rblock);
static int lfs_simple_read(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* node, uint64_t off, void* ptr, uint32_t cnt);
static int lfs_simple_write(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* node, uint64_t off, void* ptr, uint32_t cnt);
static uint32_t lfs_try_create_dir(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* _node, wchar_t* name, size_t name_len,uint64_t off);
static uint32_t lfs_try_create_file(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* _node, wchar_t* name, size_t name_len,uint64_t off);

#define lfs_inter_bread(_lfs,_iblock)			bread((_lfs)->part, ((u64)(_iblock)) << (_lfs)->sec_per_block_shift)
#define lfs_inter_cache_bwrite(_lfs,_iblock)	cache_bwrite((_lfs)->part, ((u64)(_iblock)) << (_lfs)->sec_per_block_shift)

static uint32_t lfs_inter_alloc_block(struct LFS_DATA * lfs){
	struct LFS_HEAD* head;
	uint32_t res,_res;
	uint32_t vblock;
	uint32_t rblock;
	uint32_t byte_off;
	uint32_t bit_off;
	uint64_t * bit_map,_bit_map,mask;
	struct _BUFFER_HEAD_* bh;
	struct LFS_NODE* node;

	head = lfs->head->addr;
	while(spin_try_lock_bit(&lfs->map_updating,0)) wait(0);
	_res = res = head->first_free_block;
	if (!head->free_block_count) return 0;
	bit_off = res & 0x07;
	res >>= 3;
	byte_off = res & 0x0fff;
	node = lfs->map->addr;
	if (node->attr & NODE_ATTR_DATA && byte_off < 64) {
		bdirty(lfs->map);
		node->data[byte_off] |= 1 << bit_off;
		bit_map = (uint64_t*)(node->data);
		byte_off >>= 3;
		while (byte_off < 64 / 8) {
			if (~bit_map[byte_off]) {
				_bit_map = bit_map[byte_off];
				mask = 1;
				for (bit_off = 0;bit_off < 64;bit_off++) {
					if (!(_bit_map & mask)) break;
					mask <<= 1;
				}
				bit_off |= byte_off << 6;
				bdirty(lfs->head);
				head->first_free_block = bit_off;
				head->free_block_count--;
				spin_unlock_bit(&lfs->map_updating,0);
				return _res;
			}
			byte_off++;
		}
		if (head->free_block_count < 2) {
			node->data[byte_off] &= ~(1 << bit_off);
			head->first_free_block = _res;
			spin_unlock_bit(&lfs->map_updating,0);
			return 0;
		}
		bh = lfs_inter_bread(lfs, 64 * 8);
		if (!bh) {
			head->first_free_block = _res;
			spin_unlock_bit(&lfs->map_updating,0);
			return 0;
		}
		bdirty(bh);
		memset(bh->addr, 0, 4096);
		memcpy(bh->addr, node->data, 64);
		memset(node->data, 0, 64);
		node->attr &= ~NODE_ATTR_DATA;
		node->block[0] = 64 * 8;
		node->file_size = 4096;
		bit_map = bh->addr;
		bit_map[64 / 8] = 0x01;
		bdirty(lfs->head);
		head->first_free_block = 64 * 8 + 1;
		head->free_block_count -= 2;
		bfree(bh);
		spin_unlock_bit(&lfs->map_updating,0);
		return _res;
	}
	res >>= 12;
	vblock = res;
	rblock = lfs_inter_blockv2r(lfs, lfs->map, vblock);
	bh = lfs_inter_bread(lfs, rblock);
	bdirty(bh);
	((uint8_t*)(bh->addr))[byte_off] |= 1 << bit_off;
	bit_map = bh->addr;
	byte_off >>= 3;
	while (byte_off < 4096 / 8) {
		if (~bit_map[byte_off]) {
			_bit_map = bit_map[byte_off];
			bfree(bh);
			mask = 1;
			for (bit_off = 0;bit_off < 64;bit_off++) {
				if (!(_bit_map & mask)) break;
				mask <<= 1;
			}
			bit_off |= byte_off << 6;
			bit_off |= vblock << 15;
			bdirty(lfs->head);
			head->first_free_block = bit_off;
			head->free_block_count--;
			spin_unlock_bit(&lfs->map_updating,0);
			return _res;
		}
		byte_off++;
	}
	while (1) {
		vblock++;
		if (node->file_size <= (uint64_t)vblock * 4096) {
			lfs_inter_list(lfs, lfs->map, vblock - 1, 0xffffffff);
			memset(bh->addr, 0, 4096);
			rblock = vblock * 4096 * 8;
			bdirty(lfs->map);
			node->file_size += 4096;
			lfs_inter_list(lfs, lfs->map, vblock, rblock);
			bdirty(lfs->head);
			head->first_free_block = vblock * 4096 * 8;
			spin_unlock_bit(&lfs->map_updating,0);
			return _res;
		}
		bfree(bh);
		rblock = lfs_inter_blockv2r(lfs, lfs->map, vblock);
		if (rblock == 0xffffffff) continue;
		bh = lfs_inter_bread(lfs, rblock);
		bit_map = bh->addr;
		for (byte_off = 0;byte_off < 4096 / 8;byte_off++) {
			if (~bit_map[byte_off]) {
				_bit_map = bit_map[byte_off];
				bfree(bh);
				mask = 1;
				for (bit_off = 0;bit_off < 64;bit_off++) {
					if (!(_bit_map & mask)) break;
					mask <<= 1;
				}
				bit_off |= byte_off << 6;
				bit_off |= vblock << 15;
				bdirty(lfs->head);
				head->first_free_block = bit_off;
				head->free_block_count--;
				spin_unlock_bit(&lfs->map_updating,0);
				return _res;
			}
		}
	}
}
static uint32_t lfs_inter_alloc_node(struct LFS_DATA* lfs) {
	struct LFS_HEAD* head;
	uint32_t res;
	uint32_t index;
	uint32_t vblock;
	uint32_t rblock;
	struct _BUFFER_HEAD_* bh;

	head = lfs->head->addr;
	res = xaddd(&head->first_free_node,1);
	if(!res) return 0;
	res = head->first_free_node;
	bdirty(lfs->head);
	index = res % (4096 / 128);
	vblock = res / (4096 / 128);
	rblock = lfs_inter_blockv2r(lfs, lfs->node, vblock);
	if (!rblock) {
		rblock = lfs_inter_alloc_block(lfs);
		if (!rblock) {
			head->first_free_node = 0;
			return 0;
		}
		lfs_inter_list(lfs, lfs->node, vblock, rblock);
		bh = lfs_inter_bread(lfs, rblock);
		bdirty(bh);
		memset(bh->addr, 0, 4096);
		bfree(bh);
	}
	head->first_free_node++;
	return res;
}
static uint32_t lfs_inter_blockv2r(struct LFS_DATA * lfs,struct _BUFFER_HEAD_ * _node,uint32_t vblock) {
	uint32_t t1,t2,t3;
	uint32_t iblock;
	struct _BUFFER_HEAD_* bh;
	struct LFS_NODE* node;

	node = _node->addr;
	if (vblock < NODE_ZONE_32_DIRECT_CNT) {
		if (!node->block[vblock]) return 0;
		return node->block[vblock];
	}
	vblock -= NODE_ZONE_32_DIRECT_CNT;
	if (vblock < 1024) {
		if (!node->block[NODE_ZONE_32_L1]) return 0;
		bh = lfs_inter_bread(lfs, node->block[NODE_ZONE_32_L1]);
		iblock = ((uint32_t*)(bh->addr))[vblock];
		bfree(bh);
		return iblock;
	}
	vblock -= 1024;
	t3 = vblock & 1023;
	vblock >>= 10;
	if (vblock < 1024) {
		if (!node->block[NODE_ZONE_32_L2]) return 0;
		bh = lfs_inter_bread(lfs, node->block[NODE_ZONE_32_L2]);
		iblock = ((uint32_t*)(bh->addr))[vblock];
		bfree(bh);
		if (!iblock) return 0;
		bh = lfs_inter_bread(lfs, iblock);
		iblock = ((uint32_t*)(bh->addr))[t3];
		bfree(bh);
		return iblock;
	}
	vblock -= 1024;
	t2 = vblock & 1023;
	vblock >>= 10;
	if (vblock < 1024) {
		if (!node->block[NODE_ZONE_32_L3]) return 0;
		bh = lfs_inter_bread(lfs, node->block[NODE_ZONE_32_L3]);
		iblock = ((uint32_t*)(bh->addr))[vblock];
		bfree(bh);
		if (!iblock) return 0;
		bh = lfs_inter_bread(lfs, iblock);
		iblock = ((uint32_t*)(bh->addr))[t2];
		bfree(bh);
		if (!iblock) return 0;
		bh = lfs_inter_bread(lfs, iblock);
		iblock = ((uint32_t*)(bh->addr))[t3];
		bfree(bh);
		return iblock;
	}
	vblock -= 1024;
	t1 = vblock & 1023;
	vblock >>= 10;
	if (!node->block[NODE_ZONE_32_L4]) return 0;
	bh = lfs_inter_bread(lfs, node->block[NODE_ZONE_32_L4]);
	iblock = ((uint32_t*)(bh->addr))[vblock];
	bfree(bh);
	if (!iblock) return 0;
	bh = lfs_inter_bread(lfs, iblock);
	iblock = ((uint32_t*)(bh->addr))[t1];
	bfree(bh);
	if (!iblock) return 0;
	bh = lfs_inter_bread(lfs, iblock);
	iblock = ((uint32_t*)(bh->addr))[t2];
	bfree(bh);
	if (!iblock) return 0;
	bh = lfs_inter_bread(lfs, iblock);
	iblock = ((uint32_t*)(bh->addr))[t3];
	bfree(bh);
	return iblock;
}
static int lfs_inter_list(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* _node, uint32_t vblock,uint32_t rblock) {
	uint32_t t1, t2, t3;
	uint32_t iblock;
	struct _BUFFER_HEAD_* bh,*_bh;
	struct LFS_NODE* node;
	int ret;

	node = _node->addr;
	if (vblock < NODE_ZONE_32_DIRECT_CNT) {
		if(node->block[vblock]) return -1;
		if(cmpxchg4b(node->block[vblock],0,rblock,NULL)) return -1;
		bdirty(_node);
		return 0;
	}
	vblock -= NODE_ZONE_32_DIRECT_CNT;
	if (vblock < 1024) {
		if (!node->block[NODE_ZONE_32_L1] && 
			!cmpxchg4b(node->block + NODE_ZONE_32_L1,0,UNVAILD_IBLOCK,NULL)){
			bdirty(_node);
			iblock = lfs_inter_alloc_block(lfs);
			bh = lfs_inter_cache_bwrite(lfs, iblock);
			bdirty(bh);
			memset(bh->addr, 0, 4096);
			((uint32_t*)(bh->addr))[vblock] = rblock;
			node->block[NODE_ZONE_32_L1] = iblock;
			bfree(bh);
			return 0;
		}
		while((iblock = node->block[NODE_ZONE_32_L1]) == UNVAILD_IBLOCK) wait(0);
		bh = lfs_inter_bread(lfs, iblock);
		ret = cmpxchg4b(vblock + (uint32_t*)(bh->addr),0,rblock,NULL);
		if (!ret) bdirty(bh);
		bfree(bh);
		return ret;
	}
	vblock -= 1024;
	t3 = vblock & 1023;
	vblock >>= 10;
	if (vblock < 1024) {
		if(!node->block[NODE_ZONE_32_L2] && 
			!cmpxchg4b(node->block + NODE_ZONE_32_L2,0,UNVAILD_IBLOCK,NULL)){
			bdirty(_node);
			iblock =  lfs_inter_alloc_block(lfs);
			bh = lfs_inter_bread(lfs, iblock);
			bdirty(bh);
			memset(bh->addr, 0, 4096);
			node->block[NODE_ZONE_32_L2] = iblock;
		}
		else{
			while((iblock = node->block[NODE_ZONE_32_L2]) == UNVAILD_IBLOCK) wait(0);
			bh = lfs_inter_bread(lfs,iblock);
		}
		if(!((volatile uint32_t *)(bh->addr))[vblock] && 
			!cmpxchg4b(((volatile uint32_t *)(bh->addr)) + vblock,0,UNVAILD_IBLOCK,NULL)){
			bdirty(bh);
			iblock = lfs_inter_alloc_block(lfs);
			_bh = lfs_inter_bread(lfs,iblock);
			bdirty(_bh);
			memset(_bh->addr,0,4096);
			((volatile uint32_t *)(bh->addr))[vblock] = iblock;
		}
		else{
			while((iblock = node->block[vblock]) == UNVAILD_IBLOCK) wait(0);
			_bh = lfs_inter_bread(lfs,iblock);
		}
		bfree(bh);
		bh = _bh;
		ret = cmpxchg4b(((uint32_t*)(bh->addr)) + t3,0,rblock,NULL);
		if(!ret) bdirty(bh);
		bfree(bh);
		return ret;
	}
	vblock -= 1024;
	t2 = vblock & 1023;
	vblock >>= 10;
	if (vblock < 1024) {
		if(!node->block[NODE_ZONE_32_L3] && 
			!cmpxchg4b(node->block + NODE_ZONE_32_L3,0,UNVAILD_IBLOCK,NULL)){
			bdirty(_node);
			iblock =  lfs_inter_alloc_block(lfs);
			bh = lfs_inter_bread(lfs, iblock);
			bdirty(bh);
			memset(bh->addr, 0, 4096);
			node->block[NODE_ZONE_32_L3] = iblock;
		}
		else{
			while((iblock = node->block[NODE_ZONE_32_L3]) == UNVAILD_IBLOCK) wait(0);
			bh = lfs_inter_bread(lfs,iblock);
		}
		if(!((volatile uint32_t *)(bh->addr))[vblock] && 
			!cmpxchg4b(((volatile uint32_t *)(bh->addr)) + vblock,0,UNVAILD_IBLOCK,NULL)){
			bdirty(bh);
			iblock = lfs_inter_alloc_block(lfs);
			_bh = lfs_inter_bread(lfs,iblock);
			bdirty(_bh);
			memset(_bh->addr,0,4096);
			((volatile uint32_t *)(bh->addr))[vblock] = iblock;
		}
		else{
			while((iblock = node->block[vblock]) == UNVAILD_IBLOCK) wait(0);
			_bh = lfs_inter_bread(lfs,iblock);
		}
		bfree(bh);
		bh = _bh;
		if(!((volatile uint32_t *)(bh->addr))[t2] && 
			!cmpxchg4b(((volatile uint32_t *)(bh->addr)) + t2,0,UNVAILD_IBLOCK,NULL)){
			bdirty(bh);
			iblock = lfs_inter_alloc_block(lfs);
			_bh = lfs_inter_bread(lfs,iblock);
			bdirty(_bh);
			memset(_bh->addr,0,4096);
			((volatile uint32_t *)(bh->addr))[t2] = iblock;
		}
		else{
			while((iblock = node->block[t2]) == UNVAILD_IBLOCK) wait(0);
			_bh = lfs_inter_bread(lfs,iblock);
		}
		bfree(bh);
		bh = _bh;
		ret = cmpxchg4b(((uint32_t*)(bh->addr)) + t3,0,rblock,NULL);
		if(!ret) bdirty(bh);
		bfree(bh);
		return ret;
	}
	vblock -= 1024;
	t1 = vblock & 1023;
	vblock >>= 10;
	if(!node->block[NODE_ZONE_32_L4] && 
		!cmpxchg4b(node->block + NODE_ZONE_32_L4,0,UNVAILD_IBLOCK,NULL)){
		bdirty(_node);
		iblock =  lfs_inter_alloc_block(lfs);
		bh = lfs_inter_bread(lfs, iblock);
		bdirty(bh);
		memset(bh->addr, 0, 4096);
		node->block[NODE_ZONE_32_L4] = iblock;
	}
	else{
		while((iblock = node->block[NODE_ZONE_32_L4]) == UNVAILD_IBLOCK) wait(0);
		bh = lfs_inter_bread(lfs,iblock);
	}
	if(!((volatile uint32_t *)(bh->addr))[vblock] && 
		!cmpxchg4b(((volatile uint32_t *)(bh->addr)) + vblock,0,UNVAILD_IBLOCK,NULL)){
		bdirty(bh);
		iblock = lfs_inter_alloc_block(lfs);
		_bh = lfs_inter_bread(lfs,iblock);
		bdirty(_bh);
		memset(_bh->addr,0,4096);
		((volatile uint32_t *)(bh->addr))[vblock] = iblock;
	}
	else{
		while((iblock = node->block[vblock]) == UNVAILD_IBLOCK) wait(0);
		_bh = lfs_inter_bread(lfs,iblock);
	}
	bfree(bh);
	bh = _bh;
	if(!((volatile uint32_t *)(bh->addr))[t1] && 
		!cmpxchg4b(((volatile uint32_t *)(bh->addr)) + t1,0,UNVAILD_IBLOCK,NULL)){
		bdirty(bh);
		iblock = lfs_inter_alloc_block(lfs);
		_bh = lfs_inter_bread(lfs,iblock);
		bdirty(_bh);
		memset(_bh->addr,0,4096);
		((volatile uint32_t *)(bh->addr))[t1] = iblock;
	}
	else{
		while((iblock = node->block[t1]) == UNVAILD_IBLOCK) wait(0);
		_bh = lfs_inter_bread(lfs,iblock);
	}
	bfree(bh);
	bh = _bh;
	if(!((volatile uint32_t *)(bh->addr))[t2] && 
		!cmpxchg4b(((volatile uint32_t *)(bh->addr)) + t2,0,UNVAILD_IBLOCK,NULL)){
		bdirty(bh);
		iblock = lfs_inter_alloc_block(lfs);
		_bh = lfs_inter_bread(lfs,iblock);
		bdirty(_bh);
		memset(_bh->addr,0,4096);
		((volatile uint32_t *)(bh->addr))[t2] = iblock;
	}
	else{
		while((iblock = node->block[t2]) == UNVAILD_IBLOCK) wait(0);
		_bh = lfs_inter_bread(lfs,iblock);
	}
	bfree(bh);
	bh = _bh;
	ret = cmpxchg4b(((uint32_t*)(bh->addr)) + t3,0,rblock,NULL);
	if(!ret) bdirty(bh);
	bfree(bh);
	return ret;
}
static int lfs_inter_free_block(struct LFS_DATA* lfs,uint32_t iblock) {
	struct LFS_HEAD* head;
	uint32_t byte_off;
	uint32_t bit_off;
	uint32_t rblock,_rblock;
	struct _BUFFER_HEAD_* bh;
	struct LFS_NODE* node;
	uint32_t busy_count;
	int64_t* bit_map,_bit_map;
	int64_t mask;

	head = lfs->head->addr;
	while(spin_try_lock_bit(&lfs->map_updating,0)) wait(0);
	bdirty(lfs->head);
	head->free_block_count++;
	if (head->first_free_block > iblock) 
		head->first_free_block = iblock;
	node = lfs->map->addr;
	bit_off = iblock & 0x07;
	iblock >>= 3;
	byte_off = iblock & 0x0fff;
	iblock >>= 12;
	if (node->file_size <= 64 && byte_off < 64) {
		bdirty(lfs->map);
		node->data[byte_off] &= ~(1 << bit_off);
		spin_unlock_bit(&lfs->map_updating,0);
		return 0;
	}
	rblock = lfs_inter_blockv2r(lfs, lfs->map, iblock);
	bh = lfs_inter_bread(lfs, rblock);
	bdirty(bh);
	((uint8_t*)(bh->addr))[byte_off] &= ~(1 << bit_off);
	bit_map = bh->addr;
	busy_count = 0;
	rblock = 0;
	for (byte_off = 0;byte_off < 4096 / 8;byte_off++) {
		if (bit_map[byte_off]) {
			_bit_map = bit_map[byte_off];
			mask = 1;
			for (bit_off = 0;bit_off < 64;bit_off++) {
				if (_bit_map & mask) {
					busy_count++;
					if (busy_count >= 2) {
						bfree(bh);
						spin_unlock_bit(&lfs->map_updating,0);
						return 0;
					}
					_rblock = (iblock << 15) | (byte_off << 6) | (bit_off);
				}
				mask <<= 1;
			}
		}
	}
	if (!busy_count) {
		lfs_inter_list(lfs, lfs->map, iblock, 0);
		lfs_inter_free_block(lfs, rblock);
	}
	else {
		if (_rblock == rblock) {
			lfs_inter_list(lfs, lfs->map, iblock, 0);
			head->free_block_count++;
			if (head->first_free_block > rblock)
				head->first_free_block = rblock;
		}
	}
	bfree(bh);
	spin_unlock_bit(&lfs->map_updating,0);
	return 0;
}
static struct _BUFFER_HEAD_* lfs_inter_nread(struct LFS_DATA* lfs, uint32_t inode) {
	uint32_t vblock, boff;
	uint32_t rblock;
	struct _BUFFER_HEAD_* bh,*_bh;

	boff = inode & (4096/128 - 1);
	vblock = inode /= 4096 / 128;
	rblock = lfs_inter_blockv2r(lfs, lfs->node, vblock);
	if (!rblock) return NULL;
	bh = lfs_inter_bread(lfs, rblock);
	if (!bh) return NULL;
	_bh = hl_blink(bh, boff * 128, 128);
	bfree(bh);
	return _bh;
}
static struct _BUFFER_HEAD_ * lfs_inter_syn_nread(struct LFS_DATA * lfs,uint32_t inode){
	uint32_t vblock, boff;
	uint32_t rblock;
	struct _BUFFER_HEAD_* bh,*_bh;

	boff = inode & (4096/128 - 1);
	vblock = inode /= 4096 / 128;
	rblock = lfs_inter_blockv2r(lfs, lfs->node, vblock);
	if (!rblock) return NULL;
	bh = lfs_inter_bread(lfs, rblock);
	if (!bh) return NULL;
	_bh = syn_hl_blink(bh, boff * 128, 128);
	bfree(bh);
	return _bh;
}
static LPSTREAM lfs_open_file(wchar_t * name,u64 mode, struct _SIMPLE_PATH_* _path) {
	struct LFS_DATA* lfs;
	wchar_t* path, * tpath;
	struct LFS_DIR dir;
	wchar_t rname[256];
	size_t name_len;
	uint64_t off;
	LPSTREAM file;
	struct _BUFFER_HEAD_* node;
	struct _BUFFER_HEAD_* _cur_path;
	uint32_t inode;
	uint32_t i,j,k;

	wprintk(L"lfs_open_file():%s.\n",name);
	lfs = _path->fs->data;
	_cur_path = _path->data;
	inode = ((struct LFS_NODE*)(_cur_path->addr))->inode;
	path = name;
	while (1) {
		name = path;
		tpath = wcschr(path, L'\\');
		if (!tpath) break;
		name_len = tpath - path;
		if (name_len >= 256) return NULL;
		node = lfs_inter_nread(lfs,inode);
		path = tpath + 1;
		off = 0;
retry_scan_folder:
		while (off < ((struct LFS_NODE*)(node->addr))->file_size) {
			if (lfs_simple_read(lfs, node, off, &dir, sizeof(struct LFS_DIR))) {
				bfree(node);
				return NULL;
			}
			if (dir.name_len == name_len && dir.attr & DIR_ATTR_DIR && dir.attr & DIR_ATTR_EXIST) {
				off += sizeof(struct LFS_DIR);
				if (lfs_simple_read(lfs, node, off, rname, dir.name_len * sizeof(wchar_t))) {
					bfree(node);
					return NULL;
				}
				rname[name_len] = 0;
				if (!wcsncmp(rname, name, name_len)) break;
			}
			else off += sizeof(struct LFS_DIR);
			off += ((dir.name_len * sizeof(wchar_t) + 0x0f) & ~0x0f);
		}
		if (off >= ((struct LFS_NODE*)(node->addr))->file_size) {
			inode = lfs_try_create_dir(lfs, node, name, name_len,off);
			if(!inode) goto retry_scan_folder;
		}
		else inode = dir.node;
		bfree(node);
	}
	name_len = wcslen(name);
	if (name_len >= 256) return NULL;
	node = lfs_inter_nread(lfs,inode);
	off = 0;
retry_scan_file:
	while (off < ((struct LFS_NODE*)(node->addr))->file_size) {
		if (lfs_simple_read(lfs, node, off, &dir, sizeof(struct LFS_DIR))) {
			bfree(node);
			return NULL;
		}
		if (dir.name_len == name_len && !(dir.attr & DIR_ATTR_DIR) && dir.attr & DIR_ATTR_EXIST) {
			off += sizeof(struct LFS_DIR);
			if (lfs_simple_read(lfs, node, off, rname, dir.name_len * sizeof(wchar_t))) {
				bfree(node);
				return NULL;
			}
			rname[name_len] = 0;
			if (!wcsncmp(rname, name, name_len)) break;
		}
		else off += sizeof(struct LFS_DIR);
		off += ((dir.name_len * sizeof(wchar_t) + 0x0f) & ~0x0f);
	}
	if (off >= ((struct LFS_NODE*)(node->addr))->file_size) {
		if (mode & FS_OPEN_N_CRATE) {
			bfree(node);
			return NULL;
		}
		inode = lfs_try_create_file(lfs, node, name, name_len,off);
		if(!inode) goto retry_scan_file;
	}
	else {
		if (mode & FS_OPEN_N_REPLACE) {
			bfree(node);
			return NULL;
		}
		inode = dir.node;
	}
	bfree(node);
	node = lfs_inter_syn_nread(lfs, inode);
	if (!node) return NULL;
	
	file = kmalloc(sizeof(STREAM),0);
	if (!file) {
		bfree(node);
		return NULL;
	}
	i = insert_file(lfs->file_table,file);
	memset(file, 0, sizeof(STREAM));
	file->edata.idata[2] = i;
	file->fc = &lfs->fc;
	file->mode = (int)mode;
	file->edata.pdata[0] = node;
	return file;
}
static int lfs_close(LPSTREAM file) {
	struct _BUFFER_HEAD_* node;
	struct LFS_DATA * lfs;
	struct LFS_BUF_EDATA * edata;
	uint32_t i;

	lfs = file->fc->data;
	node = file->edata.pdata[0];
	if(remove_file(lfs->file_table,file,file->edata.idata[2]))
		return -1;
	if(file->mode & FS_OPEN_MONOPOLIZE)
		lock_andq(&node->edata.idata[1],~FS_OPEN_MONOPOLIZE);
	bfree(node);
	kfree(file);
	return -1;
}
static int lfs_read(LPSTREAM file, size_t count, void * buf) {
	struct _BUFFER_HEAD_* node;
	struct LFS_DATA* lfs;
	int ret;
	struct LFS_NODE* _node;

	lfs = file->fc->data;
	node = file->edata.pdata[0];
	_node = node->addr;
	ret = lfs_simple_read(lfs, node, file->read_pos, buf, (uint32_t)count);
	file->read_pos += count;
	if (_node->file_size < file->read_pos) {
		bdirty(node);
		_node->file_size = file->read_pos;
	}
	return ret;
}
static int lfs_write(LPSTREAM file, size_t count, void* buf) {
	struct _BUFFER_HEAD_* node;
	struct LFS_DATA* lfs;
	int ret;
	struct LFS_NODE* _node;

	lfs = file->fc->data;
	node = file->edata.pdata[0];
	_node = node->addr;
	if (node->edata.idata[0] & EATTR_MONOPOLIZE && !(file->mode & FS_OPEN_MONOPOLIZE)) return -1;
	ret = lfs_simple_write(lfs, node, file->write_pos, buf,(uint32_t) count);
	file->write_pos += count;
	if (_node->file_size < file->write_pos) {
		bdirty(node);
		_node->file_size = file->write_pos;
	}
	return ret;
}
static int lfs_seek_g(int64_t Pos, int Org, LPSTREAM file) {
	struct _BUFFER_HEAD_* node;
	struct LFS_NODE* _node;

	node = file->edata.pdata[0];
	_node = node->addr;
	if (Org == SEEK_SET) {
		file->read_pos = Pos;
	}
	else if (Org == SEEK_CUR) {
		file->read_pos += Pos;
	}
	else {
		file->read_pos = _node->file_size + Pos;
	}
	if (_node->file_size < file->read_pos) { 
		bdirty(node);
		_node->file_size = file->read_pos; 
	}
	return 0;
}
static int lfs_seek_p(int64_t Pos, int Org, LPSTREAM file) {
	struct _BUFFER_HEAD_* node;
	struct LFS_NODE* _node;

	node = file->edata.pdata[0];
	_node = node->addr;
	if (Org == SEEK_SET) {
		file->write_pos = Pos;
	}
	else if (Org == SEEK_CUR) {
		file->write_pos += Pos;
	}
	else {
		file->write_pos = _node->file_size + Pos;
	}
	if (_node->file_size < file->write_pos) {
		bdirty(node);
		_node->file_size = file->write_pos;
	}
	return 0;
}
static uint64_t lfs_tell_g(LPSTREAM file) {
	return file->read_pos;
}
static uint64_t lfs_tell_p(LPSTREAM file) {
	return file->write_pos;
}
static int lfs_put(int Element, LPSTREAM file) {
	struct _BUFFER_HEAD_* node;
	struct LFS_DATA* lfs;
	int ret;
	struct LFS_NODE* _node;

	lfs = file->fc->data;
	node = file->edata.pdata[0];
	_node = node->addr;
	if (node->edata.idata[0] & EATTR_MONOPOLIZE && !(file->mode & FS_OPEN_MONOPOLIZE)) return -1;
	ret = lfs_simple_read(lfs, node, file->write_pos, &Element, 1);
	file->write_pos += 1;
	if (_node->file_size < file->write_pos) {
		bdirty(node);
		_node->file_size = file->write_pos;
	}
	return ret;
}
static int lfs_get(LPSTREAM file) {
	struct _BUFFER_HEAD_* node;
	struct LFS_DATA* lfs;
	int ret;
	int Element;
	struct LFS_NODE* _node;

	lfs = file->fc->data;
	node = file->edata.pdata[0];
	_node = node->addr;
	ret = lfs_simple_read(lfs, node, file->read_pos, &Element, 1);
	file->read_pos += 1;
	if (_node->file_size < file->read_pos) {
		bdirty(node);
		_node->file_size = file->read_pos;
	}
	if (ret) return ret;
	return Element;
}
static int64_t lfs_get_size(LPSTREAM file) {
	struct _BUFFER_HEAD_* node;
	struct LFS_NODE* _node;

	node = file->edata.pdata[0];
	_node = node->addr;
	return _node->file_size;
}
static int lfs_set_size(int64_t Size, LPSTREAM file) {
	return -1;
}
static int lfs_info(LPSTREAM file, size_t size, void* p_info) {
	return -1;
}
static FCPEB lfs_fc = {
	.close = lfs_close,
	.read = lfs_read,
	.write = lfs_write,
	.seek_get = lfs_seek_g,
	.seek_put = lfs_seek_p,
	.tell_get = lfs_tell_g,
	.tell_put = lfs_tell_p,
	.get = lfs_get,
	.put = lfs_put,
	.get_size = lfs_get_size,
	.set_size = lfs_set_size,
	.info = lfs_info
};
static FSCTRL lfs_fs = {.open = lfs_open_file};

static int lfs_simple_read(struct LFS_DATA * lfs,struct _BUFFER_HEAD_ * node, uint64_t off, void* ptr, uint32_t cnt) {
	uint32_t vblock;
	uint32_t rblock;
	uint32_t boff;
	struct _BUFFER_HEAD_* bh;
	struct LFS_NODE* _node;

	_node = node->addr;
	if(_node->attr & NODE_ATTR_DATA) {
		if (off < _node->file_size) {
			if (off + cnt <= _node->file_size) memcpy(ptr, off + _node->data, cnt);
			else {
				memcpy(ptr, off + _node->data, _node->file_size - off);
				memset(_node->file_size + (char*)ptr, -1, off + cnt - _node->file_size);
			}
		}
		else memset(ptr, -1, cnt);
		return 0;
	}
	vblock = (uint32_t)(off >> 12);
	boff = (uint32_t)(off & 0xfff);
	rblock = lfs_inter_blockv2r(lfs, node, vblock);
	if (!rblock) return -1;
	bh = lfs_inter_bread(lfs, rblock);
	if (!bh) return -1;
	if (boff) {
		if (cnt < (4096 - boff)) {
			memcpy(ptr, boff + (char*)bh->addr, cnt);
			bfree(bh);
			return 0;
		}
		cnt -= 4096 - boff;
		memcpy(ptr, boff + (char*)bh->addr, 4096 - boff);
		bfree(bh);
		ptr = (void*)(((u64)ptr) + 4096 - boff);
		vblock++;
		rblock = lfs_inter_blockv2r(lfs, node, vblock);
		if (!rblock) return -1;
		bh = lfs_inter_bread(lfs, rblock);
		if (!bh) return -1;
	}
	while (cnt >= 4096) {
		cnt -= 4096;
		memcpy(ptr, bh->addr, 4096);
		bfree(bh);
		ptr = (void*)(((u64)ptr) + 4096);
		if (!cnt) return 0;
		vblock++;
		rblock = lfs_inter_blockv2r(lfs, node, vblock);
		if (!rblock) return -1;
		bh = lfs_inter_bread(lfs, rblock);
		if (!bh) return -1;
	}
	memcpy(ptr, bh->addr, cnt);
	bfree(bh);
	return 0;
}
static int lfs_simple_write(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* node, uint64_t off, void* ptr, uint32_t cnt) {
	uint32_t vblock;
	uint32_t rblock;
	uint32_t boff;
	struct _BUFFER_HEAD_* bh;
	struct LFS_NODE* _node;
	
	_node = node->addr;
	if(_node->attr & NODE_ATTR_DATA){
		if (off + cnt <= NODE_ZONE_32_MAX * 4) {
			bdirty(node);
			memcpy(_node->data + off, ptr, cnt);
			if (off + cnt > _node->file_size) _node->file_size = off + cnt;
			return 0;
		}
		bdirty(node);
		_node->attr &= ~NODE_ATTR_DATA;
		rblock = lfs_inter_alloc_block(lfs);
		if (!rblock) return -1;
		bh = lfs_inter_cache_bwrite(lfs, rblock);
		if (!bh) return -1;
		bdirty(bh);
		memcpy(bh->addr, _node->data, _node->file_size);
		memset(_node->file_size + (char*)(bh->addr), 0, 4096 - _node->file_size);
		bdirty(node);
		memset(_node->data, 0, NODE_ZONE_32_MAX * 4);
		_node->block[0] = rblock;
		if (off + cnt > _node->file_size) _node->file_size = off + cnt;
		bfree(bh);
	}
	vblock = (uint32_t)(off >> 12);
	boff = (uint32_t)(off & 0xfff);
	rblock = lfs_inter_blockv2r(lfs, node, vblock);
	if (!rblock) {
		rblock = lfs_inter_alloc_block(lfs);
		if (!rblock) return -1;
		lfs_inter_list(lfs, node, vblock, rblock);
	}
	bh = lfs_inter_bread(lfs, rblock);
	if (!bh) return -1;
	if (boff) {
		if (cnt <= (4096 - boff)) {
			bdirty(bh);
			memcpy(boff + (char*)bh->addr, ptr, cnt);
			bfree(bh);
			return 0;
		}
		cnt -= 4096 - boff;
		bdirty(bh);
		memcpy(boff + (char*)bh->addr, ptr, 4096 - boff);
		bfree(bh);
		ptr = (void*)(((u64)ptr) + 4096 - boff);
		vblock++;
		rblock = lfs_inter_blockv2r(lfs, node, vblock);
		if (!rblock) {
			rblock = lfs_inter_alloc_block(lfs);
			if (!rblock) return -1;
			lfs_inter_list(lfs, node, vblock, rblock);
		}
		bh = lfs_inter_bread(lfs, rblock);
		if (!bh) return -1;
	}
	while (cnt >= 4096) {
		cnt -= 4096;
		bdirty(bh);
		memcpy(bh->addr, ptr, 4096);
		bfree(bh);
		ptr = (void*)(((u64)ptr) + 4096);
		if (!cnt) return 0;
		vblock++;
		rblock = lfs_inter_blockv2r(lfs, node, vblock);
		if (!rblock) {
			rblock = lfs_inter_alloc_block(lfs);
			if (!rblock) return -1;
			lfs_inter_list(lfs, node, vblock, rblock);
		}
		bh = lfs_inter_bread(lfs, rblock);
		if (!bh) return -1;
	}
	bdirty(bh);
	memcpy(bh->addr, ptr, cnt);
	bfree(bh);
	return 0;
}
static uint32_t lfs_try_create_dir(struct LFS_DATA* lfs, struct _BUFFER_HEAD_ * _node, wchar_t* name,size_t name_len,uint64_t off) {
	struct LFS_DIR dir;
	struct LFS_NODE* node;
	uint32_t iblock;
	struct _BUFFER_HEAD_* bh;
	uint32_t inode;

	node = _node->addr;
    buf_lock(_node,0,sizeof(struct LFS_NODE) - 1);
    if(off != node->file_size){
        buf_unlock(_node,0);
        return 0;
    }
	dir.attr = DIR_ATTR_DIR | DIR_ATTR_EXIST;
	dir.node = lfs_inter_alloc_node(lfs);
	dir.name_len = (uint8_t)name_len;
	time(&dir.create_time);
	off = node->file_size;
	lfs_simple_write(lfs, _node, off, &dir, sizeof(struct LFS_DIR));
	off += sizeof(struct LFS_DIR);
	lfs_simple_write(lfs, _node, off, name, (uint32_t)(name_len * sizeof(wchar_t)));
	bdirty(_node);
	node->file_size = off + +((name_len * sizeof(wchar_t) + 0x0f) & ~0x0f);
	bh = lfs_inter_nread(lfs, dir.node);
	node = bh->addr;
	bdirty(bh);
	memset(node, 0, sizeof(struct LFS_NODE));
	node->access_time = dir.create_time;
	node->create_time = dir.create_time;
	node->modify_time = dir.create_time;
	node->attr = NODE_ATTR_DATA | NODE_ATTR_EXIST | NODE_ATTR_DIR;
	node->file_size = sizeof(struct LFS_DIR) * 2 + 32;
	node->inode = dir.node;
	node->links = 1;
	dir.name_len = 2;
	dir.node = ((struct LFS_NODE*)(bh->addr))->inode;
	memcpy(node->data, &dir, sizeof(struct LFS_DIR));
	memcpy(&node->owner, &LFS_SYS_OWNER_GUID, sizeof(GUID));
	node->data[sizeof(struct LFS_DIR)] = L'.' & 0x0ff;
	node->data[sizeof(struct LFS_DIR) + 1] = (L'.' >> 8) & 0x0ff;
	node->data[sizeof(struct LFS_DIR) + 2] = L'.' & 0x0ff;
	node->data[sizeof(struct LFS_DIR) + 3] = (L'.' >> 8) & 0x0ff;
	dir.name_len = 1;
	dir.node = node->inode;
	memcpy(node->data + sizeof(struct LFS_DIR) + 16, &dir, sizeof(struct LFS_DIR));
	node->data[sizeof(struct LFS_DIR) * 2 + 16] = L'.' & 0x0ff;
	node->data[sizeof(struct LFS_DIR) * 2 + 16 + 1] = (L'.' >> 8) & 0x0ff;
	inode = node->inode;
	bfree(bh);
    buf_unlock(_node,0);
	return inode;
}
static uint32_t lfs_try_create_file(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* _node, wchar_t* name, size_t name_len,uint64_t off) {
	struct LFS_DIR dir;
	struct LFS_NODE* node;
	uint32_t iblock;
	struct _BUFFER_HEAD_* bh;
	uint32_t inode;

	printk("lfs_try_create_file().\n");
	node = _node->addr;
    buf_lock(_node,0,sizeof(struct LFS_NODE) - 1);
    if(off != node->file_size){
        buf_unlock(_node,0);
        return 0;
    }
	dir.attr = DIR_ATTR_EXIST;
	dir.node = lfs_inter_alloc_node(lfs);
	dir.name_len = (uint8_t)name_len;
	time(&dir.create_time);
	lfs_simple_write(lfs, _node, off, &dir, sizeof(struct LFS_DIR));
	off += sizeof(struct LFS_DIR);
	lfs_simple_write(lfs, _node, off, name, (uint32_t)(name_len * sizeof(wchar_t)));
	bdirty(_node);
	node->file_size = off + ((name_len * sizeof(wchar_t) + 0x0f) & ~0x0f);;
	bh = lfs_inter_nread(lfs, dir.node);
	node = bh->addr;
	bdirty(bh);
	memset(node, 0, sizeof(struct LFS_NODE));
	node->access_time = dir.create_time;
	node->create_time = dir.create_time;
	node->modify_time = dir.create_time;
	node->attr = NODE_ATTR_DATA | NODE_ATTR_EXIST;
	memcpy(&node->owner, &LFS_SYS_OWNER_GUID, sizeof(GUID));
	node->file_size = 0;
	node->inode = dir.node;
	node->links = 1;
	inode = node->inode;
	bfree(bh);
    buf_unlock(_node,0);
	return inode;
}

static void lfs_open_call_back(LPSTREAM _file, void* c_data) {unsummon_lfs(c_data);}
int lfs_open(const wchar_t * path){
    struct LFS_DATA * lfs;
    struct LFS_HEAD * head;
    int sec_per_block;

	printk("lfs_open().\n");
    lfs = kmalloc(sizeof(struct LFS_DATA),0);
    memset(lfs,0,sizeof(struct LFS_DATA));
    lfs->part = open(path,FS_OPEN_READ | FS_OPEN_WRITE,lfs,lfs_open_call_back);
    if(!lfs->part){
        kfree(lfs);
        return -1;
    }
	info(lfs->part,sizeof(struct _PART_INFO_),&lfs->info);
    if (lfs->info.cache_block_size < 4096) {
		close(lfs->part);
		kfree(lfs);
		return NULL;
	}
    wprintk(L"open part success:%s,%P.\n",path,path);
    sec_per_block = 4096/lfs->info.logical_sector_size;
    lfs->sec_per_block_shift = BSF(&sec_per_block);
    lfs->head = lfs_inter_bread(lfs, 1);
	if (!lfs->head) {
		return lfs;
	}
	if (buf_lock(lfs->head, 0, 4096)) {
		bfree(lfs->head);
		return lfs;
	}
	head = lfs->head->addr;
    if (head->version != LFS_CUR_VERSION) {
		buf_unlock(lfs->head,0);
		bfree(lfs->head);
		return lfs;
	}
	lfs->node = lfs_inter_bread(lfs, head->node_0_block);
	if (!lfs->node) {
		buf_unlock(lfs->head, 0);
		bfree(lfs->head);
		return lfs;
	}
    lfs->root = hl_blink(lfs->node, sizeof(struct LFS_NODE) * 1, sizeof(struct LFS_NODE));
	lfs->bad = hl_blink(lfs->node, sizeof(struct LFS_NODE) * 2, sizeof(struct LFS_NODE));
	lfs->map = hl_blink(lfs->node, sizeof(struct LFS_NODE) * 3, sizeof(struct LFS_NODE));
	lfs->my_path.data = lfs;
	lfs->my_path.fs = &lfs->fs;
	memcpy(&lfs->fs, &lfs_fs, sizeof(FSCTRL));
	memcpy(&lfs->fc, &lfs_fc, sizeof(FCPEB));
	lfs->fs.data = lfs;
	lfs->fc.data = lfs;
	lfs->my_path.data = lfs->root;
	if(!memcmp(&lfs->info.g_disk,&init_msg.DiskGUID,sizeof(GUID)) && 
		!memcmp(&lfs->info.g_id,&init_msg.PartGUID,sizeof(GUID))){
			wcscpy(lfs->_map_path,L"/system/");
			lfs->map_path = lfs->_map_path;
		}
	else{
		printk("%P,%P.%P,%P.\n",&lfs->info.g_disk,&init_msg.DiskGUID,&lfs->info.g_id,&init_msg.PartGUID);
		wcscpy(lfs->_map_path, L"/fs/lfs/");
		lfs->map_path = lfs->_map_path;
	}
	
	lfs->vaild = 1;
	lfs->file_table = alloc_file_table();
	fs_map(lfs->map_path, &lfs->my_path, &lfs->f_path);
    return 0;
}
int unsummon_lfs(HANDLE _fs) {
	struct LFS_DATA* lfs;
	LPSTREAM file;

	if (!_fs) return -1;
	lfs = _fs;

	if (lfs->vaild) {
		free_file_table(lfs->file_table);
		bfree(lfs->node);
		bfree(lfs->root);
		bfree(lfs->map);
		bfree(lfs->bad);
		buf_unlock(lfs->head, 0);
		bfree(lfs->head);
	}
	fs_unmap(lfs->map_path);
	close(lfs->part);
	kfree(lfs);
	return 0;
}