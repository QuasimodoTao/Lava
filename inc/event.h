//Lava OS
//PageFault
//20-04-10-15-53

#ifndef _EVENT_H_
#define _EVENT_H_



typedef struct _EVENT_ {
	u16 type;
	u16 len;
	u8 data[];
} EVENT,*LPEVENT;










#endif