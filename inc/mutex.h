//Lava OS
//PageFault
//20-03-25-15-07

#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <stddef.h>
#include <kernel.h>
#include <spinlock.h>

#define MUTEX_BUSY_BIT		0
#define MUTEX_ALLOC_BIT		1
#define MUTEX_KILLING_BIT	2
#define MUTEX_BUSY			(1 << MUTEX_BUSY_BIT)
#define MUTEX_ALLOC			(1 << MUTEX_ALLOC_BIT)
#define MUTEX_KILLING		(1 << MUTEX_KILLING_BIT)
#define MUTEX_LOCKED		8


typedef struct _MUTEX_ {
	u32 gst;
	volatile int flags;
	struct{
		volatile struct _THREAD_ ** p;
		volatile struct _THREAD_ * t;
	} wait,release;
} MUTEX,*LPMUTEX;

spin_optr_struct_member_bit(Mutex,struct _MUTEX_,flags,MUTEX_BUSY_BIT);

#define create_mutex()		create_mutex_ex(0,NULL)
LPMUTEX	create_mutex_ex(int locked,LPMUTEX mutex);
int destory_mutex(LPMUTEX mutex);
int wait_mutex(LPMUTEX mutex,int time);
int release_mutex(LPMUTEX mutex,int time);

#endif