/*
	vm/access.c
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

#include <vm.h>
#include <config.h>
#include <stddef.h>

u32 vm_read(u32 addr,u32 len) {
	u32 data = 0;

	addr &= 0xfffff;
	data = ((u8*)vm_memory_base)[addr];
	len--;
	if (!len) return data;
	data |= ((u32)(((u8*)vm_memory_base)[addr + 1])) << 8;
	len--;
	if (!len) return data;
	data |= ((u32)(((u8*)vm_memory_base)[addr + 2])) << 16;
	data |= ((u32)(((u8*)vm_memory_base)[addr + 3])) << 24;
	return data;
}
void vm_write(u32 addr, u32 data, u32 len) {
	addr &= 0xfffff;
	((u8*)vm_memory_base)[addr] = (u8)data;
	len--;
	if (!len) return;
	((u8*)vm_memory_base)[addr + 1] = (u8)(data >> 8);
	len--;
	if (!len) return;
	((u8 *)vm_memory_base)[addr + 2] = (u8)(data >> 16);
	((u8 *)vm_memory_base)[addr + 3] = (u8)(data >> 24);
}