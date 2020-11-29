//Lava OS
//PageFault
//20-02-20-15-29

#include "lm.h"
#define VideoBase ((struct _CHAR_ *)0xb8000)
static int X = 0;
static int Y = 0;
static wchar_t PrintfBuf[256];
struct _CHAR_ {
	char Ch;
	char Attr;
};

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
	ret = vsprintf((char*)PrintfBuf, 512, fmt, ((int64_t*)(&fmt)) + 1);
	print((char*)PrintfBuf);
	return ret;
}
