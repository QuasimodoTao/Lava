;
;	arch/inite.asm
;	Copyright (C) 2020  Quasimodo
;
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <https://www.gnu.org/licenses/>.
;


[cpu X64]
[bits 64]

db "BIN2C",0,0,0
dd 0
dd 12;array count
dd array0
dd array1
dd array2
dd array3
dd array4
dd array5
dd array6
dd array7
dd array8
dd array9
dd array10
dd array11

align 4
array11:
dd 0
dd switch_cs
dd array11_0
dd switch_cs_end - switch_cs
dd array11_1

array11_0:	db	"switch_CS",0
array11_1:	db	"switch_CS_size",0

switch_cs:
	pop rax
	push rcx
	push rax
	db 0x48
	retf
align 16
switch_cs_end:

align 4
array10:
dd 0
dd user_code
dd array10_0
dd user_code_end - user_code
dd array10_1

array10_0:	db	"user_code",0
array10_1:	db	"user_code_size",0
align 16
user_code:
	pause
	pause
	pause
	pause
	pause
	pause
	pause
	jmp user_code
align 16
user_code_end:

align 4
array9:
dd 0
dd back2user
dd array9_0
dd back2user_end - back2user
dd array9_1
array9_0:	db	"back_to_user_mode",0
array9_1:	db	"back_to_user_size",0
align 16
back2user:
;void back_to_user_mode(u64 rip,u64 rflags,u64 rsp,u64 cs);
	mov r11,r9
	add r9,8
	push r9;ss
	push r8;rsp
	push rdx;rflags
	push r11;cs
	push rcx;rip
	iretq
align 16
back2user_end:
array0:
dd 1;5 variable
dd __int_start;array_start
dd array0_0;array name
dd __int_end - __int_start;offset 0
dd array0_1;name
dd __int_enter - __int_start
dd array0_2
array0_0:	db	"__int_start",0
array0_1:	db	"__int_size",0
array0_2:	db	"__int_enter",0
align 16
__int_start:
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rdi
	push rsi
	push rbp
	sub rsp,8
	push rbx
	push rdx
	push rcx
	push rax
	mov rbx,rsp
	mov rcx,rsp
	sub rsp,0x20
	db 0x48,0xb8
__int_enter:
	dq 0
	call rax
	mov rsp,rbx
	pop rax
	pop rcx
	pop rdx
	pop rbx
	add rsp,8
	pop rbp
	pop rsi
	pop rdi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	iretq
align 16
__int_end:

align 4
array1:
dd 1
dd __int_code_start;array start
dd array1_0;array name
dd __int_code_end - __int_code_start
dd array1_1
dd __int_code_enter - __int_code_start
dd array1_2
array1_0:	db	"__int_code_start",0
array1_1:	db	"__int_code_size",0
array1_2:	db	"__int_code_enter",0
align 16
__int_code_start:
	xchg r15,[rsp]
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rdi
	push rsi
	push rbp
	push r15
	push rbx
	push rdx
	push rcx
	push rax
	mov rbx,rsp
	mov rcx,rsp
	sub rsp,0x20
	db 0x48,0xb8
__int_code_enter:
	dq 0
	call rax
	mov rsp,rbx
	pop rax
	pop rcx
	pop rdx
	pop rbx
	add rsp,8
	pop rbp
	pop rsi
	pop rdi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	iretq
align 16
__int_code_end:

align 4
array2:
dd 1
dd __syscall_start
dd array2_0
dd __syscall_end - __syscall_start
dd array2_1
dd __syscall_enter - __syscall_start
dd array2_2
array2_0:	db	"__syscall_start",0
array2_1:	db	"__syscall_size",0
array2_2:	db	"__syscall_enter",0
align 16
;void syscall(rcx,rdx,r8,r9);
;	mov rax,rcx
;	syscall
;	ret
;
;must set IA32_STAR & IA32_LSTAR & IA32_FMASK & IA32_EFER.SCE
;MSR[0xc0000081],MSR[0xc0000082],MSR[0xc0000084],MSR[0xc0000080].[0]
__syscall_start:
	mov r10,rsp
	mov esp,4
	mov rsp,[gs:rsp]
	push r10;rsp
	push rcx;rip
	mov rcx,rax
	sub rsp,0x20
	db 0x48,0xb8
__syscall_enter:
	dq 0
	call rax	
	add rsp,0x20
	pop rcx
	pop rsp
	db 0x48;constrain return to 64-bits code
	sysret
align 16
__syscall_end:

align 4
array3:
dd 1
dd __sysenter_start
dd array3_0
dd __sysenter_end - __sysenter_start
dd array3_1
dd __sysenter_enter - __sysenter_start
dd array3_2
array3_0:	db	"__sysenter_start",0
array3_1:	db	"__sysenter_size",0
array3_2:	db	"__sysenter_enter",0
align 16
;void sysenter(rcx,rdx,r8,r9);
;	mov rax,returnaddr
;	mov r10,rsp
;	sysenter
;returnaddr:
;	ret
;
;must set IA32_SYSENTER_ESP & IA32_SYSENTER_EIP & IA32_SYSENTER_CS
;MSR[0x175],MSR[0x176],MSR[0x174]
__sysenter_start:
	push r10;rsp
	push rax;rip
	sub rsp,0x20
	db 0x48,0xb8
