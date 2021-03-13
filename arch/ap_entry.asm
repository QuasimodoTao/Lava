;
;	arch/ap_entry.asm
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
dd 1;array count
dd array4

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
	pause
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
		db	0xff,0xff,0x00,0x00,0x00,0x92,0xcf,0x00	;32-bits data segment
align 16
AP_init_end: