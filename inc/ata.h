//Lava OS
//PageFault
//20-02-22-20-27

#ifndef _ATA_H_
#define _ATA_H_
#include <stddef.h>




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