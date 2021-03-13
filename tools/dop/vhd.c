/*
	tools/dop/vhd.c
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

#include "dop.h"
#include <objbase.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "error.h"
#include <string.h>
#include "fs.h"

struct VHDFooter {
	int64_t cookie;
	int32_t features;
	int32_t file_format_version;
	int64_t data_offest;
	int32_t time_stamp;
	int32_t creator_application;
	int32_t creator_version;
	int32_t creator_host_os;
	int64_t original_size;
	int64_t current_size;
	int32_t disk_geometry;
	int32_t disk_type;
	int32_t checksum;
	GUID guid;
	int8_t saved_state[428];
};
struct ParentLocatorEntries {
	int32_t platfoem_code;
	int32_t platform_data_space;
	int32_t platform_data_length;
	int32_t rsvd;
	int64_t platform_data_offest;
};
struct VHDDynamicHeader {
	int64_t cookie;
	int64_t data_offset;
	int64_t table_offset;
	int32_t header_version;
	int32_t max_table_entries;
	int32_t block_size;
	int32_t checksum;
	GUID parent_guid;
	int32_t parent_time_stamp;
	int32_t rsvd;
	wchar_t parent_name[256];
	struct ParentLocatorEntries parent_locator_entry[8];
	int8_t rvd2[256];
};
typedef struct _VHD_ {
	u8 dynamic;
	u8 monopolized;
	unsigned int block_size;
	unsigned int block_max;
	unsigned int sector_per_block;
	unsigned int bat_size;
	uint32_t cache_size;
	struct _DISK_INFO_ info;
	FILE* file;
	uint32_t* bat;
	uint64_t count;
	uint64_t bat_offset;
	uint64_t size;
	struct VHDFooter footer;
	struct VHDDynamicHeader header;
	LPDOPSTREAM flist;
} VHD, * LPVHD;
static int vhd_read_block(struct _BUFFER_HEAD_* bh);
static int vhd_write_block(struct _BUFFER_HEAD_* bh);

static VHD vhd;
static int is_open = 0;
static struct _LL_BLOCK_DEV_ vhd_ll_rw = {
	.cache_size = DISK_CACHE_BLOCK_SIZE,
	.read_block = vhd_read_block,
	.write_block = vhd_write_block
};
static int vhd_read_block(struct _BUFFER_HEAD_ * bh) {
	unsigned int Table, Off, i;
	u64 iblock;
	u32 count;
	void* pblock;

	if (!is_open) return -1;
	iblock = bh->iblock;
	count = DISK_CACHE_BLOCK_SIZE >> 9;
	pblock = bh->addr;
	if (iblock >= vhd.count) {
		return -DiskSeekOverflow;
	}
	if (iblock + (u64)count >= vhd.count) count = (uint32_t)(vhd.count - iblock);
	if (vhd.dynamic) {
		if (vhd.sector_per_block >= (DISK_CACHE_BLOCK_SIZE >> 9)) {
			Table = (uint32_t)(iblock / vhd.sector_per_block);
			if (vhd.bat[Table] == 0xffffffff) memset(pblock, 0, 512LLU * count);
			else {
				Off = (uint32_t)(iblock % vhd.sector_per_block);
				Off++;
				_fseeki64(vhd.file, (((uint64_t)(vhd.bat[Table])) + Off) << 9, SEEK_SET);
				fread(pblock, 512, count, vhd.file);
			}
		}
		else {
			for (i = 0;i < count;i++) {
				Table = (uint32_t)(iblock / vhd.sector_per_block);
				if (vhd.bat[Table] == 0xffffffff) memset(pblock, 0, 512);
				else {
					Off = (uint32_t)(iblock % vhd.sector_per_block);
					Off++;
					_fseeki64(vhd.file, (((uint64_t)(vhd.bat[Table])) + Off) << 9, SEEK_SET);
					fread(pblock, 1, 512, vhd.file);
				}
				pblock = (void*)((int64_t)pblock + 512);
				iblock++;
			}
		}
	}
	else {
		_fseeki64(vhd.file, iblock << 9, SEEK_SET);
		fread(pblock, 1, (int64_t)count << 9, vhd.file);
		iblock += count;
	}
	return 0;
}
static int vhd_write_block(struct _BUFFER_HEAD_ * bh) {
	unsigned int Table, Off, i, j;
	char * Sector;
	uint64_t iblock;
	void* pblock;
	unsigned int count;

	if (!is_open) return - 1;
	iblock = bh->iblock;
	pblock = bh->addr;
	count = DISK_CACHE_BLOCK_SIZE >> 9;
	if (iblock >= vhd.count) {
		return -DiskSeekOverflow;
	}
	if (iblock + count >= vhd.count) count = (uint32_t)(vhd.count - iblock);
	if (!count) return 0;
	if (vhd.dynamic) {
		if (vhd.sector_per_block >= (DISK_CACHE_BLOCK_SIZE >> 9)) {
			Table = (uint32_t)(iblock / vhd.sector_per_block);
			Off = (uint32_t)(iblock % vhd.sector_per_block);
			Off++;
			if (vhd.bat[Table] == 0xffffffff) {
				vhd.bat[Table] = (uint32_t)((vhd.size - 512) >> 9);
				vhd.size += 2 * 1024 * 1024 + 512;
				if (!(Sector = malloc(512))) exit(-1);
				memset(Sector, 0xffffffff, 512);
				_fseeki64(vhd.file, ((uint64_t)(vhd.bat[Table])) << 9, SEEK_SET);
				fwrite(Sector, 1, 512, vhd.file);
				memset(Sector, 0, 512);
				for (j = 0;j < vhd.sector_per_block;j++) fwrite(Sector, 1, 512, vhd.file);
				free(Sector);
			}
			_fseeki64(vhd.file, (((uint64_t)(vhd.bat[Table])) + Off) << 9, SEEK_SET);
			fwrite(pblock, 512, count, vhd.file);
			pblock = (void*)((int64_t)pblock + 512);
			iblock++;
		}
		else {
			for (i = 0;i < count;i++) {
				Table = (uint32_t)(iblock / vhd.sector_per_block);
				Off = (uint32_t)(iblock % vhd.sector_per_block);
				Off++;
				if (vhd.bat[Table] == 0xffffffff) {
					vhd.bat[Table] = (uint32_t)((vhd.size - 512) >> 9);
					vhd.size += 2 * 1024 * 1024 + 512;
					if (!(Sector = malloc(512))) exit(-1);
					memset(Sector, 0xffffffff, 512);
					_fseeki64(vhd.file, ((uint64_t)(vhd.bat[Table])) << 9, SEEK_SET);
					fwrite(Sector, 1, 512, vhd.file);
					memset(Sector, 0, 512);
					for (j = 0;j < vhd.sector_per_block;j++) fwrite(Sector, 1, 512, vhd.file);
					free(Sector);
				}
				_fseeki64(vhd.file, (((uint64_t)(vhd.bat[Table])) + Off) << 9, SEEK_SET);
				fwrite(pblock, 1, 512, vhd.file);
				pblock = (void*)((int64_t)pblock + 512);
				iblock++;
			}
		}
	}
	else {
		_fseeki64(vhd.file, iblock << 9, SEEK_SET);
		fwrite(pblock, 1, (int64_t)count << 9, vhd.file);
		iblock += count;
	}
	return 0;
}
static LPDOPSTREAM vhd_open(wchar_t* name, u64 mode, LPFCPEB fc) {
	LPDOPSTREAM _vhd;

	if (!is_open) return NULL;
	if (mode & FS_OPEN_MONOPOLIZE && !(mode & FS_OPEN_WRITE)) mode ^= FS_OPEN_MONOPOLIZE;
	if (vhd.monopolized && mode & FS_OPEN_MONOPOLIZE) return NULL;
	_vhd = malloc(sizeof(DOPSTREAM));
	if (!_vhd) return NULL;
	memset(_vhd, 0, sizeof(DOPSTREAM));
	if (mode & FS_OPEN_MONOPOLIZE) vhd.monopolized = 1;
	_vhd->mode = (int)mode;
	_vhd->fc = fc;
	InsertList(vhd.flist, _vhd, d_prev, d_next);
	return _vhd;
}
static int vhd_close(LPDOPSTREAM _vhd) {
	if (!is_open) return -1;
	RemoveList(vhd.flist, _vhd, d_prev, d_next);
	if (_vhd->mode & FS_OPEN_MONOPOLIZE) vhd.monopolized = 0;
	free(_vhd);
	return 0;
}
static int vhd_info(LPDOPSTREAM _vhd, size_t size, void* _info) {

	if (!is_open) return -1;
	if (size < sizeof(struct _DISK_INFO_)) return -1;
	memcpy(_info, &(vhd.info), sizeof(struct _DISK_INFO_));
	return 0;
}
static struct _BUFFER_HEAD_* vhd_bread(LPDOPSTREAM _vhd, u64 iblock) {
	if (!is_open) return NULL;
	if (vhd.monopolized && !(_vhd->mode & FS_OPEN_MONOPOLIZE)) return NULL;
	if (iblock & ((DISK_CACHE_BLOCK_SIZE / 512) - 1)) {
		return ml_blink(&vhd_ll_rw, 
			iblock & ~((DISK_CACHE_BLOCK_SIZE / 512) - 1), 
			(iblock & ((DISK_CACHE_BLOCK_SIZE / 512) - 1)) * 512, 
			DISK_CACHE_BLOCK_SIZE - (iblock & ((DISK_CACHE_BLOCK_SIZE / 512) - 1)) * 512);
	}
	else {
		return ll_bread(&vhd_ll_rw, iblock);
	}
}
static struct _BUFFER_HEAD_* vhd_cache_bwrite(LPDOPSTREAM _vhd, u64 iblock) {
	if (!is_open) return NULL;
	if (vhd.monopolized && !(_vhd->mode & FS_OPEN_MONOPOLIZE)) return NULL;
	return ll_balloc(&vhd_ll_rw, iblock);
}
static FCPEB vhd_fc = {
	.open = vhd_open,
	.close = vhd_close,
	.bread = vhd_bread,
	.cache_bwrite = vhd_cache_bwrite,
	.info = vhd_info
};
int close_vhd(HANDLE _vhd) {//�ر��������
	unsigned int i;
	LPDOPSTREAM f0;

	if (_vhd != &vhd || !is_open) return -1;
	while (vhd.flist) {
		if (vhd.flist->call_back) vhd.flist->call_back(vhd.flist, vhd.flist->call_back_data);
		else {
			f0 = vhd.flist->d_next;
			free(vhd.flist);
			f0->d_prev = NULL;
			vhd.flist = f0;
		}
	}
	bsync(&vhd_ll_rw);
	if (vhd.dynamic) {
		for (i = 0;i < vhd.block_max;i++) vhd.bat[i] = Bswap32(vhd.bat[i]);
		_fseeki64(vhd.file, vhd.bat_offset, SEEK_SET);
		fwrite(vhd.bat, 1, vhd.bat_size, vhd.file);
		free(vhd.bat);
		_fseeki64(vhd.file, vhd.size - 512, SEEK_SET);
		fwrite(&vhd.footer, 1, 512, vhd.file);
	}
	fclose(vhd.file);
	fs_unmap(VHD_MAP_NAME);
	return 0;
}
HANDLE create_vhd(const wchar_t * name, uint64_t count) {//�����������
	GUID guid;
	int i, DiskGeometry;
	time_t TimeStamp;
	unsigned int Checksum;

	if (is_open) return NULL;
	memset(&vhd, 0, sizeof(VHD));
	vhd.file = _wfopen(name, L"wb+");
	if (!vhd.file) {
		wprintf(L"Can't create vhd file.\n");
		return NULL;
	}
	fseek(vhd.file, 0, SEEK_END);
	if (_ftelli64(vhd.file)) {
		wprintf(L"Can not create an exist VHD file.\n");
		fclose(vhd.file);
		return NULL;
	}
	vhd.dynamic = 1;
	vhd.count = count;
	vhd.footer.cookie = 0x78697463656e6f63;
	vhd.footer.creator_host_os = 0x6b326957;
	vhd.footer.features = 0x02000000;
	vhd.footer.file_format_version = 0x00000100;
	vhd.footer.data_offest = 0x0002000000000000;
	vhd.footer.creator_application = 0x206e6977;
	vhd.footer.creator_version = 0x01000600;
	vhd.footer.current_size = vhd.footer.original_size = Bswap64(count << 9);
	vhd.footer.disk_type = 0x03000000;
	if (CreateGuid(&guid) != S_OK) return NULL;
	time(&TimeStamp);
	vhd.footer.time_stamp = Bswap32((unsigned int)TimeStamp);
	DiskGeometry = 0x103f;
	i = (int)((count + 63 * 16) / (63 * 16));
	DiskGeometry |= i << 16;
	vhd.footer.disk_geometry = Bswap32(DiskGeometry);
	memcpy(&vhd.footer.guid, &guid, sizeof(GUID));
	Checksum = 0;
	for (i = 0;i < sizeof(struct VHDFooter);i++) Checksum += ((unsigned char*)&vhd.footer)[i];
	Checksum ^= 0xffffffff;
	vhd.footer.checksum = Bswap32(Checksum);
	vhd.block_size = 0x00200000;
	vhd.sector_per_block = 0x00200000 >> 9;
	vhd.block_max = (uint32_t)(count / vhd.sector_per_block);
	if (count%vhd.sector_per_block) vhd.block_max++;
	vhd.bat_size = (vhd.block_max * 4 + 0x1ff) & 0xfffffe00;
	if (!(vhd.bat = malloc(vhd.bat_size))) return NULL;
	memset(vhd.bat, 0xffffffff, (size_t)(vhd.block_max) * 4);
	memset(vhd.bat + vhd.block_max, 0, vhd.bat_size - vhd.block_max * 4);
	vhd.header.cookie = 0x6573726170737863;
	vhd.header.data_offset = 0xffffffffffffffff;
	vhd.header.table_offset = 0x0006000000000000;
	vhd.bat_offset = 0x600;
	vhd.header.header_version = 0x00000100;
	vhd.header.max_table_entries = Bswap32(vhd.block_max);
	vhd.header.block_size = 0x00002000;
	Checksum = 0;
	for (i = 0;i < sizeof(struct VHDDynamicHeader);i++) Checksum += ((unsigned char*)&vhd.header)[i];
	Checksum ^= 0xffffffff;
	vhd.header.checksum = Bswap32(Checksum);
	_fseeki64(vhd.file, 0, SEEK_SET);
	fwrite(&vhd.footer, 1, 512, vhd.file);
	_fseeki64(vhd.file, 512, SEEK_SET);
	fwrite(&vhd.header, 1, 1024, vhd.file);
	_fseeki64(vhd.file, 1024 + 512, SEEK_SET);
	fwrite(vhd.bat, 1, vhd.bat_size, vhd.file);
	_fseeki64(vhd.file, 1024LLU + 512 + vhd.bat_size, SEEK_SET);
	fwrite(&vhd.footer, 1, 512, vhd.file);
	vhd.size = (int64_t)(vhd.bat_size) + 2048;

	vhd.info.size = sizeof(struct _DISK_INFO_);
	vhd.info.physical_sector_size = 512;
	vhd.info.logical_sector_size = 512;
	vhd.info.cache_block_size = DISK_CACHE_BLOCK_SIZE;
	vhd.info.logical_sector_count = vhd.count;
	vhd.info.path = VHD_MAP_NAME;
	memcpy(&vhd.info.g_id, &(vhd.footer.guid), sizeof(GUID));
	fs_map(VHD_MAP_NAME, &vhd_fc, NULL);
	is_open = 1;
	return &vhd;
}
HANDLE open_vhd(const wchar_t * name) {//���������
	unsigned int Checksum = 0, _Checksum;
	unsigned int i = 0;

	if (is_open) return NULL;
	memset(&vhd, 0, sizeof(VHD));
	vhd.file = _wfopen(name, L"rb+");
	if (!vhd.file) {
		wprintf(L"Open vhd file faile.");
		return NULL;
	}
	_fseeki64(vhd.file, -512, SEEK_END);
	fread(&vhd.footer, 1, sizeof(struct VHDFooter), vhd.file);
	if (vhd.footer.cookie != 0x78697463656e6f63) {
		fclose(vhd.file);
		wprintf(L"Bad vhd file.");
		return NULL;
	}
	if (vhd.footer.file_format_version != 0x00000100) {
		fclose(vhd.file);
		wprintf(L"Unsuppose vhd version.");
		return NULL;
	}
	if (vhd.footer.data_offest == 0xffffffffffffffff) vhd.dynamic = 0;
	else vhd.dynamic = 1;
	vhd.size = _ftelli64(vhd.file);
	_Checksum = vhd.footer.checksum;
	vhd.footer.checksum = 0;
	for (i = 0;i < sizeof(struct VHDFooter);i++) Checksum += ((unsigned char*)&vhd.footer)[i];
	Checksum ^= 0xffffffff;
	Checksum = Bswap32(Checksum);
	if (Checksum != _Checksum) {
		fclose(vhd.file);
		wprintf(L"Bad vhd file.");
		return NULL;
	}
	vhd.footer.checksum = _Checksum;
	vhd.count = Bswap64(vhd.footer.original_size) >> 9;
	if (!vhd.dynamic) {

		vhd.info.size = sizeof(struct _DISK_INFO_);
		vhd.info.physical_sector_size = 512;
		vhd.info.logical_sector_size = 512;
		vhd.info.cache_block_size = DISK_CACHE_BLOCK_SIZE;
		vhd.info.logical_sector_count = vhd.count;
		vhd.info.path = VHD_MAP_NAME;
		memcpy(&vhd.info.g_id, &(vhd.footer.guid), sizeof(GUID));

		fs_map(VHD_MAP_NAME, &vhd_fc, NULL);
		is_open = 1;
		return &vhd;
	}
	_fseeki64(vhd.file, 512, SEEK_SET);
	fread(&vhd.header, 1, sizeof(struct VHDDynamicHeader), vhd.file);
	if (vhd.header.cookie != 0x6573726170737863) {
		fclose(vhd.file);
		wprintf(L"Bad vhd file.");
		return NULL;
	}
	if ((vhd.header.data_offset != 0xffffffffffffffff)
		|| (vhd.header.header_version != 0x00000100)) {
		fclose(vhd.file);
		wprintf(L"Unsuppose vhd version.");
		return NULL;
	}
	_Checksum = vhd.header.checksum;
	vhd.header.checksum = 0;
	Checksum = 0;
	for (i = 0;i < sizeof(struct VHDDynamicHeader);i++) Checksum += ((unsigned char*)&vhd.header)[i];
	_Checksum = Bswap32(_Checksum);
	if (Checksum + _Checksum != 0xffffffff) {
		fclose(vhd.file);
		wprintf(L"Bad vhd file.");
		return NULL;
	}
	vhd.header.checksum = _Checksum;
	vhd.block_size = Bswap32(vhd.header.block_size);
	vhd.block_max = Bswap32(vhd.header.max_table_entries);
	vhd.sector_per_block = vhd.block_size >> 9;
	if (!(vhd.bat = malloc(vhd.block_max * sizeof(uint32_t))));
	vhd.bat_offset = Bswap64(vhd.header.table_offset);
	_fseeki64(vhd.file, vhd.bat_offset, SEEK_SET);
	fread(vhd.bat, sizeof(uint32_t), vhd.block_max, vhd.file);
	vhd.bat_size = 0xfffffe00 & (vhd.block_max * sizeof(uint32_t) + 0x1ff);
	for (i = 0;i < vhd.block_max;i++) vhd.bat[i] = Bswap32(vhd.bat[i]);

	vhd.info.size = sizeof(struct _DISK_INFO_);
	vhd.info.physical_sector_size = 512;
	vhd.info.logical_sector_size = 512;
	vhd.info.cache_block_size = DISK_CACHE_BLOCK_SIZE;
	vhd.info.logical_sector_count = vhd.count;
	vhd.info.path = VHD_MAP_NAME;
	memcpy(&vhd.info.g_id, &(vhd.footer.guid), sizeof(GUID));

	fs_map(VHD_MAP_NAME, &vhd_fc, NULL);
	is_open = 1;
	return &vhd;
}
//Quasimodo