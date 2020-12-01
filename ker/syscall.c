/*
	ker/sys_call.c
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
#include <stdio.h>
#include <kernel.h>

int syscall_wait(int msecond);



static void * syscall_table[] = {
    syscall_wait
};
static void * linux_syscall_table[] = {
    NULL
};






u64 sys_call(u64 c,u64 d,u64 r8,u64 r9){
    u64 ret;
    if(GetCurThread()->need_destory) exit(ERR_BE_DESTORY);
    if(c >= sizeof(syscall_table)/sizeof(void*)) return ERR_UNVAILD_SYSCALL;
    ret = ((u64(*)(u64,u64,u64))(syscall_table[c]))(d,r8,r9);
    if(GetCurThread()->need_destory) exit(ERR_BE_DESTORY);
    return ret;
}
u64 sys_call_80(u64 c,u64 d,u64 r8,u64 r9){
	u64 ret;
    if(GetCurThread()->need_destory) exit(ERR_BE_DESTORY);
    if(c >= sizeof(linux_syscall_table)/sizeof(void*)) return ERR_UNVAILD_SYSCALL;
    ret = ((u64(*)(u64,u64,u64))(linux_syscall_table[c]))(d,r8,r9);
    if(GetCurThread()->need_destory) exit(ERR_BE_DESTORY);
    return ret;
}
u64 sys_call_82(u64 c,u64 d,u64 r8,u64 r9){
	return ERR_UNVAILD_SYSCALL; 
}
u64 sys_call_83(u64 c,u64 d,u64 r8,u64 r9){
	return ERR_UNVAILD_SYSCALL;
}



