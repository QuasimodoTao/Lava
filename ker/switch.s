/*
	ker/switch.s
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

