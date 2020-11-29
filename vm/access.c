//Lava OS
//PageFault
//20-04-11-15-07

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