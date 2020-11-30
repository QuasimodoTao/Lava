/*
	arch/interrupt.c
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

.globl __int00
.globl __int01
.globl __int02
.globl __int03
.globl __int04
.globl __int05
.globl __int06
.globl __int07
.globl __int08
.globl __int09
.globl __int0a
.globl __int0b
.globl __int0c
.globl __int0d
.globl __int0e
.globl __int0f
.globl __int10
.globl __int11
.globl __int12
.globl __int13
.globl __int14
.globl __int15
.globl __int16
.globl __int17
.globl __int18
.globl __int19
.globl __int1a
.globl __int1b
.globl __int1c
.globl __int1d
.globl __int1e
.globl __int1f

.globl __irq00
.globl __irq01
.globl __irq02
.globl __irq03
.globl __irq04
.globl __irq05
.globl __irq06
.globl __irq07
.globl __irq08
.globl __irq09
.globl __irq0a
.globl __irq0b
.globl __irq0c
.globl __irq0d
.globl __irq0e
.globl __irq0f
.globl __irq10
.globl __irq11
.globl __irq12
.globl __irq13
.globl __irq14
.globl __irq15
.globl __irq16
.globl __irq17
.globl __irq18
.globl __irq19
.globl __irq1a
.globl __irq1b
.globl __irq1c
.globl __irq1d
.globl __irq1e
.globl __irq1f
.globl __irq20
.globl __irq21
.globl __irq22
.globl __irq23
.globl __irq24
.globl __irq25
.globl __irq26
.globl __irq27
.globl __irq28
.globl __irq29
.globl __irq2a
.globl __irq2b
.globl __irq2c
.globl __irq2d
.globl __irq2e
.globl __irq2f

.globl __ipi00
.globl __ipi01
.globl __ipi02
.globl __ipi03
.globl __ipi04
.globl __ipi05
.globl __ipi06
.globl __ipi07
.globl __ipi08
.globl __ipi09
.globl __ipi0a
.globl __ipi0b
.globl __ipi0c
.globl __ipi0d
.globl __ipi0e
.globl __ipi0f

.def divide_error;	.scl	2;	.type	32;	.endef

.text
.align 8
__int00:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.divide_error(%rip), %r14
    movb $0,%r13b
    jmp _int_routine
.align 8
__int01:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.debug_exception(%rip), %r14
    movb $1,%r13b
    jmp _int_routine
.align 8
__int02:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.NMI_int(%rip), %r14
    movb $2,%r13b
    jmp _int_routine
.align 8
__int03:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.break_point(%rip), %r14
    movb $3,%r13b
    jmp _int_routine
.align 8
__int04:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.overflow(%rip), %r14
    movb $4,%r13b
    jmp _int_routine
.align 8
__int05:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.bound_range_exceeded(%rip), %r14
    movb $5,%r13b
    jmp _int_routine
.align 8
__int06:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.invaild_opcode(%rip), %r14
    movb $6,%r13b
    jmp _int_routine
.align 8
__int07:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.no_math_coprocessor(%rip), %r14
    movb $7,%r13b
    jmp _int_routine
.align 8
__int08:
    xchgq %r15,(%rsp)
    pushq %r14
    pushq %r13
    movq .refptr.double_fault(%rip), %r14
    movb $8,%r13b
    jmp _int_routine
.align 8
__int09:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.coprocessor_segment_overrun(%rip), %r14
    movb $9,%r13b
    jmp _int_routine
.align 8
__int0a:
    xchgq %r15,(%rsp)
    pushq %r14
    pushq %r13
    movq .refptr.invaild_TSS(%rip), %r14
    movb $10,%r13b
    jmp _int_routine
.align 8
__int0b:
    xchgq %r15,(%rsp)
    pushq %r14
    pushq %r13
    movq .refptr.segment_not_present(%rip), %r14
    movb $11,%r13b
    jmp _int_routine
.align 8
__int0c:
    xchgq %r15,(%rsp)
    pushq %r14
    pushq %r13
    movq .refptr.stack_segment_fault(%rip), %r14
    movb $12,%r13b
    jmp _int_routine
.align 8
__int0d:
    xchgq %r15,(%rsp)
    pushq %r14
    pushq %r13
    movq .refptr.general_protection(%rip), %r14
    movb $13,%r13b
    jmp _int_routine
.align 8
__int0e:
    xchgq %r15,(%rsp)
    pushq %r14
    pushq %r13
    movq .refptr.page_fault(%rip), %r14
    movb $14,%r13b
    jmp _int_routine
.align 8
__int0f:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.int_ignore(%rip), %r14
    movb $15,%r13b
    jmp _int_routine
.align 8
__int10:
    xchgq %r15,(%rsp)
    pushq %r14
    pushq %r13
    movq .refptr.FPU_error(%rip), %r14
    movb $16,%r13b
    jmp _int_routine
.align 8
__int11:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.alignment_check(%rip), %r14
    movb $17,%r13b
    jmp _int_routine
.align 8
__int12:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.machine_check(%rip), %r14
    movb $18,%r13b
    jmp _int_routine
.align 8
__int13:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.SIMD_FP_exception(%rip), %r14
    movb $19,%r13b
    jmp _int_routine
.align 8
__int14:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.virtualization_exception(%rip), %r14
    movb $20,%r13b
    jmp _int_routine
.align 8
__int15:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.int_ignore(%rip), %r14
    movb $21,%r13b
    jmp _int_routine
.align 8
__int16:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.int_ignore(%rip), %r14
    movb $22,%r13b
    jmp _int_routine
.align 8
__int17:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.int_ignore(%rip), %r14
    movb $23,%r13b
    jmp _int_routine
.align 8
__int18:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.int_ignore(%rip), %r14
    movb $24,%r13b
    jmp _int_routine
.align 8
__int19:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.int_ignore(%rip), %r14
    movb $25,%r13b
    jmp _int_routine
.align 8
__int1a:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.int_ignore(%rip), %r14
    movb $26,%r13b
    jmp _int_routine
.align 8
__int1b:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.int_ignore(%rip), %r14
    movb $27,%r13b
    jmp _int_routine
.align 8
__int1c:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.int_ignore(%rip), %r14
    movb $28,%r13b
    jmp _int_routine
.align 8
__int1d:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.int_ignore(%rip), %r14
    movb $29,%r13b
    jmp _int_routine
.align 8
__int1e:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.int_ignore(%rip), %r14
    movb $30,%r13b
    jmp _int_routine
.align 8
__int1f:
    pushq %r15
    pushq %r14
    pushq %r13
    movq .refptr.int_ignore(%rip), %r14
    movb $31,%r13b
_int_routine:
    pushq %r12
    pushq %r11
    pushq %r10
    pushq %r9
    pushq %r8
    pushq %rdi
    pushq %rsi
    pushq %rbp
    pushq %r15
    pushq %rbx
    pushq %rdx
    pushq %rcx
    pushq %rax
    movq %rsp,%rbx
    movzx %r13b,%rdx
    movq %rsp,%rcx
    subq $32,%rsp
    call *%r14
    movq %rbx,%rsp
    popq %rax
    popq %rcx
    popq %rdx
    popq %rbx
    addq $8,%rsp
    popq %rbp
    popq %rsi
    popq %rdi
    popq %r8
    popq %r9
    popq %r10
    popq %r11
    popq %r12
    popq %r13
    popq %r14
    popq %r15
    iretq

.align 8
__irq00:
    pushq %rax
    movb $0,%al
    jmp _irq_routine
.align 8
__irq01:
    pushq %rax
    movb $1,%al
    jmp _irq_routine
.align 8
__irq02:
    pushq %rax
    movb $2,%al
    jmp _irq_routine
.align 8
__irq03:
    pushq %rax
    movb $3,%al
    jmp _irq_routine
.align 8
__irq04:
    pushq %rax
    movb $4,%al
    jmp _irq_routine
.align 8
__irq05:
    pushq %rax
    movb $5,%al
    jmp _irq_routine
.align 8
__irq06:
    pushq %rax
    movb $6,%al
    jmp _irq_routine
.align 8
__irq07:
    pushq %rax
    movb $7,%al
    jmp _irq_routine
.align 8
__irq08:
    pushq %rax
    movb $8,%al
    jmp _irq_routine
.align 8
__irq09:
    pushq %rax
    movb $9,%al
    jmp _irq_routine
.align 8
__irq0a:
    pushq %rax
    movb $10,%al
    jmp _irq_routine
.align 8
__irq0b:
    pushq %rax
    movb $11,%al
    jmp _irq_routine
.align 8
__irq0c:
    pushq %rax
    movb $12,%al
    jmp _irq_routine
.align 8
__irq0d:
    pushq %rax
    movb $13,%al
    jmp _irq_routine
.align 8
__irq0e:
    pushq %rax
    movb $14,%al
    jmp _irq_routine
.align 8
__irq0f:
    pushq %rax
    movb $15,%al
    jmp _irq_routine
.align 8
__irq10:
    pushq %rax
    movb $16,%al
    jmp _irq_routine
.align 8
__irq11:
    pushq %rax
    movb $17,%al
    jmp _irq_routine
.align 8
__irq12:
    pushq %rax
    movb $18,%al
    jmp _irq_routine
.align 8
__irq13:
    pushq %rax
    movb $19,%al
    jmp _irq_routine
.align 8
__irq14:
    pushq %rax
    movb $20,%al
    jmp _irq_routine
.align 8
__irq15:
    pushq %rax
    movb $21,%al
    jmp _irq_routine
.align 8
__irq16:
    pushq %rax
    movb $22,%al
    jmp _irq_routine
.align 8
__irq17:
    pushq %rax
    movb $23,%al
    jmp _irq_routine
.align 8
__irq18:
    pushq %rax
    movb $24,%al
    jmp _irq_routine
.align 8
__irq19:
    pushq %rax
    movb $25,%al
    jmp _irq_routine
.align 8
__irq1a:
    pushq %rax
    movb $26,%al
    jmp _irq_routine
.align 8
__irq1b:
    pushq %rax
    movb $27,%al
    jmp _irq_routine
.align 8
__irq1c:
    pushq %rax
    movb $28,%al
    jmp _irq_routine
.align 8
__irq1d:
    pushq %rax
    movb $29,%al
    jmp _irq_routine
.align 8
__irq1e:
    pushq %rax
    movb $30,%al
    jmp _irq_routine
.align 8
__irq1f:
    pushq %rax
    movb $31,%al
    jmp _irq_routine
.align 8
__irq20:
    pushq %rax
    movb $32,%al
    jmp _irq_routine
.align 8
__irq21:
    pushq %rax
    movb $33,%al
    jmp _irq_routine
.align 8
__irq22:
    pushq %rax
    movb $34,%al
    jmp _irq_routine
.align 8
__irq23:
    pushq %rax
    movb $35,%al
    jmp _irq_routine
.align 8
__irq24:
    pushq %rax
    movb $36,%al
    jmp _irq_routine
.align 8
__irq25:
    pushq %rax
    movb $37,%al
    jmp _irq_routine
.align 8
__irq26:
    pushq %rax
    movb $38,%al
    jmp _irq_routine
.align 8
__irq27:
    pushq %rax
    movb $39,%al
    jmp _irq_routine
.align 8
__irq28:
    pushq %rax
    movb $40,%al
    jmp _irq_routine
.align 8
__irq29:
    pushq %rax
    movb $41,%al
    jmp _irq_routine
.align 8
__irq2a:
    pushq %rax
    movb $42,%al
    jmp _irq_routine
.align 8
__irq2b:
    pushq %rax
    movb $43,%al
    jmp _irq_routine
.align 8
__irq2c:
    pushq %rax
    movb $44,%al
    jmp _irq_routine
.align 8
__irq2d:
    pushq %rax
    movb $45,%al
    jmp _irq_routine
.align 8
__irq2e:
    pushq %rax
    movb $46,%al
    jmp _irq_routine
.align 8
__irq2f:
    pushq %rax
    movb $47,%al
_irq_routine:
    pushq %r12
    pushq %r11
    pushq %r10
    pushq %r9
    pushq %r8
    pushq %rdx
    pushq %rcx
    pushq %rbx
    movq %rsp,%rbx
    subq $32,%rsp
    movzx %al,%rcx
    call irq_arise
    movq %rbx,%rsp
    popq %rbx
    popq %rcx
    popq %rdx
    popq %r8
    popq %r9
    popq %r10
    popq %r11
    popq %r12
    popq %rax
    iretq

.align 8
__ipi00:
    pushq %rax
    movb $0,%al
    jmp _ipi_routine
.align 8
__ipi01:
    pushq %rax
    movb $1,%al
    jmp _ipi_routine
.align 8
__ipi02:
    pushq %rax
    movb $2,%al
    jmp _ipi_routine
.align 8
__ipi03:
    pushq %rax
    movb $3,%al
    jmp _ipi_routine
.align 8
__ipi04:
    pushq %rax
    movb $4,%al
    jmp _ipi_routine
.align 8
__ipi05:
    pushq %rax
    movb $5,%al
    jmp _ipi_routine
.align 8
__ipi06:
    pushq %rax
    movb $6,%al
    jmp _ipi_routine
.align 8
__ipi07:
    pushq %rax
    movb $7,%al
    jmp _ipi_routine
.align 8
__ipi08:
    pushq %rax
    movb $8,%al
    jmp _ipi_routine
.align 8
__ipi09:
    pushq %rax
    movb $9,%al
    jmp _ipi_routine
.align 8
__ipi0a:
    pushq %rax
    movb $10,%al
    jmp _ipi_routine
.align 8
__ipi0b:
    pushq %rax
    movb $11,%al
    jmp _ipi_routine
.align 8
__ipi0c:
    pushq %rax
    movb $12,%al
    jmp _ipi_routine
.align 8
__ipi0d:
    pushq %rax
    movb $13,%al
    jmp _ipi_routine
.align 8
__ipi0e:
    pushq %rax
    movb $14,%al
    jmp _ipi_routine
.align 8
__ipi0f:
    pushq %rax
    movb $15,%al
_ipi_routine:
    pushq %r12
    pushq %r11
    pushq %r10
    pushq %r9
    pushq %r8
    pushq %rdx
    pushq %rcx
    pushq %rbx
    movq %rsp,%rbx
    subq $32,%rsp
    movzx %al,%rcx
    call ipi_arise
    movq %rbx,%rsp
    popq %rbx
    popq %rcx
    popq %rdx
    popq %r8
    popq %r9
    popq %r10
    popq %r11
    popq %r12
    popq %rax
    iretq

.align 16
.globl __back_to_user_mode
/*void back_to_user_mode(u64 rip,u64 rflags,u64 rsp,u64 cs)*/
__bakc_to_user_mode:
    movq %r9,%r11
    addq $8,%r9
    pushq %r9
    pushq %r8
    pushq %rdx
    pushq %r11
    pushq %rcx
    iretq

