/*
	inc/graph.h
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

#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <stddef.h>


#define RECT_ATTR_UNVISABLE		1

typedef unsigned int RGB;



typedef struct _FORM_ {
	wchar_t * name;
	int top,bottom,right,left;
	int orgx,orgy;
	struct _FORM_ * father;
	//LPBRUSH brush;
	//LPPEN pen;
	//LPFONT font;
	int (*event_arise)(int);
} FORM,*LPFORM;

typedef struct _RECT_ {
	int top,bottom,right,left;
	int attr;
} RECT,*LPRECT;
typedef struct _POS_ {
	int x, y;
} POS,*LPPOS;

int rect_move(LPRECT,signed int dx,signed int dy);
int rect_draw(LPRECT rect,RGB color0,RGB color1);

#endif