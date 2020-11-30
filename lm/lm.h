/*
	lm/lm.h
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
#ifndef _LM_H_
#define _LM_H_

#include <asm.h>
#include <stddef.h>
#include <pci.h>
#include <string.h>
#include <lfs.h>

struct _NODE_ {
	struct Node pNode;
	u32 iNode;
	u32 iBlock0;
	u32 iBlock1;
	u32 iBlock2;
	u32 viBuf;
	u64 GPos;
	u32 * pBlock0;
	u32 * pBlock1;
	u32 * pBlock2;
	void * RBuf;
};


unsigned int ComputeCRC32(void * Data, int Len);

uint8_t lm_PCIReadByte(HANDLE _Dev,uint8_t Reg);
uint16_t lm_PCIReadWord(HANDLE _Dev,uint8_t Reg);
uint32_t lm_PCIReadDword(HANDLE _Dev,uint8_t Reg);
int lm_PCIWriteByte(HANDLE _Dev,uint8_t Reg,uint8_t v);
int lm_PCIWriteWord(HANDLE _Dev,uint8_t Reg,uint16_t v);
int lm_PCIWriteDword(HANDLE _Dev,uint8_t Reg,uint32_t v);
HANDLE lm_PCISearchClass(HANDLE _Dev,uint32_t Class,uint32_t Subclass,uint32_t ProgIF);
int64_t lm_CfgPCI(int64_t MemoryStart, int32_t PCIBIOSEnt,uint16_t PCIax,uint16_t PCIbx,uint16_t PCIcx);
int ReadDisk(u64 LBA,void * Buf,u16 Count);
int Seek(s64 Pos,int Org,struct _NODE_ * pNode);
int Read(struct _NODE_ * pNode,void * Buf,u32 Count);
u64 Tell(struct _NODE_ * pNode);
struct _NODE_ * Open(const wchar_t * Name);
void * malloc(u32 Size);
void free(void * P);
void ShowHexs(void * _p,int Line);
struct _NODE_ * Open(const wchar_t * Name);
int Close(struct _NODE_ * File);

#endif