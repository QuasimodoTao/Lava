/*
	lm/int.c
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

#include "lm.h"

void __Int00(void);
void __Int01(void);
void __Int02(void);
void __Int03(void);
void __Int04(void);
void __Int05(void);
void __Int06(void);
void __Int07(void);
void __Int08(void);
void __Int09(void);
void __Int0a(void);
void __Int0b(void);
void __Int0c(void);
void __Int0d(void);
void __Int0e(void);
void __Int10(void);
void __Int11(void);
void __Int12(void);
void __Int13(void);
void __Int14(void);
void __IntEgnore(void);

static struct {
	uint16_t Limt;
	uint16_t Base0;
	uint16_t Base1;
	uint16_t Base2;
	uint16_t Base3;
} IDTR;
static uint64_t * IDT;

static void MakeGate(void * Handle,void * Descriptor){
	uint64_t IDTEL = 0x00008e0000080000;
	uint64_t des;
	
	des = (uint64_t)Handle;
	IDTEL |= des & 0x000000000000ffff;
	IDTEL |= (des << 32) & 0xffff000000000000;
	((uint64_t*)Descriptor)[0] = IDTEL;
	((uint64_t*)Descriptor)[1] = (des >> 32) & 0x00000000ffffffff;
}
void _IntEgnore(uint64_t RIP){//
	print("Ignore interrupt.\n");
	stop();
}
void __DivideError(uint64_t RIP){//0
	print("Divide error.\n");
	stop();
}
void __RESERVED(uint64_t RIP){//1

	stop();
}
void __NMIInterrupt(void){//2
	print("NMI interrupt.\n");
	stop();
}
void __BreakPoint(uint64_t RIP){//3
	print("Break point.\n");
	stop();
}
void __Overflow(uint64_t RIP){//4
	print("Over flow.\n");
	stop();
}
void __BoundRangeExceeded(uint64_t RIP){//5
	print("Bound range exceeded.\n");
	stop();
}
void __InvaildOpcode(uint64_t RIP){//6
	print("Invaild opcode.\n");
	stop();
}
void __NoMathCoprocessor(uint64_t RIP){//7
	print("No math coprocessor.\n");
	stop();
}
void __DoubleFault(uint64_t RIP,uint64_t ErrCode){//8
	print("Double fault.\n");
	stop();
}
void __CoprocessorSegmentOverrun(uint64_t RIP){//9
	print("Coprocessoe segment overrun.\n");
	stop();
}
void __InvaildTSS(uint64_t RIP,uint64_t ErrCode){//a
	print("Invaild TSS.\n");
	stop();
}
void __SegmentNotPresent(uint64_t RIP,uint64_t ErrCode){//b
	print("Segment not present.\n");
	stop();
}
void __StackSegmentFault(uint64_t RIP,uint64_t ErrCode){//c
	print("Stack segment fault.\n");
	stop();
}
void __GeneralProtection(uint64_t RIP,uint64_t ErrCode){//d
	print("General protection.\n");
	stop();
}
void __PageFault(uint64_t RIP,uint64_t ErrCode){//e
	print("Page fault.\n");
	stop();
}
void __FPUError(uint64_t RIP){//10
	print("FPU Error.\n");
	stop();
}
void __AlignmentCheck(uint64_t RIP,uint64_t ErrCode){//11
	print("Alignment check.\n");
	stop();
}	
void __MachineCheck(uint64_t RIP){//12
	print("Machine check.\n");
	stop();
}
void __SIMDFPException(uint64_t RIP){//13
	print("SIMDFP exception.\n");
	stop();
}
void __VirtualizationException(uint64_t RIP){//14
	print("Virtualization exception.\n");
	stop();
}
uint64_t int_init(uint64_t MemoryStart){
	uint64_t IDTE[2];
	int i;
	
	IDT = (uint64_t*)MemoryStart;
	MemoryStart += 16 * 256;
	IDTR.Limt = 0xfff;
	IDTR.Base0 = ((uint64_t)IDT) & 0xffff;
	IDTR.Base1 = (((uint64_t)IDT) >> 16) & 0xffff;
	IDTR.Base2 = (((uint64_t)IDT) >> 32) & 0xffff;
	IDTR.Base3 = (((uint64_t)IDT) >> 48) & 0xffff;
	
	MakeGate(&__Int00,IDT + 0x00);
	MakeGate(&__Int01,IDT + 0x02);
	MakeGate(&__Int02,IDT + 0x04);
	MakeGate(&__Int03,IDT + 0x06);
	MakeGate(&__Int04,IDT + 0x08);
	MakeGate(&__Int05,IDT + 0x0a);
	MakeGate(&__Int06,IDT + 0x0c);
	MakeGate(&__Int07,IDT + 0x0e);
	MakeGate(&__Int08,IDT + 0x10);
	MakeGate(&__Int09,IDT + 0x12);
	MakeGate(&__Int0a,IDT + 0x14);
	MakeGate(&__Int0b,IDT + 0x16);
	MakeGate(&__Int0c,IDT + 0x18);
	MakeGate(&__Int0d,IDT + 0x1a);
	MakeGate(&__Int0e,IDT + 0x1c);
	MakeGate(&__IntEgnore,IDT + 0x1e);
	MakeGate(&__Int10,IDT + 0x20);
	MakeGate(&__Int11,IDT + 0x22);
	MakeGate(&__Int12,IDT + 0x24);
	MakeGate(&__Int13,IDT + 0x26);
	MakeGate(&__Int14,IDT + 0x28);
	MakeGate(&__IntEgnore,IDT + 0x2a);
	MakeGate(&__IntEgnore,IDT + 0x2c);
	MakeGate(&__IntEgnore,IDT + 0x2e);
	MakeGate(&__IntEgnore,IDT + 0x30);
	MakeGate(&__IntEgnore,IDT + 0x32);
	MakeGate(&__IntEgnore,IDT + 0x34);
	MakeGate(&__IntEgnore,IDT + 0x36);
	MakeGate(&__IntEgnore,IDT + 0x38);
	MakeGate(&__IntEgnore,IDT + 0x3a);
	MakeGate(&__IntEgnore,IDT + 0x3c);
	MakeGate(&__IntEgnore,IDT + 0x3e);
	MakeGate(&__IntEgnore,IDTE);
	for(i = 32;i < 256;i++){
		(IDT)[i * 2] = IDTE[0];
		(IDT)[i * 2 + 1] = IDTE[1];
	}
	lidt(&IDTR);
	sti();
	return MemoryStart;
}
