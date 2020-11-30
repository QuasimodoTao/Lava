/*
	inc/lfs.h
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

#ifndef _LFS_H_
#define _LFS_H_

#define	NodeAttrSys		0x0001
#define NodeAttrDir		0x0002
#define NodeAttrData	0x0004
#define NodeAttrLinker	0x0008
#define NodeAttrUCopy	0x0010
#define NodeAttrULink	0x0020
#define NodeAttrUExe	0x0040
#define NodeAttrURead	0x0080
#define NodeAttrUWrite	0x0100
#define NodeAttrUDel	0x0200
#define NodeAttrSCopy	0x0400
#define NodeAttrSLink	0x0800
#define NodeAttrSExe	0x1000
#define NodeAttrSRead	0x2000
#define NodeAttrSWrite	0x4000
#define NodeAttrSDel	0x8000

#define DirAttrHide		0x0001
#define DirAttrDir		0x0002
#define DirAttrExist	0x0004
#define DirAttrLinker	0x0008
#define DirAttrUCopy	0x0010
#define DirAttrULink	0x0020
#define DirAttrUExe		0x0040
#define DirAttrURead	0x0080
#define DirAttrUWrite	0x0100
#define DirAttrUDel		0x0200
#define DirAttrSCopy	0x0400
#define DirAttrSLink	0x0800
#define DirAttrSExe		0x1000
#define DirAttrSRead	0x2000
#define DirAttrSWrite	0x4000
#define DirAttrSDel		0x8000

struct LFS_DataHeader {
	//NodeNode is 0,RootNode is 1,BadNode is 2,MapNode is 3
	u16 MinorVersion;
	u16 MajorVersion;
	u32 BlockCount;
	u64 SectorCount;
	u32 Node0Block;//2
	u32 rvd0;//0
	u32 rvd1;//1
	u32 rvd2;//2
	u32 rvd3;//3
	u32 LMNode;//4
	u32 FirstFreeBlock;
	u32 FirstFreeNode;
	u64 GlobalBlockCount;
	u64 GlobalBlockFree;
	GUID FirstDisk;
	GUID FirstPart;
	GUID LastDisk;
	GUID LastPart;
	GUID PrevDisk;
	GUID PrevPart;
	GUID NextDisk;
	GUID NextPart;
	wchar_t VolName[(512 - 192) / sizeof(wchar_t)];//
};
struct LFS_Dictionary {
	uint16_t NameLen;
	u16 Attr;
	u32 Node;
	u64 Create;
	GUID Auther;
	//wchar_t Name[NameLen + 1]
	//wchar_t Descript[DescLen + 1]
	//pack size align to 16;
};

struct Node {
	u64 Create;
	u64 Access;
	u64 Modify;
	u64 Size;
	GUID Auther;
	u16 Attr;
	u16 Link;
	union {
		u32 Block[19];
		u8 Data[76];
	};
};


/*


#define NODE_ATTR_SYS_COPY	0x00000001
#define NODE_ATTR_SYS_LINK	0x00000002
#define NODE_ATTR_SYS_EXE	0x00000004
#define NODE_ATTR_SYS_READ	0x00000008
#define NODE_ATTR_SYS_WRITE	0x00000010
#define NODE_ATTR_SYS_DEL	0x00000020

#define NODE_ATTR_USR_COPY	0x00000100
#define NODE_ATTR_USR_LINK	0x00000200
#define NODE_ATTR_USR_EXE	0x00000400
#define NODE_ATTR_USR_READ	0x00000800
#define NODE_ATTR_USR_WRITE	0x00001000
#define NODE_ATTR_USR_DEL	0x00002000

#define NODE_ATTR_DIR		0x00010000
#define NODE_ATTR_DATA		0x00020000
#define NODE_ATTR_EXIST		0x00040000

#define DIR_ATTR_SYS_COPY	0x00000001
#define DIR_ATTR_SYS_LINK	0x00000002
#define DIR_ATTR_SYS_EXE	0x00000004
#define DIR_ATTR_SYS_READ	0x00000008
#define DIR_ATTR_SYS_WRITE	0x00000010
#define DIR_ATTR_SYS_DEL	0x00000020

#define DIR_ATTR_USR_COPY	0x00000100
#define DIR_ATTR_USR_LINK	0x00000200
#define DIR_ATTR_USR_EXE	0x00000400
#define DIR_ATTR_USR_READ	0x00000800
#define DIR_ATTR_USR_WRITE	0x00001000
#define DIR_ATTR_USR_DEL	0x00002000

#define DIR_ATTR_DIR		0x00010000
#define DIR_ATTR_HIDE		0x00020000
#define DIR_ATTR_EXIST		0x00040000

struct _node_ {
	u64 create;
	u64 access;
	u64 modify;
	u64 size;
	u32 attr;
	u32 blocks;
	u16 link;
	u16 rvd0;
	u32 rvd1[5];
	union{
		u32 block[16];
		u8 data[64];
	};
};

//boot code store at block 0,-2,
struct _head_ {//store at block 1,-1
	u16 minor_version;
	u16 major_version;
	u32 block_count;
	u64 sector_count;
	u32 node0block;//node's node is 0,root node is 1,bad block node is 2,map node is 3
	u32 first_free_block;
	u32 first_free_node;
	u32 rvd0;
	GUID first_disk;
	GUID first_part;
	GUID last_disk;
	GUID last_part;
	GUID prev_disk;
	GUID prev_part;
	GUID next_disk;
	GUID next_part;
	u32 first_block_num;
	u32 last_block_num;
	u32 rvd1[10];
	wchar_t volue_name[];
};

struct _dir_ {
	u16 name_len;
	u16 desc_len;
	u32 attr;
	u32 node;
	u32 rvd1[5];
	wchar_t name_desc[];//name[],desc[]
};

*/

#endif