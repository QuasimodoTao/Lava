/*
	inc/mutex.h
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