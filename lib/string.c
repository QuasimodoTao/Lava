/*
	lib/string.c
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

char * strcpy(char * str1,const char * str2){
	asm("_strcpyl0:\n\t"
		"lodsb\n\t"
		"stosb\n\t"
		"testb %%al,%%al\n\t"
		"jnz _strcpyl0\n\t"
		:"=r"(str1)
		:"0"(str1),"S"(str2),"D"(str1)
		:"al"
	);
	return str1;
}
wchar_t * wcscpy(wchar_t * str1,const wchar_t * str2){
	asm("_wcscpyl0:\n\t"
		"lodsw\n\t"
		"stosw\n\t"
		"testw %%ax,%%ax\n\t"
		"jnz _wcscpyl0\n\t"
		:"=r"(str1)
		:"0"(str1),"S"(str2),"D"(str1)
		:"ax"
	);
	return str1;
}
char * strncpy(char * str1,const char * str2,size_t count){
	asm("rep\n\t"
		"movsb\n\t"
		:"=r"(str1)
		:"0"(str1),"S"(str2),"D"(str1),"c"(count)
	);
	return str1;
}
wchar_t * wcsncpy(wchar_t * str1,const wchar_t * str2,size_t count){
	asm("rep\n\t"
		"movsw\n\t"
		:"=r"(str1)
		:"0"(str1),"S"(str2),"D"(str1),"c"(count)
	);
	return str1;
}
char * strcat(char * des,const char * scr){
	asm("_strcatl0:\n\t"
		"lodsb\n\t"
		"testb %%al,%%al\n\t"
		"jnz _strcatl0\n\t"
		"xchgq %%rdi,%%rsi\n\t"
		"_strcatl1:\n\t"
		"lodsb\n\t"
		"stosb\n\t"
		"testb %%al,%%al\n\t"
		"jnz _strcatl1"
		:"=r"(des)
		:"0"(des),"S"(des),"D"(scr)
		:"al"
	);
	return des;
}
wchar_t * wcscat(wchar_t * des,const wchar_t * scr){
	asm("_wcscatl0:\n\t"
		"lodsw\n\t"
		"testw %%ax,%%ax\n\t"
		"jnz _wcscatl0\n\t"
		"xchgq %%rdi,%%rsi\n\t"
		"_wcscatl1:\n\t"
		"lodsw\n\t"
		"stosw\n\t"
		"testw %%ax,%%ax\n\t"
		"jnz _wcscatl1"
		:"=r"(des)
		:"0"(des),"S"(des),"D"(scr)
		:"ax"
	);
	return des;
}
char * strncat(char * des,const char * scr,size_t count){
	asm("_strncatl0:\n\t"
		"lodsb\n\t"
		"testb %%al,%%al\n\t"
		"jnz _strncatl0\n\t"
		"xchgq %%rdi,%%rsi\n\t"
		"rep\n\t"
		"movsb\n\t"
		"stosb\n\t"
		:"=r"(des)
		:"0"(des),"S"(des),"D"(scr),"c"(count)
		:"al"
	);
	return des;
}
wchar_t * wcsncat(wchar_t * des,const wchar_t * scr,size_t count){
	asm("_wcsncatl0:\n\t"
		"lodsw\n\t"
		"testw %%ax,%%ax\n\t"
		"jnz _wcsncatl0\n\t"
		"xchgq %%rdi,%%rsi\n\t"
		"rep\n\t"
		"movsw\n\t"
		"stosw\n\t"
		:"=r"(des)
		:"0"(des),"S"(des),"D"(scr),"c"(count)
		:"ax"
	);
	return des;
}
int strcmp(const char * str1,const char * str2){
	register char ret;
	asm("_strcmpl0:"
		"lodsb\n\t"
		"testb %%al,%%al\n\t"
		"jz _strcmpl1\n\t"
		"scasb\n\t"
		"je _strcmpl0\n\t"
		"subb -1(%%rdi),%%al\n\t"
		"jmp _strcmpl2\n\t"
		"_strcmpl1:\n\t"
		"subb (%%rdi),%%al\n\t"
		"_strcmpl2:\n\t"
		:"=a"(ret)
		:"S"(str1),"D"(str2)
	);
	return ret;
}
int wcscmp(const wchar_t * str1,const wchar_t * str2){
	register char ret;
	asm("_wcscmpl0:"
		"lodsw\n\t"
		"testw %%ax,%%ax\n\t"
		"jz _wcscmpl1\n\t"
		"scasw\n\t"
		"je _wcscmpl0\n\t"
		"subw -2(%%rdi),%%ax\n\t"
		"jmp _wcscmpl2\n\t"
		"_wcscmpl1:\n\t"
		"subw (%%rdi),%%ax\n\t"
		"_wcscmpl2:\n\t"
		:"=a"(ret)
		:"S"(str1),"D"(str2)
	);
	return ret;
}
int strncmp(const char * str1,const char * str2,size_t count){
	register int ret;
	asm("repe\n\t"
		"cmpsb\n\t"
		"ja _strncmpl0\n\t"
		"jb _strncmpl1\n\t"
		"xorl %%eax,%%eax\n\t"
		"jmp _strncmpl2\n\t"
		"_strncmpl0:\n\t"
		"movl $1,%%eax\n\t"
		"jmp _strncmpl2\n\t"
		"_strncmpl1:\n\t"
		"movl $-1,%%eax\n\t"
		"_strncmpl2:\n\t"
		:"=a"(ret)
		:"S"(str1),"D"(str2),"c"(count)
	);
	return ret;
}
int wcsncmp(const wchar_t * str1,const wchar_t * str2,size_t count){
	register int ret;
	asm("repe\n\t"
		"cmpsw\n\t"
		"ja _wcsncmpl0\n\t"
		"jb _wcsncmpl1\n\t"
		"xorl %%eax,%%eax\n\t"
		"jmp _wcsncmpl2\n\t"
		"_wcsncmpl0:\n\t"
		"movl $1,%%eax\n\t"
		"jmp _wcsncmpl2\n\t"
		"_wcsncmpl1:\n\t"
		"movl $-1,%%eax\n\t"
		"_wcsncmpl2:\n\t"
		:"=a"(ret)
		:"S"(str1),"D"(str2),"c"(count)
	);
	return ret;
}
char * strchr(const char * str,int c){
	asm("_strchrl0:\n\t"
		"lodsb\n\t"
		"testb %%al,%%al\n\t"
		"jz _strchrl1\n\t"
		"cmpb %%cl,%%al\n\t"
		"jne _strchrl0\n\t"
		"decq %%rsi\n\t"
		"jmp _strchrl2\n\t"
		"_strchrl1:\n\t"
		"xorq %%rsi,%%rsi\n\t"
		"_strchrl2:\n\t"
		:"=S"(str)
		:"S"(str),"c"(c)
		:"al"
	);
	return (char*)str;
}
wchar_t * wcschr(const wchar_t * str,int c){
	asm("_wcschrl0:\n\t"
		"lodsw\n\t"
		"testw %%ax,%%ax\n\t"
		"jz _wcschrl1\n\t"
		"cmpw %%cx,%%ax\n\t"
		"jne _wcschrl0\n\t"
		"decq %%rsi\n\t"
		"decq %%rsi\n\t"
		"jmp _wcschrl2\n\t"
		"_wcschrl1:\n\t"
		"xorq %%rsi,%%rsi\n\t"
		"_wcschrl2:\n\t"
		:"=S"(str)
		:"S"(str),"c"(c)
		:"ax"
	);
	return (wchar_t*)str;
}
char * strrchr(const char * str,int c){
	asm("xorq %0,%0\n\t"
		"_strrchrl0:\n\t"
		"lodsb\n\t"
		"testb %%al,%%al\n\t"
		"jz _strrchrl1\n\t"
		"cmpb %%al,%%cl\n\t"
		"jnz _strrchrl0\n\t"
		"leaq -1(%%rsi),%0\n\t"
		"jmp _strrchrl0\n\t"
		"_strrchrl1:\n\t"
		:"=r"(str)
		:"S"(str),"c"(c)
		:"al"
	);
	return (char*)str;
}
wchar_t * wcsrchr(const wchar_t * str,int c){
	asm("xorq %0,%0\n\t"
		"_wcsrchrl0:\n\t"
		"lodsw\n\t"
		"testw %%ax,%%ax\n\t"
		"jz _wcsrchrl1\n\t"
		"cmpw %%ax,%%cx\n\t"
		"jnz _wcsrchrl0\n\t"
		"leaq -2(%%rsi),%0\n\t"
		"jmp _wcsrchrl0\n\t"
		"_wcsrchrl1:\n\t"
		:"=r"(str)
		:"S"(str),"c"(c)
		:"ax"
	);
	return (wchar_t*)str;
}
size_t strlen(const char * str){
	register size_t len;
	asm("xorq %%rcx,%%rcx\n\t"
		"xorq %%rax,%%rax\n\t"
		"decq %%rcx\n\t"
		"repne\n\t"
		"scasb\n\t"
		"notq %%rcx\n\t"
		"decq %%rcx\n\t"
		:"=c"(len)
		:"D"(str)
		:"al"
	);
	return len;
}
size_t wcslen(const wchar_t * str){
	register size_t len;
	asm("xorq %%rcx,%%rcx\n\t"
		"xorq %%rax,%%rax\n\t"
		"decq %%rcx\n\t"
		"repne\n\t"
		"scasw\n\t"
		"notq %%rcx\n\t"
		"decq %%rcx\n\t"
		:"=c"(len)
		:"D"(str)
		:"ax"
	);
	return len;
}
void * memset(void * Des,int Val,size_t Count){
	int64_t val;
	void * _d = Des;
	char * p;
	size_t c;
	int64_t * p2;
	if(((int64_t)Des) & 0x07){
		p = (char*)Des;
		c = 8 - (((int64_t)Des) & 0x07);
		if(c > Count) c = Count;
		Des = p + c;
		Count -= c;
		while(c) {p[c - 1] = Val;c--;};
	}
	if(!Count) return Des;
	val = ((int64_t)Val) & 0xff;
	val |= val << 8;
	val |= val << 16;
	val |= val << 32;
	c = Count >> 3;
	p2 = (int64_t*)Des;
	Des = p2 + c;
	while(c) {p2[c - 1] = val;c--;}
	c = Count & 0x07;
	p = (char*)Des;
	while(c) {p[c - 1] = Val;c--;}
	return Des;
}
void * wmemset(void * Des,int Val,size_t Count){
	int64_t val;
	void * _d = Des;
	wchar_t * p;
	size_t c;
	int64_t * p2;
	if(((int64_t)Des) & 0x03){
		p = (wchar_t*)Des;
		c = 4 - (((int64_t)Des) & 0x03);
		Des = p + c;
		Count -= c;
		while(c) {p[c - 1] = Val;c--;};
	}
	val = ((int64_t)Val) & 0xffff;
	val |= val << 16;
	val |= val << 32;
	c = Count >> 2;
	p2 = (int64_t*)Des;
	Des = p2 + c;
	while(c) {p2[c - 1] = val;c--;}
	c = Count & 0x03;
	p = (wchar_t*)Des;
	while(c) {p[c - 1] = Val;c--;}
	return Des;
}
void * memcpy(void * d,void * s,size_t n){
	asm(
		"cmpq %%rsi,%%rdi\n\t"
		"jz _memcpyl0\n\t"
		"jb _memcpyl1\n\t"
		"leaq -1(%%rsi,%%rcx,1),%%rsi\n\t"
		"leaq -1(%%rdi,%%rcx,1),%%rdi\n\t"
		"std\n\t"
		"_memcpyl1:\n\t"
		"rep\n\t"
		"movsb\n\t"
		"cld\n\t"
		"_memcpyl0:\n\t"
		:"=D"(d)
		:"D"(d),"S"(s),"c"(n)
	);
	return d;
}
void * wmemcpy(void * d,void * s,size_t n){
	asm(
		"cmpq %%rsi,%%rdi\n\t"
		"jz _wmemcpyl0\n\t"
		"jb _wmemcpyl1\n\t"
		"leaq -2(%%rsi,%%rcx,2),%%rsi\n\t"
		"leaq -2(%%rdi,%%rcx,2),%%rdi\n\t"
		"std\n\t"
		"_wmemcpyl1:\n\t"
		"rep\n\t"
		"movsw\n\t"
		"cld\n\t"
		"_wmemcpyl0:\n\t"
		:"=D"(d)
		:"D"(d),"S"(s),"c"(n)
	);
	return d;
}
void * memmove(void * Des,void * Scr,size_t Count){
	asm("cmpq %%rsi,%%rdi\n\t"
		"jb _memmovel0\n\t"
		"je _memmovel1\n\t"
		"addq %%rcx,%%rsi\n\t"
		"addq %%rcx,%%rdi\n\t"
		"std\n\t"
		"_memmovel0:\n\t"
		"rep\n\t"
		"movsb\n\t"
		"cld\n\t"
		"_memmovel1:"
		:"=r"(Des)
		:"S"(Scr),"D"(Des),"c"(Count),"0"(Des)
	);
	return Des;
}
void * wmemmove(void * Des,void * Scr,size_t Count){
	asm("cmpq %%rsi,%%rdi\n\t"
		"jb _wmemmovel0\n\t"
		"je _wmemmovel1\n\t"
		"leaq -2(%%rsi,%%rcx,2),%%rsi\n\t"
		"leaq -2(%%rdi,%%rcx,2),%%rdi\n\t"
		"std\n\t"
		"_wmemmovel0:\n\t"
		"rep\n\t"
		"movsw\n\t"
		"cld\n\t"
		"_wmemmovel1:"
		:"=r"(Des)
		:"S"(Scr),"D"(Des),"c"(Count),"0"(Des)
	);
	return Des;
}
int memcmp(void * s1,void * s2,size_t n){
	register int ret;
	asm("repe\n\t"
		"cmpsb\n\t"
		"ja _memcmpl0\n\t"
		"jb _memcmpl1\n\t"
		"xorl %%eax,%%eax\n\t"
		"jmp _memcmpl2\n\t"
		"_memcmpl0:\n\t"
		"movl $1,%%eax\n\t"
		"jmp _memcmpl2\n\t"
		"_memcmpl1:\n\t"
		"movl $-1,%%eax\n\t"
		"_memcmpl2:\n\t"
		:"=a"(ret)
		:"S"(s1),"D"(s2),"c"(n)
	);
	return ret;
}
int wmemcmp(void * s1,void * s2,size_t n){
	register int ret;
	asm("repe\n\t"
		"cmpsw\n\t"
		"ja _wmemcmpl0\n\t"
		"jb _wmemcmpl1\n\t"
		"xorl %%eax,%%eax\n\t"
		"jmp _wmemcmpl2\n\t"
		"_wmemcmpl0:\n\t"
		"movl $1,%%eax\n\t"
		"jmp _wmemcmpl2\n\t"
		"_wmemcmpl1:\n\t"
		"movl $-1,%%eax\n\t"
		"_wmemcmpl2:\n\t"
		:"=a"(ret)
		:"S"(s1),"D"(s2),"c"(n)
	);
	return ret;
}
void * memchr(void * s,int ch,size_t n){
	asm("_memchrl0:\n\t"
		"testq %0,%0\n\t"
		"jz _memchrl1\n\t"
		"lodsb\n\t"
		"cmpb %%al,%%cl\n\t"
		"jne _memchrl0\n\t"
		"lea -1(%%rsi),%%rsi\n\t"
		"jmp _memchrl2\n\t"
		"_memchrl1:\n\t"
		"xorq %%rsi,%%rsi\n\t"
		"_memchrl2:\n\t"
		:"=S"(s)
		:"S"(s),"c"(ch),"r"(n)
		:"al"
	);
	return s;
}
void * wmemchr(void * s,int ch,size_t n){
	asm("_wmemchrl0:\n\t"
		"testq %0,%0\n\t"
		"jz _wmemchrl1\n\t"
		"lodsw\n\t"
		"cmpw %%ax,%%cx\n\t"
		"jne _wmemchrl0\n\t"
		"lea -2(%%rsi),%%rsi\n\t"
		"jmp _wmemchrl2\n\t"
		"_wmemchrl1:\n\t"
		"xorq %%rsi,%%rsi\n\t"
		"_wmemchrl2:\n\t"
		:"=S"(s)
		:"S"(s),"c"(ch),"r"(n)
		:"ax"
	);
	return s;
}