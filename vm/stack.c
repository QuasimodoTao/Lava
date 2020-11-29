//Lava OS
//PageFault
//20-04-11-15-08

#include <vm.h>
#include <config.h>
#include <stdio.h>
#include <stddef.h>

void vm_push(LPVCPU cpu,u32 len, u32 data) {
	u32 addr = cpu->base[SS] + GETR16(SP);
	//sprintf(cpu->ins_str,64,"%08X.",addr);
	if (len == 2) {
		VMWrite16(addr - 2, (unsigned short)data);
		SETR16(SP, GETR16(SP) - 2);
	}
	else {
		VMWrite32(addr - 4, data);
		SETR16(SP, GETR16(SP) - 4);
	}
}
u32 vm_pop(LPVCPU cpu,u32 len) {
	u32 addr = cpu->base[SS] + GETR16(SP);
	if (len == 2) {
		SETR16(SP, GETR16(SP) + 2);
		return VMRead16(addr);
	}
	else {
		SETR16(SP, GETR16(SP) + 4);
		return VMRead32(addr);
	}
}
void POPAD_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"POPAD");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EDI, VMPop32());
	SETR32(ESI, VMPop32());
	SETR32(EBP, VMPop32());
	VMPop32();
	SETR32(EBX, VMPop32());
	SETR32(EDX, VMPop32());
	SETR32(ECX, VMPop32());
	SETR32(EAX, VMPop32());
}
void POPA_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"POPA");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(DI, VMPop16());
	SETR16(SI, VMPop16());
	SETR16(BP, VMPop16());
	VMPop16();
	SETR16(BX, VMPop16());
	SETR16(DX, VMPop16());
	SETR16(CX, VMPop16());
	SETR16(AX, VMPop16());
}
void POPFD_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"POPFD");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eflags.erx = VMPop32();
}
void POPF_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"POPF");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eflags.rx = VMPop16();
}
void POP_Md(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"POP DWORD [%s:%s]", 
		SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"POP DWORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, VMPop32());
}
void POP_Mw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"POP WORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"POP WORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, VMPop16());
}
void POP_Rd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"POP %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, VMPop32());
}
void POP_Rw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"POP %s", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, VMPop16());
}
void POP_Sr(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"POP %s", SREG[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[cpu->ins.des] = VMPop16();
	cpu->base[cpu->ins.des] = cpu->sr[cpu->ins.des] << 4;
}
void PUSHAD_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"PUSHAD");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush32(GETR32(EAX));
	VMPush32(GETR32(ECX));
	VMPush32(GETR32(EDX));
	VMPush32(GETR32(EBX));
	VMPush32(GETR32(ESP));
	VMPush32(GETR32(EBP));
	VMPush32(GETR32(ESI));
	VMPush32(GETR32(EDI));
}
void PUSHA_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"PUSHA");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(GETR16(AX));
	VMPush16(GETR16(CX));
	VMPush16(GETR16(DX));
	VMPush16(GETR16(BX));
	VMPush16(GETR16(SP));
	VMPush16(GETR16(BP));
	VMPush16(GETR16(SI));
	VMPush16(GETR16(DI));
}
void PUSHFD_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"PUSHFD");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush32(cpu->eflags.erx);
}
void PUSHF_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"PUSHF");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->eflags.rx);
}
void PUSH_Ib(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"PUSH 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16((signed char)cpu->ins.imm1.byte);
}
void PUSH_Id(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"PUSH 0x%08X", cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush32(cpu->ins.imm1.dword);
}
void PUSH_Iw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"PUSH 0x%04X", cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->ins.imm1.word);
}
void PUSH_Md(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"PUSH DWORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"PUSH DWORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush32(VMRead32(addr));
}
void PUSH_Mw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"PUSH WORD [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"PUSH WORD [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(VMRead16(addr));
}
void PUSH_Rd(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"PUSH %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush32(GETR32(cpu->ins.des));
}
void PUSH_Rw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"PUSH %s", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(GETR16(cpu->ins.des));
}
void PUSH_Sr(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"PUSH %s", SREG[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMPush16(cpu->sr[cpu->ins.des]);
}
