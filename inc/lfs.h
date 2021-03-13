/*
	inc/lfs.h
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

#ifndef _LFS_H_
#define _LFS_H_

#include <stddef.h>

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
#define NODE_ZONE_SIZE			64
#define LFS_GPT_TYPE_GUID        (GUID){0xa23a4c32, 0x5650, 0x47c6, { 0xaf,0x40,0x5c,0xd9,0x48,0x78,0xc6,0x71 }}
#define LFS_SYS_OWNER_GUID       (GUID){0xa23a4c32, 0x5650, 0x47c6, { 0xaf,0x40,0x5c,0xd9,0x48,0x78,0xc6,0x71 }}

struct LFS_HEAD {
	int32_t version;//���ڵİ汾��1.0��0x00010000
	int32_t crc32;
	uint32_t head_size;
	uint32_t path_len;
	uint32_t name_len;
	volatile uint32_t first_free_node;
	volatile uint32_t first_free_block;//��һ�����еĿ飬�ڵ��
	uint32_t block_count;//��ǰ�����еĿ�����
	uint32_t node_0_block;//0 node,1 root,2 bad,3 map
	volatile uint32_t free_block_count;//���п�����
	stamp64_t create_time;//��������ʽ����ʱ��
	GUID owner;
	wchar_t name_path[];
};
struct LFS_NODE {//pack 128 bytes
	stamp64_t create_time, access_time;
	stamp64_t modify_time, delete_time;
	volatile uint64_t file_size;
	uint16_t attr, links;
	uint32_t inode;
	GUID owner;
	union {
		volatile uint32_t block[16];
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

#endif