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
#include <spinlock.h>
#include <kernel.h>
void draw_char(int ch);

static int lock = 0;

void putwchar(wchar_t ch){
	u64 rf;

	SFI(rf);
	spin_lock_bit(&lock,0);
	draw_char(ch);
	spin_unlock_bit(&lock,0);
	LF(rf);
}
void wprint(const wchar_t * Str){
	u64 rf;
	
	SFI(rf);
	spin_lock_bit(&lock,0);
	while(*Str){
		draw_char(*Str);
		Str++;
	}
	spin_unlock_bit(&lock,0);
	LF(rf);
}
void putws(const wchar_t * Str){
	u64 rf;

	SFI(rf);
	spin_lock_bit(&lock,0);
	while(*Str){
		draw_char(*Str);
		Str++;
	}
	draw_char(L'\n');
	spin_unlock_bit(&lock,0);
	LF(rf);
}
int __attribute__((noinline)) wprintk(const wchar_t * fmt,...) {
	int ret;
	wchar_t PrintfBuf[256];
	ret = vwsprintf(PrintfBuf, 256, fmt, ((int64_t*)(&fmt)) + 1);
	wprint(PrintfBuf);
	return ret;
}

void putchar(char ch){
	u64 rf;

	SFI(rf);
	spin_lock_bit(&lock,0);
	draw_char(ch);
	spin_unlock_bit(&lock,0);
	LF(rf);
}
void print(const char * Str){
	u64 rf;

	SFI(rf);
	spin_lock_bit(&lock,0);
	while(*Str){
		draw_char(*Str);
		Str++;
	}
	spin_unlock_bit(&lock,0);
	LF(rf);
}
void puts(const char * Str){
	u64 rf;

	SFI(rf);
	spin_lock_bit(&lock,0);
	while(*Str){
		draw_char(*Str);
		Str++;
	}
	draw_char('\n');
	spin_unlock_bit(&lock,0);
	LF(rf);
}
int __attribute__((noinline)) printk(const char * fmt,...) {
	int ret;
	char PrintfBuf[256];
	ret = vsprintf((char*)PrintfBuf, 256, fmt, ((int64_t*)(&fmt)) + 1);
	print((char*)PrintfBuf);
	return ret;
}