//Lava OS
//PageFault
//20-04-11-15-08

#include <vm.h>
#include <config.h>
#include <stdio.h>
#include <stddef.h>

void LAHF_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"LAHF");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETR8(AH, cpu->eflags.rl);
}
void SAHF_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"SAHF");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eflags.rl = GETR8(AH);
}
void CLC_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CLC");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	CLRF(CF);
}
void CLD_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CLD");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	CLRF(DF);
}
void CLI_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CLI");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	CLRF(IF);
}
void CMC_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"CMC");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	cpu->eflags.erx ^= CF;
}
void STC_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"STC");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETF(CF);
}
void STD_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"STD");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETF(DF);
}
void STI_No(LPVCPU cpu) {
#ifdef VM_DEBUG
	sprintf(cpu->ins_str,64,"STI");
	if(cpu->cond) cpu->cond(cpu->cpu,cpu);
#endif
	SETF(IF);
}