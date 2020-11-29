//Lava OS
//PageFault
//20-02-19-16-50

#ifndef _STRING_H_
#define _STRING_H_
#include <stddef.h>
char * strcpy(char * str1,const char * str2);
wchar_t * wcscpy(wchar_t * str1,const wchar_t * str2);
char * strncpy(char * str1,const char * str2,size_t count);
wchar_t * wcsncpy(wchar_t * str1,const wchar_t * str2,size_t count);
char * strcat(char * des,const char * scr);
wchar_t * wcscat(wchar_t * des,const wchar_t * scr);
char * strncat(char * des,const char * scr,size_t count);
wchar_t * wcsncat(wchar_t * des,const wchar_t * scr,size_t count);
int strcmp(const char * str1,const char * str2);
int wcscmp(const wchar_t * str1,const wchar_t * str2);
int strncmp(const char * str1,const char * str2,size_t count);
int wcsncmp(const wchar_t * str1,const wchar_t * str2,size_t count);
char * strchr(const char * str,int c);
wchar_t * wcschr(const wchar_t * str,int c);
char * strrchr(const char * str,int c);
wchar_t * wcsrchr(const wchar_t * str,int c);
size_t strlen(const char * str);
size_t wcslen(const wchar_t * str);
void * memset(void * Des,int Val,size_t Count);
void * wmemset(void * Des,int Val,size_t Count);
void * memcpy(void * d,void * s,size_t n);
void * wmemcpy(void * d,void * s,size_t n);
void * memmove(void * Des,void * Scr,size_t Count);
void * wmemmove(void * Des,void * Scr,size_t Count);
int memcmp(void * s1,void * s2,size_t n);
int wmemcmp(void * s1,void * s2,size_t n);
void * memchr(void * s,int ch,size_t n);
void * wmemchr(void * s,int ch,size_t n);
#endif