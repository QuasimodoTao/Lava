/*
	inc/ata.h
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

#ifndef _ATA_H_
#define _ATA_H_
#include <stddef.h>

#define AP_GENERAL_CONFIG					0
#define APGC_RESPONSE_INCOMPLETE	0x0004
#define APGC_ATA_DEVICE				0x8000
#define AP_SPECIFIC_CONFIG					2
#define AP_SERIAL_NUMBER					10
#define AP_SERIAL_NUMBER_LEN		10
#define AP_FIRMWARE_REVISION				23
#define AP_FIRMWARE_REVISION_LEN	4
#define AP_MODEL_NUMBER						27
#define AP_MODEL_NUMBER_LEN			20
#define AP_SECTORS_PER_TRANSFER				47
#define APSPT_MASK					0x00ff
#define AP_TRUST_FEATURE					48
#define AP_TRUSE_FEATURE_SUP		0x0001
#define AP_CAP1								49
#define APC_DMA_SUPPOSE				0x0100
#define APC_LBA_SUPPOSE				0x0200
#define APC_IORDY_DISABLE			0x0400
#define APC_IORDY_SUPPOSE			0x0800
#define APC_STANDBY_TMR_SUP			0x2000
#define AP_CAP2								50
#define APC_STANDBY_TMR_MIN			0x0001
#define AP_CAP3								53
#define APC_REPORT70_64_VAILD		0x0002
#define APC_REPORT88_VAILD			0x0004
#define APC_FREE_FALL_CONTROL_SEN	0xff00
#define AP_CUR_SECTORS_PER_TRANSFER			59
#define APCSPT_MASK					0x00ff
#define APCSPT_MULT_LOG_SEC_VAILD	0x0100
#define AP_SEC_CNT_28_0						60
#define AP_SEC_CNT_28_1						61
#define AP_MULT_DMA_FEA						63
#define APMDF_MODE_0_SUP			0x0001
#define APMDF_MODE_1_SUP			0x0002
#define APMDF_MODE_2_SUP			0x0004
#define APMDF_MODE_0_SEL			0x0100
#define APMDF_MODE_1_SEL			0x0200
#define APMDF_MODE_2_SEL			0x0400
#define AP_PIO_SUPPOSE						64
#define APPS_MASK					0x00ff
#define AP_MIN_DMA_CYCLE					65
#define AP_MAN_REC_DMA_CYCLE				66
#define AP_MIN_PIO_CYCLE					67
#define AP_MIN_PIO_IORDY_CYCLE				68
#define AP_QUEUE_DEPTH						75
#define APQD_MASK					0x001f
#define AP_SATA_CAP							76
#define APSC_GEN1_SUPPOSE			0x0002
#define APSC_GEN2_SUPPOSE			0x0004
#define APSC_NCQ_SUPPOSE			0x0100
#define APSC_HOST_POWER_INIT_SUP	0x0200
#define APSC_PHY_EVENT_CNT_SUP		0x0400
#define AP_SATA_FEA							78
#define APSF_NZ_OFFSET_SUP			0x0002
#define APSF_DMA_SETUP_AUTO_SUP		0x0004
#define APSF_POWER_MAN_INIT_SUP		0x0008
#define APSF_IN_ORDER_DATA_SUP		0x0010
#define APSF_SOFT_SET_PRESE_SUP		0x0040
#define AP_SATA_FEA_ENABLE					79
#define APSFE_NZ_BUF_OFFSET_EN		0x0002
#define APSFE_DMA_SETUP_AUTO_EN		0x0004
#define APSFE_POWER_MAN_INIT_EN		0x0008
#define APSFE_IN_ORDER_DATA_EN		0x0010
#define APSFE_SOFT_SET_PRESE_EN		0x0040
#define AP_MAJOR_VERSION					80
#define APMV_ATA_PI_4				0x0010
#define APMV_ATA_PI_5				0x0020
#define APMV_ATA_PI_6				0x0040
#define APMV_ATA_PI_7				0x0080
#define APMV_ATA8					0x0100
#define AP_MINOR_VERSION					81
#define AP_CMD_FEA_SETS1					82
#define APCFS_SMART_SUP				0x0001
#define APCFS_SECURITY_SUP			0x0002
#define APCFS_POWER_MAN_SUP			0x0008
#define APCFS_PACKET_SUP			0x0010
#define APCFS_VOLATILE_CACHE_SUP	0x0020
#define APCFS_READ_LOOK_AHEAD_SUP	0x0040
#define APCFS_RELEASE_INT_SUP		0x0080
#define APCFS_SERVICE_IN_SUP		0x0100
#define APCFS_SEVICE_RESTAR_SUP		0x0200
#define APCFS_HBA_SUP				0x0400
#define APCFS_WRITE_BUFFER_SUP		0x1000
#define APCFS_READ_BUFFER_SUP		0x2000
#define APCFS_NOP_SUP				0x4000
#define AP_CMD_FEA_SETS2					83
#define APCFS_DOWNLOAD_MICROCODE_SUP	0x0001
#define APCFS_TCQ_FEA_SUP			0x0002
#define APCFS_CFA_FEA_SUP			0x0004
#define APCFS_APM_FEA_SUP			0x0008
#define APCFS_PUIS_FEA_SUP			0x0020
#define APCFS_SET_FEATURES_NEED		0x0040
#define APCFS_SET_MAX_SUP			0x0100
#define APCFS_AAM_SUP				0x0200
#define APCFS_LBA48_SUP				0x0400
#define APCFS_DCO_SUP				0x0800
#define APCFS_FLUSH_CACHE_SUP		0x1000
#define APCFS_FLUSH_CACHE_EXT_SUP	0x2000
#define AP_CMD_FEA_SETS3					84
#define APCFS_SMART_LOG_SUP			0x0001
#define APCFS_SMART_ST_SUP			0x0002
#define APCFS_SERIAL_NUMBER_SUP		0x0004
//The Media Card Pass Through Command feature set is supported
//The Streaming feature set is supported
#define APCFS_GPL_SUP				0x0020
//

#define AP_SEC_CNT_48_0						100
#define AP_SEC_CNT_48_1						101
#define AP_SEC_CNT_48_2						102
#define AP_SEC_CNT_48_3						103

#define AP_PHY_SEC_SIZE						106
#define APPSS_LOG_SHIF_MASK			0x000f
#define APPSS_BIG_LOG_SEC			0x1000
#define APPSS_MULT_LOG_SEC			0x2000

#define AP_LOGIC_SEC_SIZE_0					117
#define AP_LOGIC_SEC_SIZE_1					118

#define AP_CUR_SERIAL_NUMBER				176
#define AP_CUR_SERIAL_NUMBER_LEN	10

#define ACS_NOP				0x00
//Data set management		0x06
#define ACS_RESET			0x08
//Request sense data ext	0x0b
//Recalibrate				0x1x
#define ACS_READ_SECTOR_RT	0x20
#define ACS_READ_SECTOR_NR	0x21
#define ACS_READ_LONG_RT	0x22
#define ACS_READ_LONG_NR	0x23
#define ACS_READ_SECTOR_EXT	0x24
#define ACS_READ_DMA_EXT	0x25
//Read DMA queued ext		0x26
//Read native max address ext	0x27
//Read multiplex ext		0x29
#define ACS_READ_STREAM_DMA	0x2a
#define ACS_READ_STREAM		0x2b
#define ACS_READ_LOG_EXT	0x2f
#define ACS_WRITE_SECTOR_RT	0x30
#define ACS_WRITE_SECTOR_NR	0x31
#define ACS_WRITE_LONG_RT	0x32
#define ACS_WRITE_LONG_NR	0x33
#define ACS_WRITE_SECTOR_EXT	0x34
#define ACS_WRITE_DMA_EXT	0x35
//Write DMA queued ext		0x36
//Set Native max address ext	0x37
//write multiple ext		0x39
#define ACS_WRITE_STREAM_EXT	0x3a
#define ACS_WRITE_STREAM	0x3b
//Write verify				0x3c
//Write DMA FUA ext			0x3d
//Write DMA queued FUA ext	0x3e
#define ACS_WRITE_LONG_EXT	0x3f
//Read verify sector with retry	0x40
//Read verify sector without retry	0x41
//Write uncorrectable ext	0x45
//Read log DAM ext			0x47
#define ACS_FORMAT_TRACK	0x50
#define ACS_CONFIG_STREAM	0x51
//Write log DMA				0x57
//Trusted non-data			0x5b
//Trusted recive			0x5c
//Trusted revice DMA		0x5d
//Trusted send				0x5e
//Trusted send DMA			0x5f
//Read FPDMA queued			0x60
//Write FPDMA queued		0x61
//NCQ queued management		0x63
//Send FPDMA queued			0x64
//Recive FPDMA queued		0x65
#define ACS_SEEK			0x70
//Set data & time ext		0x77
//Accessible max address configuration	0x78
//Vendor specific			0x8x
//Exxecute device diagnostic	0x90
//Initialize device paramenters	0x91
//Download microcode 		0x92
//Download microcode DMA	0x93
#define ACS_STANDBY_IMM		0x94
#define ACS_IDLE_IMM		0x95
#define ACS_STANDBY			0x96
#define ACS_IDLE			0x97
#define ACS_CHECK_POWER_MODE	0x98
#define ACS_SLEEP			0x99
//Vendor specific			0x9a
#define ACS_PACKET			0xa0
#define ACS_INDENTIFY_PACKET_DEVICE	0xa1
#define ACS_SERVICE			0xa2
#define ACS_SMART			0xb0
#define ACS_DEVICE_CONFIG	0xb1
#define ACS_SANITIZE_DEVICE	0xb4
#define ACS_NV_CACHE		0xb6
#define ACS_CFA_ERASER_SECTOR	0xc0
//Vendor specific			0xc1-0xc3
#define ACS_READ_MULTIPLE	0xc4
#define ACS_WRITE_MULTIPLE	0xc5
#define ACS_SET_MULTIPLE_MODE	0xc6
#define ACS_READ_DMA_QUEUED	0xc7
#define ACS_READ_DMA_RT		0xc8
#define ACS_READ_DMA_NR		0xc9
#define ACS_WRITE_DMA_RT	0xca
#define ACS_WRITE_DMA_NR	0xcb
#define ACS_WRITE_DMA_QUEUED	0xcc
//Write multiple FUA ext	0xce
//Check media card type		0xd1
//Get media status			0xda
//Achnowlwdge media change	0xdb
//Boot-post-boot			0xdc
//Boot-pre-boot				0xdd
//Door lock					0xde
//Door unlock				0xdf
//read buffer				0xe4
//flush cache				0xe7
//write buffer				0xe8
//write same				0xe9
//flush cache ext			0xea
//write buffer DMA			0xeb
#define ACS_IDENTIFY_DEVICE	0xec
//media eject				0xed
#define ACS_EDENTIFY_DEVICE_DMA	0xee
//Set features				0xef
//security set password		0xf1
//security unlock			0xf2
//security erase prepare	0xf3
//security erase unit		0xf4
//security freeze lock		0xf5
//security disable password	0xf6
//vednor specific			0xf7
//read native max address	0xf8
//set max address			0xf9
//vendor specific			0xfa-0xff

#define ATA_REG_DATA		0x00	//data regiester
#define ATA_REG_ERROR		0x01	//error code register,read only
#define ATA_REG_FEATURES	0x01	//features register,write only
#define ATA_REG_SECCOUNT0	0x02	//sectors count register
#define ATA_REG_LBA0		0x03	//LBA
#define ATA_REG_LBA1		0x04	//LBA
#define ATA_REG_LBA2		0x05	//LBA
#define ATA_REG_HDDEVSEL	0x06	//device register
#define ATA_REG_COMMAND		0x07	//command register,write only
#define ATA_REG_STATUS		0x07	//status register,read only

#define ATA_SR_BUSY			0x80	//busy
#define ATA_SR_DRVREDY		0x40	//driver ready
#define ATA_SR_DEVFAULT		0x20	//driver fault
#define ATA_SR_DEVSEEKCOMP	0x10	//
#define ATA_SR_DATAREADY	0x08	//data ready
#define ATA_SR_CORR			0x04	
#define ATA_SR_INLEX		0x02
#define ATA_SR_ERR			0x01	//error

#define ATA_ER_BADSEC		0x80
#define ATA_ER_UNC			0x40
#define ATA_ER_NOMEDIA		0x28
#define ATA_ER_IDNF			0x10

#define ATA_ER_ABRT			0x04
#define ATA_ER_TK0NF		0x02
#define ATA_ER_AMNF			0x01

struct _FIS_REG_H2D_ {//register host to device
	u8 type;//x027
	u8 pmp:4;
	u8 :3;
	u8 c:1;
	u8 cmd,fet0;
	u8 lba0,lba1,lba2,dev;
	u8 lba3,lba4,lba5,fet1;
	u8 cnt0,cnt1,rvd0,ctrl;
	u32 rvd1;
};

struct _FIS_REG_D2H_ {//register device to host
	u8 type;//0x34
	u8 pmp:4;
	u8 :2;
	u8 i:1;
	u8 :1;
	u8 status,err;
	u8 lba0,lba1,lba2,dev;
	u8 lba3,lba4,lba5,rvd0;
	u8 cnt0,cnt1;
	u8 rvd1[6];
};

struct _FIS_SDB_D2H_ {//set device bits fis,device to host
	u8 type;//0xa1
	u8 pmp:4;
	u8 :2;
	u8 i:1;
	u8 n:1;
	u8 status0:3;
	u8 :1;
	u8 status1:3;
	u8 :1;
	u8 err;
	u32 rvd;
};
struct _FIS_DA_D2H_ {//dma active fis,device to host
	u8 type;//0x39
	u8 pmp:4;
	u8 :4;
	u8 rvd0;
	u8 rvd1;
};
struct _FIS_DS_DOH_ {//dma setup fis
	u8 type;//0x41
	u8 pmp:4;
	u8 :1;
	u8 d:1;
	u8 i:1;
	u8 a:1;
	u8 rvd0,rvd1;
	u8 dbi0,dbi1,dbi2,dbi3;
	u8 dbi4,dbi5,dbi6,dbi7;
	u32 rvd2;
	u8 dbo0,db01,dbo2,dbo3;
	u8 dct0,dct1,dct2,dct3;
	u32 rvd3;
};
struct _FIS_BISTA_ {//BIST fis
	u8 type;//0x58
	u8 pmp:4;
	u8 :4;
	u8 v:1;
	u8 :1;
	u8 p:1;
	u8 f:1;
	u8 l:1;
	u8 s:1;
	u8 a:1;
	u8 t:1;
	u8 rvd0;
	u32 data[2];
};
struct _FIS_PS_D2H_ {//pio setup fis,device to host
	u8 type;//0x5f
	u8 pmp:4;
	u8 :1;
	u8 d:1;
	u8 i:1;
	u8 :1;
	u8 status;
	u8 err;
	u8 lba0,lba1,lba2,dev;
	u8 lba3,lba4,lba5,rvd0;
	u8 cnt0,cnt1,rvd1,estatus;
	u16 cnt;
	u16 rvd2;
};
struct _FIS_DATA_DOH_ {//data fis
	u8 type;//0x46
	u8 pmp:4;
	u8 :4;
	u16 rvd;
	uint32_t data[];
};

#endif