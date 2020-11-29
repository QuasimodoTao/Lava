//Lava OS
//PageFault
//20-03-20-22-53

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
static LPTHREAD killing_thread = NULL;
static volatile int need_updata_schedule = 0;
static volatile int updata_count = 0;
static volatile short current_thread_max = 0;
static volatile short freshing_cur_max = 0;

#define thread_list			((struct _THREAD_ **)TLB)
#define PROCESS_LIST_BUSY_BIT	0
spin_optr_def_bit(ProcessGlobalList,&list_busy,PROCESS_LIST_BUSY_BIT);

static int find_first_empty(){
	u64 page, rf;
	short cur, real, i = 0;
	
	rf = sflags();
	cli();
	if(freshing_cur_max){//如果已经在更新了，就不往里面凑了
		i = current_thread_max - PAGE_SIZE/sizeof(void*);//已经在更新说明前面没有空的位置了
		while(freshing_cur_max);
	}
	while(1){
		cur = current_thread_max;//获取当前的最大值
		for(;i < cur;i++)//搜索空位
			if(!thread_list[i] &&
				!cmpxchg8b(thread_list + i,NULL,1,NULL)){//抢占空位
				lflags(rf);
				return i;//成功返回值
			}
		freshing_cur_max = 1;//设置正在更新标志
		if(cmpxchg2b(&current_thread_max,cur,cur + PAGE_SIZE/sizeof(void*),NULL)){//抢占更新权
			while(freshing_cur_max);//抢占失败则等待
			if(current_thread_max == cur) {//前后值相等，说明没有更多空位了
				lflags(rf);
				return -1;
			}
			continue;
		}
		real = cur;//
		cur += PAGE_SIZE/sizeof(void*);
		if(cur > MAX_THREAD){
			xaddw(&current_thread_max,-(PAGE_SIZE/sizeof(void*)));//恢复值
			lflags(rf);
			return -1;
		}
		page = get_free_page(0,0);//映射页面
		memset(PADDR2V(page),0,PAGE_SIZE);
		put_page(page,NULL,thread_list+ real);
		freshing_cur_max = 0;
	}
}

//将进程从进程链表中移出
void remove_process(LPPROCESS process){
///	LPPROCESS prev,next;
///	
///	ID();
///	next = xchgq(&(process->next),NULL);
///	if(!process->prev){
///		//i am first of list
///rep:
///		if(cmpxchg8b(&list,process,next,NULL)){
///			//there are/is some/one node(s) inserting list
///			while(!process->prev);
///			process->prev->next = process->next;
///			if(next) next->prev = process->prev;
///			IE();
///			return;
///		}
///		else{
///			if(next) next->prev = NULL;
///			IE();
///			return;
///		}
///	}
///	//i am not first of list
///	while(1){
///		prev = process->prev;
///		if(prev)
///			if(!cmpxchg8b(&(prev->next),process,next,NULL)) {
///				if(next) next->prev = process->prev;//i am last
///				IE();
///				return;
///			}
///			else
///				while(prev == process->prev);
///		else 
///			goto rep;
///	}

	ID();
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
	IE();
}
//将进程插入到链表中
void insert_process(LPPROCESS process){
	///LPPROCESS next;
	///process->prev = process->next = NULL;
	///ID();
	///next = xchgq(&list,process);
	///if(next) 
	///	while(next->prev);
	///process->next = next;
	///if(next) next->prev = process;
	///IE();
	
	LPPROCESS _process;
	ID();
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
	IE();
}
//将线程从进程的线程链表中移出
void remove_process_thread(LPPROCESS process,LPTHREAD thread){
	ID();
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
	IE();
}
//将线程插入到进程的线程链表中
void insert_process_thread(LPPROCESS process,LPTHREAD thread){
	LPTHREAD _thread;
	ID();
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
	IE();	
}

