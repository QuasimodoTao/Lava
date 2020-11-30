/*
	arch/sdt.c
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
#include <asm.h>
#include <string.h>
#include <arch.h>
#include <config.h>
#include <mm.h>
#include <lava.h>
#include <spinlock.h>

#define GDT_SIZE			(MAX_CPU_COUNT * 16 + FIRST_SELECTOR + 6 * 8)
#define FIRST_IRQ_VECTOR	32
#define FIRST_SYS_VECTOR	(32 + IRQ_COUNT)
#define LAST_INT_GATE		(31 + IRQ_COUNT + SYS_CALL_COUNT)

#define IDT_SIZE			(LAST_INT_GATE * 0x10 + 0x10)
#define gdt					((u64*)GDT_BASE)
#define idt					((u64*)IDT_BASE)

extern u8 __int_start[];//
extern int __int_size;//
extern int __int_enter;
//extern int __int_kernel_ss_0;//
//extern int __int_kernel_ss_1;//

extern u8 __int_code_start[];//
extern int __int_code_size;//
extern int __int_code_enter;
//extern int __int_code_kernel_ss_0;//
//extern int __int_code_kernel_ss_1;//

void int_ignore(struct _REG_STATUS_ * status);
void divide_error(struct _REG_STATUS_ * status);
void debug_exception(struct _REG_STATUS_ * stack);
void NMI_int(struct _REG_STATUS_ * status);
void break_point(struct _REG_STATUS_ * status);
void overflow(struct _REG_STATUS_ * status);
void bound_range_exceeded(struct _REG_STATUS_ * status);
void invaild_opcode(struct _REG_STATUS_ * status);
void no_math_coprocessor(struct _REG_STATUS_ * status);
void double_fault(struct _REG_STATUS_ * status);
void coprocessor_segment_overrun(struct _REG_STATUS_ * status);
void invaild_TSS(struct _REG_STATUS_ * status);
void segment_not_present(struct _REG_STATUS_ * status);
void stack_segment_fault(struct _REG_STATUS_ * status);
void general_protection(struct _REG_STATUS_ * status);
void page_fault(struct _REG_STATUS_ * status);
void FPU_error(struct _REG_STATUS_ * status);
void alignment_check(struct _REG_STATUS_ * status);
void machine_check(struct _REG_STATUS_ * status);
void SIMD_FP_exception(struct _REG_STATUS_ * status);
void virtualization_exception(struct _REG_STATUS_ * status);
void switch_CS(u16);
void irq_arise(int);

static struct {
	u16 limit;
	u16 base0;
	u16 base1;
	u16 base2;
	u16 base3;
} gdtr,idtr;

static u32 gdt_first_free;
u64 int_ent_last_address;

void make_error(){
	idtr.limit = 0;
	lidt(&idtr);
	asm("ud2");
}

u16 put_TSS(struct _TSS_64_ * tss){
	u64 dtl,dth;
	static int lock = 0;
	u64 base;
	u16 selector;
	
	base = (u64)tss;
	dtl = 0x0000890000000000 | sizeof(struct _TSS_64_);
	dtl |= (base << 16) & 0x000000ffffff0000;
	dtl |= (base << 32) & 0xff00000000000000;
	dth = base >> 32;
	spin_lock_bit(&lock,0);
	selector = gdt_first_free;
	gdt[gdt_first_free] = dtl;
	gdt[gdt_first_free + 1] = dth;
	gdt_first_free += 2;
	spin_unlock_bit(&lock,0);
	selector <<= 3;
	return selector;
}
void make_gate(int vector,u64 addr,int ist,int dpl,int type){
	u64 idtel = 0x0000800000100000;
	
	type &= 0x0f;
	dpl &= 0x03;
	vector &= 0xff;
	idtel |= addr & 0x000000000000ffff;
	idtel |= (addr << 32) & 0xffff000000000000;
	idtel |= ((u64)ist) << 32;
	idtel |= ((u64)type) << (32 + 8);
	idtel |= ((u64)dpl) << (32 + 13);
	idt[vector * 2] = idtel;
	idt[vector * 2 + 1] = (addr >> 32) & 0x00000000ffffffff;

}

#define MakeInt(vector,dpl,ist,type,addr)	{\
	make_gate(vector,INT_ENT_BASE + vector * int_ent_size,ist,dpl,type);\
	memcpy((void*)(INT_ENT_BASE + vector * int_ent_size),__int_start,int_ent_size);\
	*(void**)(INT_ENT_BASE + vector * int_ent_size + __int_enter) = addr;\
}
#define MakeCodeInt(vector,dpl,ist,type,addr)	{\
	make_gate(vector,INT_ENT_BASE + vector * int_ent_size,ist,dpl,type);\
	memcpy((void*)(INT_ENT_BASE + vector * int_ent_size),__int_code_start,int_ent_size);\
	*(void**)(INT_ENT_BASE + vector * int_ent_size + __int_code_enter) = addr;\
}

void sdt_init_bp(){
	int i;
	u64 * ent;
	u8 * vector;
	void * addr;
	int int_ent_size;
	u64 idteh,idtel;
	
	gdtr.limit = GDT_SIZE - 1;
	gdtr.base0 = (u64)gdt & 0xffff;
	gdtr.base1 = ((u64)gdt >> 16) & 0xffff;
	gdtr.base2 = ((u64)gdt >> 32) & 0xffff;
	gdtr.base3 = ((u64)gdt >> 48) & 0xffff;
	memset(gdt,0,GDT_SIZE);
	gdt[FIRST_SELECTOR >> 3] = 0x00209a0000000000;//Code L0 64-bits
	gdt[(FIRST_SELECTOR >> 3) + 1] = 0x0020920000000000;//Data L0 64-bits
	gdt[(FIRST_SELECTOR >> 3) + 2] = 0x00cffa000000ffff;//Code L3 32-bits
	gdt[(FIRST_SELECTOR >> 3) + 3] = 0x00cff2000000ffff;//Data L3 32-bits
	gdt[(FIRST_SELECTOR >> 3) + 4] = 0x0020fa0000000000;//Code L3 64-bits
	gdt[(FIRST_SELECTOR >> 3) + 5] = 0x0020f20000000000;//Data L3 64-bits;
	gdt_first_free = (FIRST_SELECTOR >> 3) + 6;
	lgdt(&gdtr);
	sss(KERNEL_SS);
	switch_CS(KERNEL_CS);
	
	idtr.limit = 4095;
	idtr.base0 = (u64)idt & 0xffff;
	idtr.base1 = ((u64)idt >> 16) & 0xffff;
	idtr.base2 = ((u64)idt >> 32) & 0xffff;
	idtr.base3 = ((u64)idt >> 48) & 0xffff;
//	__int_start[__int_kernel_ss_0] = __int_start[__int_kernel_ss_1] = KERNEL_SS;
//	__int_code_start[__int_code_kernel_ss_0] = __int_code_start[__int_code_kernel_ss_1] = KERNEL_SS;
	int_ent_size = __int_size;
	if(int_ent_size < __int_code_size) int_ent_size = __int_code_size;
	int_ent_size = NATURALALIGNU(int_ent_size);
	for(i = 0;i < int_ent_size * 21 + int_ent_size;i+= PAGE_SIZE) put_page(get_free_page(0,0),NULL,(void*)(INT_ENT_BASE + i));
	int_ent_last_address = INT_ENT_BASE + int_ent_size * 21;
	MakeInt(0,0,0,0x0e,divide_error);//divide_error
	MakeInt(1,0,0,0x0e,debug_exception);//debug_exception
	MakeInt(2,0,0,0x0f,NMI_int);//NMI_int
	MakeInt(3,3,0,0x0e,break_point);//break_point
	MakeInt(4,3,0,0x0f,overflow);//overflow
	MakeInt(5,3,0,0x0e,bound_range_exceeded);//bound_range_exceeded
	MakeInt(6,0,0,0x0e,invaild_opcode);//invaild_opcode
	MakeInt(7,0,0,0x0f,no_math_coprocessor);//no_math_coprocessor
	MakeCodeInt(8,0,0,0x0f,double_fault);//double_fault
	MakeInt(9,0,0,0x0f,coprocessor_segment_overrun);//coprocessor_segment_overrun
	MakeCodeInt(10,0,0,0x0f,invaild_TSS);//invaild_TSS
	MakeCodeInt(11,0,0,0x0f,segment_not_present);//segment_not_present
	MakeCodeInt(12,0,0,0x0f,stack_segment_fault);//stack_segment_fault
	MakeCodeInt(13,0,0,0x0f,general_protection);//general_protection
	MakeCodeInt(14,0,0,0x0f,page_fault);//page_fault
	MakeInt(15,0,0,0x0f,int_ignore);//int_ignore
	MakeCodeInt(16,0,0,0x0e,FPU_error);//FPU_error
	MakeInt(17,0,0,0x0e,alignment_check);//alignment_check
	MakeInt(18,0,0,0x0f,machine_check);//machine_check
	MakeInt(19,0,0,0x0e,SIMD_FP_exception);//SIMD_FP_exception
	MakeInt(20,0,0,0x0f,virtualization_exception);//virtualization_exception
	
	idtel = idt[30];
	idteh = idt[31];
	for(i = 21;i < 256;i++){
		idt[i * 2] = idtel;
		idt[i * 2 + 1] = idteh;
	}
	lidt(&idtr);
	sti();
}
void sdt_init_ap(){
	lgdt(&gdtr);
	lidt(&idtr);
	sss(KERNEL_SS);
	switch_CS(KERNEL_CS);
}