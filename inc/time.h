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
struct tm {
    int tm_sec; /* 秒 – 取值区间为[0,59] */
    int tm_min; /* 分 - 取值区间为[0,59] */
    int tm_hour; /* 时 - 取值区间为[0,23] */
    int tm_mday; /* 一个月中的日期 - 取值区间为[1,31] */
    int tm_mon; /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
    int tm_year; /* 年份，其值等于实际年份减去1900 */
    int tm_wday; /* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
    int tm_yday; /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */
    int tm_isdst; /* 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的时候，tm_isdst为0；不了解情况时，tm_isdst()为负。*/
    long int tm_gmtoff; /*指定了日期变更线东面时区中UTC东部时区正秒数或UTC西部时区的负秒数*/
    const char *tm_zone; /*当前时区的名字(与环境变量TZ有关)*/
};
u64 kernel_time;

int time2stamp(struct time_t * time, u64 * stamp);
int stamp2time(struct time_t * time, u64 stamp);
struct tm * localtime(stamp64_t * time);

#endif