void schedule2(){
	LPTHREAD old,new;
	LPTHREAD * next;
	u64 * pdb;
	int cur_max;
	int time = 0;
	
	lock_btr_private(flags,CPU_FLAGS_NEED_SCHEDULE);
	write_private_dword(cpu_time,CPU_TIME);
	while(freshing_cur_max);
	cur_max = current_thread_max;
	ID(); SD();
	old = GetCurThread();
	next = thread_list + old->solt + 1;
	while(1){
		if(next >= thread_list + cur_max){
			next = thread_list;
			time++;
			if(time > 4) {
				IE();SE();
				return;
			}
		}
		if(!(new = *next) || new->flags != TF_ACTIVE || TryLockThreadStatus(new)) {
			next++;
			continue;
		}
		new->processor = GetCPUId();
		pdb = PADDR2V(get_cr3());
		if(new->father != GetCurProcess()){
			SetCurProcess(new->father);
			memcpy(pdb,new->father->pdbe,256 * 8);
		}
		pdb[USER_STACK_P4E_INDEX] = new->user_stack_p4e;
		fpg();
		write_private_dword(cpu_time,CPU_TIME);
		SetCurThread(new);
		if(suppose_sysenter) wrmsr(IA32_SYSENTER_ESP,new->ker_ent_rsp);//切换内核堆栈入口
		write_private_dword(TSS.reg[RSPL(0)],new->ker_ent_rsp & 0x00000000ffffffff);//切换内核堆栈入口
		write_private_dword(TSS.reg[RSPH(0)],new->ker_ent_rsp >> 32);//切换内核堆栈入口
		old = switch_task_b(&(old->rsp),new->rsp,old);
		UnlockThreadStatus(old);
		IE();SE();
		if(old->flags == TF_DESTORY)
			kill(old);
		return;
	}
}
static int updata_schedule_receive(){
	int i;
	
	xaddd(&updata_count,1);
	if(!need_updata_schedule){
		if(xaddd(&updata_count,-1) == 1) need_updata_schedule = 0;
		return -1;
	}
	if((i = read_private_dword(cpu_time)) > 0){
		write_private_dword(cpu_time,i-1);
		if(xaddd(&updata_count,-1) == 1) need_updata_schedule = 0;
		return 0;
	}
	lock_bts_private(flags,CPU_FLAGS_NEED_SCHEDULE);
	if(lock_bt_private(flags,CPU_FLAGS_SCHEDULE_DISABLE)) return;
	schedule2();
	if(xaddd(&updata_count,-1) == 1) need_updata_schedule = 0;
	return 0;
}
static int updata_schedule_send(){
	updata_count = 0;
	need_updata_schedule = 1;
	send_ipi(CPU_TIME_UPDATA_IPI,0,1,IPIM_ALL_EX);
}
void schedule(){
	//static int count = 0;
	int i;
	//count++;
	//if(count == 10) {updata_schedule_send(); count = 0;}
	updata_schedule_send();
	if((i = read_private_dword(cpu_time)) > 0){
		write_private_dword(cpu_time,i-1);
		return;
	}
	lock_bts_private(flags,CPU_FLAGS_NEED_SCHEDULE);
	if(lock_bt_private(flags,CPU_FLAGS_SCHEDULE_DISABLE)) return;
	schedule2();
}
void __attribute__((noreturn)) exit(int code){
	LPTHREAD thread;
	LPPROCESS process;
	
	thread = GetCurThread();
	ID();
	thread->flags = TF_DESTORY;
	remove_process_thread(thread->father,thread);
	if(!thread->father->thread_count){
		thread->father->ret = code;
		process = thread->father;
		if(process->by_shell) {
			if(process->f_thread) wake_up(process->f_thread);
		}
		else{
			if(process->image_name) kfree(process->image_name);
			kfree(process);
		}
	}
	timer_free(thread->timer);
	free_area(thread,0x00007ff000000000,0x0000000fffffefff);
	if(thread = xchgq(&killing_thread,thread)) {
		free_stack(thread->stack);
		thread_list[thread->solt] = NULL;
		kfree(thread);
	}
	write_private_dword(cpu_time,0);
	IE();
	schedule2();
}
static int raw_kill(LPTHREAD thread){
	LPPROCESS process;
	
	thread->flags = TF_DESTORY;
	remove_process_thread(thread->father,thread);
	if(!thread->father->thread_count){
		thread->father->ret = -1;
		process = thread->father;
		if(process->by_shell) {
			if(process->f_thread) wake_up(process->f_thread);
		}
		else{
			if(process->image_name) kfree(process->image_name);
			kfree(process);
		}
	}
	timer_free(thread->timer);
	free_stack(thread->stack);
	thread_list[thread->solt] = NULL;
	kfree(thread);
}
int kill(LPTHREAD thread){
	LPPROCESS process;
	int ret;
	
	if(!thread || thread == GetCurThread()) exit(-1);
	if(!addr_vaild(NULL,thread) || thread->gst != GST_THREAD) return -1;
	ID();
	if(TryLockThreadStatus(thread)){
		lock_bts(&thread->flags,THREAD_KILL_WHEN_WAKE_BIT);
		thread->flags = TF_DESTORY;
		IE();
		return 0;
	}
	ret = raw_kill(thread);
	IE();
	return ret;
}
int wake_up(LPTHREAD thread){
	LPTHREAD _thread;
	int ret;
	
	//printk("weak up thread %d,%d.\n",thread->id,thread->flags);
	if(!thread || !addr_vaild(NULL,thread) || thread->gst != GST_THREAD) return -1;
	if(cmpxchg4b(&thread->flags,TF_BLOCK,TF_ACTIVE,NULL)){
		if(lock_bts(&thread->flags,THREAD_KILL_WHEN_WAKE_BIT)){
			ID();
			ret = raw_kill(thread);
			IE();
			return ret;
		}
		else{
			//error
			
			
		}
	}
	else{
		if(GetCurThread()->solt < processor_count) schedule2();
	}
	return 0;
}
static void wait_call_back(LPTIMER timer,LPTHREAD thread){
	wake_up(thread);
}
int wait(int msecond){
	LPTHREAD thread;
	int ret;
	
	if(msecond){
		thread = GetCurThread();
		if(cmpxchg4b(&thread->flags,TF_ACTIVE,TF_BLOCK,NULL)){
			if(lock_bts(&thread->flags,THREAD_KILL_WHEN_WAKE_BIT)){
				exit(-1);
			}
			else{
				//error
				
				
			}
		}
		timer_reset(thread->timer,msecond,TMR_MOD_ONCE,wait_call_back,thread);
		timer_start(thread->timer);
	}
	schedule2();
	return 0;
}

