//Lava OS
//PageFault
//20-04-11-15-08

#include <vm.h>
#include <config.h>
#include <stdio.h>
#include <stddef.h>
void AAA_No(LPVCPU cpu){
	u8 ah = GETR8(AH);
	u8 al = GETR8(AL);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AAA");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if ((al & 0x0f) > 9 || CHEF(AF)) {
		SETR8(AL, (al + 6) & 0x0f);
		SETR8(AH, ah + 1);
		SETF(AF);
		SETF(CF);
	}
	else {
		CLRF(AF);
		CLRF(CF);
		SETR8(AL, al & 0x0f);
	}
}
void AAD_Ib(LPVCPU cpu){
	u8 al = GETR8(AL);
	u8 ah = GETR8(AH);
	int _PF,n;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AAD 0x%02X",cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AL,(al + ah * cpu->ins.imm1.byte) & 0xff);
	SETR8(AH, 0);
	al = GETR8(AL);
	if(al & 0x80) SETF(SF);
	else CLRF(SF);
	if(!al) SETF(ZF);
	else CLRF(ZF);
	_PF = 0;
	for(n = 0;n < 8;n++){
		_PF ^= al & 1;
		al >>= 1;
	}
	if(_PF) CLRF(PF);
	else SETF(PF);
}
void AAM_Ib(LPVCPU cpu){
	u8 al = GETR8(AL);
	u8 ah = GETR8(AH);
	int _PF,n;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AAM 0x%02X", cpu->ins.imm1.byte);
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AH, al / cpu->ins.imm1.byte);
	SETR8(AL, al%cpu->ins.imm1.byte);
	al = GETR8(AL);
	if(al & 0x80) SETF(SF);
	else CLRF(SF);
	if(!al) SETF(ZF);
	else CLRF(ZF);
	_PF = 0;
	for(n = 0;n < 8;n++){
		_PF ^= al & 1;
		al >>= 1;
	}
	if(_PF) CLRF(PF);
	else SETF(PF);
}
void AAS_No(LPVCPU cpu){
	u8 al = GETR8(AL);
	u8 ah = GETR8(AH);
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"AAS");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	if ((al & 0x0f) > 9 || CHEF(AF)) {
		SETR8(AL, (al - 6) & 0x0f);
		SETR8(AH, ah - 1);
		SETF(AF);
		SETF(CF);
	}
	else {
		SETR8(AL, al & 0x0f);
		CLRF(CF);
		CLRF(AF);
	}
}
void DAA_No(LPVCPU cpu){
	u8 al = GETR8(AL);
	u8 ah = GETR8(AH);
	u8 cf = GETF(CF);
	int _PF,n;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"DAA");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	CLRF(CF);
	if ((al & 0x0f) > 9 || CHEF(AF)) {
		SETR8(AL, GETR8(AL) + 6);
		if (cf || al >= 0xfa) SETF(CF);
		SETF(AF);
	}
	else CLRF(AF);
	if (al > 0x99 || cf) {
		SETR8(AL, GETR8(AL) + 0x60);
		SETF(CF);
	}
	else CLRF(CF);
	al = GETR8(AL);
	if(al & 0x80) SETF(SF);
	else CLRF(SF);
	if(!al) SETF(ZF);
	else CLRF(ZF);
	_PF = 0;
	for(n = 0;n < 8;n++){
		_PF ^= al & 1;
		al >>= 1;
	}
	if(_PF) CLRF(PF);
	else SETF(PF);
}
void DAS_No(LPVCPU cpu){
	u8 al = GETR8(AL);
	u8 ah = GETR8(AH);
	u8 cf = GETF(CF);
	int _PF,n;
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"DAS");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	CLRF(CF);
	if ((al & 0x0f) > 9 || CHEF(AF)) {
		SETR8(AL, GETR8(AL) - 6);
		if (cf || al <= 5) SETF(CF);
		SETF(AF);
	}
	else CLRF(AF);
	if (al > 0x99 || cf) {
		SETR8(AL, GETR8(AL - 0x60));
		SETF(CF);
	}
	al = GETR8(AL);
	if(al & 0x80) SETF(SF);
	else CLRF(SF);
	if(!al) SETF(ZF);
	else CLRF(ZF);
	_PF = 0;
	for(n = 0;n < 8;n++){
		_PF ^= al & 1;
		al >>= 1;
	}
	if(_PF) CLRF(PF);
	else SETF(PF);
}
