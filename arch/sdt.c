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

void switch_cs(u16);

void __int00(void);
void __int01(void);
void __int02(void);
void __int03(void);
void __int04(void);
void __int05(void);
void __int06(void);
void __int07(void);
void __int08(void);
void __int09(void);
void __int0a(void);
void __int0b(void);
void __int0c(void);
void __int0d(void);
void __int0e(void);
void __int0f(void);
void __int10(void);
void __int11(void);
void __int12(void);
void __int13(void);
void __int14(void);
void __int15(void);
void __int16(void);
void __int17(void);
void __int18(void);
void __int19(void);
void __int1a(void);
void __int1b(void);
void __int1c(void);
void __int1d(void);
void __int1e(void);
void __int1f(void);

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
int sdt_init_bp(){
	int i;
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
	switch_cs(KERNEL_CS);
	
	idtr.limit = 4095;
	idtr.base0 = (u64)idt & 0xffff;
	idtr.base1 = ((u64)idt >> 16) & 0xffff;
	idtr.base2 = ((u64)idt >> 32) & 0xffff;
	idtr.base3 = ((u64)idt >> 48) & 0xffff;
	make_gate(0,__int00,0,0,GATE_TYPE_TRAP);
	make_gate(1,__int01,0,0,GATE_TYPE_TRAP);
	make_gate(2,__int02,0,0,GATE_TYPE_INT);
	make_gate(3,__int03,0,3,GATE_TYPE_TRAP);
	make_gate(4,__int04,0,3,GATE_TYPE_TRAP);
	make_gate(5,__int05,0,3,GATE_TYPE_TRAP);
	make_gate(6,__int06,0,0,GATE_TYPE_TRAP);
	make_gate(7,__int07,0,0,GATE_TYPE_TRAP);
	make_gate(8,__int08,1,0,GATE_TYPE_INT);
	make_gate(9,__int09,0,0,GATE_TYPE_TRAP);
	make_gate(10,__int0a,0,0,GATE_TYPE_TRAP);
	make_gate(11,__int0b,0,0,GATE_TYPE_TRAP);
	make_gate(12,__int0c,0,0,GATE_TYPE_TRAP);
	make_gate(13,__int0d,0,0,GATE_TYPE_TRAP);
	make_gate(14,__int0e,0,0,GATE_TYPE_TRAP);
	make_gate(15,__int0f,0,0,GATE_TYPE_TRAP);
	make_gate(16,__int10,0,0,GATE_TYPE_TRAP);
	make_gate(17,__int11,0,0,GATE_TYPE_TRAP);
	make_gate(18,__int12,0,0,GATE_TYPE_TRAP);
	make_gate(19,__int13,0,0,GATE_TYPE_TRAP);
	make_gate(20,__int14,0,0,GATE_TYPE_TRAP);
	make_gate(21,__int15,0,0,GATE_TYPE_TRAP);
	make_gate(22,__int16,0,0,GATE_TYPE_TRAP);
	make_gate(23,__int17,0,0,GATE_TYPE_TRAP);
	make_gate(24,__int18,0,0,GATE_TYPE_TRAP);
	make_gate(25,__int19,0,0,GATE_TYPE_TRAP);
	make_gate(26,__int1a,0,0,GATE_TYPE_TRAP);
	make_gate(27,__int1b,0,0,GATE_TYPE_TRAP);
	make_gate(28,__int1c,0,0,GATE_TYPE_TRAP);
	make_gate(29,__int1d,0,0,GATE_TYPE_TRAP);
	make_gate(30,__int1e,0,0,GATE_TYPE_TRAP);
	make_gate(31,__int1f,0,0,GATE_TYPE_TRAP);
	
	idtel = idt[30];
	idteh = idt[31];
	for(i = 21;i < 256;i++){
		idt[i * 2] = idtel;
		idt[i * 2 + 1] = idteh;
	}
	lidt(&idtr);
	return 0;
}
void sdt_init_ap(){
	lgdt(&gdtr);
	lidt(&idtr);
	sss(KERNEL_SS);
	switch_cs(KERNEL_CS);
}