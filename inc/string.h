/*
	inc/string.h
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