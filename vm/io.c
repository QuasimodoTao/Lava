/*
	vm/io.c
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
#include <asm.h>
#include <stdio.h>
#include <stddef.h>

void INSB_No(LPVCPU cpu) {
	u16 di = GETR16(DI);
	u16 cx = GETR16(CX);
	u32 dbase = cpu->base[ES];
	u16 port = GETR16(DX);
	unsigned char val0;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REP INSB");
	else sprintf(cpu->ins_str,64,"INSB");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;cx;cx--) {
			val0 = inb(port);
			VMWrite8(dbase + di, val0);
			if (CHEF(DF)) di -= 1;
			else di += 1;
		}
	}
	else {
		val0 = inb(port);
		VMWrite8(dbase + di, val0);
		if (CHEF(DF)) di -= 1;
		else di += 1;
	}
	SETR16(CX, cx);
	SETR16(DI, di);
}
void INSD_No(LPVCPU cpu) {
	u16 di = GETR16(DI);
	u16 cx = GETR16(CX);
	u32 dbase = cpu->base[ES];
	u16 port = GETR16(DX);
	u32 val0;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REP INSD");
	else sprintf(cpu->ins_str,64,"INSD");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;cx;cx--) {
			val0 = ind(port);
			VMWrite32(dbase + di, val0);
			if (CHEF(DF)) di -= 4;
			else di += 4;
		}
	}
	else {
		val0 = ind(port);
		VMWrite32(dbase + di, val0);
		if (CHEF(DF)) di -= 4;
		else di += 4;
	}
	SETR16 (CX, cx);
	SETR16 (DI, di);
}
void INSW_No(LPVCPU cpu) {
	u16 di = GETR16(DI);
	u16 cx = GETR16(CX);
	u32 dbase = cpu->base[ES];
	u16 port = GETR16(DX);
	u16 val0;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REP INSW");
	else sprintf(cpu->ins_str,64,"INSW");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;cx;cx--) {
			val0 = inw(port);
			VMWrite16(dbase + di, val0);
			if (CHEF(DF)) di -= 2;
			else di += 2;
		}
	}
	else {
		val0 = inw(port);
		VMWrite16(dbase + di, val0);
		if (CHEF(DF)) di -= 2;
		else di += 2;
	}
	SETR16(CX, cx);
	SETR16(DI, di);
}
void IN_AbDw(LPVCPU cpu) {
	u16 port = GETR16(DX);
	unsigned char val0 = inb(port);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IN AL, DX");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AL, val0);
}
void IN_AbIb(LPVCPU cpu) {
	u16 port = cpu->ins.imm1.byte;
	unsigned char val0 = inb(port);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IN AL, 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AL, val0);
}
void IN_AdDw(LPVCPU cpu) {
	u16 port = GETR16(DX);
	u32 val0 = ind(port);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IN EAX, DX");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, val0);
}
void IN_AdIb(LPVCPU cpu) {
	u16 port = cpu->ins.imm1.byte;
	u32 val0 = ind(port);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IN EAX, 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, val0);
}
void IN_AwDw(LPVCPU cpu) {
	u16 port = GETR16(DX);
	u16 val0 = inw(port);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IN AX, DX");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, val0);
}
void IN_AwIb(LPVCPU cpu) {
	u16 port = cpu->ins.imm1.byte;
	u16 val0 = inw(port);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"IN AX, 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, val0);
}
void OUTSB_No(LPVCPU cpu) {
	u32 sbase;
	u16 si = GETR16(SI);
	u16 cx = GETR16(CX);
	u16 port = GETR16(DX);
	unsigned char val0;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) {
		if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REP %s:OUTSB",SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"%s:OUTSB",SREG[cpu->ins.sreg]);
	}
	else {
		if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REP OUTSB");
		else sprintf(cpu->ins_str,64,"OUTSB");
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _SREGP) sbase = cpu->base[cpu->ins.sreg];
	else sbase = cpu->base[DS];
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;cx;cx--) {
			val0 = VMRead8(sbase + si);
			outb(port, val0);
			if (CHEF(DF)) si -= 1;
			else si += 1;
		}
	}
	else {
		val0 = VMRead8(sbase + si);
		outb(port, val0);
		if (CHEF(DF)) si -= 1;
		else si += 1;
	}
	SETR16(CX, cx);
	SETR16(SI, si);
}
void OUTSD_No(LPVCPU cpu) {
	u32 sbase;
	u16 si = GETR16(SI);
	u16 cx = GETR16(CX);
	u16 port = GETR16(DX);
	u32 val0;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) {
		if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REP %s:OUTSD", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"%s:OUTSD", SREG[cpu->ins.sreg]);
	}
	else {
		if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REP OUTSD");
		else sprintf(cpu->ins_str,64,"OUTSD");
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _SREGP) sbase = cpu->base[cpu->ins.sreg];
	else sbase = cpu->base[DS];
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;cx;cx--) {
			val0 = VMRead32(sbase + si);
			outd(port, val0);
			if (CHEF(DF)) si -= 4;
			else si += 4;
		}
	}
	else {
		val0 = VMRead32(sbase + si);
		outd(port, val0);
		if (CHEF(DF)) si -= 4;
		else si += 4;
	}
	SETR16(CX, cx);
	SETR16(SI, si);
}
void OUTSW_No(LPVCPU cpu) {
	u32 sbase;
	u16 si = GETR16(SI);
	u16 cx = GETR16(CX);
	u16 port = GETR16(DX);
	u16 val0;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) {
		if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REP %s:OUTSW", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"%s:OUTSW", SREG[cpu->ins.sreg]);
	}
	else {
		if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REP OUTSW");
		else sprintf(cpu->ins_str,64,"OUTSW");
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _SREGP) sbase = cpu->base[cpu->ins.sreg];
	else sbase = cpu->base[DS];
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;cx;cx--) {
			val0 = VMRead16(sbase + si);
			outw(port, val0);
			if (CHEF(DF)) si -= 2;
			else si += 2;
		}
	}
	else {
		val0 = VMRead16(sbase + si);
		outw(port, val0);
		if (CHEF(DF)) si -= 2;
		else si += 2;
	}
	SETR16(CX, cx);
	SETR16(SI, si);
}
void OUT_DwAb(LPVCPU cpu) {
	u16 port = GETR16(DX);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OUT DX, AL");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	outb(port, GETR8(AL));
}
void OUT_DwAd(LPVCPU cpu) {
	u16 port = GETR16(DX);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OUT DX, EAX");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	outd(port, GETR32(EAX));
}
void OUT_DwAw(LPVCPU cpu) {
	u16 port = GETR16(DX);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OUT DX, AX");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	outw(port, GETR16(AX));
}
void OUT_IbAb(LPVCPU cpu) {
	u16 port = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OUT 0x%02X, AL", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	outb(port, GETR8(AL));
}
void OUT_IbAd(LPVCPU cpu) {
	u16 port = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OUT 0x%02X, EAX", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	outd(port, GETR32(EAX));
}
void OUT_IbAw(LPVCPU cpu) {
	u16 port = cpu->ins.imm1.byte;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"OUT 0x%02X, AX", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	outw(port, GETR16(AX));
}
