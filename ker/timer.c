/*
	ker/timer.c
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
#include <timer.h>
#include <mm.h>
#include <asm.h>
#include <lava.h>
#include <mutex.h>
#include <kernel.h>
#include <gst.h>
#include <int.h>
#include <spinlock.h>
#include <error.h>
#include <config.h>

#define cacu_tick(time_ms)	({register u64 __t;\
	asm("movq $0xe8d4a51000,%%rcx\n\tmulq %%rcx\n\tdivq %2"\
	:"=a"(__t):"a"((u64)(time_ms)),"r"((u64)tick_time):"rcx","rdx");__t;})

#define TIMER_LIST_BUSY_BIT		0
#define TIMER_FIELD_BUSY_BIT	1
#define TIMER_LIST_BUSY			(1 << TIMER_LIST_BUSY_BIT)
#define TIMER_FIELD_BUSY		(1 << TIMER_FIELD_BUSY_BIT)

spin_optr_struct_member_bit(TimerList,struct _TIMER_,busy,TIMER_LIST_BUSY_BIT);
spin_optr_struct_member_bit(TimerField,struct _TIMER_,busy,TIMER_FIELD_BUSY_BIT);

void schedule();

static LPTIMER timer_list;//时钟链表，链表头一经初始化则不修改
static u64 ticks;
static u64 tick_time;
static int cur_counter_type;
static void (*counter_enable)();
static void (*counter_disable)();
static void (*counter_mask)();
static void (*counter_unmask)();

//抢占时钟时必须从前向后抢占，以免死锁

void counter_updata(){//中断代码，要求处理器禁止调度
	//counter_updata仅使用时钟链表中的next字段
	LPTIMER cur;
	u64 tick_count;

	ticks++;
	ID();
	LockTimerList(timer_list);
	cur = timer_list->next;
	if(cur && cur != timer_list){
		LockTimerList(cur);
		UnlockTimerList(timer_list);
		while(cur && cur != timer_list){
			if(!TryLockTimerField(cur)){
				if(!(cur->mode & 0x01) && !(ticks ^ cur->next_ticks)){
					if(cur->mode == TMR_MOD_ONCE) cur->mode = TMR_MOD_ONCE_HLT;
					else{
						if(cur->next_accu_time & 0x80000000){
							cur->next_accu_time = cur->time_ms;
							cur->start_ticks = ticks;
						}
						else cur->next_accu_time += cur->time_ms;
						tick_count = cacu_tick(cur->next_accu_time);
						cur->next_ticks = cur->start_ticks + tick_count;
					}
					UnlockTimerField(cur);
					if(cur->call_back) cur->call_back(cur,cur->data);
				}
				else UnlockTimerField(cur);
			}
			LockTimerList(cur->next);
			cur = cur->next;
			UnlockTimerList(cur->prev);
		}
	}
	UnlockTimerList(cur);
	IE();
	schedule();
}
int reg_counter(struct _COUNTER_ * counter){
	LPTIMER cur;
	u64 tick;
	u64 time;
	
	if(counter->type > cur_counter_type) return -1;
	if(cur_counter_type != COUNTER_TYPE_UNDEF){
		counter_disable();//将避免counter_updata被调用
		LockTimerList(timer_list);
		cur = timer_list->next;
		if(cur != timer_list){
			LockTimerList(cur);
			UnlockTimerList(timer_list);
			while(cur != timer_list){
				LockTimerField(cur);
				tick = cur->next_ticks - ticks;
				asm(
					"mulq %%rdx\n\t"
					"pushq %%rdx\n\t"
					"pushq %%rax\n\t"
					"divq %%rcx\n\t"
					"popq %%rax\n\t"
					"popq %%rdx\n\t"
					"pushq %%rcx\n\t"
					"movq $1000000000000,%%rcx\n\t"
					"divq %%rcx\n\t"
					"popq %%rcx\n\t"
					:"=c"(tick),"=a"(time)
					:"a"(tick_time),"c"(counter->tick_time_fs),"d"(tick):
				);
				tick++;
				cur->next_ticks = tick;
				cur->next_accu_time = time;
				cur->start_ticks = 0;
				UnlockTimerField(cur);
				LockTimerList(cur->next);
				cur = cur->next;
				UnlockTimerList(cur->prev);
			}
		}
		UnlockTimerList(cur);
		ticks = 0;
	}
	tick_time = counter->tick_time_fs;
	counter_disable = counter->disable;
	counter_enable = counter->enable;
	cur_counter_type = counter->type;
	counter_mask = counter->mask;
	counter_unmask = counter->unmask;
	counter_enable();
	return 0;
}
LPTIMER timer_alloc(u32 time,int mode,void (*proc)(LPTIMER,void*),void * data){
	LPTIMER timer;

	timer = kmalloc(sizeof(TIMER),0);
	timer->mode = mode | 0x01;
	timer->time_ms = time;
	timer->call_back = proc;
	timer->data = data;
	timer->busy = TIMER_LIST_BUSY;
	timer->gst = GST_TIMER;
	SD();//禁止调度
	if(counter_mask) counter_mask();
	LockTimerList(timer_list);
	if(timer_list->next == timer_list){
		timer_list->next = timer_list->prev = timer;
		timer->prev = timer->next = timer_list;
	}
	else{
		LockTimerList(timer_list->next);
		timer->next = timer_list->next;
		timer_list->next = timer;
		timer->prev = timer_list;
		timer->next->prev = timer;
	}
	UnlockTimerList(timer_list);
	UnlockTimerList(timer->next);
	UnlockTimerList(timer);
	if(counter_unmask) counter_unmask();
	SE();
	return timer;
}
int timer_free(LPTIMER timer){
#ifdef CHECK
	if(!timer) return ERR_INVAILD_PTR;
	if(timer->gst != GST_TIMER) return ERR_BAD_TYPE;
#endif
	SD();
	if(counter_mask) counter_mask();
	LockTimerField(timer);
	timer->gst = GST_UNDEF;
	while(TryLockTimerList(timer->prev)) nop();
	LockTimerList(timer);
	timer->prev->next = timer->next;
	UnlockTimerList(timer->prev);
	LockTimerList(timer->next);
	timer->next->prev = timer->prev;
	UnlockTimerList(timer->next);
	if(counter_unmask) counter_unmask();
	SE();
	timer->gst = GST_UNDEF;
	kfree(timer);
	return 0;
}
int timer_reset(LPTIMER timer,u32 time,int mode,void (*proc)(LPTIMER,void*),void * data){
#ifdef CHECK
	if(!timer) return ERR_INVAILD_PTR;
	if(timer->gst != GST_TIMER) return ERR_BAD_TYPE;
#endif
	if(counter_mask) counter_mask();
	while(TryLockTimerField(timer)) if(timer->gst != GST_TIMER) return -1;
	timer->mode = mode | 0x01;
	timer->time_ms = time;
	timer->call_back = proc;
	timer->data = data;
	UnlockTimerField(timer);
	if(counter_unmask) counter_unmask();
	return 0;
}
int timer_start(LPTIMER timer){
#ifdef CHECK
	if(!timer) return ERR_INVAILD_PTR;
	if(timer->gst != GST_TIMER) return ERR_BAD_TYPE;
#endif
	if(counter_mask) counter_mask();
	while(TryLockTimerField(timer)) if(timer->gst != GST_TIMER) return -1;
	timer->mode &= 0xfffe;
	timer->start_ticks = ticks;
	timer->next_accu_time = timer->time_ms;
	timer->next_ticks = timer->start_ticks + cacu_tick(timer->time_ms);
	UnlockTimerField(timer);
	if(counter_unmask) counter_unmask();
	return 0;
}
int timer_stop(LPTIMER timer){
#ifdef CHECK
	if(!timer) return ERR_INVAILD_PTR;
	if(timer->gst != GST_TIMER) return ERR_BAD_TYPE;
#endif
	if(counter_mask) counter_mask();
	while(TryLockTimerField(timer)) if(timer->gst != GST_TIMER) return -1;
	timer->mode |= 0x0001;
	UnlockTimerField(timer);
	if(counter_unmask) counter_unmask();
	return 0;
}
void timer_init(){
	timer_list = kmalloc(sizeof(TIMER),0);
	timer_list->mode = TMR_MOD_PRDC_HLT;
	timer_list->prev = timer_list->next = timer_list;
	timer_list->gst = GST_TIMER;
	timer_list->busy = 0;
	cur_counter_type = COUNTER_TYPE_UNDEF;
	counter_enable = counter_disable = NULL;
	counter_mask = counter_unmask = NULL;
}



