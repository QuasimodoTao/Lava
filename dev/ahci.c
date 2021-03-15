/*
	dev/ahci.c
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

#include <lava.h>
#include <mm.h>
#include <ahci.h>
#include <stddef.h>
#include <pci.h>
#include <spinlock.h>

void * ata_on_ahci_open(struct _AHCI_ * ahci,int port);
int ata_on_ahci_int_handle(struct _AHCI_ * ahci,void * ata);
int ata_on_ahci_power_on(struct _AHCI_ * ahci,void * ata);
int ata_on_ahci_power_off(struct _AHCI_ * ahci,void * ata);
int ata_on_ahci_close(struct _AHCI_ * ahci,void * ata);
int ata_on_ahci_restart(struct _AHCI_ * ahci,void * ata);

static struct _AHCI_ * ahci_list = NULL;
static int busy = 0;



static int ahci_restart(struct _PCIDEV_ * dev,struct _AHCI_ * ahci){
	int i;
	
	ahci->hba->ghc |= HBA_GHC_HR;
	return 0;
}
static int ahci_close(LPPCIDEV dev,struct _AHCI_ * ahci){
	int i;
	
	for(i = 0;i < ahci->port_count;i++)
		if(ahci->ctrl[i]) ata_on_ahci_close(ahci,ahci->ctrl[i]);
	return 0;
}
static int ahci_power_off(LPPCIDEV dev,struct _AHCI_ * ahci){
	int i;
	
	for(i = 0;i < ahci->port_count;i++)
		if(ahci->ctrl[i]) ata_on_ahci_power_off(ahci,ahci->ctrl[i]);
	return 0;
}
static int ahci_power_on(LPPCIDEV dev,struct _AHCI_ * ahci){
	int i;
	
	for(i = 0;i < ahci->port_count;i++)
		if(ahci->ctrl[i]) ata_on_ahci_power_on(ahci,ahci->ctrl[i]);
	return 0;
}
static int ahci_int_handle(struct _PCIDEV_ * dev,struct _AHCI_ * ahci,u16 pci_status){
	u32 is;
	int i;
	
	is = ahci->hba->is;
	ahci->hba->is = 0xffffffff;
	for(i = 0;i < ahci->port_count;i++,is >>= 1)
		if((is & 1) && ahci->ctrl[i]) 
			ata_on_ahci_int_handle(ahci,ahci->ctrl[i]);
	if(pci_status & PCI_STATUS_DPD){
		
		
	}
	if(pci_status & PCI_STATUS_STA){
		
	}
	if(pci_status & PCI_STATUS_RTA){
		
		
	}
	if(pci_status & PCI_STATUS_RMA){
		
		
	}
	if(pci_status & PCI_STATUS_SSE){
		
		
	}
	if(pci_status & PCI_STATUS_DPE){
		
		
	}
	return 0;
}

struct _INI_ARGV_ {
	volatile struct _AHCI_ * ahci;
	int i;
};

static int ata_on_ahci_enum(void * argv){
	struct _INI_ARGV_ * arg;
	struct _AHCI_ * ahci;
	int port;

	arg = argv;
	ahci = arg->ahci;
	port = arg->i;
	arg->ahci = NULL;
	ahci->ctrl[port] = ata_on_ahci_open(ahci,port);
	return 0;
}

struct _AHCI_ * ahci_open(LPPCIDEV dev){
	int port_count;
	volatile struct _HBA_ * hba;
	struct _AHCI_ * ahci;
	int i;
	struct _INI_ARGV_ arg;

	spin_lock_bit(&busy,0);
	for(ahci = ahci_list;ahci && ahci->dev != dev;ahci = ahci->next);
	if(!ahci){
		spin_unlock_bit(&busy,0);
		hba = ADDRP2V(pci_read_dword(dev,PCI0BAR(5)));
		page_uncacheable(NULL,hba,0x1100);
		port_count = (hba->cap & HBA_CAP_NP) + 1;
		ahci = kmalloc(sizeof(struct _AHCI_) + sizeof(void*) * port_count,0);
		memset(ahci,0,sizeof(struct _AHCI_));
		ahci->port_count = port_count;
		spin_lock_bit(&busy,0);
		ahci->next = ahci_list;
		ahci_list = ahci;
	}
	spin_unlock_bit(&busy,0);
	ahci->dev = dev;
	ahci->hba = hba;
	pci_write_word(dev,PCIStatus,
		pci_read_word(dev,PCIStatus) | PCI_CMD_SEE | PCI_CMD_MBE | PCI_CMD_MSE);
	hba->ghc |= HBA_GHC_AE | HBA_GHC_IE;
	ahci->hba_busy = 0;
	dev->int_handle = ahci_int_handle;
	dev->restart = ahci_restart;
	dev->close = ahci_close;
	dev->power_off = ahci_power_off;
	dev->power_on = ahci_power_on;
	dev->ctrl = ahci;
	for(i = 0;i < port_count;i++) {
		if(hba->pi & (1 << i)){//if port is avaiable for software
			arg.ahci = ahci;
			arg.i = i;
			create_thread(NULL,ata_on_ahci_enum,&arg);
			while(arg.ahci) wait(0);
		}
	}
	return ahci;
}

PCI_DEV_MODULE(ahci_open,ahci_close,1,6,-1,serial_ata);