__sysenter_enter:
	dq 0
	call rax
	add rsp,0x20
	pop rdx
	pop rcx
	db 0x48;constrain return to 64-bits code
	sysexit
align 16
__sysenter_end:

align 4
array4:
dd 1
dd AP_init
dd array4_0
dd AP_init_end - AP_init
dd array4_1
dd AP_enter - AP_init
dd array4_2
array4_0:	db	"AP_init",0
array4_1:	db	"AP_init_size",0
array4_2:	db	"AP_enter",0
align 16
AP_init:
[bits 16]
	cli
	xor ax,ax
	mov ds,ax			;segment address base = vector * 4096;segment value = vector * 256
	lgdt [GDTR - AP_init + 0x1000]			;load GDTR
	mov ax,1
	lmsw ax				;set CR0.PE
	jmp dword 0x18:_in32 - AP_init + 0x1000
[bits 32]
align 16
_in32:
	mov eax,0x00002000	;load top paging table
	mov cr3,eax
	
	mov eax,cr4			;set CR4.PAE
	or eax,0x20
	mov cr4,eax
	
	mov ecx,0xc0000080	;set IA32_EFER.LME
	rdmsr
	or eax,0x100
	wrmsr
	
	mov eax,cr0
	and eax,0x1fffffff
	bts eax,31
	mov cr0,eax			;set CR0.PG
	jmp 0x08:_in64 - AP_init + 0x1000		;ea 00 00 00 00 08 00
[bits 64]
align 16
_in64:
	mov eax,0x000010f0
_mutex_again:
	lock bts dword [rax],0	;wait for mutex
	jc _mutex_again
	mov bx,0x10
	mov ss,bx			;load stack segment
	mov rsp,0xffff80000000fff8
	db 0x48,0xb8
AP_enter:
	dq 0
	jmp rax			;jump to kernel
align 8
GDTR:	dw	0x1f
		dd	GDT - AP_init + 0x1000
align 8
GDT:	db	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
		db	0x00,0x00,0x00,0x00,0x00,0x9a,0x20,0x00	;64-bits code segment
		db	0x00,0x00,0x00,0x00,0x00,0x92,0x20,0x00	;64-bits data segment
		db	0xff,0xff,0x00,0x00,0x00,0x9a,0xcf,0x00	;32-bits code segment
		db	0xff,0xff,0x00,0x00,0x00,0x92,0xcf,0x00
align 16
AP_init_end:

align 4
array5:
dd 0
dd system_enter_start
dd array5_0
dd system_enter_end - system_enter_start
dd array5_1
array5_0:	db	"system_enter_start",0
array5_1:	db	"system_enter_size",0
align 16
system_enter_start:
	;u64 system_enter(u64 rcx,u64 rdx,u64 r8,u64 r9);
	;volatile rax,r10,r11
	mov rax,0x00007ffffffff0f0
	mov r10,[rax]
	test r10,r10
	jnz suppose_syscall
	mov r10,[rax + 8]
	test r10,r10
	jnz suppose_sysenter
	int 0x81;rcx,rdx,r8,r9
	ret
suppose_sysenter:
	pop rax
	mov r10,rsp
	sysenter;rcx,rdx,r8,r9
suppose_syscall:
	mov rax,rcx
	syscall;rax,rdx,r8,r9
	ret
align 16
system_enter_end:

align 4
array6:
dd 1
dd __int_sys_start
dd array6_0
dd __int_sys_end - __int_sys_start
dd array6_1
dd __int_sys_enter - __int_sys_start
dd array6_2
array6_0:	db	"__int_sys_start",0
array6_1:	db	"__int_sys_size",0
array6_2:	db	"__int_sys_enter",0
align 16
__int_sys_start:;rcx,rdx,r8,r9
	sub rsp,0x20
	db 0x48,0xb8
__int_sys_enter:
	dq 0
	call rax
	add rsp,0x20
	iretq
align 16
__int_sys_end:


align 4
array7:
dd 2
dd __irq_start
dd array7_0
dd __irq_end - __irq_start
dd array7_1
dd __irq_code - __irq_start
dd array7_2
dd __irq_enter - __irq_start
dd array7_3
array7_0:	db	"__irq_start",0
array7_1:	db	"__irq_size",0
array7_2:	db	"__irq_code",0
array7_3:	db	"__irq_enter",0
align 16
__irq_start:
	push rax
	push rcx
	push rdx
	push r8
	push r9
	push r10
	push r11
	push r12
;	push rsi
;	push rdi
	db 0xb1
__irq_code:
	db 0
	movzx rcx,cl
	sub rsp,0x20
	db 0x48,0xb8
__irq_enter:
	dq 0
	call rax
	add rsp,0x20
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdx
	pop rcx
	pop rax
	iretq
align 16
__irq_end:

align 4
array8:
dd 1
dd system_enter_int8x
dd array8_0
dd system_enter_int8x_end - system_enter_int8x
dd array8_1
dd system_enter_int8x_vector - system_enter_int8x
dd array8_2
array8_0:	db	"system_enter_int8x",0
array8_1:	db	"system_enter_int8x_size",0
array8_2:	dd	"system_enter_int8x_vector",0

align 16
system_enter_int8x:
	db 0xcd
system_enter_int8x_vector:
	db 0
	ret
align 16
system_enter_int8x_end:








