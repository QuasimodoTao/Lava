/*
	inc/spinlock.h
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

#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <stddef.h>
#include <asm.h>

#define spin_lock_common_bit(name,type,member,off)	static inline void name(type * ptr){\
	while(lock_bts(ptr,offsetof(type,member) * 8 + (off)));}
#define spin_unlock_common_bit(name,type,member,off)	static inline void name(type * ptr){\
	naked_lock_btr(ptr,offsetof(type,member) * 8 + (off));}
#define spin_try_lock_common_bit(name,type,member,off)	static inline u8 name(type * ptr){\
	return lock_bts(ptr,offsetof(type,member) * 8 + (off));}


#define spin_optr_struct_member_bit(name,type,member,off)	\
static inline void Lock##name (type * ptr){while(lock_bts(ptr,offsetof(type,member) * 8 + (off)));}\
static inline void Unlock##name (type * ptr){naked_lock_btr(ptr,offsetof(type,member) * 8 + (off));}\
static inline u8 TryLock##name (type * ptr){return lock_bts(ptr,offsetof(type,member) * 8 + (off));}

#define spin_optr_def_bit(name,ptr,off)	\
static inline void Lock##name (){while(lock_bts(ptr,off));}\
static inline void Unlock##name (){naked_lock_btr(ptr,off);};\
static inline u8 TryLock##name (){return lock_bts(ptr,off);}

#define spin_optr_def_arg_bit(name,ptr,off)	\
static inline void Lock##name (int bit_off){while(lock_bts(ptr,off + bit_off));}\
static inline void Unlock##name (int bit_off){naked_lock_btr(ptr,off + bit_off);}\
static inline u8 TryLock##name (int bit_off){return lock_bts(ptr,off + bit_off);}

#define spin_optr_bit(name,off)	\
static inline void Lock##name (void * ptr){while(lock_bts(ptr,off));}\
static inline void Unlock##name (void * ptr){naked_lock_btr(ptr,off);};\
static inline u8 TryLock##name (void * ptr){return lock_bts(ptr,off);}

#define spin_optr_array_bit(name,type,off)	\
static inline void Lock##name (type * array,int index){while(lock_bts(array,index*sizeof(type)*8 + off));}\
static inline void Unlock##name (type * array,int index){naked_lock_btr(array,index * sizeof(type) * 8 + off);}\
static inline u8 TryLock##name (type * array,int index){return lock_bts(array,index * sizeof(type) * 8 + off);}

static inline void spin_lock_bit(void * ptr,u64 off){while(lock_bts(ptr,off));}
static inline void spin_unlock_bit(void * ptr,u64 off){naked_lock_btr(ptr,off);}
static inline u8 spin_try_lock_bit(void * ptr,u64 off){return lock_bts(ptr,off);}


#endif