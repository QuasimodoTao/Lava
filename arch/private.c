/*
	arch/private.c
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
#include <arch.h>
#include <config.h>
#include <kernel.h>
#include <lava.h>
#include <mm.h>

void private_data_init(){
	u64 page;
	struct _PRIVATE_DATA_ * private_data;
	u16 selector;
	
	page = get_free_page(0,0,0);
	memset(PADDR2V(page),0,0x1000);
	private_data = PADDR2V(page);
	private_data->cpu_id = processor_count;
	wrmsr(IA32_KERNEL_GS_BASE,private_data);
	wrmsr(IA32_GS_BASE,private_data);
	processor_count++;
	private_data->TSS.IO_map_base = offsetof(struct _TSS_64_,IO_map);
	private_data->TSS.IO_map[0] = private_data->TSS.IO_map[1] = 0xffffffff;
	selector = put_TSS(&(private_data->TSS));
	private_data->schedule_disable_count = 1;
	ltr(selector);
}