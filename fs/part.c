/*
	fs/storage.c
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
#include <fs.h>
#include <fctrl.h>
#include <spinlock.h>
#include <buffer.h>
#include <mm.h>
#include <string.h>
#include <lava.h>

struct _MBR_RECODE_ {
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
    u8 state;
    u8 s_type;
    uint32_t s_flag;
	int num;            //index in partition table
	GUID type;			
	GUID flag;			
	uint64_t first;		
	uint64_t last;		
	struct _PART_* prev, * next;
	struct _DISK_* disk;
	wchar_t name[24];
	LPSTREAM fdisk;
	LPSTREAM flist[MAX_OPEN_FILE];
	FCPEB fc;
} PART,*LPPART;
typedef struct _DISK_ {
    int status;
	int vaild;
	int error;
    uint32_t s_flag;
	struct _BUFFER_HEAD_* head1;
	struct _BUFFER_HEAD_* head2;
    LPSTREAM disk;
    GUID l_flag;
    struct _PART_* part_list;
    uint64_t sec_count;
    const wchar_t * path;
    struct _DISK_ * prev,*next;
	struct _DISK_INFO_ info;
} DISK,*LPDISK;

int esp_open(const wchar_t * path);
int fat_open(const wchar_t * path);
int lfs_open(const wchar_t * path);

static LPDISK disk_list = NULL;
static int part_mask[32] = {0,};
static int list_lock = 0;
static struct _FS_LIST_{
	GUID g_id;
	int (*open)(const wchar_t * path);
	const wchar_t * name;
} fs_list[] = {
	{{0xc12a7328, 0xf81f, 0x11d2, {0xba, 0x4b, 0x00, 0xa0, 0xc9, 0x3e, 0xc9, 0x3b }},esp_open,L"ESP"},
	{{0xebd0a0a2, 0xb9e5, 0x4433, {0x87, 0xc0, 0x68, 0xb6, 0xb7, 0x26, 0x99, 0xc7 }},fat_open,L"FAT"},
	{{0xa23a4c32, 0x5650, 0x47c6, {0xaf, 0x40, 0x5c, 0xd9, 0x48, 0x78, 0xc6, 0x71 }},lfs_open,L"LFS"}
};

spin_optr_struct_member_bit(Disk,struct _DISK_,status,0);
spin_optr_struct_member_bit(Part,struct _PART_,state,0);
spin_optr_struct_member_bit(PartList,struct _DISK_,status,1);
spin_optr_struct_member_bit(List,struct _PART_,state,1);

static inline int get_first_part_id(){
	int i;
	for(i = 0;i < sizeof(part_mask) * 8;i++)
		if(!spin_try_lock_bit(part_mask,i)) return i;
}
static inline void free_part_id(int id){
	if(id <= 0 || id >= sizeof(part_mask) * 8) return;
	spin_unlock_bit(part_mask,id);
}
static struct _BUFFER_HEAD_ * part_bread(LPSTREAM _part, u64 iblock) {
	LPPART part;

	part = _part->edata.pdata[0];
	if (iblock >= part->last - part->first) return NULL;
	return bread(part->fdisk, iblock + part->first);
}
static struct _BUFFER_HEAD_* part_cache_bwrite(LPSTREAM _part, u64 iblock) {
	LPPART part;

	part = _part->edata.pdata[0];
	if (part->monopolized && !(_part->mode & FS_OPEN_MONOPOLIZE)) return NULL;
	if (iblock >= part->last - part->first) return NULL;
	return cache_bwrite(part->fdisk, iblock + part->first);
}
static LPSTREAM part_open(wchar_t* name, u64 mode, LPFCPEB fc) {
	LPSTREAM _part;
	LPPART part;
    int i;

	part = fc->data;

	if (mode & FS_OPEN_MONOPOLIZE && !(mode & FS_OPEN_WRITE)) mode ^= FS_OPEN_MONOPOLIZE;
	if (part->monopolized && mode & FS_OPEN_MONOPOLIZE) return NULL;
    for(i = 0;i < MAX_OPEN_FILE;i++){
        if(!part->flist[i] && !cmpxchg8b(part->flist + i,NULL,1,NULL)){
	        _part = kmalloc(sizeof(STREAM),0);
	        if (!_part) return NULL;
	        memset(_part, 0, sizeof(STREAM));
	        if (mode & FS_OPEN_MONOPOLIZE) {
		        part->monopolized = 1;
	        }
	        _part->mode = (int)mode;
	        _part->fc = fc;
	        _part->edata.pdata[0] = part;
			part->flist[i] = _part;
			return _part;
        }
    }
	return NULL;
}
static int part_close(LPSTREAM _part) {
	LPPART part;
	int i;

	part = _part->edata.pdata[0];
	for(i = 0;i < MAX_OPEN_FILE;i++) {
		if(part->flist[i] == _part && !cmpxchg8b(part->flist + i,_part,NULL,NULL)) {
			kfree(_part);
			return 0;
		}
	}
	return -1;
}
static uint64_t part_get_count(LPSTREAM _part){
	LPPART part;

	part = _part->edata.pdata[0];
	return part->last - part->first + 1;
}
static int part_info(LPSTREAM _part,size_t size,void * __info){
	struct _PART_ * part;
	struct _PART_INFO_ * _info;
	struct _DISK_ * disk;

	part = _part->edata.pdata[0];
	disk = part->disk;
	if(size < sizeof(struct _PART_INFO_)) return -1;
	_info = __info;
	_info->size = sizeof(struct _PART_INFO_);
	_info->physical_sector_size = disk->info.physical_sector_size;
	_info->logical_sector_size = disk->info.logical_sector_size;
	_info->cache_block_size = disk->info.cache_block_size;
	_info->i_disk = disk->s_flag;
	_info->logical_sector_count = part->last - part->first + 1;
	_info->path = part->name;
	memcpy(&_info->g_id,&part->flag,sizeof(GUID));
	memcpy(&_info->g_type,&part->type,sizeof(GUID));
	memcpy(&_info->g_disk,&disk->l_flag,sizeof(GUID));
	_info->first_lba = part->first;
	return 0;
}
static FCPEB part_fc = {
	.bread = part_bread,
	.cache_bwrite = part_cache_bwrite,
	.open = part_open,
	.close = part_close,
    .get_count = part_get_count,
	.info = part_info
};
static int compute_entry_crc32(LPDISK disk,u64 lba, unsigned int size,unsigned int * _crc) {
	struct _BUFFER_HEAD_* bh;
	unsigned int crc = 0;

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
#define BAD_HEAD_CRC			0x00000001
#define BAD_ENTRY_CRC			0x00000002
#define ACCESS_FAIL				0x00000004

static int check_part_table(LPDISK disk, struct GPTHeader* head) {
	char sector[512];
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
static void call_back(LPSTREAM _disk, LPDISK disk) {
	LPPART part,_part;
	LPSTREAM file;
	int i;

	part = disk->part_list;
	while (part) {
		for(i = 0;i < MAX_OPEN_FILE;i++){
			if (part->flist[i] && part->flist[i]->call_back) { 
				file = part->flist[i];
				file->call_back(part->flist, file->call_back_data); 
			}
			else{
				file = part->flist[i];
				if(!cmpxchg8b(part->flist + i,file,NULL,NULL)) kfree(file);
			}
		}
		_part = part->next;
		fs_unmap(part->name);
		kfree(part);
		part = _part;
	}
	bfree(disk->head2);
	buf_unlock(disk->head1, 0);
	bfree(disk->head1);
	close(disk->disk);
	kfree(disk);
}
static int install_dfs_thread(HANDLE _part){
	LPPART part;
	int i;

	part = _part;
	for(i = 0;i < sizeof(fs_list)/sizeof(struct _FS_LIST_);i++){
		if(!memcmp(&part->type,&fs_list[i].g_id,sizeof(GUID))){
			if(fs_list[i].open) fs_list[i].open(part->name);
			return 0;
		}
	}
}
HANDLE summon_disk(const wchar_t * path){
	LPDISK disk;
	LPPART part;
	unsigned int i;
	struct _BUFFER_HEAD_ * mbr,*bh;
	int head1_check,head2_check;
	int size,_size;
	uint64_t lba;
	struct GPTHeader* head;
	struct GPTEntry* entry;
	int ie;

	disk = kmalloc(sizeof(DISK),0);
	if (!disk) return NULL;
	ie = IE();
	memset(disk, 0, sizeof(DISK));
	disk->disk = open(path, FS_OPEN_READ | FS_OPEN_WRITE | FS_OPEN_N_CRATE, disk, call_back);
	if (!disk->disk) {
		kfree(disk);
		return NULL;
	}
	info(disk->disk, sizeof(struct _DISK_INFO_), &disk->info);
	mbr = bread(disk->disk, 0);
	if (((unsigned char*)(mbr->addr))[446LLU + offsetof(struct _MBR_RECODE_, type)] != 0xee) {
		bfree(mbr);
		cli();
		spin_lock_bit(&list_lock,0);
		disk->next = disk_list;
		disk_list = disk;
		spin_unlock_bit(&list_lock,0);
		if(ie) sti();
		return disk;
	}
	disk->s_flag = *(uint32_t*)(440 + (char*)(mbr->addr));
	bfree(mbr);
	disk->head1 = bread(disk->disk, 1);
	if (!disk->head1) {
		cli();
		spin_lock_bit(&list_lock,0);
		disk->next = disk_list;
		disk_list = disk;
		spin_unlock_bit(&list_lock,0);
		if(ie) sti();
		return disk;
	}
	disk->head2 = bread(disk->disk, disk->info.logical_sector_count - 1);
	if (!disk->head2) {
		cli();
		spin_lock_bit(&list_lock,0);
		disk->next = disk_list;
		disk_list = disk;
		spin_unlock_bit(&list_lock,0);
		if(ie) sti();
		bfree(disk->head1);
		return disk;
	}
	if (buf_lock(disk->head1, 0, disk->info.logical_sector_size)) {
		bfree(disk->head1);
		bfree(disk->head2);
		kfree(disk);
		return NULL;
	}
	cli();
	spin_lock_bit(&list_lock,0);
	disk->next = disk_list;
	disk_list = disk;
	spin_unlock_bit(&list_lock,0);
	if(ie) sti();
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
	memcpy(&disk->l_flag,&head->disk_guid,sizeof(GUID));
	i = 0;
	lba = head->partition_entry_lba;
	while (size) {
		bh = bread(disk->disk, lba);
		_size = disk->info.logical_sector_size;
		size -= _size;
		entry = bh->addr;
		while (_size) {
			if (!IsNulGUID(&(entry->partition_guid))) {
				part = kmalloc(sizeof(PART),0);
				if (!part) return disk;
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
				part->num = i;
				wsprintf(part->name,24,L"/.dev/%d.prt",get_first_part_id());
				fs_map(part->name, &part->fc, NULL);
				create_thread(NULL,install_dfs_thread,part);
				//summon_part
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
int unsummon_disk(HANDLE _disk){
	LPDISK disk = _disk;
	LPPART part, _part;
	LPSTREAM file;
	int i;

	if (!_disk) return -1;
	part = disk->part_list;
	while (part) {
		for(i = 0;i < MAX_OPEN_FILE;i++){
			if (part->flist[i] && part->flist[i]->call_back) { 
				file = part->flist[i];
				file->call_back(part->flist, file->call_back_data); 
			}
			else close(part->flist[i]);
		}
		_part = part->next;
		fs_unmap(part->name);
		kfree(part);
		part = _part;
	}
	bfree(disk->head2);
	buf_unlock(disk->head1, 0);
	bfree(disk->head1);
	close(disk->disk);
	kfree(disk);
}
void close_all_disk(){
	struct _DISK_ * disk,*_disk;
	struct _PART_ * part,*_part;
	int i;
	LPSTREAM file;

	for(disk = disk_list;disk;disk = disk->next){
		unsummon_disk(disk);
	}
}