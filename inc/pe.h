/*
	inc/pe.h
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

#ifndef _PE_H_
#define _PE_H_

#include <stddef.h>

struct _MZ_HEAD_ {
	u16 Magic;
	u16 res2;
	u32 Res[14];
	u32 PEHead;
};
struct _TABLE_ {
	u32 Base;
	u32 Size;
};
struct _PE_SECTION_ {
	char Name[8];
	u32 VirtualSize;
	u32 VirtualAddress;
	u32 SizeOfRawData;
	u32 PointerToRAWData;
	u32 PointToRelocations;
	u32 PointerToLineNumbers;
	u16 NumberOfRelocations;
	u16 NumberOfLinenumbers;
	u32 Characteristics;
};
struct _PEP_HEAD_ {
	u32 MagicPE;
	u16 machine;
	u16 NumberOfSections;
	u32  TimeDataStamp;
	u32  PointerToSymblTable;
	u32  NumberOfSymbols;
	u16 SizeOfOptionalHeader;
	u16 Characteristics;
	u16 Magic;
	u8 MajorLinkerVersion;
	u8 MinorLinkerVersion;
	u32 SizeOdCode;
	u32 SizeOfInitlizedData;
	u32 SizeOfUninitlizedDta;
	u32 AddressOfEntryPoint;
	u32 BaseOfCode;
	u64 ImageBase;
	u32 SectionAlignment;
	u32 FileAlignment;
	u16 MajorOperatingSystemVersion;
	u16 MinorOperatingSystemVersion;
	u16 MajorImageVersion;
	u16 MinorImageVersion;
	u16 MajorSubsystemVersion;
	u16 MinorSubsystemVersion;
	u32  Win32VersionValue;
	u32 SizeOfimage;
	u32 SizeOfHeader;
	u32  CheckSum;
	u16 Subsystem;
	u16 DLLCharacteristics;
	u64 SizeOfStackReserve;
	u64 SizeOfStackCommit;
	u64 SizeOfHeapReserve;
	u64 SizeOfHeapCommit;
	u32 LoaderFlags;
	u32 NumberOfRvaAndSizes;
	struct _TABLE_ ExportTable;
	struct _TABLE_ ImportTable;
	struct _TABLE_ ResourceTable;
	struct _TABLE_ ExceptionTable;
	struct _TABLE_ CertificateTable;
	struct _TABLE_ BaseRelocationTable;
	struct _TABLE_ Debug;
	u64 Architecture;
	u64 GlobalPtr;
	u64 TLSTable;
	struct _TABLE_ LoadConfigTable;
	struct _TABLE_ BondImport;
	struct _TABLE_ IAT;
	struct _TABLE_ DelayImportDescriptor;
	struct _TABLE_ CLRRuntimeHeader;
	u64 Res;
};
struct _reloc_ {
	u32 PageRVA;
	u32 BlockSize;
};
struct _TypeOffset_ {
	u16 Offset : 12;
	u16 Type : 4;
};

#endif
