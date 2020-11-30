/*
	vm/other.c
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

#include <vm.h>
#include <config.h>
#include <stdio.h>
#include <stddef.h>

void BOUND_RdMq(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BOUND");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void BOUND_RwMd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BOUND");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void CLTS_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CLTS");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void CPUID_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CPUID");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void ENTER_IwIb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ENTER %04X,%02X.", cpu->ins.imm1.word, cpu->ins.imm2.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void NOP_No(LPVCPU cpu) {
	
}
void WBINVD_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"WBINVD");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void LEAVE_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LEAVE");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void INVD_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"INVD");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void INVLPG_Mu(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"INVLPG");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void LGDT_MPd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LGDT");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void LIDT_MPd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LIDT");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void LLDT_Mw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LLDT");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void LLDT_Rw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LLDT");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void LMSW_Mw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LMSW");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void LMSW_Rw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LMSW");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void MOV_CdRd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV Cr,R");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void MOV_DdRd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV Dr,R");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void MOV_RdCd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV R,Cr");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void MOV_RdDd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV R,Dr");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void MOV_TdRd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV Tr,R");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void MOV_RdTd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV R,Tr");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void RDMSR_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"RDMSR");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void RDPMC_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"RDPMC");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void RDTSC_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"RDTSC");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void SGDT_MPd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SGDT");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void SIDT_MPd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SIDT");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void SMSW_Mw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SMSW");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void SMSW_Rw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SMSW");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void WRMSR_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"WRMSR");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void INS_ERR(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"cpu_ERR.%02X",cpu->ins.op0);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void HLT_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"HLT");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void SMSW_Rd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SMSW");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
void SMSW_Md(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SMSW");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
}
