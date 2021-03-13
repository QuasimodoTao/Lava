/*
	tools/dop/dop.h
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

#ifndef _DOP_H_
#define _DOP_H_

#include <objbase.h>
#include <stdio.h>

#define U64	unsigned long long
#define U32 unsigned int
#define Bswap64(x)	(((((U64)x) & 0xff) << 56) | ((((U64)x) & 0xff00) << 40) \
	| ((((U64)x) & 0xff0000) << 24) | ((((U64)x) & 0xff000000) << 8)\
	| ((((U64)x) & 0xff00000000) >> 8) | ((((U64)x) & 0xff0000000000) >> 24) \
	| ((((U64)x) & 0xff000000000000) >> 40) | ((((U64)x) & 0xff00000000000000) >> 56))
#define Bswap32(x)	((((U32)x) & 0xff) << 24) | (((((U32)x) & 0xff00) << 8)) \
	| (((((U32)x) & 0xff0000) >> 8)) | (((((U32)x) & 0xff000000) >> 24))


#define MULT_PROCR
#define DISK_CACHE_BLOCK_SIZE	4096
#define PATH_MAX_LENGTH			4096

typedef void * HANDLE;
typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef unsigned short wchar_t;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef long long stamp64_t;

typedef unsigned long long iblock_t;

#ifndef kmalloc
#define kmalloc(x,y)	malloc(x)
#define kfree(x)		free(x)
#endif

unsigned int ComputeCRC32(unsigned int crc,void * Data, int Len);
int IsZero(char * Data, int Len);
int IsNulGUID(GUID * guid);

int (*CreateGuid)(GUID * pid);

#define VHD_MAP_NAME	L"/.dev/vhd0.dev"

#define ESP_GUID	((GUID){0xc12a7328, 0xf81f, 0x11d2, { \
	0xba, 0x4b, 0x00, 0xa0, 0xc9, 0x3e, 0xc9, 0x3b }})
#define FAT_GUID	((GUID){0xebd0a0a2, 0xb9e5, 0x4433, { \
	0x87, 0xc0, 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7 }})
#define LFS_GUID	((GUID){0xa23a4c32, 0x5650, 0x47c6, {\
	0xaf, 0x40, 0x5c, 0xd9, 0x48, 0x78, 0xc6, 0x71}})


#define InsertList(_head,_this,_prev,_next)	\
{\
	_this->_prev = NULL;\
	_this->_next = _head;\
	if(_head) _head->_prev = _this;\
	_head = _this;\
}
#define RemoveList(_head,_this,_prev,_next) \
{\
	if(_this->_prev) _this->_prev->_next = _this->_next;\
	else _head = _this->_next;\
	if(_this->_next) _this->_next->_prev = _this->_prev;\
}


#endif