/*
Mutex
Wait();
	if(CheckArgument()) return ERR;
	if(mutex->flags & MUTEX_LOCKED){
		if(!time) return ERR_BUSY;
		InserIntoWaitList();
		RemoveFromActiveList();
		InsertIntoBlockList();
		UpdataDestoryData();
		Thread->semaphore_val = 0;
		if(time != INFINE) ResetTimer();
		Schedule();
		if(IsKilling()){
			if(time != INFINE) StopTimer();
			return ERR;
		}
		RemoveFromWaitList();
		if(thread->semaphore_val)//wake up by timer
			return ERR_BUSY;
	}
	mutex->flags |= MUTEX_LOCKED;
	if(mutex->release.head) WakeUp();
	return 0;
Release();
	if(CheckArgument()) retunr ERR;
	if(!(mutex->flags & MUTEX_LOCKED)){
		if(!time) return ERR_BUSY;
		InsertIntoReleaseList();
		RemoveFromActiveList();
		InsertIntoBlockList();
		UpdataDestoryData();
		Thread->semaphore_val = 0;
		if(time != INFINE) ResetTimer();
		Schedule();
		if(IsKilling){
			if(time != INFINE) StopTimer();
			return ERR;
		}
		RemoveFromReleaseList();
		if(thread->semaphore_val) return ERR_BUSY;
	}
	mutex->flags &= ~MUTEX_LOCKED;
	if(mutex->wait_head) WakeUp();
	return 0;

Semaphore
Wait();
	if(CheckArgument()) return ERR;
	if(se->wait.head || se->cur < val){
		if(!time) return ERR_BUSY;
		InsertIntoWaitList();
		RemoveFromActiveList();
		InsetIntoBlockList();
		UpdataDestoryData();
		Thread->semaphore_val = val;
		if(time != INFINE) ResetTimer();	//if out of time,will wake up thread,but don't remove thread from wait list;
		Schedule();
		if(IsKilling(se)) {//if semaphore is killing, just return is enough;
			if(time != INFINE) StopTimer();
			return ERR;
		}
		if(se->cur < val){//wake up because of out of time;
			//Might i am not the first one
			if(CurThread == se->wait.head){
				//if i am first one,should test second thread weather wake up;
				RemoveFromWaitList();
				if(se->wait.head && se->cur >= se->wait.head->semaphore_val) WakeUp();
			}
			else RemoveFromWaitList();//if i am not the first one,remove myself from wait list is complex.
			return ERR_BUSY;
		}
		//wake up because resource is enough;
		RemoveFromWaitList();
	}
	se->cur -= val;
	if(se->release.head && se->max - se->cur >= se->release.head->semaphore_val) WakeUp();
	if(se->wait.head && se->cur >= se->wait.head->semaphore_val) WakeUp();
	return 0;
Release();
	if(heckArgument()) return ERR;
	if(se->release.head || se->max - se->cur < val){
		if(!time) return ERR_BUSY;
		InsertIntoReleaseList();
		RemoveFromActiveList();
		InsertIntoBlockList();
		UpdataDestoryData();
		Thread->semaphore_val = val;
		if(time != INFINE) ResetTimer();
		Schedule();
		if(IsKilling(se)) {//if semaphore is killing, just return is enough;
			if(time != INFINE) StopTimer();
			return ERR;
		}
		if(se->max - se->cur < val){
			if(CurThread == se->release.head){
				RemoveFromReleaseList();
				if(se->release.head && se->release.head->semaphore_val + se->cur <= se->max) WakeUp();
			}
			else RemoveFromWaitList();
			return ERR_BUSY;
		}
		RemoveFromReleaseList();
	}
	se->cur += val;
	if(se->wait.head && se->cur >= se->wait.head->semaphore_val) WakeUp();
	if(se->release.head && se->release.hread->semaphore_val + se->cur <= se->max) WakeUp();
	return 0;
*/

