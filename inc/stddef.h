/*
	inc/stddef.h
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

#ifndef _STDDEF_H_
#define _STDDEF_H_

#define offsetof(type,member)	((size_t)(&(((type *)0)->member)))
#define NULL 					((void*)0)
#define ptrdiff_t(p1,p2)		(((unsigned long long)(p1)) - ((unsigned long long)(p2)))

typedef unsigned short wchar_t;
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned long long size_t;
typedef void * HANDLE;

typedef float f32_t;
typedef double f64_t;
typedef long double f80_t;

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
typedef float f32;
typedef double f64;
typedef long double f80;

typedef unsigned char * __m8;
typedef unsigned short * __m16;
typedef unsigned int * __m32;
typedef unsigned long long * __m64;
typedef unsigned long long * __m128[2];
typedef unsigned char __i8;
typedef unsigned short __i16;
typedef unsigned int __i32;
typedef unsigned long long __i64;


typedef struct _GUID_{
	int32_t Data1;
	int16_t Data2;
	int16_t Data3;
	int8_t Data4[8];
} GUID;

#endif