//Lava OS
//PageFault
//20-04-11-15-08

#include <vm.h>
#include <config.h>
#include <stdio.h>
#include <stddef.h>

void cf_and(LPVCPU cpu,u32 val0, u32 val1, unsigned int len) {
	u64 flags;
	if(len == 1) asm("andb %%al,%%dl\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"((u8)val0),"d"((u8)val1));
	else if(len == 2) asm("andw %%ax,%%dx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"((u16)val0),"d"((u16)val1));
	else asm("andl %%eax,%%edx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"((u32)val0),"d"((u32)val1));
	CLRF(OF|SF|ZF|CF|PF);
	SETF(flags & (OF|SF|ZF|CF|PF));
}
void cf_or(LPVCPU cpu,u32 val0, u32 val1, unsigned int len) {
	u64 flags;
	if(len == 1) asm("orb %%al,%%dl\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"((u8)val0),"d"((u8)val1));
	else if(len == 2) asm("orw %%ax,%%dx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"((u16)val0),"d"((u16)val1));
	else asm("orl %%eax,%%edx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"((u32)val0),"d"((u32)val1));
	CLRF(OF|SF|ZF|CF|PF);
	SETF(flags & (OF|SF|ZF|CF|PF));
}
void cf_xor(LPVCPU cpu,u32 val0, u32 val1, unsigned int len) {
	u64 flags;
	if(len == 1) asm("xorb %%al,%%dl\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"((u8)val0),"d"((u8)val1));
	else if(len == 2) asm("xorw %%ax,%%dx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"((u16)val0),"d"((u16)val1));
	else asm("xorl %%eax,%%edx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"((u32)val0),"d"((u32)val1));
	CLRF(OF|SF|ZF|CF|PF);
	SETF(flags & (OF|SF|ZF|CF|PF));
}
void cf_not(LPVCPU cpu,u32 val, unsigned int len) {
	u64 flags;
	if(len == 1) asm("notb %%al\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"((u8)val));
	else if(len == 2) asm("notw %%ax\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"((u16)val));
	else asm("notl %%eax\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"((u32)val));
	CLRF(OF|SF|ZF|CF|PF);
	SETF(flags & (OF|SF|ZF|CF|PF));
}
void AND_AbIb(LPVCPU cpu) {
	u8 val0 = GETR8(AL);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AND AL, 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AL, val0 & val1);
	cf_and(cpu, val0, val1, 1);
}
void AND_AdId(LPVCPU cpu) {
	u32 val0 = GETR32(EAX);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AND EAX, 0x%08X", cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, val0 & val1);
	cf_and(cpu, val0, val1, 4);
}
void AND_AwIw(LPVCPU cpu) {
	u16 val0 = GETR16(AX);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AND AX, 0x%04X", cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, val0 & val1);
	cf_and(cpu, val0, val1, 2);
}
void AND_MbIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"AND BYTE [%s:%s], 0x%02X", 
		SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"AND BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 & val1);
	cf_and(cpu, val0, val1, 1);
}
void AND_MbRb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"AND [%s:%s], %s", 
		SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"AND [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 & val1);
	cf_and(cpu, val0, val1, 1);
}
void AND_MdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"AND DWORD [%s:%s], 0x%02X", 
		SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"AND DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 & val1);
	cf_and(cpu, val0, val1, 4);
}
void AND_MdId(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"AND DWORD [%s:%s], 0x%08X", 
		SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.dword);
	else sprintf(cpu->ins_str,64,"AND DWORD [%s], 0x%08X", cpu->rm_str, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 & val1);
	cf_and(cpu, val0, val1, 4);
}
void AND_MdRd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"AND [%s:%s], %s", 
		SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"AND [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 & val1);
	cf_and(cpu, val0, val1, 4);
}
void AND_MwIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"AND WORD [%s:%s], 0x%02X", 
		SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"AND WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 & val1);
	cf_and(cpu, val0, val1, 2);
}
void AND_MwIw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"AND WORD [%s:%s], 0x%04X", 
		SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.word);
	else sprintf(cpu->ins_str,64,"AND WORD [%s], 0x%04X", cpu->rm_str, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 & val1);
	cf_and(cpu, val0, val1, 2);
}
void AND_MwRw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"AND WORD [%s:%s], %s", 
		SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"AND WORD [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 & val1);
	cf_and(cpu, val0, val1, 2);
}
void AND_RbIb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AND %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 & val1);
	cf_and(cpu, val0, val1, 1);
}
void AND_RbMb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"AND %s, [%s:%s]", REG8[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"AND %s, [%s]", REG8[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 & val1);
	cf_and(cpu, val0, val1, 1);
}
void AND_RbRb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AND %s, %s", REG8[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 & val1);
	cf_and(cpu, val0, val1, 1);
}
void AND_RdIb(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AND %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 & val1);
	cf_and(cpu, val0, val1, 4);
}
void AND_RdId(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AND %s, 0x%08X", REG32[cpu->ins.des], cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 & val1);
	cf_and(cpu, val0, val1, 4);
}
void AND_RdMd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"AND %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"AND %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 & val1);
	cf_and(cpu, val0, val1, 4);
}
void AND_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AND %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 & val1);
	cf_and(cpu, val0, val1, 4);
}
void AND_RwIb(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AND %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 & val1);
	cf_and(cpu, val0, val1, 2);
}
void AND_RwIw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AND %s, 0x%04X", REG16[cpu->ins.des], cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 & val1);
	cf_and(cpu, val0, val1, 2);
}
void AND_RwMw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"AND %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"AND %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 & val1);
	cf_and(cpu, val0, val1, 2);
}
void AND_RwRw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AND %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 & val1);
	cf_and(cpu, val0, val1, 2);
}
void NOT_Mb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	s8 val0 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"NOT BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"NOT BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, ~val0);
	cf_not(cpu, val0, 1);
}
void NOT_Md(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	s32 val0 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"NOT DWORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"NOT DWORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, ~val0);
	cf_not(cpu, val0, 4);
}
void NOT_Mw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	s16 val0 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"NOT WORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"NOT WORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, ~val0);
	cf_not(cpu, val0, 2);
}
void NOT_Rb(LPVCPU cpu) {
	s8 val0 = GETR8(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"NOT %s", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, ~val0);
	cf_not(cpu, val0, 1);
}
void NOT_Rd(LPVCPU cpu) {
	s32 val0 = GETR32(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"NOT %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, ~val0);
	cf_not(cpu, val0, 4);
}
void NOT_Rw(LPVCPU cpu) {
	s16 val0 = GETR16(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"NOT %s", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, ~val0);
	cf_not(cpu, val0, 2);
}
void OR_AbIb(LPVCPU cpu) {
	u8 val0 = GETR8(AL);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OR AL, 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AL, val0 | val1);
	cf_or(cpu, val0, val1, 1);
}
void OR_AdId(LPVCPU cpu) {
	u32 val0 = GETR32(EAX);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OR EAX, 0x%08X", cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, val0 | val1);
	cf_or(cpu, val0, val1, 4);
}
void OR_AwIw(LPVCPU cpu) {
	u16 val0 = GETR16(AX);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OR AX, 0x%04X", cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, val0 | val1);
	cf_or(cpu, val0, val1, 2);
}
void OR_MbIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"OR BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"OR BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 | val1);
	cf_or(cpu, val0, val1, 1);
}
void OR_MbRb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"OR [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"OR [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 | val1);
	cf_or(cpu, val0, val1, 1);
}
void OR_MdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"OR DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"OR DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 | val1);
	cf_or(cpu, val0, val1, 4);
}
void OR_MdId(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"OR DWORD [%s:%s], 0x%08X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.dword);
	else sprintf(cpu->ins_str,64,"OR DWORD [%s], 0x%08X", cpu->rm_str, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 | val1);
	cf_or(cpu, val0, val1, 4);
}
void OR_MdRd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"OR [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"OR [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 | val1);
	cf_or(cpu, val0, val1, 4);
}
void OR_MwIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"OR WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"OR WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 | val1);
	cf_or(cpu, val0, val1, 2);
}
void OR_MwIw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"OR WORD [%s:%s], 0x%04X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.word);
	else sprintf(cpu->ins_str,64,"OR WORD [%s], 0x%04X", cpu->rm_str, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 | val1);
	cf_or(cpu, val0, val1, 2);
}
void OR_MwRw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"OR WORD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"OR WORD [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 | val1);
	cf_or(cpu, val0, val1, 2);
}
void OR_RbIb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OR %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 | val1);
	cf_or(cpu, val0, val1, 1);
}
void OR_RbMb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"OR %s, [%s:%s]", REG8[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"OR %s, [%s]", REG8[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 | val1);
	cf_or(cpu, val0, val1, 1);
}
void OR_RbRb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OR %s, %s", REG8[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 | val1);
	cf_or(cpu, val0, val1, 1);
}
void OR_RdIb(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OR %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 | val1);
	cf_or(cpu, val0, val1, 4);
}
void OR_RdId(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OR %s, 0x%08X", REG32[cpu->ins.des], cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 | val1);
	cf_or(cpu, val0, val1, 4);
}
void OR_RdMd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"OR %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"OR %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 | val1);
	cf_or(cpu, val0, val1, 4);
}
void OR_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OR %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 | val1);
	cf_or(cpu, val0, val1, 4);
}
void OR_RwIb(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OR %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 | val1);
	cf_or(cpu, val0, val1, 2);
}
void OR_RwIw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OR %s, 0x%04X", REG16[cpu->ins.des], cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 | val1);
	cf_or(cpu, val0, val1, 2);
}
void OR_RwMw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"OR %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"OR %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 | val1);
	cf_or(cpu, val0, val1, 2);
}
void OR_RwRw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OR %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 | val1);
	cf_or(cpu, val0, val1, 2);
}
void TEST_AbIb(LPVCPU cpu) {
	u8 val0 = GETR8(AL);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"TEST AL, 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 1);
}
void TEST_AdId(LPVCPU cpu) {
	u32 val0 = GETR32(EAX);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"TEST EAX, 0x%08X", cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 4);
}
void TEST_AwIw(LPVCPU cpu) {
	u16 val0 = GETR16(AX);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"TEST AX, 0x%04X", cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 2);
}
void TEST_MbIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"TEST BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"TEST BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 1);
}
void TEST_MbRb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"TEST [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"TEST [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 1);
}
void TEST_MdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"TEST DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"TEST DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 4);
}
void TEST_MdId(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"TEST DWORD [%s:%s], 0x%08X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.dword);
	else sprintf(cpu->ins_str,64,"TEST DWORD [%s], 0x%08X", cpu->rm_str, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 4);
}
void TEST_MdRd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"TEST [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"TEST [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 4);
}
void TEST_MwIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"TEST WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"TEST WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 2);
}
void TEST_MwIw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"TEST WORD [%s:%s], 0x%04X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.word);
	else sprintf(cpu->ins_str,64,"TEST WORD [%s], 0x%04X", cpu->rm_str, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 2);
}
void TEST_MwRw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"TEST WORD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"TEST WORD [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 2);
}
void TEST_RbIb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"TEST %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 1);
}
void TEST_RbMb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"TEST %s, [%s:%s]", REG8[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"TEST %s, [%s]", REG8[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 1);
}
void TEST_RbRb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"TEST %s, %s", REG8[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 1);
}
void TEST_RdIb(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"TEST %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 4);
}
void TEST_RdId(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"TEST %s, 0x%08X", REG32[cpu->ins.des], cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 4);
}
void TEST_RdMd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"TEST %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"TEST %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 4);
}
void TEST_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"TEST %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 4);
}
void TEST_RwIb(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"TEST %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 2);
}
void TEST_RwIw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"TEST %s, 0x%04X", REG16[cpu->ins.des], cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 2);
}
void TEST_RwMw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"TEST %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"TEST %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 2);
}
void TEST_RwRw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"TEST %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_and(cpu, val0, val1, 2);
}
void XOR_AbIb(LPVCPU cpu) {
	u8 val0 = GETR8(AL);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"TEST AL, 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AL, val0 ^ val1);
	cf_xor(cpu, val0, val1, 1);
}
void XOR_AdId(LPVCPU cpu) {
	u32 val0 = GETR32(EAX);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XOR EAX, 0x%08X", cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, val0 ^ val1);
	cf_xor(cpu, val0, val1, 4);
}
void XOR_AwIw(LPVCPU cpu) {
	u16 val0 = GETR16(AX);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XOR AX, 0x%04X", cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, val0 ^ val1);
	cf_xor(cpu, val0, val1, 2);
}
void XOR_MbIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XOR BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"XOR BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 ^ val1);
	cf_xor(cpu, val0, val1, 1);
}
void XOR_MbRb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XOR [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"XOR [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 ^ val1);
	cf_xor(cpu, val0, val1, 1);
}
void XOR_MdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XOR DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"XOR DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 ^ val1);
	cf_xor(cpu, val0, val1, 4);
}
void XOR_MdId(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XOR DWORD [%s:%s], 0x%08X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.dword);
	else sprintf(cpu->ins_str,64,"XOR DWORD [%s], 0x%08X", cpu->rm_str, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 ^ val1);
	cf_xor(cpu, val0, val1, 4);
}
void XOR_MdRd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XOR [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"XOR [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 ^ val1);
	cf_xor(cpu, val0, val1, 4);
}
void XOR_MwIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XOR WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"XOR WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 ^ val1);
	cf_xor(cpu, val0, val1, 2);
}
void XOR_MwIw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XOR WORD [%s:%s], 0x%04X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.word);
	else sprintf(cpu->ins_str,64,"XOR WORD [%s], 0x%04X", cpu->rm_str, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 ^ val1);
	cf_xor(cpu, val0, val1, 2);
}
void XOR_MwRw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XOR WORD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"XOR WORD [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 ^ val1);
	cf_xor(cpu, val0, val1, 2);
}
void XOR_RbIb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XOR %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 ^ val1);
	cf_xor(cpu, val0, val1, 1);
}
void XOR_RbMb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XOR %s, [%s:%s]", REG8[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"XOR %s, [%s]", REG8[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 ^ val1);
	cf_xor(cpu, val0, val1, 1);
}
void XOR_RbRb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XOR %s, %s", REG8[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 ^ val1);
	cf_xor(cpu, val0, val1, 1);
}
void XOR_RdIb(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XOR %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 ^ val1);
	cf_xor(cpu, val0, val1, 4);
}
void XOR_RdId(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XOR %s, 0x%08X", REG32[cpu->ins.des], cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 ^ val1);
	cf_xor(cpu, val0, val1, 4);
}
void XOR_RdMd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XOR %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"XOR %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 ^ val1);
	cf_xor(cpu, val0, val1, 4);
}
void XOR_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XOR %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 ^ val1);
	cf_xor(cpu, val0, val1, 4);
}
void XOR_RwIb(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XOR %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 ^ val1);
	cf_xor(cpu, val0, val1, 2);
}
void XOR_RwIw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XOR %s, 0x%04X", REG16[cpu->ins.des], cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 ^ val1);
	cf_xor(cpu, val0, val1, 2);
}
void XOR_RwMw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XOR %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"XOR %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 ^ val1);
	cf_xor(cpu, val0, val1, 2);
}
void XOR_RwRw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XOR %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 ^ val1);
	cf_xor(cpu, val0, val1, 2);
}
