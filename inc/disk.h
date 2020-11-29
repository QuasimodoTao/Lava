//Lava OS
//PageFault
//20-10-15-16-11

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