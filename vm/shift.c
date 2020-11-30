/*
	vm/shift.c
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

void ROR_Mb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROR BYTE [%s:%s], 0x%02X", 
			SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"ROR BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd0) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROR BYTE [%s:%s], 1", 
			SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"ROR BYTE [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd2) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROR BYTE [%s:%s], CL", 
			SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"ROR BYTE [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 9;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		val0 |= CHEF(CF) ? 0x80 : 0;
	}
	VMWrite8(addr, val0);
}
void ROR_Mw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROR WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"ROR WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROR WORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"ROR WORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROR WORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"ROR WORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 17;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		val0 |= CHEF(CF) ? 0x8000 : 0;
	}
	VMWrite16(addr, val0);
}
void ROR_Md(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROR DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"ROR DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROR DWORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"ROR DWORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROR DWORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"ROR DWORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		val0 |= CHEF(CF) ? 0x80000000 : 0;
	}
	VMWrite32(addr, val0);
}
void ROR_Rb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) sprintf(cpu->ins_str,64,"ROR %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd0) sprintf(cpu->ins_str,64,"ROR %s, 1", REG8[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd2) sprintf(cpu->ins_str,64,"ROR %s, CL", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 9;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		val0 |= CHEF(CF) ? 0x80 : 0;
	}
	SETR8(cpu->ins.des, val0);
}
void ROR_Rw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"ROR %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"ROR %s, 1", REG16[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"ROR %s, CL", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 17;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		val0 |= CHEF(CF) ? 0x8000 : 0;
	}
	SETR16(cpu->ins.des, val0);
}
void ROR_Rd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"ROR %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"ROR %s, 1", REG32[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"ROR %s, CL", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		val0 |= CHEF(CF) ? 0x80000000 : 0;
	}
	SETR32(cpu->ins.des, val0);
}
void ROL_Mb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROL BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"ROL BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd0) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROL BYTE [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"ROL BYTE [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd2) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROL BYTE [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"ROL BYTE [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 9;
	for (;counter;counter--) {
		if (val0 & 0x80) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
		val0 |= GETF(CF);
	}
	VMWrite8(addr, val0);
}
void ROL_Mw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROL WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"ROL WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROL WORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"ROL WORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROL WORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"ROL WORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 17;
	for (;counter;counter--) {
		if (val0 & 0x8000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
		val0 |= GETF(CF);
	}
	VMWrite16(addr, val0);
}
void ROL_Md(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROL DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"ROL DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROL DWORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"ROL DWORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"ROL DWORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"ROL DWORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x80000000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
		val0 |= GETF(CF);
	}
	VMWrite32(addr, val0);
}
void ROL_Rb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) sprintf(cpu->ins_str,64,"ROL %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd0) sprintf(cpu->ins_str,64,"ROL %s, 1", REG8[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd2) sprintf(cpu->ins_str,64,"ROL %s, CL", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 9;
	for (;counter;counter--) {
		if (val0 & 0x80) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
		val0 |= GETF(CF);
	}
	SETR8(cpu->ins.des, val0);
}
void ROL_Rw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"ROL %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"ROL %s, 1", REG16[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"ROL %s, CL", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 17;
	for (;counter;counter--) {
		if (val0 & 0x8000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
		val0 |= GETF(CF);
	}
	SETR16(cpu->ins.des, val0);
}
void ROL_Rd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"ROL %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"ROL %s, 1", REG32[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"ROL %s, CL", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x80000000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
		val0 |= GETF(CF);
	}
	SETR32(cpu->ins.des, val0);
}
void RCR_Mb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 cf;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCR BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"RCR BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd0) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCR BYTE [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"RCR BYTE [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd2) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCR BYTE [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"RCR BYTE [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 9;
	for (;counter;counter--) {
		cf = CHEF(CF) ? 0x80 : 0;
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		val0 |= cf;
	}
	VMWrite8(addr, val0);
}
void RCR_Mw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 cf;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCR WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"RCR WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCR WORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"RCR WORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCR WORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"RCR WORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 17;
	for (;counter;counter--) {
		cf = CHEF(CF) ? 0x8000 : 0;
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		val0 |= cf;
	}
	VMWrite16(addr, val0);
}
void RCR_Md(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 cf;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCR DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"RCR DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCR DWORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"RCR DWORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCR DWORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"RCR DWORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 0x1f;
	for (;counter;counter--) {
		cf = CHEF(CF) ? 0x80000000 : 0;
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		val0 |= cf;
	}
	VMWrite32(addr, val0);
}
void RCR_Rb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 cf;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) sprintf(cpu->ins_str,64,"RCR %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd0) sprintf(cpu->ins_str,64,"RCR %s, 1", REG8[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd2) sprintf(cpu->ins_str,64,"RCR %s, CL", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 9;
	for (;counter;counter--) {
		cf = CHEF(CF) ? 0x80 : 0;
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		val0 |= cf;
	}
	SETR8(cpu->ins.des, val0);
}
void RCR_Rw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 cf;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"RCR %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"RCR %s, 1", REG16[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"RCR %s, CL", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 17;
	for (;counter;counter--) {
		cf = CHEF(CF) ? 0x8000 : 0;
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		val0 |= cf;
	}
	SETR16(cpu->ins.des, val0);
}
void RCR_Rd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 cf;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"RCR %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"RCR %s, 1", REG32[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"RCR %s, CL", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 0x1f;
	for (;counter;counter--) {
		cf = CHEF(CF) ? 0x80000000 : 0;
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		val0 |= cf;
	}
	SETR32(cpu->ins.des, val0);
}
void RCL_Mb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 cf;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCL BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"RCL BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd0) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCL BYTE [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"RCL BYTE [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd2) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCL BYTE [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"RCL BYTE [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 9;
	for (;counter;counter--) {
		cf = GETF(CF);
		if (val0 & 0x80) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
		val0 |= cf;
	}
	VMWrite8(addr, val0);
}
void RCL_Mw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 cf;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCL WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"RCL WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCL WORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"RCL WORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCL WORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"RCL WORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 17;
	for (;counter;counter--) {
		cf = GETF(CF);
		if (val0 & 0x8000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
		val0 |= cf;
	}
	VMWrite16(addr, val0);
}
void RCL_Md(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 cf;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCL DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"RCL DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCL DWORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"RCL DWORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"RCL DWORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"RCL DWORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 0x1f;
	for (;counter;counter--) {
		cf = GETF(CF);
		if (val0 & 0x80000000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
		val0 |= cf;
	}
	VMWrite32(addr, val0);
}
void RCL_Rb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 cf;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) sprintf(cpu->ins_str,64,"RCL %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd0) sprintf(cpu->ins_str,64,"RCL %s, 1", REG8[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd2) sprintf(cpu->ins_str,64,"RCL %s, CL", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 9;
	for (;counter;counter--) {
		cf = GETF(CF);
		if (val0 & 0x80) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
		val0 |= cf;
	}
	SETR8(cpu->ins.des, val0);
}
void RCL_Rw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 cf;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"RCL %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"RCL %s, 1", REG16[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"RCL %s, CL", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 17;
	for (;counter;counter--) {
		cf = GETF(CF);
		if (val0 & 0x8000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
		val0 |= cf;
	}
	SETR16(cpu->ins.des, val0);
}
void RCL_Rd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 cf;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"RCL %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"RCL %s, 1", REG32[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"RCL %s, CL", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter %= 0x1f;
	for (;counter;counter--) {
		cf = GETF(CF);
		if (val0 & 0x80000000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
		val0 |= cf;
	}
	SETR32(cpu->ins.des, val0);
}
void SHR_Mb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val0_o = val0;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHR BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"SHR BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd0) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHR BYTE [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SHR BYTE [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd2) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHR BYTE [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SHR BYTE [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
	}
	if((cpu->ins.counter & 0x1f) == 1){
		if (val0_o & 0x80) SETF(OF);
		else CLRF(OF);
	}
	VMWrite8(addr, val0);
}
void SHR_Mw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val0_o = val0;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHR WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"SHR WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHR WORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SHR WORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHR WORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SHR WORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (val0_o & 0x8000) SETF(OF);
		else CLRF(OF);
	}
	VMWrite16(addr, val0);
}
void SHR_Md(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val0_o = val0;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHR DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"SHR DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHR DWORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SHR DWORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHR DWORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SHR DWORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (val0_o & 0x80000000) SETF(OF);
		else CLRF(OF);
	}
	VMWrite32(addr, val0);
}
void SHR_Rb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val0_o = val0;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) sprintf(cpu->ins_str,64,"SHR %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd0) sprintf(cpu->ins_str,64,"SHR %s, 1", REG8[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd2) sprintf(cpu->ins_str,64,"SHR %s, CL", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (val0_o & 0x80) SETF(OF);
		else CLRF(OF);
	}
	SETR8(cpu->ins.des, val0);
}
void SHR_Rw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val0_o = val0;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"SHR %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"SHR %s, 1", REG16[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"SHR %s, CL", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (val0_o & 0x8000) SETF(OF);
		else CLRF(OF);
	}
	SETR16(cpu->ins.des, val0);
}
void SHR_Rd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val0_o = val0;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"SHR %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"SHR %s, 1", REG32[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"SHR %s, CL", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (val0_o & 0x80000000) SETF(OF);
		else CLRF(OF);
	}
	SETR32(cpu->ins.des, val0);
}
void SHL_Mb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHL BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"SHL BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd0) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHL BYTE [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SHL BYTE [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd2) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHL BYTE [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SHL BYTE [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x80) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (GETF(CF) ^ (val0 & 0x80 ? 1 : 0)) SETF(OF);
		else CLRF(OF);
	}
	VMWrite8(addr, val0);
}
void SHL_Mw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHL WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"SHL WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHL WORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SHL WORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHL WORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SHL WORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x8000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (GETF(CF) ^ (val0 & 0x8000 ? 1 : 0)) SETF(OF);
		else CLRF(OF);
	}
	VMWrite16(addr, val0);
}
void SHL_Md(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHL DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"SHL DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHL DWORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SHl DWORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHL DWORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SHL DWORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x80000000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (GETF(CF) ^ (val0 & 0x80000000 ? 1 : 0)) SETF(OF);
		else CLRF(OF);
	}
	VMWrite32(addr, val0);
}
void SHL_Rb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) sprintf(cpu->ins_str,64,"SHL %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd0) sprintf(cpu->ins_str,64,"SHL %s, 1", REG8[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd2) sprintf(cpu->ins_str,64,"SHL %s, CL", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x80) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (GETF(CF) ^ (val0 & 0x80 ? 1 : 0)) SETF(OF);
		else CLRF(OF);
	}
	SETR8(cpu->ins.des, val0);
}
void SHL_Rw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"SHL %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"SHL %s, 1", REG16[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"SHL %s, CL", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x8000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (GETF(CF) ^ (val0 & 0x8000 ? 1 : 0)) SETF(OF);
		else CLRF(OF);
	}
	SETR16(cpu->ins.des, val0);
}
void SHL_Rd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"SHL %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"SHL %s, 1", REG32[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"SHL %s, CL", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x80000000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (GETF(CF) ^ (val0 & 0x80000000 ? 1 : 0)) SETF(OF);
		else CLRF(OF);
	}
	SETR32(cpu->ins.des, val0);
}
void SAR_Mb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 val0_o = val0;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAR BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"SAR BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd0) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAR BYTE [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SAR BYTE [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd2) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAR BYTE [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SAR BYTE [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		if (val0 & 0x40) val0 |= 0x80;
	}
	if ((cpu->ins.counter & 0x1f) == 1) CLRF(OF);
	VMWrite8(addr, val0);
}
void SAR_Mw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val0_o = val0;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAR WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"SAR WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAR WORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SAR WORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAR WORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SAR WORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		if (val0 & 0x4000) val0 |= 0x8000;
	}
	if ((cpu->ins.counter & 0x1f) == 1) CLRF(OF);
	VMWrite16(addr, val0);
}
void SAR_Md(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val0_o = val0;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAR DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"SAR DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAR DWORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SAR DWORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAR DWORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SAR DWORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		if (val0 & 0x40000000) val0 |= 0x80000000;
	}
	if ((cpu->ins.counter & 0x1f) == 1) CLRF(OF);
	VMWrite32(addr, val0);
}
void SAR_Rb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 val0_o = val0;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) sprintf(cpu->ins_str,64,"SAR %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd0) sprintf(cpu->ins_str,64,"SAR %s, 1", REG8[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd2) sprintf(cpu->ins_str,64,"SAR %s, CL", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		if (val0 & 0x40) val0 |= 0x80;
	}
	if ((cpu->ins.counter & 0x1f) == 1) CLRF(OF);
	SETR8(cpu->ins.des, val0);
}
void SAR_Rw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u16 val0_o = val0;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"SAR %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"SAR %s, 1", REG16[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"SAR %s, CL", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		if (val0 & 0x4000) val0 |= 0x8000;
	}
	if ((cpu->ins.counter & 0x1f) == 1) CLRF(OF);
	SETR16(cpu->ins.des, val0);
}
void SAR_Rd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val0_o = val0;
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"SAR %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"SAR %s, 1", REG32[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"SAR %s, CL", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 1) SETF(CF);
		else CLRF(CF);
		val0 >>= 1;
		if (val0 & 0x40000000) val0 |= 0x80000000;
	}
	if ((cpu->ins.counter & 0x1f) == 1) CLRF(OF);
	SETR32(cpu->ins.des, val0);
}
void SAL_Mb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0 = VMRead8(addr);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAL BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"SAL BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd0) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAL BYTE [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SAL BYTE [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd2) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAL BYTE [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SAL BYTE [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x80) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (GETF(CF) ^ (val0 & 0x80 ? 1 : 0)) SETF(OF);
		else CLRF(OF);
	}
	VMWrite8(addr, val0);
}
void SAL_Mw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAL WORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"SAL WORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAL WORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SAL WORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAL WORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SAL WORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x8000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (GETF(CF) ^ (val0 & 0x8000 ? 1 : 0)) SETF(OF);
		else CLRF(OF);
	}
	VMWrite16(addr, val0);
}
void SAL_Md(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) {//imm8
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAL DWORD [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
		else sprintf(cpu->ins_str,64,"SAL DWORD [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	}
	else if (cpu->ins.op0 == 0xd1) {//1
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAL DWORD [%s:%s], 1", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SAL DWORD [%s], 1", cpu->rm_str);
	}
	else if (cpu->ins.op0 == 0xd3) {//CL
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SAL DWORD [%s:%s], CL", SREG[cpu->ins.sreg], cpu->rm_str);
		else sprintf(cpu->ins_str,64,"SAL DWORD [%s], CL", cpu->rm_str);
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x80000000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (GETF(CF) ^ (val0 & 0x80000000 ? 1 : 0)) SETF(OF);
		else CLRF(OF);
	}
	VMWrite32(addr, val0);
}
void SAL_Rb(LPVCPU cpu) {
	u8 val0 = GETR8(cpu->ins.des);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc0) sprintf(cpu->ins_str,64,"SAL %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd0) sprintf(cpu->ins_str,64,"SAL %s, 1", REG8[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd2) sprintf(cpu->ins_str,64,"SAL %s, CL", REG8[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x80) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (GETF(CF) ^ (val0 & 0x80 ? 1 : 0)) SETF(OF);
		else CLRF(OF);
	}
	SETR8(cpu->ins.des, val0);
}
void SAL_Rw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"SAL %s, 0x%02X", REG16[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"SAL %s, 1", REG16[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"SAL %s, CL", REG16[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x8000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (GETF(CF) ^ (val0 & 0x8000 ? 1 : 0)) SETF(OF);
		else CLRF(OF);
	}
	SETR16(cpu->ins.des, val0);
}
void SAL_Rd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u8 counter = cpu->ins.counter;
#ifdef VM_DEBUG
	if (cpu->ins.op0 == 0xc1) sprintf(cpu->ins_str,64,"SAL %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	else if (cpu->ins.op0 == 0xd1) sprintf(cpu->ins_str,64,"SAL %s, 1", REG32[cpu->ins.des]);
	else if (cpu->ins.op0 == 0xd3) sprintf(cpu->ins_str,64,"SAL %s, CL", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	for (;counter;counter--) {
		if (val0 & 0x80000000) SETF(CF);
		else CLRF(CF);
		val0 <<= 1;
	}
	if ((cpu->ins.counter & 0x1f) == 1) {
		if (GETF(CF) ^ (val0 & 0x80000000 ? 1 : 0)) SETF(OF);
		else CLRF(OF);
	}
	SETR32(cpu->ins.des, val0);
}
void SHLD_MdRdCL(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
	u8 counter = GETR8(CL);
	u8 i;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHLD [%s:%s], %s, CL", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"SHLD [%s], %s, CL", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	if (counter == 0) return;
	if (val0 & (1 << (32 - counter))) SETF(CF);
	else CLRF(CF);
	for (i = 31; i >= counter;i--) {
		if (val0 & (1 << (i - counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = counter - 1;i;i--) {
		if (val1 & (1 << (i + 32 - counter))) val1 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	VMWrite32(addr, val0);
}
void SHLD_MdRdIb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
	u8 counter = cpu->ins.imm1.byte;
	u8 i;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHLD [%s:%s], %s, 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src],cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"SHLD [%s], %s, 0x%02X", cpu->rm_str, REG32[cpu->ins.src],cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	if (counter == 0) return;
	if (val0 & (1 << (32 - counter))) SETF(CF);
	else CLRF(CF);
	for (i = 31; i >= counter;i--) {
		if (val0 & (1 << (i - counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = counter - 1;i;i--) {
		if (val1 & (1 << (i + 32 - counter))) val1 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	VMWrite32(addr, val0);
}
void SHLD_MwRwCL(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
	u8 counter = GETR8(CL);
	u8 i;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHLD [%s:%s], %s, CL", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"SHLD [%s], %s, CL", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x0f;
	if (counter == 0) return;
	if (val0 & (1 << (16 - counter))) SETF(CF);
	else CLRF(CF);
	for (i = 15; i >= counter;i--) {
		if (val0 & (1 << (i - counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = counter - 1;i;i--) {
		if (val1 & (1 << (i + 16 - counter))) val1 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	VMWrite16(addr, val0);
}
void SHLD_MwRwIb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
	u8 counter = cpu->ins.imm1.byte;
	u8 i;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHLD [%s:%s], %s, 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src], cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"SHLD [%s], %s, 0x%02X", cpu->rm_str, REG16[cpu->ins.src], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x0f;
	if (counter == 0) return;
	if (val0 & (1 << (16 - counter))) SETF(CF);
	else CLRF(CF);
	for (i = 15; i >= counter;i--) {
		if (val0 & (1 << (i - counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = counter - 1;i;i--) {
		if (val1 & (1 << (i + 16 - counter))) val1 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	VMWrite16(addr, val0);
}
void SHLD_RdRdCL(LPVCPU cpu){
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
	u8 counter = GETR8(CL);
	u8 i;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SHLD %s, %s, CL", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	if (counter == 0) return;
	if (val0 & (1 << (32 - counter))) SETF(CF);
	else CLRF(CF);
	for (i = 31; i >= counter;i--) {
		if (val0 & (1 << (i - counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = counter - 1;i;i--) {
		if (val1 & (1 << (i + 32 - counter))) val1 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	SETR32(cpu->ins.des, val0);
}
void SHLD_RdRdIb(LPVCPU cpu){
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
	u8 counter = cpu->ins.imm1.byte;
	u8 i;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SHLD %s, %s, 0x%02X", REG32[cpu->ins.des], REG32[cpu->ins.src],cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	if (counter == 0) return;
	if (val0 & (1 << (32 - counter))) SETF(CF);
	else CLRF(CF);
	for (i = 31; i >= counter;i--) {
		if (val0 & (1 << (i - counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = counter - 1;i;i--) {
		if (val1 & (1 << (i + 32 - counter))) val1 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	SETR32(cpu->ins.des, val0);
}
void SHLD_RwRwCL(LPVCPU cpu){
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
	u8 counter = GETR8(CL);
	u8 i;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SHLD %s, %s, CL", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x0f;
	if (counter == 0) return;
	if (val0 & (1 << (16 - counter))) SETF(CF);
	else CLRF(CF);
	for (i = 15; i >= counter;i--) {
		if (val0 & (1 << (i - counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = counter - 1;i;i--) {
		if (val1 & (1 << (i + 16 - counter))) val1 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	SETR16(cpu->ins.des, val0);
}
void SHLD_RwRwIb(LPVCPU cpu){
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
	u8 counter = cpu->ins.imm1.byte;
	u8 i;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SHLD %s, %s, 0x%02X", REG16[cpu->ins.des], REG16[cpu->ins.src], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x0f;
	if (counter == 0) return;
	if (val0 & (1 << (16 - counter))) SETF(CF);
	else CLRF(CF);
	for (i = 15; i >= counter;i--) {
		if (val0 & (1 << (i - counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = counter - 1;i;i--) {
		if (val1 & (1 << (i + 16 - counter))) val1 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	SETR16(cpu->ins.des, val0);
}
void SHRD_MdRdCL(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
	u8 counter = GETR8(CL);
	u8 size = 32;
	u8 i;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHRD [%s:%s], %s, CL", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"SHRD [%s], %s, CL", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	if (!counter) return;
	if (val0 & (1 << (counter - 1))) SETF(CF);
	else CLRF(CF);
	for (i = 0;i < size - 1 - counter;i++) {
		if (val0 & (1 << (i + counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = size - counter;i < size;i++) {
		if (val1 & (1 << (i + counter - size))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	VMWrite32(addr, val0);
}
void SHRD_MdRdIb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u32 val0 = VMRead32(addr);
	u32 val1 = GETR32(cpu->ins.src);
	u8 counter = cpu->ins.imm1.byte;
	u8 size = 32;
	u8 i;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHRD [%s:%s], %s, 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src], cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"SHRD [%s], %s, 0x%02X", cpu->rm_str, REG32[cpu->ins.src], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	if (!counter) return;
	if (val0 & (1 << (counter - 1))) SETF(CF);
	else CLRF(CF);
	for (i = 0;i < size - 1 - counter;i++) {
		if (val0 & (1 << (i + counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = size - counter;i < size;i++) {
		if (val1 & (1 << (i + counter - size))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	VMWrite32(addr, val0);
}
void SHRD_MwRwCL(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
	u8 counter = GETR8(CL);
	u8 size = 16;
	u8 i;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHRD [%s:%s], %s, CL", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"SHRD [%s], %s, CL", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	if (!counter) return;
	if (val0 & (1 << (counter - 1))) SETF(CF);
	else CLRF(CF);
	for (i = 0;i < size - 1 - counter;i++) {
		if (val0 & (1 << (i + counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = size - counter;i < size;i++) {
		if (val1 & (1 << (i + counter - size))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	VMWrite16(addr, val0);
}
void SHRD_MwRwIb(LPVCPU cpu){
	u32 addr = ModRM0(cpu);
	u16 val0 = VMRead16(addr);
	u16 val1 = GETR16(cpu->ins.src);
	u8 counter = cpu->ins.imm1.byte;
	u8 size = 16;
	u8 i;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SHRD [%s:%s], %s, 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src], cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"SHRD [%s], %s, 0x%02X", cpu->rm_str, REG16[cpu->ins.src], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	if (!counter) return;
	if (val0 & (1 << (counter - 1))) SETF(CF);
	else CLRF(CF);
	for (i = 0;i < size - 1 - counter;i++) {
		if (val0 & (1 << (i + counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = size - counter;i < size;i++) {
		if (val1 & (1 << (i + counter - size))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	VMWrite16(addr, val0);
}
void SHRD_RdRdCL(LPVCPU cpu){
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
	u8 counter = GETR8(CL);
	u8 size = 32;
	u8 i;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SHRD %s, %s, CL", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	if (!counter) return;
	if (val0 & (1 << (counter - 1))) SETF(CF);
	else CLRF(CF);
	for (i = 0;i < size - 1 - counter;i++) {
		if (val0 & (1 << (i + counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = size - counter;i < size;i++) {
		if (val1 & (1 << (i + counter - size))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	SETR32(cpu->ins.des, val0);
}
void SHRD_RdRdIb(LPVCPU cpu){
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = GETR32(cpu->ins.src);
	u8 counter = cpu->ins.imm1.byte;
	u8 size = 32;
	u8 i;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SHRD %s, %s, 0x%02X", REG32[cpu->ins.des], REG32[cpu->ins.src], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	if (!counter) return;
	if (val0 & (1 << (counter - 1))) SETF(CF);
	else CLRF(CF);
	for (i = 0;i < size - 1 - counter;i++) {
		if (val0 & (1 << (i + counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = size - counter;i < size;i++) {
		if (val1 & (1 << (i + counter - size))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	SETR32(cpu->ins.des, val0);
}
void SHRD_RwRwCL(LPVCPU cpu){
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
	u8 counter = GETR8(CL);
	u8 size = 16;
	u8 i;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SHRD %s, %s, CL", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	if (!counter) return;
	if (val0 & (1 << (counter - 1))) SETF(CF);
	else CLRF(CF);
	for (i = 0;i < size - 1 - counter;i++) {
		if (val0 & (1 << (i + counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = size - counter;i < size;i++) {
		if (val1 & (1 << (i + counter - size))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	SETR16(cpu->ins.des, val0);
}
void SHRD_RwRwIb(LPVCPU cpu){
	u16 val0 = GETR16(cpu->ins.des);
	u16 val1 = GETR16(cpu->ins.src);
	u8 counter = cpu->ins.imm1.byte;
	u8 size = 16;
	u8 i;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SHRD %s, %s, 0x%02X", REG16[cpu->ins.des], REG16[cpu->ins.src], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	counter &= 0x1f;
	if (!counter) return;
	if (val0 & (1 << (counter - 1))) SETF(CF);
	else CLRF(CF);
	for (i = 0;i < size - 1 - counter;i++) {
		if (val0 & (1 << (i + counter))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	for (i = size - counter;i < size;i++) {
		if (val1 & (1 << (i + counter - size))) val0 |= 1 << i;
		else val0 &= ~(1 << i);
	}
	SETR16(cpu->ins.des, val0);
}