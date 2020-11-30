/*
	inc/acpi.h
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

#ifndef _ACPI_H_
#define _ACPI_H_

#include <stddef.h>

struct _RSDP_ {
	u64 sign;//"RSD PTR "
	u8 check;
	char oem_id[6];
	u8 revision;
	u32 rsdt_addr;
	u32 length;
	u64 xsdt_addr;
	u8 xcheck;
	u8 rvd[3];
};

struct _DESC_HEAD_ {
	u32 sign;
	u32 length;
	u8 revision;
	u8 check;
	char oem_id[6];
	u64 oem_table_id;
	u32 oem_revision;
	u32 creator_id;
	u32 creator_revision;
};

struct _RSDT_HEAD_ {
	struct _DESC_HEAD_ head;//'RSDT'
	u32 entry[];
};
struct _XSDT_HEAD_ {
	struct _DESC_HEAD_ head;//'XSDT'
	u64 entry[];
};
struct _FADT_ {
	struct _DESC_HEAD_ head;//'FACP'
	u32 firmware_ctrl;
	u32 dsdt;
	u8 rvd0;
	u8 preferend_pm_profile;
	u16 sci_int;
	u32 smi_cmd;
	u8 acpi_enable;
	u8 acpi_disable;
	u8 s2bios_req;
	u8 pstate_cnt;
	u32 pm1a_evt_blk;
	u32 pm1b_evt_blk;
	u32 pm1a_cnt_blk;
	u32 pm1b_cnt_blk;
	u32 pm2_cnt_blk;
	u32 pm_tmr_blk;
	u32 gpe0_blk;
	u32 gpe1_blk;
	u8 pm1_evt_len;
	u8 pm1_cnt_len;
	u8 pm2_cnt_len;
	u8 pm_tmr_len;
	u8 gpe0_blk_len;
	u8 gpe1_blk_len;
	u8 gpe1_base;
	u8 cst_cnt;
	u16 p_lvl2_lat;
	u16 p_lvl3_lat;
	u16 flush_size;
	u16 flush_stride;
	u8 duty_offset;
	u8 duty_width;
	u8 day_alarm;
	u8 mon_alarm;
	u8 century;
	u8 iapc_boot_arch_l;
	u8 iapc_boot_arch_h;
	u8 rvd1;
	u32 flags;
	u32 reset_reg_l;
	u32 reset_reg_m;
	u32 reset_reg_h;
	u8 reset_value;
	u8 arm_boot_arch_l;
	u8 arm_boot_arch_h;
	u8 fadt_minor_version;
	u64 x_firmware_ctrl;
	u64 x_dsdt;
	u32 x_pm1a_evt_blk_l;
	u32 x_pm1a_evt_blk_m;
	u32 x_pm1a_evt_blk_h;
	u32 x_pm1b_evt_blk_l;
	u32 x_pm1b_evt_blk_m;
	u32 x_pm1b_evt_blk_h;
	u32 x_pm1a_cnt_blk_l;
	u32 x_pm1a_cnt_blk_m;
	u32 x_pm1a_cnt_blk_h;
	u32 x_pm1b_cnt_blk_l;
	u32 x_pm1b_cnt_blk_m;
	u32 x_pm1b_cnt_blk_h;
	u32 x_pm2_cnt_blk_l;
	u32 x_pm2_cnt_blk_m;
	u32 x_pm2_cnt_blk_h;
	u32 x_pm_tmr_blk_l;
	u32 x_pm_tmr_blk_m;
	u32 x_pm_tmr_blk_h;
	u32 x_gpe0_blk_l;
	u32 x_gpe0_blk_m;
	u32 x_gpe0_blk_h;
	u32 x_gpe1_blk_l;
	u32 x_gpe1_blk_m;
	u32 x_gpe1_blk_h;
	u32 sleep_control_reg_l;
	u32 sleep_control_reg_m;
	u32 sleep_control_reg_h;
	u32 sleep_status_reg_l;
	u32 sleep_status_reg_m;
	u32 sleep_status_reg_h;
	u64 hypervisor_vendor_identity;
};
#define FACP_FLAGS_WBINVD								0x00000001
#define FACP_FLAGS_WBINVD_FLUSH							0x00000002
#define FACP_FLAGS_PROC_C1								0x00000004
#define FACP_FLAGS_P_LVL2_UP							0x00000008
#define FACP_FLAGS_PWR_BUTTON							0x00000010
#define FACP_FLAGS_SLP_BUTTON							0x00000020
#define FACP_FLAGS_FIX_RTC								0x00000040
#define FACP_FLAGS_RTC_S4								0x00000080
#define FACP_FLAGS_TMR_VAL_EXT							0x00000100
#define FACP_FLAGS_DCK_CAP								0x00000200
#define FACP_FLAGS_REST_REG_SUP							0x00000400
#define FACP_FLAGS_SEALED_CASE							0x00000800
#define FACP_FLAGS_HEADLESS								0x00001000
#define FACP_FLAGS_CPU_SW_SLP							0x00002000
#define FACP_FLAGS_PCI_ECP_WAK							0x00004000
#define FACP_FLAGS_USE_PLATFORM_CLOCK					0x00008000
#define FACP_FLAGS_S4_RTC_STS_VAILD						0x00010000
#define FACP_FLAGS_REMOTE_POWER_ON_CAPABLE				0x00020000
#define FACP_FLAGS_FORCE_APIC_CLUSTER_MODEL				0x00040000
#define FACP_FLAGS_FORCE_APIC_PHYSICAL_ESTINATION_MODE	0x00080000
#define FACP_FLAGS_HW_REDUCE_AAPC						0x00100000
#define FACP_FLAGS_LOW_POWER_S0_IDLE_CAPABLE			0x00200000

#define FACP_IAPC_LEGACY_DEVICE							0x0001
#define FACP_IAPC_8042									0x0002
#define FACP_IAPC_NO_VGA								0x0004
#define FACP_IAPC_NO_MSI								0x0008
#define FACP_IAPC_PCIE_ASPM_CONTROL						0x0010
#define FACP_IAPC_NO_CMOS								0x0020

#endif