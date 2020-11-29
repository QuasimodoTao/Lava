//Lava OS
//PageFault
//20-02-21-15-30

#include "lm.h"
#include <ahci.h>
#include <pci.h>
#include <gpt.h>

#define CMD_DIRECT_READ		0
#define CMD_DIRECT_WRITE	1


struct ATA{
	struct _CLS_ cmd[8];//align 1024,32 bytes per entry,total 256 byte
	struct {
		struct _FIS_DS_DOH_ dma;
		u32 fisrvd0;
		struct _FIS_PS_D2H_ pio;
		u32 fisrvd1[3];
		struct _FIS_REG_D2H_ reg;
		u32 fisrvd2;
		uint8_t sdbfis[8];
		uint8_t ufis[64];
		uint8_t rvd3[0x60];
	} fis;//align 256,4096;total 256 bytes
	struct {
		struct _FIS_REG_H2D_ reg;//24bytes
		u8 rvd[40];
		u8 acmd[16];
		u8 rvd1[48];
	} header;//total 128 bytes;
	struct _PRDTI_ item[24];//16 bytes per entry,total 512 - 128 byte
	//cmd,align 128
};//Total 1024 bytes

struct _Cmd_ {
	u64 lba;
	u64 buf;
	u16 cnt;
	u8 cmd;
	u8 direct;
};
static HANDLE dev;
static struct _HBA_ * hba;
static int port_count;
static u64 sec_count;
static struct ATA * ata;
static u8 channel;
static u8 command_solt;
static void port_strat(int port){
	if(port < port_count) hba->port[port].cmd |= HBA_PxCMD_ST;
}
static void port_end(int port){
	if(port < port_count) hba->port[port].cmd &= ~HBA_PxCMD_ST;
}
static void port_receice_fis(int port){
	if(port < port_count) hba->port[port].cmd |= HBA_PxCMD_FRE;
}
static void port_reject_fis(int port){
	if(port < port_count){ 
		hba->port[port].cmd &= ~HBA_PxCMD_FRE;
		while(hba->port[port].cmd & HBA_PxCMD_FR) asm("nop");
	}
}

int ahci_do_cmd(struct _Cmd_ * cmd){
	u32 item = 0;
	
	memset(ata,0,sizeof(struct ATA));
	hba->port[channel].fb = (u64)&(ata->fis);
	hba->port[channel].clb = (u64)ata;
	hba->port[channel].ie = 0;
	hba->port[channel].is = 0xffffffff;
	hba->port[channel].serr = 0xffffffff;
	
	ata->header.reg.type = 0x27;
	ata->header.reg.pmp = 0;
	ata->header.reg.c = 1;
	ata->header.reg.cmd = cmd->cmd;
	ata->header.reg.fet0 = 0;
	ata->header.reg.lba0 = cmd->lba & 0xff;
	ata->header.reg.lba1 = (cmd->lba >> 8) & 0xff;
	ata->header.reg.lba2 = (cmd->lba >> 16) & 0xff;
	ata->header.reg.dev = 0x40;
	ata->header.reg.lba3 = (cmd->lba >> 24) & 0xff;
	ata->header.reg.lba4 = (cmd->lba >> 32) & 0xff;
	ata->header.reg.lba5 = (cmd->lba >> 40) & 0xff;
	ata->header.reg.fet1 = 0;
	ata->header.reg.cnt0 = cmd->cnt & 0xff;
	ata->header.reg.cnt1 = (cmd->cnt >> 8) & 0xff;
	ata->header.reg.ctrl = 0;
	
	while(cmd->cnt > 4096){
		ata->item[item].dba = cmd->buf + item * 4096 * 512;
		ata->item[item].dbc = 4096 * 512 - 1;
		cmd->cnt -= 4096;
		item++;
	}
	ata->item[item].dba = cmd->buf + item * 4096 * 512;
	ata->item[item].dbc = cmd->cnt * 512 - 1;
	
	ata->cmd[0].cfl = 5;
	ata->cmd[0].a = 0;
	ata->cmd[0].w = cmd->direct == CMD_DIRECT_READ ? 0 : 1;
	ata->cmd[0].p = 1;
	ata->cmd[0].r = 0;
	ata->cmd[0].b = 0;
	ata->cmd[0].c = 0;
	ata->cmd[0].pmp = 0;
	ata->cmd[0].prdtl = item + 1;
	ata->cmd[0].prdbc = 0;
	ata->cmd[0].ctba = (u64)&(ata->header);
	
	port_receice_fis(channel);
	port_strat(channel);
	hba->port[channel].ci = 1;
	
	while(!(hba->port[channel].is & (HBA_PxIS_DHRS | HBA_PxIS_TFES))){}
	hba->port[channel].is |= HBA_PxIE_DHRE;
	port_end(channel);
	port_reject_fis(channel);
	return ata->fis.reg.err;
}