int destory_mutex(struct _MUTEX_ * mutex){
	struct _THREAD_ * thread;
	
	LockMutex(mutex);//抢占信号量
	mutex->flags |= MUTEX_KILLING;//设置销毁标志
	while(thread = mutex->wait.t)//逐个唤醒正在等待的线程
		wake_up(thread);
	while(thread = mutex->release.t)
		wake_up(thread);
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
	thread->semaphore_val = 1;
	wake_up(thread);
}
int wait_mutex(struct _MUTEX_ * mutex,int time){//抢占资源
	LPTHREAD thread;
	LPTHREAD * prev;
	volatile LPTHREAD next;

	if(!mutex) return ERR_INVAILD_PTR;//非法指针
	thread = GetCurThread();
	ID();//关中断
	LockMutex(mutex);//抢占信号量
	if(mutex->wait.t || mutex->flags & MUTEX_LOCKED){//若资源不足或已有线程在等待（服从先到先服务）
		if(!time){//不等待则直接返回资源不足
			UnlockMutex(mutex);
			IE();
			return ERR_RESOURCE_BUSY;
		}
		if(cmpxchg4b(&thread->flags,TF_ACTIVE,TF_BLOCK,NULL)){
			if(lock_bts(&thread->flags,THREAD_KILL_WHEN_WAKE_BIT)){
				exit(-1);
			}
			else{
				//error

			}
		}
		prev = mutex->wait.p;
		mutex->wait.p = & next;
		if(prev) * prev = thread;
		if(!mutex->wait.t) mutex->wait.t = thread;
		thread->semaphore_val = 0;
		UnlockMutex(mutex);
		if(time != -1){//有限等待则重启计时器
			timer_reset(thread->timer,time,TMR_MOD_ONCE,mutex_timer_call_back,mutex);
			timer_start(thread->timer);
		}
		IE();
		schedule();//调度
		if(mutex->flags & MUTEX_KILLING) {//若信号量正在销毁，则返回 无效的指针//if mutex is killing, just return is enough;
			if(time != -1) timer_stop(thread->timer);
			mutex->wait.t = next;
			return ERR_INVAILD_PTR;
		}
		ID();
		LockMutex(mutex);
		if(thread->semaphore_val) {//wake up because of out of time;
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
			IE();
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
	IE();
	if(thread) wake_up(thread);	
	return 0;
}
int release_mutex(struct _MUTEX_ * mutex,int time){
	LPTHREAD thread;
	LPTHREAD * prev;
	volatile LPTHREAD next;
	
///	if(heckArgument()) return ERR;
	if(!mutex) return ERR_INVAILD_PTR;
	thread = GetCurThread();
	ID();
	LockMutex(mutex);
	if(mutex->flags & MUTEX_LOCKED || mutex->release.t){
		if(!time){
			UnlockMutex(mutex);
			IE();
			return ERR_RESOURCE_BUSY;
		}
		
		prev = mutex->release.p;
		if(cmpxchg4b(&thread->flags,TF_ACTIVE,TF_BLOCK,NULL)){
			if(lock_bts(&thread->flags,THREAD_KILL_WHEN_WAKE_BIT)){
				exit(-1);
			}
			else{
				//error

			}
		}
		mutex->release.p = & next;
		if(prev) * prev = thread;
		if(!mutex->release.t) mutex->release.t = thread;
		thread->semaphore_val = 0;
		UnlockMutex(mutex);
		if(time != -1){//有限等待则重启计时器
			timer_reset(thread->timer,time,TMR_MOD_ONCE,mutex_timer_call_back,mutex);
			timer_start(thread->timer);
		}
		IE();
		write_private_dword(cpu_time,0);
		schedule();
		if(mutex->flags & MUTEX_KILLING) {
			if(time != -1) timer_stop(GetCurThread()->timer);
			mutex->release.t = next;
			return ERR_INVAILD_PTR;
		}
///		if(IsKilling(mutex)) {//if mutex is killing, just return is enough;
///			if(time != INFINE) StopTimer();
///			return ERR;
///		}
		thread = GetCurThread();
		ID();
		LockMutex(mutex);
		
		if(thread->semaphore_val) {//wake up because of out of time;
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
			IE();
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
	IE();
	if(thread) wake_up(thread);	
	return 0;
}


int destory_semaphore(struct _SEMAPHORE_ * se){
	struct _THREAD_ * thread,*thread2;
	
	LockSemaphore(se);//抢占信号量
	se->flags |= SE_KILLING;//设置销毁标志
	while(thread = se->wait.p)//逐个唤醒正在等待的线程
		wake_up(thread);
	while(thread = se->release.p)
		wake_up(thread);
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
	wake_up(thread);
}
int wait_semaphore(int val,struct _SEMAPHORE_ * se,int time){//抢占资源
	LPTHREAD thread;
	LPTHREAD * prev;
	LPTHREAD next;

	if(!se) return ERR_INVAILD_PTR;//非法指针
	if(!val) return 0;//不抢占任何资源则直接返回
	if(val > se->max) return ERR_OUT_OF_RANGE;//抢占超过总数的资源
	thread = GetCurThread();
	ID();//关中断
	LockSemaphore(se);//抢占信号量
	if(se->wait.t || se->cur < val){//若资源不足或已有线程在等待（服从先到先服务）
		if(!time){//不等待则直接返回资源不足
			UnlockSemaphore(se);
			IE();
			return ERR_RESOURCE_BUSY;
		}
		if(cmpxchg4b(&thread->flags,TF_ACTIVE,TF_BLOCK,NULL)){
			if(lock_bts(&thread->flags,THREAD_KILL_WHEN_WAKE_BIT)){
				exit(-1);
			}
			else{
				//error

			}
		}
		prev = se->wait.p;
		se->wait.p = &next;
		if(prev) * prev = thread;
		if(!se->wait.t) se->wait.t = thread;   
		thread->semaphore_val = val;
		UnlockSemaphore(se);
		IE();
		if(time != -1){//有限等待则重启计时器
			timer_reset(thread->timer,time,TMR_MOD_ONCE,semaphore_timer_call_back,se);
			timer_start(thread->timer);
		}
		schedule();//调度
		if(se->flags & SE_KILLING) {//若信号量正在销毁，则返回 无效的指针
			if(time == -1) timer_stop(thread->timer);
			se->wait.t = next;
			return ERR_INVAILD_PTR;
		}
		ID();
		LockSemaphore(se);
		thread = GetCurThread();
		if(se->cur < val) {
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
			IE();
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
		wake_up(thread);
	}
	if(se->wait.t && se->cur >= se->wait.t->semaphore_val) {
		thread = se->wait.t;
		se->wait.t = next;
		if(!next) se->wait.p = NULL;
		wake_up(thread);
	}
	UnlockSemaphore(se);
	IE();
	return 0;
}
int release_semaphore(int val,struct _SEMAPHORE_ * se,int time){
	LPTHREAD thread;
	LPTHREAD * prev;
	volatile LPTHREAD next;
	
	if(!se) return ERR_INVAILD_PTR;
	if(!val) return 0;
	if(val > se->max) return ERR_OUT_OF_RANGE;
	ID();
	LockSemaphore(se);
	if(se->cur + val > se->max || se->release.t){
		if(!time){
			UnlockSemaphore(se);
			IE();
			return ERR_RESOURCE_BUSY;
		}
		if(cmpxchg4b(&thread->flags,TF_ACTIVE,TF_BLOCK,NULL)){
			if(lock_bts(&thread->flags,THREAD_KILL_WHEN_WAKE_BIT)){
				exit(-1);
			}
			else{
				//error

			}
		}
		prev = se->release.p;
		se->release.p = &next;
		if(prev) * prev = thread;
		if(!se->release.t) se->release.t = thread;   
		thread->semaphore_val = val;
		UnlockSemaphore(se);
		IE();
		if(time != -1){//有限等待则重启计时器
			timer_reset(thread->timer,time,TMR_MOD_ONCE,semaphore_timer_call_back,se);
			timer_start(thread->timer);
		}
		schedule();
		if(se->flags & SE_KILLING) {//若信号量正在销毁，则返回 无效的指针
			if(time == -1) timer_stop(thread->timer);
			se->release.t = next;
			return ERR_INVAILD_PTR;
		}
		ID();
		LockSemaphore(se);
		thread = GetCurThread();
		if(se->cur < val) {
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
			IE();
			return ERR_RESOURCE_BUSY;
		}
		se->release.t = next;
		if(!next) se->release.p = NULL;
	}
	se->cur -= val;
	if(se->wait.t && se->max - se->cur >= se->wait.t->semaphore_val){
		thread = se->wait.t;
		se->wait.t = next;
		if(!next) se->wait.p = NULL;
		wake_up(thread);
	}
	if(se->release.t && se->cur >= se->release.t->semaphore_val) {
		thread = se->release.t;
		se->release.t = next;
		if(!next) se->release.p = NULL;
		wake_up(thread);
	}
	UnlockSemaphore(se);
	IE();
	return 0;
}

static void thread_entry(int (*entry)(void*),LPTHREAD old){
	LPSTREAM image;
	LPPROCESS process;
	LPTHREAD self;
	
	if(old) UnlockThreadStatus(old);
	IE();
	SE();
	self = GetCurThread();
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
	
	{//initlize process data
		process = kmalloc(sizeof(PROCESS),0);
		memset(process,0,sizeof(PROCESS));
		LockProcessField(process);
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
		thread->flags = TF_STOP;
		thread->processor = -1;
		thread->argv = argv;
		//thread->status_disable = 0;
		//thread->level = 0;//useless
		//thread->cur_level = 0;
		//thread->busy = 0;//useless
		thread->timer = timer_alloc(0,TMR_MOD_ONCE,NULL,NULL);
		//thread->user_stack_p4e = 0;//useless
		thread->stack = stack = alloc_stack(1);
		stack = stack + KSS/sizeof(u64) - 12;
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
	}
	put_page(system_enter_page,thread,SYSTEM_CALL_PROC_BASE);
	put_page(get_free_page(1,1),thread,USER_INIT_RSP);
	insert_process_thread(process,thread);
	ID();
	insert_process(process);
	thread->solt = find_first_empty();
	thread_list[thread->solt] = thread;
	UnlockProcessField(thread);
	IE();
	return process;
}
LPTHREAD create_thread(LPPROCESS process,int (*entry)(void*),void * argv){
	LPTHREAD thread,_thread;
	u64 * stack;

	ID();
	{//initlize thread status.
		thread = kmalloc(sizeof(THREAD),0);
		memset(thread,0,sizeof(THREAD));
		thread->gst = GST_THREAD;
		thread->flags = TF_STOP;
		thread->processor = -1;
		thread->argv = argv;
		//thread->status_disable = 0;
		thread->timer = timer_alloc(0,TMR_MOD_ONCE,NULL,NULL);
		//thread->user_stack_p4e = 0;//useless
		thread->stack = stack = alloc_stack(1);
		stack = stack + KSS/sizeof(u64) - 12;
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
		//printk("thread %d stack:%P.",thread->id,stack);;
		//thread->level = 0;//useless
		//thread->cur_level = 0;
	}
	put_page(system_enter_page,thread,SYSTEM_CALL_PROC_BASE);
	put_page(get_free_page(1,1),thread,USER_INIT_RSP);
	if(!process) process = GetCurProcess();
	insert_process_thread(process,thread);
	thread->solt = find_first_empty();
	thread_list[thread->solt] = thread;
	IE();
	return thread;
}

static int shell_destory(LPTHREAD thread,LPPROCESS process){
	process->by_shell = 0;
	return 0;
}

int shell(const wchar_t * name,void * argv){
	LPPROCESS process;
	LPTHREAD thread;
	int ret;
	
	process = create_process(name,argv);
	process->by_shell = 1;
	thread = GetCurThread();
	if(cmpxchg4b(&thread->flags,TF_ACTIVE,TF_BLOCK,NULL)){
		if(lock_bts(&thread->flags,THREAD_KILL_WHEN_WAKE_BIT)){
			exit(-1);
			return ret;
		}
		else{
			//error
			
			
		}
	}
	ret = process->ret;
	if(process->image_name) kfree(process->image_name);
	kfree(process);
	return ret;
}
void __attribute__((noreturn)) schedule_init_ap(int (*entry)(void*),void * argv){
	LPTHREAD thread,_thread;
	LPPROCESS process;
	u64 * stack;
	u64 __rsp;
	
	ID();
	{//initlize thread status.
		thread = kmalloc(sizeof(THREAD),0);
		memset(thread,0,sizeof(THREAD));
		if((thread->solt = find_first_empty()) < 0){
			//err
		}
		thread->gst = GST_THREAD;
		thread->flags = TF_ACTIVE;
		thread->processor = GetCPUId();
		thread->timer = timer_alloc(0,TMR_MOD_ONCE,NULL,NULL);
		thread->argv = argv;
		//thread->user_stack_p4e = 0;//useless
		thread->id = xaddd(&thread_count,1);
		//thread->level = 0;//useless
		//thread->cur_level = 0;
		thread->stack = stack = alloc_stack(1);
		__rsp = stack = stack + KSS/sizeof(u64) - 8;
		thread->ker_ent_rsp = __rsp + 56;
		//thread->rsp = 0;
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
	//if(suppose_sysenter) wrmsr(IA32_SYSENTER_ESP,thread->ker_ent_rsp);
	//write_private_dword(TSS.reg[RSPL(0)],thread->ker_ent_rsp & 0x00000000ffffffff);
	//write_private_dword(TSS.reg[RSPH(0)],thread->ker_ent_rsp >> 32);
	//put_page(system_enter_page,thread,SYSTEM_CALL_PROC_BASE);
	//put_page(get_free_page(1,1),thread,USER_INIT_RSP);
	//((u64*)PADDR2V(get_cr3()))[USER_STACK_P4E_INDEX] = thread->user_stack_p4e;
	LockThreadStatus(thread);
	thread_exit(__rsp,entry,NULL);
}
void __attribute__((noreturn)) schedule_init(int (*entry)(void*),void * argv){
	LPPROCESS process;
	LPTHREAD thread;
	u64 * stack;
	u64 __rsp;
	
	//enable other processor(s) to schedule
	request_ipi(CPU_TIME_UPDATA_IPI,updata_schedule_receive);
	{//process 0
		process = kmalloc(sizeof(PROCESS),0);
		memset(process,0,sizeof(PROCESS));
		//LockProcessField(process);//useless
		process->gst = GST_PROCESS;
		//process->id = 0;//useless
		//process->thread_count = 0;//useless
		//process->father = NULL;//useless
		//UnlockProcessField(process);//useless
		//LockProcessLocalList(process);//useless
		//UnlockProcessLocalList(process);//useless
		//LockProcessGlobalList();
		//UnlockProcessGlobalList();
	}
	{//thread 0
		thread = kmalloc(sizeof(THREAD),0);
		memset(thread,0,sizeof(THREAD));
		if((thread->solt = find_first_empty()) < 0){
			
			
		}
		thread->gst = GST_THREAD;
		thread->father = process;
		thread->flags = TF_ACTIVE;
		thread->argv = argv;
		thread->timer = timer_alloc(0,TMR_MOD_ONCE,NULL,NULL);
		thread->stack = stack = alloc_stack(1);
		__rsp = stack = stack + KSS/sizeof(u64) - 8;
		thread->ker_ent_rsp = __rsp + 56;
		//thread->id = 0;//useless
		//thread->processor = GetCPUId();//useless
		//thread->user_stack_p4e = 0;//useless
		//thread->level = 0;//useless
		//thread->cur_level = 0;
		//thread->busy = 0;//useless
		//thread->rsp = 0;//useless
		stack[0] = thread_entry;
		thread->id = xaddd(&thread_count,1);
	}
	thread_list[thread->solt] = thread;
	thread_count = 1;
	process_list = process;
	process_count = 1;
	process->thread = thread;
	process->thread_count = 1;
	SetCurProcess(process);
	SetCurThread(thread);
	//if(suppose_sysenter) wrmsr(IA32_SYSENTER_ESP,thread->ker_ent_rsp);
	//write_private_dword(TSS.reg[RSPL(0)],thread->ker_ent_rsp & 0x00000000ffffffff);
	//write_private_dword(TSS.reg[RSPH(0)],thread->ker_ent_rsp >> 32);
	//put_page(system_enter_page,thread,SYSTEM_CALL_PROC_BASE);
	//put_page(get_free_page(1,1),thread,USER_INIT_RSP);
	//((u64*)PADDR2V(get_cr3()))[USER_STACK_P4E_INDEX] = thread->user_stack_p4e;
	write_private_dword(cpu_time,CPU_TIME);
	ID();
	LockThreadStatus(thread);
	thread_exit(__rsp,entry,NULL);
}

