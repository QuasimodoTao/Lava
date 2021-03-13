
/*
	arch/asm.c
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


#include <asm.h>
#include <stddef.h>
int cmpxchg16b(__m128 ptr,__m128 cval,__m128 dval,__m128 rval){
	u8 cond;
	static u16 cmpxchg16b_lock = 0;
	static volatile u16 cmpxchg16b_init = 0;
	static int support_cmpxchg16b;
	u64 hash;
	int _cpuid[4];
	u64 rf;
	
	if(!cmpxchg1b(&cmpxchg16b_init,0,1,NULL)){
		cpuid(1,0,_cpuid);
		if(bt(&_cpuid[2],13)) support_cmpxchg16b = 1;
		else support_cmpxchg16b = 0;
		cmpxchg16b_init = 2;
	}
	while(cmpxchg16b_init == 1) pause();
	if(support_cmpxchg16b){
		if(rval){
			asm volatile (
				"lock\n\t"
				"cmpxchg16b (%3)\n\t"
				"setnz %0\n\t"
				:"=r"(cond),"=b"(*(u64*)rval),"=c"(*(((u64*)rval)+1))
				:"r"(ptr),"b"(*(u64*)dval),"c"(*(((u64*)dval)+1)),"a"(*(u64*)cval),"d"(*(((u64*)cval)+1))
			);
		}
		else{
			asm volatile (
				"lock\n\t"
				"cmpxchg16b (%1)\n\t"
				"setnz %0\n\t"
				:"=r"(cond)
				:"r"(ptr),"b"(*(u64*)dval),"c"(*(((u64*)dval)+1)),"a"(*(u64*)cval),"d"(*(((u64*)cval)+1))
			);
			
		}
		return cond;
	}
	else{
		hash = (u64)ptr;
		hash ^= hash >> 32;
		hash ^= hash >> 16;
		hash ^= hash >> 8;
		hash ^= hash >> 4;
		hash &= 0x0000000f;
		SFI(rf);
		while(bts(&cmpxchg16b_lock,hash));
		if(((u64*)ptr)[0] == ((u64*)cval)[0] &&
			((u64*)ptr)[1] == ((u64*)cval)[1]){
			((u64*)ptr)[0] = ((u64*)dval)[0];
			((u64*)ptr)[1] = ((u64*)dval)[1];
			LF(rf);
			btr(&cmpxchg16b_lock,hash);
			return 0;
		}
		else{
			if(rval){
				((u64*)rval)[0] = ((u64*)ptr)[0];
				((u64*)rval)[1] = ((u64*)ptr)[1];
			}
			btr(&cmpxchg16b_lock,hash);
			LF(rf);
			return 1;
		}
	}
}
int cmpxchg1b(__m8 ptr,__i8 cval,__i8 dval,__m8 rval){
	u8 cond;
	if(rval){
		asm volatile (
			"lock\n\t"
			"cmpxchgb %3,%2\n\t"
			"setnz %0\n\t"
			:"=r"(cond),"=a"(*rval)
			:"m"(*ptr),"r"(dval),"a"(cval)
		);
	}
	else{
		asm volatile (
			"lock\n\t"
			"cmpxchgb %2,%1\n\t"
			"setnz %0\n\t"
			:"=r"(cond)
			:"m"(*ptr),"r"(dval),"a"(cval)
		);
	}
	return cond;
}
int cmpxchg2b(__m16 ptr,__i16 cval,__i16 dval,__m16 rval){
	u8 cond;
	if(rval){
		asm volatile (
			"lock\n\t"
			"cmpxchgw %3,%2\n\t"
			"setnz %0\n\t"
			:"=r"(cond),"=a"(*rval)
			:"m"(*ptr),"r"(dval),"a"(cval)
		);
	}
	else{
		asm volatile (
			"lock\n\t"
			"cmpxchgw %2,%1\n\t"
			"setnz %0\n\t"
			:"=r"(cond)
			:"m"(*ptr),"r"(dval),"a"(cval)
		);
	}
	return cond;
}
int cmpxchg4b(__m32 ptr,__i32 cval,__i32 dval,__m32 rval){
	u8 cond;
	if(rval){
		asm volatile (
			"lock\n\t"
			"cmpxchgl %3,%2\n\t"
			"setnz %0\n\t"
			:"=r"(cond),"=a"(*rval)
			:"m"(*ptr),"r"(dval),"a"(cval)
		);
	}
	else{
		asm volatile (
			"lock\n\t"
			"cmpxchgl %2,%1\n\t"
			"setnz %0\n\t"
			:"=r"(cond)
			:"m"(*ptr),"r"(dval),"a"(cval)
		);
	}
	return cond;
}
int cmpxchg8b(__m64 ptr,__i64 cval,__i64 dval,__m64 rval){
	u8 cond;
	if(rval){
		asm volatile (
			"lock\n\t"
			"cmpxchgq %3,%2\n\t"
			"setnz %0\n\t"
			:"=r"(cond),"=a"(*rval)
			:"m"(*ptr),"r"(dval),"a"(cval)
		);
	}
	else{
		asm volatile (
			"lock\n\t"
			"cmpxchgq %2,%1\n\t"
			"setnz %0\n\t"
			:"=r"(cond)
			:"m"(*ptr),"r"(dval),"a"(cval)
		);
	}
	return cond;
}