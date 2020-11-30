/*
	inc/semaphore.h
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