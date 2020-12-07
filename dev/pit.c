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
#include <eisa.h>

#define PIT_CMD		0x43
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
	u64 rf;
	
	val = (1193182 * usecond);
	val /= 1000000;
	val++;
	wait_nock = 0;
	request_irq(EISA_TIMER_IRQ,internel_wait_handle);
	SFI(rf);
	outb(EISA_PIT1_CTRL_PORT,EISA_PIT_SELECT_CH0 | EISA_PIT_LOW_HI |
		EISA_PIT_MODE2 | EISA_PIT_BIN);
	outb(EISA_PIT1_CH0_PORT,val & 0xff);
	outb(EISA_PIT1_CH0_PORT,val >> 8);
	irq_enable(EISA_TIMER_IRQ);
	sti();
	while(!wait_nock) halt();
	cli();
	irq_disable(EISA_TIMER_IRQ);
	LF(rf);
	reject_irq(EISA_TIMER_IRQ,internel_wait_handle);
}
static int pit_handle(int irq){
	if(mask){
		nock = 1;
		return 0;
	}
	counter_updata();
	return 0;
}
static void pit_enable(){irq_enable(EISA_TIMER_IRQ);}
static void pit_disable(){irq_disable(EISA_TIMER_IRQ);}
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
	u64 rf;

	SFI(rf);
	outb(EISA_PIT1_CTRL_PORT,EISA_PIT_SELECT_CH0 | EISA_PIT_LOW_HI |
		EISA_PIT_MODE2 | EISA_PIT_BIN);
	outb(EISA_PIT1_CH0_PORT,PIT_DIV & 0xff);
	outb(EISA_PIT1_CH0_PORT,PIT_DIV >> 8);
	LF(rf);
	request_irq(EISA_TIMER_IRQ,pit_handle);
	mask = nock = 0;
	reg_counter(&pit);
}