.align 16
.globl __syscall
__syscall:
    movq %rsp,%r10
    movl $4,%esp
    gs
    movq (%rsp),%rsp
    pushq %r10
    pushq %rcx
    pushq %rbx
    movq %rax,%rcx
    movq %rsp,%rbx
    subq $32,%rsp
    call sys_call
    movq %rbx,%rsp
    popq %rcx
    popq %rsp
    .byte 0x48,0x0f,0x07/*sysret*/

.align 16
.globl __sysenter
__sysenter:
    pushq %r10
    pushq %rax
    pushq %rbx
    movq %rsp,%rbx
    subq $32,%rsp
    call sys_call
    movq %rbx,%rsp
    popq %rdx
    popq %rcx
    .byte 0x48,0x0f,0x35/*sysexit*/

.align 16
.globl user_syscall
user_syscall:
    movq $0x00007ffffffff8f0,%rax
    cmpb $0,(%rax)
    jnz support_syscall
    cmpb $0,8(%rax)
    jnz support_sysenter
    int $0x81
    ret
support_syscall:
    movq %rcx,%rax
    syscall
    ret
support_sysenter:
    popq %rax
    movq %r10,%rsp
    sysenter

.align 16
.globl __sys_call_80
__sys_call_80:
    subq $32,%rsp
    call sys_call_80
    addq $32,%rsp
    iretq

