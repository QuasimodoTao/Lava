	.file	"ahci.c"
	.text
	.def	ahci_restart;	.scl	3;	.type	32;	.endef
	.seh_proc	ahci_restart
ahci_restart:
	.seh_endprologue
	movq	24(%rdx), %rdx
	movl	4(%rdx), %eax
	orl	$1, %eax
	movl	%eax, 4(%rdx)
	xorl	%eax, %eax
	ret
	.seh_endproc
	.def	ahci_power_on;	.scl	3;	.type	32;	.endef
	.seh_proc	ahci_power_on
ahci_power_on:
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	xorl	%ebx, %ebx
	movq	%rdx, %rsi
.L3:
	cmpl	%ebx, 20(%rsi)
	jle	.L10
	movq	32(%rsi,%rbx,8), %rdx
	testq	%rdx, %rdx
	je	.L4
	movq	%rsi, %rcx
	call	ata_on_ahci_power_on
.L4:
	incq	%rbx
	jmp	.L3
.L10:
	xorl	%eax, %eax
	addq	$40, %rsp
	popq	%rbx
	popq	%rsi
	ret
	.seh_endproc
	.def	ahci_power_off;	.scl	3;	.type	32;	.endef
	.seh_proc	ahci_power_off
ahci_power_off:
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	xorl	%ebx, %ebx
	movq	%rdx, %rsi
.L12:
	cmpl	%ebx, 20(%rsi)
	jle	.L18
	movq	32(%rsi,%rbx,8), %rdx
	testq	%rdx, %rdx
	je	.L13
	movq	%rsi, %rcx
	call	ata_on_ahci_power_off
.L13:
	incq	%rbx
	jmp	.L12
.L18:
	xorl	%eax, %eax
	addq	$40, %rsp
	popq	%rbx
	popq	%rsi
	ret
	.seh_endproc
	.def	ahci_close;	.scl	3;	.type	32;	.endef
	.seh_proc	ahci_close
ahci_close:
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	xorl	%ebx, %ebx
	movq	%rdx, %rsi
.L20:
	cmpl	%ebx, 20(%rsi)
	jle	.L26
	movq	32(%rsi,%rbx,8), %rdx
	testq	%rdx, %rdx
	je	.L21
	movq	%rsi, %rcx
	call	ata_on_ahci_close
.L21:
	incq	%rbx
	jmp	.L20
.L26:
	xorl	%eax, %eax
	addq	$40, %rsp
	popq	%rbx
	popq	%rsi
	ret
	.seh_endproc
	.def	ahci_int_handle;	.scl	3;	.type	32;	.endef
	.seh_proc	ahci_int_handle
ahci_int_handle:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	andb	$8, %r8b
	movq	%rdx, %rsi
	je	.L39
	movq	24(%rdx), %rax
	xorl	%ebx, %ebx
	movl	$1, %ebp
	movl	8(%rax), %edi
.L29:
	cmpl	%ebx, 20(%rsi)
	jle	.L39
	movl	%ebp, %eax
	movl	%ebx, %ecx
	sall	%cl, %eax
	testl	%edi, %eax
	je	.L30
	movq	32(%rsi,%rbx,8), %rdx
	testq	%rdx, %rdx
	je	.L30
	movq	%rsi, %rcx
	call	ata_on_ahci_int_handle
.L30:
	incq	%rbx
	jmp	.L29
.L39:
	xorl	%eax, %eax
	addq	$40, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.globl	ahci_open
	.def	ahci_open;	.scl	2;	.type	32;	.endef
	.seh_proc	ahci_open
ahci_open:
	pushq	%r12
	.seh_pushreg	%r12
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$32, %rsp
	.seh_stackalloc	32
	.seh_endprologue
	leaq	busy(%rip), %rax
	movq	%rax, %r12
	movq	%rcx, %rsi
	xorl	%ecx, %ecx
.L41:
/APP
 # 123 "../inc/asm.h" 1
	lock
	btsq %rcx,(%rax)
	setc %dl
 # 0 "" 2
/NO_APP
	testb	%dl, %dl
	jne	.L41
	movq	ahci_list(%rip), %rbx
