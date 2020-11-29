/*Lava OS*/
/*PageFault*/
/*20-04-11-15-04*/

.text

.def __DivideError;	.scl	2;	.type	32;	.endef
.def __DivideError;	.scl	2;	.type	32;	.endef
.def __NMIInterrupt;	.scl	2;	.type	32;	.endef
.def __BreakPoint;	.scl	2;	.type	32;	.endef
.def __Overflow;	.scl	2;	.type	32;	.endef
.def __BoundRangeExceeded;	.scl	2;	.type	32;	.endef
.def __InvaildOpcode;	.scl	2;	.type	32;	.endef
.def __NoMathCoprocessor;	.scl	2;	.type	32;	.endef
.def __DoubleFault;	.scl	2;	.type	32;	.endef
.def __CoprocessorSegmentOverrun;	.scl	2;	.type	32;	.endef
.def __InvaildTSS;	.scl	2;	.type	32;	.endef
.def __SegmentNotPresent;	.scl	2;	.type	32;	.endef
.def __StackSegmentFault;	.scl	2;	.type	32;	.endef
.def __GeneralProtection;	.scl	2;	.type	32;	.endef
.def __PageFault;	.scl	2;	.type	32;	.endef
.def __FPUError;	.scl	2;	.type	32;	.endef
.def __AlignmentCheck;	.scl	2;	.type	32;	.endef
.def __MachineCheck;	.scl	2;	.type	32;	.endef
.def __SIMDFPException;	.scl	2;	.type	32;	.endef
.def __VirtualizationException;	.scl	2;	.type	32;	.endef
.def _IntEgnore;	.scl	2;	.type	32;	.endef

.globl __Int00
.align 16
__Int00:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 8(%rax),%rcx
	call __DivideError
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq
.globl __Int01
.align 16
__Int01:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 8(%rax),%rcx
	call __DivideError
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq
.globl __Int02
.align 16
__Int02:
	sti
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	call __NMIInterrupt
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq
.globl __Int03
.align 16
__Int03:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 8(%rax),%rcx
	call __BreakPoint
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq

.globl __Int04
.align 16
__Int04:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 8(%rax),%rcx
	call __Overflow
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq

.globl __Int05
.align 16
__Int05:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 8(%rax),%rcx
	call __BoundRangeExceeded
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq

.globl __Int06
.align 16
__Int06:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 8(%rax),%rcx
	call __InvaildOpcode
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq
.globl __Int07
.align 16
__Int07:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 16(%rax),%rcx
	movq 8(%rax),%rdx
	call __NoMathCoprocessor
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	addq $8,%rsp
	iretq
.globl __Int08
.align 16
__Int08:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 16(%rax),%rcx
	movq 8(%rax),%rdx
	call __DoubleFault
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	addq $8,%rsp
	iretq
.globl __Int09
.align 16
__Int09:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 8(%rax),%rcx
	call __CoprocessorSegmentOverrun
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq
.globl __Int0a
.align 16
__Int0a:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 16(%rax),%rcx
	movq 8(%rax),%rdx
	call __InvaildTSS
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	addq $8,%rsp
	iretq
.globl __Int0b
.align 16
__Int0b:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 16(%rax),%rcx
	movq 8(%rax),%rdx
	call __SegmentNotPresent
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	addq $8,%rsp
	iretq
.globl __Int0c
.align 16
__Int0c:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 16(%rax),%rcx
	movq 8(%rax),%rdx
	call __StackSegmentFault
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	addq $8,%rsp
	iretq
.globl __Int0d
.align 16
__Int0d:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 16(%rax),%rcx
	movq 8(%rax),%rdx
	call __GeneralProtection
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	addq $8,%rsp
	iretq
.globl __Int0e
.align 16
__Int0e:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 16(%rax),%rcx
	movq 8(%rax),%rdx
	call __PageFault
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	addq $8,%rsp
	iretq
.globl __Int10
.align 16
__Int10:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 8(%rax),%rcx
	call __FPUError
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq
.globl __Int11
.align 16
__Int11:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 16(%rax),%rcx
	movq 8(%rax),%rdx
	call __AlignmentCheck
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	addq $8,%rsp
	iretq
.globl __Int12
.align 16
__Int12:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 8(%rax),%rcx
	call __MachineCheck
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq
.globl __Int13
.align 16
__Int13:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 8(%rax),%rcx
	call __SIMDFPException
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq
.globl __Int14
.align 16
__Int14:
	sti
	pushq %rax
	movq %rsp,%rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	movq 8(%rax),%rcx
	call __VirtualizationException
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq
.globl __IntEgnore
.align 16
__IntEgnore:
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	movq %rsp,%rbx
	subq $0x20,%rsp
	call _IntEgnore
	movq %rbx,%rsp
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	iretq
