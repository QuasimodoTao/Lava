/*
	graph/consule.c
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
#include <keyboard.h>
#include <graph.h>
#include <error.h>


struct _CONSULE_ {
    short key_buf[32];
    int pos_x;//def 0
    int pos_y;//def 0
    int width;//def screen width
    int high;//def screen high
    int byte_per_pixel;
    int byte_per_scan_line;
    int chr_x;
    int chr_y;
    int chr_width;
    int chr_high;
    RGB test_color;
    RGB back_color;
};

int line_up(struct _CONSULE_ * con){
    RECT rect;

    if(!con) return ERR_INVAILD_PTR;
    rect.top = con->pos_y + 16;
    rect.bottom = con->pos_y + con->chr_high * 16;
    rect.left = con->pos_x;
    rect.right = con->pos_x + con->chr_width * 8;
    rect_up(&rect,16);
    rect.top = con->pos_y + con->chr_high * 16 - 16;
    rect.bottom = con->pos_y + con->chr_high * 16;
    rect_set(&rect,con->back_color);
    return 0;
}
int line_down(struct _CONSULE_ * con){
    RECT rect;

    if(!con) return ERR_INVAILD_PTR;
    rect.top = con->pos_y;
    rect.bottom = con->pos_y + con->chr_high * 16 - 16;
    rect.left = con->pos_x;
    rect.right = con->pos_x + con->chr_width * 8;
    rect_move(&rect,0,-16);
    rect.top = con->pos_y;
    rect.bottom = con->pos_y + 16;
    rect_set(&rect,con->back_color);
    return 0;
}
int con_show(struct _CONSULE_ * con){

}
int con_hide(struct _CONSULE_ * con){

}


int consule_init(){
    //count 8 consule






}