.L42:
	testq	%rbx, %rbx
	je	.L43
	cmpq	%rsi, 8(%rbx)
	je	.L45
	movq	(%rbx), %rbx
	jmp	.L42
.L43:
/APP
 # 125 "../inc/asm.h" 1
	lock
	btrq %rbx,(%r12)
 # 0 "" 2
/NO_APP
	movl	$36, %edx
	movq	%rsi, %rcx
	call	pci_read_dword
	movl	$4352, %edx
	movl	%eax, %ecx
	call	mmio_map
	xorl	%edx, %edx
	movl	(%rax), %edi
	movq	%rax, %rbp
	andl	$31, %edi
	leal	1(%rdi), %eax
	leaq	32(,%rax,8), %rcx
	movq	%rax, %rdi
	call	kmalloc
	movq	%rax, %rbx
	xorl	%eax, %eax
.L46:
/APP
 # 123 "../inc/asm.h" 1
	lock
	btsq %rax,(%r12)
	setc %dl
 # 0 "" 2
/NO_APP
	testb	%dl, %dl
	jne	.L46
	movq	ahci_list(%rip), %rax
	movq	%rbx, ahci_list(%rip)
	movq	%rax, (%rbx)
.L45:
	xorl	%eax, %eax
/APP
 # 125 "../inc/asm.h" 1
	lock
	btrq %rax,(%r12)
 # 0 "" 2
/NO_APP
	movl	$6, %edx
	movq	%rsi, %rcx
	movq	%rsi, 8(%rbx)
	movl	$1, %r12d
	movq	%rbp, 24(%rbx)
	call	pci_read_word
	movq	%rsi, %rcx
	movl	$6, %edx
	orw	$774, %ax
	movzwl	%ax, %r8d
	call	pci_write_word
	movl	4(%rbp), %eax
	orl	$-2147483648, %eax
	movl	%eax, 4(%rbp)
	leaq	ahci_int_handle(%rip), %rax
	movl	$0, 16(%rbx)
	movq	%rax, 24(%rsi)
	leaq	ahci_restart(%rip), %rax
	movq	%rax, 32(%rsi)
	leaq	ahci_close(%rip), %rax
	movq	%rax, 40(%rsi)
	leaq	ahci_power_off(%rip), %rax
	movq	%rax, 48(%rsi)
	leaq	ahci_power_on(%rip), %rax
	movq	%rax, 56(%rsi)
	xorl	%esi, %esi
.L48:
	movl	12(%rbp), %eax
	movl	%r12d, %edx
	movl	%esi, %ecx
	sall	%cl, %edx
	testl	%eax, %edx
	je	.L47
	movl	%esi, %edx
	movq	%rbx, %rcx
	call	ata_on_ahci_open
	movq	%rax, 32(%rbx,%rsi,8)
.L47:
	incq	%rsi
	cmpl	%esi, %edi
	jg	.L48
	movq	%rbx, %rax
	addq	$32, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r12
	ret
	.seh_endproc
.lcomm busy,4,4
.lcomm ahci_list,8,8
	.comm	system_enter_page, 8, 3
	.comm	current_user, 8, 3
	.comm	global_flags, 4, 2
	.comm	kernel_time, 8, 3
	.comm	processor_count, 4, 2
	.comm	suppose_sysenter, 4, 2
	.comm	suppose_syscall, 4, 2
	.comm	init_msg, 224, 5
	                                                                        
	.def	ata_on_ahci_power_on;	.scl	2;	.type	32;	.endef
	.def	ata_on_ahci_power_off;	.scl	2;	.type	32;	.endef
	.def	ata_on_ahci_close;	.scl	2;	.type	32;	.endef
	.def	ata_on_ahci_int_handle;	.scl	2;	.type	32;	.endef
	.def	pci_read_dword;	.scl	2;	.type	32;	.endef
	.def	mmio_map;	.scl	2;	.type	32;	.endef
	.def	kmalloc;	.scl	2;	.type	32;	.endef
	.def	pci_read_word;	.scl	2;	.type	32;	.endef
	.def	pci_write_word;	.scl	2;	.type	32;	.endef
	.def	ata_on_ahci_open;	.scl	2;	.type	32;	.endef
