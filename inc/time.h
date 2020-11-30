/*
	inc/time.h
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

#ifndef _TIME_H_
#define _TIME_H_

#include <stddef.h>
#define time(p)	*(u64*)(p) = kernel_time

struct time_t{
	u16 year;
	u8 week;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
};
u64 kernel_time;

int time2stamp(struct time_t * time, u64 * stamp);
int stamp2time(struct time_t * time, u64 stamp);

#endif