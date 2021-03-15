/*
	arch/syscall.c
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
#include <config.h>
#include <string.h>
#include <asm.h>
#include <mm.h>
#include <lava.h>
#include <stdio.h>
#include <int.h>

void __syscall(void);
void __sysenter(void);
void user_syscall(void);
void __sys_call_80(void);
void __sys_call_81(void);
void __sys_call_82(void);
void __sys_call_83(void);
u64 sys_call(u64 c,u64 d,u64 r8,u64 r9);
u64 sys_call_80(u64 c,u64 d,u64 r8,u64 r9);
u64 sys_call_82(u64 c,u64 d,u64 r8,u64 r9);
u64 sys_call_83(u64 c,u64 d,u64 r8,u64 r9);

void INIT_CODE syscall_init(){
	int _cpuid[4];
	u64 msr;
	
	make_gate(0x80,__sys_call_80,0,3,0x0e);
	make_gate(0x81,__sys_call_81,0,3,0x0e);
	make_gate(0x82,__sys_call_82,0,3,0x0e);
	make_gate(0x83,__sys_call_83,0,3,0x0e);
	memcpy((void*)USER_SYSCALL_ENTER_CODE_START,user_syscall,0xf0);
	*(u32*)USER_SYSCALL_80_ENTER = 0x00c380cd;
	*(u32*)USER_SYSCALL_81_ENTER = 0x00c381cd;
	*(u32*)USER_SYSCALL_82_ENTER = 0x00c382cd;
	*(u32*)USER_SYSCALL_83_ENTER = 0x00c383cd;

	suppose_syscall = 0;
	*(u64*)(SUPPOSE_SYSCALL_PTR) = 0;
	suppose_sysenter = 0;
	*(u64*)(SUPPOSE_SYSENTER_PTR) = 0;
	cpuid(0x80000001,0,_cpuid);
	if(_cpuid[3] & 0x800){
		//suppose syscall
		msr = rdmsr(IA32_EFER);
		msr |= 0x01;
		wrmsr(IA32_EFER,msr);//set IA32_EFER.SCE
		wrmsr(IA32_LSTAR,__syscall);//enter
		msr = rdmsr(IA32_STAR);
		msr &= 0x00000000ffffffff;
		msr |= ((u64)(KERNEL_CS | USER_CS_32)) << 32;
		wrmsr(IA32_STAR,msr);//selector
		wrmsr(IA32_FMASK,0);//flags mask
		*(u64*)SUPPOSE_SYSENTER_PTR = 1;
		suppose_syscall = 1;
	}
	cpuid(1,0,_cpuid);
	if(_cpuid[3] & 0x800){
		if(((_cpuid[0] >> 8) & 0x07) >= 6){
			if(((_cpuid[0] >> 4) & 0x0f) >= 3){
				if((_cpuid[0] & 0x0f) >= 3){
					//suppose sysenter
					*(u64*)SUPPOSE_SYSCALL_PTR = 1;
					suppose_sysenter = 1;
					wrmsr(0x176,__sysenter);//rip
					wrmsr(0x174,KERNEL_CS);//cs
				}
			}
		}
	}
	
	
}