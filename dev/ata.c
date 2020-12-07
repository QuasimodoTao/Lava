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

#define ATA_TYPE_AHCI		1
#define ATA_TYPE_IDE		2
#define CMD_DIRECT_READ		0
#define CMD_DIRECT_WRITE	1

struct _ATA_ {
	u8 type;
	u8 ata_number;//identity path
	u8 port;
	struct _DISK_CMD_ cmd;
	s64 sectors;
	struct _ATA_ * next;
	wchar_t path[32];//L"/.dev/ata%d.dev"
	FCPEB fc;
	union{
		struct _ATA_AHCI_ {
			u8 solts;
			struct _HBA_ * hba;
			struct _AHCI_PORT_ * h_port;
			struct _AHCI_ * ahci;
			struct _AHCI_CMD_ * cmd[32];
			struct _AHCI_SOLT_ * solt;
			struct _AHCI_FIS_ * fis;
			u16 * ident;
		} ahci;
		struct{
			u16 port_base;
			u16 ctrl_base;
			u16 ident[256];
		} leg;
	};
};
struct _ATA_CMD_ {
	u64 lba;
	u64 buf;
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
	solt.p = 1;
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
	
	ata = fc->data;
	if(!ata) return NULL;
	stream = kmalloc(sizeof(STREAM),0);
	stream->gst = GST_FILE;
	stream->flags = 0;
	stream->read_pos = 0;
	stream->write_pos = 0;
	stream->data = ata;
	stream->fc = fc;
	return stream;
}
static int ata_close(LPSTREAM stream){
	struct _ATA_ * ata;
	
	ata = stream->data;
	if(ata != stream->fc->data) return -1;
	return 0;
}
static int ata_read_block_ahci(struct _ATA_ * ata,u64 iblock,void * pblock,size_t count){
	
}
static int ata_read_block_ide(struct _ATA_ * ata,u64 iblock,void * pblock,size_t count){
	
	
}
static int ata_write_block_ahci(struct _ATA_ * ata,u64 iblock,void * pblock,size_t count){
	
}
static int ata_write_block_ide(struct _ATA_ * ata,u64 iblock,void * pblock,size_t count){
	
	
}
static int ata_read_block(LPSTREAM file,u64 iblock,void * pblock,size_t count){
	struct _ATA_ * ata;
	
	ata = file->data;
	if(!ata) return -1;
	if(iblock >= ata->sectors) return -1;
	if(iblock + count > ata->sectors) count = ata->sectors - iblock;
	if(ata->type == ATA_TYPE_AHCI) return ata_read_block_ahci(ata,iblock,pblock,count);
	else return ata_read_block_ide(ata,iblock,pblock,count);
}
static int ata_write_block(LPSTREAM file,u64 iblock,void * pblock,size_t count){
	struct _ATA_ * ata;
	
	ata = file->data;
	if(!ata) return -1;
	if(iblock >= ata->sectors) return -1;
	if(iblock + count > ata->sectors) count = ata->sectors - iblock;
	if(ata->type == ATA_TYPE_AHCI) return ata_write_block_ahci(ata,iblock,pblock,count);
	else return ata_write_block_ide(ata,iblock,pblock,count);
}
static u64 ata_get_size(LPSTREAM file){
	return 512;
}
static u64 ata_get_count(LPSTREAM file){
	struct _ATA_ * ata;
	
	ata = file->data;
	if(!ata) return -1;
	return ata->sectors;
}
static FCPEB ata_fc = {
	.in = NULL,
	.out = NULL,
	.open = ata_open,
	.close = ata_close,
	.read = NULL,
	.write = NULL,
	.seek_get = NULL,
	.seek_put = NULL,
	.tell_get = NULL,
	.tell_put = NULL,
	.get = NULL,
	.put = NULL,
	.read_block = ata_read_block,
	.write_block = ata_write_block,
	.fresh_get = NULL,
	.fresh_put = NULL,
	.get_size = ata_get_size,
	.set_size = NULL,
	.get_count = ata_get_count,
	.set_count = NULL,
	.data = NULL
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
	struct _ATA_CMD_ * _cmd;
	
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
		solt.prdtl = 8;
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
	if(ahci->hba->cap & HBA_CAP_SPM && //HBA must suppose PMP
		ahci->hba->cap & HBA_CAP_FBSS &&//HBA must suppose FIS-base switch if want to suppose PMP
		ata->ahci.h_port->cmd & HBA_PxCMD_FBSCP) //Port must suppose FIS-base switch is want to suppose PMP
		ata->ahci.h_port->cmd &= ~HBA_PxCMD_PMA;//we not suppose PMP
	{//map device into file system
		wsprintf(ata->path,32,L"/.dev/ata%d.dev",ata->ata_number);
		memcpy(&(ata->fc),&ata_fc,sizeof(FCPEB));
		ata->fc.data = ata;
		fs_map(ata->path,&(ata->fc));
	}
	{//get disk identity
		cmd.lba = 0;
		cmd.buf = ata->ahci.ident;
		cmd.cnt = 1;
		cmd.cmd = 0xec;//get identity
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
	ata->sectors = ((u64)ata->ahci.ident[100]) + (((u64)ata->ahci.ident[101]) << 16) + (((u64)ata->ahci.ident[102]) << 32);
	if(ata->sectors < 2 * 1024)
		printk("ATA %d on AHCI %d total %lld KiB.\n",ata->ata_number,ata->port,ata->sectors/2);
	else if(ata->sectors < 2 * 1024 * 1024)
		printk("ATA %d on AHCI %d total %lld MiB.\n",ata->ata_number,ata->port,ata->sectors/2048);
	else if(ata->sectors < 2LL * 1024 * 1024 * 1024)
		printk("ATA %d on AHCI %d total %lld GiB.\n",ata->ata_number,ata->port,ata->sectors/(2048*1024));
	else
		printk("ATA %d on AHCI %d total %lld TiB.\n",ata->ata_number,ata->port,ata->sectors/(2048*1024*1024));
	return ata;
}
int ata_on_ahci_int_handle(struct _AHCI_ * ahci,void * ata){
	
}
int ata_on_ahci_power_on(struct _AHCI_ * ahci,void * ata){
	
}
int ata_on_ahci_power_off(struct _AHCI_ * ahci,void * ata){
	
}
int ata_on_ahci_close(struct _AHCI_ * ahci,void * ata){
	
}
int ata_on_ahci_restart(struct _AHCI_ * ahci,void * ata){
	
}