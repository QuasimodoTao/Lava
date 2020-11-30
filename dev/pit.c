/*
	dev/pit.c
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
#include <int.h>
#include <kernel.h>
#include <timer.h>

#define PIT_CMD		0x43
#define PIT_CHAN0	0x40
#define PIT_DIV		1193

//1.1931816666 MHz
//999847746012fs
void counter_updata();
static volatile int wait_nock;
static volatile int nock;
static volatile int mask;




static int internel_wait_handle(int irq){
	wait_nock = 1;
	return 0;
}

void internel_wait(int usecond){
	volatile u64 val;
	u64 flags;
	
	val = (1193182 * usecond);
	val /= 1000000;
	val++;
	wait_nock = 0;
	//printk("val = %d.\n",val);
	request_irq(PIT_IRQ,internel_wait_handle);
	ID();
	outb(PIT_CMD,0x34);
	outb(PIT_CHAN0,val & 0xff);
	outb(PIT_CHAN0,val >> 8);
	irq_enable(PIT_IRQ);
	sti();
	while(!wait_nock) halt();
	cli();
	irq_disable(PIT_IRQ);
	IE();
	reject_irq(PIT_IRQ,internel_wait_handle);
}
static int pit_handle(int irq){
	if(mask){
		nock = 1;
		return 0;
	}
	counter_updata();
	return 0;
}
static void pit_enable(){irq_enable(PIT_IRQ);}
static void pit_disable(){irq_disable(PIT_IRQ);}
static void pit_unmask(){
	mask = 0;
	if(nock) counter_updata();
	nock = 0;
}
static void pit_mask(){mask = 1;}
static struct _COUNTER_ pit = {
	pit_enable,
	pit_disable,
	pit_unmask,
	pit_mask,
	999847746012,
	COUNTER_TYPE_PIT
};
void pit_init(){
	ID();
	outb(PIT_CMD,0x34);
	outb(PIT_CHAN0,PIT_DIV & 0xff);
	outb(PIT_CHAN0,PIT_DIV >> 8);
	IE();
	request_irq(PIT_IRQ,pit_handle);
	mask = nock = 0;
	reg_counter(&pit);
}
