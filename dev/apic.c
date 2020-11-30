/*
	dev/apic.c
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

#include <stddef.h>
#include <asm.h>
#include <config.h>
#include <mp.h>
#include <arch.h>
#include <kernel.h>
#include <mm.h>
#include <lava.h>
#include <int.h>
#include <spinlock.h>
#include <error.h>
#include <stdio.h>

#define IOAPICEOI(index,vector)	asm("movl %0,64(%1)"::"r"(((u32)(vector)) & 0xff),"r"(ioapic_mmio + index * PAGE_SIZE))
#define IOAPIC_ID		0x00
#define IOAPIC_VER		0x01
#define IOAPIC_ARB		0x02
#define IOAPIC_TABL(x)	(0x10+(x)*2)
#define IOAPIC_TABH(x)	(0x11+(x)*2)
#define LAPIC_ID		 0x0020
#define LAPIC_VER		0x0030
#define LAPIC_TPR		0x0080
#define LAPIC_APR		0x0090
#define LAPIC_PPR		0x00a0
#define LAPIC_EOI		0x00b0
#define LAPIC_RRR		0x00c0
#define LAPIC_LDR		0x00d0
#define LAPIC_DFR		0x00e0
#define LAPIC_SIVR		0x00f0
#define LAPIC_ISR(x)	(0x0100+(x)*0x10)
#define LAPIC_TMR(x)	(0x0180+(x)*0x10)
#define LAPIC_IRR(x)	(0x0200+(x)*0x10)
#define LAPIC_ESR		0x0280
#define LAPIC_CMCIR		0x02f0
#define LAPIC_ICRL		0x0300
#define LAPIC_ICRH		0x0310
#define LAPIC_TR		0x0320
#define LAPIC_TSR		0x0330
#define LAPIC_PMCR		0x0340
#define LAPIC_LINT0		0x0350
#define LAPIC_LINT1		0x0360
#define LAPIC_ER		0x0370
#define LAPIC_TICR		0x0380
#define LAPIC_TCCR		0x0390
#define LAPIC_TDCR		0x03e0
#define IOAPIC_ID_MASK	0x0f00000000
#define IOAPIC_ID_SHIFT	24
#define IOAPIC_RED_ENT_MASK	0x00ff0000
#define IOAPIC_RED_ENT_SHIFT	16
#define LAPIC_ID_MASK	0xff000000
#define LAPIC_ID_SHIFT	24
#define LAPICI(reg)		(*(volatile u32*)(lapic_mmio+(reg)))
#define LAPICO(reg,val)	(*(volatile u32*)(lapic_mmio+(reg))=(u32)(val))
#define lapic_mmio		LAPIC_MMIO
#define ioapic_mmio		IOAPIC_MMIO
#define IRQ_LIST_BUSY	MAX_IOAPIC
#define IOAPIC_BUSY		0
#define EOI()	apic_eoi()

static inline void IOAPICO(int index,int reg,u32 val){
	*(volatile u32*)(ioapic_mmio + index * PAGE_SIZE) = reg;
	*(volatile u32*)(ioapic_mmio + index * PAGE_SIZE + 0x10) = val;
}
static inline u32 IOAPICI(int index,int reg){
	*(volatile u32*)(ioapic_mmio + index * PAGE_SIZE) = reg;
	return *(volatile u32*)(ioapic_mmio + index * PAGE_SIZE + 0x10);
}
struct _IRQ_HANDLE_ {
	int (*handle)(int);
	struct _IRQ_HANDLE_ * next;
};

void ap_entry_point(u16 * ap_lock);
void switchcs(u16 selector);
void internel_wait(int usecond);

extern u8 AP_init[];
extern int AP_init_size;
extern int AP_enter;

struct _IOAPIC_ {
	u8 id;
	u8 lint_count;
};
struct _IRQ2LINT_ {
	u8 index;
	u8 lint;
};

extern u8 __irq_start[];
extern int __irq_size;
extern int __irq_code;//
extern int __irq_enter;//
//extern int __irq_kernel_ss_0;//
//extern int __irq_kernel_ss_1;//
extern u64 int_ent_last_address;

static struct _IOAPIC_ ioapic[MAX_IOAPIC];
static struct _IRQ2LINT_ irq2lint[IRQ_COUNT];
static struct _IRQ_HANDLE_ * irq_handle[IRQ_COUNT];
static u8 cpu_id2lapic_id[MAX_CPU_COUNT];
static int lock;
static int apic_timer_time;
static int irq_mask_count[IRQ_COUNT];
static u8 irq_mask_busy[(IRQ_COUNT + 0x07) >> 3];
spin_optr_def_bit(IRQList,&lock,IRQ_LIST_BUSY);
spin_optr_def_arg_bit(IOAPIC,&lock,IOAPIC_BUSY);
static int (*ipi_handle[IPI_COUNT])() = {NULL,};


int request_irq(int irq,int(*handle)(int)){
	struct _IRQ_HANDLE_ * cur;
	
	if(irq >= IRQ_COUNT) return ERR_OUT_OF_RANGE;
	cur = kmalloc(sizeof(struct _IRQ_HANDLE_),0);
	cur->handle = handle;
	ID();
	LockIRQList();
	cur->next = irq_handle[irq];
	irq_handle[irq] = cur;
	UnlockIRQList();
	IE();
	return 0;
}
int reject_irq(int irq,int(*handle)(int)){
	struct _IRQ_HANDLE_ * _handle, * prev = NULL;

	if(irq >= IRQ_COUNT) return ERR_OUT_OF_RANGE;
	ID();
	LockIRQList();
	for(_handle = irq_handle[irq];_handle;prev = _handle,_handle = _handle->next)
		if(_handle->handle == handle){
			if(prev) xchgq((void*)&(prev->next),(u64)(_handle->next));
			else irq_handle[irq] = _handle->next;
			UnlockIRQList();
			IE();
			kfree(_handle);
			return 0;
		}
	UnlockIRQList();
	IE();
	return ERR_OUT_OF_RANGE;
}
int irq_routine(int irq,int lapic_id){
	
	if(irq >= IRQ_COUNT || irq2lint[irq].index == 0xff) return ERR_OUT_OF_RANGE;
	ID();
	LockIOAPIC(irq2lint[irq].index);
	IOAPICO(irq2lint[irq].index,IOAPIC_TABH(irq2lint[irq].lint),irq + 32);
	UnlockIOAPIC(irq2lint[irq].index);
	IE();
	return 0;
}
static int assign_int(u32 lint,u32 ioapic_id,u32 irq){
	u32 tmp;
	int index;
	
	for(index = 0;index < init_msg.IOAPICCount;index++){
		if(ioapic[index].id == ioapic_id) break;
	}
	if(index == init_msg.IOAPICCount) return ERR_OUT_OF_RANGE;
	if(lint >= ioapic[index].lint_count) return ERR_OUT_OF_RANGE;
	ID();
	LockIOAPIC(index);
	while((tmp = IOAPICI(index,IOAPIC_TABL(lint))) & 0x00002000) nop();
	IOAPICO(index,IOAPIC_TABL(lint),0);
	tmp &= 0x00002000;
	if(irq >= 0x100) tmp |= 0x000100ff;
	else {
		tmp |= 0x00010000 | ((irq + 32) & 0xff);
		irq2lint[irq].lint = lint;
		irq2lint[irq].index = index;
	}
	IOAPICO(index,IOAPIC_TABL(lint),tmp);
	UnlockIOAPIC(index);
	IE();
	return 0;
}
int apic_enable(){
	u32 tmp;

	ID();
	tmp = LAPICI(LAPIC_SIVR);
	tmp |= 0x100;
	LAPICO(LAPIC_SIVR,tmp);
	IE();
}
int apic_disable(){
	u32 tmp;
	
	ID();
	tmp = LAPICI(LAPIC_SIVR);
	tmp &= ~0x100;
	LAPICO(LAPIC_SIVR,tmp);
	IE();
}
static void apic_eoi(){
	ID();
	LAPICO(LAPIC_EOI,0);
	IE();
}
int irq_enable(u32 irq){
	u32 tmp;
	
	if(irq >= IRQ_COUNT) return ERR_OUT_OF_RANGE;
	if(irq2lint[irq].index == 0xff) return ERR_OUT_OF_RANGE;
	ID();
	spin_lock_bit(irq_mask_busy,irq);
	irq_mask_count[irq]--;
	if(!irq_mask_count[irq]) {
		LockIOAPIC(irq2lint[irq].index);
		tmp = IOAPICI(irq2lint[irq].index,IOAPIC_TABL(irq2lint[irq].lint));
		tmp &= 0xfffeffff;
		IOAPICO(irq2lint[irq].index,IOAPIC_TABL(irq2lint[irq].lint),tmp);
		UnlockIOAPIC(irq2lint[irq].index);
	}
	spin_unlock_bit(irq_mask_busy,irq);
	IE();
	return 0;
}
int irq_disable(u32 irq){
	u32 tmp;
	
	if(irq >= IRQ_COUNT) return ERR_OUT_OF_RANGE;
	if(irq2lint[irq].index == 0xff) return ERR_OUT_OF_RANGE;
	ID();
	spin_lock_bit(irq_mask_busy,irq);
	if(!irq_mask_count[irq]){
		LockIOAPIC(irq2lint[irq].index);
		tmp = IOAPICI(irq2lint[irq].index,IOAPIC_TABL(irq2lint[irq].lint));
		tmp |= 0x00010000;
		IOAPICO(irq2lint[irq].index,IOAPIC_TABL(irq2lint[irq].lint),tmp);
		UnlockIOAPIC(irq2lint[irq].index);
	}
	irq_mask_count[irq]++;
	spin_unlock_bit(irq_mask_busy,irq);
	IE();
	return 0;
}
void send_ipi(u8 ipi,u8 dest_cpu,int priority,int mode){
	u32 tmp;

	if(priority) tmp = 0x00004000;
	else tmp = 0x00004100;
	tmp |= (mode & 0x03) << 18;
	ID();
	LAPICO(LAPIC_ICRH,dest_cpu << 24);
	LAPICO(LAPIC_ICRL,(ipi + IPI_START_IRQ) | tmp);
	IE();
}
static void ipi_arise(int ipi){
	if(ipi > IPI_COUNT) return;
	if(ipi_handle[ipi]) ipi_handle[ipi]();
	EOI();
}
void request_ipi(u8 ipi,int(*handle)()){
	if(ipi > IPI_COUNT) return;
	ipi_handle[ipi] = handle;
}
void irq_arise(int irq){
	struct _IRQ_HANDLE_ * cur;
	
	SD();
	for(cur = irq_handle[irq];cur;cur = cur->next){
		if(!cur->handle){
			print("BUG:arch/init.c::int_vector_ent().\n");
			stop();
		}
		cur->handle(irq);
	}
	EOI();
	SE();
}
void lapic_init(){
	u8 lapic_id;
	u32 tmp;
	ID();
	tmp = LAPICI(LAPIC_LINT0);
	tmp &= 0xffff00ff;
	tmp |= 0x00005700;
	LAPICO(LAPIC_LINT0,tmp);
	tmp = LAPICI(LAPIC_LINT1);
	tmp &= 0xffff00ff;
	tmp |= 0x00005400;
	LAPICO(LAPIC_LINT1,tmp);
	lapic_id = (LAPICI(LAPIC_ID) & LAPIC_ID_MASK) >> LAPIC_ID_SHIFT;
	IE();
	cpu_id2lapic_id[GetCPUId()] = lapic_id;
}
void ioapic_init(){
	int index;
	
	for(index = 0;index < init_msg.IOAPICCount;index++){
		put_page(init_msg.IOAPIC[index] | 0x03,NULL,(void*)(ioapic_mmio + index * PAGE_SIZE));
		page_uncacheable(NULL,(void*)(ioapic_mmio + index * PAGE_SIZE),PAGE_SIZE);
		ioapic[index].id = (IOAPICI(index,IOAPIC_ID) & IOAPIC_ID_MASK) >> IOAPIC_ID_SHIFT;
		ioapic[index].lint_count = (IOAPICI(index,IOAPIC_VER) & IOAPIC_RED_ENT_MASK) >> IOAPIC_RED_ENT_SHIFT;
	}
	memset(irq2lint,0xff,sizeof(irq2lint));
}
void mp_init(){
	u32 i;
	u8 * cur;
	u8 ioapic_id;
	struct _MPCTH_ * header;
	u8 check;
	u64 base;
	struct _MPS_IO_INT_ * io_int;
	u32 tmp;
	u64 _int_ent_last_address,__int_ent_last_address;

//	__irq_start[__irq_kernel_ss_0] = __irq_start[__irq_kernel_ss_1] = KERNEL_SS;
	
	_int_ent_last_address = int_ent_last_address + __irq_size * (IRQ_COUNT + IPI_COUNT);
	__int_ent_last_address = int_ent_last_address;
	__int_ent_last_address += PAGE_SIZE - 1;
	__int_ent_last_address &= ~(PAGE_SIZE - 1);
	while(__int_ent_last_address < _int_ent_last_address){
		put_page(get_free_page(0,0),NULL,(void*)__int_ent_last_address);
		__int_ent_last_address += PAGE_SIZE;
	}
	for(i = 0;i < IRQ_COUNT;i++) {
		make_gate(i + 32,int_ent_last_address + __irq_size * i,0,0,0x0f);
		memcpy((void*)(int_ent_last_address + __irq_size * i),__irq_start,__irq_size);
		*(void**)(int_ent_last_address + __irq_size * i + __irq_enter) = irq_arise;
		*(u8*)(int_ent_last_address + __irq_size * i + __irq_code) = i;
	}
	for(i = 0;i < IPI_COUNT;i++){
		make_gate(i + IPI_START_IRQ,int_ent_last_address + __irq_size * (i + IPI_START_IRQ),0,0,0x0f);
		memcpy((void*)(int_ent_last_address + __irq_size * (i + IPI_START_IRQ)),__irq_start,__irq_size);
		*(void**)(int_ent_last_address + __irq_size * (i + IPI_START_IRQ) + __irq_enter) = ipi_arise;
		*(u8*)(int_ent_last_address + __irq_size * (i + IPI_START_IRQ) + __irq_code) = i;
	}
	int_ent_last_address = _int_ent_last_address;
	memset(irq_handle,0,sizeof(irq_handle));
	memset(cpu_id2lapic_id,0xff,sizeof(cpu_id2lapic_id));
	for(i = 0;i < IRQ_COUNT;i++) irq_mask_count[i] = 1;
	memset(irq_mask_busy,0,sizeof(irq_mask_busy));
	lock = 0;
	ioapic_init();
	put_page(init_msg.LAPIC | 0x03,NULL,(void*)lapic_mmio);
	page_uncacheable(NULL,(void*)lapic_mmio,PAGE_SIZE);
	if(init_msg.Feature & 0xff){
		//1:ISA,2:EISA,3:EISA,4:MCA,5:ISA+PCI,6:EISA+PCI,7:MCA+PCI
		ioapic_id = (IOAPICI(0,IOAPIC_ID) & IOAPIC_ID_MASK) >> IOAPIC_ID_SHIFT;
		assign_int(0,ioapic_id,0xffffffff);
		assign_int(1,ioapic_id,1);
		assign_int(2,ioapic_id,0);
		assign_int(3,ioapic_id,3);
		assign_int(4,ioapic_id,4);
		assign_int(5,ioapic_id,5);
		assign_int(6,ioapic_id,6);
		assign_int(7,ioapic_id,7);
		assign_int(8,ioapic_id,8);
		assign_int(9,ioapic_id,9);
		assign_int(10,ioapic_id,10);
		assign_int(11,ioapic_id,11);
		assign_int(12,ioapic_id,12);
		assign_int(13,ioapic_id,13);
		assign_int(14,ioapic_id,14);
		assign_int(15,ioapic_id,15);
	}
	else{
		header = ADDRP2V(init_msg.PCMP);
		base = (u64)header;
		cur = (u8*)(((u64)header) + sizeof(struct _MPCTH_));
		while(((u64)cur) - base < header->tab_len){
			switch(*cur){
			case 0: cur += sizeof(struct _MPS_CPU_); break;
			case 1: cur += sizeof(struct _MPS_BUS_); break;
			case 2: cur += sizeof(struct _MPS_IOAPIC_); break;
			case 3:
				io_int = (struct _MPS_IO_INT_ *)cur;
				assign_int(io_int->dst_ioapic_int,io_int->dst_ioapic_id,io_int->sor_bus_irq);
				cur += sizeof(struct _MPS_IO_INT_);
				break;
			case 4: cur += sizeof(struct _MPS_LINT_ASGN_); break;
			case 128: cur += sizeof(struct _MPS_MAP_); break;
			case 129: cur += sizeof(struct _MPS_BHD_); break;
			case 130: cur += sizeof(struct _MPS_CBASM_ ); break;
			}
		}
	}
	lapic_init();
	apic_enable();
	*(void**)(AP_init + AP_enter) = ap_entry_point;
	memcpy((void*)AP_INIT_BASE,AP_init,AP_init_size);
	LAPICO(LAPIC_ICRL,0x000c4500);
	internel_wait(200);
	LAPICO(LAPIC_ICRL,0x000c4600 | AP_ENT_VECTOR);
	internel_wait(10000);
	LAPICO(LAPIC_ICRL,0x000c4600 | AP_ENT_VECTOR);
}