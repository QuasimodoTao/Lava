/*
	ker/schedule.c
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

#include <lava.h>
#include <stddef.h>
#include <stdio.h>
#include <kernel.h>
#include <mm.h>
#include <asm.h>
#include <arch.h>
#include <timer.h>
#include <config.h>
#include <int.h>
#include <gst.h>
#include <mutex.h>
#include <semaphore.h>
#include <error.h>

void thread_exit(u64 rsp,void * entry,LPTHREAD thread);
LPTHREAD switch_task_b(u64 * osp,u64 nsp,LPTHREAD thread);

static LPPROCESS process_list;
static volatile u32 process_count;
static volatile u32 thread_count = 0;
static int list_busy = 0;
static volatile int need_updata_schedule = 0;
static volatile int updata_count = 0;
static volatile unsigned int current_thread_max = 0;
static volatile int freshing_cur_max = 0; 
static volatile int need_destory = 0;

#define thread_list			((struct _THREAD_ **)TLB)
#define PROCESS_LIST_BUSY_BIT	0
spin_optr_def_bit(ProcessGlobalList,&list_busy,PROCESS_LIST_BUSY_BIT);

static int find_first_empty(){
	u64 page;
	short cur, real, i = 0;
	
	CHECK_IF();
	if(freshing_cur_max){//如果已经在更新了，就不往里面凑了
		i = current_thread_max - PAGE_SIZE/sizeof(void*);//已经在更新说明前面没有空的位置了
		while(freshing_cur_max);
	}
	while(1){
		cur = current_thread_max;//获取当前的最大值
		for(;i < cur;i++)//搜索空位
			if(!thread_list[i] && !cmpxchg8b(thread_list + i,NULL,1,NULL))return i;//抢占空位,成功返回值
		freshing_cur_max = 1;//设置正在更新标志
		if(cmpxchg4b(&current_thread_max,cur,cur + PAGE_SIZE/sizeof(void*),NULL)){//抢占更新权
			while(freshing_cur_max);//抢占失败则等待
			if(current_thread_max == cur) return -1;//前后值相等，说明没有更多空位了
			continue;
		}
		real = cur;//
		cur += PAGE_SIZE/sizeof(void*);
		if(cur > MAX_THREAD){
			xaddd(&current_thread_max,-(PAGE_SIZE/sizeof(void*)));//恢复值
			freshing_cur_max = 0;
			return -1;
		}
		page = get_free_page(0,0,0);//映射页面
		memset(PADDR2V(page),0,PAGE_SIZE);
		put_page(page,NULL,thread_list+ real);
		freshing_cur_max = 0;
	}
}
//将进程从进程链表中移出
void remove_process(LPPROCESS process){
	CHECK_IF();	
	LockProcessGlobalList();//D0
	process_count--;
	if(process_list == process) process_list = process->next;
	if(process_count == 1){
		LockProcessLocalList(process);//D1
		LockProcessLocalList(process->next);//D2
		process_list->prev = process_list->next = process_list;
		UnlockProcessGlobalList();//E0
		UnlockProcessLocalList(process->next);//E2
		UnlockProcessLocalList(process);//E1
	}
	else{
		while(TryLockProcessLocalList(process->prev));//D1
		UnlockProcessGlobalList();//E0
		LockProcessLocalList(process);//D2
		LockProcessLocalList(process->next);//D3
		process->prev->next = process->next;
		process->next->prev = process->prev;
		UnlockProcessLocalList(process->prev);//E1
		UnlockProcessLocalList(process->next);//E3
		UnlockProcessLocalList(process);//E2
	}
}
//将进程插入到链表中
void insert_process(LPPROCESS process){
	LPPROCESS _process;
	CHECK_IF();
	LockProcessGlobalList();
	process_count++;
	if(process_count == 1){
		process_list = process;
		LockProcessLocalList(process);
		UnlockProcessGlobalList();
		process->prev = process->next = process;
		UnlockProcessLocalList(process);
	}
	else{
		LockProcessLocalList(process_list);
		_process = process_list;
		if(process_count == 2){
			LockProcessLocalList(process);
			_process->prev = _process->next = process;
			process->prev = process->next = _process;
			UnlockProcessGlobalList();
			UnlockProcessLocalList(process->next);
			UnlockProcessLocalList(process);
		}
		else{
			LockProcessLocalList(_process->next);
			LockProcessLocalList(process);
			UnlockProcessGlobalList();
			process->next = _process->next;
			process->prev = _process;
			_process->next->prev = process;
			_process->next = process;
			UnlockProcessLocalList(_process);
			UnlockProcessLocalList(process->next);
			UnlockProcessLocalList(process);
		}
	}
}
//将线程从进程的线程链表中移出
void remove_process_thread(LPPROCESS process,LPTHREAD thread){
	CHECK_IF();
	LockProcessThread(process);//D0
	process->thread_count--;
	if(!process->thread_count){
		process->thread = NULL;
		UnlockProcessThread(process);//E0
	}
	else {
		if(process->thread == thread) process->thread = thread->next;
		if(process->thread_count == 1){
			LockThreadPList(thread);//D1
			LockThreadPList(thread->next);//D2
			process->thread->prev = process->thread->next = process->thread;
			UnlockProcessThread(process);//E0
			UnlockThreadPList(thread->next);//E2
			UnlockThreadPList(thread);//E1
		}
		else{
			while(TryLockThreadPList(thread->prev));//D1
			UnlockProcessThread(process);//E0
			LockThreadPList(thread);//D2
			LockThreadPList(thread->next);//D3
			thread->prev->next = thread->next;
			thread->next->prev = thread->prev;
			UnlockThreadPList(thread->prev);//E1
			UnlockThreadPList(thread->next);//E3
			UnlockThreadPList(thread);//E2
		}
	}
}
//将线程插入到进程的线程链表中
void insert_process_thread(LPPROCESS process,LPTHREAD thread){
	LPTHREAD _thread;
	CHECK_IF();
	LockProcessThread(process);//D0
	process->thread_count++;
	thread->father = process;
	if(process->thread_count == 1){
		process->thread = thread;
		LockThreadPList(thread);//D1
		UnlockProcessThread(process);//E0
		process->thread = thread->prev = thread->next = thread;
		UnlockThreadPList(thread);//E1
	}
	else{
		LockThreadPList(process->thread);//D1
		_thread = process->thread;
		if(process->thread_count == 2){
			LockThreadPList(thread);//D2
			_thread->prev = _thread->next = thread;
			thread->prev = thread->next = _thread;
			UnlockProcessThread(process);//E0
			UnlockThreadPList(thread->next);//E1
			UnlockThreadPList(thread);//E2
		}
		else{
			LockThreadPList(_thread->next);//D2
			LockThreadPList(thread);//D3
			UnlockProcessThread(process);//E0
			thread->next = _thread->next;
			thread->prev = _thread;
			_thread->next->prev = thread;
			_thread->next = thread;
			UnlockThreadPList(_thread);//E1
			UnlockThreadPList(thread->next);//E2
			UnlockThreadPList(thread);//E3
		}
	}
}

//must musk interrupt before call
void schedule_imm(){
	LPTHREAD old,new;
	LPTHREAD * next;
	u64 * pdb;
	int cur_max;
	int time = 0;
	LPPROCESS process;
	
	CHECK_IF();
	lock_btr_private(flags,CPU_FLAGS_NEED_SCHEDULE);
	write_private_dword(cpu_time,CPU_TIME);
	while(freshing_cur_max);
	cur_max = current_thread_max;
	old = GetCurThread();
	next = thread_list + old->solt + 1;
	while(1){
		if(next >= thread_list + cur_max){
			next = thread_list;
			time++;
			if(time > 4) return;
		}
		new = *next;

		if(!new || (u64)new == 1 || 
			new->flag != TF_ACTIVE || 
			TryLockThreadStatus(new)) {
			next++;
			continue;
		}
		new->processor = GetCPUId();
		pdb = PADDR2V(get_cr3());
		if(new->father != GetCurProcess()){
			SetCurProcess(new->father);
			memcpy(pdb,new->father->pdbe,255 * 8);
		}
		pdb[USER_STACK_P4E_INDEX] = new->user_stack_p4e;
		fpg();
		write_private_dword(cpu_time,CPU_TIME);
		SetCurThread(new);
		if(suppose_sysenter) wrmsr(IA32_SYSENTER_ESP,new->ker_ent_rsp);//切换内核堆栈入口
		write_private_dword(TSS.reg[RSPL(0)],new->ker_ent_rsp & 0x00000000ffffffff);//切换内核堆栈入口
		write_private_dword(TSS.reg[RSPH(0)],new->ker_ent_rsp >> 32);//切换内核堆栈入口
		//printk("cpu %d,Switch to %d.\n",GetCPUId(),new->id);
		old = switch_task_b(&(old->rsp),new->rsp,old);
		UnlockThreadStatus(old);
		if(old->flag == TF_DESTORY){
			thread_list[old->solt] = NULL;
			free_stack(old->stack);
			kfree(old);
		}
		return;
	}
}
//must musk interrupt before call
static int schedule_update_recive(){
	int i;

	CHECK_IF();
	if((i = read_private_dword(cpu_time)) > 0) write_private_dword(cpu_time,i-1);
	else lock_bts_private(flags,CPU_FLAGS_NEED_SCHEDULE);
	if(xaddd(&updata_count,-1) == 1)need_updata_schedule = 0;
	return 0;
}
//must musk interrupt before call
static int schedule_request_recive(){
	CHECK_IF();
	lock_bts_private(flags,CPU_FLAGS_NEED_SCHEDULE);
	return 0;
}
//must musk interrupt before call
void schedule_update(){
	int i;

	CHECK_IF();
	if(cmpxchg4b(&updata_count,0,process_count,NULL)) send_ipi(CPU_TIME_UPDATA_IPI,0,1,IPIM_ALL_EX);
	if((i = read_private_dword(cpu_time)) > 0) write_private_dword(cpu_time,i-1);
	else lock_bts_private(flags,CPU_FLAGS_NEED_SCHEDULE);
	if(xaddd(&updata_count,-1) == 1)need_updata_schedule = 0;
	return 0;
}
//must musk interrupt before call
void schedule_request(){
	CHECK_IF();
	send_ipi(CPU_SCHEDULE_REQUEST_IPI,0,1,IPIM_ALL_EX);
}
void __attribute__((noreturn)) exit(int code){
	LPTHREAD thread;
	LPPROCESS process;
	thread = GetCurThread();
	free_area(thread,0x00007f8000000000,0x0000007ffffff000);
	process = thread->father;
	process->ret = code;
	cli();
	remove_process_thread(process,thread);
	if(!process->thread_count){
		free_page_table(process);
		if(process->image_name) kfree(process->image_name);
		if(process->by_shell) process->f_thread->flag = TF_ACTIVE;
		else {
			remove_process(process);
			kfree(process);
		}
	}
	thread->flag = TF_DESTORY;
	thread_list[thread->solt] = NULL;
	schedule_imm();
}
int kill(LPTHREAD thread){
	if(!thread || thread == GetCurThread()) exit(ERR_BE_DESTORY);
	if(!addr_vaild(NULL,thread) || thread->gst != GST_THREAD) return ERR_INVAILD_PTR;
	thread->need_destory = 1;
	send_ipi(CPU_SCHEDULE_IPI,thread->processor,0,IPIM_FIXED);
	return 0;
}
int wake_up(LPTHREAD thread){
	LPTHREAD _thread;
	int ret;

	if(!thread || !addr_vaild(NULL,thread) || thread->gst != GST_THREAD) return ERR_INVAILD_PTR;
	thread->flag = TF_ACTIVE;
	schedule_request();
	return 0;
}
static void wait_call_back(LPTIMER timer,LPTHREAD thread){
	thread->wait_state = TFW_OUT_OF_TIME;
	thread->flag = TF_ACTIVE;
	schedule_request();
}
int wait(int msecond){
	LPTHREAD thread;
	LPTIMER timer;
	int ret;
	int ie;

	thread = GetCurThread();
	ie = IE();
	cli();
	if(msecond){
		timer = timer_alloc(msecond,TMR_MOD_ONCE,wait_call_back,thread);
		timer_start(timer);
		thread->flag = TF_BLOCK;
		schedule_imm();
		timer_free(timer);
	}
	else schedule_imm();
	if(ie) sti();
	if(thread->need_destory) exit(ERR_BE_DESTORY);
	return 0;
}
int syscall_wait(msecond){
	return wait(msecond);
}
int destory_mutex(struct _MUTEX_ * mutex){
	struct _THREAD_ * thread;
	
	LockMutex(mutex);//抢占信号量
	while(thread = mutex->wait.t){//逐个唤醒正在等待的线程
		thread->wait_state = TFW_RESOURCE_DESTORY;
		thread->flag = TF_ACTIVE;
		schedule_request();
	}
	while(thread = mutex->release.t){
		thread->wait_state = TFW_RESOURCE_DESTORY;
		thread->flag = TF_ACTIVE;
		schedule_request();
	}
	if(mutex->flags & MUTEX_ALLOC) kfree(mutex);//释放信号量
	return 0;
}
struct _MUTEX_ * create_mutex_ex(int locked,struct _MUTEX_ * mutex){//创建一个信号量
	if(!mutex){//若se不存在则申请内存，并标注是被申请的
		mutex = kmalloc(sizeof(struct _MUTEX_),0);
		mutex->flags = MUTEX_ALLOC;
	}
	else mutex->flags = 0;
	mutex->gst = GST_MUTEX;//设置全局标志
	if(locked) mutex->flags |= MUTEX_LOCKED;
	mutex->wait.p = mutex->wait.t = NULL;
	mutex->release.p = mutex->release.t = NULL;
	return mutex;
}
static void mutex_timer_call_back(LPTIMER timer,LPTHREAD thread){//线程等待超时时则调用此函数，唤醒线程
	thread->wait_state = TFW_OUT_OF_TIME;
	thread->flag = TF_ACTIVE;
	schedule_request();
}
int wait_mutex(struct _MUTEX_ * mutex,int time){//抢占资源
	LPTHREAD thread;
	LPTHREAD * prev = NULL;
	volatile LPTHREAD next = NULL;
	LPTIMER timer;
	int ie;

	if(!mutex) return ERR_INVAILD_PTR;//非法指针
	thread = GetCurThread();
	ie = IE();
	cli();//关中断
	LockMutex(mutex);//抢占信号量
	if(mutex->wait.t || mutex->flags & MUTEX_LOCKED){//若资源不足或已有线程在等待（服从先到先服务）
		if(!time){//不等待则直接返回资源不足
			UnlockMutex(mutex);
			if(ie) sti();
			return ERR_RESOURCE_BUSY;
		}
		thread->flag = TF_BLOCK;
		prev = mutex->wait.p;
		mutex->wait.p = & next;
		if(prev) * prev = thread;
		if(!mutex->wait.t) mutex->wait.t = thread;
		thread->wait_state = 0;
		UnlockMutex(mutex);
		if(ie) sti();
		if(time != -1){//有限等待则重启计时器
			timer = timer_alloc(time,TMR_MOD_ONCE,mutex_timer_call_back,thread);
			timer_start(timer);
		}
		cli();
		schedule_imm();//调度
		if(ie) sti();
		if(time != -1) timer_free(timer);
		if(thread->wait_state == TFW_RESOURCE_DESTORY) {//若信号量正在销毁，则返回 无效的指针//if mutex is killing, just return is enough;
			mutex->wait.t = next;
			return ERR_INVAILD_PTR;
		}
		cli();
		LockMutex(mutex);
		if(thread->wait_state == TFW_OUT_OF_TIME) {//wake up because of out of time;
	///		//Might i am not the first one	
			if(thread == mutex->wait.t){//if i am first one;
				mutex->wait.t = next;
				if(!next) mutex->wait.p = NULL;
			}
			else{//if i am not the first one,remove myself from wait list is complex.
				*prev = next;
				if(!next) mutex->wait.p = prev;
				if(prev) *prev = next;
			}
			UnlockMutex(mutex);
			if(ie) sti();
			return ERR_RESOURCE_BUSY;
		}
	///	//wake up because resource is enough;
		mutex->wait.t = next;
		if(!next) mutex->wait.p = NULL;
	}
	mutex->flags |= MUTEX_LOCKED;
	if(mutex->release.t){
		thread = mutex->release.t;
		mutex->release.t = next;
		if(!next) mutex->release.p = NULL;
	}
	UnlockMutex(mutex);
	if(ie) sti();
	if(thread) {
		thread->flag = TF_ACTIVE;	
		schedule_request();
	}
	return 0;
}
int release_mutex(struct _MUTEX_ * mutex,int time){
	LPTHREAD thread;
	LPTHREAD * prev = NULL;
	volatile LPTHREAD next = NULL;
	LPTIMER timer;
	int ie;
	
///	if(heckArgument()) return ERR;
	if(!mutex) return ERR_INVAILD_PTR;
	ie = IE();
	cli();
	LockMutex(mutex);
	if(mutex->flags & MUTEX_LOCKED || mutex->release.t){
		if(!time){
			UnlockMutex(mutex);
			if(ie) sti();
			return ERR_RESOURCE_BUSY;
		}
		thread = GetCurThread();
		prev = mutex->release.p;
		thread->flag = TF_BLOCK;
		mutex->release.p = & next;
		if(prev) * prev = thread;
		if(!mutex->release.t) mutex->release.t = thread;
		thread->wait_state = 0;
		UnlockMutex(mutex);
		if(ie) sti();
		if(time != -1){//有限等待则重启计时器
			timer = timer_alloc(time,TMR_MOD_ONCE,mutex_timer_call_back,thread);
			timer_start(timer);
		}
		cli();
		schedule_imm();
		if(ie) sti();
		if(time != -1) timer_free(timer);
		if(thread->wait_state == TFW_RESOURCE_DESTORY) {
			mutex->release.t = next;
			return ERR_INVAILD_PTR;
		}
///		if(IsKilling(mutex)) {//if mutex is killing, just return is enough;
///			if(time != INFINE) StopTimer();
///			return ERR;
///		}
		thread = GetCurThread();
		cli();
		LockMutex(mutex);
		if(thread->wait_state == TFW_OUT_OF_TIME) {//wake up because of out of time;
	///		//Might i am not the first one	
			if(thread == mutex->release.t){//if i am first one;
				mutex->release.t = next;
				if(!next) mutex->release.p = NULL;
			}
			else{//if i am not the first one,remove myself from release list is complex.
				*prev = next;
				if(!next) mutex->release.p = prev;
				if(prev) *prev = next;
			}
			UnlockMutex(mutex);
			if(ie) sti();
			return ERR_RESOURCE_BUSY;
		}
	///	//wake up because resource is enough;
		mutex->release.t = next;
		if(!next) mutex->release.p = NULL;
	}
///	}
	mutex->flags &= ~MUTEX_LOCKED;
	if(mutex->wait.t){
		thread = mutex->wait.t;
		mutex->wait.t = next;
		if(!next) mutex->wait.p = NULL;
	}
	UnlockMutex(mutex);
	if(ie) sti();
	if(thread) {
		thread->flag = TF_ACTIVE;
		schedule_request();
	}
	return 0;
}
int destory_semaphore(struct _SEMAPHORE_ * se){
	struct _THREAD_ * thread,*thread2;
	
	LockSemaphore(se);//抢占信号量
	se->flags |= SE_KILLING;//设置销毁标志
	while(thread = se->wait.p){//逐个唤醒正在等待的线程
		thread->wait_state = TFW_RESOURCE_DESTORY;
		thread->flag = TF_ACTIVE;
		schedule_request();
	}
	while(thread = se->release.p){
		thread->wait_state = TFW_RESOURCE_DESTORY;
		thread->flag = TF_ACTIVE;
		schedule_request();
	}
	if(se->flags & SE_ALLOC) kfree(se);//释放信号量
	return 0;
}
struct _SEMAPHORE_ * create_semaphore_ex(int max,int cur,struct _SEMAPHORE_ * se){//创建一个信号量
	if(!se){//若se不存在则申请内存，并标注是被申请的
		se = kmalloc(sizeof(struct _SEMAPHORE_),0);
		se->flags = SE_ALLOC;
	}
	else se->flags = 0;
	se->gst = GST_SEMAPHORE;//设置全局标志
	se->cur = cur;//设置当前值
	se->max = max;//设置最大值
	se->wait.p = se->wait.t = NULL;//初始化阻塞列表
	se->release.p = se->release.t = NULL;
	return se;
}
static void semaphore_timer_call_back(LPTIMER timer,LPTHREAD thread){//线程等待超时时则调用此函数，唤醒线程
	thread->wait_state = TFW_OUT_OF_TIME;
	thread->flag = TF_ACTIVE;
	schedule_request();
}
int wait_semaphore(int val,struct _SEMAPHORE_ * se,int time){//抢占资源
	LPTHREAD thread;
	LPTHREAD * prev = NULL;
	LPTHREAD next = NULL;
	LPTIMER timer;
	int ie;

	if(!se) return ERR_INVAILD_PTR;//非法指针
	if(!val) return 0;//不抢占任何资源则直接返回
	if(val > se->max) return ERR_OUT_OF_RANGE;//抢占超过总数的资源
	ie = IE();
	cli();//关中断
	LockSemaphore(se);//抢占信号量
	if(se->wait.t || se->cur < val){//若资源不足或已有线程在等待（服从先到先服务）
		if(!time){//不等待则直接返回资源不足
			UnlockSemaphore(se);
			if(ie) sti();
			return ERR_RESOURCE_BUSY;
		}
		thread = GetCurThread();
		thread->flag = TF_BLOCK;
		prev = se->wait.p;
		se->wait.p = &next;
		if(prev) * prev = thread;
		if(!se->wait.t) se->wait.t = thread;   
		thread->semaphore_val = val;
		thread->wait_state = 0;
		UnlockSemaphore(se);
		if(ie) sti();
		if(time != -1){//有限等待则重启计时器
			timer = timer_alloc(time,TMR_MOD_ONCE,semaphore_timer_call_back,thread);
			timer_start(timer);
		}
		cli();
		schedule_imm();//调度
		if(ie) sti();
		if(time != -1) timer_free(timer);
		if(thread->wait_state == TFW_RESOURCE_DESTORY) {//若信号量正在销毁，则返回 无效的指针
			se->wait.t = next;
			return ERR_INVAILD_PTR;
		}
		cli();
		LockSemaphore(se);
		if(thread->wait_state == TFW_OUT_OF_TIME) {
			if(thread == se->wait.t){
				se->wait.t = next;
				if(!next) se->wait.p = NULL;
			}
			else{
				*prev = next;
				if(!next) se->wait.p = prev;
				if(prev) *prev = next;
			}
			UnlockSemaphore(se);
			if(ie) sti();
			return ERR_RESOURCE_BUSY;
		}
		se->wait.t = next;
		if(!next) se->wait.p = NULL;
	}
	se->cur -= val;
	if(se->release.t && se->max - se->cur >= se->release.t->semaphore_val){
		thread = se->release.t;
		se->release.t = next;
		if(!next) se->release.p = NULL;
		thread->flag = TF_ACTIVE;
		schedule_request();
	}
	if(se->wait.t && se->cur >= se->wait.t->semaphore_val) {
		thread = se->wait.t;
		se->wait.t = next;
		if(!next) se->wait.p = NULL;
		thread->flag = TF_ACTIVE;
		schedule_request();
	}
	UnlockSemaphore(se);
	if(ie) sti();
	return 0;
}
int release_semaphore(int val,struct _SEMAPHORE_ * se,int time){
	LPTHREAD thread;
	LPTHREAD * prev = NULL;
	volatile LPTHREAD next = NULL;
	LPTIMER timer;
	int ie;
	
	if(!se) return ERR_INVAILD_PTR;
	if(!val) return 0;
	if(val > se->max) return ERR_OUT_OF_RANGE;
	ie = IE();
	cli();
	LockSemaphore(se);
	if(se->cur + val > se->max || se->release.t){
		if(!time){
			UnlockSemaphore(se);
			if(ie) sti();
			return ERR_RESOURCE_BUSY;
		}
		thread = GetCurThread();
		thread->flag = TF_BLOCK;
		prev = se->release.p;
		se->release.p = &next;
		if(prev) * prev = thread;
		if(!se->release.t) se->release.t = thread;   
		thread->semaphore_val = val;
		thread->wait_state = 0;
		UnlockSemaphore(se);
		if(ie) sti();
		if(time != -1){//有限等待则重启计时器
			timer = timer_alloc(time,TMR_MOD_ONCE,semaphore_timer_call_back,thread);
			timer_start(timer);
		}
		cli();
		schedule_imm();
		if(ie) sti();
		if(time != -1) timer_free(timer);
		if(thread->wait_state == TFW_RESOURCE_DESTORY) {//若信号量正在销毁，则返回 无效的指针
			se->release.t = next;
			return ERR_INVAILD_PTR;
		}
		cli();
		LockSemaphore(se);
		if(thread->wait_state == TFW_OUT_OF_TIME) {
			if(thread == se->release.t){
				se->release.t = next;
				if(!next) se->release.p = NULL;
			}
			else{
				*prev = next;
				if(!next) se->release.p = prev;
				if(prev) *prev = next;
			}
			UnlockSemaphore(se);
			if(ie) sti();
			return ERR_RESOURCE_BUSY;
		}
		se->release.t = next;
		if(!next) se->release.p = NULL;
	}
	se->cur += val;
	if(se->wait.t && se->max - se->cur >= se->wait.t->semaphore_val){
		thread = se->wait.t;
		se->wait.t = next;
		if(!next) se->wait.p = NULL;
		thread->flag = TF_ACTIVE;
		schedule_request();
	}
	if(se->release.t && se->cur >= se->release.t->semaphore_val) {
		thread = se->release.t;
		se->release.t = next;
		if(!next) se->release.p = NULL;
		thread->flag = TF_ACTIVE;
		schedule_request();
	}
	UnlockSemaphore(se);
	if(ie) sti();
	return 0;
}
static void thread_entry(int (*entry)(void*),LPTHREAD old){
	LPSTREAM image;
	LPPROCESS process;
	LPTHREAD self;

	sti();
	if(old) {
		UnlockThreadStatus(old);
		if(old->flag == TF_DESTORY){
			free_stack(old->stack);
			kfree(old);
		}
	}
	self = GetCurThread();
	if(!entry){
		printk("Bug:bad entry.\n");
		stop();
	}
	if(entry) exit(entry(self->argv));
	asm("ud2");
	//process = GetCurProcess();
	//image = open(process->image_name,0);
	//entry = load_image(image);
	//back_to_user_mode(entry,0x202,USER_INIT_RSP,USER_CS_64);
}
LPPROCESS create_process(const wchar_t * name,void * argv){
	LPPROCESS _process;
	LPPROCESS process;
	LPTHREAD thread;
	LPTHREAD _thread;
	u64 * stack;
	size_t name_len;
	wchar_t * _name,*__name,*___name;
	int ie;
	
	{//initlize process data
		process = kmalloc(sizeof(PROCESS),0);
		memset(process,0,sizeof(PROCESS));
		process->gst = GST_PROCESS;
		process->id = xaddd(&process_count,1);
		//process->thread_count = 0;//uesless
		//process->busy = 0;//useless
		process->father = GetCurProcess();
		name_len = wcslen(name);
		process->image_name = _name = kmalloc(sizeof(wchar_t) * name_len + sizeof(wchar_t),0);
		wmemcpy(_name,name,name_len * sizeof(wchar_t));
		_name[name_len] = 0;
		__name = wcsrchr(_name,L'/');
		___name = wcsrchr(_name,L'\\');
		process->short_name = __name > ___name ? __name : ___name;
		if(!process->short_name) process->short_name = _name;
	}
	{//initlize thread data
		thread = kmalloc(sizeof(THREAD),0);
		memset(thread,0,sizeof(THREAD));
		thread->gst = GST_THREAD;
		thread->id = xaddd(&thread_count,1);
		thread->processor = -1;
		thread->argv = argv;
		thread->stack = stack = alloc_stack();
		stack = stack + STACK_SIZE/sizeof(u64) - 12;
		thread->rsp = (u64)stack;
		thread->ker_ent_rsp = thread->rsp + 88;
		stack[0] = 0;//argument,rcx
		stack[1] = 0;//rbx
		stack[2] = 0;//rbp
		stack[3] = 0;//rsi
		stack[4] = 0;//rdi
		stack[5] = 0;//r12
		stack[6] = 0;//r13
		stack[7] = 0;//r14
		stack[8] = 0;//r15
		stack[9] = thread_entry;
		thread->father = process;
	}
	put_page(0x1000 | PAGE_EXIST | PAGE_USER,thread,0x00007ffffffff000);
	allocate_area(thread,0x00007f8000000000,0x0000007ffffff000,PAGE_WRITE | PAGE_USER);
	ie = IE();
	cli();
	insert_process_thread(process,thread);
	insert_process(process);
	thread->solt = find_first_empty();
	if(ie) sti();
	thread_list[thread->solt] = thread;
	return process;
}
LPTHREAD create_thread(LPPROCESS process,int (*entry)(void*),void * argv){
	LPTHREAD thread,_thread;
	u64 * stack;
	int ie;

	if(!process) process = GetCurProcess();
	{//initlize thread status.
		thread = kmalloc(sizeof(THREAD),0);
		memset(thread,0,sizeof(THREAD));
		thread->gst = GST_THREAD;
		thread->processor = -1;
		thread->argv = argv;
		thread->stack = stack = alloc_stack();
		stack = stack + STACK_SIZE/sizeof(u64) - 12;
		thread->rsp = (u64)stack;
		thread->ker_ent_rsp = thread->rsp + 88;
		stack[0] = entry;//argument,rcx
		stack[1] = 0;//rbx
		stack[2] = 0;//rbp
		stack[3] = 0;//rsi
		stack[4] = 0;//rdi
		stack[5] = 0;//r12
		stack[6] = 0;//r13
		stack[7] = 0;//r14
		stack[8] = 0;//r15
		stack[9] = thread_entry;
		thread->id = xaddd(&thread_count,1);
		thread->father = process;
	}
	put_page(0x1000 | PAGE_EXIST | PAGE_USER,thread,0x00007ffffffff000);
	allocate_area(thread,0x00007f8000000000,0x0000007ffffff000,PAGE_WRITE | PAGE_USER);
	ie = IE();
	cli();
	insert_process_thread(process,thread);
	thread->solt = find_first_empty();
	if(ie) sti();
	//printk("create %P.",thread);
	thread_list[thread->solt] = thread;
	return thread;
}
int shell(const wchar_t * name,void * argv){
	LPPROCESS process;
	LPTHREAD thread;
	int ret;
	int ie;
	
	process = create_process(name,argv);
	ie = IE();
	cli();
	process->by_shell = 1;
	thread = GetCurThread();
	thread->flag = TF_BLOCK;
	schedule_imm();
	if(ie) sti();
	ret = process->ret;
	if(process->image_name) kfree(process->image_name);
	kfree(process);
	if(thread->need_destory) exit(ERR_BE_DESTORY);
	return ret;
}
void __attribute__((noreturn)) schedule_init_ap(int (*entry)(void*),void * argv){
	LPTHREAD thread,_thread;
	LPPROCESS process;
	u64 * stack;
	u64 __rsp;
	
	CHECK_IF();
	{//initlize thread status.
		thread = kmalloc(sizeof(THREAD),0);
		memset(thread,0,sizeof(THREAD));
		thread->solt = find_first_empty();
		thread->gst = GST_THREAD;
		thread->flag = TF_ACTIVE;
		thread->processor = GetCPUId();
		thread->argv = argv;
		thread->id = xaddd(&thread_count,1);
		thread->stack = stack = alloc_stack();
		__rsp = stack = stack + STACK_SIZE/sizeof(u64) - 8;
		thread->ker_ent_rsp = __rsp + 56;
		stack[0] = thread_entry;
	}
	{//search and lock process 0.
		LockProcessGlobalList();
		process = process_list;
		LockProcessLocalList(process);
		UnlockProcessGlobalList();
		if(process->next != process){
			while(1){
				if(!process->id) break;
				LockProcessLocalList(process->next);
				process = process->next;
				UnlockProcessLocalList(process->prev);
			}
		}
		UnlockProcessLocalList(process);
	}
	thread_list[thread->solt] = thread;
	insert_process_thread(process,thread);
	thread->father = process;
	SetCurProcess(process);
	SetCurThread(thread);
	LockThreadStatus(thread);
	thread_exit(__rsp,entry,NULL);
}
void __attribute__((noreturn)) schedule_init(int (*entry)(void*),void * argv){
	LPPROCESS process;
	LPTHREAD thread;
	u64 * stack;
	u64 __rsp;
	
	CHECK_IF();
	//enable other processor(s) to schedule
	request_ipi(CPU_TIME_UPDATA_IPI,schedule_update_recive);
	request_ipi(CPU_SCHEDULE_IPI,NULL);
	request_ipi(CPU_SCHEDULE_REQUEST_IPI,schedule_request_recive);
	{//process 0
		process = kmalloc(sizeof(PROCESS),0);
		memset(process,0,sizeof(PROCESS));
		process->gst = GST_PROCESS;
	}
	{//thread 0
		thread = kmalloc(sizeof(THREAD),0);
		memset(thread,0,sizeof(THREAD));
		thread->solt = find_first_empty();
		thread->gst = GST_THREAD;
		thread->father = process;
		thread->flag = TF_ACTIVE;
		thread->argv = argv;
		thread->stack = stack = alloc_stack();
		__rsp = stack = stack + STACK_SIZE/sizeof(u64) - 8;
		thread->ker_ent_rsp = __rsp + 56;
		stack[0] = thread_entry;
	}
	thread_list[thread->solt] = thread;
	thread_count = 1;
	process_list = process;
	process_count = 1;
	process->thread = thread;
	process->thread_count = 1;
	SetCurProcess(process);
	SetCurThread(thread);
	write_private_dword(cpu_time,CPU_TIME);
	LockThreadStatus(thread);
	thread_exit(__rsp,entry,NULL);
}

