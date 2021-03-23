/*
	dev/ata.c
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

#include <ahci.h>
#include <ata.h>
#include <stddef.h>
#include <spinlock.h>
#include <lava.h>
#include <mm.h>
#include <gst.h>
#include <stdio.h>
#include <string.h>
#include <buffer.h>
#include <fctrl.h>
#include <eisa.h>

#define ATA_TYPE_AHCI		1
#define ATA_TYPE_IDE		2
#define CMD_DIRECT_READ		0
#define CMD_DIRECT_WRITE	1

#define ACCESS_DIRECT_INNER	0
#define ACCESS_DIRECT_OUTER	1

struct _ATA_ {
	u8 type;
	u8 ata_number;//identity path
	u8 port;
	u8 activing;
	u8 direct;
	uint32_t sec_size;
	uint32_t phy_sec_size;
	struct _DISK_CMD_ cmd;
	s64 sectors;
	struct _ATA_ * next;
	HANDLE summon;
	GUID g_disk;
	wchar_t path[32];//L"/.dev/ata%d.dev"
	FCPEB fc;
	union{
		struct _ATA_AHCI_ {
			u8 solts;
			u32 ci;
			struct _HBA_ * hba;
			struct _AHCI_PORT_ * h_port;
			struct _AHCI_ * ahci;
			struct _AHCI_CMD_ * cmd[32];
			struct _AHCI_SOLT_ * solt;
			struct _AHCI_FIS_ * fis;
			u16 * ident;
			LPTHREAD wait[32];
		} ahci;
		struct{
			u16 port_base;
			u16 ctrl_base;
			u8 r_mutex;
			u8 suppose_lba48;
			u8 * mutex;
			u16 ident0[256];
		} leg;
	};
	struct _LL_BLOCK_DEV_ block_ctrl;
	LPSTREAM file_array[MAX_OPEN_FILE];
	struct _ATA_CMD_ * active_cmd_list;
	struct _ATA_CMD_ * cmd_list;
};
struct _ATA_CMD_ {
	u64 lba;
	void * buf;
	struct _ATA_CMD_ * next;
	LPTHREAD wait;
	u16 cnt;
	u8 cmd;
	u8 direct;
	u8 restart;
};

static struct _ATA_ * ata_list = NULL;
static volatile int ata_counter = 0;
static int busy = 0;



static int ata_on_ahci_do_cmd(struct _ATA_ * ata,struct _ATA_CMD_ * cmd){
	struct _FIS_REG_H2D_ fis0;
	struct _FIS_REG_D2H_ fis1;
	struct _CLS_ solt;
	struct _PRDTI_ item;
	
	ata->ahci.h_port->ie = 0;
	ata->ahci.h_port->is = 0xffffffff;
	ata->ahci.h_port->serr = 0xffffffff;
	fis0.type = 0x27;
	fis0.pmp = 0;
	fis0.c = 1;
	fis0.cmd = cmd->cmd;
	fis0.fet0 = 0;
	fis0.fet1 = 0;
	fis0.dev = 0x40;
	fis0.ctrl = 0;
	fis0.lba0 = cmd->lba;
	fis0.lba1 = cmd->lba >> 8;
	fis0.lba2 = cmd->lba >> 16;
	fis0.lba3 = cmd->lba >> 24;
	fis0.lba4 = cmd->lba >> 32;
	fis0.lba5 = cmd->lba >> 40;
	fis0.cnt0 = cmd->cnt;
	fis0.cnt1 = cmd->cnt >> 8;
	item.dba = ADDRV2P(NULL,cmd->buf);
	item.dbc = cmd->cnt * 512 - 1;
	memcpy(&solt,&(ata->ahci.solt[0]),sizeof(solt));
	solt.cfl = sizeof(fis0)/sizeof(u32);
	solt.a = 0;
	solt.w = cmd->direct == CMD_DIRECT_READ ? 0 : 1;
	solt.p = 0;
	solt.r = 0;
	solt.b = 0;
	solt.c = 0;
	solt.pmp = 0;
	solt.prdtl = 1;
	solt.prdbc = cmd->cnt * 512;
	if(cmd->restart){
		solt.r = 1;
		solt.c = 1;
	}
	memcpy((void*)&(ata->ahci.cmd[0]->r_fis),&fis0,sizeof(fis0));
	memcpy((void*)&(ata->ahci.cmd[0]->data_block[0]),&item,sizeof(item));
	memcpy((void*)&(ata->ahci.solt[0]),&solt,sizeof(solt));
	
	ata->ahci.h_port->cmd |= HBA_PxCMD_ST | HBA_PxCMD_FRE;
	ata->ahci.h_port->ci = 1;
	while(!(ata->ahci.h_port->is & (HBA_PxIS_DHRS | HBA_PxIS_TFES)));
	ata->ahci.h_port->is |= HBA_PxIE_DHRE;
	ata->ahci.h_port->cmd &= ~(HBA_PxCMD_ST | HBA_PxCMD_FRE);
	while(ata->ahci.h_port->cmd & HBA_PxCMD_FR);
	memcpy(&fis1,(void*)&(ata->ahci.fis->r_fis),sizeof(fis1));
	return fis1.err;
}
static LPSTREAM ata_open(wchar_t * name,u64 mode,struct _FCPEB_ * fc){
	struct _ATA_ * ata;
	LPSTREAM stream;
	int i;

	ata = fc->data;
	if(!ata) return NULL;
	stream = kmalloc(sizeof(STREAM),0);
	for(i = 0;i < MAX_OPEN_FILE;i++)
		if(!ata->file_array[i] && 
			!cmpxchg8b(ata->file_array + i,NULL,stream,NULL)) {
				stream->gst = GST_FILE;
				stream->read_pos = 0;
				stream->write_pos = 0;
				stream->edata.pdata[0] = ata;
				stream->fc = fc;
				return stream;
			}
	kfree(stream);
	return NULL;
}
static int ata_close(LPSTREAM stream){
	struct _ATA_ * ata;
	int i;

	ata = stream->edata.pdata[0];
	if(ata != stream->fc->data) return -1;
	for(i = 0;i < MAX_OPEN_FILE;i++)
		if(ata->file_array[i] == stream) return cmpxchg8b(ata->file_array + i,stream,NULL,NULL);
	return -1;
}
static int ata_deal_cmd_list(struct _ATA_ * ata,struct _ATA_CMD_ * cmd_list){
	struct _ATA_CMD_ *_cmd_list;
	struct _ATA_CMD_ * prev,*cur;
	struct _ATA_CMD_ * uter,*uter_prev;
	int i;
	struct _FIS_REG_H2D_ fis;
	struct _CLS_ solt;
	struct _PRDTI_ item;
	u8 status;

	_cmd_list = NULL;
	if(ata->direct == ACCESS_DIRECT_OUTER){//small to grate
		while(1){
			uter = cmd_list;
			cur = cmd_list->next;
			if(!cur) break;
			prev = cmd_list;
			uter_prev = NULL;
			while(cur){
				if(cur->lba > uter->lba){
					uter = cur;
					uter_prev = prev;
				}
				prev = cur;
				cur = cur->next;
			}
			if(uter_prev) uter_prev->next = uter->next;
			else cmd_list = cmd_list->next;
			uter->next = _cmd_list;
			_cmd_list = uter;
		}
	}
	else{//grate to small
		while(1){
			uter = cmd_list;
			cur = cmd_list->next;
			if(!cur) break;
			prev = cmd_list;
			uter_prev = NULL;
			while(cur){
				if(cur->lba < uter->lba){
					uter = cur;
					uter_prev = prev;
				}
				prev = cur;
				cur = cur->next;
			}
			if(uter_prev) uter_prev->next = uter->next;
			else cmd_list = cmd_list->next;
			uter->next = _cmd_list;
			_cmd_list = uter;
		}
	}
	uter->next = _cmd_list;
	cmd_list = uter;
	if(ata->type == ATA_TYPE_AHCI){
		for(i = 0;i < ata->ahci.solts && cmd_list;i++){
			if(ata->ahci.ci & (1 << i)) continue;
			cur = cmd_list;
			//printk("LBA:%lld,%P.",cur->lba,cur->buf);
			cmd_list = cmd_list->next;
			ata->ahci.wait[i] = cur->wait;
			memset(&fis,0,sizeof(fis));
			memset(&item,0,sizeof(item));
			fis.type = 0x27;
			fis.pmp = 0;
			fis.c = 1;
			fis.cmd = cur->cmd;
			fis.fet0 = 0;
			fis.fet1 = 0;
			fis.dev = 0x40;
			fis.ctrl = 0;
			fis.lba0 = cur->lba;
			fis.lba1 = cur->lba >> 8;
			fis.lba2 = cur->lba >> 16;
			fis.lba3 = cur->lba >> 24;
			fis.lba4 = cur->lba >> 32;
			fis.lba5 = cur->lba >> 40;
			fis.cnt0 = cur->cnt;
			fis.cnt1 = cur->cnt >> 8;
			item.dba = ADDRV2P(NULL,cur->buf);
			item.dbc = (uint32_t)cur->cnt * ata->sec_size - 1;
			item.i = 1;
			memcpy(&solt,&(ata->ahci.solt[i]),sizeof(solt));
			//solt.cfl = sizeof(fis)/sizeof(u32);
			solt.a = 0;
			solt.w = cur->direct == CMD_DIRECT_READ ? 0 : 1;
			solt.p = 1;
			solt.r = 0;
			solt.b = 0;
			solt.c = 0;
			solt.pmp = 0;
			//solt.prdtl = 1;s
			solt.prdbc = (uint32_t)cur->cnt * ata->sec_size;
			if(cur->restart){
				solt.r = 1;
				solt.c = 1;
			}
			memcpy((void*)&(ata->ahci.cmd[i]->r_fis),&fis,sizeof(fis));
			memcpy((void*)&(ata->ahci.cmd[i]->data_block[0]),&item,sizeof(item));
			memcpy((void*)&(ata->ahci.solt[i]),&solt,sizeof(solt));

			//printk("%P,%P,%P.",&(ata->ahci.cmd[i]->r_fis),&(ata->ahci.cmd[i]->data_block[0]),&(ata->ahci.solt[i]));

			ata->ahci.ci |= 1 << i;
			ata->ahci.h_port->ci |= 1 << i;
		}
		//printk("old ci:%08X.",ata->ahci.ci);
		ata->active_cmd_list = cmd_list;
	}
	else{
		ata->active_cmd_list = cmd_list;
		i = 0;
		spin_lock_bit(ata->leg.mutex,0);
		while(1){
			status = inb(ata->leg.port_base + EISA_HD_STATUS);
			if(!(status & (ATA_SR_BUSY | ATA_SR_DATAREADY))) break;
			i++;
			if(i >= 12000000) {
				spin_unlock_bit(ata->leg.mutex,0);
				return -1;
			}
			nop();nop();nop();nop();nop();nop();nop();
		}
		outb(ata->leg.port_base + EISA_HD_HDDEVSEL,0xa0 | 0x40 | (ata->port & 0x01 ? 0x10 : 0));
		i = 0;
		while(1){
			status = inb(ata->leg.port_base + EISA_HD_STATUS);
			if(!(status & (ATA_SR_BUSY | ATA_SR_DATAREADY))) break;
			i++;
			if(i >= 12000000) {
				spin_unlock_bit(ata->leg.mutex,0);
				return -1;
			}
			nop();nop();nop();nop();nop();nop();nop();
		}
		if(ata->leg.suppose_lba48){
			outb(ata->leg.port_base + EISA_HD_FEATURE_PORT,0);
			outb(ata->leg.port_base + EISA_HD_FEATURE_PORT,0);
			outb(ata->leg.port_base + EISA_HD_SEC_CNT_PORT,cmd_list->cnt >> 8);
			outb(ata->leg.port_base + EISA_HD_SEC_CNT_PORT,cmd_list->cnt);
			outb(ata->leg.port_base + EISA_HD_LBA0_PORT,cmd_list->lba >> 24);
			outb(ata->leg.port_base + EISA_HD_LBA0_PORT,cmd_list->lba);
			outb(ata->leg.port_base + EISA_HD_LBA1_PORT,cmd_list->lba >> 32);
			outb(ata->leg.port_base + EISA_HD_LBA1_PORT,cmd_list->lba >> 8);
			outb(ata->leg.port_base + EISA_HD_LBA2_PORT,cmd_list->lba >> 40);
			outb(ata->leg.port_base + EISA_HD_LBA2_PORT,cmd_list->lba >> 16);
			outb(ata->leg.port_base + EISA_HD_HDDEVSEL,0xa0 | 0x40 | (ata->port & 0x01 ? 0x10 : 0));
			if(cmd_list->cmd == ACS_READ_DMA_EXT) outb(ata->leg.port_base + EISA_HD_COMMAND,ACS_READ_SECTOR_EXT);
			else if(cmd_list->cmd == ACS_WRITE_DMA_EXT) outb(ata->leg.port_base + EISA_HD_COMMAND,ACS_WRITE_SECTOR_EXT);
		}
		else{
			outb(ata->leg.port_base + EISA_HD_FEATURE_PORT,0);
			outb(ata->leg.port_base + EISA_HD_SEC_CNT_PORT,cmd_list->cnt);
			outb(ata->leg.port_base + EISA_HD_LBA0_PORT,cmd_list->lba);
			outb(ata->leg.port_base + EISA_HD_LBA1_PORT,cmd_list->lba >> 8);
			outb(ata->leg.port_base + EISA_HD_LBA2_PORT,cmd_list->lba >> 16);
			outb(ata->leg.port_base + EISA_HD_HDDEVSEL,0xa0 | 0x40 | (ata->port & 0x01 ? 0x10 : 0) | ((cmd_list->lba >> 24) & 0x0f));
			if(cmd_list->cmd == ACS_READ_DMA_EXT) outb(ata->leg.port_base + EISA_HD_COMMAND,ACS_READ_SECTOR_RT);
			else if(cmd_list->cmd == ACS_WRITE_DMA_EXT) outb(ata->leg.port_base + EISA_HD_COMMAND,ACS_WRITE_SECTOR_RT);
		}
		spin_unlock_bit(ata->leg.mutex,0);
	}
	return 0;
}
static int ata_read_block(struct _BUFFER_HEAD_ * bh){
	struct _ATA_ * ata;
	u64 iblock;
	size_t count;
	struct _ATA_CMD_ cmd;
	LPTHREAD thread;
	int ie;
	
	ata = bh->dev->data;
	if(!ata) return -1;
	count = BLOCK_DEV_CACHE_SIZE/ata->sec_size;
	iblock = bh->iblock;
	if(iblock >= ata->sectors) return -1;
	if(iblock + count > ata->sectors) count = ata->sectors - iblock;

	cmd.lba = iblock;
	cmd.cmd = ACS_READ_DMA_EXT;
	cmd.cnt = count;
	cmd.direct = CMD_DIRECT_READ;
	cmd.buf = bh->addr;
	cmd.restart = 0;
	cmd.wait = thread = GetCurThread();
	do{
		cmd.next = ata->cmd_list;
	} while(cmpxchg8b(&ata->cmd_list,cmd.next,&cmd,NULL));
	if(!spin_try_lock_bit(&ata->activing,0))//device is not activeing
		if(ata_deal_cmd_list(ata,xchgq(&ata->cmd_list,NULL))) 
			return -1;
	ie = IE();
	//printk("waiting for read disk finish,%lld.",iblock);
	cli();
	thread->flag = TF_BLOCK;
	schedule_imm();
	if(ie) sti();
	//printk("ata_read_block(%P,%lld).",bh,bh->iblock);
	return 0;
}
static int ata_write_block(struct _BUFFER_HEAD_ * bh){
	struct _ATA_ * ata;
	u64 iblock;
	size_t count;
	struct _ATA_CMD_ cmd;
	LPTHREAD thread;
	struct _ATA_CMD_ * _cmd;
	int ie;
	
	ata = bh->dev->data;
	if(!ata) return -1;
	count = BLOCK_DEV_CACHE_SIZE/ata->sec_size;
	iblock = bh->iblock;
	if(iblock >= ata->sectors) return -1;
	if(iblock + count > ata->sectors) count = ata->sectors - iblock;

	cmd.lba = iblock;
	cmd.cmd = ACS_WRITE_DMA_EXT;
	cmd.cnt = count;
	cmd.direct = CMD_DIRECT_READ;
	cmd.buf = bh->addr;
	cmd.restart = 0;
	cmd.wait = thread = GetCurThread();
	do{
		cmd.next = ata->cmd_list;
	} while(cmpxchg8b(&ata->cmd_list,cmd.next,&cmd,NULL));
	thread->flag = TF_BLOCK;
	if(!spin_try_lock_bit(&ata->activing,0)){//device is not activeing
		if(ata_deal_cmd_list(ata,xchgq(&ata->cmd_list,NULL))) 
			return -1;
	}
	ie = IE();
	cli();
	schedule_imm();
	if(ie) sti();
	return 0;
}
static u64 ata_get_size(LPSTREAM file){
	struct _ATA_ * ata = file->edata.pdata[0];
	return ata->sec_size;
}
static u64 ata_get_count(LPSTREAM file){
	struct _ATA_ * ata;
	
	ata = file->edata.pdata[0];
	if(!ata) return -1;
	return ata->sectors;
}
static int ata_info(LPSTREAM file,size_t size,void * __info){
	struct _ATA_ * ata;
	struct _DISK_INFO_ * _info;

	if(size < sizeof(struct _DISK_INFO_)) return -1;
	_info = __info;
	ata = file->edata.pdata[0];
	_info->cache_block_size = BLOCK_DEV_CACHE_SIZE;
	memcpy(&_info->g_id,&ata->g_disk,sizeof(GUID));
	_info->logical_sector_count = ata->sectors;
	_info->logical_sector_size = 512;
	_info->path = ata->path;
	_info->physical_sector_size = ata->phy_sec_size;
	_info->size = sizeof(struct _DISK_INFO_);
	return 0;
}
static struct _LL_BLOCK_DEV_ ata_ll_rw = {
	.cache_size = BLOCK_DEV_CACHE_SIZE,
	.read_block = ata_read_block,
	.write_block = ata_write_block
};
static struct _BUFFER_HEAD_ * ata_bread(LPSTREAM file,u64 iblock){
	struct _ATA_ * ata = file->edata.pdata[0];
	//printk("ata_bread:%P,%lld.\n",file,iblock);
	if (iblock & 7) return ml_blink(&ata->block_ctrl, iblock & ~7, (iblock & 7) * 512, BLOCK_DEV_CACHE_SIZE - (iblock & 7) * 512);
	else return ll_bread(&ata->block_ctrl, iblock);
}
static struct _BUFFER_HEAD_ * ata_cache_bwrite(LPSTREAM file,u64 iblock){
	struct _ATA_ * ata = file->edata.pdata[0];
	return ll_balloc(&ata->block_ctrl, iblock);
}
static FCPEB ata_fc = {
	.open = ata_open,
	.close = ata_close,
	.bread = ata_bread,
	.cache_bwrite = ata_cache_bwrite,
	.get_size = ata_get_size,
	.get_count = ata_get_count,
	.info = ata_info
};
void * ata_on_ahci_open(struct _AHCI_ * ahci,int port){
	struct _ATA_ * ata;
	struct _CLS_ solt;
	u64 addr;
	int i;
	struct _FIS_REG_H2D_ rfis;
	struct _FIS_REG_D2H_ rfis2;
	struct _PRDTI_ item;
	struct _ATA_CMD_ cmd;
	struct _AHCI_CMD_ * _cmd;
	
	{//ata struct init
		ata = kmalloc(sizeof(struct _ATA_),0);
		ata->next = xchgq(&ata_list,ata);
		ata->ata_number = (unsigned short)xaddd(&ata_counter,1);
		ata->port = port;
		ata->type = ATA_TYPE_AHCI;
		ata->ahci.h_port = (void*)&(ahci->hba->port[port]);
		ata->ahci.hba = (void*)ahci->hba;
		ata->ahci.ahci = ahci;
	}
	{//ahci port init
		_cmd = PADDR2V(get_free_page(0,0,ata->ahci.hba->cap & HBA_CAP_S64A ? 0 : 32));
		for(i = 0;i < 16;i++) ata->ahci.cmd[i] = _cmd + i;
		_cmd = PADDR2V(get_free_page(0,0,ata->ahci.hba->cap & HBA_CAP_S64A ? 0 : 32));
		for(i = 0;i < 16;i++) ata->ahci.cmd[i + 16] = _cmd + i;
		ata->ahci.solt = PADDR2V(get_free_page(0,0,ata->ahci.hba->cap & HBA_CAP_S64A ? 0 : 32));
		ata->ahci.fis = (void*)(((u64)ata->ahci.solt) + sizeof(struct _AHCI_SOLT_));
		ata->ahci.ident = (void*)(((u64)ata->ahci.fis) + sizeof(struct _AHCI_FIS_));
		page_uncacheable(NULL,ata->ahci.cmd[0],PAGE_SIZE);//DMA space
		page_uncacheable(NULL,ata->ahci.cmd[16],PAGE_SIZE);//DMA space
		page_uncacheable(NULL,ata->ahci.solt,PAGE_SIZE);//DMA space
		
		ata->ahci.h_port->clb = ADDRV2P(NULL,ata->ahci.solt);
		ata->ahci.h_port->fb = ADDRV2P(NULL,ata->ahci.fis);
		memset(&solt,0,sizeof(solt));
		solt.cfl = sizeof(struct _FIS_REG_H2D_) / sizeof(u32);
		solt.prdtl = 1;
		addr = ADDRV2P(NULL,ata->ahci.cmd[0]);
		for(i = 0;i < 16;i++){
			solt.ctba = addr;
			addr += sizeof(struct _AHCI_CMD_);
			memcpy(&(ata->ahci.solt->solt[i]),&solt,sizeof(solt));
		}
		addr = ADDRV2P(NULL,ata->ahci.cmd[16]);
		for(;i < 32;i++){
			solt.ctba = addr;
			addr += sizeof(struct _AHCI_CMD_);
			memcpy(&(ata->ahci.solt->solt[i]),&solt,sizeof(solt));
		}
	}
	ata->ahci.solts = ((ahci->hba->cap & HBA_CAP_NCS) >> 8) + 1; 
	if(ata->ahci.solts == 32)ata->ahci.ci = 0;
	else ata->ahci.ci = 0xffffffff << ata->ahci.solts;
	if(ahci->hba->cap & HBA_CAP_SPM && //HBA must suppose PMP
		ahci->hba->cap & HBA_CAP_FBSS &&//HBA must suppose FIS-base switch if want to suppose PMP
		ata->ahci.h_port->cmd & HBA_PxCMD_FBSCP) //Port must suppose FIS-base switch is want to suppose PMP
		ata->ahci.h_port->cmd &= ~HBA_PxCMD_PMA;//we not suppose PMP
	memcpy(&ata->block_ctrl,&ata_ll_rw,sizeof(struct _LL_BLOCK_DEV_));
	ata->block_ctrl.data = ata;
	if(ahci->hba->cap & HBA_CAP_S64A) ata->block_ctrl.bits = 64;
	else ata->block_ctrl.bits = 32;
	{//map device into file system
		wsprintf(ata->path,32,L"/.dev/ata%d.dev",ata->ata_number);
		memcpy(&(ata->fc),&ata_fc,sizeof(FCPEB));
		ata->fc.data = ata;
		fs_map(ata->path,&(ata->fc),NULL);
	}
	{//get disk identity
		cmd.lba = 0;
		cmd.buf = ata->ahci.ident;
		cmd.cnt = 1;
		cmd.cmd = ACS_IDENTIFY_DEVICE;//get identity
		cmd.direct = CMD_DIRECT_READ;
		cmd.restart = 0;
		if(ata_on_ahci_do_cmd(ata,&cmd)){
			cmd.restart = 1;
			ata_on_ahci_do_cmd(ata,&cmd);
			cmd.restart = 0;
			if(ata_on_ahci_do_cmd(ata,&cmd)){
				ata->sectors = -1;
				return NULL;
			}
		}
	}

	ahci->ctrl[port] = ata;
	ata->ahci.h_port->cmd |= HBA_PxCMD_ST | HBA_PxCMD_FRE;

	ata->ahci.h_port->ie = HBA_PxIE_DPE | HBA_PxIE_PCE | HBA_PxIE_OFE | HBA_PxIE_INFE | HBA_PxIE_IFE |
		HBA_PxIE_HBDE | HBA_PxIE_HBFE | HBA_PxIE_DHRE | HBA_PxIE_PSE | HBA_PxIE_DSE | HBA_PxIE_SDBE;
	ata->sectors = ((u64)ata->ahci.ident[AP_SEC_CNT_48_0]) + (((u64)ata->ahci.ident[AP_SEC_CNT_48_1]) << 16) + (((u64)ata->ahci.ident[AP_SEC_CNT_48_2]) << 32);
	if(ata->ahci.ident[AP_PHY_SEC_SIZE] & APPSS_BIG_LOG_SEC){
		ata->sec_size = (((u32)(ata->ahci.ident[AP_LOGIC_SEC_SIZE_0])) | (((u32)(ata->ahci.ident[AP_LOGIC_SEC_SIZE_1])) << 16)) << 1;
		if(ata->ahci.ident[AP_PHY_SEC_SIZE] & APPSS_MULT_LOG_SEC)
			ata->phy_sec_size = ata->sec_size << (ata->ahci.ident[AP_PHY_SEC_SIZE] & APPSS_LOG_SHIF_MASK);
		else ata->phy_sec_size = ata->sec_size;
	}
	else ata->sec_size = ata->phy_sec_size = 512;
	//TODO:must got a disk GUID to ata->g_disk;
	//printk("%P.\n",ata->ahci.ident);
	if(ata->sectors < 2 * 1024)
		printk("ATA %d on AHCI %d total %lld KiB.\n",ata->ata_number,ata->port,ata->sectors/2);
	else if(ata->sectors < 2 * 1024 * 1024)
		printk("ATA %d on AHCI %d total %lld MiB.\n",ata->ata_number,ata->port,ata->sectors/2048);
	else if(ata->sectors < 2LL * 1024 * 1024 * 1024)
		printk("ATA %d on AHCI %d total %lld GiB.\n",ata->ata_number,ata->port,ata->sectors/(2048*1024));
	else
		printk("ATA %d on AHCI %d total %lld TiB.\n",ata->ata_number,ata->port,ata->sectors/(2048*1024*1024));
	ata->summon = summon_disk(ata->path);
	return ata;
}
int ata_on_ahci_int_handle(struct _AHCI_ * ahci,void * _ata){
	uint32_t is;
	struct _ATA_ * ata;
	uint32_t ci,_ci;
	int i;
	struct _ATA_CMD_ * cmd_list;

	ata = _ata;
	is = ata->ahci.h_port->is;
	ata->ahci.h_port->is = is;
	//printk("IS:%08X.",is);
	if(is & (HBA_PxIS_DHRS | HBA_PxIS_PSS | HBA_PxIS_DSS | HBA_PxIS_SDBS)){
		//recive a FIS
		ci = ata->ahci.h_port->ci;
		ci ^= ata->ahci.ci;
		lock_andd(&ata->ahci.ci,~ci);
		if(!ci){
			//command on error;
		}
		else {
			//printk("ci:%08X.",ci);
			_ci = ci;
			for(i = 0;i < ata->ahci.solts;i++,ci >>= 1)
				if(ci & 0x01) wake_up(ata->ahci.wait[i]);
			ci = ata->ahci.h_port->ci;
			if(!ci){
				cmd_list = ata->active_cmd_list;
				if(cmd_list) ata_deal_cmd_list(ata,cmd_list);
				else {
					ata->direct = ata->direct == ACCESS_DIRECT_INNER ? ACCESS_DIRECT_OUTER:ACCESS_DIRECT_INNER;
					spin_unlock_bit(&ata->activing,0);
					if(!spin_try_lock_bit(&ata->activing,0)){
						cmd_list = xchgq(&ata->cmd_list,NULL);
						if(cmd_list) ata_deal_cmd_list(ata,cmd_list);
						else spin_unlock_bit(&ata->activing,0);
					}
				}
			}
		}
	}
	if(is & HBA_PxIS_DPS){
		//Processed a descriptor
		printk("Processed a descriptor.\n");
	}
	if(is & HBA_PxIS_PCS){
		//Port connect change status

	}
	if(is & HBA_PxIS_DMPS){

	}

	return 0;

	
}
int ata_on_ahci_power_on(struct _AHCI_ * ahci,void * ata){
	
}
int ata_on_ahci_power_off(struct _AHCI_ * ahci,void * ata){
	
}
int ata_on_ahci_close(struct _AHCI_ * ahci,void * ata){
	
}
int ata_on_ahci_restart(struct _AHCI_ * ahci,void * ata){
	
}