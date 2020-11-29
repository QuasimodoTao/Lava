//Lava OS
//PageFault
//20-04-11-15-07

#include <vm.h>
#include <config.h>
#include <stdio.h>
#include <stddef.h>

void cf_adc(LPVCPU cpu,u32 val1, u32 val2, u32 len) {
	u64 flags;
	if(len == 1){
		if(GETF(CF)) asm("stc");
		else asm("clc");
		asm("adcb %%al,%%dl\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	}
	else if(len == 2){
		if(GETF(CF)) asm("stc");
		else asm("clc");
		asm("adcw %%ax,%%dx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	}
	else{
		if(GETF(CF)) asm("stc");
		else asm("clc");
		asm("adcl %%eax,%%edx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	}
	CLRF(OF|SF|ZF|AF|CF|PF);
	SETF(flags & (OF|SF|ZF|AF|CF|PF));
}
void cf_add(LPVCPU cpu,u32 val1,u32 val2,u32 len) {
	u64 flags;
	if(len == 1) asm("addb %%al,%%dl\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	else if(len == 2) asm("addw %%ax,%%dx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	else asm("addl %%eax,%%edx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	CLRF(OF|SF|ZF|AF|CF|PF);
	SETF(flags & (OF|SF|ZF|AF|CF|PF));
}
void cf_sub(LPVCPU cpu,u32 val1, u32 val2, u32 len) {
	u64 flags;
	if(len == 1) asm("subb %%al,%%dl\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	else if(len == 2) asm("subw %%ax,%%dx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	else asm("subl %%eax,%%edx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	CLRF(OF|SF|ZF|AF|CF|PF);
	SETF(flags & (OF|SF|ZF|AF|CF|PF));
}
void cf_sbb(LPVCPU cpu,u32 val1, u32 val2, u32 len) {
	u64 flags;
	if(len == 1){
		if(GETF(CF)) asm("stc\n\t");
		else asm("clc\n\t");
		asm("adcb %%al,%%cl\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	}
	else if(len == 2){
		if(GETF(CF)) asm("stc\n\t");
		else asm("clc\n\t");
		asm("adcw %%ax,%%dx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	}
	else{
		if(GETF(CF)) asm("stc\n\t");
		else asm("clc\n\t");
		asm("adcl %%eax,%%edx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	}
	CLRF(OF|SF|ZF|AF|CF|PF);
	SETF(flags & (OF|SF|ZF|AF|CF|PF));
}
void cf_inc(LPVCPU cpu,u32 val, u32 len) {
	u64 flags;
	if(len == 1) asm("incb %%al\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val));
	else if(len == 2) asm("incw %%ax\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val));
	else asm("incl %%eax\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val));
	CLRF(OF|SF|ZF|AF|PF);
	SETF(flags & (OF|SF|ZF|AF|PF));
}
void cf_dec(LPVCPU cpu,u32 val, u32 len) {
	u64 flags;
	if(len == 1) asm("decb %%al\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val));
	else if(len == 2) asm("decw %%ax\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val));
	else asm("decl %%eax\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val));
	SETF(flags & (OF|SF|ZF|AF|PF));
}
void cf_mul(LPVCPU cpu,u32 val1, u32 val2, u32 len) {
	u64 flags;
	if(len == 1) asm("mulb %%dl\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	else if(len == 2) asm("mulw %%dx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	else asm("mull %%edx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	CLRF(OF|CF);
	SETF(flags & (OF|CF));
}
void cf_imul(LPVCPU cpu,u32 val1, u32 val2, u32 len) {
	u64 flags;
	if(len == 1) asm("imulb %%dl\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	else if(len == 2) asm("imulw %%dx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	else asm("imull %%edx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val2),"d"(val1));
	CLRF(OF|CF);
	SETF(flags & (OF|CF));
}
void cf_div(LPVCPU cpu,u32 val1l, u32 val1h, u32 val2, u32 len) {
	u64 flags;
	if(len == 1) asm("divb %%cl\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val1l),"d"(val1h),"c"(val2));
	else if(len == 2) asm("divw %%cx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val1l),"d"(val1h),"c"(val2));
	else asm("divl %%ecx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val1l),"d"(val1h),"c"(val2));
	CLRF(OF|CF);
	SETF(flags & (OF|CF));
}
void cf_idiv(LPVCPU cpu,u32 val1l, u32 val1h, u32 val2, u32 len) {
	u64 flags;
	if(len == 1) asm("idivb %%cl\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val1l),"d"(val1h),"c"(val2));
	else if(len == 2) asm("idivw %%cx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val1l),"d"(val1h),"c"(val2));
	else asm("idivl %%ecx\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val1l),"d"(val1h),"c"(val2));
	CLRF(OF|CF);
	SETF(flags & (OF|CF));
}
void cf_neg(LPVCPU cpu,u32 val, u32 len) {
	u64 flags;
	if(len == 1) asm("negb %%al\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val));
	else if(len == 2) asm("negw %%ax\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val));
	else asm("negl %%eax\n\tpushfq\n\tpopq %%rax":"=a"(flags):"a"(val));
	CLRF(OF|SF|ZF|AF|CF|PF);
	SETF(flags & (OF|SF|ZF|AF|CF|PF));
}
void ADC_AbIb(LPVCPU cpu){
	u8 val0 = GETR8(AL);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADC AL, 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AL, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 1);
}
void ADC_AdId(LPVCPU cpu){
	u32 val0 = GETR32(EAX);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADC EAX, 0x%08X", cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX,val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 4);
}
void ADC_AwIw(LPVCPU cpu){
	u16 val0 = GETR16(AX);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADC AX, 0x%04X", cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX,val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 2);
}
void ADC_MbIb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADC BYTE [%s:%s], 0x%02X", 
		SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"ADC BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 1);
}
void ADC_MbRb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADC [%s:%s], %s", 
		SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"ADC [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 1);
}
void ADC_MdIb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADC DWORD [%s:%s], 0x%02X", 
		SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"ADC DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
		if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 4);
}
void ADC_MdId(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADC DWORD [%s:%s], 0x%08X", 
		SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.dword);
	else sprintf(cpu->ins_str,64,"ADC DWORD [%s], 0x%08X", cpu->rm_str, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 4);
}
void ADC_MdRd(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADC [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"ADC [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 4);
}
void ADC_MwIb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADC WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"ADC WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 2);
}
void ADC_MwIw(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADC WORD [%s:%s], 0x%04X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.word);
	else sprintf(cpu->ins_str,64,"ADC WORD [%s], 0x%04X", cpu->rm_str, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 2);
}
void ADC_MwRw(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADC WORD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"ADC WORD [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 2);
}
void ADC_RbIb(LPVCPU cpu){
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADC %s, 0x%02X",REG8[cpu->ins.des],cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 1);
}
void ADC_RbMb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADC %s, [%s:%s]", REG8[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"ADC %s, [%s]", REG8[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 1);
}
void ADC_RbRb(LPVCPU cpu){
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADC %s, %s", REG8[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 1);
}
void ADC_RdIb(LPVCPU cpu){
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADC %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 4);
}
void ADC_RdId(LPVCPU cpu){
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADC %s, 0x%08X", REG32[cpu->ins.des], cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 4);
}
void ADC_RdMd(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADC %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"ADC %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 4);
}
void ADC_RdRd(LPVCPU cpu){
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADC %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 4);
}
void ADC_RwIb(LPVCPU cpu){
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADC %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 2);
}
void ADC_RwIw(LPVCPU cpu){
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADC %s, 0x%04X", REG16[cpu->ins.des], cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 2);
}
void ADC_RwMw(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADC %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"ADC %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 2);
}
void ADC_RwRw(LPVCPU cpu){
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADC %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 + val1 + GETF(CF));
	cf_adc(cpu,val0, val1, 2);
}
void ADD_AbIb(LPVCPU cpu) {
	u8 val0 = GETR8(AL);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADD AL, 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AL, val0 + val1);
	cf_add(cpu,val0, val1, 1);
}
void ADD_AdId(LPVCPU cpu) {
	u32 val0 = GETR32(EAX);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADD EAX, 0x%08X", cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, val0 + val1);
	cf_add(cpu,val0, val1, 4);
}
void ADD_AwIw(LPVCPU cpu) {
	u16 val0 = GETR16(AX);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADD AX, 0x%04X", cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, val0 + val1);
	cf_add(cpu,val0, val1, 2);
}
void ADD_MbIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADD BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"ADD BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 + val1);
	cf_add(cpu,val0, val1, 1);
}
void ADD_MbRb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"ADD [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 + val1);
	cf_add(cpu,val0, val1, 1);
}
void ADD_MdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADD DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"ADD DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 + val1);
	cf_add(cpu,val0, val1, 4);
}
void ADD_MdId(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADD DWORD [%s:%s], 0x%08X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.dword);
	else sprintf(cpu->ins_str,64,"ADD DWORD [%s], 0x%08X", cpu->rm_str, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 + val1);
	cf_add(cpu,val0, val1, 4);
}
void ADD_MdRd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"ADD [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 + val1);
	cf_add(cpu,val0, val1, 4);
}
void ADD_MwIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADD WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"ADD WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 + val1);
	cf_add(cpu,val0, val1, 2);
}
void ADD_MwIw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADD WORD [%s:%s], 0x%04X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.word);
	else sprintf(cpu->ins_str,64,"ADD WORD [%s], 0x%04X", cpu->rm_str, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 + val1);
	cf_add(cpu,val0, val1, 2);
}
void ADD_MwRw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADD WORD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"ADD WORD [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 + val1);
	cf_add(cpu,val0, val1, 2);
}
void ADD_RbIb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADD %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 + val1);
	cf_add(cpu,val0, val1, 1);
}
void ADD_RbMb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADD %s, [%s:%s]", REG8[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"ADD %s, [%s]", REG8[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 + val1);
	cf_add(cpu,val0, val1, 1);
}
void ADD_RbRb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADD %s, %s", REG8[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 + val1);
	cf_add(cpu,val0, val1, 1);
}
void ADD_RdIb(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADD %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 + val1);
	cf_add(cpu,val0, val1, 4);
}
void ADD_RdId(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADD %s, 0x%08X", REG32[cpu->ins.des], cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 + val1);
	cf_add(cpu,val0, val1, 4);
}
void ADD_RdMd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADD %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"ADD %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 + val1);
	cf_add(cpu,val0, val1, 4);
}
void ADD_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADD %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 + val1);
	cf_add(cpu,val0, val1, 4);
}
void ADD_RwIb(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADD %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 + val1);
	cf_add(cpu,val0, val1, 2);
}
void ADD_RwIw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADD %s, 0x%04X", REG16[cpu->ins.des], cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 + val1);
	cf_add(cpu,val0, val1, 2);
}
void ADD_RwMw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ADD %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"ADD %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 + val1);
	cf_add(cpu,val0, val1, 2);
}
void ADD_RwRw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"ADD %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 + val1);
	cf_add(cpu,val0, val1, 2);
}
void CMPSB_No(LPVCPU cpu){
	u8 val0;
	u8 val1;
	u16 Counter;
	u32 saddr;
	u32 daddr = cpu->base[ES] + GETR16(DI);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"REP %s:CMPSB", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"REP CMPSB");
	}
	else if (cpu->ins.muchine_flag & _REPNEP) {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"REPNE %s:CMPSB", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"REPNE CMPSB");
	}
	else {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"%s:CMPSB", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"CMPSB");
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _SREGP) saddr = cpu->base[cpu->ins.sreg] + GETR16(SI);
	else saddr = cpu->base[DS] + GETR16(SI);
	Counter = GETR16(CX);
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;Counter;Counter--) {
			val0 = VMRead8(saddr);
			val1 = VMRead8(daddr);
			if (CHEF(DF)) { 
				SETR16(SI, GETR16(SI) - 1); 
				SETR16(DI, GETR16(DI) - 1);
				saddr--;
				daddr--;
			}
			else {
				SETR16(SI, GETR16(SI) + 1);
				SETR16(DI, GETR16(DI) + 1);
				saddr++;
				daddr++;
			}
			if (val0 != val1) {
				Counter--;
				break;
			}
		}
		cf_sub(cpu,val0, val1, 1);
		SETR16(CX, Counter);
	}
	else if (cpu->ins.muchine_flag & _REPNEP) {
		for (;Counter;Counter--) {
			val0 = VMRead8(saddr);
			val1 = VMRead8(daddr);
			if (CHEF(DF)) {
				SETR16(SI, GETR16(SI) - 1);
				SETR16(DI, GETR16(DI) - 1);
				saddr--;
				daddr--;
			}
			else {
				SETR16(SI, GETR16(SI) + 1);
				SETR16(DI, GETR16(DI) + 1);
				saddr++;
				daddr++;
			}
			if (val0 == val1) {
				Counter--;
				break;
			}
		}
		cf_sub(cpu,val0, val1, 1);
		SETR16(CX, Counter);
	}
	else {
		val0 = VMRead8(saddr);
		val1 = VMRead8(daddr);
		if (CHEF(DF)) {
			SETR16(SI, GETR16(SI) - 1);
			SETR16(DI, GETR16(DI) - 1);
		}
		else {
			SETR16(SI, GETR16(SI) + 1);
			SETR16(DI, GETR16(DI) + 1);
		}
		cf_sub(cpu,val0, val1, 1);
	}
}
void CMPSD_No(LPVCPU cpu){
	u32 val0;
	u32 val1;
	u16 Counter;
	u32 saddr;
	u32 daddr = cpu->base[ES] + GETR16(DI);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"REPE %s:CMPSD", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"REPE CMPSD");
	}
	else if (cpu->ins.muchine_flag & _REPNEP) {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"REPNE %s:CMPSD", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"REPNE CMPSD");
	}
	else {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"%s:CMPSD", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"CMPSD");
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _SREGP) saddr = cpu->base[cpu->ins.sreg] + GETR16(SI);
	else saddr = cpu->base[DS] + GETR16(SI);
	Counter = GETR16(CX);
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;Counter;Counter--) {
			val0 = VMRead32(saddr);
			val1 = VMRead32(daddr);
			if (CHEF(DF)) {
				SETR16(SI, GETR16(SI) - 4);
				SETR16(DI, GETR16(DI) - 4);
				saddr -= 4;
				daddr -= 4;
			}
			else {
				SETR16(SI, GETR16(SI) + 4);
				SETR16(DI, GETR16(DI) + 4);
				saddr += 4;
				daddr += 4;
			}
			if (val0 != val1) {
				Counter--;
				break;
			}
		}
		cf_sub(cpu,val0, val1, 2);
		SETR16(CX, Counter);
	}
	else if (cpu->ins.muchine_flag & _REPNEP) {
		for (;Counter;Counter--) {
			val0 = VMRead32(saddr);
			val1 = VMRead32(daddr);
			if (CHEF(DF)) {
				SETR16(SI, GETR16(SI) - 4);
				SETR16(DI, GETR16(DI) - 4);
				saddr -= 4;
				daddr -= 4;
			}
			else {
				SETR16(SI, GETR16(SI) + 4);
				SETR16(DI, GETR16(DI) + 4);
				saddr += 4;
				daddr += 4;
			}
			if (val0 == val1) {
				Counter--;
				break;
			}
		}
		cf_sub(cpu,val0, val1, 4);
		SETR16(CX, Counter);
	}
	else {
		val0 = VMRead32(saddr);
		val1 = VMRead32(daddr);
		if (CHEF(DF)) {
			SETR16(SI, GETR16(SI) - 4);
			SETR16(DI, GETR16(DI) - 4);
		}
		else {
			SETR16(SI, GETR16(SI) + 4);
			SETR16(DI, GETR16(DI) + 4);
		}
		cf_sub(cpu,val0, val1, 4);
	}
}
void CMPSW_No(LPVCPU cpu){
	u16 val0;
	u16 val1;
	u16 Counter;
	u32 saddr;
	u32 daddr = cpu->base[ES] + GETR16(DI);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"REPE %s:CMPSW", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"REPE CMPSW");
	}
	else if (cpu->ins.muchine_flag & _REPNEP) {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"REPNE %s:CMPSW", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"REPNE CMPSW");
	}
	else {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"%s:CMPSW", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"CMPSW");
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _SREGP) saddr = cpu->base[cpu->ins.sreg] + GETR16(SI);
	else saddr = cpu->base[DS] + GETR16(SI);
	Counter = GETR16(CX);
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;Counter;Counter--) {
			val0 = VMRead16(saddr);
			val1 = VMRead16(daddr);
			if (CHEF(DF)) {
				SETR16(SI, GETR16(SI) - 2);
				SETR16(DI, GETR16(DI) - 2);
				saddr -= 2;
				daddr -= 2;
			}
			else {
				SETR16(SI, GETR16(SI) + 2);
				SETR16(DI, GETR16(DI) + 2);
				saddr += 2;
				daddr += 2;
			}
			if (val0 != val1) {
				Counter--;
				break;
			}
		}
		cf_sub(cpu,val0, val1, 2);
		SETR16(CX, Counter);
	}
	else if (cpu->ins.muchine_flag & _REPNEP) {
		for (;Counter;Counter--) {
			val0 = VMRead16(saddr);
			val1 = VMRead16(daddr);
			if (CHEF(DF)) {
				SETR16(SI, GETR16(SI) - 2);
				SETR16(DI, GETR16(DI) - 2);
				saddr -= 2;
				daddr -= 2;
			}
			else {
				SETR16(SI, GETR16(SI) + 2);
				SETR16(DI, GETR16(DI) + 2);
				saddr += 2;
				daddr += 2;
			}
			if (val0 == val1) {
				Counter--;
				break;
			}
		}
		cf_sub(cpu,val0, val1, 2);
		SETR16(CX, Counter);
	}
	else {
		val0 = VMRead16(saddr);
		val1 = VMRead16(daddr);
		if (CHEF(DF)) {
			SETR16(SI, GETR16(SI) - 2);
			SETR16(DI, GETR16(DI) - 2);
		}
		else {
			SETR16(SI, GETR16(SI) + 2);
			SETR16(DI, GETR16(DI) + 2);
		}
		cf_sub(cpu,val0, val1, 2);
	}
}
void CMPXCHG8B_Mq(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0l = GETR32(EAX);
	u32 val0h = GETR32(EDX);
	u32 val1l = VMRead32(addr);
	u32 val1h = VMRead32(addr + 4);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMPXCHG8B [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"CMPXCHG8B [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (val0l == val1l && val0h == val1h) SETF(ZF);
	else CLRF(ZF);
	SETR32(EAX, val1l);
	SETR32(EDX, val1h);
}
void CMPXCHG_MbRb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = GETR8(AL);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMPXCHG [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"CMPXCHG [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (val0 == val1) {
		SETF(ZF);
		VMWrite8(addr, GETR8(cpu->ins.src));
	}
	else {
		CLRF(ZF);
		SETR8(AL, val0);
	}
	cf_sub(cpu,val0, val1,1);
}
void CMPXCHG_MdRd(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(EAX);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMPXCHG [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"CMPXCHG [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (val0 == val1) {
		SETF(ZF);
		VMWrite32(addr, GETR32(cpu->ins.src));
	}
	else {
		CLRF(ZF);
		SETR32(EAX, val0);
	}
	cf_sub(cpu,val0, val1, 4);
}
void CMPXCHG_MwRw(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(AX);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMPXCHG [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"CMPXCHG [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (val0 == val1) {
		SETF(ZF);
		VMWrite16(addr, GETR16(cpu->ins.src));
	}
	else {
		CLRF(ZF);
		SETR16(AX, val0);
	}
	cf_sub(cpu,val0, val1, 2);
}
void CMPXCHG_RbRb(LPVCPU cpu){
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = GETR8(AL);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMPXCHG %s, %s",REG8[cpu->ins.des],REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (val0 == val1) {
		SETF(ZF);
		SETR8(cpu->ins.des, GETR8(cpu->ins.src));
	}
	else {
		CLRF(ZF);
		SETR8(AL, val0);
	}
	cf_sub(cpu,val0, val1, 1);
}
void CMPXCHG_RdRd(LPVCPU cpu){
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(EAX);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMPXCHG %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (val0 == val1) {
		SETF(ZF);
		SETR32(cpu->ins.des, GETR32(cpu->ins.src));
	}
	else {
		CLRF(ZF);
		SETR32(EAX, val0);
	}
	cf_sub(cpu,val0, val1, 4);
}
void CMPXCHG_RwRw(LPVCPU cpu){
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(EAX);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMPXCHG %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (val0 == val1) {
		SETF(ZF);
		SETR16(cpu->ins.des, GETR16(cpu->ins.src));
	}
	else {
		CLRF(ZF);
		SETR16(EAX, val0);
	}
	cf_sub(cpu,val0, val1, 2);
}
void CMP_AbIb(LPVCPU cpu) {
	u8 val0 = GETR8(AL);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMP AL, 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 1);
}
void CMP_AdId(LPVCPU cpu) {
	u32 val0 = GETR32(EAX);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMP EAX, 0x%08X", cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 4);
}
void CMP_AwIw(LPVCPU cpu) {
	u16 val0 = GETR16(AX);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMP AX, 0x%04X", cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 2);
}
void CMP_MbIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMP BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"CMP BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 1);
}
void CMP_MbRb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMP [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"CMP [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 1);
}
void CMP_MdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMP DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"CMP DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 4);
}
void CMP_MdId(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMP DWORD [%s:%s], 0x%08X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.dword);
	else sprintf(cpu->ins_str,64,"CMP DWORD [%s], 0x%08X", cpu->rm_str, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 4);
}
void CMP_MdRd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMP [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"CMP [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 4);
}
void CMP_MwIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMP WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"CMP WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 2);
}
void CMP_MwIw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMP WORD [%s:%s], 0x%04X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.word);
	else sprintf(cpu->ins_str,64,"CMP WORD [%s], 0x%04X", cpu->rm_str, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 2);
}
void CMP_MwRw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMP WORD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"CMP WORD [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 2);
}
void CMP_RbIb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMP %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 1);
}
void CMP_RbMb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMP %s, [%s:%s]", REG8[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"CMP %s, [%s]", REG8[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 1);
}
void CMP_RbRb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMP %s, %s", REG8[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 1);
}
void CMP_RdIb(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMP %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 4);
}
void CMP_RdId(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMP %s, 0x%08X", REG32[cpu->ins.des], cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 4);
}
void CMP_RdMd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMP %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"CMP %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 4);
}
void CMP_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMP %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 4);
}
void CMP_RwIb(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMP %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 2);
}
void CMP_RwIw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMP %s, 0x%04X", REG16[cpu->ins.des], cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 2);
}
void CMP_RwMw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"CMP %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"CMP %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 2);
}
void CMP_RwRw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMP %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cf_sub(cpu,val0, val1, 2);
}
void CWDE_No(LPVCPU cpu) {
	u32 val0 = (s32)(s16)GETR16(AX);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CWDE");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, val0);
}
void CWD_No(LPVCPU cpu){
	u16 val0 = GETR16(AX);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CWD");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (val0 & 0x8000) SETR16(DX, 0xffff);
	else SETR16(DX, 0);
}
void DEC_Mb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"DEC BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"DEC BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 - 1);
	cf_dec(cpu,val0, 1);
}
void DEC_Md(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"DEC DWORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"DEC DWORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 - 1);
	cf_dec(cpu,val0, 4);
}
void DEC_Mw(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"DEC WORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"DEC WORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 - 1);
	cf_dec(cpu,val0, 2);
}
void DEC_Rb(LPVCPU cpu){
	u8 val0 = GETR8(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"DEC %s", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 - 1);
	cf_dec(cpu,val0, 1);
}
void DEC_Rd(LPVCPU cpu){
	u32 val0 = GETR32(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"DEC %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 - 1);
	cf_dec(cpu,val0, 4);
}
void DEC_Rw(LPVCPU cpu){
	u16 val0 = GETR16(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"DEC %s", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 - 1);
	cf_dec(cpu,val0, 2);
}
void DIV_Mb(LPVCPU cpu){
	u16 val0 = GETR16(AX);
	u32 addr = ModRM0(cpu);
	u8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"DIV BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"DIV BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if(!val1) {
		SETR8(AL, 0);
		SETR8(AH, 0);
		printk("DIV 0");
		return;
	}
	SETR8(AL, val0 / val1);
	SETR8(AH, val0%val1);
	cf_div(cpu,val0, 0, val1, 1);
}
void DIV_Md(LPVCPU cpu){
	u32 val0l = GETR32(EAX);
	u32 val0h = GETR32(EDX);
	u32 addr = ModRM0(cpu);
	u32 val1 = VMRead32(addr);
	u32 res0;
	u32 res1;
	uint64_t val0 = (((uint64_t)GETR32(EAX)) << 32) | (uint64_t)GETR32(EDX);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"DIV DWORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"DIV DWORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if(!val1) {
		SETR32(EAX, 0);
		SETR32(EDX, 0);
		printk("DIV 0");
		return;
	}
	res0 = val0/val1;
	res1 = val0%val1;
	SETR32(EAX, res0);
	SETR32(EDX, res1);
	cf_div(cpu,val0l, val0h, val1, 4);
}
void DIV_Mw(LPVCPU cpu){
	u32 val0 = (u32)GETR16(AX) | ((u32)GETR16(DX) << 16);
	u32 addr = ModRM0(cpu);
	u16 val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"DIV WORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"DIV WORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
		if(!val1) {
		SETR16(AX, 0);
		SETR16(DX, 0);
		printk("DIV 0");
		return;
	}
	SETR16(AX, val0 / val1);
	SETR16(DX, val0%val1);
	cf_div(cpu,val0, 0, val1, 2);
}
void DIV_Rb(LPVCPU cpu){
	u16 val0 = GETR16(AX);
	u8 val1 = GETR8(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"DIV %s", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
		if(!val1) {
		SETR8(AL, 0);
		SETR8(AH, 0);
		printk("DIV 0");
		return;
	}
	SETR8(AL, val0 / val1);
	SETR8(AH, val0%val1);
	cf_div(cpu,val0, 0, val1, 1);
}
void DIV_Rd(LPVCPU cpu){
	u32 val0l = GETR32(EAX);
	u32 val0h = GETR32(EDX);
	u32 val1 = GETR32(cpu->ins.des);
	u32 res0;
	u32 res1;
	uint64_t val0 = (((uint64_t)GETR32(EAX)) << 32) | (uint64_t)GETR32(EDX);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"DIV %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
		if(!val1) {
		SETR32(EAX, 0);
		SETR32(EDX, 0);
		printk("DIV 0");
		return;
	}
	res0 = val0/val1;
	res1 = val0%val1;
	SETR32(EAX, res0);
	SETR32(EDX, res1);
	cf_div(cpu,val0l, val0h, val1, 4);
}
void DIV_Rw(LPVCPU cpu){
	u32 val0 = (u32)GETR16(AX) | ((u32)GETR16(DX) << 16);
	u16 val1 = GETR16(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"DIV %s", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
		if(!val1) {
		SETR16(AX, 0);
		SETR16(DX, 0);
		printk("DIV 0");
		return;
	}
	SETR16(AX, val0 / val1);
	SETR16(DX, val0%val1);
	cf_div(cpu,val0, 0, val1, 2);
}
void IDIV_Mb(LPVCPU cpu){
	s16 val0 = GETR16(AX);
	u32 addr = ModRM0(cpu);
	s8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"IDIV BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"IDIV BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if(!val1) {
		SETR8(AL, 0);
		SETR8(AH, 0);
		printk("IDIV 0");
		return;
	}
	SETR8(AL, val0 / val1);
	SETR8(AH, val0%val1);
	cf_idiv(cpu,val0, 0, val1, 1);
}
void IDIV_Md(LPVCPU cpu){
	u32 val0l = GETR32(EAX);
	u32 val0h = GETR32(EDX);
	u32 addr = ModRM0(cpu);
	u32 val1 = VMRead32(addr);
	u32 res0;
	u32 res1;
	uint64_t val0 = (((uint64_t)GETR32(EAX)) << 32) | (uint64_t)GETR32(EDX);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"IDIV DWORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"IDIV DWORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if(!val1) {
		SETR32(EAX, 0);
		SETR32(EDX, 0);
		printk("IDIV 0");
		return;
	}
	res0 = ((int64_t)val0)/((s32)val1);
	res1 = ((int64_t)val0)%((s32)val1);
	SETR32(EAX, res0);
	SETR32(EDX, res1);
	cf_idiv(cpu,val0l, val0h, val1, 4);
}
void IDIV_Mw(LPVCPU cpu){
	s32 val0 = (u32)GETR16(AX) | ((u32)GETR16(DX) << 16);
	u32 addr = ModRM0(cpu);
	s16 val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"IDIV WORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"IDIV WORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if(!val1) {
		SETR16(AX, 0);
		SETR16(DX, 0);
		printk("IDIV 0");
		return;
	}
	SETR32(EAX, val0 / val1);
	SETR32(EDX, val0%val1);
	cf_idiv(cpu,val0, 0, val1, 2);
}
void IDIV_Rb(LPVCPU cpu){
	s16 val0 = GETR16(AX);
	s8 val1 = GETR8(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IDIV %s", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if(!val1) {
		SETR8(AL, 0);
		SETR8(AH, 0);
		printk("IDIV 0");
		return;
	}
	SETR8(AL, val0 / val1);
	SETR8(AH, val0%val1);
	cf_idiv(cpu,val0, 0, val1, 1);
}
void IDIV_Rd(LPVCPU cpu){
	u32 val0l = GETR32(EAX);
	u32 val0h = GETR32(EDX);
	u32 val1 = GETR32(cpu->ins.des);
	u32 res0;
	u32 res1;
	uint64_t val0 = (((uint64_t)GETR32(EAX)) << 32) | (uint64_t)GETR32(EDX);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IDIV %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if(!val1) {
		SETR32(EAX, 0);
		SETR32(EDX, 0);
		printk("IDIV 0");
		return;
	}
	res0 = ((int64_t)val0)/((s32)val1);
	res1 = ((int64_t)val0)%((s32)val1);
	SETR32(EAX, res0);
	SETR32(EDX, res1);
	cf_idiv(cpu,val0l, val0h, val1, 4);
}
void IDIV_Rw(LPVCPU cpu){
	s32 val0 = (u32)GETR16(AX) | ((u32)GETR16(DX) << 16);
	s16 val1 = GETR16(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IDIV %s", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if(!val1) {
		SETR16(AX, 0);
		SETR16(DX, 0);
		printk("IDIV 0");
		return;
	}
	SETR16(AX, val0 / val1);
	SETR16(DX, val0%val1);
	cf_div(cpu,val0, 0, val1, 2);
}
void IMUL_Mb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	s8 val0 = GETR8(AL);
	s8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"IMUL BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"IMUL BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, (s16)val0 * (s16)val1);
	cf_imul(cpu,val0, val1, 1);
}
void IMUL_Md(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	s32 val0 = GETR32(EAX);
	s32 val1 = VMRead32(addr);
	s64 res = (s64)val0 * (s64)val1;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"IMUL DWORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"IMUL DWORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, (u32)(res));
	SETR32(EDX, (u32)(res >> 32));
	cf_imul(cpu,val0, val1, 4);
}
void IMUL_Mw(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	s16 val0 = GETR16(AX);
	s16 val1 = VMRead16(addr);
	s32 res = (s32)val0 * (s32)val1;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"IMUL WORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"IMUL WORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, (u16)res);
	SETR16(DX, (u16)(res >> 16));
	cf_imul(cpu,val0, val1, 2);
}
void IMUL_Rb(LPVCPU cpu){
	s8 val0 = GETR8(AL);
	s8 val1 = GETR8(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IMUL %s", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, (s16)val0 * (s16)val1);
	cf_imul(cpu,val0, val1, 1);
}
void IMUL_Rd(LPVCPU cpu){
	s32 val0 = GETR32(EAX);
	s32 val1 = GETR32(cpu->ins.des);
	s64 res = (s64)val0 * (s64)val1;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IMUL %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, (u32)res);
	SETR32(EDX, (u32)(res >> 32));
	cf_imul(cpu,val0, val1, 4);
}
void IMUL_RdMd(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	s32 val0 = GETR32(cpu->ins.des);
	s32 val1 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"IMUL %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"IMUL %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0*val1);
	cf_imul(cpu,val0, val1, 4);
}
void IMUL_RdMdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	s32 val0 = VMRead32(addr);
	s32 val1 = (s8)cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"IMUL %s, [%s:%s], 0x%02X", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"IMUL %s, [%s], 0x%02X", REG32[cpu->ins.des], cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0*val1);
	cf_imul(cpu,val0, val1, 4);
}
void IMUL_RdMdId(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	s32 val0 = VMRead32(addr);
	s32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"IMUL %s, [%s:%s], 0x%08X", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.dword);
	else sprintf(cpu->ins_str,64,"IMUL %s, [%s], 0x%08X", REG32[cpu->ins.des], cpu->rm_str, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0*val1);
	cf_imul(cpu,val0, val1, 4);
}
void IMUL_RdRd(LPVCPU cpu) {
	s32 val0 = GETR32(cpu->ins.des);
	s32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IMUL %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0*val1);
	cf_imul(cpu,val0, val1, 4);
}
void IMUL_RdRdIb(LPVCPU cpu){
	s32 val0 = GETR32(cpu->ins.src);
	s32 val1 = (s8)cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IMUL %s, %s, 0x%02X", REG32[cpu->ins.des], REG32[cpu->ins.src], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0*val1);
	cf_imul(cpu,val0, val1, 4);
}
void IMUL_RdRdId(LPVCPU cpu){
	s32 val0 = GETR32(cpu->ins.src);
	s32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IMUL %s, %s, 0x%08X", REG32[cpu->ins.des], REG32[cpu->ins.src], cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0*val1);
	cf_imul(cpu,val0, val1, 4);
}
void IMUL_Rw(LPVCPU cpu) {
	s16 val0 = GETR16(AX);
	s16 val1 = GETR16(cpu->ins.des);
	s32 res = (s32)val0 * (s32)val1;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IMUL %s", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, (u16)res);
	SETR16(DX, (u16)(res >> 16));
	cf_imul(cpu,val0, val1, 2);
}
void IMUL_RwMw(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	s16 val0 = GETR16(cpu->ins.des);
	s16 val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"IMUL %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"IMUL %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0*val1);
	cf_imul(cpu,val0, val1, 2);
}
void IMUL_RwMwIb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	s16 val0 = VMRead16(addr);
	s16 val1 = (s8)cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"IMUL %s, [%s:%s], 0x%02X", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"IMUL %s, [%s], 0x%02X", REG16[cpu->ins.des], cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0*val1);
	cf_imul(cpu,val0, val1, 2);
}
void IMUL_RwMwIw(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	s16 val0 = VMRead16(addr);
	s16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"IMUL %s, [%s:%s], 0x%04X", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.word);
	else sprintf(cpu->ins_str,64,"IMUL %s, [%s], 0x%04X", REG16[cpu->ins.des], cpu->rm_str, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0*val1);
	cf_imul(cpu,val0, val1, 2);
}
void IMUL_RwRw(LPVCPU cpu) {
	s16 val0 = GETR16(cpu->ins.des);
	s16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IMUL %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0*val1);
	cf_imul(cpu,val0, val1, 2);
}
void IMUL_RwRwIb(LPVCPU cpu){
	s16 val0 = GETR16(cpu->ins.src);
	s16 val1 = (s8)cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IMUL %s, %s, 0x%02X", REG16[cpu->ins.des], REG16[cpu->ins.src], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0*val1);
	cf_imul(cpu,val0, val1, 2);
}
void IMUL_RwRwIw(LPVCPU cpu){
	s16 val0 = GETR16(cpu->ins.src);
	s16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IMUL %s, %s, 0x%04X", REG16[cpu->ins.des], REG16[cpu->ins.src], cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0*val1);
	cf_imul(cpu,val0, val1, 2);
}
void INC_Mb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"INC BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"INC BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 + 1);
	cf_inc(cpu,val0, 1);
}
void INC_Md(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"INC DWORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"INC DWORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 + 1);
	cf_inc(cpu,val0, 4);
}
void INC_Mw(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"INC WORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"INC WORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 + 1);
	cf_inc(cpu,val0, 2);
}
void INC_Rb(LPVCPU cpu){
	u8 val0 = GETR8(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"INC %s", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 + 1);
	cf_inc(cpu,val0, 1);
}
void INC_Rd(LPVCPU cpu){
	u32 val0 = GETR32(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"INC %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 + 1);
	cf_inc(cpu,val0, 4);
}
void INC_Rw(LPVCPU cpu){
	u16 val0 = GETR16(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"INC %s", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 + 1);
	cf_inc(cpu,val0, 2);
}
void MUL_Mb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u8 val0 = GETR8(AL);
	u8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MUL BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MUL BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, (u16)val0*(u16)val1);
	cf_mul(cpu,val0, val1, 1);
}
void MUL_Md(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = GETR32(EAX);
	u32 val1 = VMRead32(addr);
	u64 res = (u64)val0*(u64)val1;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MUL DWORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MUL DWORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, (u32)res);
	SETR32(EDX, (u32)(res >> 32));
	cf_mul(cpu,val0, val1, 4);
}
void MUL_Mw(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = GETR16(EAX);
	u16 val1 = VMRead16(addr);
	u64 res = (u32)val0*(u32)val1;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MUL WORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MUL WORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, (u16)res);
	SETR16(DX, (u16)(res >> 16));
	cf_mul(cpu,val0, val1, 2);
}
void MUL_Rb(LPVCPU cpu){
	u8 val0 = GETR8(AL);
	u8 val1 = GETR8(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MUL %s", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, (u16)val0*(u16)val1);
	cf_mul(cpu,val0, val1, 1);
}
void MUL_Rd(LPVCPU cpu){
	u32 val0 = GETR32(EAX);
	u32 val1 = GETR32(cpu->ins.des);
	u64 res = (u64)val0*(u64)val1;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MUL %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, (u32)res);
	SETR32(EDX, (u32)(res >> 32));
	cf_mul(cpu,val0, val1, 4);
}
void MUL_Rw(LPVCPU cpu){
	u16 val0 = GETR16(EAX);
	u16 val1 = GETR16(cpu->ins.des);
	u64 res = (u32)val0*(u32)val1;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MUL %s", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, (u16)res);
	SETR16(DX, (u16)(res >> 16));
	cf_mul(cpu,val0, val1, 2);
}
void NEG_Mb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	s8 val0 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"NEG BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"NEG BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, -val0);
	cf_neg(cpu,val0, 1);
}
void NEG_Md(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	s32 val0 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"NEG DWORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"NEG DWORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, -val0);
	cf_neg(cpu,val0, 4);
}
void NEG_Mw(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	s16 val0 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"NEG WORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"NEG WORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, -val0);
	cf_neg(cpu,val0, 2);
}
void NEG_Rb(LPVCPU cpu){
	s8 val0 = GETR8(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"NEG %s", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, -val0);
	cf_neg(cpu,val0, 1);
}
void NEG_Rd(LPVCPU cpu){
	s32 val0 = GETR32(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"NEG %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, -val0);
	cf_neg(cpu,val0, 4);
}
void NEG_Rw(LPVCPU cpu){
	s16 val0 = GETR16(cpu->ins.des);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"NEG %s", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, -val0);
	cf_neg(cpu,val0, 2);
}
void SBB_AbIb(LPVCPU cpu) {
	u8 val0 = GETR8(AL);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SBB AL, 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AL, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 1);
}
void SBB_AdId(LPVCPU cpu) {
	u32 val0 = GETR32(EAX);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SBB EAX, 0x%08X", cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 4);
}
void SBB_AwIw(LPVCPU cpu) {
	u16 val0 = GETR16(AX);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SBB AX, 0x%04X", cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 2);
}
void SBB_MbIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SBB BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"SBB BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 1);
}
void SBB_MbRb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SBB [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"SBB [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 1);
}
void SBB_MdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SBB DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"SBB DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 4);
}
void SBB_MdId(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SBB DWORD [%s:%s], 0x%08X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.dword);
	else sprintf(cpu->ins_str,64,"SBB DWORD [%s], 0x%08X", cpu->rm_str, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 4);
}
void SBB_MdRd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SBB [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"SBB [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 4);
}
void SBB_MwIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SBB WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"SBB WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 2);
}
void SBB_MwIw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SBB WORD [%s:%s], 0x%04X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.word);
	else sprintf(cpu->ins_str,64,"SBB WORD [%s], 0x%04X", cpu->rm_str, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 2);
}
void SBB_MwRw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SBB WORD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"SBB WORD [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 2);
}
void SBB_RbIb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SBB %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 1);
}
void SBB_RbMb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SBB %s, [%s:%s]", REG8[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SBB %s, [%s]", REG8[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 1);
}
void SBB_RbRb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SBB %s, %s", REG8[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 1);
}
void SBB_RdIb(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SBB %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 4);
}
void SBB_RdId(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SBB %s, 0x%08X", REG32[cpu->ins.des], cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 4);
}
void SBB_RdMd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SBB %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SBB %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 4);
}
void SBB_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SBB %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 4);
}
void SBB_RwIb(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SBB %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 2);
}
void SBB_RwIw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SBB %s, 0x%04X", REG16[cpu->ins.des], cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 2);
}
void SBB_RwMw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SBB %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SBB %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 2);
}
void SBB_RwRw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SBB %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 - val1 - GETF(CF));
	cf_sbb(cpu,val0, val1, 2);
}
void SUB_AbIb(LPVCPU cpu) {
	u8 val0 = GETR8(AL);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SUB AL, 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AL, val0 - val1);
	cf_sub(cpu,val0, val1, 1);
}
void SUB_AdId(LPVCPU cpu) {
	u32 val0 = GETR32(EAX);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SUB EAX, 0x%08X", cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, val0 - val1);
	cf_sub(cpu,val0, val1, 4);
}
void SUB_AwIw(LPVCPU cpu) {
	u16 val0 = GETR16(AX);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SUB AX, 0x%04X", cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, val0 - val1);
	cf_sub(cpu,val0, val1, 2);
}
void SUB_MbIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SUB BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"SUB BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 - val1);
	cf_sub(cpu,val0, val1, 1);
}
void SUB_MbRb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SUB [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"SUB [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 - val1);
	cf_sub(cpu,val0, val1, 1);
}
void SUB_MdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SUB DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"SUB DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 - val1);
	cf_sub(cpu,val0, val1, 4);
}
void SUB_MdId(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SUB DWORD [%s:%s], 0x%08X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.dword);
	else sprintf(cpu->ins_str,64,"SUB DWORD [%s], 0x%08X", cpu->rm_str, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 - val1);
	cf_sub(cpu,val0, val1, 4);
}
void SUB_MdRd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SUB [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"SUB [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 - val1);
	cf_sub(cpu,val0, val1, 4);
}
void SUB_MwIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SUB WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"SUB WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 - val1);
	cf_sub(cpu,val0, val1, 2);
}
void SUB_MwIw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SUB WORD [%s:%s], 0x%04X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.word);
	else sprintf(cpu->ins_str,64,"SUB WORD [%s], 0x%04X", cpu->rm_str, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 - val1);
	cf_sub(cpu,val0, val1, 2);
}
void SUB_MwRw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SUB WORD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"SUB WORD [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 - val1);
	cf_sub(cpu,val0, val1, 2);
}
void SUB_RbIb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SUB %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 - val1);
	cf_sub(cpu,val0, val1, 1);
}
void SUB_RbMb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SUB %s, [%s:%s]", REG8[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SUB %s, [%s]", REG8[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 - val1);
	cf_sub(cpu,val0, val1, 1);
}
void SUB_RbRb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SUB %s, %s", REG8[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 - val1);
	cf_sub(cpu,val0, val1, 1);
}
void SUB_RdIb(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = (s32)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SUB %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 - val1);
	cf_sub(cpu,val0, val1, 4);
}
void SUB_RdId(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = cpu->ins.imm1.dword;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SUB %s, 0x%08X", REG32[cpu->ins.des], cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 - val1);
	cf_sub(cpu,val0, val1, 4);
}
void SUB_RdMd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SUB %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SUB %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 - val1);
	cf_sub(cpu,val0, val1, 4);
}
void SUB_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SUB %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 - val1);
	cf_sub(cpu,val0, val1, 4);
}
void SUB_RwIb(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = (s16)(s8)(cpu->ins.imm1.byte);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SUB %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 - val1);
	cf_sub(cpu,val0, val1, 2);
}
void SUB_RwIw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = cpu->ins.imm1.word;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SUB %s, 0x%04X", REG16[cpu->ins.des], cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 - val1);
	cf_sub(cpu,val0, val1, 2);
}
void SUB_RwMw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SUB %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SUB %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 - val1);
	cf_sub(cpu,val0, val1, 2);
}
void SUB_RwRw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SBB %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 - val1);
	cf_sub(cpu,val0, val1, 2);
}
void SCASB_No(LPVCPU cpu){
	u32 daddr = cpu->base[ES] + GETR16(DI);
	u8 val0;
	u8 val1 = GETR8(AL);
	u16 Counter = GETR16(CX);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REPE SCASB");
	else if (cpu->ins.muchine_flag & _REPNEP) sprintf(cpu->ins_str,64,"REPNE SCASB");
	else sprintf(cpu->ins_str,64,"SCASB");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;Counter;Counter--) {
			val0 = VMRead8(daddr);
			if (val0 != val1) {
				Counter--;
				break;
			}
			if (CHEF(DF)) {
				SETR16(DI, GETR16(DI) - 1);
				daddr -= 1;
			}
			else {
				SETR16(DI, GETR16(DI) + 1);
				daddr += 1;
			}
		}
		cf_sub(cpu,val0, val1, 1);
		SETR16(CX, Counter);
	}
	else if (cpu->ins.muchine_flag & _REPNEP) {
		for (;Counter;Counter--) {
			val0 = VMRead8(daddr);
			if (val0 == val1) {
				Counter--;
				break;
			}
			if (CHEF(DF)) {
				SETR16(DI, GETR16(DI) - 1);
				daddr -= 1;
			}
			else {
				SETR16(DI, GETR16(DI) + 1);
				daddr += 1;
			}
		}
		cf_sub(cpu,val0, val1, 1);
		SETR16(CX, Counter);
	}
	else {
		val0 = VMRead8(daddr);
		if (CHEF(DF)) SETR16(DI, GETR16(DI) - 1);
		else SETR16(DI, GETR16(DI) + 1);
		cf_sub(cpu,val0, val1, 1);
	}
}
void SCASD_No(LPVCPU cpu){
	u32 daddr = cpu->base[ES] + GETR16(DI);
	u32 val0;
	u32 val1 = GETR32(EAX);
	u16 Counter = GETR16(CX);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REPE SCASD");
	else if (cpu->ins.muchine_flag & _REPNEP) sprintf(cpu->ins_str,64,"REPNE SCASD");
	else sprintf(cpu->ins_str,64,"SCASD");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;Counter;Counter--) {
			val0 = VMRead32(daddr);
			if (val0 != val1) {
				Counter--;
				break;
			}
			if (CHEF(DF)) {
				SETR16(DI, GETR16(DI) - 4);
				daddr -= 4;
			}
			else {
				SETR16(DI, GETR16(DI) + 4);
				daddr += 4;
			}
		}
		cf_sub(cpu,val0, val1, 4);
		SETR16(CX, Counter);
	}
	else if (cpu->ins.muchine_flag & _REPNEP) {
		for (;Counter;Counter--) {
			val0 = VMRead32(daddr);
			if (val0 == val1)  {
				Counter--;
				break;
			}
			if (CHEF(DF)) {
				SETR16(DI, GETR16(DI) - 4);
				daddr -= 4;
			}
			else {
				SETR16(DI, GETR16(DI) + 4);
				daddr += 4;
			}
		}
		cf_sub(cpu,val0, val1, 4);
		SETR16(CX, Counter);
	}
	else {
		val0 = VMRead32(daddr);
		if (CHEF(DF)) SETR16(DI, GETR16(DI) - 4);
		else SETR16(DI, GETR16(DI) + 4);
		cf_sub(cpu,val0, val1, 1);
	}
}
void SCASW_No(LPVCPU cpu){
	u32 daddr = cpu->base[ES] + GETR16(DI);
	u16 val0;
	u16 val1 = GETR16(AX);
	u16 Counter = GETR16(CX);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REPE SCASW");
	else if (cpu->ins.muchine_flag & _REPNEP) sprintf(cpu->ins_str,64,"REPNE SCASW");
	else sprintf(cpu->ins_str,64,"SCASW");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;Counter;Counter--) {
			val0 = VMRead16(daddr);
			if (val0 != val1)  {
				Counter--;
				break;
			}
			if (CHEF(DF)) {
				SETR16(DI, GETR16(DI) - 2);
				daddr -= 2;
			}
			else {
				SETR16(DI, GETR16(DI) + 2);
				daddr += 2;
			}
		}
		cf_sub(cpu,val0, val1, 2);
		SETR16(CX, Counter);
	}
	else if (cpu->ins.muchine_flag & _REPNEP) {
		for (;Counter;Counter--) {
			val0 = VMRead16(daddr);
			if (val0 == val1)  {
				Counter--;
				break;
			}
			if (CHEF(DF)) {
				SETR16(DI, GETR16(DI) - 2);
				daddr -= 2;
			}
			else {
				SETR16(DI, GETR16(DI) + 2);
				daddr += 2;
			}
		}
		cf_sub(cpu,val0, val1, 2);
		SETR16(CX, Counter);
	}
	else {
		val0 = VMRead16(daddr);
		if (CHEF(DF)) SETR16(DI, GETR16(DI) - 2);
		else SETR16(DI, GETR16(DI) + 2);
		cf_sub(cpu,val0, val1, 2);
	}
}
void XADD_MbRb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XADD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"XADD [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0 + val1);
	SETR8(cpu->ins.src, val0);
	cf_add(cpu,val0, val1, 1);
}
void XADD_MdRd(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XADD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"XADD [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0 + val1);
	SETR32(cpu->ins.src, val0);
	cf_add(cpu,val0, val1, 4);
}
void XADD_MwRw(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XADD WORD [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"XADD WORD [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0 + val1);
	SETR16(cpu->ins.src, val0);
	cf_add(cpu,val0, val1, 2);
}
void XADD_RbRb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XADD %s, %s", REG8[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0 + val1);
	SETR8(cpu->ins.src, val0);
	cf_add(cpu,val0, val1, 1);
}
void XADD_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XADD %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0 + val1);
	SETR32(cpu->ins.src, val0);
	cf_add(cpu,val0, val1, 4);
}
void XADD_RwRw(LPVCPU cpu){
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XADD %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0 + val1);
	SETR16(cpu->ins.src, val0);
	cf_add(cpu,val0, val1, 2);
}
void CBW_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CBW");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR8(AL) & 0x80) SETR8(AH, 0xff);
	else SETR8(AH, 0);
}
void CDQ_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CDQ");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR32(EAX) & 0x80000000) SETR32(EDX, 0xffffffff);
	else SETR32(EDX, 0);
}
