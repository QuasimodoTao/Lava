/*
	vm/transd.c
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

void LDS_RdMPd(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if(cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"LDS %s, [%s:%s]",
		REG32[cpu->ins.des],SREG[cpu->ins.sreg],cpu->rm_str);
	else sprintf(cpu->ins_str,64,"LDS %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[DS] = VMRead16(addr + 4);
	cpu->base[DS] = cpu->sr[DS] << 4;
	SETR32(cpu->ins.des, VMRead32(addr));
}
void LDS_RwMPw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"LDS %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"LDS %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[DS] = VMRead16(addr + 2);
	cpu->base[DS] = cpu->sr[DS] << 4;
	SETR16(cpu->ins.des, VMRead16(addr));
}
void LEA_RdMu(LPVCPU cpu) {
	unsigned long addr = ModRM1(cpu);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LEA %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, addr);
}
void LEA_RwMu(LPVCPU cpu) {
	unsigned long addr = ModRM1(cpu);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LEA %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, (unsigned short)addr);
}
void LES_RdMPd(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"LES %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"LES %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[ES] = VMRead16(addr + 4);
	cpu->base[ES] = cpu->sr[ES] << 4;
	SETR32(cpu->ins.des, VMRead32(addr));
}
void LES_RwMPw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"LES %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"LES %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[ES] = VMRead16(addr + 2);
	cpu->base[ES] = cpu->sr[ES] << 4;
	SETR16(cpu->ins.des, VMRead16(addr));
}
void LFS_RdMPd(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"LFS %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"LFS %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[FS] = VMRead16(addr + 4);
	cpu->base[FS] = cpu->sr[FS] << 4;
	SETR32(cpu->ins.des, VMRead32(addr));
}
void LFS_RwMPw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"LFS %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"LFS %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[FS] = VMRead16(addr + 2);
	cpu->base[FS] = cpu->sr[FS] << 4;
	SETR16(cpu->ins.des, VMRead16(addr));
}
void LGS_RdMPd(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"LGS %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"LGS %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[GS] = VMRead16(addr + 4);
	cpu->base[GS] = cpu->sr[GS] << 4;
	SETR32(cpu->ins.des, VMRead32(addr));
}
void LGS_RwMPw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"LGS %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"LGS %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[GS] = VMRead16(addr + 2);
	cpu->base[GS] = cpu->sr[GS] << 4;
	SETR16(cpu->ins.des, VMRead16(addr));
}
void LODSB_No(LPVCPU cpu) {
	unsigned long saddr;
	unsigned short counter;
	unsigned char val0;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"REP %s:LODSB", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"REP LODSB");
	}
	else {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"%s:LODSB", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"LODSB");
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!(cpu->ins.muchine_flag & _SREGP)) saddr = cpu->base[DS] + GETR16(SI);
	else saddr = cpu->base[cpu->ins.sreg] + GETR16(SI);
	if (cpu->ins.muchine_flag & _REPEP) {
		counter = GETR16(CX);
		if (CHEF(DF)) { 
			val0 = VMRead8(saddr - counter);
			SETR16(SI, GETR16(SI) - counter);
		}
		else {
			val0 = VMRead8(saddr + counter - 1);
			SETR16(SI, GETR16(SI) + counter);
		}
	}
	else {
		if (CHEF(DF)) {
			val0 = VMRead8(saddr - 1);
			SETR16(SI, GETR16(SI) - 1);
		}
		else {
			val0 = VMRead8(saddr);
			SETR16(SI, GETR16(SI) + 1);
		}
	}
	SETR8(AL, val0);
}
void LODSD_No(LPVCPU cpu) {
	unsigned long saddr;
	unsigned short counter;
	unsigned long val0;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"REP %s:LODSD", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"REP LODSD");
	}
	else {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"%s:LODSD", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"LODSD");
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!(cpu->ins.muchine_flag & _SREGP)) saddr = cpu->base[DS] + GETR16(SI);
	else saddr = cpu->base[cpu->ins.sreg] + GETR16(SI);
	if (cpu->ins.muchine_flag & _REPEP) {
		counter = GETR16(CX);
		if (CHEF(DF)) {
			val0 = VMRead32(saddr - counter * 4);
			SETR16(SI, GETR16(SI) - counter * 4);
		}
		else {
			val0 = VMRead32(saddr + counter * 4 - 4);
			SETR16(SI, GETR16(SI) + counter * 4);
		}
	}
	else {
		if (CHEF(DF)) {
			val0 = VMRead32(saddr - 4);
			SETR16(SI, GETR16(SI) - 4);
		}
		else {
			val0 = VMRead32(saddr);
			SETR16(SI, GETR16(SI) + 4);
		}
	}
	SETR32(EAX, val0);
}
void LODSW_No(LPVCPU cpu) {
	unsigned long saddr;
	unsigned short counter;
	unsigned short val0;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"REP %s:LODSW", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"REP LODSW");
	}
	else {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"%s:LODSW", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"LODSW");
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!(cpu->ins.muchine_flag & _SREGP)) saddr = cpu->base[DS] + GETR16(SI);
	else saddr = cpu->base[cpu->ins.sreg] + GETR16(SI);
	if (cpu->ins.muchine_flag & _REPEP) {
		counter = GETR16(CX);
		if (CHEF(DF)) {
			val0 = VMRead16(saddr - counter * 2);
			SETR16(SI, GETR16(SI) - counter * 2);
		}
		else {
			val0 = VMRead16(saddr + counter * 2 - 2);
			SETR16(SI, GETR16(SI) + counter * 2);
		}
	}
	else {
		if (CHEF(DF)) {
			val0 = VMRead16(saddr - 2);
			SETR16(SI, GETR16(SI) - 2);
		}
		else {
			val0 = VMRead16(saddr);
			SETR16(SI, GETR16(SI) + 2);
		}
	}
	SETR16(AX, val0);
}
void LSS_RdMPd(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"LSS %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"LSS %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[SS] = VMRead16(addr + 4);
	cpu->base[SS] = cpu->sr[SS] << 4;
	SETR32(cpu->ins.des, VMRead32(addr));
}
void LSS_RwMPw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"LSS %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"LSS %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[SS] = VMRead16(addr + 2);
	cpu->base[SS] = cpu->sr[SS] << 4;
	SETR16(cpu->ins.des, VMRead16(addr));
}
void MOVSB_No(LPVCPU cpu) {
	unsigned char val0;
	unsigned short si;
	unsigned short di;
	unsigned long sbase;
	unsigned long dbase;
	unsigned short cx;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"REP %s:MOVSB", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"REP MOVSB");
	}
	else {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"%s:MOVSB", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"MOVSB");
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _SREGP) sbase = cpu->base[cpu->ins.sreg];
	else sbase = cpu->base[DS];
	dbase = cpu->base[ES];
	si = GETR16(SI);
	di = GETR16(DI);
	cx = GETR16(CX);
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;cx;cx--) {
			val0 = VMRead8(si + sbase);
			VMWrite8(di + dbase, val0);
			if (CHEF(DF)) {
				si--;
				di--;
			}
			else {
				si++;
				di++;
			}
		}
	}
	else {
		val0 = VMRead8(si + sbase);
		VMWrite8(di + dbase, val0);
		if (CHEF(DF)) {
			si--;
			di--;
		}
		else {
			si++;
			di++;
		}
	}
	SETR16(SI, si);
	SETR16(DI, di);
	SETR16(CX, cx);
}
void MOVSD_No(LPVCPU cpu) {
	unsigned long val0;
	unsigned short si;
	unsigned short di;
	unsigned long sbase;
	unsigned long dbase;
	unsigned short cx;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"REP %s:MOVSD", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"REP MOVSD");
	}
	else {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"%s:MOVSD", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"MOVSD");
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _SREGP) sbase = cpu->base[cpu->ins.sreg];
	else sbase = cpu->base[DS];
	dbase = cpu->base[ES];
	si = GETR16(SI);
	di = GETR16(DI);
	cx = GETR16(CX);
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;cx;cx--) {
			val0 = VMRead32(si + sbase);
			VMWrite32(di + dbase, val0);
			if (CHEF(DF)) {
				si-=4;
				di-=4;
			}
			else {
				si+=4;
				di+=4;
			}
		}
	}
	else {
		val0 = VMRead32(si + sbase);
		VMWrite32(di + dbase, val0);
		if (CHEF(DF)) {
			si-=4;
			di-=4;
		}
		else {
			si+=4;
			di+=4;
		}
	}
	SETR16(SI, si);
	SETR16(DI, di);
	SETR16(CX, cx);
}
void MOVSW_No(LPVCPU cpu) {
	unsigned short val0;
	unsigned short si;
	unsigned short di;
	unsigned long sbase;
	unsigned long dbase;
	unsigned short cx;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"REP %s:MOVSW", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"REP MOVSW");
	}
	else {
		if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"%s:MOVSW", SREG[cpu->ins.sreg]);
		else sprintf(cpu->ins_str,64,"MOVSW");
	}
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _SREGP) sbase = cpu->base[cpu->ins.sreg];
	else sbase = cpu->base[DS];
	dbase = cpu->base[ES];
	si = GETR16(SI);
	di = GETR16(DI);
	cx = GETR16(CX);
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;cx;cx--) {
			val0 = VMRead16(si + sbase);
			VMWrite16(di + dbase, val0);
			if (CHEF(DF)) {
				si -= 2;
				di -= 2;
			}
			else {
				si += 2;
				di += 2;
			}
		}
	}
	else {
		val0 = VMRead16(si + sbase);
		VMWrite16(di + dbase, val0);
		if (CHEF(DF)) {
			si -= 2;
			di -= 2;
		}
		else {
			si += 2;
			di += 2;
		}
	}
	SETR16(SI, si);
	SETR16(DI, di);
	SETR16(CX, cx);
}
void MOVSX_RdMb(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	signed char val0 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOVSX %s, BYTE [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MOVSX %s, BYTE [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, (signed long)val0);
}
void MOVSX_RdMw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	signed short val0 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOVSX %s, WORD [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MOVSX %s, WORD [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, (signed long)val0);
}
void MOVSX_RdRb(LPVCPU cpu) {
	signed char val0 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOVSX %s, %s", REG32[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, (signed long)val0);
}
void MOVSX_RdRw(LPVCPU cpu) {
	signed short val0 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOVSX %s, %s", REG32[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, (signed long)val0);
}
void MOVSX_RwMb(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	signed char val0 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOVSX %s, BYTE [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MOVSX %s, BYTE [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, (signed short)val0);
}
void MOVSX_RwRb(LPVCPU cpu) {
	signed char val0 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOVSX %s, %s", REG16[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, (signed short)val0);
}
void MOVZX_RdMb(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned char val0 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOVZX %s, BYTE [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MOVZX %s, BYTE [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, (unsigned long)val0);
}
void MOVZX_RdMw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned short val0 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOVZX %s, WORD [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MOVZX %s, WORD [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, (unsigned long)val0);
}
void MOVZX_RdRb(LPVCPU cpu) {
	unsigned char val0 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOVZX %s, %s", REG32[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, (unsigned long)val0);
}
void MOVZX_RdRw(LPVCPU cpu) {
	unsigned short val0 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOVZX %s, %s", REG32[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, (unsigned long)val0);
}
void MOVZX_RwMb(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned char val0 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOVZX %s, BYTE [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MOVZX %s, BYTE [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, (unsigned short)val0);
}
void MOVZX_RwRb(LPVCPU cpu) {
	unsigned char val0 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOVZX %s, %s", REG16[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, (unsigned short)val0);
}
void MOV_MbIb(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV BYTE [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"MOV BYTE [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, cpu->ins.imm1.byte);
}
void MOV_MbRb(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"MOV [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, GETR8(cpu->ins.src));
}
void MOV_MdId(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV DWORD [%s:%s], 0x%08X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.dword);
	else sprintf(cpu->ins_str,64,"MOV DWORD [%s], 0x%08X", cpu->rm_str, cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, cpu->ins.imm1.dword);
}
void MOV_MdRd(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"MOV [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, GETR32(cpu->ins.src));
}
void MOV_MwIw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV WORD [%s:%s], 0x%04X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.word);
	else sprintf(cpu->ins_str,64,"MOV WORD [%s], 0x%04X", cpu->rm_str, cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, cpu->ins.imm1.word);
}
void MOV_MwRw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"MOV [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, GETR16(cpu->ins.src));
}
void MOV_MwSw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, SREG[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"MOV [%s], %s", cpu->rm_str, SREG[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, cpu->sr[cpu->ins.src]);
}
void MOV_RbIb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV %s, 0x%02X", REG8[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, cpu->ins.imm1.byte);
}
void MOV_RbMb(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned char val0 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV %s,[%s:%s]", REG8[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MOV %s, [%s]", REG8[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0);
}
void MOV_RbRb(LPVCPU cpu) {
	unsigned char val0 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV %s, %s", REG8[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.des, val0);
}
void MOV_RdId(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV %s, 0x%08X", REG32[cpu->ins.des], cpu->ins.imm1.dword);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, cpu->ins.imm1.dword);
}
void MOV_RdMd(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned long val0 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV %s,[%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MOV %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0);
}
void MOV_RdRd(LPVCPU cpu) {
	unsigned long val0 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.des, val0);
}
void MOV_RwIw(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV %s, 0x%04X", REG16[cpu->ins.des], cpu->ins.imm1.word);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, cpu->ins.imm1.word);
}
void MOV_RwMw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned short val0 = VMRead16(addr);
#ifdef VM_DEBUG
	//stop();
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV %s,[%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MOV %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0);
}
void MOV_RwRw(LPVCPU cpu) {
	unsigned short val0 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0);
}
void MOV_RwSw(LPVCPU cpu) {
	unsigned short val0 = cpu->sr[cpu->ins.src];
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV %s, %s", REG16[cpu->ins.des], SREG[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.des, val0);
}
void MOV_SwMw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned short val0 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV %s, [%s:%s]", SREG[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"MOV %s, [%s]", SREG[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[cpu->ins.des] = val0;
	cpu->base[cpu->ins.des] = val0 << 4;
}
void MOV_SwRw(LPVCPU cpu) {
	unsigned short val0 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"MOV %s, %s", SREG[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->sr[cpu->ins.des] = val0;
	cpu->base[cpu->ins.des] = val0 << 4;
}
void XCHG_MbRb(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned char val0 = VMRead8(addr);
	unsigned char val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XCHG [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG8[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"XCHG [%s], %s", cpu->rm_str, REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.src, val0);
	VMWrite8(addr, val1);
}
void XCHG_MdRd(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned long val0 = VMRead32(addr);
	unsigned long val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XCHG [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"XCHG [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.src, val0);
	VMWrite32(addr, val1);
}
void XCHG_MwRw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned short val0 = VMRead16(addr);
	unsigned short val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XCHG [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"XCHG [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.src, val0);
	VMWrite16(addr, val1);
}
void XCHG_RbMb(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned char val0 = GETR8(cpu->ins.src);
	unsigned char val1 = VMRead8(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XCHG %s, [%s:%s]", REG8[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"XCHG %s, [%s]", REG8[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, val0);
	SETR8(cpu->ins.src, val1);
}
void XCHG_RbRb(LPVCPU cpu) {
	unsigned char val0 = GETR8(cpu->ins.des);
	unsigned char val1 = GETR8(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XCHG %s, %s", REG8[cpu->ins.des], REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(cpu->ins.src, val0);
	SETR8(cpu->ins.des, val1);
}
void XCHG_RdMd(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned long val0 = GETR32(cpu->ins.src);
	unsigned long val1 = VMRead32(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XCHG %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"XCHG %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, val0);
	SETR32(cpu->ins.src, val1);
}
void XCHG_RdRd(LPVCPU cpu) {
	unsigned long val0 = GETR32(cpu->ins.des);
	unsigned long val1 = GETR32(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XCHG %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(cpu->ins.src, val0);
	SETR32(cpu->ins.des, val1);
}
void XCHG_RwMw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
	unsigned short val0 = GETR16(cpu->ins.src);
	unsigned short val1 = VMRead16(addr);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"XCHG %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"XCHG %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, val0);
	SETR16(cpu->ins.src, val1);
}
void XCHG_RwRw(LPVCPU cpu) {
	unsigned short val0 = GETR16(cpu->ins.des);
	unsigned short val1 = GETR16(cpu->ins.src);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"XCHG %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(cpu->ins.src, val0);
	SETR16(cpu->ins.des, val1);
}
void XLATB_No(LPVCPU cpu) {
	unsigned long addr;
	unsigned char val0;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"%s:XLATB", SREG[cpu->ins.sreg]);
	else sprintf(cpu->ins_str,64,"XLATB");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _SREGP) addr = cpu->base[cpu->ins.sreg] + GETR16(BX);
	else addr = cpu->base[DS] + GETR16(BX);
	val0 = VMRead8(addr + GETR8(AL));
	SETR8(AL, val0);
}
void STOSB_No(LPVCPU cpu) {
	unsigned short di = GETR16(DI);
	unsigned short cx = GETR16(CX);
	unsigned long dbase = cpu->base[ES];
	unsigned char val0 = GETR8(AL);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REP STOSB");
	else sprintf(cpu->ins_str,64,"STOSB");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;cx;cx--) {
			VMWrite8(dbase + di, val0);
			if (CHEF(DF)) di--;
			else di++;
		}
	}
	else {
		VMWrite8(dbase + di, val0);
		if (CHEF(DF)) di--;
		else di++;
	}
	SETR16(CX, cx);
	SETR16(DI, di);
}
void STOSD_No(LPVCPU cpu) {
	unsigned short di = GETR16(DI);
	unsigned short cx = GETR16(CX);
	unsigned long dbase = cpu->base[ES];
	unsigned long val0 = GETR32(EAX);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REP STOSD");
	else sprintf(cpu->ins_str,64,"STOSD");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;cx;cx--) {
			VMWrite32(dbase + di, val0);
			if (CHEF(DF)) di-=4;
			else di+=4;
		}
	}
	else {
		VMWrite32(dbase + di, val0);
		if (CHEF(DF)) di-=4;
		else di+=4;
	}
	SETR16(CX, cx);
	SETR16(DI, di);
}
void STOSW_No(LPVCPU cpu) {
	unsigned short di = GETR16(DI);
	unsigned short cx = GETR16(CX);
	unsigned long dbase = cpu->base[ES];
	unsigned short val0 = GETR16(AX);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _REPEP) sprintf(cpu->ins_str,64,"REP STOSW");
	else sprintf(cpu->ins_str,64,"STOSW");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.muchine_flag & _REPEP) {
		for (;cx;cx--) {
			VMWrite16(dbase + di, val0);
			if (CHEF(DF)) di -= 2;
			else di += 2;
		}
	}
	else {
		VMWrite16(dbase + di, val0);
		if (CHEF(DF)) di -= 2;
		else di += 2;
	}
	SETR16(CX, cx);
	SETR16(DI, di);
}
void MOV_AbMb(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV AL, [%s:0x%04X]", SREG[cpu->ins.sreg], cpu->ins.addr);
	else sprintf(cpu->ins_str,64,"MOV AL, [0x%04X]", cpu->ins.addr);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AL, VMRead8(addr));
}
void MOV_AwMw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV AX, [%s:0x%04X]", SREG[cpu->ins.sreg], cpu->ins.addr);
	else sprintf(cpu->ins_str,64,"MOV AX, [0x%04X]", cpu->ins.addr);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR16(AX, VMRead16(addr));

}
void MOV_AdMd(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV EAX, [%s:0x%04X]", SREG[cpu->ins.sreg], cpu->ins.addr);
	else sprintf(cpu->ins_str,64,"MOV EAX, [0x%04X]", cpu->ins.addr);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR32(EAX, VMRead32(addr));
}
void MOV_MbAb(LPVCPU cpu){
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV [%s:0x%04X], AL", SREG[cpu->ins.sreg], cpu->ins.addr);
	else sprintf(cpu->ins_str,64,"MOV [0x%04X], AL", cpu->ins.addr);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite8(addr, GETR8(AL));
}
void MOV_MwAw(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV [%s:0x%04X], AX", SREG[cpu->ins.sreg], cpu->ins.addr);
	else sprintf(cpu->ins_str,64,"MOV [0x%04X], AX", cpu->ins.addr);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite16(addr, GETR16(AX));
}
void MOV_MdAd(LPVCPU cpu) {
	unsigned long addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"MOV [%s:0x%04X], EAX", SREG[cpu->ins.sreg], cpu->ins.addr);
	else sprintf(cpu->ins_str,64,"MOV [0x%04X], EAX", cpu->ins.addr);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	VMWrite32(addr, GETR32(EAX));
}