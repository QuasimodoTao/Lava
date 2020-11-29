//Lava OS
//PageFault
//20-03-27-23-36

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
	
	page = get_free_page(0,0);
	memset(PADDR2V(page),0,0x1000);
	put_page(page,0,(void*)(SPEC_DATA_BASE + processor_count * 0x1000));
	private_data = (void*)(SPEC_DATA_BASE + processor_count * 0x1000);
	private_data->cpu_id = processor_count;
	wrmsr(IA32_KERNEL_GS_BASE,SPEC_DATA_BASE + processor_count * 0x1000);
	wrmsr(IA32_GS_BASE,SPEC_DATA_BASE + processor_count * 0x1000);
	processor_count++;
	private_data->TSS.IO_map_base = offsetof(struct _TSS_64_,IO_map);
	private_data->TSS.IO_map[0] = private_data->TSS.IO_map[1] = 0xffffffff;
	selector = put_TSS(&(private_data->TSS));
	ltr(selector);
}