.align 16
.globl __sys_call_81
__sys_call_81:
    subq $32,%rsp
    call sys_call
    addq $32,%rsp
    iretq
.align 16
.globl __sys_call_82
__sys_call_82:
    subq $32,%rsp
    call sys_call_82
    addq $32,%rsp
    iretq
.align 16
.globl __sys_call_83
__sys_call_83:
    subq $32,%rsp
    call sys_call_83
    addq $32,%rsp
    iretq

.align 16
.globl switch_cs
switch_cs:
    pop %rax
    push %rcx
    push %rax
    .byte 0x48,0xcb/*retf*/



	.section	.rdata$.refptr.divide_error, "dr"
	.globl	.refptr.divide_error
	.linkonce	discard
.refptr.divide_error:
	.quad	divide_error

	.section	.rdata$.refptr.debug_exception, "dr"
	.globl	.refptr.debug_exception
	.linkonce	discard
.refptr.debug_exception:
	.quad	debug_exception
	.section	.rdata$.refptr.NMI_int, "dr"
	.globl	.refptr.NMI_int
	.linkonce	discard
.refptr.NMI_int:
	.quad	NMI_int
	.section	.rdata$.refptr.break_point, "dr"
	.globl	.refptr.break_point
	.linkonce	discard
.refptr.break_point:
	.quad	break_point
	.section	.rdata$.refptr.overflow, "dr"
	.globl	.refptr.overflow
	.linkonce	discard
