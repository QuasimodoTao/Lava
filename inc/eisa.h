/*
	inc/isa.h
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
#ifndef _EISA_H_
#define _EISA_H_

enum ISA {
    EISA_TIMER_IRQ =        0,
    EISA_KEYBOARD_IRQ =     1,
    EISA_SERIAL2_IRQ =      3,
    EISA_SERIAL1_IRQ =      4,
    EISA_PARALLEL2_IRQ =    5,
    EISA_FLOPPY_IRQ =       6,
    EISA_PARALLEL1_IRQ =    7,
    EISA_RTC_IRQ =          8,
    EISA_MOUSE_IRQ =        12,
    EISA_HD1_IRQ =          14,
    EISA_HD2_IRQ =          15,

    EISA_DMA0_ADDR_PORT =   0x0000,
    EISA_DMA0_CNT_PORT =    0x0001,
    EISA_DMA0_STATUS_PORT = 0x0008,
    EISA_DMA0_CTRL_PORT =   0x0008,
    EISA_DMA0_DRQ_PORT =    0x0009,
    EISA_DMA0_MASK_PORT =   0x000a,
    EISA_DMA0_MODE_PORT =   0x000b,
    EISA_DMA0_CLR_PORT =    0x000c,
    EISA_DMA0_MASTER_PORT = 0x000d,
    EISA_DMA0_RESET_PORT =  0x000e,
    EISA_DMA0_GER_MASK_PORT = 0x000f,
    EISA_DMA0_PAGE_PORT =   0x0087,
    EISA_DMA1_ADDR_PORT =   0x0002,
    EISA_DMA1_CNT_PORT =    0x0003,
    EISA_DMA1_STATUS_PORT = 0x0008,
    EISA_DMA1_CTRL_PORT =   0x0008,
    EISA_DMA1_DRQ_PORT =    0x0009,
    EISA_DMA1_MASK_PORT =   0x000a,
    EISA_DMA1_MODE_PORT =   0x000b,
    EISA_DMA1_CLR_PORT =    0x000c,
    EISA_DMA1_MASTER_PORT = 0x000d,
    EISA_DMA1_RESET_PORT =  0x000e,
    EISA_DMA1_GER_MASK_PORT = 0x000f,
    EISA_DMA1_PAGE_PORT =   0x0083,
    EISA_DMA2_ADDR_PORT =   0x0004,
    EISA_DMA2_CNT_PORT =    0x0005,
    EISA_DMA2_STATUS_PORT = 0x0008,
    EISA_DMA2_CTRL_PORT =   0x0008,
    EISA_DMA2_DRQ_PORT =    0x0009,
    EISA_DMA2_MASK_PORT =   0x000a,
    EISA_DMA2_MODE_PORT =   0x000b,
    EISA_DMA2_CLR_PORT =    0x000c,
    EISA_DMA2_MASTER_PORT = 0x000d,
    EISA_DMA2_RESET_PORT =  0x000e,
    EISA_DMA2_GER_MASK_PORT = 0x000f,
    EISA_DMA2_PAGE_PORT =   0x0081,
    EISA_DMA3_ADDR_PORT =   0x0006,
    EISA_DMA3_CNT_PORT =    0x0007,
    EISA_DMA3_STATUS_PORT = 0x0008,
    EISA_DMA3_CTRL_PORT =   0x0008,
    EISA_DMA3_DRQ_PORT =    0x0009,
    EISA_DMA3_MASK_PORT =   0x000a,
    EISA_DMA3_MODE_PORT =   0x000b,
    EISA_DMA3_CLR_PORT =    0x000c,
    EISA_DMA3_MASTER_PORT = 0x000d,
    EISA_DMA3_RESET_PORT =  0x000e,
    EISA_DMA3_GER_MASK_PORT = 0x000f,
    EISA_DMA3_PAGE_PORT =   0x0082,
    
    EISA_PIC0_IRR_PORT =    0x0020,
    EISA_PIC0_ISR_PORT =    0x0020,
    EISA_PIC0_ICW1_PORT =   0x0020,
    EISA_PIC0_OCW2_PORT =   0x0020,
    EISA_PIC0_OCW3_PORT =   0x0020,
    EISA_PCI0_ICW2_PORT =   0x0021,
    EISA_PCI0_ICW3_PORT =   0x0021,
    EISA_PCI0_ICW4_PORT =   0x0021,
    EISA_PCI0_OCW1_PORT =   0x0021,
    EISA_PIC1_IRR_PORT =    0x00a0,
    EISA_PIC1_ISR_PORT =    0x00a0,
    EISA_PIC1_ICW1_PORT =   0x00a0,
    EISA_PIC1_OCW2_PORT =   0x00a0,
    EISA_PIC1_OCW3_PORT =   0x00a0,
    EISA_PCI1_ICW2_PORT =   0x00a1,
    EISA_PCI1_ICW3_PORT =   0x00a1,
    EISA_PCI1_ICW4_PORT =   0x00a1,
    EISA_PCI1_OCW1_PORT =   0x00a1,

    EISA_PIT1_CH0_PORT =    0x0040,
    EISA_PIT1_CH1_PORT =    0x0041,
    EISA_PIT1_CH2_PORT =    0x0042,
    EISA_PIT1_CTRL_PORT =   0x0043,
    EISA_PIT2_CH0_PORT =    0x0048,
    EISA_PIT2_CH1_PORT =    0x0049,
    EISA_PIT2_CH2_PORT =    0x004a,
    EISA_PIT2_CTRL_PORT =   0x004b,
        EISA_PIT_SELECT_CH0 =   0x00,
        EISA_PIT_SELECT_CH1 =   0x40,
        EISA_PIT_SELECT_CH2 =   0x80,
        EISA_PIT_READ_BACK =    0xc0,
        EISA_PIT_LATCH =        0x00,
        EISA_PIT_LOW =          0x10,
        EISA_PIT_HI =           0x20,
        EISA_PIT_LOW_HI =       0x30,
        EISA_PIT_MODE0 =        0x00,
        EISA_PIT_MODE1 =        0x02,
        EISA_PIT_MODE2 =        0x04,
        EISA_PIT_MODE3 =        0x06,
        EISA_PIT_MODE4 =        0x08,
        EISA_PIT_MODE5 =        0x0a,
        EISA_PIT_BIN =          0x00,
        EISA_PIT_BCD =          0x01,

    EISA_KBD_DATA_PORT =    0x0060,
    EISA_KBD_CMD_STATUS_PORT = 0x0064,
    EISA_MOUSE_DATA_PORT =  0x0060,
    EISA_MOUSE_CMD_STATUS_PORT = 0x0064,
        EISA_8042_READ_REG =    0x20,
        EISA_8042_WRITE_REG =   0x60,
        EISA_8042_TEST_PASSWORD = 0xa4,
            EISA_8042_PASSWORD_INSTALL = 0xfa,
            EISA_8042_PASSWORD_UNINSTALL = 0xf1,
        EISA_8042_LOAD_PASSWORD = 0xa5,
        EISA_8042_ENABLE_PASSWORD = 0xa6,
        EISA_8042_DISABLE_MOUSE = 0xa7,
        EISA_8042_ENABLE_MOUSE = 0xa8,
        EISA_8042_MOUSE_TEST =  0xa9,
        EISA_8042_SELF_TEST =   0xaa,
        EISA_8042_KBY_TEST =    0xab,
        EISA_8042_KBD_DISABLE = 0xad,
        EISA_8042_KDB_ENABLE =  0xae,
        EISA_8042_READ_INPUT =  0xc0,
        EISA_8042_POLL_LOW =    0xc1,
        EISA_8042_POLL_HIGH =   0xc2,
        EISA_8042_READ_OUTPUT = 0xd0,
        EISA_8042_WRITE_OUTPUT = 0xd1,
        EISA_8042_WRITE_KBD_OUTPUT = 0xd2,
        EISA_8042_WRITE_MOUSE_OUTPUT = 0xd3,
        EISA_8042_WRITE_MOUSE = 0xd4,
        EISA_8042_WRITE_TEST_INPUT = 0xe0,
        EISA_8042_SET_LED =     0xed,
        EISA_8042_PULSED_LOW =  0xf0,
        EISA_8042_OUT_BUF_FULL = 0x01,
        EISA_8042_IN_BUF_FULL = 0x02,

    EISA_SYS_CTRL_A_PORT =  0x0092,
    EISA_SYS_CTRL_B_PORT =  0x0061,
    EISA_SYS_CTRL_B_STATUS_PORT = 0x0061,
        EISA_ALTER_HOT_RESET =  0x01,
        EISA_ALTER_A20 =        0x02,
        EISA_SECURITY_LOCK_LATCH = 0x08,
        EISA_WATCHDOD_STATUS =  0x10,
        EISA_HD_LED0 =          0x40,
        EISA_HD_LED1 =          0x80,
        ESIA_TIMER2_TO_SPEAKER = 0x01,
        ESIA_SPEAKER_DATA_ENABLE = 0x02,
        EISA_PARITY_CHECK_ENABLE = 0x04,
        EISA_CHANNEL_CHECK_ENABLE = 0x08,
        ESIA_REFRESH_REQUEST =  0x10,
        EISA_TIMER2_OUTPUT =    0x20,
        EISA_CHANNEL_CHECK =    0x40,
        EISA_BOARD_RAM_PARTY_CHECK = 0x80,

    EISA_CMOS_ADDR_PORT =   0x0070,
    EISA_CMOS_DATA_PORT =   0x0071,
        EISA_CMOS_SECOND =      0x00,
        EISA_CMOS_SECOND_ALARM = 0x01,
        EISA_CMOS_MINUTE =      0x02,
        EISA_CMOS_MINUTE_ALARM = 0x03,
        EISA_CMOS_HOUR =        0x04,
        EISA_CMOS_HOUR_ALARM =  0x05,
        EISA_CMOS_WEEK_DAY =    0x06,
        EISA_CMOS_MONTH_DAY =   0x07,
        EISA_CMOS_MONTH =       0x08,
        EISA_CMOS_YEAR =        0x09,
        EISA_CMOS_RTC_A =       0x0a,
        EISA_CMOS_RTC_B =       0x0b,
        EISA_CMOS_RTC_C =       0x0c,
        EISA_CMOS_RTC_D =       0x0d,
            EISA_CMOS_RTC_RATE =    0x0f,
            EISA_CMOS_RTC_DIV =     0x70,
            EISA_CMOS_RTC_UPDATE =  0x80,
            EISA_CMOS_RTC_DAYLIGH_SCAING_ENABLE = 0x01,
            EISA_CMOS_RTC_24_MODE = 0x02,
            EISA_CMOS_RTC_DATE_MODE = 0x04,
            EISA_CMOS_RTC_SQUARE_ENABLE = 0x08,
            EISA_CMOS_RTC_UPDATA_ENABLE = 0x10,
            EISA_CMOS_RTC_ALARM_ENABLE = 0x20,
            EISA_CMOS_RTC_PERIODIC_ENABLE = 0x40,
            EISA_CMOS_RTC_SET =     0x80,
            EISA_CMOS_RTC_UPDATE_FLAG = 0x10,
            EISA_CMOS_RTC_ALARM_FLAG = 0x20,
            EISA_CMOS_RTC_PERIODIC_FLAG = 0x40,
            EISA_CMOS_RTC_INT_REQ = 0x80,
            EISA_CMOS_RAM_VAILD = 0x80,
        EISA_CMOS_DIAGN_STATUS = 0x0e,
        EISA_CMOS_RESET_CMOD =  0x0f,
        EISA_CMOS_FD_TYPE =     0x10,
        EISA_CMOS_HD_TYPE =     0x12,
        EISA_CMOS_EQUIPEMENT =  0x14,
        EISA_CMOS_BMEM_SIZE_LOW = 0x15,
        EISA_CMOS_BMEM_SIZE_HI = 0x16,
        EISA_CMOS_EMEM_SIZE_LOW = 0x17,
        EISA_CMOS_EMEM_SIZE_HI = 0x18,
        EISA_CMOS_ADD_FLAGS =   0x2d,
        EISA_CMOS_1MEM_SIZE_LOW = 0x30,
        EISA_CMOS_1MEM_SIZE_HI = 0x31,
        EISA_CMOS_CENTURY =     0x32,
        EISA_CMOS_SYSTEM_INFO = 0x33,

    EISA_DMA4_ADDR_PORT =   0x00c0,
    EISA_DMA4_CNT_PORT =    0x00c2,
    EISA_DMA4_CTRL_PORT =   0x00d0,
    EISA_DMA4_STATUS_PORT = 0x00d0,
    EISA_DMA4_DRQ_PORT =    0x00d2,
    EISA_DMA4_MASK_PORT =   0x00d4,
    EISA_DMA4_MODE_PORT =   0x00d6,
    EISA_DMA4_CLR_PORT =    0x00d8,
    EISA_DMA4_MASTER_PORT = 0x00da,
    EISA_DMA4_RESET_PORT =  0x00dc,
    EISA_DMA4_GER_MASK_PORT = 0x00de,
    EISA_DMA4_PAGE_PORT =   0x008f,
    EISA_DMA5_ADDR_PORT =   0x00c4,
    EISA_DMA5_CNT_PORT =    0x00c6,
    EISA_DMA5_CTRL_PORT =   0x00d0,
    EISA_DMA5_STATUS_PORT = 0x00d0,
    EISA_DMA5_DRQ_PORT =    0x00d2,
    EISA_DMA5_MASK_PORT =   0x00d4,
    EISA_DMA5_MODE_PORT =   0x00d6,
    EISA_DMA5_CLR_PORT =    0x00d8,
    EISA_DMA5_MASTER_PORT = 0x00da,
    EISA_DMA5_RESET_PORT =  0x00dc,
    EISA_DMA5_GER_MASK_PORT = 0x00de,
    EISA_DMA5_PAGE_PORT =   0x008b,
    EISA_DMA6_ADDR_PORT =   0x00c8,
    EISA_DMA6_CNT_PORT =    0x00ca,
    EISA_DMA6_CTRL_PORT =   0x00d0,
    EISA_DMA6_STATUS_PORT = 0x00d0,
    EISA_DMA6_DRQ_PORT =    0x00d2,
    EISA_DMA6_MASK_PORT =   0x00d4,
    EISA_DMA6_MODE_PORT =   0x00d6,
    EISA_DMA6_CLR_PORT =    0x00d8,
    EISA_DMA6_MASTER_PORT = 0x00da,
    EISA_DMA6_RESET_PORT =  0x00dc,
    EISA_DMA6_GER_MASK_PORT = 0x00de,
    EISA_DMA6_PAGE_PORT =   0x0089,
    EISA_DMA7_ADDR_PORT =   0x00cc,
    EISA_DMA7_CNT_PORT =    0x00ce,
    EISA_DMA7_CTRL_PORT =   0x00d0,
    EISA_DMA7_STATUS_PORT = 0x00d0,
    EISA_DMA7_DRQ_PORT =    0x00d2,
    EISA_DMA7_MASK_PORT =   0x00d4,
    EISA_DMA7_MODE_PORT =   0x00d6,
    EISA_DMA7_CLR_PORT =    0x00d8,
    EISA_DMA7_MASTER_PORT = 0x00da,
    EISA_DMA7_RESET_PORT =  0x00dc,
    EISA_DMA7_GER_MASK_PORT = 0x00de,
    EISA_DMA7_PAGE_PORT =   0x008a,

    EISA_NUM_PROC_RESTART_PORT = 0x00f1,

    EISA_PARALLEL1_PORT_BASE = 0x03bc,
    ESIA_PARALLEL2_PORT_BASE = 0x03f8,
    EISA_PARALLEL3_PORT_BASE = 0x0278,
        EISA_PARALLET_DATA_PORT = 0,
        EISA_PARALLET_STATUS_PORT = 1,
        EISA_PARALLET_CMD_PORT = 2,

    EISA_SERIAL1_PORT_BASE = 0x03f8,
    EISA_SERIAL2_PORT_BASE = 0x02f8,
    EISA_SERIAL3_PORT_BASE = 0x3220,
    EISA_SERIAL4_PORT_BASE = 0x3228,
    EISA_SERIAL5_PORT_BASE = 0x4220,
    EISA_SERIAL6_PORT_BASE = 0x4228,
    EISA_SERIAL7_PORT_BASE = 0x5220,
    EISA_SERIAL8_PORT_BASE = 0x5228,
    EISA_SERIAL9_PORT_BASE = 0x03e8,
    EISA_SERIAL10_PORT_BASE = 0x02e8,
        EISA_SERIAL_DATA_PORT = 0,
        EISA_SERIAL_DIV_LOW_PORT = 0,
        EISA_SERIAL_DIV_HIG_PORT = 1,
        EISA_SERIAL_IE_PORT =   1,
        EISA_SERIAL_INT_ID_PORT = 2,
        EISA_SERIAL_FIFO_CTRL_PORT = 2,
        EISA_SERIAL_LINE_CTRL_PORT = 3,
        EISA_SERIAL_MODE_CTRL_PORT = 4,
        EISA_SERIAL_LINE_STATUS_PORT = 5,
        EISA_SERIAL_MODE_STATUS_PORT = 6,
        EISA_SERIAL_SCRETCH_PORT = 7,
            EISA_SERIAL_WORD_LEN_5 = 0x00,
            EISA_SERIAL_WORD_LEN_6 = 0x01,
            EISA_SERIAL_WORD_LEN_7 = 0x02,
            EISA_SERIAL_WORD_LEN_8 = 0x03,
            EISA_SERIAL_STOP_LEN_1 = 0x00,
            EISA_SERIAL_STOP_LEN_2 = 0x04,
            EISA_SERIAL_PARITY_EN = 0x08,
            EISA_SERIAL_PARITY_ODD = 0x00,
            EISA_SERIAL_PARITY_EVEN = 0x10,
            EISA_SERIAL_PARITY_MARK = 0x20,
            EISA_SERIAL_PARITY_SPACE = 0x30,
            EISA_SERIAL_SET_BACK =  0x40,
            EISA_SERIAL_DLA =       0x80,
            EISA_SERIAL_INT_ARISE = 0x01,
            EISA_SERIAL_INT_ID_MASK = 0x06,
            EISA_SERIAL_RECIVE_DATA = 0x01,
            EISA_SERIAL_TRANSMIT =  0x02,
            EISA_SERIAL_LINE_STATUS = 0x04,
            EISA_SERIAL_MODEM_STATUS = 0x08,

    EISA_VGA_CRT_CTRL_ADDR_PORT = 0x03b4,
    EISA_VGA_CRT_CTRL_DATA_PORT = 0x03b5,
    EISA_VGA_INPUT_STATUS1_PORT = 0x03ba,
    EISA_VGA_FEATURE_CTRL_PORT = 0x03ba,
    EISA_VGA_ATTR_CTRL_ADDR_PORT = 0x03c0,
    EISA_VGA_ATTR_CTRL_WDATA_PORT = 0x03c0,
    EISA_VGA_ATTR_CTRL_RDATA_PORT = 0x03c1,
    EISA_VGA_W_OUTPUT_PORT =  0x03c2,
    EISA_VGA_INPUT_STATUS_PORT = 0x03c2,
    EISA_VGA_SUBSYS_EN_PORT = 0x03c3,
    EISA_VGA_SEQ_ADDR_PORT = 0x03c4,
    EISA_VGA_SEQ_DATA_PORT = 0x03c5,
    EISA_VGA_DAC_PIXEL_MASK_PORT = 0x03c6,
    EISA_VGA_DAC_PALETTE_RADDR_PORT = 0x03c7,
    EISA_VGA_DAC_STATUS_PORT = 0x03c7,
    EISA_VGA_DAC_PALETTE_WADDR_PORT = 0x03c8,
    EISA_VGA_DAC_PALETTE_DATA_PORT = 0x03c9,
    EISA_VGA_FEATURE_PORT = 0x03ca,
    EISA_VGA_R_OUTPUT_PORT = 0x03cc,
    EISA_VGA_GRAPH_CTRL_ADDR_PORT = 0x03ce,
    EISA_VGA_GRAPH_CTRL_DATA_PORT = 0x03cf,
    EISA_VGA_CRT_COLOR_CTRL_ADDR_PORT = 0x03d4,
    EISA_VGA_CRT_COLOR_CTRL_DATA_PORT = 0x03d5,
    EISA_VGA_COLOR_INPUT_STATUS_PORT = 0x03da,
    EISA_VGA_COLOR_FEATURE_CTRL_PORT = 0x03da,

    EISA_FLOPPY_STATUS_A_PORT = 0x03f0,
    EISA_FLOPPY_STATUS_B_PORT = 0x03f1,
    EISA_FLOPPY_DOR_PORT =  0x03f2,
    EISA_FLOPPY_FDC_PORT =  0x03f4,
    EISA_FLOPPY_DATA_PORT = 0x03f5,
    EISA_FLOPPY_DIR_PORT =  0x03f7,
    EISA_FLOPPY_CCR_PORT =  0x03f7,

    EISA_HD1_PORT_BASE =    0x01f0,
    EISA_HD2_PORT_BASE =    0x0170,
    EISA_HD1_CTRL_PORT =    0x03f6,
    EISA_HD2_CTRL_PORT =    0x0376,
        EISA_HD_DATA_PORT =     0x00,
        EISA_HD_ERROR_PORT =    0x01,
        EISA_HD_FEATURE_PORT =  0x01,
        EISA_HD_SEC_CNT_PORT =  0x02,
        EISA_HD_LBA0_PORT =     0x03,
        EISA_HD_LBA1 =          0x04,
        EISA_HD_LBA2 =          0x05,
        EISA_HD_HDDEVSEL =      0x06,
        EISA_HD_COMMAND =       0x07,
        EISA_HD_STATUS =        0x07   
};

#endif