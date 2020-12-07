/*
	inc/asm.h
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

#ifndef _ASM_H_
#define _ASM_H_

#include <stddef.h>

#define port_in_data(ta,name,asm_ins,asm_reg) \
static inline ta name(u16 port){ \
	register ta data; \
	asm volatile(\
		#asm_ins " %%dx,%%" #asm_reg\
		:"=a"(data):"d"(port)\
	); \
	return data;\
}
#define port_out_data(ta,name,asm_ins,asm_reg) \
static inline void name(u16 port,ta data){\
	asm volatile(\
		#asm_ins " %%" #asm_reg ",%%dx"\
		::"d"(port),"a"(data)\
	);\
}

#define movs_data(name,asm_ins) \
static inline void name(void * des,void * scr,size_t count){\
	asm volatile(\
		"rep\n\t" #asm_ins \
		::"S"(scr),"D"(des),"c"(count)\
	);\
}
#define stos_data(name,asm_ins,ta) \
static inline void name(void * des,ta val,size_t count){\
	asm volatile(\
		"rep\n\t" #asm_ins \
		::"D"(des),"a"(val),"c"(count)\
	);\
}
#define set_mr(name,asm_reg) \
static inline void name(u64 val){\
	asm volatile(\
		"movq %0,%%" #asm_reg \
		::"r"(val)\
	);\
}
#define get_mr(name,asm_reg) \
static inline u64 name(){\
	register u64 val;\
	asm volatile(\
		"movq %%" #asm_reg ",%0" \
		:"=r"(val)::\
	);\
	return val;\
}
#define single_ins(name,asm_ins) \
static inline void name(){\
	asm volatile(#asm_ins);\
}
#define bit_optr(name,asm_ins) \
static inline u8 name(void * ptr,s64 index){\
	register u8 cond;\
	asm volatile(\
		#asm_ins " %2,%1\n\t"\
		"setc %0"\
		:"=r"(cond)\
		:"m"(*(u8*)ptr),"r"(index)\
		:"cc","memory"\
	);\
	return cond;\
}
#define bit_naked_optr(name,asm_ins) \
static inline u8 name(void * ptr,s64 index){\
	asm volatile(\
		#asm_ins " %1,%0"\
		::"m"(*(u8*)ptr),"r"(index)\
		:"cc","memory"\
	);\
}
#define lock_bit_optr(name,asm_ins) \
static inline u8 name(void * ptr,s64 index){\
	register u8 cond;\
	asm volatile(\
		"lock\n\t"\
		#asm_ins " %2,%1\n\t"\
		"setc %0"\
		:"=r"(cond)\
		:"m"(*(u8*)ptr),"r"(index)\
		:"cc","memory"\
	);\
	return cond;\
}
#define lock_bit_naked_optr(name,asm_ins) \
static inline u8 name(void * ptr,s64 index){\
	asm volatile(\
		"lock\n\t"\
		#asm_ins " %1,%0"\
		::"m"(*(u8*)ptr),"r"(index)\
		:"cc","memory"\
	);\
}
#define bit_optr_seg(name,asm_ins,seg) \
static inline u8 name(size_t off,s64 index){\
	register u8 cond;\
	asm volatile(\
		#seg "\n\t"\
		#asm_ins "q %2,(%1)\n\t"\
		"setc %0"\
		:"=r"(cond)\
		:"r"(off),"r"(index)\
		:"cc","memory"\
	);\
	return cond;\
}
#define bit_naked_optr_seg(name,asm_ins,seg) \
static inline u8 name(size_t off,s64 index){\
	asm volatile(\
		#seg "\n\t"\
		#asm_ins " %1,(%0)"\
		::"r"(off),"r"(index)\
		:"cc","memory"\
	);\
}
	
#define lock_bit_optr_seg(name,asm_ins,seg)  \
static inline u8 name(size_t off,s64 index){\
	register u8 cond;\
	asm volatile(\
		"lock\n\t"\
		#seg "\n\t"\
		#asm_ins "q %2,(%1)\n\t"\
		"setc %0"\
		:"=r"(cond)\
		:"r"(off),"r"(index)\
		:"cc","memory"\
	);\
	return cond;\
}
#define lock_bit_naked_optr_seg(name,asm_ins,seg) \
static inline u8 name(size_t off,s64 index){\
	asm volatile(\
		"lock\n\t"\
		#seg "\n\t"\
		#asm_ins " %1,(%0)"\
		::"r"(off),"r"(index)\
		:"cc","memory"\
	);\
}
#define get_seg_data(name,ta,asm_ins,seg) \
static inline ta name(size_t off){\
	register ta data;\
	asm volatile(\
		#seg "\n\t"\
		#asm_ins " (%1),%0"\
		:"=r"(data)\
		:"r"(off)\
	);\
	return data;\
}
#define set_seg_data(name,ta,asm_ins,seg) \
static inline void name(size_t off,ta data){\
	asm volatile(\
		#seg "\n\t"\
		#asm_ins " %1,(%0)"\
		::"r"(off),"r"(data)\
	);\
}
#define xchg_data(name,ta,asm_ins) \
static inline ta name(ta * ptr,ta data){\
	asm volatile(\
		"lock\n\t"\
		#asm_ins " %0,%1"\
		:"=r"(data)\
		:"m"(*(ta*)ptr),"0"(data)\
	);\
	return data;\
}
#define xchg_data_seg(name,ta,asm_ins,seg) \
static inline ta name(void * ptr,ta data){\
	asm volatile(\
		"lock\n\t"\
		#seg "\n\t"\
		#asm_ins " %0,(%1)"\
		:"=r"(data)\
		:"r"(ptr),"0"(data)\
	);\
	return data;\
}	
#define ins_data(name,asm_ins) \
static inline void name(void * ptr,u16 port,size_t count){\
	asm volatile(\
		"rep\n\t"\
		#asm_ins\
		::"D"(ptr),"c"(count),"d"(port)\
		:"memory"\
	);\
}
#define outs_data(name,asm_ins) \
static inline void name(void * ptr,u16 port,size_t count){\
	asm volatile(\
		"rep\n\t"\
		#asm_ins\
		::"S"(ptr),"c"(count),"d"(port)\
		:"memory"\
	);\
}
#define xadd(name,ta,asm_ins) \
static inline volatile ta name(ta * ptr,ta data){\
	asm volatile(\
		"lock\n\t"\
		#asm_ins " %0,%1"\
		:"=r"(data)\
		:"m"(*(ta*)ptr),"0"(data)\
	);\
	return data;\
}
#define xadd_seg(name,ta,asm_ins,seg) \
static inline ta name(ta * ptr,ta data){\
	asm volatile(\
		"lock\n\t"\
		#seg "\n\t"\
		#asm_ins " %0,(%1)"\
		:"=r"(data)\
		:"0"(data),"r"(ptr)\
	);\
	return data;\
}
#define lock_and(name,ta,asm_ins) \
static inline void name(ta * ptr,ta data){\
	asm volatile(\
		"lock\n\t"\
		#asm_ins " %0,%1"\
		::"r"(data),"m"(*(ta*)ptr)\
	);\
}
#define lock_and_seg(name,ta,asm_ins,seg) \
static inline void name(ta * ptr,ta data){\
	asm volatile(\
		#seg "\n\t"\
		"lock\n\t"\
		#asm_ins " %0,(%1)"\
		::"r"(data),"r"(ptr)\
	);\
}
#define lock_or(name,ta,asm_ins) \
static inline void name(ta * ptr,ta data){\
	asm volatile(\
		"lock\n\t"\
		#asm_ins " %0,%1"\
		::"r"(data),"m"(*(ta*)ptr)\
	);\
}
#define lock_or_seg(name,ta,asm_ins,seg) \
static inline void name(ta * ptr,ta data){\
	asm volatile(\
		#seg "\n\t"\
		"lock\n\t"\
		#asm_ins " %0,(%1)"\
		::"r"(data),"r"(ptr)\
	);\
}
#define lock_add(name,ta,asm_ins) \
static inline void name(ta * ptr,ta data){\
	asm volatile(\
		"lock\n\t"\
		#asm_ins " %0,%1"\
		::"r"(data),"m"(*(ta*)ptr)\
	);\
}
#define lock_add_seg(name,ta,asm_ins,seg) \
static inline void name(ta * ptr,ta data){\
	asm volatile(\
		#seg "\n\t"\
		"lock\n\t"\
		#asm_ins " %0,(%1)"\
		::"r"(data),"r"(ptr)\
	);\
}
lock_and(lock_andb,u8,andb);
lock_and(lock_andw,u16,andw);
lock_and(lock_andd,u32,andl);
lock_and(lock_andq,u64,andq);

lock_and_seg(lock_fs_andb,u8,andb,fs);
lock_and_seg(lock_fs_andw,u16,andw,fs);
lock_and_seg(lock_fs_andd,u32,andl,fs);
lock_and_seg(lock_fs_andq,u64,andq,fs);
lock_and_seg(lock_gs_andb,u8,andb,gs);
lock_and_seg(lock_gs_andw,u16,andw,gs);
lock_and_seg(lock_gs_andd,u32,andl,gs);
lock_and_seg(lock_gs_andq,u64,andq,gs);

lock_or(lock_orb,u8,orb);
lock_or(lock_orw,u16,orw);
lock_or(lock_ord,u32,orl);
lock_or(lock_orq,u64,orq);
lock_or_seg(lock_fs_orb,u8,orb,fs);
lock_or_seg(lock_fs_orw,u16,orw,fs);
lock_or_seg(lock_fs_ord,u32,orl,fs);
lock_or_seg(lock_fs_orq,u64,orq,fs);
lock_or_seg(lock_gs_orb,u8,orb,gs);
lock_or_seg(lock_gs_orw,u16,orw,gs);
lock_or_seg(lock_gs_ord,u32,orl,gs);
lock_or_seg(lock_gs_orq,u64,orq,gs);

lock_add(lock_addb,u8,addb);
lock_add(lock_addw,u16,addw);
lock_add(lock_addd,u32,addl);
lock_add(lock_addq,u64,addq);
lock_add_seg(lock_fs_addb,u8,addb,fs);
lock_add_seg(lock_fs_addw,u16,addw,fs);
lock_add_seg(lock_fs_addd,u32,addl,fs);
lock_add_seg(lock_fs_addq,u64,addq,fs);
lock_add_seg(lock_gs_addb,u8,addb,gs);
lock_add_seg(lock_gs_addw,u16,addw,gs);
lock_add_seg(lock_gs_addd,u32,addl,gs);
lock_add_seg(lock_gs_addq,u64,addq,gs);

xadd(xaddb,s8,xaddb);
xadd(xaddw,s16,xaddw);
xadd(xaddd,s32,xaddl);
xadd(xaddq,s64,xaddq);
xadd_seg(fs_xaddb,s8,xaddb,fs);
xadd_seg(fs_xaddw,s16,xaddw,fs);
xadd_seg(fs_xaddd,s32,xaddl,fs);
xadd_seg(fs_xaddq,s64,xaddq,fs);
xadd_seg(gs_xaddb,s8,xaddb,gs);
xadd_seg(gs_xaddw,s16,xaddw,gs);
xadd_seg(gs_xaddd,s32,xaddl,gs);
xadd_seg(gs_xaddq,s64,xaddq,gs);

bit_optr(bt,btq);
bit_optr(bts,btsq);
bit_optr(btr,btrq);
bit_optr(btc,btcq);
bit_optr_seg(fs_bt,bt,fs);
bit_optr_seg(fs_btr,btr,fs);
bit_optr_seg(fs_btc,btc,fs);
bit_optr_seg(fs_bts,bts,fs);
bit_optr_seg(gs_bt,bt,gs);
bit_optr_seg(gs_btr,btr,gs);
bit_optr_seg(gs_btc,btc,gs);
bit_optr_seg(gs_bts,bts,gs);

bit_naked_optr(naked_bts,btsq);
bit_naked_optr(naked_btr,btrq);
bit_naked_optr(naked_btc,btcq);
bit_naked_optr_seg(naked_fs_btr,btr,fs);
bit_naked_optr_seg(naked_fs_btc,btr,fs);
bit_naked_optr_seg(naked_fs_bts,btr,fs);
bit_naked_optr_seg(naked_gs_btr,btr,gs);
bit_naked_optr_seg(naked_gs_btc,btr,gs);
bit_naked_optr_seg(naked_gs_bts,btr,gs);

lock_bit_optr(lock_bts,btsq);
lock_bit_optr(lock_btr,btrq);
lock_bit_optr(lock_btc,btcq);
lock_bit_optr_seg(lock_fs_btr,btr,fs);
lock_bit_optr_seg(lock_fs_btc,btc,fs);
lock_bit_optr_seg(lock_fs_bts,bts,fs);
lock_bit_optr_seg(lock_gs_btr,btr,gs);
lock_bit_optr_seg(lock_gs_btc,btc,gs);
lock_bit_optr_seg(lock_gs_bts,bts,gs);

lock_bit_naked_optr(naked_lock_bts,btsq);
lock_bit_naked_optr(naked_lock_btr,btrq);
lock_bit_naked_optr(naked_lock_btc,btcq);
lock_bit_naked_optr_seg(lock_naked_fs_btr,btr,fs);
lock_bit_naked_optr_seg(lock_naked_fs_btc,btr,fs);
lock_bit_naked_optr_seg(lock_naked_fs_bts,btr,fs);
lock_bit_naked_optr_seg(lock_naked_gs_btr,btr,gs);
lock_bit_naked_optr_seg(lock_naked_gs_btc,btr,gs);
lock_bit_naked_optr_seg(lock_naked_gs_bts,btr,gs);

port_in_data(u8,in_byte,inb,al);
port_in_data(u16,in_word,inw,ax);
port_in_data(u32,in_dword,inl,eax);
port_in_data(u8,inb,inb,al);
port_in_data(u16,inw,inw,ax);
port_in_data(u32,ind,inl,eax);
port_out_data(u8,out_byte,outb,al);
port_out_data(u16,out_word,outw,ax);
port_out_data(u32,out_dword,outl,eax);
port_out_data(u8,outb,outb,al);
port_out_data(u16,outw,outw,ax);
port_out_data(u32,outd,outl,eax);

movs_data(movsb,movsb);
movs_data(movsw,movsw);
movs_data(movsd,movsl);
movs_data(movsq,movsq);
stos_data(stosb,stosb,u8);
stos_data(stosw,stosw,u16);
stos_data(stosd,stosl,u32);
stos_data(stosq,stosq,u64);

set_mr(set_cr0,cr0);
set_mr(set_cr2,cr2);
set_mr(set_cr3,cr3);
set_mr(set_cr4,cr4);
set_mr(set_cr8,cr8);
set_mr(set_dr0,dr0);
set_mr(set_dr1,dr1);
set_mr(set_dr2,dr2);
set_mr(set_dr3,dr3);
set_mr(set_dr6,dr6);
set_mr(set_dr7,dr7);
set_mr(put_cr0,cr0);
set_mr(put_cr2,cr2);
set_mr(put_cr3,cr3);
set_mr(put_cr4,cr4);
set_mr(put_cr8,cr8);
set_mr(put_dr0,dr0);
set_mr(put_dr1,dr1);
set_mr(put_dr2,dr2);
set_mr(put_dr3,dr3);
set_mr(put_dr6,dr6);
set_mr(put_dr7,dr7);
get_mr(get_cr0,cr0);
get_mr(get_cr2,cr2);
get_mr(get_cr3,cr3);
get_mr(get_cr4,cr4);
get_mr(get_cr8,cr8);
get_mr(get_dr0,dr0);
get_mr(get_dr1,dr1);
get_mr(get_dr2,dr2);
get_mr(get_dr3,dr3);
get_mr(get_dr6,dr6);
get_mr(get_dr7,dr7);

single_ins(cli,cli);
single_ins(sti,sti);
single_ins(cld,cld);
single_ins(std,std);
single_ins(clc,clc);
single_ins(stc,stc);
single_ins(cmc,cmc);
single_ins(ud2,ud2);
single_ins(clts,clts);
single_ins(halt,hlt);
single_ins(pause,pause);
single_ins(nop,nop);
single_ins(invd,invd);
single_ins(wbinvd,wbinvd);
single_ins(swapgs,swapgs);

get_seg_data(get_fs_byte,u8,movb,fs);
get_seg_data(get_fs_word,u16,movw,fs);
get_seg_data(get_fs_dword,u32,movl,fs);
get_seg_data(get_fs_qword,u64,movq,fs);
get_seg_data(get_gs_byte,u8,movb,gs);
get_seg_data(get_gs_word,u16,movw,gs);
get_seg_data(get_gs_dword,u32,movl,gs);
get_seg_data(get_gs_qword,u64,movq,gs);

set_seg_data(set_fs_byte,u8,movb,fs);
set_seg_data(set_fs_word,u16,movw,fs);
set_seg_data(set_fs_dword,u32,movl,fs);
set_seg_data(set_fs_qword,u64,movq,fs);
set_seg_data(set_gs_byte,u8,movb,gs);
set_seg_data(set_gs_word,u16,movw,gs);
set_seg_data(set_gs_dword,u32,movl,gs);
set_seg_data(set_gs_qword,u64,movq,gs);
set_seg_data(put_fs_byte,u8,movb,fs);
set_seg_data(put_fs_word,u16,movw,fs);
set_seg_data(put_fs_dword,u32,movl,fs);
set_seg_data(put_fs_qword,u64,movq,fs);
set_seg_data(put_gs_byte,u8,movb,gs);
set_seg_data(put_gs_word,u16,movw,gs);
set_seg_data(put_gs_dword,u32,movl,gs);
set_seg_data(put_gs_qword,u64,movq,gs);

xchg_data(xchgb,u8,xchgb);
xchg_data(xchgw,u16,xchgw);
xchg_data(xchgd,u32,xchgl);
xchg_data(xchgq,u64,xchgq);
xchg_data_seg(fs_xchgb,u8,xchgb,fs);
xchg_data_seg(fs_xchgw,u16,xchgw,fs);
xchg_data_seg(fs_xchgd,u32,xchgl,fs);
xchg_data_seg(fs_xchgq,u64,xchgq,fs);
xchg_data_seg(gs_xchgb,u8,xchgb,gs);
xchg_data_seg(gs_xchgw,u16,xchgw,gs);
xchg_data_seg(gs_xchgd,u32,xchgl,gs);
xchg_data_seg(gs_xchgq,u64,xchgq,gs);

ins_data(insb,insb);
ins_data(insw,insw);
ins_data(insd,insl);
outs_data(outsb,outsb);
outs_data(outsw,outsw);
outs_data(outsd,outsl);

#define lgdt(GDTR)	asm volatile("lgdt (%0)"::"r"(GDTR))
#define lidt(IDTR)	asm volatile("lidt (%0)"::"r"(IDTR))
#define ltr(Tr)		asm volatile("ltr %0"::"r"(Tr))
#define FreshPaging()	asm volatile("movq %%cr3,%%rax\n\tmovq %%rax,%%cr3":::"rax")
#define fpg()	FreshPaging()
#define invtlb(addr)	asm volatile("invlpg (%0)"::"r"(addr))
#define Stop()	asm volatile("cli\n\thlt")
#define stop()	Stop()
#define Rdmsr(Index)	({register u64 Reg;asm volatile("xorq %%rax,%%rax\n\trdmsr\n\tshlq $32,%%rdx\
	\n\torq %%rax,%%rdx":"=d"(Reg):"c"((u32)(Index)):"rax");Reg;})
#define Wrmsr(Index,Reg)	asm volatile("shrq $32,%%rdx\n\twrmsr\n\tmovq %%rax,%%rdx"::"a"(Reg),"d"(Reg),"c"((u32)(Index)))
#define rdmsr(Index)		Rdmsr(Index)
#define wrmsr(Index,Reg)	Wrmsr(Index,Reg)
#define Cpuid(Leaf,SubLeaf,Des)	asm volatile("cpuid\n\tmovl %%eax,(%0)\n\tmovl %%ebx,4(%0)\n\t"\
	"movl %%ecx,8(%0)\n\tmovl %%edx,12(%0)"::"r"((void*)(Des)),"a"((u32)(Leaf)),"c"((u32)(SubLeaf)):"ebx","edx")
#define cpuid(Leaf,SubLeaf,Des)	Cpuid(Leaf,SubLeaf,Des)
#define Bswapw(Val)	({register u16 Ret;asm volatile("bswapw %0":"=r"(Ret):"0"(Val):);Val;})
#define Bswapd(Val)	({register u32 Ret;asm volatile("bswapl %0":"=r"(Ret):"0"(Val):);Val;})
#define Bswapq(Val)	({register u64 Ret;asm volatile("bswapq %0":"=r"(Ret):"0"(Val):);Val;})
#define bswapw(Val)	Bswapw(Val)
#define bswapd(Val)	Bswapd(Val)
#define bswapq(Val)	Bswapq(Val)
#define Sflags()	({register u64 f;asm("pushfq\n\tpopq %0":"=r"(f)::);f;})
#define Lflags(f)	asm("pushq %0\n\tpopfq"::"r"((u64)(f)))
#define sflags()	Sflags()
#define lflags(f)	Lflags(f)
#define sss(v)		asm("movw %0,%%ss"::"r"((u16)(v)))

#define SFI(f)		asm("pushfq\n\tpopq %0\n\tcli\n\t":"=m"(f)::)
#define LF(f)		asm("pushq %0\n\tpopfq\n\t"::"m"(f):)

int cmpxchg16b(__m128 ptr,__m128 cval,__m128 dval,__m128 rval);
int cmpxchg1b(__m8 ptr,__i8 cval,__i8 dval,__m8 rval);
int cmpxchg2b(__m16 ptr,__i16 cval,__i16 dval,__m16 rval);
int cmpxchg4b(__m32 ptr,__i32 cval,__i32 dval,__m32 rval);
int cmpxchg8b(__m64 ptr,__i64 cval,__i64 dval,__m64 rval);

#endif