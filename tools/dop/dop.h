#ifndef _DOP_H_
#define _DOP_H_

#include <objbase.h>
#include <stdio.h>

#define U64	unsigned long long
#define U32 unsigned int
#define Bswap64(x)	(((((U64)x) & 0xff) << 56) | ((((U64)x) & 0xff00) << 40) \
	| ((((U64)x) & 0xff0000) << 24) | ((((U64)x) & 0xff000000) << 8)\
	| ((((U64)x) & 0xff00000000) >> 8) | ((((U64)x) & 0xff0000000000) >> 24) \
	| ((((U64)x) & 0xff000000000000) >> 40) | ((((U64)x) & 0xff00000000000000) >> 56))
#define Bswap32(x)	((((U32)x) & 0xff) << 24) | (((((U32)x) & 0xff00) << 8)) \
	| (((((U32)x) & 0xff0000) >> 8)) | (((((U32)x) & 0xff000000) >> 24))


#define MULT_PROCR
#define DISK_CACHE_BLOCK_SIZE	4096
#define PATH_MAX_LENGTH			4096

typedef void * HANDLE;
typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef unsigned short wchar_t;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef long long stamp64_t;

typedef unsigned long long iblock_t;

#ifndef kmalloc
#define kmalloc(x,y)	malloc(x)
#define kfree(x)		free(x)
#endif

unsigned int ComputeCRC32(unsigned int crc,void * Data, int Len);
int IsZero(char * Data, int Len);
int IsNulGUID(GUID * guid);

int (*CreateGuid)(GUID * pid);

#define VHD_MAP_NAME	L"/.dev/vhd0.dev"

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
#define APPSS_LOG_SHIF				0x0004
#define APPSS_LOG_SEC_MAX			0x0100
#define APPSS_LOG_SEC_PRE_PHY		0x0200

#define AP_LOGIC_SEC_SIZE_0					117
#define AP_LOGIC_SEC_SIZE_1					118

#define AP_CUR_SERIAL_NUMBER				176
#define AP_CUR_SERIAL_NUMBER_LEN	10


#define ESP_GUID	((GUID){0xc12a7328, 0xf81f, 0x11d2, { \
	0xba, 0x4b, 0x00, 0xa0, 0xc9, 0x3e, 0xc9, 0x3b }})
#define FAT_GUID	((GUID){0xebd0a0a2, 0xb9e5, 0x4433, { \
	0x87, 0xc0, 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7 }})
#define LFS_GUID	((GUID){0xa23a4c32, 0x5650, 0x47c6, {\
	0xaf, 0x40, 0x5c, 0xd9, 0x48, 0x78, 0xc6, 0x71}})


#define InsertList(_head,_this,_prev,_next)	\
{\
	_this->_prev = NULL;\
	_this->_next = _head;\
	if(_head) _head->_prev = _this;\
	_head = _this;\
}
#define RemoveList(_head,_this,_prev,_next) \
{\
	if(_this->_prev) _this->_prev->_next = _this->_next;\
	else _head = _this->_next;\
	if(_this->_next) _this->_next->_prev = _this->_prev;\
}


#endif