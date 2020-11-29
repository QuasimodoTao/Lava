/*Lava OS*/
/*PageFault*/
/*20-04-11-15-03*/

.globl thread_exit
thread_exit:
/*thread_exit(u64 rsp,void entry,LPTHREAD thread*/
	movq %rcx,%rsp
	movq %rdx,%rcx
	movq %r8,%rdx
	ret

.globl switch_task_b
/*LPTHREAD switch_task_b(u64 * osp,u64 nsp,LPTHREAD thread)*/
switch_task_b:
	pushq %r15
	pushq %r14
	pushq %r13
	pushq %r12
	pushq %rdi
	pushq %rsi
	pushq %rbp
	pushq %rbx
	pushq %rcx
	movq %rsp,(%rcx)
	movq %rdx,%rsp
	movq %r8,%rax
	movq %r8,%rdx
	popq %rcx
	popq %rbx
	popq %rbp
	popq %rsi
	popq %rdi
	popq %r12
	popq %r13
	popq %r14
	popq %r15
	ret

