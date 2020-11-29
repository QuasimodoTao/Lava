//Lava OS
//PageFault
//20-03-11-13-21

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