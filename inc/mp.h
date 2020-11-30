/*
	inc/mp.h
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

#ifndef _MP_H_
#define _MP_H_

struct _MPFT_ {
	uint32_t Sign;//0x5f4d505f
	uint32_t Addr;
	uint8_t Length;
	uint8_t Revision;
	uint8_t CheckSum;
	uint8_t Feature[5];
};
struct _MPCTH_ {
	u32 sign;//0x504d4350
	u16 tab_len;
	u8 rev;
	u8 check_sum;
	char oem_id[8];
	char prod_id[12];
	u32 oem_ptr;
	u16 oem_tab_len;
	u16 ent_count;
	u32 lapic_mmio;
	u16 ext_tab_len;
	u8 ext_tab_chk_sum;
	u8 rvd;
};
struct _MPS_CPU_ {
	u8 type;//0
	u8 id;
	u8 ver;
	u8 cpu_flags;
	u32 cpu_sign;
	u32 flags;
	u32 rvd0;
	u32 rvd1;
};
struct _MPS_BUS_ {
	u8 type;//1
	u8 id;
	char str[6];
};
struct _MPS_IOAPIC_ {
	u8 type;//2
	u8 id;
	u8 ver;
	u8 flags;
	u32 mmio;
};
struct _MPS_IO_INT_ {
	u8 type;//3
	u8 int_type;
	u16 flags;
	u8 sor_bus_id;
	u8 sor_bus_irq;
	u8 dst_ioapic_id;
	u8 dst_ioapic_int;
};
struct _MPS_LINT_ASGN_{
	u8 type;//4
	u8 int_type;
	u16 flags;
	u8 sor_bus_id;
	u8 sor_bus_irq;
	u8 dst_ioapic_id;
	u8 dst_ioapic_int;
};
struct _MPS_MAP_ {
	u8 type;//128
	u8 len;
	u8 bus_id;
	u8 addr_type;
	u32 abl;
	u32 abh;
	u32 all;
	u32 alh;
};
struct _MPS_BHD_ {
	u8 type;//129
	u8 len;//8
	u8 bus_id;
	u8 bus_info;
	u8 parent_bus;
	u8 rvd[3];
};
struct _MPS_CBASM_{
	u8 type;//130
	u8 len;//8
	u8 bus_id;
	u8 mod;//
	u32 list;
};

#endif