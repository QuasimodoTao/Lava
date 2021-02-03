#include "dop.h"
#include "error.h"
#include <stdio.h>
#include "fs.h"
#include <stddef.h>

//we not suppose MBR
//only suppose GPT

struct _MBRRECODE_ {
	uint8_t flag;
	uint8_t s_head;
	uint8_t s_sector;
	uint8_t s_track;
	uint8_t type;
	uint8_t e_head;
	uint8_t e_sector;
	uint8_t e_track;
	uint32_t s_lba;
	uint32_t sector_count;
};
struct GPTHeader {
	int64_t signature;
	int32_t revision;
	uint32_t header_size;
	uint32_t header_crc32;
	int32_t rsvd;
	uint64_t my_lba;
	uint64_t alternate_lba;
	uint64_t first_useable_lba;
	uint64_t last_useable_lba;
	GUID disk_guid;
	uint64_t partition_entry_lba;
	uint32_t entry_count;
	uint32_t entry_size;
	int32_t entry_array_crc32;
};
struct GPTEntry {
	GUID partition_type_guid;
	GUID partition_guid;
	uint64_t start_lba;
	uint64_t ending_lba;
	uint64_t attribute;
	wchar_t name[36];
};
typedef struct _PART_ {
	u8 monopolized;
	int num;
	GUID type;			//��������
	GUID flag;			//������ʶ
	uint64_t first;		//��һ������LBA����TypeΪMBR��չ����ʱ��Ҳ��ʾ��������������LBA
	uint64_t last;		//���һ������LBA
	struct _PART_* prev, * next;
	struct _DISK_* disk;
	wchar_t name[24];
	LPDOPSTREAM flist;
	LPDOPSTREAM fdisk;
	FCPEB fc;
} PART, * LPPART;
typedef struct _DISK_ {
	int vaild;
	int error;
	struct _PART_* part_list;
	struct _BUFFER_HEAD_* head1;
	struct _BUFFER_HEAD_* head2;
	LPDOPSTREAM disk;
	struct _DISK_* prev, * next;
	struct _DISK_INFO_ info;
} DISK,*LPDISK;

static LPDISK disk_list = NULL;
#define BAD_HEAD_CRC			0x00000001
#define BAD_ENTRY_CRC			0x00000002
#define ACCESS_FAIL				0x00000004

static struct _BUFFER_HEAD_ * part_bread(LPDOPSTREAM _part, u64 iblock) {
	LPPART part;

	part = _part->data;
	if (iblock >= part->last - part->first) return NULL;
	return bread(part->fdisk, iblock + part->first);
}
static struct _BUFFER_HEAD_* part_cache_bwrite(LPDOPSTREAM _part, u64 iblock) {
	LPPART part;

