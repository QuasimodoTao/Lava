/*
	fs/file_table.c
	Copyright (C) 2020  Quasimodo

    This program is kfree software: you can redistribute it and/or modify
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

#include <config.h>
#include <stddef.h>
#include <mm.h>
#include <string.h>
#include <spinlock.h>
#include <fs.h>
#include <asm.h>
#include <lava.h>

#define first_free  ((volatile uint32_t*)FTFFB)
#define current_max ((volatile uint32_t*)FTCMB)

static char table_mask[(FT_TOTAL + 0x07) >> 3] = {0,};
static char free_cur_first[(FT_TOTAL + 0x07) >> 3] = {0,};

 

LPSTREAM * alloc_file_table(){
    uint32_t i;

    for(i = 0;i < FT_TOTAL;i++)
        if(!spin_try_lock_bit(table_mask,i)) {
            put_page(get_free_page(0,0,0),NULL,(void*)(FILE_TABLE_BASE + i * SIZE_PER_FT));
            return (void*)(FILE_TABLE_BASE + i * SIZE_PER_FT);
        }
    return NULL;
}
void free_file_table(LPSTREAM * table){
    uint32_t i,j;
    uint32_t cur_max;

    if(!table) return;
    j = (((uint64_t)table) - FILE_TABLE_BASE) / SIZE_PER_FT;
    cur_max = current_max[j];
    for(i = 0;i < current_max[j];i++){
        if(table[i]){
            if(table[i]->call_back) table[i]->call_back(table[i],table[i]->call_back_data);
            else close(table[i]);
        }
    }
    free_area(NULL,table,SIZE_PER_FT);
    spin_unlock_bit(table_mask,j);
}
int insert_file(LPSTREAM * table,LPSTREAM file){
    uint32_t i,j;
    uint32_t cur,real;
    u64 page;

    if(!table || !file) return -1;
    j = (((uint64_t)table) - FILE_TABLE_BASE) / SIZE_PER_FT;
    if(bt(free_cur_first,j)){
        i = current_max[j] - PAGE_SIZE/sizeof(void*);
        while(bt(free_cur_first,j)) pause();
    }
    while(1){
        cur = current_max[j];
        for(;i < cur;i++)
            if(!table[i] && !cmpxchg8b(table + i,NULL,file,NULL)) return i;
        bt(free_cur_first,j);
        if(cmpxchg4b(current_max + j,cur,cur + PAGE_SIZE/sizeof(void*),NULL)){
            while(bt(free_cur_first,j)) nop();
            if(current_max[j] == cur) return -1;
            continue;
        }
        real = cur;
        cur += PAGE_SIZE/sizeof(void*);
        if(cur > SIZE_PER_FT/sizeof(void*)){
            xaddd(current_max + j,-(PAGE_SIZE/sizeof(void*)));
            naked_btr(free_cur_first,j);
            return -1;
        }
        page = get_free_page(0,0,0);//映射页面
		memset(PADDR2V(page),0,PAGE_SIZE);
		put_page(page,NULL,table+ real);
		naked_btr(free_cur_first,j);
    }
}
int remove_file(LPSTREAM * table,LPSTREAM file,int i){
    uint32_t j,k;

    if(!table || !file || i < 0) return -1;
    if(cmpxchg8b(table + i,file,NULL,NULL)) return -1;
    j = (((uint64_t)table) - FILE_TABLE_BASE) / SIZE_PER_FT;
    do{
        k = first_free[j];
        if(k < (uint32_t)i) return 0;
    } while(cmpxchg4b(first_free + j,k,i,NULL));
    return 0;
}
void file_table_init(){
    memset(first_free,0,FT_TOTAL * sizeof(uint32_t));
    memset(current_max,0,FT_TOTAL * sizeof(uint32_t));
}







