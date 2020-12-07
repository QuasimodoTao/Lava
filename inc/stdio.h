/*
	inc/stdio.h
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

#ifndef _STDIO_H_
#define _STDIO_H_

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#include <stddef.h>

int vsprintf(char * _buf, size_t bsize, const char * fmt, int64_t * argvlist);
int sprintf(char * _buf, size_t size, const char * fmt,...);
int vwsprintf(wchar_t * _buf, size_t bsize, const wchar_t * fmt, int64_t * argvlist);
int wsprintf(wchar_t * _buf, size_t size, const wchar_t * fmt,...);
void putwchar(wchar_t ch);
void wprint(const wchar_t * Str);
void putws(const wchar_t * Str);
int wprintk(const wchar_t * fmt,...);
void putchar(char ch);
void print(const char * Str);
void puts(const char * Str);
int printk(const char * fmt,...);


#endif