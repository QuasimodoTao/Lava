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
#include <eisa.h>

#define PBCD2BIN(x)		(((((x) & 0xf0) >> 4) * 10) + ((x) & 0x0f))

void counter_updata();
void pit_correct();

static volatile int nock;
static volatile int mask;

static u8 cmos_in(u8 reg){
	u8 val;
	u64  rf;

	SFI(rf);
	outb(EISA_CMOS_ADDR_PORT,reg);
	val = inb(EISA_CMOS_DATA_PORT);
	LF(rf);
	return val;
}
static void cmos_out(u8 reg,u8 val){
	u64 rf;

	SFI(rf);
	outb(EISA_CMOS_ADDR_PORT,reg);
	outb(EISA_CMOS_DATA_PORT,val);
	LF(rf);
}
static int rtc_handle(int irq){
	u8 reg;
	
	reg = cmos_in(0x80 | EISA_CMOS_RTC_C);
	cmos_out(0x80 | EISA_CMOS_RTC_C,0);
	if(!(reg & EISA_CMOS_RTC_PERIODIC_ENABLE)) return -1;
	if(mask) {
		nock = 1;
		return 0;
	}
	counter_updata();
	return 0;
}
static void rtc_enable(){
	u8 prev;
	u64 rf;

	irq_enable(EISA_RTC_IRQ);
	SFI(rf);
	prev = cmos_in(0x80 | EISA_CMOS_RTC_B);
	cmos_out(0x80 | EISA_CMOS_RTC_B,prev | EISA_CMOS_RTC_PERIODIC_ENABLE);
	LF(rf);
}
static void rtc_disable(){
	u8 prev;
	u64 rf;

	irq_disable(EISA_RTC_IRQ);
	SFI(rf);
	prev = cmos_in(0x80 | EISA_CMOS_RTC_B);
	cmos_out(0x80 | EISA_CMOS_RTC_B,prev & ~EISA_CMOS_RTC_PERIODIC_ENABLE);
	LF(rf);
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
	u64 rf;
	
	SFI(rf);
	prev = cmos_in(0x80 | EISA_CMOS_RTC_A);
	cmos_out(0x80 | EISA_CMOS_RTC_A,(prev & 0xf0) | 0x06);//1024HZ
	prev = cmos_in(0x80 | EISA_CMOS_RTC_B);
	cmos_out(0x80 | EISA_CMOS_RTC_B,prev & ~EISA_CMOS_RTC_PERIODIC_ENABLE);//disable Periodic interrupt
	request_irq(EISA_RTC_IRQ,rtc_handle);
	while(cmos_in(EISA_CMOS_RTC_A) & EISA_CMOS_RTC_UPDATE) nop();
	cur_time.second = cmos_in(EISA_CMOS_SECOND);
	cur_time.minute = cmos_in(EISA_CMOS_SECOND);
	cur_time.hour = cmos_in(EISA_CMOS_HOUR);
	cur_time.week = cmos_in(EISA_CMOS_WEEK_DAY);
	cur_time.day = cmos_in(EISA_CMOS_MONTH_DAY);
	cur_time.month = cmos_in(EISA_CMOS_MONTH);
	cur_time.year = cmos_in(EISA_CMOS_YEAR);
	prev = cmos_in(0x80 | EISA_CMOS_RTC_B);
	LF(rf);	
	if(prev & EISA_CMOS_RTC_DATE_MODE){
		cur_time.second = PBCD2BIN(cur_time.second);
		cur_time.minute = PBCD2BIN(cur_time.minute);
		cur_time.hour = PBCD2BIN(cur_time.hour);
		cur_time.day = PBCD2BIN(cur_time.day);
		cur_time.month = PBCD2BIN(cur_time.month);
		cur_time.year = PBCD2BIN(cur_time.year);
	}
	cur_time.year += 2000;
	time2stamp(&cur_time, &kernel_time);
	mask = nock = 0;
	reg_counter(&rtc);
}

