//Lava OS
//PageFault
//20-02-20-17-05

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
int wprintf(const wchar_t * fmt,...);
void putchar(char ch);
void print(const char * Str);
void puts(const char * Str);
int printk(const char * fmt,...);


#endif