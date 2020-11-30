/*
	arch/int.c
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
#include <arch.h>
#include <asm.h>
#include <stdio.h>

int paging_fault(struct _REG_STATUS_ *);

void show_register(struct _REG_STATUS_ * stack){
	printk("rax:%016llX rcx:%016llX rdx:%016llX\n",stack->rax,stack->rcx,stack->rdx);
	printk("rbx:%016llX rsp:%016llX rbp:%016llX\n",stack->rbx,stack->rsp,stack->rbp);
	printk("rsi:%016llX rdi:%016llX  r8:%016llX\n",stack->rsi,stack->rdi,stack->r8);
	printk(" r9:%016llX r10:%016llX r11:%016llX\n",stack->r9,stack->r10,stack->r11);
	printk("r12:%016llX r13:%016llX r14:%016llX\n",stack->r12,stack->r13,stack->r14);
	printk("r15:%016llX rip:%016llX rflgas:%016llX\n",stack->r15,stack->rip,stack->rflags);
	printk("err:%016llX cs:%04X ss:%04X\n",stack->err_code,stack->cs,stack->ss);
}

void int_ignore(struct _REG_STATUS_ * stack,u64 vector){
	print("ignore int arise.\n");
	show_register(stack);
	stop();
}
void divide_error(struct _REG_STATUS_ * stack){
	print("divide.\n");
	show_register(stack);
	stop();
}
void debug_exception(struct _REG_STATUS_ * stack){
	print("debug.\n");
	show_register(stack);
	stop();
}
void NMI_int(struct _REG_STATUS_ * stack){
	print("nmi.\n");
	show_register(stack);
	stop();
}
void break_point(struct _REG_STATUS_ * stack){
	print("bp.\n");
	show_register(stack);
	stop();
}
void overflow(struct _REG_STATUS_ * stack){
	print("of.\n");
	show_register(stack);
	stop();
}
void bound_range_exceeded(struct _REG_STATUS_ * stack){
	print("BoundRangeExceeded.\n");
	show_register(stack);
	stop();
}
void invaild_opcode(struct _REG_STATUS_ * stack){
	print("invaild_opcode.\n");
	show_register(stack);
	stop();
}
void no_math_coprocessor(struct _REG_STATUS_ * stack){
	print("no_math_coprocessor.\n");
	show_register(stack);
	stop();
}
void double_fault(struct _REG_STATUS_ * stack){
	print("double_fault.\n");
	show_register(stack);
	stop();
}
void coprocessor_segment_overrun(struct _REG_STATUS_ * stack){
	print("coprocessor_segment_overrun.\n");
	show_register(stack);
	stop();
}
void invaild_TSS(struct _REG_STATUS_ * stack){
	print("invaild_tss.\n");
	show_register(stack);
	stop();
}
void segment_not_present(struct _REG_STATUS_ * stack){
	print("segment_not_present.\n");
	show_register(stack);
	stop();
}
void stack_segment_fault(struct _REG_STATUS_ * stack){
	print("stack_segment_fault\n");
	show_register(stack);
	stop();
}
void general_protection(struct _REG_STATUS_ * stack){
	print("general_protection.\n");
	show_register(stack);
	stop();
}
void page_fault(struct _REG_STATUS_ * stack){
	paging_fault(stack);
}
void FPU_error(struct _REG_STATUS_ * stack){
	print("fpu_error.\n");
	show_register(stack);
	stop();
}
void alignment_check(struct _REG_STATUS_ * stack){
	print("alignment_check.\n");
	show_register(stack);
	stop();
}
void machine_check(struct _REG_STATUS_ * stack){
	print("machine_check.\n");
	show_register(stack);
	stop();
}
void SIMD_FP_exception(struct _REG_STATUS_ * stack){
	print("simdfp_exception.\n");
	show_register(stack);
	stop();
}
void virtualization_exception(struct _REG_STATUS_ * stack){
	print("virtualization_exception.\n");
	show_register(stack);
	stop();
}