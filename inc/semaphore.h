//Lava OS
//PageFault
//20-08-31-16-11

#ifndef _SEAMPHORE_H_
#define _SEAMPHORE_H_

#include <stddef.h>
#include <kernel.h>
#include <spinlock.h>

#define SE_BUSY_BIT		0
#define SE_ALLOC_BIT	1
#define SE_KILLING_BIT	2
#define SE_BUSY			(1 << SE_BUSY_BIT)
#define SE_ALLOC		(1 << SE_ALLOC_BIT)
#define SE_KILLING		(1 << SE_KILLING_BIT)

typedef struct _SEMAPHORE_ {
	int gst;
	int cur;
	int max;
	volatile int flags;
	struct {
		struct _THREAD_ ** p;
		struct _THREAD_ * t;
	} wait,release;
} SEMAPHORE,*LPSEMAPHORE;
spin_optr_struct_member_bit(Semaphore,struct _SEMAPHORE_,flags,SE_BUSY_BIT);

#define create_semaphore(max)	create_semaphore_ex(max,0,NULL)
LPSEMAPHORE create_semaphore_ex(int max,int cur,LPSEMAPHORE se);
int destory_semaphore(LPSEMAPHORE se);
int wait_semaphore(int val,LPSEMAPHORE se,int time);
int release_semaphore(int val,LPSEMAPHORE se,int time);

#endif