.refptr.overflow:
	.quad	overflow
	.section	.rdata$.refptr.bound_range_exceeded, "dr"
	.globl	.refptr.bound_range_exceeded
	.linkonce	discard
.refptr.bound_range_exceeded:
	.quad	bound_range_exceeded
	.section	.rdata$.refptr.invaild_opcode, "dr"
	.globl	.refptr.invaild_opcode
	.linkonce	discard
.refptr.invaild_opcode:
	.quad	invaild_opcode
	.section	.rdata$.refptr.no_math_coprocessor, "dr"
	.globl	.refptr.no_math_coprocessor
	.linkonce	discard
.refptr.no_math_coprocessor:
	.quad	no_math_coprocessor
	.section	.rdata$.refptr.double_fault, "dr"
	.globl	.refptr.double_fault
	.linkonce	discard
.refptr.double_fault:
	.quad	double_fault
	.section	.rdata$.refptr.coprocessor_segment_overrun, "dr"
	.globl	.refptr.coprocessor_segment_overrun
	.linkonce	discard
.refptr.coprocessor_segment_overrun:
	.quad	coprocessor_segment_overrun
	.section	.rdata$.refptr.invaild_TSS, "dr"
	.globl	.refptr.invaild_TSS
	.linkonce	discard
.refptr.invaild_TSS:
	.quad	invaild_TSS
	.section	.rdata$.refptr.segment_not_present, "dr"
	.globl	.refptr.segment_not_present
	.linkonce	discard
