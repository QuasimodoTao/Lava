/*
	dev/out.c
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

#include <stddef.h>
#include <string.h>
#include <asm.h>
#include <int.h>
#include <stdio.h>
void draw_char(int ch);

void putwchar(wchar_t ch){
	draw_char(ch);
}
void wprint(const wchar_t * Str){
	while(*Str){
		draw_char(*Str);
		Str++;
	}
}
void putws(const wchar_t * Str){
	while(*Str){
		draw_char(*Str);
		Str++;
	}
	draw_char(L'\n');
}
int __attribute__((noinline)) wprintf(const wchar_t * fmt,...) {
	int ret;
	wchar_t PrintfBuf[256];
	ret = vwsprintf(PrintfBuf, 256, fmt, ((int64_t*)(&fmt)) + 1);
	wprint(PrintfBuf);
	return ret;
}

void putchar(char ch){
	draw_char(ch);
}
void print(const char * Str){
	while(*Str){
		draw_char(*Str);
		Str++;
	}
}
void puts(const char * Str){
	while(*Str){
		draw_char(*Str);
		Str++;
	}
	draw_char('\n');
}
int __attribute__((noinline)) printk(const char * fmt,...) {
	int ret;
	char PrintfBuf[256];
	ret = vsprintf((char*)PrintfBuf, 256, fmt, ((int64_t*)(&fmt)) + 1);
	print((char*)PrintfBuf);
	return ret;
}