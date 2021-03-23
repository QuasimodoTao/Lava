/*
	ker/time.c
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

#include <time.h>
#include <lava.h>
#include <stddef.h>

static int16_t days[] = {
	0,
	31,
	31+28,
	31+28+31,
	31+28+31+30,
	31+28+31+30+31,
	31+28+31+30+31+30,
	31+28+31+30+31+30+31,
	31+28+31+30+31+30+31+31,
	31+28+31+30+31+30+31+31+30,
	31+28+31+30+31+30+31+31+30+31,
	31+28+31+30+31+30+31+31+30+31+30,
	365
};
int time2stamp(struct time_t * time, u64 * stamp){
	u16 year = time->year - 1970;
	u8 month = time->month - 1;
	u8 day = time->day - 1;
	u64 res;
	int leap_year;	
	
	leap_year = time->year/4 - time->year/100 + time->year/400 - 477;
	if(month > 1){
		if(time->year % 100){
			if(!(time->year % 4)) leap_year--;
		}
		else if(!(time->year % 400)) leap_year--;
	}
	res = (((u64)((days[month]) + day + leap_year + year * 365) * 24 + time->hour) * 60 + time->minute) * 60 + time->second;
	//时区
	res -= 8 * 60 * 60;
	*stamp = res;
	return 0;
}
int stamp2time(struct time_t * time,u64 stamp){
	int year0;
	int day_of_year;
	int leap_year;
	int leap_year0;
	int month;
	int Isleap_year = 0;
	
	//时区
	stamp += 8 * 60 * 60;
	time->day = 1;
	time->week = 1;
	time->second = stamp % 60;
	stamp /= 60;
	time->minute = stamp % 60;
	stamp /= 60;
	time->hour = stamp % 24;
	stamp /= 24;
	stamp += 719527;
	time->week += stamp % 7;
	year0 = stamp / 365;
	day_of_year = stamp & 365;
	leap_year = year0/4 - year0/100 + year0/400;
	year0 = (stamp - leap_year) / 365;
	day_of_year = (stamp - leap_year) % 365;
	leap_year0 = year0/4 - year0/100 + year0/400;
	if(leap_year != leap_year0){
		day_of_year ++;
		if(day_of_year >= 366){
			if(!(year0 % 100)){
				if(!(year0 % 4)){
					year0++;
					day_of_year -= 366;
					Isleap_year = 1;
				}
			}
			else{
				if(!(year0 % 400)){
					year0++;
					day_of_year -= 366;
					Isleap_year = 1;
				}
			}
		}
	}
	time->year = year0;
	if(Isleap_year && day_of_year > 31 + 29){
		day_of_year--;
	}
	for(month = 0;days[month] < day_of_year;month++);
	time->day += day_of_year - days[month - 1];
	time->month = month;
	return 0;
}
struct tm * localtime(stamp64_t * time){
	struct tm _tm;
	struct time_t __tm;

	stamp2time(&__tm,*time);
	_tm.tm_sec = __tm.second;
	_tm.tm_min = __tm.minute;
	_tm.tm_hour = __tm.hour;
	_tm.tm_mday = __tm.day;
	_tm.tm_mon = __tm.month;
	_tm.tm_year = __tm.year;
	_tm.tm_wday = __tm.week;
	_tm.tm_yday = days[__tm.month] + __tm.day;
	_tm.tm_isdst = -1;
	_tm.tm_gmtoff = 0;
	_tm.tm_zone = NULL;
	return &_tm;
}