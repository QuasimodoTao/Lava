/*
	tools/dop/lfs.c
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

#include "fs.h"
#include "dop.h"
#include "error.h"
#include <stdio.h>
#include <time.h>

#define LFS_CUR_VERSION			0x00010000

#define NODE_ATTR_SYS_READ      0x0001
#define NODE_ATTR_SYS_WRITE     0x0002
#define NODE_ATTR_SYS_EXC       0x0004
#define NODE_ATTR_SYS_DEL       0x0008
#define NODE_ATTR_USR_READ      0x0010
#define NODE_ATTR_USR_WRITE     0x0020
#define NODE_ATTR_USR_EXC       0x0040
#define NODE_ATTR_USR_DEL       0x0080
#define NODE_ATTR_EXIST         0x0100
#define NODE_ATTR_DIR           0x0200
#define NODE_ATTR_DATA          0x0400
#define NODE_ATTR_ENC           0x0800

#define DIR_ATTR_SYS_READ       0x0001
#define DIR_ATTR_SYS_WRITE      0x0002
#define DIR_ATTR_SYS_EXC        0x0004
#define DIR_ATTR_SYS_DEL        0x0008
#define DIR_ATTR_USR_READ       0x0010
#define DIR_ATTR_USR_WRITE      0x0020
#define DIR_ATTR_USR_EXC        0x0040
#define DIR_ATTR_USR_DEL        0x0080
#define DIR_ATTR_EXIST          0x0100
#define DIR_ATTR_DIR            0x0200
#define DIR_ATTR_HIDE           0x0400

#define NODE_ZONE_32_MAX        16
#define NODE_ZONE_32_DIRECT_CNT 12
#define NODE_ZONE_32_L1         12
#define NODE_ZONE_32_L2         13
#define NODE_ZONE_32_L3         14
#define NODE_ZONE_32_L4         15
#define LFS_GPT_TYPE_GUID        (GUID){0xa23a4c32, 0x5650, 0x47c6, { 0xaf,0x40,0x5c,0xd9,0x48,0x78,0xc6,0x71 }}
#define LFS_SYS_OWNER_GUID       (GUID){0xa23a4c32, 0x5650, 0x47c6, { 0xaf,0x40,0x5c,0xd9,0x48,0x78,0xc6,0x71 }}

struct LFS_HEAD {
	int32_t version;//���ڵİ汾��1.0��0x00010000
	int32_t crc32;
	uint32_t head_size;
	uint32_t path_len;
	uint32_t name_len;
	uint32_t first_free_node;
	uint32_t first_free_block;//��һ�����еĿ飬�ڵ��
	uint32_t block_count;//��ǰ�����еĿ�����
	uint32_t node_0_block;//0 node,1 root,2 bad,3 map
	uint32_t free_block_count;//���п�����
	stamp64_t create_time;//��������ʽ����ʱ��
	GUID owner;
	wchar_t name_path[];
};
struct LFS_NODE {//pack 128 bytes
	stamp64_t create_time, access_time;
	stamp64_t modify_time, delete_time;
	uint64_t file_size;
	uint16_t attr, links;
	uint32_t inode;
	GUID owner;
	union {
		uint32_t block[16];
		uint8_t data[64];//������ݴ�СС�ڵ���160�ֽڣ��򱣴������������
	};
};
struct LFS_DIR {//pack align to 16
	uint8_t name_len;
	uint8_t rvd;
	uint16_t attr;
	uint32_t node;
	stamp64_t create_time;
	wchar_t name[];
};

struct LFS_DATA {
	LPDOPSTREAM part;
	int vaild;
	struct _BUFFER_HEAD_ * node;//0
	struct _BUFFER_HEAD_ * root;//1
	struct _BUFFER_HEAD_ * bad;//2
	struct _BUFFER_HEAD_ * map;//3
	SIMPLE_FILE my_path;
	struct _BUFFER_HEAD_* head;
	struct LFS_DATA* prev;
	struct LFS_DATA* next;
	wchar_t map_path[24];
	LPDOPSTREAM stream_list;
	struct _PART_INFO_ info;
	struct _SIMPLE_FILE_ f_path;
	FCPEB fc;
	FSCTRL fs;
};
static uint32_t lfs_inter_blockv2r(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* _node, uint32_t vblock);
static int lfs_inter_list(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* _node, uint32_t vblock, uint32_t rblock);
static int lfs_simple_read(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* node, uint64_t off, void* ptr, uint32_t cnt);
static int lfs_simple_write(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* node, uint64_t off, void* ptr, uint32_t cnt);
static uint32_t lfs_create_dir(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* _node, wchar_t* name, size_t name_len);
static uint32_t lfs_create_file(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* _node, wchar_t* name, size_t name_len);

static struct LFS_DATA * lfs_list = NULL;

#define lfs_inter_bread(_lfs,_iblock)			bread((_lfs)->part, ((u64)(_iblock)) * 8)
#define lfs_inter_cache_bwrite(_lfs,_iblock)	cache_bwrite((_lfs)->part, ((u64)(_iblock)) * 8)
static uint32_t lfs_inter_alloc_block(struct LFS_DATA * lfs) {
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
				return _res;
			}
			byte_off++;
		}
		if (head->free_block_count < 2) {
			node->data[byte_off] &= ~(1 << bit_off);
			head->first_free_block = _res;
			return 0;
		}
		bh = lfs_inter_bread(lfs, 64 * 8);
		if (!bh) return -1;
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
	res = head->first_free_node;
	bdirty(lfs->head);
	index = res % (4096 / 128);
	vblock = res / (4096 / 128);
	rblock = lfs_inter_blockv2r(lfs, lfs->node, vblock);
	if (!rblock) {
		rblock = lfs_inter_alloc_block(lfs);
		if (!rblock) return 0;
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
	struct _BUFFER_HEAD_* bh;
	struct LFS_NODE* node;

	node = _node->addr;
	if (vblock < NODE_ZONE_32_DIRECT_CNT) {
		if (node->block[vblock]) return -1;
		bdirty(_node);
		node->block[vblock] = rblock;
		return 0;
	}
	vblock -= NODE_ZONE_32_DIRECT_CNT;
	if (vblock < 1024) {
		iblock = node->block[NODE_ZONE_32_L1];
		if (!iblock) {
			bdirty(_node);
			iblock = node->block[NODE_ZONE_32_L1] = lfs_inter_alloc_block(lfs);
			bh = lfs_inter_cache_bwrite(lfs, iblock);
			bdirty(bh);
			memset(bh->addr, 0, bh->buf_size);
			((uint32_t*)(bh->addr))[vblock] = rblock;
			bfree(bh);
			return 0;
		}
		bh = lfs_inter_bread(lfs, iblock);
		if (((uint32_t*)(bh->addr))[vblock]) {
			bfree(bh);
			return -1;
		}
		bdirty(bh);
		((uint32_t*)(bh->addr))[vblock] = rblock;
		bfree(bh);
		return 0;
	}
	vblock -= 1024;
	t3 = vblock & 1023;
	vblock >>= 10;
	if (vblock < 1024) {
		iblock = node->block[NODE_ZONE_32_L2];
		if (!iblock) {
			bdirty(_node);
			iblock = node->block[NODE_ZONE_32_L2] = lfs_inter_alloc_block(lfs);
			bh = lfs_inter_cache_bwrite(lfs, iblock);
			bdirty(bh);
			memset(bh->addr, 0, bh->buf_size);
			iblock = ((uint32_t*)(bh->addr))[vblock] = lfs_inter_alloc_block(lfs);
			bfree(bh);
			bh = lfs_inter_cache_bwrite(lfs, iblock);
			memset(bh->addr, 0, bh->buf_size);
			((uint32_t*)(bh->addr))[t3] = rblock;
			bfree(bh);
			return 0;
		}
		bh = lfs_inter_bread(lfs, iblock);
		iblock = ((uint32_t*)(bh->addr))[vblock];
		if (!iblock) {
			iblock = ((uint32_t*)(bh->addr))[vblock] = lfs_inter_alloc_block(lfs);
			bfree(bh);
			bh = lfs_inter_cache_bwrite(lfs, iblock);
			memset(bh->addr, 0, bh->buf_size);
			((uint32_t*)(bh->addr))[t3] = rblock;
			bfree(bh);
			return 0;
		}
		bfree(bh);
		bh = lfs_inter_bread(lfs, iblock);
		if (((uint32_t*)(bh->addr))[t3]) {
			bfree(bh);
			return -1;
		}
		bdirty(bh);
		((uint32_t*)(bh->addr))[t3] = iblock;
		bfree(bh);
		return 0;
	}
	vblock -= 1024;
	t2 = vblock & 1023;
	vblock >>= 10;
	if (vblock < 1024) {
		iblock = node->block[NODE_ZONE_32_L3];
		if (!iblock) {
			bdirty(_node);
			iblock = node->block[NODE_ZONE_32_L3] = lfs_inter_alloc_block(lfs);
			bh = lfs_inter_cache_bwrite(lfs, iblock);
			bdirty(bh);
			memset(bh->addr, 0, bh->buf_size);
			iblock = ((uint32_t*)(bh->addr))[vblock] = lfs_inter_alloc_block(lfs);
			bfree(bh);
			bh = lfs_inter_cache_bwrite(lfs, iblock);
			bdirty(bh);
			memset(bh->addr, 0, bh->buf_size);
			iblock = ((uint32_t*)(bh->addr))[t2] = lfs_inter_alloc_block(lfs);
			bfree(bh);
			bh = lfs_inter_cache_bwrite(lfs, iblock);
			memset(bh->addr, 0, bh->buf_size);
			((uint32_t*)(bh->addr))[t3] = rblock;
			bfree(bh);
			return 0;
		}
		bh = lfs_inter_bread(lfs, iblock);
		iblock = ((uint32_t*)(bh->addr))[vblock];
		if (!iblock) {
			bdirty(bh);
			memset(bh->addr, 0, bh->buf_size);
			iblock = ((uint32_t*)(bh->addr))[vblock] = lfs_inter_alloc_block(lfs);
			bfree(bh);
			bh = lfs_inter_cache_bwrite(lfs, iblock);
			bdirty(bh);
			memset(bh->addr, 0, bh->buf_size);
			iblock = ((uint32_t*)(bh->addr))[t2] = lfs_inter_alloc_block(lfs);
			bfree(bh);
			bh = lfs_inter_cache_bwrite(lfs, iblock);
			memset(bh->addr, 0, bh->buf_size);
			((uint32_t*)(bh->addr))[t3] = rblock;
			bfree(bh);
			return 0;
		}
		bfree(bh);
		bh = lfs_inter_bread(lfs, iblock);
		iblock = ((uint32_t*)(bh->addr))[t2];
		if (!iblock) {
			bdirty(bh);
			memset(bh->addr, 0, bh->buf_size);
			iblock = ((uint32_t*)(bh->addr))[t2] = lfs_inter_alloc_block(lfs);
			bfree(bh);
			bh = lfs_inter_cache_bwrite(lfs, iblock);
			memset(bh->addr, 0, bh->buf_size);
			if (((uint32_t*)(bh->addr))[t3]) {
				bfree(bh);
				return -1;
			}
			((uint32_t*)(bh->addr))[t3] = rblock;
			bfree(bh);
			return 0;
		}
		bfree(bh);
		bh = lfs_inter_bread(lfs, iblock);
		if (((uint32_t*)(bh->addr))[t3]) {
			bfree(bh);
			return -1;
		}
		bdirty(bh);
		((uint32_t*)(bh->addr))[t3] = rblock;
		bfree(bh);
		return iblock;
	}
	vblock -= 1024;
	t1 = vblock & 1023;
	vblock >>= 10;
	iblock = node->block[NODE_ZONE_32_L4];
	if (!iblock) {
		bdirty(_node);
		iblock = node->block[NODE_ZONE_32_L3] = lfs_inter_alloc_block(lfs);
		bh = lfs_inter_cache_bwrite(lfs, iblock);
		bdirty(bh);
		memset(bh->addr, 0, bh->buf_size);
		iblock = ((uint32_t*)(bh->addr))[vblock] = lfs_inter_alloc_block(lfs);
		bfree(bh);
		bh = lfs_inter_cache_bwrite(lfs, iblock);
		bdirty(bh);
		memset(bh->addr, 0, bh->buf_size);
		iblock = ((uint32_t*)(bh->addr))[t1] = lfs_inter_alloc_block(lfs);
		bfree(bh);
		bh = lfs_inter_cache_bwrite(lfs, iblock);
		bdirty(bh);
		memset(bh->addr, 0, bh->buf_size);
		iblock = ((uint32_t*)(bh->addr))[t2] = lfs_inter_alloc_block(lfs);
		bfree(bh);
		bh = lfs_inter_cache_bwrite(lfs, iblock);
		memset(bh->addr, 0, bh->buf_size);
		((uint32_t*)(bh->addr))[t3] = rblock;
		bfree(bh);
		return 0;
	}
	bh = lfs_inter_bread(lfs, iblock);
	iblock = ((uint32_t*)(bh->addr))[vblock];
	if (!iblock) {
		bdirty(bh);
		memset(bh->addr, 0, bh->buf_size);
		iblock = ((uint32_t*)(bh->addr))[vblock] = lfs_inter_alloc_block(lfs);
		bfree(bh);
		bh = lfs_inter_cache_bwrite(lfs, iblock);
		bdirty(bh);
		memset(bh->addr, 0, bh->buf_size);
		iblock = ((uint32_t*)(bh->addr))[t1] = lfs_inter_alloc_block(lfs);
		bfree(bh);
		bh = lfs_inter_cache_bwrite(lfs, iblock);
		bdirty(bh);
		memset(bh->addr, 0, bh->buf_size);
		iblock = ((uint32_t*)(bh->addr))[t2] = lfs_inter_alloc_block(lfs);
		bfree(bh);
		bh = lfs_inter_cache_bwrite(lfs, iblock);
		memset(bh->addr, 0, bh->buf_size);
		((uint32_t*)(bh->addr))[t3] = rblock;
		bfree(bh);
		return 0;
	}
	bfree(bh);
	bh = lfs_inter_bread(lfs, iblock);
	iblock = ((uint32_t*)(bh->addr))[t1];
	if (!iblock) {
		bdirty(bh);
		memset(bh->addr, 0, bh->buf_size);
		iblock = ((uint32_t*)(bh->addr))[t1] = lfs_inter_alloc_block(lfs);
		bfree(bh);
		bh = lfs_inter_cache_bwrite(lfs, iblock);
		bdirty(bh);
		memset(bh->addr, 0, bh->buf_size);
		iblock = ((uint32_t*)(bh->addr))[t2] = lfs_inter_alloc_block(lfs);
		bfree(bh);
		bh = lfs_inter_cache_bwrite(lfs, iblock);
		memset(bh->addr, 0, bh->buf_size);
		((uint32_t*)(bh->addr))[t3] = rblock;
		bfree(bh);
		return 0;
	}
	bfree(bh);
	bh = lfs_inter_bread(lfs, iblock);
	iblock = ((uint32_t*)(bh->addr))[t2];
	if (!iblock) {
		bdirty(bh);
		memset(bh->addr, 0, bh->buf_size);
		iblock = ((uint32_t*)(bh->addr))[t2] = lfs_inter_alloc_block(lfs);
		bfree(bh);
		bh = lfs_inter_cache_bwrite(lfs, iblock);
		memset(bh->addr, 0, bh->buf_size);
		((uint32_t*)(bh->addr))[t3] = rblock;
		bfree(bh);
		return 0;
	}
	bfree(bh);
	bh = lfs_inter_bread(lfs, iblock);
	if (((uint32_t*)(bh->addr))[t3]) {
		bfree(bh);
		return -1;
	}
	iblock = ((uint32_t*)(bh->addr))[t3];
	bfree(bh);
	return iblock;
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
static LPDOPSTREAM lfs_open_file(wchar_t * name,u64 mode, struct _SIMPLE_FILE_* _path) {
	struct LFS_DATA* lfs;
	wchar_t* path, * tpath;
	struct LFS_DIR dir;
	wchar_t rname[256];
	size_t name_len;
	uint64_t off;
	LPDOPSTREAM file;
	struct _BUFFER_HEAD_* node;
	struct _BUFFER_HEAD_* _cur_path;
	uint32_t inode;

	lfs = _path->fs->data;
	_cur_path = _path->data;
	node = lfs->root;
	path = name;
	while (1) {
		name = path;
		tpath = wcschr(path, L'\\');
		if (!tpath) break;
		name_len = tpath - path;
		if (name_len >= 256) {
			if (node != lfs->root) bfree(node);
			return NULL;
		}
		path = tpath + 1;
		off = 0;
		while (off < ((struct LFS_NODE*)(node->addr))->file_size) {
			if (lfs_simple_read(lfs, node, off, &dir, sizeof(struct LFS_DIR))) {
				if (node != _cur_path) bfree(node);
				return NULL;
			}
			if (dir.name_len == name_len && dir.attr & DIR_ATTR_DIR && dir.attr & DIR_ATTR_EXIST) {
				off += sizeof(struct LFS_DIR);
				if (lfs_simple_read(lfs, node, off, rname, dir.name_len * sizeof(wchar_t))) {
					if (node != _cur_path) bfree(node);
					return NULL;
				}
				rname[name_len] = 0;
				if (!wcsncmp(rname, name, name_len)) break;
			}
			else off += sizeof(struct LFS_DIR);
			off += ((dir.name_len * sizeof(wchar_t) + 0x0f) & ~0x0f);
		}
		if (off >= ((struct LFS_NODE*)(node->addr))->file_size) inode = lfs_create_dir(lfs, node, name, name_len);
		else inode = dir.node;
		if (node != lfs->root) bfree(node);
		node = lfs_inter_nread(lfs, inode);
		if (!node) return NULL;
	}
	name_len = wcslen(name);
	if (name_len >= 256) {
		if (node != lfs->root) bfree(node);
		return NULL;
	}
	off = 0;
	while (off < ((struct LFS_NODE*)(node->addr))->file_size) {
		if (lfs_simple_read(lfs, node, off, &dir, sizeof(struct LFS_DIR))) {
			if (node != _cur_path) bfree(node);
			return NULL;
		}
		if (dir.name_len == name_len && !(dir.attr & DIR_ATTR_DIR) && dir.attr & DIR_ATTR_EXIST) {
			off += sizeof(struct LFS_DIR);
			if (lfs_simple_read(lfs, node, off, rname, dir.name_len * sizeof(wchar_t))) {
				if (node != _cur_path) bfree(node);
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
			if (node != lfs->root) bfree(node);
			return NULL;
		}
		inode = lfs_create_file(lfs, node, name, name_len);
	}
	else {
		if (mode & FS_OPEN_N_REPLACE) {
			if (node != lfs->root) bfree(node);
			return NULL;
		}
		inode = dir.node;
	}
	if (node != lfs->root) bfree(node);
	node = lfs_inter_nread(lfs, inode);
	if (!node) return NULL;
	if (mode & FS_OPEN_MONOPOLIZE) {
		if ((uint64_t)node->data & EATTR_MONOPOLIZE) {
			bfree(node);
			return NULL;
		}
		else node->data = (void*)((uint64_t)node->data | EATTR_MONOPOLIZE);
	}
	file = malloc(sizeof(DOPSTREAM));
	if (!file) {
		bfree(node);
		return NULL;
	}
	memset(file, 0, sizeof(DOPSTREAM));
	InsertList(lfs->stream_list, file, d_prev, d_next);
	file->fc = &lfs->fc;
	file->mode = (int)mode;
	file->data = node;
	return file;
}
static int lfs_close(LPDOPSTREAM file) {
	struct _BUFFER_HEAD_* node;
	struct LFS_DATA * lfs;

	lfs = file->fc->data;
	node = file->data;
	bfree(node);

	RemoveList(lfs->stream_list, file, d_prev, d_next);
	free(file);
	return 0;
}
static int lfs_read(LPDOPSTREAM file, size_t count, void * buf) {
	struct _BUFFER_HEAD_* node;
	struct LFS_DATA* lfs;
	int ret;
	struct LFS_NODE* _node;

	lfs = file->fc->data;
	node = file->data;
	_node = node->addr;
	ret = lfs_simple_read(lfs, node, file->read_pos, buf, (uint32_t)count);
	file->read_pos += count;
	if (_node->file_size < file->read_pos) {
		bdirty(node);
		_node->file_size = file->read_pos;
	}
	return ret;
}
static int lfs_write(LPDOPSTREAM file, size_t count, void* buf) {
	struct _BUFFER_HEAD_* node;
	struct LFS_DATA* lfs;
	int ret;
	struct LFS_NODE* _node;

	lfs = file->fc->data;
	node = file->data;
	_node = node->addr;
	if ((uint64_t)node->data & EATTR_MONOPOLIZE && !(file->mode & FS_OPEN_MONOPOLIZE)) return -1;
	ret = lfs_simple_write(lfs, node, file->write_pos, buf,(uint32_t) count);
	file->write_pos += count;
	if (_node->file_size < file->write_pos) {
		bdirty(node);
		_node->file_size = file->write_pos;
	}
	return ret;
}
static int lfs_seek_g(int64_t Pos, int Org, LPDOPSTREAM file) {
	struct _BUFFER_HEAD_* node;
	struct LFS_NODE* _node;

	node = file->data;
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
static int lfs_seek_p(int64_t Pos, int Org, LPDOPSTREAM file) {
	struct _BUFFER_HEAD_* node;
	struct LFS_NODE* _node;

	node = file->data;
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
static uint64_t lfs_tell_g(LPDOPSTREAM file) {
	return file->read_pos;
}
static uint64_t lfs_tell_p(LPDOPSTREAM file) {
	return file->write_pos;
}
static int lfs_put(int Element, LPDOPSTREAM file) {
	struct _BUFFER_HEAD_* node;
	struct LFS_DATA* lfs;
	int ret;
	struct LFS_NODE* _node;

	lfs = file->fc->data;
	node = file->data;
	_node = node->addr;
	if ((uint64_t)node->data & EATTR_MONOPOLIZE && !(file->mode & FS_OPEN_MONOPOLIZE)) return -1;
	ret = lfs_simple_read(lfs, node, file->write_pos, &Element, 1);
	file->write_pos += 1;
	if (_node->file_size < file->write_pos) {
		bdirty(node);
		_node->file_size = file->write_pos;
	}
	return ret;
}
static int lfs_get(LPDOPSTREAM file) {
	struct _BUFFER_HEAD_* node;
	struct LFS_DATA* lfs;
	int ret;
	int Element;
	struct LFS_NODE* _node;

	lfs = file->fc->data;
	node = file->data;
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
static int64_t lfs_get_size(LPDOPSTREAM file) {
	struct _BUFFER_HEAD_* node;
	struct LFS_NODE* _node;

	node = file->data;
	_node = node->addr;
	return _node->file_size;
}
static int lfs_set_size(int64_t Size, LPDOPSTREAM file) {
	return -1;
}
static int lfs_info(LPDOPSTREAM file, size_t size, void* p_info) {
	return -1;
}
static FCPEB fc = {
	.close = lfs_close,
	.read = lfs_read,
	.write = lfs_write,
	.seek_get = lfs_seek_g,
	.seek_put = lfs_seek_p,
	.tell_get = lfs_tell_g,
	.tell_put = lfs_tell_g,
	.get = lfs_get,
	.put = lfs_put,
	.get_size = lfs_get_size,
	.set_size = lfs_set_size,
	.info = lfs_info
};

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
static uint32_t lfs_create_dir(struct LFS_DATA* lfs, struct _BUFFER_HEAD_ * _node, wchar_t* name,size_t name_len) {
	struct LFS_DIR dir;
	struct LFS_NODE* node;
	uint64_t off;
	uint32_t iblock;
	struct _BUFFER_HEAD_* bh;
	uint32_t inode;

	dir.attr = DIR_ATTR_DIR | DIR_ATTR_EXIST;
	dir.node = lfs_inter_alloc_node(lfs);
	dir.name_len = (uint8_t)name_len;
	time(&dir.create_time);
	node = _node->addr;
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
	return inode;
}
static uint32_t lfs_create_file(struct LFS_DATA* lfs, struct _BUFFER_HEAD_* _node, wchar_t* name, size_t name_len) {
	struct LFS_DIR dir;
	struct LFS_NODE* node;
	uint64_t off;
	uint32_t iblock;
	struct _BUFFER_HEAD_* bh;
	uint32_t inode;

	dir.attr = DIR_ATTR_EXIST;
	dir.node = lfs_inter_alloc_node(lfs);
	dir.name_len = (uint8_t)name_len;
	time(&dir.create_time);
	node = _node->addr;
	off = node->file_size;
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
	return inode;
}

static LPDOPSTREAM lfs_open(wchar_t* name, u64 mode, LPSIMPLE_FILE _path) {
	struct LFS_DATA* lfs;
	wchar_t* path, * tpath;
	struct LFS_DIR dir;
	wchar_t rname[256];
	size_t name_len;
	uint64_t off;
	LPDOPSTREAM file;
	struct _BUFFER_HEAD_* node;
	uint32_t inode;

	lfs = _path->fs->data;
	node = _path->data;
	path = name;
	while (1) {
		name = path;
		tpath = wcschr(path, L'\\');
		if (!tpath) break;
		name_len = tpath - path;
		if (name_len > 255) {
			if (node != _path->data) bfree(node);
			return NULL;
		}
		path = tpath + 1;
		off = 0;
		while (off < ((struct LFS_NODE*)(node->addr))->file_size) {
			if (lfs_simple_read(lfs, node, off, &dir, sizeof(struct LFS_DIR))) {
				if (node != _path->data) bfree(node);
				return NULL;
			}
			if (dir.name_len == name_len && dir.attr & DIR_ATTR_DIR && dir.attr & DIR_ATTR_EXIST) {
				off += sizeof(struct LFS_DIR);
				if (lfs_simple_read(lfs, node, off, rname, dir.name_len * sizeof(wchar_t))) {
					if (node != _path->data) bfree(node);
					return NULL;
				}
				rname[name_len] = 0;
				if (!wcsncmp(rname, name, name_len)) break;
			}
			else off += sizeof(struct LFS_DIR);
			off += ((dir.name_len * sizeof(wchar_t) + 0x0f) & ~0x0f);
		}
		if (off >= ((struct LFS_NODE*)(node->addr))->file_size) inode = lfs_create_dir(lfs, node, name, name_len);
		else inode = dir.node;
		if (node != _path->data) bfree(node);
		node = lfs_inter_nread(lfs, inode);
		if (!node) return NULL;
	}
	name_len = wcslen(name);
	if (name_len > 255) {
		if (node != _path->data) bfree(node);
		return NULL;
	}
	off = 0;
	while (off < ((struct LFS_NODE*)(node->addr))->file_size) {
		if (lfs_simple_read(lfs, node, off, &dir, sizeof(struct LFS_DIR))) {
			if (node != _path->data) bfree(node);
			return NULL;
		}
		if (dir.name_len == name_len && !(dir.attr & DIR_ATTR_DIR) && dir.attr & DIR_ATTR_EXIST) {
			off += sizeof(struct LFS_DIR);
			if (lfs_simple_read(lfs, node, off, rname, dir.name_len * sizeof(wchar_t))) {
				if (node != _path->data) bfree(node);
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
			if (node != lfs->root) bfree(node);
			return NULL;
		}
		inode = lfs_create_file(lfs, node, name, name_len);
	}
	else {
		if (mode & FS_OPEN_N_REPLACE) {
			if (node != lfs->root) bfree(node);
			return NULL;
		}
		inode = dir.node;
	}
	if (node != _path->data) bfree(node);
	node = lfs_inter_nread(lfs, inode);
	if (!node) return NULL;
	if (mode & FS_OPEN_MONOPOLIZE) {
		if ((uint64_t)node->data & EATTR_MONOPOLIZE) {
			bfree(node);
			return NULL;
		}
		else node->data = (void*)((uint64_t)(node->data) | EATTR_MONOPOLIZE);
	}
	file = malloc(sizeof(DOPSTREAM));
	if(!file) {
		bfree(node);
		return NULL;
	}
	memset(file, 0, sizeof(DOPSTREAM));
	InsertList(lfs->stream_list, file, d_prev, d_next);
	file->mode = (int)mode;
	file->data = node;
	file->fc = &(lfs->fc);
	return file;
}
static FSCTRL fs = {.open = lfs_open};
int unsummon_lfs(HANDLE _fs);
static void lfs_open_call_back(LPDOPSTREAM _file, void* c_data) {unsummon_lfs(c_data);}
HANDLE summon_lfs() {
	struct LFS_DATA* lfs;
	struct LFS_HEAD* head;

	lfs = malloc(sizeof(struct LFS_DATA));
	if (!lfs) return NULL;
	memset(lfs, 0, sizeof(struct LFS_DATA));
	lfs->part = open(L"/.dev/lfs.dev", FS_OPEN_READ | FS_OPEN_WRITE, lfs, lfs_open_call_back);
	if (!lfs->part) {
		free(lfs);
		return NULL;
	}
	info(lfs->part, sizeof(struct _PART_INFO_), &(lfs->info));
	if (lfs->info.cache_block_size < 4096) {
		close(lfs->part);
		free(lfs);
		return NULL;
	}
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
	memcpy(&lfs->fs, &fs, sizeof(FSCTRL));
	memcpy(&lfs->fc, &fc, sizeof(FCPEB));
	lfs->fs.data = lfs;
	lfs->fc.data = lfs;
	lfs->my_path.data = lfs->root;
	wcscpy(lfs->map_path, L"/fs/lfs/");
	lfs->vaild = 1;
	InsertList(lfs_list, lfs, prev, next);
	fs_map(lfs->map_path, &lfs->my_path, &lfs->f_path);
	return lfs;
}
int format_lfs(HANDLE _fs) {
	struct LFS_DATA* lfs;
	struct LFS_HEAD* head;
	struct LFS_NODE* node;
	struct LFS_DIR dir;
	LPDOPSTREAM file;

	lfs = _fs;
	if (lfs->vaild) {
		while (lfs->stream_list) {
			file = lfs->stream_list;
			if (lfs->stream_list->call_back) {
				file->call_back(file, file->call_back_data);
				if (file != lfs->stream_list) continue;
			}
			file = lfs->stream_list->d_next;
			free(lfs->stream_list);
			file->d_prev = NULL;
			lfs->stream_list = file;
		}
	}
	else {
		lfs->head = lfs_inter_bread(lfs, 1);
		if (!lfs->head) {
			return -1;
		}
		if (buf_lock(lfs->head, 0, 4096)) {
			bfree(lfs->head);
			return -1;
		}
	}
	bdirty(lfs->head);
	memset(lfs->head->addr,0,4096);
	head = lfs->head->addr;
	head->version = LFS_CUR_VERSION;
	head->name_len = 0;
	head->path_len = 0;
	head->crc32 = 0;
	head->head_size = sizeof(struct LFS_HEAD);
	head->first_free_node = 4;
	head->first_free_block = 3;
	head->block_count = (uint32_t)(lfs->info.logical_sector_count / 8);
	head->node_0_block = 2;
	head->free_block_count = head->block_count - 3;
	time(&head->create_time);
	memcpy(&head->owner, &LFS_SYS_OWNER_GUID, sizeof(GUID));
	head->crc32 = ComputeCRC32(0, head, head->head_size);
	if (!lfs->vaild) {
		lfs->node = lfs_inter_bread(lfs, 2);
		if (!lfs->node) {
			lfs->vaild = 0;
			bfree(lfs->node);
			buf_unlock(lfs->head, 0);
			bfree(lfs->head);
			return -1;
		}
		lfs->root = hl_blink(lfs->node, sizeof(struct LFS_NODE) * 1, sizeof(struct LFS_NODE));
		lfs->bad = hl_blink(lfs->node, sizeof(struct LFS_NODE) * 2, sizeof(struct LFS_NODE));
		lfs->map = hl_blink(lfs->node, sizeof(struct LFS_NODE) * 3, sizeof(struct LFS_NODE));
	}
	bdirty(lfs->node);
	node = lfs->node->addr;
	memset(node, 0, 4096);
	node[0].create_time = head->create_time;
	node[1].create_time = head->create_time;
	node[2].create_time = head->create_time;
	node[3].create_time = head->create_time;
	node[0].access_time = head->create_time;
	node[1].access_time = head->create_time;
	node[2].access_time = head->create_time;
	node[3].access_time = head->create_time;
	node[0].modify_time = head->create_time;
	node[1].modify_time = head->create_time;
	node[2].modify_time = head->create_time;
	node[3].modify_time = head->create_time;
	node[0].file_size = 4 * sizeof(struct LFS_NODE);
	node[1].file_size = sizeof(struct LFS_DIR) + 16;
	node[2].file_size = 0;
	node[3].file_size = 64;
	node[0].attr = NODE_ATTR_EXIST;
	node[1].attr = NODE_ATTR_DATA | NODE_ATTR_EXIST | NODE_ATTR_DIR;
	node[2].attr = NODE_ATTR_DATA | NODE_ATTR_EXIST;
	node[3].attr = NODE_ATTR_DATA | NODE_ATTR_EXIST;
	node[0].links = 1;
	node[1].links = 1;
	node[2].links = 1;
	node[3].links = 1;
	node[0].inode = 0;
	node[1].inode = 1;
	node[2].inode = 2;
	node[3].inode = 3;
	memcpy(&node[0].owner, &LFS_SYS_OWNER_GUID, sizeof(GUID));
	memcpy(&node[1].owner, &LFS_SYS_OWNER_GUID, sizeof(GUID));
	memcpy(&node[2].owner, &LFS_SYS_OWNER_GUID, sizeof(GUID));
	memcpy(&node[3].owner, &LFS_SYS_OWNER_GUID, sizeof(GUID));
	node[0].block[0] = 2;
	memset(&dir, 0, sizeof(struct LFS_DIR));
	dir.name_len = 1;
	dir.attr = DIR_ATTR_EXIST | DIR_ATTR_DIR;
	dir.node = 1;
	dir.create_time = head->create_time;
	memcpy(node[1].data, &dir, sizeof(struct LFS_DIR));
	node[1].data[sizeof(struct LFS_DIR)] = L'.';
	node[3].data[0] = 0x07;
	lfs->my_path.data = lfs;
	lfs->my_path.fs = &lfs->fs;
	memcpy(&lfs->fs, &fs, sizeof(FSCTRL));
	memcpy(&lfs->fc, &fc, sizeof(FCPEB));
	lfs->fs.data = lfs;
	lfs->my_path.data = lfs->root;
	lfs->fc.data = lfs;

	wcscpy(lfs->map_path, L"/fs/lfs/");
	lfs->vaild = 1;
	fs_map(lfs->map_path, &lfs->my_path, &lfs->f_path);
	return 0;
}
int unsummon_lfs(HANDLE _fs) {
	struct LFS_DATA* lfs;
	LPDOPSTREAM file;

	if (!_fs) return -1;
	lfs = _fs;

	RemoveList(lfs_list, lfs, prev, next);
	if (lfs->vaild) {
		while (file = lfs->stream_list) {
			if (file->call_back) file->call_back(file, file->call_back_data);
			else close(file);
		}
		bfree(lfs->node);
		bfree(lfs->root);
		bfree(lfs->map);
		bfree(lfs->bad);
		buf_unlock(lfs->head, 0);
		bfree(lfs->head);
	}
	fs_unmap(lfs->map_path);
	close(lfs->part);
	free(lfs);
	return 0;
}
int lfs_write_boot(HANDLE _fs, const wchar_t* name) {
	struct LFS_DATA* lfs;
	FILE* sfile;
	int fsize;
	struct _BUFFER_HEAD_* bh;

	if (!_fs || !name) return -1;
	lfs = _fs;

	sfile = _wfopen(name, L"rb");
	if (!sfile) return -1;
	fseek(sfile, 0, SEEK_END);
	fsize = ftell(sfile);
	fseek(sfile, 0, SEEK_SET);
	if (fsize >= 4096) fsize = 4096;
	bh = bread(lfs->part, 0);
	if (!bh) {
		fclose(sfile);
		return -1;
	}
	bdirty(bh);
	fread(bh->addr, 1, fsize, sfile);
	bfree(bh);
	fclose(sfile);
	return 0;
}