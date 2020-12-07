/*
	lm/lfs.c
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
#include <stddef.h>
#include <lfs.h>
#include "lm.h"
#include <gpt.h>
#include <stdio.h>

static u64 PartFirst;
static u64 PartLast;
static u32 BlockCount;
static struct _NODE_ Node;
static struct _NODE_ Root;

static int PartRead(u64 LBA,void * Buf,u16 Cnt){
	if(PartFirst + LBA + Cnt <= PartLast) return ReadDisk(LBA + PartFirst,Buf,Cnt);
	print("Read sector out of rang.\n");
	return -1;
}
static int ReadBlock(u32 iBlock,void * pBlock,u8 Cnt){
	if(iBlock == 0xffffffff) {
		print("Read not exist block.\n");
		return -1;
	}
	if(iBlock + Cnt < BlockCount) return PartRead(iBlock * 8,pBlock,Cnt * 8);
	printk("Read block out of rang,%d.\n",iBlock);
	return -1;
}
static int BlockV2R(struct _NODE_ * pNode,u32 viBlock){
	u32 riBlock0;
	u32 riBlock1;
	u32 riBlock2;
	u32 Index;
	
	if(!viBlock && pNode->pNode.Attr & NodeAttrData) return 0xffffffff;
	if(viBlock < 16) return pNode->pNode.Block[viBlock];
	viBlock -= 16;
	if(viBlock < 1024){
		riBlock0 = pNode->pNode.Block[16];
		if(pNode->iBlock0 != riBlock0) {
			pNode->iBlock0 = riBlock0;
			ReadBlock(riBlock0,pNode->pBlock0,1);
		}
		return pNode->pBlock0[viBlock];
	}
	viBlock -= 1024;
	if(viBlock < 1024 * 1024){
		riBlock0 = pNode->pNode.Block[17];
		if(pNode->iBlock0 != riBlock0){
			pNode->iBlock0 = riBlock0;
			ReadBlock(riBlock0,pNode->pBlock0,1);
		}
		Index = viBlock >> 10;
		riBlock1 = pNode->pBlock0[Index];
		if(pNode->iBlock1 != riBlock1){
			pNode->iBlock1 = riBlock1;
			ReadBlock(riBlock1,pNode->pBlock1,1);
		}
		viBlock &= 0x3ff;
		return pNode->pBlock1[viBlock];
	}
	viBlock -= 1024*1024;
	if(viBlock < 1024 * 1024 * 1024){
		riBlock0 = pNode->pNode.Block[18];
		if(pNode->iBlock0 != riBlock0){
			pNode->iBlock0 = riBlock0;
			ReadBlock(riBlock0,pNode->pBlock0,1);
		}
		Index = viBlock >> 20;
		riBlock1 = pNode->pBlock0[Index];
		if(pNode->iBlock1 != riBlock1){
			pNode->iBlock1 = riBlock1;
			ReadBlock(riBlock1,pNode->pBlock1,1);
		}
		Index = (viBlock >> 10) & 0x3ff;
		riBlock2 = pNode->pBlock1[Index];
		if(pNode->iBlock2 != riBlock2){
			pNode->iBlock2 = riBlock2;
			ReadBlock(riBlock2,pNode->pBlock2,1);
		}
		viBlock &= 0x3ff;
		return pNode->pBlock2[viBlock];
	}
	return 0xffffffff;
}
int Seek(s64 Pos,int Org,struct _NODE_ * pNode){
	u32 viBlock;
	u32 riBlock;
	
	if(Org == SEEK_SET){
		if(Pos < 0) return -1;
		pNode->GPos = Pos;
	}
	else if(Org == SEEK_CUR){
		if(Pos + pNode->GPos < 0) return -1;
		pNode->GPos += Pos;
	}
	else if(Org == SEEK_END){
		if(Pos > 0) return -1;
		pNode->GPos = pNode->pNode.Size + Pos;
	}
	else return -1;
	viBlock = pNode->GPos >> 12;
	if(pNode->viBuf == viBlock) return 0;
	pNode->viBuf = viBlock;
	riBlock = BlockV2R(pNode,viBlock);
	if(riBlock == 0xffffffff && !viBlock) {
		memcpy(pNode->RBuf,pNode->pNode.Data,76);
		memset(((u8*)(pNode->RBuf)) + 76,0,4096-76);
		return 0;
	}
	ReadBlock(riBlock,pNode->RBuf,1);
	return 0;
}
int Read(struct _NODE_ * pNode,void * Buf,u32 Count){
	u8 * _Buf;
	u32 Rem;

	_Buf = (u8*)Buf;
	Rem = 4096 - (pNode->GPos & 0xfff);
	if(Count <= Rem){
		memcpy(_Buf,((u8*)(pNode->RBuf)) + 4096 - Rem,Count);
		Seek(Count,SEEK_CUR,pNode);
		return 0;
	}
	if(Count > Rem){
		Rem = 4096 - (pNode->GPos & 0xfff);
		memcpy(_Buf,((u8*)(pNode->RBuf)) + 4096 - Rem,Rem);
		Count -= Rem;
		_Buf += Rem;
		Seek(Rem,SEEK_CUR,pNode);
	}
	while(Count >= 4096){
		memcpy(_Buf,pNode->RBuf,4096);
		_Buf += 4096;
		Count -= 4096;
		Seek(4096,SEEK_CUR,pNode);
	}
	if(Count){
		memcpy(_Buf,pNode->RBuf,Count);
		Seek(Count,SEEK_CUR,pNode);
	}
	return 0;
}
u64 Tell(struct _NODE_ * pNode){
	return pNode->GPos;
}
static int GetNode(u32 iNode, struct Node * pNode){
	Seek(iNode * 128,SEEK_SET,&Node);
	Read(&Node,pNode,128);
	return 0;
}
struct _NODE_ * Open(const wchar_t * Name){
	struct _NODE_ * Ret;
	struct LFS_Dictionary Dir;
	wchar_t * _Name;
	u32 NameLen;
	u64 Pos;
	u32 iNode;

	Seek(0,SEEK_SET,&Root);
	if(Name[0] == L'/' || Name[0] == L'\\') Name++;
	NameLen = wcslen(Name);
	_Name = malloc(NameLen * 2 + 0x0f);
	while(1){
		Read(&Root,&Dir,sizeof(struct LFS_Dictionary));
		if(!Dir.NameLen) {
			free(_Name);
			return NULL;
		}
		if(Dir.Attr & DirAttrExist && !(Dir.Attr & DirAttrDir) && Dir.NameLen == NameLen){
			Read(&Root,_Name,sizeof(wchar_t) * ((NameLen + 0x07) & 0xfff8));
			if(!wcsncmp(Name,_Name,NameLen)){
				free(_Name);
				Ret = malloc(sizeof(struct _NODE_));
				Ret->iNode = Dir.Node;
				GetNode(Ret->iNode,&Ret->pNode);
				Ret->pBlock0 = malloc(4096);
				Ret->pBlock1 = malloc(4096);
				Ret->pBlock2 = malloc(4096);
				Ret->iBlock0 = 0;
				Ret->iBlock1 = 0;
				Ret->iBlock2 = 0;
				Ret->RBuf = malloc(4096);
				Ret->GPos = 0;
				Ret->viBuf = 0xffffffff;
				Seek(0,SEEK_SET,Ret);
				return Ret;
			}
		}
		else Seek(((Dir.NameLen + 0x7) & 0xfff8) * sizeof(wchar_t),SEEK_CUR,&Root);
	}
}
int Close(struct _NODE_ * File){
	free(File->pBlock0);
	free(File->pBlock1);
	free(File->pBlock2);
	free(File->RBuf);
	free(File);
	return 0;
}
int64_t LFSInit(int64_t MemoryStart,GUID * ActivePart){
	struct GPTHeader * Header;
	struct GPTEntry * Entry;
	u8 * MBR;
	u64 GPTFirst;
	int i;
	u32 EntSize, EntCnt, CRC32;
	struct LFS_DataHeader * fHeader;
	
	Header = (struct GPTHeader *)MemoryStart;
	Entry = (struct GPTEntry *)MemoryStart;
	MBR = (u8*)MemoryStart;
	ReadDisk(0,MBR,1);
	for(i = 0;i < 4;i++){
		if(MBR[0x1c2 + 16 * i] == 0xee){
			GPTFirst = (u64)MBR[0x1c6+16*i] | (((u64)MBR[0x1c7+16*i]) << 8) | (((u64)MBR[0x1c8+16*i]) << 8) | (((u64)MBR[0x1c9+16*i]) << 8);
			break;
		}
	}
	ReadDisk(GPTFirst,Header,1);
	if(Header->Signature != 0x5452415020494645) return 0;
	if(Header->Revision != 0x00010000) return 0;
	CRC32 = Header->HeaderCRC32;
	Header->HeaderCRC32 = 0;
	if(CRC32 != ComputeCRC32(Header,Header->HeaderSize)){
		ReadDisk(Header->AlternateLBA,Header,1);
		if(Header->Signature != 0x5452415020494645) return 0;
		if(Header->Revision != 0x00010000) return 0;
		CRC32 = Header->HeaderCRC32;
		Header->HeaderCRC32 = 0;
		if(CRC32 != ComputeCRC32(Header,Header->HeaderSize)) return 0;
	}
	CRC32 = Header->PartitionEntryArrayCRC32;
	EntSize = Header->SizeOfPartitionEntry;
	EntCnt = Header->NumberOfPartitionEntries;
	ReadDisk(Header->PartitionEntryLBA,Entry,EntSize * EntCnt / 512);
	if(CRC32 != ComputeCRC32(Entry,EntSize * EntCnt)) return 0;
	for(i = 0;i < EntCnt;i++)
		if(!memcmp(&Entry[i].PatitionGUID,ActivePart,sizeof(GUID))) break;
	PartFirst = Entry[i].StartLBA;
	PartLast = Entry[i].EndingLBA;
	fHeader = (struct LFS_DataHeader *)MemoryStart;
	PartRead(8,fHeader,1);
	BlockCount = fHeader->BlockCount;
	Node.iBlock0 = 0xffffffff;
	Node.pBlock0 = (u32*)MemoryStart;
	Node.iBlock1 = 0xffffffff;
	Node.pBlock1 = (u32*)(MemoryStart += 4096);
	Node.iBlock2 = 0xffffffff;
	Node.pBlock2 = (u32*)(MemoryStart += 4096);
	Node.iNode = 0;
	Node.RBuf = (u32*)(MemoryStart += 4096);
	Node.viBuf = 0;
	Node.GPos = 0;
	Root.iBlock0 = 0xffffffff;
	Root.pBlock0 = (u32*)(MemoryStart += 4096);
	Root.iBlock1 = 0xffffffff;
	Root.pBlock1 = (u32*)(MemoryStart += 4096);
	Root.iBlock2 = 0xffffffff;
	Root.pBlock2 = (u32*)(MemoryStart += 4096);
	Root.iNode = 1;
	Root.RBuf = (u32*)(MemoryStart += 4096);
	Root.viBuf = 0xffffffff;
	Root.GPos = 0;
	MemoryStart += 4096;
	i = ReadBlock(fHeader->Node0Block,Node.RBuf,1);
	memcpy(&Node.pNode,((struct Node*)(Node.RBuf))+Node.iNode,sizeof(struct Node));
	memcpy(&Root.pNode,((struct Node*)(Node.RBuf))+Root.iNode,sizeof(struct Node));
	return MemoryStart;	
}

