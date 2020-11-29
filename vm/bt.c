//Lava OS
//PageFault
//20-04-11-15-08

#include <vm.h>
#include <config.h>
#include <stdio.h>
#include <stddef.h>
void BSF_RwMw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val1 = VMRead16(addr);
	u8 tmp = 0;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BSF %s, [%s:%s]",
		REG16[cpu->ins.des],SREG[cpu->ins.sreg],cpu->rm_str);
	else sprintf(cpu->ins_str,64,"BSF %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!val1) {
		SETF(ZF);
		return;
	}
	CLRF(ZF);
	while (~(val1 & (1 << tmp))) tmp++;
	SETR16(cpu->ins.des, tmp);
}
void BSF_RwRw(LPVCPU cpu) {
	u16 val1 = GETR16(cpu->ins.src);
	u8 tmp = 0;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BSF %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!val1) {
		SETF(ZF);
		return;
	}
	CLRF(ZF);
	while (~(val1 & (1 << tmp))) tmp++;
	SETR16(cpu->ins.des, tmp);
}
void BSR_RwMw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u16 val1 = VMRead16(addr);
	u8 tmp = 15;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BSR %s, [%s:%s]", REG16[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"BSR %s, [%s]", REG16[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!val1) {
		SETF(ZF);
		return;
	}
	CLRF(ZF);
	while (~(val1 & (1 << tmp))) tmp--;
	SETR16(cpu->ins.des, tmp);
}
void BSR_RwRw(LPVCPU cpu) {
	u16 val1 = GETR16(cpu->ins.src);
	u8 tmp = 15;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BSR %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!val1) {
		SETF(ZF);
		return;
	}
	CLRF(ZF);
	while (~(val1 & (1 << tmp))) tmp--;
	SETR16(cpu->ins.des, tmp);
}
void BSF_RdMd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val1 = VMRead32(addr);
	u8 tmp = 0;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BSF %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"BSF %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!val1) {
		SETF(ZF);
		return;
	}
	CLRF(ZF);
	while (~(val1 & (1 << tmp))) tmp++;
	SETR32(cpu->ins.des, tmp);
}
void BSF_RdRd(LPVCPU cpu) {
	u32 val1 = GETR32(cpu->ins.src);
	u8 tmp = 0;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BSF %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!val1) {
		SETF(ZF);
		return;
	}
	CLRF(ZF);
	while (~(val1 & (1 << tmp))) tmp++;
	SETR32(cpu->ins.des, tmp);
}
void BSR_RdMd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u32 val1 = VMRead32(addr);
	u8 tmp = 31;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BSR %s, [%s:%s]", REG32[cpu->ins.des], SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"BSR %s, [%s]", REG32[cpu->ins.des], cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!val1) {
		SETF(ZF);
		return;
	}
	CLRF(ZF);
	while (~(val1 & (1 << tmp))) tmp--;
	SETR32(cpu->ins.des, tmp);
}
void BSR_RdRd(LPVCPU cpu) {
	u32 val1 = GETR32(cpu->ins.src);
	u8 tmp = 31;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BSR %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!val1) {
		SETF(ZF);
		return;
	}
	CLRF(ZF);
	while (~(val1 & (1 << tmp))) tmp--;
	SETR32(cpu->ins.des, tmp);
}
void BSWAP_Rd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u32 val1 = 0;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BSWAP %s", REG32[cpu->ins.des]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	val1 |= (val0 & 0x000000ff) << 24;
	val1 |= (val0 & 0x0000ff00) << 8;
	val1 |= (val0 & 0x00ff0000) >> 8;
	val1 |= (val0 & 0xff000000) >> 24;
	SETR32(cpu->ins.des, val1);
}
void BTC_MdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u8 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BTC [%s:%s], 0x%02X",SREG[cpu->ins.sreg],cpu->rm_str,cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"BTC [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) {
		ByteOff = (-((s8)cpu->ins.imm1.byte)) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)cpu->ins.imm1.byte)) & 0x07);
		val0 = VMRead8(addr - ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 ^= 1 << BitOff;
		VMWrite8(addr - ByteOff, val0);
	}
	else {
		ByteOff = cpu->ins.imm1.byte >> 3;
		BitOff = cpu->ins.imm1.byte & 0x07;
		val0 = VMRead8(addr + ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 ^= 1 << BitOff;
		VMWrite8(addr + ByteOff, val0);
	}
}
void BTC_MdRd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u32 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BTC [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"BTC [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR32(cpu->ins.src) & 0x80000000) {
		ByteOff = (-((s8)GETR32(cpu->ins.src))) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)GETR32(cpu->ins.src))) & 0x07);
		val0 = VMRead8(addr - ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 ^= 1 << BitOff;
		VMWrite8(addr - ByteOff, val0);
	}
	else {
		ByteOff = GETR32(cpu->ins.src) >> 3;
		BitOff = GETR32(cpu->ins.src) & 0x07;
		val0 = VMRead8(addr + ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 ^= 1 << BitOff;
		VMWrite8(addr + ByteOff, val0);
	}
}
void BTC_MwIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u8 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BTC [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"BTC [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) {
		ByteOff = (-((s8)cpu->ins.imm1.byte)) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)cpu->ins.imm1.byte)) & 0x07);
		val0 = VMRead8(addr - ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 ^= 1 << BitOff;
		VMWrite8(addr - ByteOff, val0);
	}
	else {
		ByteOff = cpu->ins.imm1.byte >> 3;
		BitOff = cpu->ins.imm1.byte & 0x07;
		val0 = VMRead8(addr + ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 ^= 1 << BitOff;
		VMWrite8(addr + ByteOff, val0);
	}
}
void BTC_MwRw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u32 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BTC [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"BTC [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR16(cpu->ins.src) & 0x80000000) {
		ByteOff = (-((s8)GETR16(cpu->ins.src))) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)GETR16(cpu->ins.src))) & 0x07);
		val0 = VMRead8(addr - ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 ^= 1 << BitOff;
		VMWrite8(addr - ByteOff, val0);
	}
	else {
		ByteOff = GETR16(cpu->ins.src) >> 3;
		BitOff = GETR16(cpu->ins.src) & 0x07;
		val0 = VMRead8(addr + ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 ^= 1 << BitOff;
		VMWrite8(addr + ByteOff, val0);
	}
}
void BTC_RdIb(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BTC %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) BitOff = 32 - ((-((s8)cpu->ins.imm1.byte)) & 0x1f);
	else BitOff = cpu->ins.imm1.byte & 0x1f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
	val0 ^= 1 << BitOff;
	SETR32(cpu->ins.des, val0);
}
void BTC_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BTC %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR32(cpu->ins.src) & 0x80000000) BitOff = 32 - ((-((s32)GETR32(cpu->ins.src))) & 0x1f);
	else BitOff = GETR32(cpu->ins.src) & 0x1f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
	val0 ^= 1 << BitOff;
	SETR32(cpu->ins.des, val0);
}
void BTC_RwIb(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BTC %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) BitOff = 16 - ((-((s8)cpu->ins.imm1.byte)) & 0x0f);
	else BitOff = cpu->ins.imm1.byte & 0x0f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
	val0 ^= 1 << BitOff;
	SETR16(cpu->ins.des, val0);
}
void BTC_RwRw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BTC %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR16(cpu->ins.src) & 0x80000000) BitOff = 16 - ((-((s32)GETR16(cpu->ins.src))) & 0x0f);
	else BitOff = GETR16(cpu->ins.src) & 0x0f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
	val0 ^= 1 << BitOff;
	SETR16(cpu->ins.des, val0);
}
void BTR_MdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u8 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BTR [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"BTR [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) {
		ByteOff = (-((s8)cpu->ins.imm1.byte)) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)cpu->ins.imm1.byte)) & 0x07);
		val0 = VMRead8(addr - ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 &= ~(1 << BitOff);
		VMWrite8(addr - ByteOff, val0);
	}
	else {
		ByteOff = cpu->ins.imm1.byte >> 3;
		BitOff = cpu->ins.imm1.byte & 0x07;
		val0 = VMRead8(addr + ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 &= ~(1 << BitOff);
		VMWrite8(addr + ByteOff, val0);
	}
}
void BTR_MdRd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u32 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BTR [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"BTR [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR32(cpu->ins.src) & 0x80000000) {
		ByteOff = (-((s8)GETR32(cpu->ins.src))) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)GETR32(cpu->ins.src))) & 0x07);
		val0 = VMRead8(addr - ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 &= ~(1 << BitOff);
		VMWrite8(addr - ByteOff, val0);
	}
	else {
		ByteOff = GETR32(cpu->ins.src) >> 3;
		BitOff = GETR32(cpu->ins.src) & 0x07;
		val0 = VMRead8(addr + ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 &= ~(1 << BitOff);
		VMWrite8(addr + ByteOff, val0);
	}
}
void BTR_MwIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u8 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BTC [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"BTR [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) {
		ByteOff = (-((s8)cpu->ins.imm1.byte)) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)cpu->ins.imm1.byte)) & 0x07);
		val0 = VMRead8(addr - ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 &= ~(1 << BitOff);
		VMWrite8(addr - ByteOff, val0);
	}
	else {
		ByteOff = cpu->ins.imm1.byte >> 3;
		BitOff = cpu->ins.imm1.byte & 0x07;
		val0 = VMRead8(addr + ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 &= ~(1 << BitOff);
		VMWrite8(addr + ByteOff, val0);
	}
}
void BTR_MwRw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u32 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BTR [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"BTR [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR16(cpu->ins.src) & 0x80000000) {
		ByteOff = (-((s8)GETR16(cpu->ins.src))) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)GETR16(cpu->ins.src))) & 0x07);
		val0 = VMRead8(addr - ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 &= ~(1 << BitOff);
		VMWrite8(addr - ByteOff, val0);
	}
	else {
		ByteOff = GETR16(cpu->ins.src) >> 3;
		BitOff = GETR16(cpu->ins.src) & 0x07;
		val0 = VMRead8(addr + ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 &= ~(1 << BitOff);
		VMWrite8(addr + ByteOff, val0);
	}
}
void BTR_RdIb(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BTR %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) BitOff = 32 - ((-((s8)cpu->ins.imm1.byte)) & 0x1f);
	else BitOff = cpu->ins.imm1.byte & 0x1f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
	val0 &= ~(1 << BitOff);
	SETR32(cpu->ins.des, val0);
}
void BTR_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BTR %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR32(cpu->ins.src) & 0x80000000) BitOff = 32 - ((-((s32)GETR32(cpu->ins.src))) & 0x1f);
	else BitOff = GETR32(cpu->ins.src) & 0x1f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
	val0 &= ~(1 << BitOff);
	SETR32(cpu->ins.des, val0);
}
void BTR_RwIb(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BTR %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) BitOff = 16 - ((-((s8)cpu->ins.imm1.byte)) & 0x0f);
	else BitOff = cpu->ins.imm1.byte & 0x0f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
	val0 &= ~(1 << BitOff);
	SETR16(cpu->ins.des, val0);
}
void BTR_RwRw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BTR %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR16(cpu->ins.src) & 0x80000000) BitOff = 16 - ((-((s32)GETR16(cpu->ins.src))) & 0x0f);
	else BitOff = GETR16(cpu->ins.src) & 0x0f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
	val0 &= ~(1 << BitOff);
	SETR16(cpu->ins.des, val0);
}
void BTS_MdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u8 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BTS [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"BTS [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) {
		ByteOff = (-((s8)cpu->ins.imm1.byte)) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)cpu->ins.imm1.byte)) & 0x07);
		val0 = VMRead8(addr - ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 |= 1 << BitOff;
		VMWrite8(addr - ByteOff, val0);
	}
	else {
		ByteOff = cpu->ins.imm1.byte >> 3;
		BitOff = cpu->ins.imm1.byte & 0x07;
		val0 = VMRead8(addr + ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 |= 1 << BitOff;
		VMWrite8(addr + ByteOff, val0);
	}
}
void BTS_MdRd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u32 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BTS [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"BTS [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR32(cpu->ins.src) & 0x80000000) {
		ByteOff = (-((s8)GETR32(cpu->ins.src))) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)GETR32(cpu->ins.src))) & 0x07);
		val0 = VMRead8(addr - ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 |= 1 << BitOff;
		VMWrite8(addr - ByteOff, val0);
	}
	else {
		ByteOff = GETR32(cpu->ins.src) >> 3;
		BitOff = GETR32(cpu->ins.src) & 0x07;
		val0 = VMRead8(addr + ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 |= 1 << BitOff;
		VMWrite8(addr + ByteOff, val0);
	}
}
void BTS_MwIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u8 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BTC [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"BTS [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) {
		ByteOff = (-((s8)cpu->ins.imm1.byte)) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)cpu->ins.imm1.byte)) & 0x07);
		val0 = VMRead8(addr - ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 |= 1 << BitOff;
		VMWrite8(addr - ByteOff, val0);
	}
	else {
		ByteOff = cpu->ins.imm1.byte >> 3;
		BitOff = cpu->ins.imm1.byte & 0x07;
		val0 = VMRead8(addr + ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 |= 1 << BitOff;
		VMWrite8(addr + ByteOff, val0);
	}
}
void BTS_MwRw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u32 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BTS [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"BTS [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR16(cpu->ins.src) & 0x80000000) {
		ByteOff = (-((s8)GETR16(cpu->ins.src))) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)GETR16(cpu->ins.src))) & 0x07);
		val0 = VMRead8(addr - ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 |= 1 << BitOff;
		VMWrite8(addr - ByteOff, val0);
	}
	else {
		ByteOff = GETR16(cpu->ins.src) >> 3;
		BitOff = GETR16(cpu->ins.src) & 0x07;
		val0 = VMRead8(addr + ByteOff);
		if (val0 & (1 << BitOff)) SETF(CF);
		else CLRF(CF);
		val0 |= 1 << BitOff;
		VMWrite8(addr + ByteOff, val0);
	}
}
void BTS_RdIb(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BTS %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) BitOff = 32 - ((-((s8)cpu->ins.imm1.byte)) & 0x1f);
	else BitOff = cpu->ins.imm1.byte & 0x1f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
	val0 |= 1 << BitOff;
	SETR32(cpu->ins.des, val0);
}
void BTS_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BTS %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR32(cpu->ins.src) & 0x80000000) BitOff = 32 - ((-((s32)GETR32(cpu->ins.src))) & 0x1f);
	else BitOff = GETR32(cpu->ins.src) & 0x1f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
	val0 |= 1 << BitOff;
	SETR32(cpu->ins.des, val0);
}
void BTS_RwIb(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BTS %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) BitOff = 16 - ((-((s8)cpu->ins.imm1.byte)) & 0x0f);
	else BitOff = cpu->ins.imm1.byte & 0x0f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
	val0 |= 1 << BitOff;
	SETR16(cpu->ins.des, val0);
}
void BTS_RwRw(LPVCPU cpu) {
	u16 val0 = GETR16(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BTS %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR16(cpu->ins.src) & 0x80000000) BitOff = 16 - ((-((s32)GETR16(cpu->ins.src))) & 0x0f);
	else BitOff = GETR16(cpu->ins.src) & 0x0f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
	val0 |= 1 << BitOff;
	SETR16(cpu->ins.des, val0);
}
void BT_MdIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u8 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BT [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"BT [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) {
		ByteOff = (-((s8)cpu->ins.imm1.byte)) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)cpu->ins.imm1.byte)) & 0x07);
		val0 = VMRead8(addr - ByteOff);
	}
	else {
		ByteOff = cpu->ins.imm1.byte >> 3;
		BitOff = cpu->ins.imm1.byte & 0x07;
		val0 = VMRead8(addr + ByteOff);
	}
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
}
void BT_MdRd(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u32 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BT [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG32[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"BT [%s], %s", cpu->rm_str, REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR32(cpu->ins.src) & 0x80000000) {
		ByteOff = (-((s8)GETR32(cpu->ins.src))) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)GETR32(cpu->ins.src))) & 0x07);
		val0 = VMRead8(addr - ByteOff);
	}
	else {
		ByteOff = GETR32(cpu->ins.src) >> 3;
		BitOff = GETR32(cpu->ins.src) & 0x07;
		val0 = VMRead8(addr + ByteOff);
	}
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
}
void BT_MwIb(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u8 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BT [%s:%s], 0x%02X", SREG[cpu->ins.sreg], cpu->rm_str, cpu->ins.imm1.byte);
	else sprintf(cpu->ins_str,64,"BT [%s], 0x%02X", cpu->rm_str, cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) {
		ByteOff = (-((s8)cpu->ins.imm1.byte)) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)cpu->ins.imm1.byte)) & 0x07);
		val0 = VMRead8(addr - ByteOff);
	}
	else {
		ByteOff = cpu->ins.imm1.byte >> 3;
		BitOff = cpu->ins.imm1.byte & 0x07;
		val0 = VMRead8(addr + ByteOff);
	}
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
}
void BT_MwRw(LPVCPU cpu) {
	u32 addr = ModRM0(cpu);
	u8 val0;
	u32 ByteOff;
	u8 BitOff;
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"BT [%s:%s], %s", SREG[cpu->ins.sreg], cpu->rm_str, REG16[cpu->ins.src]);
	else sprintf(cpu->ins_str,64,"BT [%s], %s", cpu->rm_str, REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR16(cpu->ins.src) & 0x80000000) {
		ByteOff = (-((s8)GETR16(cpu->ins.src))) >> 3;
		ByteOff++;
		BitOff = 8 - ((-((s8)GETR16(cpu->ins.src))) & 0x07);
		val0 = VMRead8(addr - ByteOff);
	}
	else {
		ByteOff = GETR16(cpu->ins.src) >> 3;
		BitOff = GETR16(cpu->ins.src) & 0x07;
		val0 = VMRead8(addr + ByteOff);
	}
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
}
void BT_RdIb(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BT %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) BitOff = 32 - ((-((s8)cpu->ins.imm1.byte)) & 0x1f);
	else BitOff = cpu->ins.imm1.byte & 0x1f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
}
void BT_RdRd(LPVCPU cpu) {
	u32 val0 = GETR32(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BT %s, %s", REG32[cpu->ins.des], REG32[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR32(cpu->ins.src) & 0x80000000) BitOff = 32 - ((-((s32)GETR32(cpu->ins.src))) & 0x1f);
	else BitOff = GETR32(cpu->ins.src) & 0x1f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
}
void BT_RwIb(LPVCPU cpu) {
	u32 val0 = GETR16(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BT %s, 0x%02X", REG32[cpu->ins.des], cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (cpu->ins.imm1.byte & 0x80) BitOff = 16 - ((-((s8)cpu->ins.imm1.byte)) & 0x0f);
	else BitOff = cpu->ins.imm1.byte & 0x0f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
}
void BT_RwRw(LPVCPU cpu) {
	u32 val0 = GETR16(cpu->ins.des);
	u8 BitOff;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"BT %s, %s", REG16[cpu->ins.des], REG16[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETR16(cpu->ins.src) & 0x80000000) BitOff = 16 - ((-((s32)GETR16(cpu->ins.src))) & 0x0f);
	else BitOff = GETR16(cpu->ins.src) & 0x0f;
	if (val0 & (1 << BitOff)) SETF(CF);
	else CLRF(CF);
}
void SETA_Mb(LPVCPU cpu) {
	//cf == 0 && zf == 0
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETA BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETA BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(CF) && !CHEF(ZF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETA_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETA %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(CF) && !(CHEF(ZF)))SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETC_Mb(LPVCPU cpu) {
	//cf == 1
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETC BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETC BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(CF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETC_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETC %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(CF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETG_Mb(LPVCPU cpu) {
	//zf == 0 && sf == of
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETG BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETG BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(ZF) && (GETF(SF) == GETF(OF))) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETG_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETG%s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(ZF) && (GETF(SF) == GETF(OF))) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETL_Mb(LPVCPU cpu) {
	//sf != of
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETL BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETL BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETF(SF) != GETF(OF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETL_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETL %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETF(SF) != GETF(OF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETNA_Mb(LPVCPU cpu) {
	//cf == 1 || zf == 1
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETNA BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETNA BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(CF) || CHEF(ZF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETNA_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETNA %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(CF) || CHEF(ZF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETNC_Mb(LPVCPU cpu) {
	//cf == 0
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETNC BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETNC BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(CF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETNC_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETNC %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(CF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETNG_Mb(LPVCPU cpu) {
	//zf == 1 || sf == of
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETNG BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETNG BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(ZF) || (GETF(SF) == GETF(OF))) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETNG_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETNG %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(ZF) || (GETF(SF) == GETF(OF))) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETNL_Mb(LPVCPU cpu) {
	//sf == of
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETNL BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETNL BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETF(SF) == GETF(ZF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETNL_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETNL %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (GETF(SF) == GETF(ZF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETNO_Mb(LPVCPU cpu) {
	//of == 0
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETNO BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETNO BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(OF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETNO_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETNO %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(OF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETNP_Mb(LPVCPU cpu) {
	//pf == 0
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETNP BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETNP BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(PF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETNP_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETNP %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(PF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETNS_Mb(LPVCPU cpu) {
	//sf == 0
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETNS BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETNS BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(SF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETNS_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETNS %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(SF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETNZ_Mb(LPVCPU cpu) {
	//zf == 0
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETNZ BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETNZ BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(ZF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETNZ_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETNZ %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (!CHEF(ZF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETO_Mb(LPVCPU cpu) {
	//of == 1
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETO BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETO BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(OF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETO_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETO %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(OF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETP_Mb(LPVCPU cpu) {
	//pf == 1
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETP BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETP BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(PF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETP_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETP %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(PF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETS_Mb(LPVCPU cpu) {
	//sf == 1
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETS BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETS BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(SF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETS_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETS %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(SF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}
void SETZ_Mb(LPVCPU cpu) {
	//zf == 1
	u32 addr = ModRM0(cpu);
#ifdef VM_DEBUG
	if (cpu->ins.muchine_flag & _SREGP) sprintf(cpu->ins_str,64,"SETZ BYTE [%s:%s]", SREG[cpu->ins.sreg], cpu->rm_str);
	else sprintf(cpu->ins_str,64,"SETZ BYTE [%s]", cpu->rm_str);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(ZF)) VMWrite8(addr, 1);
	else VMWrite8(addr, 0);
}
void SETZ_Rb(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SETZ %s", REG8[cpu->ins.src]);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if (CHEF(ZF)) SETR8(cpu->ins.src, 1);
	else SETR8(cpu->ins.src, 0);
}

