/*
	inc/arch.h
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

#ifndef _ARCH_H_
#define _ARCH_H_

#include <asm.h>

#define IA32_P5_MC_ADDR					0x00
#define P5_MC_ADDR						0x00
#define IA32_P5_MC_TYPE					0x01
#define P5_MC_TYPE						0x01
#define IA32_MONITOR_FILTER_SIZE		0x06
#define IA32_TIME_STAMP_COUNTER			0X10
#define IA32_PLATFORM_ID				0X17
#define MSR_PLATFORM_ID					0X17
#define IA32_APIC_BASE					0X1B
#define APIC_BASE						0X1B
#define IA32_FEATURE_CONTROL			0X3A
#define IA32_TSC_ADJUST					0X3B
#define IA32_BIOS_UPDT_TRIG				0X79
#define BIOS_UPDT_TRIG					0X79
#define IA32_BIOS_SIGN_ID				0X8B
#define BIOS_SIGN_CR_D3					0X8B
#define BIOS_BBL_CR_D3					0X8B
#define IA32_SMM_MONITOR_CTL			0X9B
#define IA32_SMBASE						0X9E
#define IA32_PMC0						0XC1
#define PERFCTR0						0XC1
#define IA32_PMC1						0XC2
#define PERFCTR1						0XC2
#define IA32_PMC2						0XC3
#define IA32_PMC3						0XC4
#define IA32_PMC4						0XC5
#define IA32_PMC5						0XC6
#define IA32_PMC6						0XC7
#define IA32_PMC7						0XC8
#define IA32_MPERF						0XE7
#define IA32_APERF						0XE8
#define IA32_MTRRCAP					0XFE
#define IA32_SYSENTER_CS				0X174
#define IA32_SYSENTER_ESP				0X175
#define IA32_SYSENTER_EIP				0X176
#define IA32_MCG_CAP					0X179
#define MCG_CAP							0X179
#define IA32_MCG_STATUS					0X17A
#define MCG_STATUS						0X17A
#define IA32_MCG_CTL					0X17B
#define MCG_CTL							0X17B
#define IA32_PERFEVTSEL0				0X186
#define PERFEVTSEL0						0X186
#define IA32_PERFEVTSEL1				0X187
#define PERFEVTSEL1						0X187
#define IA32_PERFEVTSEL2				0X188
#define IA32_PERFEVTSEL3				0X189
#define IA32_PERF_STATUS				0X198
#define IA32_PERF_CTL					0X199
#define IA32_CLOCK_MODULATION			0X19A
#define IA32_THERN_INTERRUPR			0X19B
#define IA32_THERM_STATUS				0X19C
#define IA32_MISC_ENABLE				0X1A0
#define IA32_ENERGY_PERF_BIAS			0X1B0
#define IA32_PACKAGE_THERM_STATUS		0X1B1
#define IA32_PACKAGE_THERM_INTERRUPT	0X1B2
#define IA32_DEBUGCTL					0X1D9
#define MSR_DEBUGCTLA					0X1D9
#define MSR_DEBUGCTLB					0X1D9
#define IA32_SMRR_PHYSBASE				0X1F2
#define IA32_SMRR_PHYSMASK				0X1F3
#define IA32_PLATFORM_DCA_CAP			0X1F8
#define IA32_CPU_DCA_CAP				0X1F9
#define IA32_DCA_0_CAP					0X1FA
#define IA32_MTRR_PHYSBASE(x)			(0x200+(x)*2)
#define IA32_MTRR_PHYSMASK(x)			(0x201+(x)*2)
#define IA32_MTRR_PHYSBASE0				0X200
#define IA32_MTRR_PHYSMASK0				0X201
#define IA32_MTRR_PHYSBASE1				0X202
#define IA32_MTRR_PHYSMASK1				0X203
#define IA32_MTRR_PHYSBASE2				0X204
#define IA32_MTRR_PHYSMASK2				0X205
#define IA32_MTRR_PHYSBASE3				0X206
#define IA32_MTRR_PHYSMASK3				0X207
#define IA32_MTRR_PHYSBASE4				0X208
#define IA32_MTRR_PHYSMASK4				0X209
#define IA32_MTRR_PHYSBASE5				0X20A
#define IA32_MTRR_PHYSMASK5				0X20B
#define IA32_MTRR_PHYSBASE6				0X20C
#define IA32_MTRR_PHYSMASK6				0X20D
#define IA32_MTRR_PHYSBASE7				0X20E
#define IA32_MTRR_PHYSMASK7				0X20F
#define IA32_MTRR_PHYSBASE8				0X210
#define IA32_MTRR_PHYSMASK8				0X211
#define IA32_MTRR_PHYSBASE9				0X212
#define IA32_MTRR_PHYSMASK9				0X213
#define IA32_MTRR_FIX64K_00000			0X250
#define IA32_MTRR_FIX16K_80000			0X258
#define IA32_MTRR_FIX16K_A0000			0X259
#define IA32_MTRR_FIX4K_C0000			0X268
#define IA32_MTRR_FIX4K_C8000			0X269
#define IA32_MTRR_FIX4K_D0000			0X26A
#define IA32_MTRR_FIX4K_D8000			0X26B
#define IA32_MTRR_FIX4K_E0000			0X26C
#define IA32_MTRR_FIX4K_E8000			0X26D
#define IA32_MTRR_FIX4K_F0000			0X26E
#define IA32_MTRR_FIX4K_F8000			0X26F
#define IA32_PAT						0X277
#define IA32_MCx_CTL2(x)				(0x280+(x))
#define IA32_MC0_CTL2					0X280
#define IA32_MC1_CTL2					0X281
#define IA32_MC2_CTL2					0X282
#define IA32_MC3_CTL2					0X283
#define IA32_MC4_CTL2					0X284
#define IA32_MC5_CTL2					0X285
#define IA32_MC6_CTL2					0X286
#define IA32_MC7_CTL2					0X287
#define IA32_MC8_CTL2					0X288
#define IA32_MC9_CTL2					0X289
#define IA32_MC10_CTL2					0X28A
#define IA32_MC11_CTL2					0X28B
#define IA32_MC12_CTL2					0X28C
#define IA32_MC13_CTL2					0X28D
#define IA32_MC14_CTL2					0X28E
#define IA32_MC15_CTL2					0X28F
#define IA32_MC16_CTL2					0X290
#define IA32_MC17_CTL2					0X291
#define IA32_MC18_CTL2					0X292
#define IA32_MC19_CTL2					0X293
#define IA32_MC20_CTL2					0X294
#define IA32_MC21_CTL2					0X295
#define IA32_MC22_CTL2					0X296
#define IA32_MC23_CTL2					0X297
#define IA32_MC24_CTL2					0X298
#define IA32_MC25_CTL2					0X299
#define IA32_MC26_CTL2					0X29A
#define IA32_MC27_CTL2					0X29B
#define IA32_MC28_CTL2					0X29C
#define IA32_MC29_CTL2					0X29D
#define IA32_MC30_CTL2					0X29E
#define IA32_MC31_CTL2					0X29F
#define IA32_MTRR_DEF_TYPE				0X2FF
#define IA32_FIXED_CTR0					0X309
#define MSR_PERF_FIXED_CTR0				0X309
#define IA32_FIXED_CTR1					0X30A
#define MSR_PERF_FIXED_CTR1				0X30A
#define IA32_FIXED_CTR2					0X30B
#define MSR_PERF_FIXED_CTR2				0X30B
#define IA32_PERF_CAPABILITIES			0X345
#define IA32_FIXED_CTR_CTRL				0X38D
#define MSR_PERF_FIXED_CTR_CTRL			0X38D
#define IA32_PERF_GLOBAL_STATUS			0X38E
#define MSR_PERF_GLOBAL_STATUS			0X38E
#define IA32_PERF_GLOBAL_CTRL			0X38F
#define MSR_PERF_GLOBAL_CTRL			0X38F
#define IA32_PERF_GLOBAL_OVF_CTRL		0X390
#define MSR_PERF_GLOBAL_OVF_CTRL		0X390
#define IA32_PEBS_ENABLE				0X3F1
#define IA32_MCx_CTL(x)					(0X400+(x)*4)
#define IA32_MCx_STATUS(x)				(0X401+(x)*4)
#define IA32_MCx_ADDR(x)				(0X402+(x)*4)
#define IA32_MCx_MISC(x)				(0X403+(x)*4)
#define IA32_MC0_CTL					0X400
#define IA32_MC0_STATUS					0X401
#define IA32_MC0_ADDR					0X402
#define IA32_MC0_MISC					0X403
#define IA32_MC1_CTL					0X404
#define IA32_MC1_STATUS					0X405
#define IA32_MC1_ADDR					0X406
#define IA32_MC1_MISC					0X407
#define IA32_MC2_CTL					0X408
#define IA32_MC2_STATUS					0X409
#define IA32_MC2_ADDR					0X40A
#define IA32_MC2_MISC					0X40B
#define IA32_MC3_CTL					0X40C
#define IA32_MC3_STATUS					0X40D
#define IA32_MC3_ADDR					0X40E
#define IA32_MC3_MISC					0X40F
#define IA32_MC4_CTL					0X410
#define IA32_MC4_STATUS					0X411
#define IA32_MC4_ADDR					0X412
#define IA32_MC4_MISC					0X413
#define IA32_MC5_CTL					0X414
#define IA32_MC5_STATUS					0X415
#define IA32_MC5_ADDR					0X416
#define IA32_MC5_MISC					0X417
#define IA32_MC6_CTL					0X418
#define IA32_MC6_STATUS					0X419
#define IA32_MC6_ADDR					0X41A
#define IA32_MC6_MISC					0X41B
#define IA32_MC7_CTL					0X41C
#define IA32_MC7_STATUS					0X41D
#define IA32_MC7_ADDR					0X41E
#define IA32_MC7_MISC					0X41F
#define IA32_MC8_CTL					0X420
#define IA32_MC8_STATUS					0X421
#define IA32_MC8_ADDR					0X422
#define IA32_MC8_MISC					0X423
#define IA32_MC9_CTL					0X424
#define IA32_MC9_STATUS					0X425
#define IA32_MC9_ADDR					0X426
#define IA32_MC9_MISC					0X427
#define IA32_MC10_CTL					0X428
#define IA32_MC10_STATUS				0X429
#define IA32_MC10_ADDR					0X42A
#define IA32_MC10_MISC					0X42B
#define IA32_MC11_CTL					0X42C
#define IA32_MC11_STATUS				0X42D
#define IA32_MC11_ADDR					0X42E
#define IA32_MC11_MISC					0X42F
#define IA32_MC12_CTL					0X430
#define IA32_MC12_STATUS				0X431
#define IA32_MC12_ADDR					0X432
#define IA32_MC12_MISC					0X433
#define IA32_MC13_CTL					0X434
#define IA32_MC13_STATUS				0X435
#define IA32_MC13_ADDR					0X436
#define IA32_MC13_MISC					0X437
#define IA32_MC14_CTL					0X438
#define IA32_MC14_STATUS				0X439
#define IA32_MC14_ADDR					0X43A
#define IA32_MC14_MISC					0X43B
#define IA32_MC15_CTL					0X43C
#define IA32_MC15_STATUS				0X43D
#define IA32_MC15_ADDR					0X43E
#define IA32_MC15_MISC					0X43F
#define IA32_MC16_CTL					0X440
#define IA32_MC16_STATUS				0X441
#define IA32_MC16_ADDR					0X442
#define IA32_MC16_MISC					0X443
#define IA32_MC17_CTL					0X444
#define IA32_MC17_STATUS				0X445
#define IA32_MC17_ADDR					0X446
#define IA32_MC17_MISC					0X447
#define IA32_MC18_CTL					0X448
#define IA32_MC18_STATUS				0X449
#define IA32_MC18_ADDR					0X44A
#define IA32_MC18_MISC					0X44B
#define IA32_MC19_CTL					0X44C
#define IA32_MC19_STATUS				0X44D
#define IA32_MC19_ADDR					0X44E
#define IA32_MC19_MISC					0X44F
#define IA32_MC20_CTL					0X450
#define IA32_MC20_STATUS				0X451
#define IA32_MC20_ADDR					0X452
#define IA32_MC20_MISC					0X453
#define IA32_MC21_CTL					0X454
#define IA32_MC21_STATUS				0X455
#define IA32_MC21_ADDR					0X456
#define IA32_MC21_MISC					0X457
#define IA32_VMX_BASIC					0X480
#define IA32_VMX_PINBASED_CTLS			0X481
#define IA32_VMX_PROCBASED_CTLS			0X482
#define IA32_VMX_EXIT_CTLS				0X483
#define IA32_VMX_ENTRY_CTLS				0X484
#define IA32_VMX_MISC					0X485
#define IA32_VMX_CR0_FIXED0				0X486
#define IA32_VMX_CR0_FIXED1				0X487
#define IA32_VMX_CR4_FIXED0				0X488
#define IA32_VMX_CR4_FIXED1				0X489
#define IA32_VMX_VMCS_ENUM				0X48A
#define IA32_VMX_PROCBASED_CTLS2		0X48B
#define IA32_VMX_EPT_VPID_CAP			0X48C
#define IA32_VMX_TRUE_PINBASED_CTLS		0X48D
#define IA32_VMX_TRUE_PROCBASED_CTLS	0X48E
#define IA32_VMX_TRUE_EXIT_CTLS			0X48F
#define IA32_VMX_TRUE_ENTRY_CTLS		0X490
#define IA32_VMX_VMFUNC					0X491
#define IA32_A_PMC(x)					(0X4C1+(x))
#define IA32_A_PMC0						0X4C1
#define IA32_A_PMC1						0X4C2
#define IA32_A_PMC2						0X4C3
#define IA32_A_PMC3						0X4C4
#define IA32_A_PMC4						0X4C5
#define IA32_A_PMC5						0X4C6
#define IA32_A_PMC6						0X4C7
#define IA32_A_PMC7						0X4C8
#define IA32_MCG_EXT_CTL				0X4D0
#define IA32_RTIT_OUTPUT_BASE			0X560
#define IA32_RTIT_OUTPUT_MASK_PTRS		0X561
#define IA32_RTIT_CTL					0X570
#define IA32_RTIT_STATUS				0X571
#define IA32_RTIT_CR3_MATCH				0X572
#define IA32_DS_AREA					0X600
#define IA32_TSC_DEADLINE				0X6E0
#define IA32_PM_ENABLE					0X770
#define IA32_HWP_CAPABILITIES			0X771
#define IA32_HWP_REQUEST_PKG			0X772
#define IA32_HWP_INTERRUPT				0X773
#define IA32_HWP_REQUEST				0X774
#define IA32_HWP_STATUS					0X777
#define IA32_X2APIC_APICID				0X802
#define IA32_X2APIC_VERSION				0X803
#define IA32_X2APIC_TRP					0X808
#define IA32_X2APIC_PPR					0X80A
#define IA32_X2APIC_EOI					0X80B
#define IA32_X2APIC_LDR					0X80D
#define IA32_X2APIC_SIVR				0X80F
#define IA32_X2APIC_ISR(x)				(0X810+(x))
#define IA32_X2APIC_ISR0				0X810
#define IA32_X2APIC_ISR1				0X811
#define IA32_X2APIC_ISR2				0X812
#define IA32_X2APIC_ISR3				0X813
#define IA32_X2APIC_ISR4				0X814
#define IA32_X2APIC_ISR5				0X815
#define IA32_X2APIC_ISR6				0X816
#define IA32_X2APIC_ISR7				0X817
#define IA32_X2APIC_TMR(x)				(0X818+(x))
#define IA32_X2APIC_TMR0				0X818
#define IA32_X2APIC_TMR1				0X819
#define IA32_X2APIC_TMR2				0X81A
#define IA32_X2APIC_TMR3				0X81B
#define IA32_X2APIC_TMR4				0X81C
#define IA32_X2APIC_TMR5				0X81D
#define IA32_X2APIC_TMR6				0X81E
#define IA32_X2APIC_TMR7				0X81F
#define IA32_X2APIC_IRR(x)				(0X820+(x))
#define IA32_X2APIC_IRR0				0X820
#define IA32_X2APIC_IRR1				0X821
#define IA32_X2APIC_IRR2				0X822
#define IA32_X2APIC_IRR3				0X823
#define IA32_X2APIC_IRR4				0X824
#define IA32_X2APIC_IRR5				0X825
#define IA32_X2APIC_IRR6				0X826
#define IA32_X2APIC_IRR7				0X827
#define IA32_X2APIC_ESR					0X828
#define IA32_X2APIC_LVT_CMCI			0X82F
#define IA32_X2APIC_ICR					0X830
#define IA32_X2APIC_LVT_TIMER			0X832
#define IA32_X2APIC_LVT_THERMAL			0X833
#define IA32_X2APIC_LVT_PMI				0X834
#define IA32_X2APIC_LVT_LINT0			0X835
#define IA32_X2APIC_LVT_LINT1			0X836
#define IA32_X2APIC_LVT_ERROR			0X837
#define IA32_X2APIC_INIT_COUNT			0X838
#define IA32_X2APIC_CUR_COUNT			0X839
#define IA32_X2APIC_SELF_IPI			0X83F
#define IA32_DUBUG_INTERFACE			0XC80
#define IA32_QM_EVSEL					0XC8D
#define IA32_QM_CTR						0XC8E
#define IA32_PQR_ASSOC					0XC8F
#define IA32_L3_QOS_MASK_(x)			(0XC90+(x))
#define IA32_XSS						0XDA0
#define IA32_PKG_HDC_CTL				0XDB0
#define IA32_PM_CTL1					0XDB1
#define IA32_THREAD_STALL				0XDB2
#define IA32_EFER						0XC0000080
#define IA32_STAR						0XC0000081
#define IA32_LSTAR						0XC0000082
#define IA32_FMASK						0XC0000084
#define IA32_FS_BASE					0XC0000100
#define IA32_GS_BASE					0XC0000101
#define IA32_KERNEL_GS_BASE				0XC0000102
#define IA32_TSC_AUX					0XC0000103

struct _REG_STATUS_ {
	u64 rax,rcx,rdx,rbx,err_code,rbp,rsi,rdi;
	u64 r8,r9,r10,r11,r12,r13,r14,r15;
	u64 rip,cs,rflags,rsp,ss;
};

#define RSPL(x)		((x)*2+1)
#define RSPH(x)		((x)*2+2)

#define ISTL(x)		((x)*2+7)
#define ISTH(x)		((x)*2+8)

struct _TSS_64_ {
	u32 reg[25];
	u16 rvd;
	u16 IO_map_base;
	u32 IO_map[2];
};

struct _PRIVATE_DATA_ {
	struct _TSS_64_ TSS;
	u32 cpu_id;
	u32 cpu_lock;
	u32 flags;
	u32 schedule_disable_count;
	u32 int_disable_count;
	int need_schedule;
	struct _PROCESS_ * cur_process;
	struct _THREAD_ * cur_thread;
	int cpu_time;
};

#include <string.h>

static void inline copy_reg_status(struct _REG_STATUS_ * des,struct _REG_STATUS_  * scr){
	memcpy(des,scr,sizeof(struct _REG_STATUS_));
}

void make_error();

int set_ist(int vector,int ist,u64 rsp);
int enable_ist(int vector);
u16 put_TSS(struct _TSS_64_ * TSS);


int suppose_syscall;
int suppose_sysenter;


#define CPU_FLAGS_SCHEDULE_DISABLE		0x00000001
#define CPU_FLAGS_NEED_SCHEDULE			0x00000002


#define read_private_byte(m)		get_gs_byte(offsetof(struct _PRIVATE_DATA_,m))
#define read_private_word(m)		get_gs_word(offsetof(struct _PRIVATE_DATA_,m))
#define read_private_dword(m)		get_gs_dword(offsetof(struct _PRIVATE_DATA_,m))
#define read_private_qword(m)		get_gs_qword(offsetof(struct _PRIVATE_DATA_,m))
#define write_private_byte(m,v)		put_gs_byte(offsetof(struct _PRIVATE_DATA_,m),(v))
#define write_private_word(m,v)		put_gs_word(offsetof(struct _PRIVATE_DATA_,m),(v))
#define write_private_dword(m,v)	put_gs_dword(offsetof(struct _PRIVATE_DATA_,m),(v))
#define write_private_qword(m,v)	put_gs_qword(offsetof(struct _PRIVATE_DATA_,m),(v))
#define lock_bts_private(m,i)		lock_gs_bts(offsetof(struct _PRIVATE_DATA_,m),i)
#define lock_btr_private(m,i)		lock_gs_btr(offsetof(struct _PRIVATE_DATA_,m),i)
#define lock_bt_private(m,i)		gs_bt(offsetof(struct _PRIVATE_DATA_,m),i)
#define RPB(M)		read_private_byte(M)
#define RPW(M)		read_private_word(M)
#define RPD(M)		read_private_dword(M)
#define RPQ(M)		read_private_qword(M)
#define WPB(M,V)	write_private_byte(M,V)
#define WPW(M,V)	write_private_word(M,V)
#define _WPD(M,V)	write_private_dword(M,V)
#define WPQ(M,V)	write_private_qword(M,V)

#define SetKerSP(p)	{_WPD(TSS.reg[RSPL(0)],p);_WPD(TSS.reg[RSPL(0)],(u64)p >> 32);}
#define GetKerSP()	({register u32 l,h;l = RPD(TSS.reg[RSPL(0)]);\
	h = RPD(TSS.reg[RSPH(0)]);(((u64)l) | (((u64)h) << 32));})
#define SetIST(i,r)	{_WPD(TSS.reg[ISTL(i)],r);_WPD(TSS.reg[ISTH(i)],((u64)(r))>>32);}
#define GetIST(i)	({register u32 l,h;l = RPD(TSS.reg[ISTL(i)]);\
	h = RPD(TSS.reg[ISTH(i)]);(((u64)l) | (((u64)h) << 32));})
#define GetCPUId()	read_private_dword(cpu_id)

#undef RPB
#undef RPW
#undef RPD
#undef RPQ
#undef WPB
#undef WPW
#undef WPD
#undef WPQ

#endif