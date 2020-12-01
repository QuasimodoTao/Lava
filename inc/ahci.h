/*
	inc/ahci.h
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

#ifndef _AHCI_H_
#define _AHCI_H_

#include <stddef.h>
#include <ata.h>
#include <pci.h>
#include <disk.h>

struct _PMCAP_	{
	uint8_t NEXT;//Next Capabilities Pointer
	uint8_t CID;//Cap ID	0x01
	uint16_t PC;//PCI Power Management Capabilities
		//PSUP:PSUP:PSUP:PSUP:PSUP:D2S:D1S:AUXC:AUXC:AUXC:DSI:0:PMEC:VS:VS:VS
		//PSUP	RO	18	PME_Support
		//D2S	RO	0	D2_Support
		//D1S	RO	0	D1_Support
		//AUXC	RO	I	Aux_Current
		//DSI	RO	I	Device Specific Initialization
		//PMEC	RO	0	PME Clock
		//VS	RO	I	Verision
	uint16_t PMCS;//PCI Power Management Control And Status
		//PMES:0:0:0:0:0:0:PMEE:0:0:0:0:0:0:PS:PS
		//PMES	RWC	I	PME Status
		//PMEE	RW	I	PME Enable
		//PS	R/W	Power State
};
struct _MSICAP_ {
	uint8_t NEXT;//Next Capabilities Pointer
	uint8_t CID;//Cap ID	0x05
	uint16_t MC;//Message Signaled Interrupt Message Control
		//0:0:0:0:0:0:0:0:C64:MME:MME:MME:MMC:MMC:MMC:MSIE
		//C64	RO	I	64-Bit Address Capable
		//MME	RW	0	Multiple Message Enable
		//MMC	RO	I	Multiple Message Capable
		//MSIE	RW	0	MSI Enable
	union{
		struct{
			uint32_t MA;//Message Signaled Interrupt Message Address,align 4
			uint16_t MD;//Message Signaled Interrupt Message Data
		} _32;
		struct{
			uint32_t MA;//Message Signaled Interrupt Message Address,align 4
			uint32_t MUA;//Message Signaled Interrupt Message Address Upper Address
			uint16_t MD;//Message Signaled Interrupt Message Data
		} _64;
	};
};

#define HBA_CAP_NP		0x0000001f
#define HBA_CAP_SXS		0x00000020
#define HBA_CAP_EMS		0x00000040
#define HBA_CAP_CCCS	0x00000080
#define HBA_CAP_NCS		0x00001f00
#define HBA_CAP_PSC		0x00002000
#define HBA_CAP_SSC		0x00004000
#define HBA_CAP_PMD		0x00008000
#define HBA_CAP_FBSS	0x00010000
#define HBA_CAP_SPM		0x00020000
#define HBA_CAP_SAM		0x00040000
#define HBA_CAP_ISS		0x00f00000
#define HBA_CAP_SCLO	0x01000000
#define HBA_CAP_SAL		0x02000000
#define HBA_CAP_SALP	0x04000000
#define HBA_CAP_SSS		0x08000000
#define HBA_CAP_SMPS	0x10000000
#define HBA_CAP_SSNTF	0x20000000
#define HBA_CAP_SNCQ	0x40000000
#define HBA_CAP_S64A	0x80000000

#define HBA_GHC_HR		0x00000001
#define HBA_GHC_IE		0x00000002
#define HBA_GHC_MRSM	0x00000004
#define HBA_GHC_AE		0x80000000

#define HBA_VS_MNR		0x0000ffff
#define HBA_VS_MJR		0xffff0000

#define HBA_CCC_CTL_EN	0x00000001
#define HBA_CCC_CTL_INT	0x000000f8
#define HBA_CCC_CTL_CC	0x0000ff00
#define HBA_CCC_CTL_TV	0xffff0000

#define HBA_EM_LOC_SZ	0x0000ffff
#define HBA_EM_LOC_OFST	0xffff0000

#define HBA_EM_STS_MR	0x00000001
#define HBA_EM_CTL_TM	0x00000100
#define HBA_EM_CTL_RST	0x00000200
#define HBA_EM_SUPP_LED	0x00010000
#define HBA_EM_SUPP_SAFTE	0x00020000
#define HBA_EM_SUPP_SES2	0x00040000
#define HBA_EM_SUPP_SGPIO	0x00080000
#define HBA_EM_ATTR_SMB	0x01000000
#define HBA_EM_ATTR_XMT	0x02000000
#define HBA_EM_ATTR_ALHD	0x04000000
#define HBA_EM_ATTR_PM	0x08000000

#define HBA_CAP2_BOH	0x00000001
#define HBA_CAP2_NVMP	0x00000002
#define HBA_CAP2_APST	0x00000004
#define HBA_CAP2_SDS	0x00000008
#define HAB_CAP2_SADM	0x00000010
#define HBA_CAP2_DESO	0x00000020

#define HBA_BOHC_BOS	0x00000001
#define HBA_BOHC_OOS	0x00000002
#define HBA_BOHC_SOOE	0x00000004
#define HBA_BOHC_OOC	0x00000008
#define HBA_BOHC_BB		0x00000010

#define HBA_PxIS_DHRS	0x00000001
#define HBA_PxIS_PSS	0x00000002
#define HBA_PxIS_DSS	0x00000004
#define HBA_PxIS_SDBA	0x00000008
#define HBA_PxIS_UFS	0x00000010
#define HBA_PxIS_DPS	0x00000020
#define HBA_PxIS_PCS	0x00000040
#define HBA_PxIS_DMPS	0x00000080
#define HBA_PxIS_PRCS	0x00400000
#define HBA_PxIS_IPMS	0x00800000
#define HBA_PxIS_OFS	0x01000000
#define HBA_PxIS_INFS	0x04000000
#define HBA_PxIS_IFS	0x08000000
#define HBA_PxIS_HBDS	0x10000000
#define HBA_PxIS_HBFS	0x20000000
#define HBA_PxIS_TFES	0x40000000
#define HBA_PxIS_CPDS	0x80000000

#define HBA_PxIE_DHRE	0x00000001
#define HBA_PxIE_PSE	0x00000002
#define HBA_PxIE_DSE	0x00000004
#define HBA_PxIE_SDBE	0x00000008
#define HBA_PxIE_UFE	0x00000010
#define HBA_PxIE_DPE	0x00000020
#define HBA_PxIE_PCE	0x00000040
#define HBA_PxIE_DMPE	0x00000080
#define HBA_PxIE_PRCE	0x00400000
#define HBA_PxIE_IPME	0x00800000
#define HBA_PxIE_OFE	0x01000000
#define HBA_PxIE_INFE	0x04000000
#define HBA_PxIE_IFE	0x08000000
#define HBA_PxIE_HBDE	0x10000000
#define HBA_PxIE_HBFE	0x20000000
#define HBA_PxIE_TFEE	0x40000000
#define HBA_PxIE_CPDE	0x80000000

#define HBA_PxCMD_ST	0x00000001
#define HBA_PxCMD_SUD	0x00000002
#define HBA_PxCMD_POD	0x00000004
#define HBA_PxCMD_CLO	0x00000008
#define HBA_PxCMD_FRE	0x00000010
#define HBA_PxCMD_CCS	0x00001f00
#define HBA_PxCMD_MPSS	0x00002000
#define HBA_PxCMD_FR	0x00004000
#define HBA_PxCMD_CR	0x00008000
#define HBA_PxCMD_CPS	0x00010000
#define HBA_PxCMD_PMA	0x00020000
#define HBA_PxCMD_HPCP	0x00040000
#define HBA_PxCMD_MPSP	0x00080000
#define HBA_PxCMD_CPD	0x00100000
#define HBA_PxCMD_ESP	0x00200000
#define HBA_PxCMD_FBSCP	0x00400000
#define HBA_PxCMD_APSTE	0x00800000
#define HBA_PxCMD_ATAPI	0x01000000
#define HBA_PxCMD_DLAE	0x02000000
#define HBA_PxCMD_ALPE	0x04000000
#define HBA_PxCMD_ASP	0x08000000
#define HBA_PxCMD_ICC	0xf0000000

#define HBA_PxTFD_STS_ERR	0x00000001
#define HBA_PxTFD_STS_DRQ	0x00000008
#define HBA_PxTFD_STS_BST	0x00000080
#define HBA_PxTFD_ERR		0x0000ff00

#define HBA_PxSSTS_DEF	0x0000000f
#define HBA_PxSSTS_SPD	0x000000f0
#define HBA_PxSSTS_IPM	0x00000f00

#define HBA_PxSCTL_DET	0x0000000f
#define HBA_PxSCTL_SPD	0x000000f0
#define HBA_PxSCTL_IPM	0x00000f00
#define HBA_PxSCTL_SPM	0x0000f000
#define HBA_PxSCTL_PMP	0x000f0000

#define HBA_PxSERR_ERR_I	0x00000001
#define HBA_PxSERR_ERR_M	0x00000002
#define HBA_PxSERR_ERR_T	0x00000100
#define HBA_PxSERR_ERR_C	0x00000200
#define HBA_PxSERR_ERR_P	0x00000400
#define HBA_PxSERR_ERR_E	0x00000800
#define HBA_PxSERR_DIAG_N	0x00010000
#define HBA_PxSERR_DIAG_I	0x00020000
#define HBA_PxSERR_DIAG_W	0x00040000
#define HBA_PxSERR_DIAG_B	0x00080000
#define HBA_PxSERR_DIAG_D	0x00100000
#define HBA_PxSERR_DIAG_C	0x00200000
#define HBA_PxSERR_DIAG_H	0x00400000
#define HBA_PxSERR_DIAG_S	0x00800000
#define HBA_PxSERR_DIAG_T	0x01000000
#define HBA_PxSERR_DIAG_F	0x02000000
#define HBA_PxSERR_DIAG_X	0x04000000

#define HBA_PxSNTF_PMN		0x0000ffff

#define HBA_PxFBS_EN	0x00000001
#define HBA_PxFBS_DEC	0x00000002
#define HBA_PxFBS_SDE	0x00000004
#define HBA_PxFBS_DEV	0x00000f00
#define HBA_PxFBS_ADO	0x0000f000
#define HBA_PxFBS_DWE	0x000f0000

#define HBA_PxDEVSLP_ADSE	0x00000001
#define HBA_PxDEVSLP_DSP	0x00000002
#define HBA_PxDEVSLP_DETO	0x000003fc
#define HBA_PxDEVSLP_MDAT	0x00007c00
#define HBA_PxDEVSLP_DITO	0x01ff1000
#define HBA_PxDEVSLP_DM		0x1e000000

struct _HBA_ {
	u32 cap;//Host Capabilities
	volatile u32 ghc;//Global Host Control
	volatile u32 is;//Interrupt Status
	u32 pi;//Ports Implemented
	u32 vs;//Version
	volatile u32 ccc_ctl;//Command Completion Coalescing Control
	u32 ccc_ports;//Command Completion Coalescing Ports
	volatile u32 em_loc;//Enclosure Management Location
	volatile u32 em;//Enclosure Management Control
	volatile u32 cap2;//Host Capabilities Extend
	volatile u32 bohc;//BIOS/OS Handoff Control and Status
	u32 rvd[13];
	volatile u32 nvmhci[16];
	volatile u32 vsr[24];
	struct _AHCI_PORT_ {
		volatile u64 clb;//Command List Base Address,align 1K
		volatile u64 fb;//FIS Base Address,align 256,4K
		volatile u32 is;//Interrupt Sattus
		volatile u32 ie;//Interrupt Enable
		volatile u32 cmd;//Command and Status
		u32 rvd;
		volatile u32 tfd;//Task File Data
		u32 sig;//Signature
		volatile u32 ssts;//Serial ATA Status(SCR0:SStatus)
		volatile u32 sctl;//Serial ATA Control(SCR2:SControl)
		volatile u32 serr;//Serial ATA Error(SCR1:SError)
		volatile u32 sact;//Serial ATA Active(SCR3:SAtcive)
		volatile u32 ci;//Command Issue
		volatile u32 sntf;//Serial ATA Notification(SCR4:SNotification)
		volatile u32 fbs;//FIS-base Switching Cintrol
		volatile u32 devslp;//Device Sleep
		u32 rvd2[10];
		volatile u32 vs[4];
	} port[32];
};

struct _CLS_ {
	uint16_t cfl:5;
	uint16_t a:1;
	uint16_t w:1;
	uint16_t p:1;
	uint16_t r:1;
	uint16_t b:1;
	uint16_t c:1;
	uint16_t :1;
	uint16_t pmp:3;
	uint16_t prdtl;
	uint32_t prdbc;
	uint64_t ctba;//align 128
	uint32_t rvd[4];
};
struct _PRDTI_ {
	uint64_t dba;//algin 2
	uint32_t rvd;
	uint32_t dbc:22;
	uint32_t :9;
	uint32_t i:1;
};

struct _AHCI_CMD_ {
	volatile uint8_t r_fis[64];
	volatile uint8_t acmd[16];
	uint8_t rvd[48];
	volatile u8 data_block[8][16];//struct _PRDTI_
};//256
struct _AHCI_SOLT_ {
	volatile u8 solt[32][32];
};//1024
struct _AHCI_FIS_ {
	volatile uint8_t d_fis[0x1c];
	uint8_t rvd0[4];
	volatile uint8_t p_fis[0x14];
	uint8_t rvd1[0x0c];
	volatile uint8_t r_fis[0x14];
	uint8_t rvd2[4];
	volatile uint8_t sdbfis[8];
	volatile uint8_t u_fis[64];
	uint8_t rvd3[0x60];
} ;//256,we not suppose pmp,so need't align to 4K

struct _AHCI_ {
	struct _AHCI_ * next;
	LPPCIDEV dev;
	int hba_busy;
	int port_count;
	volatile struct _HBA_ * hba;
	void * ctrl[];
};


#endif