.refptr.segment_not_present:
	.quad	segment_not_present
	.section	.rdata$.refptr.stack_segment_fault, "dr"
	.globl	.refptr.stack_segment_fault
	.linkonce	discard
.refptr.stack_segment_fault:
	.quad	stack_segment_fault
	.section	.rdata$.refptr.general_protection, "dr"
	.globl	.refptr.general_protection
	.linkonce	discard
.refptr.general_protection:
	.quad	general_protection
	.section	.rdata$.refptr.page_fault, "dr"
	.globl	.refptr.page_fault
	.linkonce	discard
.refptr.page_fault:
	.quad	page_fault
	.section	.rdata$.refptr.int_ignore, "dr"
	.globl	.refptr.int_ignore
	.linkonce	discard
.refptr.int_ignore:
	.quad	int_ignore
	.section	.rdata$.refptr.FPU_error, "dr"
	.globl	.refptr.FPU_error
	.linkonce	discard
.refptr.FPU_error:
	.quad	FPU_error
	.section	.rdata$.refptr.alignment_check, "dr"
	.globl	.refptr.alignment_check
	.linkonce	discard
.refptr.alignment_check:
	.quad	alignment_check
	.section	.rdata$.refptr.machine_check, "dr"
	.globl	.refptr.machine_check
	.linkonce	discard
.refptr.machine_check:
	.quad	machine_check
	.section	.rdata$.refptr.SIMD_FP_exception, "dr"
	.globl	.refptr.SIMD_FP_exception
	.linkonce	discard
.refptr.SIMD_FP_exception:
	.quad	SIMD_FP_exception
	.section	.rdata$.refptr.virtualization_exception, "dr"
	.globl	.refptr.virtualization_exception
	.linkonce	discard
.refptr.virtualization_exception:
	.quad	virtualization_exception
