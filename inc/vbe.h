//Lava OS
//PageFault
//20-04-09-15-43

#ifndef _VBE_H_
#define _VBE_H_

#include <stddef.h>

struct VESA_INFO{
	u32 sign;
	u16 version;
	u16 oem_str_bas;
	u16 oem_str_seg;
	u8 cap0;
	u8 cap1;
	u8 cap2;
	u8 cap3;
	u16 mode_base;
	u16 mode_seg;
	u16 mem_size;
	u16 soft_ver;
	u16 oem_vendor_name_bas;
	u16 oem_vendor_name_seg;
	u16 oem_product_name_bas;
	u16 oem_product_name_seg;
	u16 oem_product_rev_bas;
	u16 oem_product_rev_seg;
	u8 rvd[222];
	u8 rvd2[256];
};
struct VESA_MODE{
	u16 mode_attr;u8 win_a_attr,win_b_attr;
	u16 granulariry,size;
	u16 win_a_seg,win_b_seg;
	u32 fun_ptr;
	u16 byte_per_scan_line,xres;
	u16 yres;u8 xchar_size,ychar_size;
	u8 plaes,bits_per_pixel,banks,memory_model;
	u8 bank_size,pages,rvd,red_mask_size;
	u8 red_pos,grees_mask_size,green_pos,blue_mask_size;
	u8 blue_pos,rvd_mask_size,rvd_pos,color_mode_attr;
	u32 lfb;
	u32 rvd2;
	u16 rvd3,lin_byte_per_scan_line;
	u8 b_images,l_images,l_red_mask_size,l_red_pos;
	u8 l_green_mask_size,l_green_pos,l_blue_mask_size,l_blue_pos;
	u8 l_rvd_mask_size,l_rvd_pos;u16 mask_pixel_clock_l;
	u16 mask_pixel_clock_h,rvd4;
	u8 rvd5[188];
};




#endif