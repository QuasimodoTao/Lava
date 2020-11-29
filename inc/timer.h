//Lava OS
//PageFault
//20-03-14-22-46

#ifndef _TIMER_H_
#define _TIMER_H_

#include <stddef.h>

#define TMR_MOD_ONCE		0
#define TMR_MOD_ONCE_HLT	1
#define TMR_MOD_PRDC		2
#define TMR_MOD_PRDC_HLT	3

#define COUNTER_TYPE_HPET	16
#define COUNTER_TYPE_PIT	17
#define COUNTER_TYPE_RTC	18
#define COUNTER_TYPE_APIC	19
#define COUNTER_TYPE_UNDEF	256

struct _COUNTER_ {
	void(*enable)();
	void(*disable)();
	void(*unmask)();
	void(*mask)();
	u64 tick_time_fs;
	int type;//HPET > PIT > RTC > APIC
};

typedef struct _TIMER_ {
	volatile int gst;
	short busy;
	volatile short mode;
	u32 time_ms,next_accu_time;//申请的毫秒数,已经过的毫秒数
	u64 next_ticks;//下一个tick计数
	u64 start_ticks;//申请时的tick计数
	void (*call_back)(struct _TIMER_ *,void *);//回调函数，必须是简单的函数
	volatile struct _TIMER_ * prev;
	volatile struct _TIMER_ * next;
	void * data;
} TIMER, *LPTIMER;

int timer_reset(LPTIMER timer,u32 time,int mode,void (*proc)(LPTIMER,void*),void * data);
LPTIMER timer_alloc(u32 time,int mode,void (*proc)(LPTIMER,void*),void * data);
int timer_free(LPTIMER timer);
int timer_start(LPTIMER timer);
int timer_stop(LPTIMER timer);
int reg_counter(struct _COUNTER_ * counter);

#endif