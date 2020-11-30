/*
	inc/gpt.h
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


#ifndef _GPT_H_
#define _GPT_H_

#include <stddef.h>

struct GPTHeader {
	u64 Signature;
	u32 Revision;
	u32 HeaderSize;
	u32 HeaderCRC32;
	u32 Rsvd;
	u64 MyLBA;
	u64 AlternateLBA;
	u64 FirstUseableLBA;
	u64 LastUseableLBA;
	GUID DiskGUID;
	u64 PartitionEntryLBA;
	u32 NumberOfPartitionEntries;
	u32 SizeOfPartitionEntry;
	u32 PartitionEntryArrayCRC32;
	u8 Rsvd1[512 - 92];
};
struct GPTEntry{
	GUID PartitionTypeGUID;
	GUID PatitionGUID;
	u64 StartLBA;
	u64 EndingLBA;
	u64 Attribute;
	u16 Name[36];
};


#endif