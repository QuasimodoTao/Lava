//Lava OS
//PageFault
//20-03-29-00-32

#include <stddef.h>
#include <arch.h>
#include <config.h>
#include <string.h>
#include <asm.h>
#include <mm.h>
#include <lava.h>
#include <stdio.h>
#include <int.h>

extern u8 __syscall_start[];
extern int __syscall_size;
extern int __syscall_enter;

extern u8 __sysenter_start[];
extern int __sysenter_size;
extern int __sysenter_enter;

extern u8 __int_sys_start[];
extern int __int_sys_size;
extern int __int_sys_enter;
//extern int __int_sys_kernel_ss_0;//
//extern int __int_sys_kernel_ss_1;//

extern u8 system_enter_start[];
extern int system_enter_size;

extern u8 system_enter_int8x[];
extern int system_enter_int8x_size;
extern int system_enter_int8x_vector;


u64 syscall(u64 c,u64 d,u64 r8,u64 r9){
	printk("Syscall recive.\n");
	printk("%d,%d,%d,%d.\n",c,d,r8,r9);
	return 0xff00ff00ff00ff00;
}
u64 syscall_int80(u64 c,u64 d,u64 r8,u64 r9){
	
}
u64 syscall_int82(u64 c,u64 d,u64 r8,u64 r9){
	
}
u64 syscall_int83(u64 c,u64 d,u64 r8,u64 r9){
	
}
u64 syscall_int84(u64 c,u64 d,u64 r8,u64 r9){
	
}
void syscall_init(){
	int _cpuid[4];
	u64 msr;
	u8 * _system_enter_page;
	
//	__int_sys_start[__int_sys_kernel_ss_0] = __int_sys_start[__int_sys_kernel_ss_1] = KERNEL_SS;
	
	*(void**)(__syscall_start + __syscall_enter) = syscall;
	memcpy(ADDRP2V(SYSCALL_ENTER),__syscall_start,__syscall_size);
	
	*(void**)(__sysenter_start + __sysenter_enter) = syscall;
	memcpy(ADDRP2V(SYSENTER_ENTER),__sysenter_start,__sysenter_size);
	
	memcpy(ADDRP2V(SYSCALL_INT_ENTER(0x80)),__int_sys_start,__int_sys_size);
	*(void**)ADDRP2V(SYSCALL_INT_ENTER(0x80) + __int_sys_enter) = syscall_int80;
	make_gate(0x80,(u64)ADDRP2V(SYSCALL_INT_ENTER(0x80)),0,3,0x0e);
	
	memcpy(ADDRP2V(SYSCALL_INT_ENTER(0x81)),__int_sys_start,__int_sys_size);
	*(void**)ADDRP2V(SYSCALL_INT_ENTER(0x81) + __int_sys_enter) = syscall;
	make_gate(0x81,(u64)ADDRP2V(SYSCALL_INT_ENTER(0x81)),0,3,0x0e);
	
	memcpy(ADDRP2V(SYSCALL_INT_ENTER(0x82)),__int_sys_start,__int_sys_size);
	*(void**)ADDRP2V(SYSCALL_INT_ENTER(0x82) + __int_sys_enter) = syscall_int82;
	make_gate(0x82,(u64)ADDRP2V(SYSCALL_INT_ENTER(0x82)),0,3,0x0e);
	
	memcpy(ADDRP2V(SYSCALL_INT_ENTER(0x83)),__int_sys_start,__int_sys_size);
	*(void**)ADDRP2V(SYSCALL_INT_ENTER(0x83) + __int_sys_enter) = syscall_int83;
	make_gate(0x83,(u64)ADDRP2V(SYSCALL_INT_ENTER(0x83)),0,3,0x0e);
	
	memcpy(ADDRP2V(SYSCALL_INT_ENTER(0x84)),__int_sys_start,__int_sys_size);
	*(void**)ADDRP2V(SYSCALL_INT_ENTER(0x84) + __int_sys_enter) = syscall_int84;
	make_gate(0x84,(u64)ADDRP2V(SYSCALL_INT_ENTER(0x84)),0,0,0x0e);
	
	system_enter_page = get_free_page(0,1);
	system_enter_page = PAGE2PAGEE(system_enter_page,1,0,1);
	put_page(system_enter_page,NULL,(void*)SYSCALL_ENTER_CODE_START);
	
	_system_enter_page = ADDRP2V(PAGEE2PAGE(system_enter_page));
	memcpy(_system_enter_page,system_enter_start,system_enter_size);
	cpuid(0x80000001,0,_cpuid);
	suppose_syscall = 0;
	*(u64*)(_system_enter_page + 240) = 0;
	suppose_sysenter = 0;
	*(u64*)(_system_enter_page + 248) = 0;
	if(_cpuid[3] & 0x800){
		//suppose syscall
		msr = rdmsr(IA32_EFER);
		msr |= 0x01;
		wrmsr(IA32_EFER,msr);//set IA32_EFER.SCE
		wrmsr(IA32_LSTAR,SYSCALL_ENTER);//enter
		msr = rdmsr(IA32_STAR);
		msr &= 0x00000000ffffffff;
		msr |= ((u64)(KERNEL_CS | USER_CS_32)) << 32;
		wrmsr(IA32_STAR,msr);//selector
		wrmsr(IA32_FMASK,0);//flags mask
		//*(u64*)(_system_enter_page + 240) = 1;
		suppose_syscall = 1;
	}
	cpuid(1,0,_cpuid);
	if(_cpuid[3] & 0x800){
		if(((_cpuid[0] >> 8) & 0x07) >= 6){
			if(((_cpuid[0] >> 4) & 0x0f) >= 3){
				if((_cpuid[0] & 0x0f) >= 3){
					//suppose sysenter
					// *(u64*)(_system_enter_page + 248) = 1;
					suppose_sysenter = 1;
					wrmsr(0x176,SYSENTER_ENTER);//rip
					wrmsr(0x174,KERNEL_CS);//cs
				}
			}
		}
	}
	
	
}