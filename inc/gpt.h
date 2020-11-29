//Lava OS
//PageFault
//20-04-11-15-00


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