	part = _part->data;
	if (part->monopolized && !(_part->mode & FS_OPEN_MONOPOLIZE)) return NULL;
	if (iblock >= part->last - part->first) return NULL;
	return cache_bwrite(part->fdisk, iblock + part->first);
}
static LPDOPSTREAM part_open(wchar_t* name, u64 mode, LPFCPEB fc) {
	LPDOPSTREAM _part;
	LPPART part;

	part = fc->data;
	if (mode & FS_OPEN_MONOPOLIZE && !(mode & FS_OPEN_WRITE)) mode ^= FS_OPEN_MONOPOLIZE;
	if (part->monopolized && mode & FS_OPEN_MONOPOLIZE) return NULL;
	_part = malloc(sizeof(DOPSTREAM));
	if (!_part) return NULL;
	memset(_part, 0, sizeof(DOPSTREAM));
	if (mode & FS_OPEN_MONOPOLIZE) {
		part->monopolized = 1;
	}
	_part->mode = (int)mode;
	_part->fc = fc;
	_part->data = part;
	InsertList(part->flist, _part, d_prev, d_next);
	return _part;
}
static int part_close(LPDOPSTREAM _part) {
	LPPART part;

	part = _part->data;
	RemoveList(part->flist, _part, d_prev, d_next);
	free(_part);
	return 0;
}
static int part_info(LPDOPSTREAM _part, size_t size, void* __info) {
	LPPART part;
	struct _PART_INFO_* _info;

	if (size < sizeof(struct _PART_INFO_)) return -1;
	part = _part->data;

	memcpy(__info, &(part->disk->info), sizeof(struct _DISK_INFO_));
	_info = __info;
	_info->size = sizeof(struct _PART_INFO_);
	_info->logical_sector_count = part->last - part->first + 1;
	_info->path = part->name;
	_info->g_id = part->flag;
	_info->g_type = part->type;
	_info->g_disk = part->disk->info.g_id;
	_info->first_lba = part->first;
	return 0;
}
static FCPEB part_fc = {
	.bread = part_bread,
	.cache_bwrite = part_cache_bwrite,
	.open = part_open,
	.close = part_close,
	.info = part_info
};
static void call_back(LPDOPSTREAM _disk, LPDISK disk) {
	LPPART part,_part;
	LPDOPSTREAM file;

	part = disk->part_list;
	while (part) {
		while (part->flist) {
			if (part->flist->call_back) { 
				file = part->flist;
				part->flist->call_back(part->flist, part->flist->call_back_data); 
				if (file != part->flist) continue;
			}
			file = part->flist->d_next;
			free(part->flist);
			file->d_prev = NULL;
			part->flist = file;
		}
		_part = part->next;
		fs_unmap(part->name);
		free(part);
		part = _part;
	}
	bfree(disk->head2);
	buf_unlock(disk->head1, 0);
	bfree(disk->head1);
	close(disk->disk);
	free(disk);
}
static int compute_entry_crc32(LPDISK disk,u64 lba, unsigned int size,unsigned int * _crc) {
	struct _BUFFER_HEAD_* bh;
	unsigned int sec_per_block;
	unsigned int crc = 0;

	sec_per_block = disk->info.cache_block_size / disk->info.logical_sector_size;
	while (size) {
		bh = bread(disk->disk, lba);
		if (!bh) return -1;
		crc = ComputeCRC32(crc, bh->addr, disk->info.logical_sector_size);
		bfree(bh);
		lba++;
		size -= disk->info.logical_sector_size;
	}
	*_crc = crc;
	return 0;
}
static int clear_entry(LPDISK disk, u64 lba, unsigned int size) {
	struct _BUFFER_HEAD_* bh;
	unsigned int sec_per_block;

	sec_per_block = disk->info.cache_block_size / disk->info.logical_sector_size;
	if (lba & (sec_per_block - 1)) {
		bh = bread(disk->disk, lba);
		if (!bh) return -1;
		bdirty(bh);
		memset(bh->addr, 0, disk->info.cache_block_size - disk->info.logical_sector_size * (lba & (sec_per_block - 1)));
		bfree(bh);
		size -= disk->info.cache_block_size - disk->info.logical_sector_size * (lba & (sec_per_block - 1));
		lba += sec_per_block - 1;
		lba &= ~(sec_per_block - 1);
	}
	while (size >= disk->info.cache_block_size) {
		bh = cache_bwrite(disk->disk, lba);
		if (!bh) return -1;
		bdirty(bh);
		memset(bh->addr, 0, disk->info.cache_block_size);
		bfree(bh);
		lba += sec_per_block;
		size -= disk->info.cache_block_size;
	}
	if (size) {
		bh = bread(disk->disk, lba);
		if (!bh) return -1;
		bdirty(bh);
		memset(bh->addr, 0, size);
		bfree(bh);
	}
	return 0;
}
static int find_empty_entry(LPDISK disk, u64 lba, unsigned int size,unsigned int entry_size) {
	struct _BUFFER_HEAD_* bh;
	struct GPTEntry* entry;
	int ientry = 0;
	unsigned int _size;

	while (size) {
		bh = bread(disk->disk, lba);
		if (!bh) return -1;
		_size = disk->info.logical_sector_size;
		size -= _size;
		entry = bh->addr;
		while (_size) {
			if (IsNulGUID(&(entry->partition_guid))) {
				bfree(bh);
				return ientry;
			}
			entry = (struct GPTEntry*)(entry_size + (char*)entry);
			_size -= entry_size;
			ientry++;
		}
		bfree(bh);
		lba++;
	}
	return -1;
}
static int check_part_table(LPDISK disk, struct GPTHeader* head) {
	char sector[4096];
	unsigned int crc;
	int res = 0;

	memcpy(sector, head, head->header_size);
	head = (struct GPTHeader*)sector;
	crc = head->header_crc32;
	head->header_crc32 = 0;
	if (crc != ComputeCRC32(0, head, head->header_size)) res |= BAD_HEAD_CRC;
	head->header_crc32 = crc;
	if (compute_entry_crc32(disk, head->partition_entry_lba, head->entry_count * head->entry_size, &crc)) 
		res |= ACCESS_FAIL | BAD_ENTRY_CRC;
	else 
		if (crc != head->entry_array_crc32) res |= BAD_ENTRY_CRC;
	return res;
}
static int check_head_content(LPDISK disk, struct GPTHeader* head1, struct GPTHeader* head2) {
	if (head1->signature != 0x5452415020494645 || head1->revision != 0x00010000) return -1;
	if (head2->signature != 0x5452415020494645 || head2->revision != 0x00010000) return -1;
	if (head1->header_size < 92) return -1;
	if (head2->header_size < 92) return -1;
	if (head1->header_size > disk->info.logical_sector_size) return -1;
	if (head2->header_size > disk->info.logical_sector_size) return -1;
 	if (head1->entry_size != head2->entry_count) return -1;
	if (head1->entry_count != head2->entry_count) return -1;
	if (head1->my_lba != 1) return -1;
	if (head1->alternate_lba != disk->info.logical_sector_count - 1) return -1;
	if (head1->alternate_lba != head2->my_lba) return -1;
	if (head1->partition_entry_lba <= head1->my_lba) return -1;
	if (head2->partition_entry_lba >= head2->my_lba) return -1;
	if (memcmp(&head1->disk_guid, &head2->disk_guid, sizeof(GUID))) return -1;
	if (head1->first_useable_lba > head1->last_useable_lba) return -1;
	if (head2->first_useable_lba != head1->first_useable_lba) return -1;
	if (head2->last_useable_lba != head2->last_useable_lba) return -1;
	if (head1->first_useable_lba - head1->partition_entry_lba != head2->my_lba - head2->partition_entry_lba) return -1;
	return 0;
}
static int deep_check_part_table(LPDISK disk, struct GPTHeader* head1,struct GPTHeader * head2) {

	return -1;
}
HANDLE summon_part_table(const wchar_t* path) {
	LPDISK disk;
	LPPART part;
	unsigned int i; 
	struct _BUFFER_HEAD_* mbr,*bh;
	int head1_check, head2_check;
	int size, _size;
	uint64_t lba;
	struct GPTHeader* head;
	struct GPTEntry* entry;
	int esp_exist = 0;
	int lfs_exist = 0;
	int fat_exist = 0;
	int type;

	disk = malloc(sizeof(DISK));
	if (!disk) return NULL;
	memset(disk, 0, sizeof(DISK));
	disk->disk = open(path, FS_OPEN_READ | FS_OPEN_WRITE | FS_OPEN_N_CRATE, disk, call_back);
	if (!disk->disk) {
		free(disk);
		return NULL;
	}
	info(disk->disk, sizeof(struct _DISK_INFO_), &disk->info);
	mbr = bread(disk->disk, 0);
	if (((unsigned char*)(mbr->addr))[446LLU + offsetof(struct _MBRRECODE_, type)] != 0xee) {
		bfree(mbr);
		InsertList(disk_list, disk, prev, next);
		return disk;
	}
	bfree(mbr);
	disk->head1 = bread(disk->disk, 1);
	if (!disk->head1) {
		InsertList(disk_list, disk, prev, next);
		return disk;
	}
	disk->head2 = bread(disk->disk, disk->info.logical_sector_count - 1);
	if (!disk->head2) {
		InsertList(disk_list, disk, prev, next);
		bfree(disk->head1);
		return disk;
	}
	if (buf_lock(disk->head1, 0, disk->info.logical_sector_size)) {
		bfree(disk->head1);
		bfree(disk->head2);
		free(disk);
		return NULL;
	}
	InsertList(disk_list, disk, prev, next);
	if (check_head_content(disk, disk->head1->addr, disk->head2->addr)) {
		buf_unlock(disk->head1, 0);
		bfree(disk->head1);
		bfree(disk->head2);
		return disk;
	}
	head1_check = check_part_table(disk, disk->head1->addr);
	head2_check = check_part_table(disk, disk->head2->addr);
	if (head1_check & ACCESS_FAIL && head2_check & ACCESS_FAIL) {
		buf_unlock(disk->head1, 0);
		bfree(disk->head1);
		bfree(disk->head2);
		return disk;
	}
	if (head1_check || head2_check) {
		if (deep_check_part_table(disk, disk->head1->addr, disk->head2->addr)) {
			buf_unlock(disk->head1, 0);
			bfree(disk->head1);
			bfree(disk->head2);
			return disk;
		}
	}
	disk->vaild = 1;
	disk->error = head1_check | (head2_check << 4);

	head = disk->head1->addr;
	size = head->entry_size * head->entry_count;
	i = 0;
	lba = head->partition_entry_lba;
	while (size) {
		bh = bread(disk->disk, lba);
		_size = disk->info.logical_sector_size;
		size -= _size;
		entry = bh->addr;
		while (_size) {
			if (!IsNulGUID(&(entry->partition_guid))) {
				if (!memcmp(&entry->partition_type_guid, &ESP_GUID, sizeof(GUID))) {
					if (esp_exist) {
						_size -= head->entry_size;
						entry = (struct GPTEntry*)(head->entry_size + (char*)entry);
						i++;
						continue;
					}
					esp_exist = 1;
					type = 0;
				}
				else if (!memcmp(&entry->partition_type_guid, &LFS_GUID, sizeof(GUID))) {
					if (lfs_exist) {
						_size -= head->entry_size;
						entry = (struct GPTEntry*)(head->entry_size + (char*)entry);
						i++;
						continue;
					}
					lfs_exist = 1;
					type = 1;
				}
				else if(!memcmp(&entry->partition_type_guid, &FAT_GUID, sizeof(GUID)) ){
					if (fat_exist) {
						_size -= head->entry_size;
						entry = (struct GPTEntry*)(head->entry_size + (char*)entry);
						i++;
						continue;
					}
					fat_exist = 1;
					type = 2;
				}
				else {
					_size -= head->entry_size;
					entry = (struct GPTEntry*)(head->entry_size + (char*)entry);
					i++;
					continue;
				}
				part = malloc(sizeof(PART));
				if (!part) return NULL;
				memset(part, 0, sizeof(PART));
				part->first = entry->start_lba;
				part->last = entry->ending_lba;
				memcpy(&part->flag, &(entry->partition_guid), sizeof(GUID));
				memcpy(&part->type, &(entry->partition_type_guid), sizeof(GUID));
				part->disk = disk;
				part->fdisk = disk->disk;
				part->next = disk->part_list;
				if (part->next) part->next->prev = part;
				disk->part_list = part;
				memcpy(&part->fc, &part_fc, sizeof(FCPEB));
				part->fc.data = part;
				if (type == 0) {
					wcscpy(part->name, L"/.dev/esp.dev");
				}
				else if (type == 1) {
					wcscpy(part->name, L"/.dev/lfs.dev");
				}
				else {
					wcscpy(part->name, L"/.dev/fat.dev");
				}
				part->num = i;
				fs_map(part->name, &part->fc, NULL);
			}
			_size -= head->entry_size;
			entry = (struct GPTEntry*)(head->entry_size + (char*)entry);
			i++;
		}
		bfree(bh);
		lba++;
	}
	return disk;
}
int unsummon_part_table(HANDLE _disk) {
	LPDISK disk = _disk;
	LPPART part, _part;
	LPDOPSTREAM file;

	if (!_disk) return -1;
	part = disk->part_list;
	while (part) {
		while (part->flist) {
			file = part->flist;
			if (part->flist->call_back) {
				file->call_back(file, file->call_back_data);
				if (file != part->flist) continue;
			}
			file = part->flist->d_next;
			free(part->flist);
			file->d_prev = NULL;
			part->flist = file;
		}
		_part = part->next;
		fs_unmap(part->name);
		free(part);
		part = _part;
	}
	bfree(disk->head2);
	buf_unlock(disk->head1, 0);
	bfree(disk->head1);
	close(disk->disk);
	free(disk);
	return 0;
}
int create_part_table(HANDLE _disk) {
	LPDISK disk;
	LPPART part, _part, part2;
	LPDOPSTREAM file;
	struct _BUFFER_HEAD_* mbr;
	struct _BUFFER_HEAD_* entry1, * entry2;
	struct GPTHeader* head1, * head2;
	struct GPTEntry *entry;
	GUID gtmp,gtmp2,gtmp3;

	if (!_disk) return -1;
	disk = _disk;
	if (disk->vaild) {
		part = disk->part_list;
		while (part) {
			while (part->flist) {
				if (part->flist->call_back) {
					file = part->flist;
					part->flist->call_back(part->flist, part->flist->call_back_data);
					if (file != part->flist) continue;
				}
				file = part->flist->d_next;
				free(part->flist);
				file->d_prev = NULL;
				part->flist = file;
			}
			_part = part->next;
			fs_unmap(part->name);
			free(part);
			part = _part;
		}
	}
	else {
		disk->head1 = bread(disk->disk, 1);
		if (!disk->head1) {
			close(disk->disk);
			free(disk);
			return -1;
		}
		if (buf_lock(disk->head1, 0, disk->info.logical_sector_size)) {
			bfree(disk->head1);
			close(disk->disk);
			free(disk);
			return -1;
		}
		disk->head2 = bread(disk->disk, disk->info.logical_sector_count - 1);
		if (!disk->head2) {
			buf_unlock(disk->head1, 0);
			bfree(disk->head1);
			close(disk->disk);
			free(disk);
			return -1;
		}
	}
	head1 = disk->head1->addr;
	head2 = disk->head2->addr;
	if (CreateGuid(&gtmp) != S_OK) return -1;
	if (CreateGuid(&gtmp2) != S_OK) return -1;
	if (CreateGuid(&gtmp3) != S_OK) return -1;
	mbr = bread(disk->disk, 0);
	if (!mbr) return -1;
	entry1 = bread(disk->disk, 2);
	if (!entry1) {
		bfree(mbr);
		return -1;
	}
	entry2 = bread(disk->disk, disk->info.logical_sector_count - 1 - 128 * sizeof(struct GPTEntry) / disk->info.logical_sector_size);
	if (!entry2) {
		bfree(entry1);
		bfree(mbr);
		return -1;
	}
	if (clear_entry(disk, 2, 128 * sizeof(struct GPTEntry)) ||
		clear_entry(disk, disk->info.logical_sector_count - 1 - 128 * sizeof(struct GPTEntry) / disk->info.logical_sector_size, 128 * sizeof(struct GPTEntry))) {
		bfree(entry2);
		bfree(entry1);
		bfree(mbr);
		return -1;
	}
	bdirty(disk->head1);
	bdirty(disk->head2);
	head1->signature = head2->signature = 0x5452415020494645;
	head1->revision = head2->revision = 0x00010000;
	head1->header_size = head2->header_size = 92;
	head1->first_useable_lba = head2->first_useable_lba = 2 + 128 * sizeof(struct GPTEntry) / disk->info.logical_sector_size;
	head1->last_useable_lba = head2->last_useable_lba = disk->info.logical_sector_count - 2 - 128 * sizeof(struct GPTEntry) / disk->info.logical_sector_size;
	memcpy(&(head1->disk_guid), &(disk->info.g_id), sizeof(GUID));
	memcpy(&(head2->disk_guid), &(disk->info.g_id), sizeof(GUID));
	head1->entry_count = head2->entry_count = 128;
	head1->entry_size = head2->entry_size = sizeof(struct GPTEntry);
	head1->rsvd = head2->rsvd = 0;


	bdirty(mbr);
	((unsigned char *)mbr->addr)[446 + offsetof(struct _MBRRECODE_, s_sector)] = 0x02;
	((unsigned char *)mbr->addr)[446 + offsetof(struct _MBRRECODE_, s_lba)] = 0x01;
	((unsigned char *)mbr->addr)[446 + offsetof(struct _MBRRECODE_, type)] = 0xee;
	((unsigned char *)mbr->addr)[446 + offsetof(struct _MBRRECODE_, e_head)] = 0xff;
	((unsigned char *)mbr->addr)[446 + offsetof(struct _MBRRECODE_, e_sector)] = 0xff;
	((unsigned char *)mbr->addr)[446 + offsetof(struct _MBRRECODE_, e_track)] = 0xff;
	((unsigned char *)mbr->addr)[446 + offsetof(struct _MBRRECODE_, sector_count)] = 0xff;
	((unsigned char *)mbr->addr)[446 + offsetof(struct _MBRRECODE_, sector_count) + 1] = 0xff;
	((unsigned char *)mbr->addr)[446 + offsetof(struct _MBRRECODE_, sector_count) + 2] = 0xff;
	((unsigned char *)mbr->addr)[446 + offsetof(struct _MBRRECODE_, sector_count) + 3] = 0xff;
	((unsigned char *)mbr->addr)[510] = 0x55;
	((unsigned char *)mbr->addr)[511] = 0xaa;
	bfree(mbr);

	entry = entry1->addr;
	entry->start_lba = 2048;
	entry->ending_lba = 65535;
	entry->attribute = 0x8000000000000001;
	memcpy(&(entry->partition_type_guid), &ESP_GUID, sizeof(GUID));
	memcpy(&(entry->partition_guid), &gtmp, sizeof(GUID));
	memcpy(entry->name, L"EFI system partition", sizeof(L"EFI system partition"));
	entry[1].start_lba = 65536;
	entry[1].ending_lba = 131071;
	entry[1].attribute = 0x8000000000000001;
	memcpy(&(entry[1].partition_type_guid), &LFS_GUID, sizeof(GUID));
	memcpy(&(entry[1].partition_guid), &gtmp2, sizeof(GUID));
	memcpy(entry[1].name, L"Lava file system partition", sizeof(L"Lava file system partition"));
	entry[2].start_lba = 131072;
	entry[2].ending_lba = 196607;
	entry[2].attribute = 0x8000000000000001;
	memcpy(&(entry[2].partition_type_guid), &FAT_GUID, sizeof(GUID));
	memcpy(&(entry[2].partition_guid), &gtmp3, sizeof(GUID));
	memcpy(entry[2].name, L"Basic data partition", sizeof(L"Basic data partition"));
	head1->partition_entry_lba = 2;
	head1->my_lba = 1;
	head1->alternate_lba = disk->info.logical_sector_count - 1;
	compute_entry_crc32(disk, head1->partition_entry_lba, head1->entry_count * head1->entry_count, &head1->entry_array_crc32);
	head1->header_crc32 = 0;
	head1->header_crc32 = ComputeCRC32(0, head1, 92);
	bfree(entry1);

	bdirty(entry2);
	entry = entry2->addr;
	entry->start_lba = 2048;
	entry->ending_lba = 65535;
	entry->attribute = 0x8000000000000001;
	memcpy(&(entry->partition_type_guid), &ESP_GUID, sizeof(GUID));
	memcpy(&(entry->partition_guid), &gtmp, sizeof(GUID));
	memcpy(entry->name, L"EFI system partition", sizeof(L"EFI system partition"));
	entry[1].start_lba = 65536;
	entry[1].ending_lba = 131071;
	entry[1].attribute = 0x8000000000000001;
	memcpy(&(entry[1].partition_type_guid), &LFS_GUID, sizeof(GUID));
	memcpy(&(entry[1].partition_guid), &gtmp2, sizeof(GUID));
	memcpy(entry[1].name, L"Lava file system partition", sizeof(L"Lava file system partition"));
	entry[2].start_lba = 131072;
	entry[2].ending_lba = 196607;
	entry[2].attribute = 0x8000000000000001;
	memcpy(&(entry[2].partition_type_guid), &FAT_GUID, sizeof(GUID));
	memcpy(&(entry[2].partition_guid), &gtmp3, sizeof(GUID));
	memcpy(entry[2].name, L"Basic data partition", sizeof(L"Basic data partition"));
	head2->partition_entry_lba = head2->last_useable_lba + 1;
	head2->my_lba = disk->info.logical_sector_count - 1;
	head2->alternate_lba = 0;
	compute_entry_crc32(disk, head2->partition_entry_lba, head2->entry_count* head2->entry_count, &head2->entry_array_crc32);
	head2->header_crc32 = 0;
	head2->header_crc32 = ComputeCRC32(0, head2, 92);
	bfree(entry2);
	
	disk->vaild = 0;
	part = malloc(sizeof(struct _PART_));
	if (!part) return -1;
	_part = malloc(sizeof(struct _PART_));
	if (!_part) {
		free(part);
		return -1;
	}
	part2 = malloc(sizeof(struct _PART_));
	if (!part2) {
		free(part);
		free(_part);
		return -1;
	}
	disk->vaild = 1;

	memset(part, 0, sizeof(struct _PART_));
	part->disk = disk;
	part->fdisk = disk->disk;
	part->first = 2048;
	memcpy(&(part->flag), &gtmp, sizeof(GUID));
	part->last = 65535;
	part->num = 0;
	memcpy(&(part->type), &ESP_GUID, sizeof(GUID));
	wcscpy(part->name, L"/.dev/esp.dev");
	memcpy(&part->fc, &part_fc, sizeof(struct _FCPEB_));
	part->fc.data = part;

	disk->part_list = NULL;
	InsertList(disk->part_list, part, prev, next);
	fs_map(part->name, &part->fc, NULL);

	part = _part;
	memset(part, 0, sizeof(struct _PART_));
	part->disk = disk;
	part->fdisk = disk->disk;
	part->first = 65536;
	memcpy(&(part->flag), &gtmp2, sizeof(GUID));
	part->last = 131071;
	part->num = 1;
	memcpy(&(part->type), &LFS_GUID, sizeof(GUID));
	wcscpy(part->name, L"/.dev/lfs.dev");
	memcpy(&part->fc, &part_fc, sizeof(struct _FCPEB_));
	part->fc.data = part;
	InsertList(disk->part_list, part, prev, next);
	fs_map(part->name, &part->fc, NULL);

	part = part2;
	memset(part, 0, sizeof(struct _PART_));
	part->disk = disk;
	part->fdisk = disk->disk;
	part->first = 131072;
	memcpy(&(part->flag), &gtmp3, sizeof(GUID));
	part->last = 196607;
	part->num = 2;
	memcpy(&(part->type), &FAT_GUID, sizeof(GUID));
	wcscpy(part->name, L"/.dev/fat.dev");
	memcpy(&part->fc, &part_fc, sizeof(struct _FCPEB_));
	part->fc.data = part;
	InsertList(disk->part_list, part, prev, next);
	fs_map(part->name, &part->fc, NULL);

	return 0;
}
//Quasimodo