/*
	inc/kernel.h
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

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <stddef.h>
#include <arch.h>
#include <fs.h>
#include <asm.h>
#include <arch.h>
#include <timer.h>
#include <spinlock.h>

#define TF_ACTIVE	0//
#define TF_BLOCK	1//block by something
#define TF_CREAT	2//creating
#define TF_DESTORY	3//destorying
#define TF_STOP		4//only control struct exist

typedef struct _USER_ {
	u32 id;
	//MUTEX mutex;
	u32 active_count[4];
	u32 process_count;
	u32 thread_count;
	u32 cpu_time;
	u32 persent;
	wchar_t * name;
	GUID guid;
	struct _USER_ * prev;
	struct _USER_ * next;
} USER,*LPUSER;
typedef struct _PROCESS_ {
	volatile int gst;
	u32 id;
	int busy;
	u32 thread_count;
	u64 pdbe[255];
	LPUSER user;
	int ret;
	int by_shell;
	struct _THREAD_ * thread;
	struct _PROCESS_ * father;
	struct _THREAD_ * f_thread;
	struct _PROCESS_ * prev;
	struct _PROCESS_ * next;
	struct _STREAM_ * file;
	struct _UNLEAK_LOG_ * unleak_memory_list;
	const wchar_t * image_name;
	const wchar_t * short_name;
	FS_PATH cur_path;
} PROCESS,*LPPROCESS;
typedef struct _THREAD_ {
	volatile int gst;
	u32 id;
	int busy;
	volatile u32 flags;
	volatile u32 status_disable;
	int processor;//hold current processor which my control,-1 is unrunning thread
	int cpu_time;
	int priority;
	volatile int semaphore_val;
	int solt;
	struct _TIMER_ * timer;
	u64 user_stack_p4e;
	u64 ker_ent_rsp;
	u64 rsp;
	void * stack;
	void * argv;
	struct _PROCESS_ * father;
	volatile struct _THREAD_ * prev;
	volatile struct _THREAD_ * next;
} THREAD,*LPTHREAD;

int processor_count;
u64 kernel_time;
int global_flags;
LPUSER current_user;
u64 system_enter_page;

void __attribute__((noreturn)) exit(int code);
int kill(LPTHREAD thread);
int wake_up(LPTHREAD thread);
int wait(int msecond);
LPPROCESS create_process(const wchar_t * name,void * argv);
LPTHREAD create_thread(LPPROCESS process,int (*entry)(void*),void*);

void schedule2();
static inline void schedule_disable(){
	lock_bts_private(flags,CPU_FLAGS_SCHEDULE_DISABLE);
	write_private_dword(schedule_disable_count,read_private_dword(schedule_disable_count) + 1);
}
static inline void schedule_enable(){
	u32 val;
	val = read_private_dword(schedule_disable_count);
	if(val){
		val--;
		write_private_dword(schedule_disable_count,val);
	}
	if(!val) {
		lock_btr_private(flags,CPU_FLAGS_SCHEDULE_DISABLE);
		if(lock_btr_private(flags,CPU_FLAGS_NEED_SCHEDULE)) schedule2();
	}
}

#define SD()	schedule_disable()
#define SE()	schedule_enable()

#define GetCurThread()			((LPTHREAD)get_gs_qword(offsetof(struct _PRIVATE_DATA_,cur_thread)))
#define SetCurThread(Thread)	put_gs_qword(offsetof(struct _PRIVATE_DATA_,cur_thread),(Thread))
#define GetCurProcess()			((LPPROCESS)get_gs_qword(offsetof(struct _PRIVATE_DATA_,cur_process)))
#define SetCurProcess(Process)	put_gs_qword(offsetof(struct _PRIVATE_DATA_,cur_process),(Process))

#define THREAD_STATUS_BUSY_BIT	0//thread can not be kill when this bit is set
#define THREAD_PLIST_BUSY_BIT	1//thread can not remove from father's list when set
#define THREAD_KILL_WHEN_WAKE_BIT	3//thread will be kill if this bit is set
#define THREAD_STATUS_BUSY		(1 << THREAD_STATUS_BUSY_BIT)
#define THREAD_KILL_WHEN_WAKE	(1 << THREAD_KILL_WHEN_WAKE_BIT)
spin_optr_struct_member_bit(ThreadStatus,struct _THREAD_,busy,THREAD_STATUS_BUSY_BIT);
spin_optr_struct_member_bit(ThreadPList,struct _THREAD_,busy,THREAD_PLIST_BUSY_BIT);

#define PROCESS_LLIST_BUSY	0
#define PROCESS_THREAD_BUSY	1
#define PROCESS_FIELD_BUSY	2
#define PROCESS_STATU_BUSY	3
spin_optr_struct_member_bit(ProcessLocalList,struct _PROCESS_,busy,PROCESS_LLIST_BUSY);
spin_optr_struct_member_bit(ProcessThread,struct _PROCESS_,busy,PROCESS_THREAD_BUSY);
spin_optr_struct_member_bit(ProcessField,struct _PROCESS_,busy,PROCESS_FIELD_BUSY);
spin_optr_struct_member_bit(ProcessStatu,struct _PROCESS_,busy,PROCESS_STATU_BUSY);

#endif