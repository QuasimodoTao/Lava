/*
	vm/if.c
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
#include "ins.h"
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <asm.h>
#include <keyboard.h>
#include <gst.h>

extern struct _OPC_ OPB[];
extern struct _OPC_ OPE[];
extern struct _OPC_ GP0F00[];
extern struct _OPC_ GP0F01[];
extern struct _OPC_ GP0FBA[];
extern struct _OPC_ GP80[];
extern struct _OPC_ GP81[];
extern struct _OPC_ GP83[];
extern struct _OPC_ GPC0[];
extern struct _OPC_ GPC1[];
extern struct _OPC_ GPD0[];
extern struct _OPC_ GPD1[];
extern struct _OPC_ GPD2[];
extern struct _OPC_ GPD3[];
extern struct _OPC_ GPF6[];
extern struct _OPC_ GPF7[];
extern struct _OPC_ GPFE[];
extern struct _OPC_ GPFF[];
extern char RMB[];
extern char RME[];
extern char BTB[];
extern char BTE[];
extern char NAB[];
extern char NAE[];
extern char RMB2[];
extern char RME2[];

#define GETSR(n) (cpu->sr[n])

struct _MLOG_ {
	int gs_type;
	int flag;
	u64 Size;
	struct _MLOG_ * Prev;
	struct _MLOG_ * Next;
};

#define Root	((short*)(vm_memory_base + VM_CALL_PROC_BASE))
#define Root_CS	(VM_CALL_PROC_BASE >> 4)
#define Root_IP	(VM_CALL_PROC_BASE & 0x0f)
#define First	((struct _MLOG_ *)((vm_memory_base + VM_FREE_SPACE_START + sizeof(struct _MLOG_) - 1) & ~(sizeof(struct _MLOG_) - 1)))

#define BLOCK_FLAG_FREE		0
#define BLOCK_FLAG_USED		1

struct _ModRM_ {
	union {
		u8 modrm;
		struct {
			u8 rm : 3;
			u8 reg : 3;
			u8 mod : 2;
		};
	};
	
};
struct _SIB_ {
	union {
		u8 sib;
		struct {
			u8 base : 3;
			u8 index : 3;
			u8 scale : 2;
		};
	};
};
u8 vm_read_byte(u32 vm_addr){
	return VMRead8(vm_addr);
}
u16 vm_read_short(u32 vm_addr){
	return VMRead16(vm_addr);
}
u32 vm_read_long(u32 vm_addr){
	return VMRead32(vm_addr);
}
void vm_write_byte(u32 vm_addr,u8 data){
	VMWrite8(vm_addr,data);
}
void vm_write_short(u32 vm_addr,u16 data){
	VMWrite16(vm_addr,data);
}
void vm_write_long(u32 vm_addr,u32 data){
	VMWrite32(vm_addr,data);
}
void vm_read_string(u32 vm_addr,u32 count,void * dest){
	memcpy(dest,(char*)(vm_memory_base + vm_addr),count);
}
void vm_write_string(u32 vm_addr,u32 count,void * srco){
	memcpy((char*)(vm_memory_base + vm_addr),srco,count);
}
int modrm_b(u32 ins_base,struct _VM_CPU_ * cpu) {
	struct _ModRM_ modrm;
	struct _SIB_ sib;
	u32 addr = 0;
	int sibt = 0;

	modrm.modrm = VMRead8(ins_base);
	cpu->ins.mod = modrm.mod;

	if (modrm.mod == 3) { 
		cpu->ins.addr = 0;
		return 1;
	}
	if (cpu->ins.muchine_flag & _ASP) {
		if (modrm.mod == 0 && modrm.rm == 5) {
			if (!(cpu->ins.muchine_flag & _SREGP)) cpu->ins.sreg = DS;
			cpu->ins.disp = cpu->ins.addr = VMRead32(ins_base + 1);
#ifdef VM_DEBUG
			sprintf(cpu->rm_str,16, "0x%08X", cpu->ins.disp);
#endif
			return 5;
		}
		else {
			if (cpu->ins.rm == 4) {
				sib.sib = VMRead8(ins_base + 1);
				sibt = 1;
				cpu->ins.muchine_flag |= _SIB;
				cpu->ins.sib = sib.sib;
				if (cpu->ins.index == 4 && cpu->ins.base == 5) {
					if (!(cpu->ins.muchine_flag & _SREGP)) cpu->ins.sreg = SS;
					if (cpu->ins.mod == 0) { 
						cpu->ins.disp = cpu->ins.addr = VMRead32(ins_base + 2);
#ifdef VM_DEBUG
						sprintf(cpu->rm_str,16, "0x%08X", cpu->ins.disp);
#endif
						return 6; 
					}
					else if (cpu->ins.mod == 1) { 
						cpu->ins.base = EBP;
						cpu->ins.disp = (signed long)(signed char)VMRead8(ins_base + 2);
						cpu->ins.addr = GETR32(EBP) + cpu->ins.disp;
						cpu->ins.muchine_flag |= _DISP8;
#ifdef VM_DEBUG
						sprintf(cpu->rm_str,16, "EBP + 0x%02X", (u8)cpu->ins.disp);
#endif
						return 3;
					}
					else { 
						cpu->ins.base = EBP;
						cpu->ins.disp = VMRead32(ins_base + 2);
						cpu->ins.addr = GETR32(EBP) + cpu->ins.disp;
#ifdef VM_DEBUG
						printk(cpu->rm_str, "EBP + 0x%08X", cpu->ins.disp);
#endif
						return 6;
					}
				}
				else {
					if (!(cpu->ins.muchine_flag & _SREGP)) {
						if (cpu->ins.base == 4) cpu->ins.sreg = SS;
						else cpu->ins.sreg = DS;
					}
					cpu->ins.index = sib.index;
					cpu->ins.base = sib.base;
					cpu->ins.scale = sib.scale;
					addr = (GETR32(cpu->ins.index) << cpu->ins.scale) + GETR32(cpu->ins.base);
					if (cpu->ins.mod == 0) { 
						cpu->ins.addr = addr;
#ifdef VM_DEBUG
						sprintf(cpu->rm_str,16, "%s + %s * %d", REG32[cpu->ins.base], REG32[cpu->ins.index], cpu->ins.scale);
#endif
						return 2;
					}
				}
			}
			else {
				if (!(cpu->ins.muchine_flag & _SREGP)) {
					if (cpu->ins.mod == 4) cpu->ins.sreg = SS;
					else cpu->ins.sreg = DS;
				}
				addr += GETR32(cpu->ins.rm);
				if (cpu->ins.mod == 1) {
					cpu->ins.disp = (signed long)(signed char)VMRead8(ins_base + 1 + sibt);
					cpu->ins.muchine_flag |= _DISP8;
					cpu->ins.addr = addr + cpu->ins.disp;
#ifdef VM_DEBUG
					if (sibt) sprintf(cpu->rm_str,16, "%s + %s * %d + 0x%02X", REG32[cpu->ins.base], REG32[cpu->ins.index], cpu->ins.scale, (u8)cpu->ins.disp);
					else sprintf(cpu->rm_str,16, "%s + 0x%02X", REG32[cpu->ins.rm], (u8)cpu->ins.disp);
#endif
					return 2 + sibt;
				}
				else {
					cpu->ins.disp = VMRead32(ins_base + 1 + sibt);
					cpu->ins.addr = addr + cpu->ins.disp;
#ifdef VM_DEBUG
					if (sibt) sprintf(cpu->rm_str,16, "%s + %s * %d + 0x%08X", REG32[cpu->ins.base], REG32[cpu->ins.index], cpu->ins.scale, cpu->ins.disp);
					else sprintf(cpu->rm_str,16, "%s + 0x%08X", REG32[cpu->ins.rm], cpu->ins.disp);
#endif
					return 5 + sibt;
				}
			}
		}
		return 0;
	}
	else {
		if (cpu->ins.rm == 2 || cpu->ins.rm == 3 || cpu->ins.rm == 6) {
			if (cpu->ins.mod == 5) {
				if (!(cpu->ins.muchine_flag & _SREGP))
					cpu->ins.sreg = DS;
			}
			else if (!(cpu->ins.muchine_flag & _SREGP))
				cpu->ins.sreg = SS;
		}
		else if (!(cpu->ins.muchine_flag & _SREGP))
			cpu->ins.sreg = DS;
		if (cpu->ins.mod == 0 && cpu->ins.rm == 6) { 
			cpu->ins.disp = cpu->ins.addr = VMRead16(ins_base + 1);
#ifdef VM_DEBUG
			sprintf(cpu->rm_str,16, "0x%04X", (unsigned short)cpu->ins.disp);
#endif
			return 3; 
		}
		switch (cpu->ins.rm) {
		case 0://bx+si
			addr = GETR16(BX) + GETR16(SI);
			break;
		case 1://bx+di
			addr = GETR16(BX) + GETR16(DI);
			break;
		case 2://bp+si
			addr = GETR16(BP) + GETR16(SI);
			break;
		case 3://bp+di
			addr = GETR16(BP) + GETR16(DI);
			break;
		case 4://si
			addr = GETR16(SI);
			break;
		case 5://di
			addr = GETR16(DI);
			break;
		case 6://bp
			addr = GETR16(BP);
			break;
		case 7://bx
			addr = GETR16(BX);
			break;
		}

		if (cpu->ins.mod == 1) {
			cpu->ins.disp = (signed long)(signed char)VMRead8(ins_base + 1);
			cpu->ins.addr = addr + cpu->ins.disp;
			cpu->ins.muchine_flag |= _DISP8;
#ifdef VM_DEBUG
			sprintf(cpu->rm_str,16, "%s + 0x%02X", RM16[cpu->ins.rm], (u8)cpu->ins.disp);
#endif
			return 2;
		}
		else if (cpu->ins.mod == 2) {
			cpu->ins.disp = (signed long)(signed short)VMRead16(ins_base + 1);
			cpu->ins.addr = addr + cpu->ins.disp;
#ifdef VM_DEBUG
			sprintf(cpu->rm_str,16, "%s + 0x%04X", RM16[cpu->ins.rm], (unsigned short)cpu->ins.disp);
#endif
			return 3;
		
		}
		cpu->ins.addr = addr;
#ifdef VM_DEBUG
		sprintf(cpu->rm_str,16, "%s", RM16[cpu->ins.rm]);
#endif
		return 1;
	}
}
int GetIns(struct _VM_CPU_ * cpu) {
	u32 ins_base;
	u32 insn = 0;
	u8 ins;

	//printk("GetIns.\n");
	
	ins_base = cpu->base[CS] + cpu->eip.rx;
#ifdef VM_DEBUG
	cpu->cur_eip.erx = cpu->eip.erx;
	cpu->cur_cs = cpu->sr[CS];
	sprintf(cpu->cs_ip_str,16,"%04X:%04X ", cpu->sr[CS], cpu->eip.rx);
#endif
	memset(&cpu->ins, 0, sizeof(struct _INS_));
nextins:
	ins = VMRead8(ins_base + insn);
	switch (ins) {
	case _CSOP:
		cpu->ins.sreg = CS;
		cpu->ins.muchine_flag |= _SREGP;
		insn++;
		goto nextins;
	case _DSOP:
		cpu->ins.sreg = DS;
		cpu->ins.muchine_flag |= _SREGP;
		insn++;
		goto nextins;
	case _ESOP:
		cpu->ins.sreg = ES;
		cpu->ins.muchine_flag |= _SREGP;
		insn++;
		goto nextins;
	case _FSOP:
		cpu->ins.sreg = FS;
		cpu->ins.muchine_flag |= _SREGP;
		insn++;
		goto nextins;
	case _GSOP:
		cpu->ins.sreg = GS;
		cpu->ins.muchine_flag |= _SREGP;
		insn++;
		goto nextins;
	case _SSOP:
		cpu->ins.sreg = SS;
		cpu->ins.muchine_flag |= _SREGP;
		insn++;
		goto nextins;
	case _REPEOP:
		cpu->ins.muchine_flag |= _REPEP;
		insn++;
		goto nextins;
	case _REPNEOP:
		cpu->ins.muchine_flag |= _REPNEP;
		insn++;
		goto nextins;
	case _LOCKOP:
		cpu->ins.muchine_flag |= _LOCKP;
		insn++;
		goto nextins;
	case _ASOP:
		cpu->ins.muchine_flag |= _ASP;
		insn++;
		goto nextins;
	case _OSOP:
		cpu->ins.muchine_flag |= _OSP;
		insn++;
		goto nextins;
	}
	cpu->ins.op0 = ins;
	insn++;
	if (ins == 0x0f) {//two byte opcode
		ins = VMRead8(ins_base + insn);
		insn++;
		cpu->ins.op1 = ins;
		if (bt(RME, ins)) {
			ins = VMRead8(ins_base + insn);
			cpu->ins.modrm = ins;
			insn += modrm_b(ins_base + insn, cpu);
			if (bt(RMB2, cpu->ins.op1)) {//r/m,r
				cpu->ins.des = cpu->ins.rm;
				cpu->ins.src = cpu->ins.reg;
			}
			else {
				cpu->ins.des = cpu->ins.reg;
				cpu->ins.src = cpu->ins.rm;
			}
		}
		if (cpu->ins.op1 >= 0x80) {
			if (cpu->ins.op1 <= 0x8f) {
				if (cpu->ins.muchine_flag & _OSP) {
					cpu->ins.imm1.dword = VMRead32(ins_base + insn);
					insn += 4;
				}
				else {
					cpu->ins.imm1.dword = VMRead16(ins_base + insn);
					insn += 2;
				}
			}
			else if (cpu->ins.op1 == 0xa4 || cpu->ins.op1 == 0xac || cpu->ins.op1 == 0xba) {
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn++;
			}
			else if (cpu->ins.op1 <= 0xcf && cpu->ins.op1 >= 0xc8) cpu->ins.des = cpu->ins.op1 & 0x07;
		}
		cpu->eip.rx += insn;
#ifdef VM_DEBUG
		cpu->ins_len = insn;
#endif
		switch (cpu->ins.op1) {
		case 0x00:
			if (cpu->ins.modrm >= 0xc0) GP0F00[cpu->ins.reg].RR16(cpu);
			else GP0F00[cpu->ins.reg].MR16(cpu);
			break;
		case 0x01:
			if (cpu->ins.modrm >= 0xc0) GP0F01[cpu->ins.reg].RR16(cpu);
			else GP0F01[cpu->ins.reg].MR16(cpu);
			break;
		case 0xba:
			if (cpu->ins.muchine_flag & _OSP) {
				if (cpu->ins.modrm >= 0xc0) GP0FBA[cpu->ins.reg].RR32(cpu);
				else GP0FBA[cpu->ins.reg].MR32(cpu);
			}
			else {
				if (cpu->ins.modrm >= 0xc0) GP0FBA[cpu->ins.reg].RR16(cpu);
				else GP0FBA[cpu->ins.reg].MR16(cpu);
			}
			break;
		default:
			if (bt(BTE, cpu->ins.op1)) {
				if (bt(RME, cpu->ins.op1)) {//r/m16/32,r16/32
					if (cpu->ins.muchine_flag & _OSP) {//r/m32,r32
						if (cpu->ins.modrm >= 0xc0) OPE[cpu->ins.op1].RR32(cpu);//r32,r32
						else OPE[cpu->ins.op1].MR32(cpu);//m32,r32
					}
					else {
						if (cpu->ins.modrm >= 0xc0)	OPE[cpu->ins.op1].RR16(cpu);//r16,r16
						else OPE[cpu->ins.op1].MR16(cpu);//m16,r16
					}
				}
				else {
					if (cpu->ins.muchine_flag & _OSP) {//r/m32 
						if (cpu->ins.modrm >= 0xc0) OPE[cpu->ins.op1].RR32(cpu);//r32
						else OPE[cpu->ins.op1].MR32(cpu);//m32
					}
					else {
						if (cpu->ins.modrm >= 0xc0) OPE[cpu->ins.op1].RR16(cpu);//r16
						else OPE[cpu->ins.op1].MR16(cpu);//m16
					}
				}
			}
			else {//rm8,r8 
				if (cpu->ins.modrm >= 0xc0) OPE[cpu->ins.op1].RR16(cpu);
				else OPE[cpu->ins.op1].MR16(cpu);
			}
		}
	}
	else {//one byte opcode
		if (bt(RMB, cpu->ins.op0)) {//rm byte
			ins = VMRead8(ins_base + insn);
			cpu->ins.modrm = ins;
			insn += modrm_b(ins_base + insn, cpu);
			if (bt(RMB2, cpu->ins.op0)) {//r/m,r
				cpu->ins.des = cpu->ins.rm;
				cpu->ins.src = cpu->ins.reg;
			}
			else {
				cpu->ins.des = cpu->ins.reg;
				cpu->ins.src = cpu->ins.rm;
			}
		}
		if (cpu->ins.op0 <= 0x3f) {
			if ((cpu->ins.op0 & 0x07) == 0x04) {// al,imm8
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn++;
			}
			else if((cpu->ins.op0 & 0x07) == 0x05) {//[e]ax,imm16/32
				if (cpu->ins.muchine_flag & _OSP) {
					cpu->ins.imm1.dword = VMRead32(ins_base + insn);
					insn += 4;
				}
				else {
					cpu->ins.imm1.word = VMRead16(ins_base + insn);
					insn += 2;
				}
			}
			else if ((cpu->ins.op0 & 0x07) == 0x06 || (cpu->ins.op0 & 0x07) == 0x07) {
				switch (cpu->ins.op0 >> 3) {
				case 0: cpu->ins.des = ES; break;
				case 1: cpu->ins.des = CS; break;
				case 2: cpu->ins.des = SS; break;
				case 3: cpu->ins.des = DS; break;
				}
			}
		}
		else if (cpu->ins.op0 <= 0x5f) cpu->ins.des = cpu->ins.op0 & 0x07;
		else if (cpu->ins.op0 <= 0x6f) {
			if (cpu->ins.op0 == 0x68 || cpu->ins.op0 == 0x69) {
				if (cpu->ins.muchine_flag & _OSP) {
					cpu->ins.imm1.dword = VMRead32(ins_base + insn);
					insn += 4;
				}
				else {
					cpu->ins.imm1.dword = VMRead16(ins_base + insn);
					insn += 2;
				}
			}
			else if (cpu->ins.op0 == 0x6b) {
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn += 1;
			}
			else if(cpu->ins.op0 == 0x6a){
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn++;
			}
		}
		else if (cpu->ins.op0 <= 0x7f) {
			cpu->ins.imm1.byte = VMRead8(ins_base + insn);
			insn += 1;
		}
		else if (cpu->ins.op0 <= 0x9f) {
			if (cpu->ins.op0 == 0x80 || cpu->ins.op0 == 0x83) {
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn += 1;
			}
			else if (cpu->ins.op0 == 0x81) {
				if (cpu->ins.muchine_flag & _OSP) {
					cpu->ins.imm1.dword = VMRead32(ins_base + insn);
					insn += 4;
				}
				else {
					cpu->ins.imm1.dword = VMRead16(ins_base + insn);
					insn += 2;
				}
			}
			else if (cpu->ins.op0 == 0x9a) {
				if (cpu->ins.muchine_flag & _OSP) {
					cpu->ins.imm2.dword = VMRead32(ins_base + insn);
					cpu->ins.imm1.word = VMRead16(ins_base + insn + 4);
					insn += 6;
				}
				else {
					cpu->ins.imm2.word = VMRead16(ins_base + insn);
					cpu->ins.imm1.word = VMRead16(ins_base + insn + 2);
					insn += 4;
				}
			}
		
		}
		else if (cpu->ins.op0 <= 0xaf) {
			if (cpu->ins.op0 == 0xa0 || cpu->ins.op0 == 0xa2 || cpu->ins.op0 == 0xa1 || cpu->ins.op0 == 0xa3) {
				cpu->ins.addr = cpu->ins.imm1.word = VMRead16(ins_base + insn);
				insn += 2;
				if (!(cpu->ins.muchine_flag & _SREGP)) cpu->ins.sreg = DS;
			}
			else if (cpu->ins.op0 == 0xa8) {
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn += 1;
			}
			else if (cpu->ins.op0 == 0xa9) {
				if (cpu->ins.muchine_flag & _OSP) {
					cpu->ins.imm1.dword = VMRead32(ins_base + insn);
					insn += 4;
				}
				else {
					cpu->ins.imm1.dword = VMRead16(ins_base + insn);
					insn += 2;
				}
			}
		}
		else if (cpu->ins.op0 <= 0xbf) {
			cpu->ins.des = cpu->ins.op0 & 0x07;
			if (cpu->ins.op0 <= 0xb7) {
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn += 1;
			}
			else {
				if (cpu->ins.muchine_flag & _OSP) {
					cpu->ins.imm1.dword = VMRead32(ins_base + insn);
					insn += 4;
				}
				else {
					cpu->ins.imm1.word = VMRead16(ins_base + insn);
					insn += 2;
				}
			}
		}
		else if (cpu->ins.op0 <= 0xcf) {
			if (cpu->ins.op0 == 0xc0 || cpu->ins.op0 == 0xc1) {
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn += 1;
				cpu->ins.counter = cpu->ins.imm1.byte;
			}
			else if (cpu->ins.op0 == 0xc2) {
				cpu->ins.imm1.dword = VMRead16(ins_base + insn);
				insn += 2;
			}
			else if (cpu->ins.op0 == 0xc6) {
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn += 1;
			}
			else if (cpu->ins.op0 == 0xc7) {
				if (cpu->ins.muchine_flag & _OSP) {
					cpu->ins.imm1.dword = VMRead32(ins_base + insn);
					insn += 4;
				}
				else {
					cpu->ins.imm1.dword = VMRead16(ins_base + insn);
					insn += 2;
				}
			}
			else if (cpu->ins.op0 == 0xc8) {
				cpu->ins.imm1.dword = VMRead16(ins_base + insn);
				insn += 2;
				cpu->ins.imm2.byte = VMRead8(ins_base + insn);
				insn += 1;
			}
			else if (cpu->ins.op0 == 0xca) {
				cpu->ins.imm1.dword = VMRead16(ins_base + insn);
				insn += 2;
			}
			else if (cpu->ins.op0 == 0xcd) {
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn += 1;
			}
		}
		else if (cpu->ins.op0 <= 0xdf) {
			if (cpu->ins.op0 == 0xd0 || cpu->ins.op0 == 0xd1) cpu->ins.counter = 1;
			else if (cpu->ins.op0 == 0xd2 || cpu->ins.op0 == 0xd3) cpu->ins.counter = GETR8(CL);
			else if (cpu->ins.op0 == 0xd4 || cpu->ins.op0 == 0xd5) {
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn += 1;
			}
		}
		else if (cpu->ins.op0 <= 0xef) {
			if (cpu->ins.op0 <= 0xe7 || cpu->ins.op0 == 0xeb) {
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn += 1;
			}
			else if (cpu->ins.op0 == 0xe8 || cpu->ins.op0 == 0xe9) {
				if (cpu->ins.muchine_flag & _OSP) {
					cpu->ins.imm1.dword = VMRead32(ins_base + insn);
					insn += 4;
				}
				else {
					cpu->ins.imm1.dword = VMRead16(ins_base + insn);
					insn += 2;
				}
			}
			else if (cpu->ins.op0 == 0xea) {
				if (cpu->ins.muchine_flag & _OSP) {
					cpu->ins.imm2.dword = VMRead32(ins_base + insn);
					cpu->ins.imm1.word = VMRead16(ins_base + insn + 4);
					insn += 6;
				}
				else {
					cpu->ins.imm2.word = VMRead16(ins_base + insn);
					cpu->ins.imm1.word = VMRead16(ins_base + insn + 2);
					insn += 4;
				}
			}
		}
		else {
			if (cpu->ins.op0 == 0xf6 && cpu->ins.reg == 0) {
				cpu->ins.imm1.byte = VMRead8(ins_base + insn);
				insn += 1;
			}
			else if (cpu->ins.op0 == 0xf7 && cpu->ins.reg == 0) {
				if (cpu->ins.muchine_flag & _OSP) {
					cpu->ins.imm1.dword = VMRead32(ins_base + insn);
					insn += 4;
				}
				else {
					cpu->ins.imm1.word = VMRead16(ins_base + insn);
					insn += 2;
				}
			}
		}
		cpu->eip.rx += insn;
#ifdef VM_DEBUG
		cpu->ins_len = insn;
#endif
		switch (cpu->ins.op0) {
		case 0x80:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.mod == 3) GP80[cpu->ins.reg].RR16(cpu);
			else GP80[cpu->ins.reg].MR16(cpu);
			break;
		case 0x81:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.muchine_flag & _OSP) {
				if (cpu->ins.mod == 3) GP81[cpu->ins.reg].RR32(cpu);
				else GP81[cpu->ins.reg].MR32(cpu);
			}
			else {
				if (cpu->ins.mod == 3) GP81[cpu->ins.reg].RR16(cpu);
				else GP81[cpu->ins.reg].MR16(cpu);
			}
			break;
		case 0x83:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.muchine_flag & _OSP) {
				if (cpu->ins.mod == 3) GP83[cpu->ins.reg].RR32(cpu);
				else GP83[cpu->ins.reg].MR32(cpu);
			}
			else {
				if (cpu->ins.mod == 3) GP83[cpu->ins.reg].RR16(cpu);
				else GP83[cpu->ins.reg].MR16(cpu);
			}
			break;
		case 0xc0:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.mod == 3) GPC0[cpu->ins.reg].RR16(cpu);
			else GPC0[cpu->ins.reg].MR16(cpu);
			break;
		case 0xc1:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.muchine_flag & _OSP) {
				if (cpu->ins.mod == 3) GPC1[cpu->ins.reg].RR32(cpu);
				else GPC1[cpu->ins.reg].MR32(cpu);
			}
			else {
				if (cpu->ins.mod == 3) GPC1[cpu->ins.reg].RR16(cpu);
				else GPC1[cpu->ins.reg].MR16(cpu);
			}
			break;
		case 0xd0:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.mod == 3) GPD0[cpu->ins.reg].RR16(cpu);
			else GPD0[cpu->ins.reg].MR16(cpu);
			break;
		case 0xd1:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.muchine_flag & _OSP) {
				if (cpu->ins.mod == 3) GPD1[cpu->ins.reg].RR32(cpu);
				else GPD1[cpu->ins.reg].MR32(cpu);
			}
			else {
				if (cpu->ins.mod == 3) GPD1[cpu->ins.reg].RR16(cpu);
				else GPD1[cpu->ins.reg].MR16(cpu);
			}
			break;
		case 0xd2:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.mod == 3) GPD2[cpu->ins.reg].RR16(cpu);
			else GPD2[cpu->ins.reg].MR16(cpu);
			break;
		case 0xd3:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.muchine_flag & _OSP) {
				if (cpu->ins.mod == 3) GPD3[cpu->ins.reg].RR32(cpu);
				else GPD3[cpu->ins.reg].MR32(cpu);
			}
			else {
				if (cpu->ins.mod == 3) GPD3[cpu->ins.reg].RR16(cpu);
				else GPD3[cpu->ins.reg].MR16(cpu);
			}
			break;
		case 0xf6:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.mod == 3) GPF6[cpu->ins.reg].RR16(cpu);
			else GPF6[cpu->ins.reg].MR16(cpu);
			break;
		case 0xf7:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.muchine_flag & _OSP) {
				if (cpu->ins.mod == 3) GPF7[cpu->ins.reg].RR32(cpu);
				else GPF7[cpu->ins.reg].MR32(cpu);
			}
			else {
				if (cpu->ins.mod == 3) GPF7[cpu->ins.reg].RR16(cpu);
				else GPF7[cpu->ins.reg].MR16(cpu);
			}
			break;
		case 0xfe:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.mod == 3) GPFE[cpu->ins.reg].RR16(cpu);
			else GPFE[cpu->ins.reg].MR16(cpu);
			break;
		case 0xff:
			cpu->ins.des = cpu->ins.rm;
			if (cpu->ins.muchine_flag & _OSP) {
				if (cpu->ins.mod == 3) GPFF[cpu->ins.reg].RR32(cpu);
				else GPFF[cpu->ins.reg].MR32(cpu);
			}
			else {
				if (cpu->ins.mod == 3) GPFF[cpu->ins.reg].RR16(cpu);
				else GPFF[cpu->ins.reg].MR16(cpu);
			}
			break;
		default:
			if (bt(BTB, cpu->ins.op0)) {
				if (bt(RMB, cpu->ins.op0)) {//r/m16/32,r16/32
					if (cpu->ins.muchine_flag & _OSP) {//r/m32,r32
						if (cpu->ins.modrm >= 0xc0) OPB[cpu->ins.op0].RR32(cpu);//r32,r32 
						else OPB[cpu->ins.op0].MR32(cpu);//m32,r32
					}
					else {
						if (cpu->ins.modrm >= 0xc0) OPB[cpu->ins.op0].RR16(cpu);//r16,r16
						else OPB[cpu->ins.op0].MR16(cpu);//m16,r16
					}
				}
				else {
					if (cpu->ins.muchine_flag & _OSP) {//r/m32 
						if (cpu->ins.modrm >= 0xc0) OPB[cpu->ins.op0].RR32(cpu);//r32 
						else OPB[cpu->ins.op0].MR32(cpu);//m32 
					}
					else {
						if (cpu->ins.modrm >= 0xc0) OPB[cpu->ins.op0].RR16(cpu);//r16 
						else OPB[cpu->ins.op0].MR16(cpu);//m16 
					}
				}
			}
			else {
				//rm8,r8
				if (cpu->ins.modrm >= 0xc0) OPB[cpu->ins.op0].RR16(cpu); 
				else OPB[cpu->ins.op0].MR16(cpu); 
			}
		}
	}
	return 0;
}
int vm_alloc_memory(u32 size){
	struct _MLOG_ * Log;
	struct _MLOG_ * Logt;
	int ret;
	
	if(!size) return -1;
	size += sizeof(struct _MLOG_) - 1;
	size &= ~(sizeof(struct _MLOG_) - 1);
	
	for(Log = First;Log;Log = Log->Next){
		if(Log->flag) continue;
		if(Log->Size < size) continue;
		if(Log->Size > size){
			if(Log->Size == size + sizeof(struct _MLOG_)){
				Log->flag = BLOCK_FLAG_USED;
				return ((u64)(Log+1)) - (u64)vm_memory_base;
			}
			else{
				Logt = (struct _MLOG_ *)(((u64)(Log + 1)) + size);
				Logt->Next = Log->Next;
				if(Logt->Next) Logt->Next->Prev = Logt;
				Logt->Prev = Log;
				Log->Next = Logt;
				Logt->Size = Log->Size - size - sizeof(struct _MLOG_);
				Log->Size = size;
				Logt->gs_type = GST_VM_MEMORY_BLOCK;
				Logt->flag = BLOCK_FLAG_FREE;
				Log->flag = BLOCK_FLAG_USED;
				return ((u64)(Log + 1)) - (u64)vm_memory_base;
			}
		}
		Log->gs_type = BLOCK_FLAG_USED;
		return ((u64)(Log+1)) - (u64)vm_memory_base;
	}
	return -1;
}
void vm_free_memory(int base){
	struct _MLOG_ * Log;
	struct _MLOG_ * Logt;
	
	Log = (struct _MLOG_ *)((vm_memory_base) + base - sizeof(struct _MLOG_));
	Log->flag = BLOCK_FLAG_FREE;
	if(Log->Prev){
		Logt = Log->Prev;
		if(Logt->flag == BLOCK_FLAG_FREE){
			Logt->gs_type = GST_UNDEF;
			Logt->Size += Log->Size + sizeof(struct _MLOG_);
			Logt->Next = Log->Next;
			if(Log->Next) Log->Next->Prev = Logt;
			Log = Logt;
		}
	}
	if(Log->Next){
		Logt = Log->Next;
		if(Log->flag == BLOCK_FLAG_FREE){
			Log->gs_type = GST_UNDEF;
			Log->Size += Logt->Size + sizeof(struct _MLOG_);
			Log->Next = Logt->Next;
			if(Logt->Next) Logt->Next->Prev = Log;
		}
	}
}
int int86_init(){
	int i;

	for(i = 0;i < 256;i++) Root[i] = 0x00cd + (i << 8);
	First->gs_type = GST_VM_MEMORY_BLOCK;
	First->flag = BLOCK_FLAG_FREE;
	First->Prev = 0;
	First->Next = 0;
	First->Size = ((vm_memory_base + VM_FREE_SPACE_START + VM_FREE_SPACE_SIZE) & 0xfffffffffffffff0) - (u64)First - sizeof(struct _MLOG_);
	return 0;
}
#ifdef VM_DEBUG

static int debug_enable;

static int cond(struct CPU cpu0,LPVCPU cpu1){
	int ch;
	int i;
	static u16 ip[] = {
		
	};
	
	if(!debug_enable) return 0;
	
	if(sizeof(ip) == 0){
		printk("EAX:%08X ECX:%08X EDX:%08X EBX:%08X ESP:%08X EBP:%08X\n"
			"ESI:%08X EDI:%08X CS:%04X DS:%04X ES:%04X SS:%04X F:%04X\n",
			cpu1->gr[EAX].erx,cpu1->gr[ECX].erx,cpu1->gr[EDX].erx,cpu1->gr[EBX].erx,
			cpu1->gr[ESP].erx,cpu1->gr[EBP].erx,cpu1->gr[ESI].erx,cpu1->gr[EDI].erx,
			cpu1->sr[CS],cpu1->sr[DS],cpu1->sr[ES],cpu1->sr[SS],cpu1->eflags.rx);
		print(cpu1->cs_ip_str);
		putchar(' ');
		for(i = 0;i < cpu1->ins_len;i++) 
			printk("%02X ",vm_read_byte((((u32)cpu1->cur_cs) << 4) + cpu1->cur_eip.rx + i));
		print(cpu1->ins_str);
		putchar('\n');
		while(1){
			ch = getchar();
			if(!(ch & KEY_S_LOOSEN)) break;
		}
		return 0;
		
	}
	else{
		for(i = 0;i < sizeof(ip)/sizeof(u16);i++){
			if(cpu1->cur_eip.rx == ip[i]){
				printk("EAX:%08X ECX:%08X EDX:%08X EBX:%08X ESP:%08X EBP:%08X\n"
					"ESI:%08X EDI:%08X CS:%04X DS:%04X ES:%04X SS:%04X F:%04X\n",
					cpu1->gr[EAX].erx,cpu1->gr[ECX].erx,cpu1->gr[EDX].erx,cpu1->gr[EBX].erx,
					cpu1->gr[ESP].erx,cpu1->gr[EBP].erx,cpu1->gr[ESI].erx,cpu1->gr[EDI].erx,
					cpu1->sr[CS],cpu1->sr[DS],cpu1->sr[ES],cpu1->sr[SS],cpu1->eflags.rx);
				print(cpu1->cs_ip_str);
				putchar(' ');
				for(i = 0;i < cpu1->ins_len;i++) 
					printk("%02X ",vm_read_byte((((u32)cpu1->cur_cs) << 4) + cpu1->cur_eip.rx + i));
				print(cpu1->ins_str);
				putchar('\n');
				while(1){
					ch = getchar();
					if(!(ch & KEY_S_LOOSEN)) break;
				}
				break;
			}	
		}
	}
	return 0;
}
#endif
int int86(uint8_t n,struct CPU * cpu){
	int c = 0;
	int stack;
	struct _VM_CPU_ vm_cpu;
	
	vm_cpu.gr[EAX].erx = cpu->gr[EAX];
	vm_cpu.gr[ECX].erx = cpu->gr[ECX];
	vm_cpu.gr[EDX].erx = cpu->gr[EDX];
	vm_cpu.gr[EBX].erx = cpu->gr[EBX];
	vm_cpu.gr[EBP].erx = cpu->gr[EBP];
	vm_cpu.gr[ESI].erx = cpu->gr[ESI];
	vm_cpu.gr[EDI].erx = cpu->gr[EDI];
	stack = vm_alloc_memory(16384);
	vm_cpu.gr[ESP].rx = 16382;
	vm_cpu.base[SS] = stack;
	vm_cpu.sr[SS] = stack >> 4;
	vm_cpu.sr[DS] = cpu->sr[DS];
	vm_cpu.base[DS] = ((u64)cpu->sr[DS]) << 4;
	vm_cpu.sr[ES] = cpu->sr[ES];
	vm_cpu.base[ES] = ((u64)cpu->sr[ES]) << 4;
	vm_cpu.eip.rx = Root_IP + 2*n;
	vm_cpu.sr[CS] = Root_CS;
	vm_cpu.base[CS] = ((u64)Root_CS) << 4;
	vm_cpu.eflags.erx = cpu->flags | 0x00000202;
#ifdef VM_DEBUG
	vm_cpu.cond = cond;
	debug_enable = 0;
#endif
	while(1){
		GetIns(&vm_cpu);
		if((vm_cpu.eip.rx == Root_IP + 2*n + 2) && vm_cpu.sr[CS] == Root_CS) break;
	}
	vm_free_memory(stack);
	cpu->gr[EAX] = vm_cpu.gr[EAX].erx;
	cpu->gr[EBX] = vm_cpu.gr[EBX].erx;
	cpu->gr[ECX] = vm_cpu.gr[ECX].erx;
	cpu->gr[EDX] = vm_cpu.gr[EDX].erx;
	cpu->gr[EBP] = vm_cpu.gr[EBP].erx;
	cpu->gr[ESI] = vm_cpu.gr[ESI].erx;
	cpu->gr[EDI] = vm_cpu.gr[EDI].erx;
	cpu->flags = vm_cpu.eflags.erx;
	cpu->sr[DS] = vm_cpu.sr[DS];
	cpu->sr[ES] = vm_cpu.sr[ES];
	return 0;
}






