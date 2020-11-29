//Lava OS
//PageFault
//20-04-08-17-06

#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <stddef.h>


#define RECT_ATTR_UNVISABLE		1


struct _BMP_FONT_ {
	struct{
		u32 off:24;
		u32 width:8;
	} pos[65536];
	void * bit_map;
};

struct _BMP_FONT_ * bit_map_font;
int screen_high;
int screen_width;
int byte_per_scan_line;
u8 * screen_lfb;
int byte_per_pixel;

typedef struct _FORM_ {
	wchar_t * name;
	u16 top,bottom,right,left;
	u16 orgx,orgy;
	struct _FORM_ * father;
	//LPBRUSH brush;
	//LPPEN pen;
	//LPFONT font;
	int (*event_arise)(int);
} FORM,*LPFORM;

typedef struct _RECT_ {
	u16 top,bottom,right,left;
	int attr;
} RECT,*LPRECT;





#endif