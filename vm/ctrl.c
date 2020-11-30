/*
	vm/ctrl.c
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

void CALL_IPd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CALL 0x%04X:0x%08X", cpu->ins.imm2.word, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->sr[CS]);
	VMPush16(cpu->eip.rx);
	cpu->sr[CS] = cpu->ins.imm2.word;
	cpu->base[CS] = ((s32)cpu->ins.imm2.word) << 4;
	cpu->eip.erx = cpu->ins.imm1.dword;
}
void CALL_IPw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CALL 0x%04X:0x%04X", cpu->ins.imm2.word, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->sr[CS]);
	VMPush16(cpu->eip.rx);
	cpu->sr[CS] = cpu->ins.imm2.word;
	cpu->base[CS] = ((s32)cpu->ins.imm2.word) << 4;
	cpu->eip.rx = cpu->ins.imm1.word;
}
void CALL_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CALL 0x%04X", cpu->eip.rx + (s32)cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->eip.rx);
	cpu->eip.erx += cpu->ins.imm1.dword;
}
void CALL_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CALL 0x%04X", cpu->eip.rx + (s16)cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->eip.rx);
	cpu->eip.rx += cpu->ins.imm1.word;
}
void CALL_MPd(LPVCPU cpu) {
	s32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CALL FAR [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"CALL FAR [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->sr[CS]);
	VMPush16(cpu->eip.rx);
	cpu->sr[CS] = VMRead16(addr + 4);
	cpu->base[CS] = ((s32)cpu->sr[CS]) << 4;
	cpu->eip.erx = VMRead32(addr);
}
void CALL_MPw(LPVCPU cpu) {
	s32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CALL FAR [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"CALL FAR [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->sr[CS]);
	VMPush16(cpu->eip.rx);
	cpu->sr[CS] = VMRead16(addr + 2);
	cpu->base[CS] = ((s32)cpu->sr[CS]) << 4;
	cpu->eip.rx = VMRead16(addr);
}
void CALL_Md(LPVCPU cpu) {
	s32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CALL NEAR [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"CALL NEAR [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->eip.rx);
	cpu->eip.erx = VMRead32(addr);
}
void CALL_Mw(LPVCPU cpu) {
	s32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CALL NEAR [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"CALL NEAR [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->eip.rx);
	cpu->eip.rx += VMRead16(addr);
}
void CALL_Rd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CALL %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->eip.rx);
	cpu->eip.erx = GETR32(cpu->ins.des);
}
void CALL_Rw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CALL %s", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->eip.rx);
	cpu->eip.rx = GETR16(cpu->ins.des);
}
void IRET_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IRET");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eip.rx = VMPop16();
	cpu->sr[CS] = VMPop16();
	cpu->base[CS] = ((s32)cpu->sr[CS]) << 4;
}
void JA_Ib(LPVCPU cpu) {
	//cf == 0 && zf == 0
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JA 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(CF) && !CHEF(ZF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JA_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JA 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(CF) && !CHEF(ZF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JA_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JA 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(CF) && !CHEF(ZF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JC_Ib(LPVCPU cpu) {
	//cf == 1
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JC 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(CF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JC_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JC 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(CF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JC_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JC 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(CF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JG_Ib(LPVCPU cpu) {
	//zf == 0 && sf == of
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JG 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(ZF) && (GETF(SF) == GETF(OF))) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JG_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JG 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(ZF) && (GETF(SF) == GETF(OF))) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JG_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JG 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(ZF) && (GETF(SF) == GETF(OF))) cpu->eip.rx += cpu->ins.imm1.word;
}
void JL_Ib(LPVCPU cpu) {
	//sf != of
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JL 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETF(SF) != GETF(OF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JL_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JL 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETF(SF) != GETF(OF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JL_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JL 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETF(SF) != GETF(OF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JNA_Ib(LPVCPU cpu) {
	//cf == 1 || zf == 1
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNA 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(CF) || CHEF(ZF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JNA_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNA 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(CF) || CHEF(ZF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JNA_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNA 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(CF) || CHEF(ZF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JNC_Ib(LPVCPU cpu) {
	//cf == 0
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNC 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(CF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JNC_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNC 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(CF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JNC_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNC 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(CF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JNG_Ib(LPVCPU cpu) {
	//zf == 1 || sf == of
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNG 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(ZF) || (GETF(SF) == GETF(OF))) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JNG_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNG 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(ZF) || (GETF(SF) == GETF(OF))) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JNG_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNG 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(ZF) || (GETF(SF) == GETF(OF))) cpu->eip.rx += cpu->ins.imm1.word;
}
void JNL_Ib(LPVCPU cpu) {
	//sf == of
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNL 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETF(SF) == GETF(ZF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JNL_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNL 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETF(SF) == GETF(ZF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JNL_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNL 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETF(SF) == GETF(ZF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JNO_Ib(LPVCPU cpu) {
	//of == 0
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNO 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(OF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JNO_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNO 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(OF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JNO_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNO 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(OF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JNP_Ib(LPVCPU cpu) {
	//pf == 0
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNP 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(PF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JNP_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNP 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(PF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JNP_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNP 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(PF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JNS_Ib(LPVCPU cpu) {
	//sf == 0
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNS 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(SF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JNS_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNS 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(SF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JNS_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNS 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(SF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JNZ_Ib(LPVCPU cpu) {
	//zf == 0
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNZ 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(ZF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JNZ_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNZ 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(ZF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JNZ_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JNZ 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(ZF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JO_Ib(LPVCPU cpu) {
	//of == 1
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JO 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(OF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JO_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JO 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(OF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JO_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JO 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(OF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JP_Ib(LPVCPU cpu) {
	//pf == 1
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JP 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(PF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JP_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JP 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(PF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JP_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JP 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(PF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JS_Ib(LPVCPU cpu) {
	//sf == 1
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JS 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(SF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JS_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JS 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(SF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JS_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JS 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(SF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void JZ_Ib(LPVCPU cpu) {
	//zf == 1
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JZ 0x%04X", cpu->eip.rx + (s8)cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(ZF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JZ_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JZ 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(ZF)) cpu->eip.erx += cpu->ins.imm1.dword;
}
void JZ_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JZ 0x%04X", cpu->eip.rx + cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(ZF)) cpu->eip.rx += cpu->ins.imm1.word;
}
void RET_FIw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"RETF 0x%04X", cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[CS] = VMPop16();
	cpu->base[CS] = ((s32)cpu->sr[CS]) << 4;
	cpu->eip.rx = VMPop16();
	SETR16(SP, GETR16(SP) + cpu->ins.imm1.word);
}
void RET_FNo(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"RETF");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[CS] = VMPop16();
	cpu->base[CS] = ((s32)cpu->sr[CS]) << 4;
	cpu->eip.rx = VMPop16();
}
void RET_NIw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"RETN 0x%04X", cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eip.rx = VMPop16();
	SETR16(SP, GETR16(SP) + cpu->ins.imm1.word);
}
void RET_NNo(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"RETN");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eip.rx = VMPop16();
}
void IRETD_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IRET");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[CS] = VMPop16();
	cpu->base[CS] = ((s32)cpu->sr[CS]) << 4;
	cpu->eip.rx = VMPop16();
}
void JMP_IPd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JMP 0x%04X:0x%08X", cpu->ins.imm2.word, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[CS] = cpu->ins.imm2.word;
	cpu->base[CS] = ((s32)cpu->ins.imm2.word) << 4;
	cpu->eip.erx = cpu->ins.imm1.dword;
}
void JMP_IPw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JMP 0x%04X:0x%04X", cpu->ins.imm2.word, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[CS] = cpu->ins.imm2.word;
	cpu->base[CS] = ((s32)cpu->ins.imm2.word) << 4;
	cpu->eip.rx = cpu->ins.imm1.word;
}
void JMP_Ib(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JMP 0x%04X", cpu->eip.rx);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JMP_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JMP 0x%04X", cpu->eip.rx);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eip.erx += cpu->ins.imm1.dword;
}
void JMP_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JMP 0x%04X", cpu->eip.rx);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eip.rx += cpu->ins.imm1.word;
}
void JMP_MPd(LPVCPU cpu) {
	s32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"JMP FAR [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"JMP FAR [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[CS] = VMRead16(addr + 4);
	cpu->base[CS] = ((s32)cpu->sr[CS]) << 4;
	cpu->eip.erx = VMRead32(addr);
}
void JMP_MPw(LPVCPU cpu) {
	s32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"JMP FAR [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"JMP FAR [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[CS] = VMRead16(addr + 2);
	cpu->base[CS] = ((s32)cpu->sr[CS]) << 4;
	cpu->eip.rx = VMRead16(addr);
}
void JMP_Md(LPVCPU cpu) {
	s32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"JMP NEAR [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"JMP NEAR [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eip.erx = VMRead32(addr);
}
void JMP_Mw(LPVCPU cpu) {
	s32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"JMP NEAR [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"JMP NEAR [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eip.rx = VMRead16(addr);
}
void JMP_Rd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JMP %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eip.erx = GETR32(cpu->ins.des);
}
void JMP_Rw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JMP %s", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eip.rx = GETR16(cpu->ins.des);
}
void JCXZ_Ib(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JCXZ 0x%04X", cpu->eip.rx);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR16(CX)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void JECXZ_Ib(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"JCXZ 0x%04X", cpu->eip.rx);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR16(CX)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void LOOP_Ib(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LOOP 0x%04X", cpu->eip.rx);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(CX, GETR16(CX) - 1);
	if (GETR16(CX)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void LOOPE_Ib(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LOOPE 0x%04X", cpu->eip.rx);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(CX, GETR16(CX) - 1);
	if (GETR16(CX) && CHEF(ZF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void LOOPNE_Ib(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LOOPNE 0x%04X", cpu->eip.rx);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(CX, GETR16(CX) - 1);
	if (GETR16(CX) && !CHEF(ZF)) cpu->eip.rx += (s8)cpu->ins.imm1.byte;
}
void INT3_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"INT 0x03");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->sr[CS]);
	VMPush16(cpu->eip.rx);
	cpu->sr[CS] = VMRead16(14);
	cpu->eip.rx = VMRead16(12);
	cpu->base[CS] = ((s32)cpu->sr[CS]) << 4;
}
void INTO_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"INTO");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(OF)) {
		VMPush16(cpu->sr[CS]);
		VMPush16(cpu->eip.rx);
		cpu->sr[CS] = VMRead16(18);
		cpu->eip.rx = VMRead16(16);
		cpu->base[CS] = ((s32)cpu->sr[CS]) << 4;
	}
}
void INT_Ib(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"INT 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->sr[CS]);
	VMPush16(cpu->eip.rx);
	cpu->sr[CS] = VMRead16(cpu->ins.imm1.byte * 4 + 2);
	cpu->eip.rx = VMRead16(cpu->ins.imm1.byte * 4);
	cpu->base[CS] = ((s32)cpu->sr[CS]) << 4;
}
