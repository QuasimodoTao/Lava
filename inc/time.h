//Lava OS
//PageFault
//20-03-16-15-52

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