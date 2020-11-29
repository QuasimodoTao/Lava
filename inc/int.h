//Lava OS
//PageFault
//20-03-27-21-16

#ifndef _INT_H_
#define _INT_H_

#include <arch.h>
#include <stddef.h>

#define PIT_IRQ		0
#define HPET0_IRQ	0
#define KBD_IRQ		1
#define RTC_IRQ		8
#define MUS_IRQ		12
#define FPU_IRQ		13
#define SERIAL1_IRQ	4
#define SERIAL2_IRQ	3

#define IPIM_FIXED	0
#define IPIM_SELF	1
#define IPIM_ALL_IN	2
#define IPIM_ALL_EX	3

int irq_routine(int irq,int lapic_id);
int irq_enable(u32 irq);
int irq_disable(u32 irq);
int request_irq(int,int(*handle)(int irq));
int reject_irq(int irq,int(*handle)(int irq));
void send_ipi(u8 ipi,u8 dest_cpu,int priority,int mode);
void request_ipi(u8 ipi,int(*handle)());
void make_gate(int vector,u64 addr,int ist,int dpl,int type);
int set_ist(int vector,int ist,u64 rsp);
int enable_ist(int vector);

#define int_disable()	{\
	cli();\
	write_private_dword(int_disable_count,read_private_dword(int_disable_count) + 1);\
}
#define int_enable()	{\
	u32 val;\
	val = read_private_dword(int_disable_count);\
	if(val){\
		val--;\
		write_private_dword(int_disable_count,val);\
	}\
	if(!val) sti();\
}

#define ID()	int_disable()
#define IE()	int_enable()

#endif