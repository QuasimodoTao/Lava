/*
	inc/disk.h
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

#ifndef _DISK_H_
#define _DISK_H_

#include <stddef.h>

struct _DISK_CMD_ {
	int cnt;
	int mask;
	struct {
		u8 write;
		u16 cnt;
		u64 lba;
		void * buf;
		struct _THREAD_ * wait;
	} request[32];
};



#endif