int ReadDisk(u64 lba,void * buf,u16 count){
	struct _Cmd_ cmd;
	
	cmd.lba = lba;
	cmd.buf = (u64)buf;
	cmd.cmd = 0x25;
	cmd.cnt = count;
	cmd.direct = CMD_DIRECT_READ;
	return ahci_do_cmd(&cmd);
}

int64_t SATAInit(int64_t MemoryStart,GUID * ActiveDisk,int * _ata_count){
	u32 tmp;
	u16 * id;
	u8 * mbr;
	int i, j;
	u64 sec_cnt;
	struct _Cmd_ cmd;
	int err;
	u64 gpt_first;
	u64 gpt_last;
	struct GPTHeader * header;
	u32 crc32;
	int ata_count = 0;
	
	dev = NULL;
	while(1){
		dev = lm_PCISearchClass(dev,0x01,0x06,0xffffffff);
		if(!dev) break;
		hba = (struct _HBA_ *)(u64)lm_PCIReadDword(dev,PCI0BAR(5));
		lm_PCIWriteWord(dev,PCICmd,lm_PCIReadWord(dev,PCICmd) | 0x0417);
		if(!hba) break;
		ata_count += (hba->cap & HBA_CAP_NP) + 1;
	}
	*_ata_count = ata_count;
	dev = NULL;
	dev = lm_PCISearchClass(dev,0x01,0x06,0xffffffff);
	if(!dev) return 0;
	hba = (struct _HBA_ *)lm_PCIReadDword(dev,PCI0BAR(5));
	if(!hba) return 0;
	port_count = (hba->cap & HBA_CAP_NP) + 1;
	MemoryStart += 1023;
	MemoryStart &= ~1023LL;
	ata = (struct ATA *)MemoryStart;
	MemoryStart += sizeof(struct ATA);
	id = (u16*)MemoryStart;
	mbr = (u8*)id;
	header = (struct GPTHeader *)id;
	
	for(channel = 0;channel < port_count;channel++){
		cmd.lba = 0;
		cmd.cmd = 0xec;
		cmd.cnt = 1;
		cmd.buf = (u64)id;
		cmd.direct = CMD_DIRECT_READ;
		if(err = ahci_do_cmd(&cmd)){
			printk("ahci_do_cmd on error %02X.\n",err);
			continue;
		}
		sec_count = ((u64)id[100]) | (((u64)id[101]) << 16) | (((u64)id[102]) << 32);
		if(!sec_count) sec_count = ((u64)id[60]) | (((u64)id[61]) << 16);
		cmd.lba = 0;
		cmd.cmd = 0x25;
		cmd.cnt = 1;
		cmd.buf = (u64)id;
		cmd.direct = CMD_DIRECT_READ;
		if(err = ahci_do_cmd(&cmd)){
			printk("ahci_do_cmd on error %02X.\n",err);
			continue;
		}
		
		for(j = 0;j < 4;j++){
			if(mbr[0x1c2 + 16 * j] == 0xee){
				gpt_first = (u64)mbr[0x1c6+16*j] | (((u64)mbr[0x1c7+16*j]) << 8) | (((u64)mbr[0x1c8+16*j]) << 8) | (((u64)mbr[0x1c9+16*j]) << 8);
				gpt_last = (u64)mbr[0x1ca+16*j] | (((u64)mbr[0x1cb+16*j]) << 8) | (((u64)mbr[0x1cc+16*j]) << 8) | (((u64)mbr[0x1cd+16*j]) << 8);
				if(gpt_last == 0xffffffff) gpt_last = sec_count - 1;
				break;
			}
		}
		cmd.lba = gpt_first;
		cmd.cmd = 0x25;
		cmd.cnt = 1;
		cmd.buf = (u64)header;
		cmd.direct = CMD_DIRECT_READ;
		if(err = ahci_do_cmd(&cmd)){
			printk("ahci_do_cmd on error %02X.\n",err);
			continue;
		}
		if(header->Signature != 0x5452415020494645) continue;
		if(header->Revision != 0x00010000) continue;
		crc32 = header->HeaderCRC32;
		header->HeaderCRC32 = 0;
		if(crc32 != ComputeCRC32(header,header->HeaderSize)){
			cmd.lba = gpt_last;
			cmd.cmd = 0x25;
			cmd.cnt = 1;
			cmd.buf = (u64)header;
			cmd.direct = CMD_DIRECT_READ;
			if(header->Signature != 0x5452415020494645) continue;
			if(header->Revision != 0x00010000) continue;
			crc32 = header->HeaderCRC32;
			header->HeaderCRC32 = 0;
			if(crc32 != ComputeCRC32(header,header->HeaderSize)) continue;
		}
		if(!memcmp(&header->DiskGUID,ActiveDisk,sizeof(GUID))) {
			return MemoryStart;
		}
	}
	return 0;
}
