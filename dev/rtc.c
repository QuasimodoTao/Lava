/*
	dev/rtc.c
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
#include <time.h>
#include <int.h>
#include <lava.h>
#include <timer.h>

#define CMOS_INDEX_PORT		0x70
#define CMOS_DATA_PORT		0x71
#define CMOS_SECOND			0x00
#define CMOS_SECOND_ALARM	0x01
#define CMOS_MINUTE			0x02
#define CMOS_MINUTE_ALARM	0x03
#define CMOS_HOUR			0x04
#define CMOS_HOUR_ALARM		0x05
#define CMOS_WEEK			0x06
#define CMOS_DAY			0x07
#define CMOS_MONTH			0x08
#define CMOS_YEAR			0x09
#define CMOS_REG_A			0x0a
#define CMOS_REG_B			0x0b
#define CMOS_REG_C			0x0c
#define CMOS_REG_D			0x0d

#define PBCD2BIN(x)		(((((x) & 0xf0) >> 4) * 10) + ((x) & 0x0f))

void counter_updata();
void pit_correct();

static volatile int nock;
static volatile int mask;

static u8 cmos_in(u8 reg){
	u8 val;
	ID();
	outb(CMOS_INDEX_PORT,reg);
	val = inb(CMOS_DATA_PORT);
	IE();
	return val;
}
static void cmos_out(u8 reg,u8 val){
	ID();
	outb(CMOS_INDEX_PORT,reg);
	outb(CMOS_DATA_PORT,val);
	IE();
}
static int rtc_handle(int irq){
	u8 reg;
	
	reg = cmos_in(0x80 | CMOS_REG_C);
	cmos_out(0x80 | CMOS_REG_C,0);
	if(!(reg & 0x40)) return -1;
	if(mask) {
		nock = 1;
		return 0;
	}
	counter_updata();
	return 0;
}
static void rtc_enable(){
	u8 prev;
	irq_enable(RTC_IRQ);
	ID();
	prev = cmos_in(0x80 | CMOS_REG_B);
	cmos_out(0x80 | CMOS_REG_B,prev | 0x40);
	IE();
}
static void rtc_disable(){
	u8 prev;
	irq_disable(RTC_IRQ);
	ID();
	prev = cmos_in(0x80 | CMOS_REG_B);
	cmos_out(0x80 | CMOS_REG_B,prev & ~0x40);
	IE();
}
static void rtc_mask(){
	mask = 1;
}
static void rtc_unmask(){
	mask = 0;
	if(nock) counter_updata();
	nock = 0;
}
static struct _COUNTER_ rtc = {
	rtc_enable,
	rtc_disable,
	rtc_unmask,
	rtc_mask,
	976562500000,
	COUNTER_TYPE_RTC
};
void time_init(){
	struct time_t cur_time;
	u8 prev;
	
	ID();
	prev = cmos_in(0x80 | CMOS_REG_A);
	cmos_out(0x80 | CMOS_REG_A,(prev & 0xf0) | 0x06);//1024HZ
	prev = cmos_in(0x80 | CMOS_REG_B);
	cmos_out(0x80 | CMOS_REG_B,prev & ~0x40);//disable Periodic interrupt
	request_irq(RTC_IRQ,rtc_handle);
	while(cmos_in(CMOS_REG_A) & 0x80) nop();
	cur_time.second = cmos_in(CMOS_SECOND);
	cur_time.minute = cmos_in(CMOS_MINUTE);
	cur_time.hour = cmos_in(CMOS_HOUR);
	cur_time.week = cmos_in(CMOS_WEEK);
	cur_time.day = cmos_in(CMOS_DAY);
	cur_time.month = cmos_in(CMOS_MONTH);
	cur_time.year = cmos_in(CMOS_YEAR);
	if(cmos_in(0x80 | CMOS_REG_B) & 0x04){
		cur_time.second = PBCD2BIN(cur_time.second);
		cur_time.minute = PBCD2BIN(cur_time.minute);
		cur_time.hour = PBCD2BIN(cur_time.hour);
		cur_time.day = PBCD2BIN(cur_time.day);
		cur_time.month = PBCD2BIN(cur_time.month);
		cur_time.year = PBCD2BIN(cur_time.year);
	}
	cur_time.year += 2000;
	time2stamp(&cur_time, &kernel_time);
	IE();	
	mask = nock = 0;
	reg_counter(&rtc);
}

