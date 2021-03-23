/*
	fs/fat.c
	Copyright (C) 2020  Quasimodo

    This program is kfree software: you can redistribute it and/or modify
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
#include <mm.h>
#include <buffer.h>
#include <time.h>


#define BS_OEM_NAME				3//8
#define BPB_BYTES_PER_SECTOR	11//2
#define BPB_SECTORS_PER_CLUSTER	13//1
#define BPB_RESERVED_SECTORS	14//2
#define BPB_FATS_NUMBER			16//1
#define BPB_ROOT_ENT_CNT		17//2
#define BPB_SECTORS_COUNT_16	19//2
#define BPB_MEDIA				21//1
#define BPB_FAT_SIZE_16			22//2
#define BPB_SECTORS_PER_TRACK	24//2
#define BPB_HEAD_COUNT			26//2
#define BPB_HIDE_SECTORS		28//4
#define BPB_SECTORS_COUNT_32	32//4

#define BS_DRIVER_NUMBER_16		36//1
#define BS_BOOT_SIGN_16			38//1
#define BS_VOLUME_ID_16			39//4
#define BS_VOLUME_LABEL_16		43//11
#define BS_FILE_SYSTEM_TYPE_16	54//8

#define BPB_FAT_SIZE_32			36//4
#define BPB_FS_VERSION_32		42//2
#define BPB_ROOT_CLUSTER_32		44//4
#define BPB_FS_INFO_32			48//2
#define BPB_BACK_BOOT_SECTOR	50//2
#define BS_DRIVER_NUMBER_32		64//1
#define BS_BOOT_SIGN_32			66//1
#define BS_VOLUME_ID_32			67//4
#define BS_VOLUME_LABEL_32		71//11
#define BS_FILE_SYSTEM_TYPE_32	82//8

#define FAT_DIR_ATTR_READ_ONLY	0x01
#define FAT_DIR_ATTR_HIDDEN		0x02
#define FAT_DIR_ATTR_SYSTEM		0x04
#define FAT_DIR_ATTR_VPLUME_ID	0x08
#define FAT_DIR_ATTR_DIRECTORY	0x10
#define FAT_DIR_ARCHIVE			0x20
#define FAT_DIR_ATTR_LONG_NAME	0x0f
#define FAT_DIR_ATTR_LONG_NAME_MASK	0x3f

#define LONG_NAME_ORDER			0//0
#define LONG_NAME_0				1//2
#define LONG_NAME_1				3//2
#define LONG_NAME_2				5//2
#define LONG_NAME_3				7//2
#define LONG_NAME_4				9//2
#define LONG_NAME_5				14//2
#define LONG_NAME_6				16//2
#define LONG_NAME_7				18//2
#define LONG_NAME_8				20//2
#define LONG_NAME_9				22//2
#define LONG_NAME_10			24//2
#define LONG_NAME_11			28//2
#define LONG_NAME_12			30//2
#define LONG_NAME_ATTR			11//1
#define LONG_NAME_CHECKSUM		13//1

#define FAT_BPB_FAT_SIZE_32		36
#define FAT_BPB_SECTORS_COUNT	32//
#define FAT_BPB_FIRST_LBA		28//

#define BOOT_FILE_SEG_OFF		492
#define BOOT_FILE_NAME_OFF		504
#define ORG_DATA_OFF			508

#define FAT_FSI_FREE_COUNT		488
#define FAT_FSI_NEXT_FREE		492

#define FAT_TYPE_12		1
#define FAT_TYPE_16		2
#define FAT_TYPE_32		3

#define FILE_EDATA_NODE		0
#define BUF_EDATA_ATTR		0
#define FILE_TABLE_INDEX	1

#define MONOPOLIZE_BIT		0
#define MONOPOLIZE			(1 << MONOPOLIZE_BIT)

struct FAT_DIR {
	union {
		struct {
			char short_name[11];
			char attr;
			char nt_rev;
			char create_time_tenth;
			short create_time;
			short create_date;
			short access_date;
			unsigned short cluster_hi;
			short modify_time;
			short modify_date;
			unsigned short cluster_low;
			unsigned int size;
		};
		unsigned char long_name[32];
	};
};
struct _FAT_INFO_ {
	uint32_t lead_sign;
	uint32_t rvd[480 / sizeof(uint32_t)];
	uint32_t struct_sign;
	uint32_t free_count;
	uint32_t next_free;
	uint32_t rvd2[3];
	uint32_t tail_sign;
};
struct _FAT_ {
	uint64_t cluster_lba;
	uint64_t fat_lba;
	uint64_t cluster_size;
	uint64_t fat_size;
	uint64_t root;
	uint64_t sector_count;
	uint64_t alter_lba;
	uint32_t root_ent_cnt;
	uint32_t fat_count;
	u8 fs_type;//FAT12,FAT16,FAT32
	u8 vaild;
	uint32_t cluster_count;
	uint32_t free_cluster_count;
	struct _BUFFER_HEAD_* boot;
	union{
		struct _BUFFER_HEAD_* fs_info;
		struct{
			uint32_t first_free_cluster;
			int comm_high;//if comm_high is zero,mean no fs_info exist
		};
	};
    wchar_t * name;
	wchar_t _name[24];
	LPSTREAM part;
	LPSTREAM * file_table;
    int (*read_root)(struct _FAT_* fat, uint32_t off,void * buf,uint32_t count);
    int (*write_root)(struct _FAT_* fat, uint32_t off, void* buf, uint32_t count);
	struct _BUFFER_HEAD_ * (*rootoff2bh)(struct _FAT_* fat, uint32_t off);
	struct _BUFFER_HEAD_ * (*syn_rootoff2bh)(struct _FAT_* fat, uint32_t off);
	uint32_t (*next_cluster)(struct _FAT_* fat,uint32_t cluster);
	uint32_t (*find_next_free_cluster)(struct _FAT_ * fat,uint32_t old);
	int (*list_cluster)(struct _FAT_ * fat, uint32_t cluster, uint32_t to);
	//struct _FAT_* prev, * next;
	struct _SIMPLE_PATH_ my_path;
	struct _SIMPLE_PATH_ f_path;
	FCPEB fc;
	FSCTRL fs;
	struct _PART_INFO_ info;
};

static uint32_t fat_alloc_cluster(struct _FAT_ * fat);
static void fat_clean_cluster(struct _FAT_* fat, uint32_t cluster);

static int fat_read_root_12_16(struct _FAT_* fat, uint32_t off,void * buf,uint32_t count){
    struct _BUFFER_HEAD_* bh;
	uint32_t sec_off;
	uint32_t byte_off;
	uint32_t clus_off;
	uint64_t lba;
	uint32_t sec_per_clus;
	uint32_t rcluster;

    if (!fat->root_ent_cnt) return -1;
	if (fat->root_ent_cnt * 32 == off) {
		memset(buf, 0, count);
		return 0;
	}
	if (fat->root_ent_cnt * 32 <= off + count) return -1;
	byte_off = off & 0x1ff;
	sec_off = off >> 9;
	bh = bread(fat->part, fat->root + sec_off);
	if (byte_off + count <= fat->info.logical_sector_size) {
		memcpy(buf, byte_off + (char*)(bh->addr), count);
		bfree(bh);
		return 0;
	}
	if (byte_off) {
		memcpy(buf, byte_off + (char*)(bh->addr), fat->info.logical_sector_size - byte_off);
		count -= fat->info.logical_sector_size - byte_off;
		buf = fat->info.logical_sector_size - byte_off + (char*)buf;
		sec_off++;
		bfree(bh);
		bread(fat->part, fat->root + sec_off);
	}
	while (count > fat->info.logical_sector_size) {
		memcpy(buf, bh->addr, fat->info.logical_sector_size);
		count -= fat->info.logical_sector_size;
		buf = fat->info.logical_sector_size + (char*)buf;
		sec_off++;
		bfree(bh);
		bread(fat->part, fat->root + sec_off);
	}
	if (count) {
		memcpy(buf, bh->addr, count);
	}
	bfree(bh);
	return 0;
}
static int fat_read_root_32(struct _FAT_* fat, uint32_t off,void * buf,uint32_t count) {
	struct _BUFFER_HEAD_* bh;
	uint32_t sec_off;
	uint32_t byte_off;
	uint32_t clus_off;
	uint64_t lba;
	uint32_t sec_per_clus;
	uint32_t rcluster;

	if (!fat->root) return -1;
	clus_off = off / fat->cluster_size;
	off %= fat->cluster_size;
	sec_off = off / fat->info.logical_sector_size;
	byte_off = off % fat->info.logical_sector_size;
	rcluster = fat->root;
	while (clus_off) {
		clus_off--;
		rcluster = fat->next_cluster(fat, rcluster);
		if (!rcluster || rcluster >= 0xfffffff0) {
			if (!clus_off) {
				memset(buf, 0, count);
				return 0;
			}
			return -1;
		}
	}
	lba = rcluster * fat->cluster_size / fat->info.logical_sector_size + fat->cluster_lba + sec_off;
	bh = bread(fat->part, lba);
	if (!bh) return -1;
	if (byte_off) {
		if (count <= fat->info.logical_sector_size - byte_off) {
			memcpy(buf, byte_off + (char*)(bh->addr), count);
			bfree(bh);
			return 0;
		}
		memcpy(buf, byte_off + (char*)(bh->addr), fat->info.logical_sector_size - byte_off);
		bfree(bh);
		sec_off++;
		byte_off = 0;
		count -= fat->info.logical_sector_size - byte_off;
		buf = (fat->info.logical_sector_size - byte_off) + (char*)buf;
		lba++;
		if (sec_off * fat->info.logical_sector_size >= fat->cluster_size) {
			sec_off = 0;
			rcluster = fat->next_cluster(fat, rcluster);
			if (!rcluster || rcluster >= 0xfffffff0) {
				memset(buf, 0, count);
				return 0;
			}
			lba = rcluster * fat->cluster_size / fat->info.logical_sector_size + fat->cluster_lba + sec_off;
		}
		bh = bread(fat->part, lba);
		if (!bh) return -1;
	}
	if (sec_off * fat->info.logical_sector_size < fat->cluster_size) {
		while (1) {
			if (count <= fat->info.logical_sector_size) {
				memcpy(buf, bh->addr, count);
				bfree(bh);
				return 0;
			}
			lba++;
			sec_off++;
			memcpy(buf, bh->addr, fat->info.logical_sector_size);
			count -= fat->info.logical_sector_size;
			buf = fat->info.logical_sector_size + (char*)buf;
			bfree(bh);
			if (sec_off * fat->info.logical_sector_size >= fat->cluster_size) break;
			bh = bread(fat->part, lba);
			if (!bh) return -1;
		}
		sec_off = 0;
		rcluster = fat->next_cluster(fat, rcluster);
		if (!rcluster || rcluster >= 0xfffffff0) {
			memset(buf, 0, count);
			return 0;
		}
		lba = rcluster * fat->cluster_size / fat->info.logical_sector_size + fat->cluster_lba + sec_off;
		bh = bread(fat->part, lba);
		if (!bh) return -1;
	}
	while (count >= fat->cluster_size) {
		sec_off = fat->cluster_size;
		while (1) {
			lba++;
			sec_off -= fat->info.logical_sector_size;
			memcpy(buf, bh->addr, fat->info.logical_sector_size);
			count -= fat->info.logical_sector_size;
			buf = fat->info.logical_sector_size + (char*)buf;
			bfree(bh);
			if (!sec_off) break;
			bh = bread(fat->part, lba);
			if (!bh) return -1;
		}
		if (!count) return 0;
		rcluster = fat->next_cluster(fat, rcluster);
		if (!rcluster || rcluster >= 0xfffffff0) {
			memset(buf, 0, count);
			return 0;
		}
		lba = rcluster * fat->cluster_size / fat->info.logical_sector_size + fat->cluster_lba + sec_off;
		bh = bread(fat->part, lba);
		if (!bh) return -1;
	}
	while (1) {
		lba++;
		memcpy(buf, bh->addr, fat->info.logical_sector_size);
		count -= fat->info.logical_sector_size;
		buf = fat->info.logical_sector_size + (char*)buf;
		bfree(bh);
		if (count < fat->info.logical_sector_size) break;
		bh = bread(fat->part, lba);
		if (!bh) return -1;
	}
	if (!count) return 0;
	rcluster = fat->next_cluster(fat, rcluster);
	if (!rcluster || rcluster >= 0xfffffff0) {
		memset(buf, 0, count);
		return 0;
	}
	memcpy(buf, bh->addr, count);
	bfree(bh);
	return 0;
}
static int fat_write_root_12_16(struct _FAT_* fat, uint32_t off, void* buf, uint32_t count){
    struct _BUFFER_HEAD_* bh;
	uint32_t sec_off;
	uint32_t byte_off;
	uint32_t rcluster,_rcluster;
	uint32_t clus_off;
	uint64_t lba;

    if (!fat->root_ent_cnt) return -1;
	if (fat->root_ent_cnt * 32 <= off + count) return -1;
	byte_off = off & 0x1ff;
	sec_off = off >> 9;
	bh = bread(fat->part, fat->root + sec_off);
	if (byte_off + count <= fat->info.logical_sector_size) {
		bdirty(bh);
		memcpy(byte_off + (char*)(bh->addr), buf, count);
		bfree(bh);
		return 0;
	}
	if (byte_off) {
		bdirty(bh);
		memcpy(byte_off + (char*)(bh->addr), buf, fat->info.logical_sector_size - byte_off);
		count -= fat->info.logical_sector_size - byte_off;
		buf = fat->info.logical_sector_size - byte_off + (char*)buf;
		sec_off++;
		bfree(bh);
		bread(fat->part, fat->root + sec_off);
	}
	while (count > fat->info.logical_sector_size) {
		bdirty(bh);
		memcpy(bh->addr, buf, fat->info.logical_sector_size);
		count -= fat->info.logical_sector_size;
		buf = fat->info.logical_sector_size + (char*)buf;
		sec_off++;
		bfree(bh);
		bread(fat->part, fat->root + sec_off);
	}
	if (count) {
		bdirty(bh);
		memcpy(buf, bh->addr, fat->info.logical_sector_size);
	}
	bfree(bh);
	return 0;
}
static int fat_write_root_32(struct _FAT_* fat, uint32_t off, void* buf, uint32_t count) {
	struct _BUFFER_HEAD_* bh;
	uint32_t sec_off;
	uint32_t byte_off;
	uint32_t rcluster,_rcluster;
	uint32_t clus_off;
	uint64_t lba;

	if (!fat->root) return -1;
	clus_off = off / fat->cluster_size;
	off %= fat->cluster_size;
	sec_off = off / fat->info.logical_sector_size;
	byte_off = off % fat->info.logical_sector_size;
	rcluster = fat->root;
	while (clus_off) {
		clus_off--;
		_rcluster = fat->next_cluster(fat, rcluster);
		if (!_rcluster || _rcluster >= 0xfffffff0) {
			_rcluster = fat_alloc_cluster(fat);
			if (!_rcluster) return -1;
			fat_clean_cluster(fat, _rcluster);
			fat->list_cluster(fat, rcluster, _rcluster);
			fat->list_cluster(fat, _rcluster, 0xffffffff);
		}
		rcluster = _rcluster;
	}
	lba = rcluster * fat->cluster_size / fat->info.logical_sector_size + fat->cluster_lba + sec_off;
	bh = bread(fat->part, lba);
	if (!bh) return -1;
	if (byte_off) {
		if (count <= fat->info.logical_sector_size - byte_off) {
			bdirty(bh);
			memcpy(byte_off + (char*)(bh->addr), buf, count);
			bfree(bh);
			return 0;
		}
		bdirty(bh);
		memcpy(byte_off + (char*)(bh->addr), buf, fat->info.logical_sector_size - byte_off);
		bfree(bh);
		sec_off++;
		byte_off = 0;
		count -= fat->info.logical_sector_size - byte_off;
		buf = (fat->info.logical_sector_size - byte_off) + (char*)buf;
		lba++;
		if (sec_off * fat->info.logical_sector_size >= fat->cluster_size) {
			sec_off = 0;
			_rcluster = fat->next_cluster(fat, rcluster);
			if (!_rcluster || _rcluster >= 0xfffffff0) {
				_rcluster = fat_alloc_cluster(fat);
				if (!_rcluster) return -1;
				fat_clean_cluster(fat, _rcluster);
				fat->list_cluster(fat, rcluster, _rcluster);
				fat->list_cluster(fat, _rcluster, 0xffffffff);
			}
			rcluster = _rcluster;
			lba = rcluster * fat->cluster_size / fat->info.logical_sector_size + fat->cluster_lba + sec_off;
		}
		bh = bread(fat->part, lba);
		if (!bh) return -1;
	}
	if (sec_off * fat->info.logical_sector_size < fat->cluster_size) {
		while (1) {
			lba++;
			sec_off++;
			bdirty(bh);
			memcpy(bh->addr, buf, fat->info.logical_sector_size);
			count -= fat->info.logical_sector_size;
			buf = fat->info.logical_sector_size + (char*)buf;
			bfree(bh);
			if (sec_off * fat->info.logical_sector_size >= fat->cluster_size) break;
			bh = bread(fat->part, lba);
			if (!bh) return -1;
		}
		sec_off = 0;
		_rcluster = fat->next_cluster(fat, rcluster);
		if (!_rcluster || _rcluster >= 0xfffffff0) {
			_rcluster = fat_alloc_cluster(fat);
			if (!_rcluster) return -1;
			fat_clean_cluster(fat, _rcluster);
			fat->list_cluster(fat, rcluster, _rcluster);
			fat->list_cluster(fat, _rcluster, 0xffffffff);
		}
		rcluster = _rcluster;
		lba = rcluster * fat->cluster_size / fat->info.logical_sector_size + fat->cluster_lba + sec_off;
		bh = bread(fat->part, lba);
		if (!bh) return -1;
	}
	while (count >= fat->cluster_size) {
		sec_off = fat->cluster_size;
		while (1) {
			lba++;
			sec_off -= fat->info.logical_sector_size;
			bdirty(bh);
			memcpy(bh->addr, buf, fat->info.logical_sector_size);
			count -= fat->info.logical_sector_size;
			buf = fat->info.logical_sector_size + (char*)buf;
			bfree(bh);
			if (!sec_off) break;
			bh = bread(fat->part, lba);
			if (!bh) return -1;
		}
		if (!count) return 0;
		rcluster = fat->next_cluster(fat, rcluster);
		if (!rcluster || rcluster >= 0xfffffff0) {
			_rcluster = fat_alloc_cluster(fat);
			if (!_rcluster) return -1;
			fat_clean_cluster(fat, _rcluster);
			fat->list_cluster(fat, rcluster, _rcluster);
			fat->list_cluster(fat, _rcluster, 0xffffffff);
		}
		rcluster = _rcluster;
		lba = rcluster * fat->cluster_size / fat->info.logical_sector_size + fat->cluster_lba + sec_off;
		bh = bread(fat->part, lba);
		if (!bh) return -1;
	}
	while (1) {
		lba++;
		bdirty(bh);
		memcpy(bh->addr, buf, fat->info.logical_sector_size);
		count -= fat->info.logical_sector_size;
		buf = fat->info.logical_sector_size + (char*)buf;
		bfree(bh);
		if (count < fat->info.logical_sector_size) break;
		if (!count) return 0;
		bh = bread(fat->part, lba);
		if (!bh) return -1;
	}
	bdirty(bh);
	memcpy(bh->addr, buf, count);
	bfree(bh);
	return 0;
}
static struct _BUFFER_HEAD_ * fat_rootoff2bh_32(struct _FAT_* fat, uint32_t off){
	struct _BUFFER_HEAD_* bh,*res;
	uint32_t sec_off;
	uint32_t byte_off;
	uint32_t rcluster, _rcluster;
	uint32_t clus_off;
	uint64_t lba;

	if (!fat->root) return NULL;
	clus_off = off / fat->cluster_size;
	off %= fat->cluster_size;
	sec_off = off / fat->info.logical_sector_size;
	byte_off = off % fat->info.logical_sector_size;
	rcluster = fat->root;
	while (clus_off) {
		clus_off--;
		_rcluster = fat->next_cluster(fat, rcluster);
		if (!_rcluster || _rcluster >= 0xfffffff0) {
			_rcluster = fat_alloc_cluster(fat);
			if (!_rcluster) return NULL;
			fat_clean_cluster(fat, _rcluster);
			fat->list_cluster(fat, rcluster, _rcluster);
			fat->list_cluster(fat, _rcluster, 0xffffffff);
		}
		rcluster = _rcluster;
	}
	lba = rcluster * fat->cluster_size / fat->info.logical_sector_size + fat->cluster_lba + sec_off;
	bh = bread(fat->part, lba);
	if (!bh) return NULL;
	res = hl_blink(bh, byte_off, 32);
	bfree(bh);
	return res;
}
static struct _BUFFER_HEAD_* fat_rootoff2bh_12_16(struct _FAT_* fat, uint32_t off) {
	struct _BUFFER_HEAD_* bh,*res;
	uint32_t sec_off;
	uint32_t byte_off;

	if (!fat->root_ent_cnt) return NULL;
	if (fat->root_ent_cnt * 32 <= off) return NULL;
	byte_off = off & 0x1ff;
	sec_off = off >> 9;
	bh = bread(fat->part, fat->root + sec_off);
	if (!bh) return NULL;
	res = hl_blink(bh, byte_off, 32);
	bfree(bh);
	return res;
}
static struct _BUFFER_HEAD_ * fat_syn_rootoff2bh_32(struct _FAT_ * fat,uint32_t off){
	struct _BUFFER_HEAD_* bh,*res;
	uint32_t sec_off;
	uint32_t byte_off;
	uint32_t rcluster, _rcluster;
	uint32_t clus_off;
	uint64_t lba;

	if (!fat->root) return NULL;
	clus_off = off / fat->cluster_size;
	off %= fat->cluster_size;
	sec_off = off / fat->info.logical_sector_size;
	byte_off = off % fat->info.logical_sector_size;
	rcluster = fat->root;
	while (clus_off) {
		clus_off--;
		_rcluster = fat->next_cluster(fat, rcluster);
		if (!_rcluster || _rcluster >= 0xfffffff0) {
			_rcluster = fat_alloc_cluster(fat);
			if (!_rcluster) return NULL;
			fat_clean_cluster(fat, _rcluster);
			fat->list_cluster(fat, rcluster, _rcluster);
			fat->list_cluster(fat, _rcluster, 0xffffffff);
		}
		rcluster = _rcluster;
	}
	lba = rcluster * fat->cluster_size / fat->info.logical_sector_size + fat->cluster_lba + sec_off;
	bh = bread(fat->part, lba);
	if (!bh) return NULL;
	res = syn_hl_blink(bh, byte_off, 32);
	bfree(bh);
	return res;
}
static struct _BUFFER_HEAD_ * fat_syn_rootoff2bh_12_16(struct _FAT_ * fat,uint32_t off){
	struct _BUFFER_HEAD_* bh,*res;
	uint32_t sec_off;
	uint32_t byte_off;

	if (!fat->root_ent_cnt) return NULL;
	if (fat->root_ent_cnt * 32 <= off) return NULL;
	byte_off = off & 0x1ff;
	sec_off = off >> 9;
	bh = bread(fat->part, fat->root + sec_off);
	if (!bh) return NULL;
	res = syn_hl_blink(bh, byte_off, 32);
	bfree(bh);
	return res;
}
static uint32_t fat_next_cluster_12(struct _FAT_* fat,uint32_t cluster){
	struct _BUFFER_HEAD_* bh;
	unsigned int _cluster;
	unsigned int off;
	unsigned int lba;

	if (cluster <= 1) return 0;
	off = cluster + (cluster >> 1);
	lba = off / fat->info.logical_sector_size;
	off &= fat->info.logical_sector_size - 1;
	bh = bread(fat->part, fat->fat_lba + lba);
	if (!bh) return 0;
	_cluster = ((unsigned char*)(bh->addr))[off];
	if (off == fat->info.logical_sector_size - 1) {
		bfree(bh);
		bh = bread(fat->part, fat->fat_lba + lba + 1);
		if (!bh) return 0;
		_cluster |= ((unsigned char*)(bh->addr))[0] << 8;
	}
	else _cluster |= ((unsigned char*)(bh->addr))[off + 1] << 8;
	if (cluster & 0x01) _cluster >>= 4;
	bfree(bh);
	_cluster &= 0x0fff;
	if (_cluster >= 0x0ff0)
		return _cluster | 0xfffffff0;
	else return _cluster;
}
static uint32_t fat_next_cluster_16(struct _FAT_* fat,uint32_t cluster){
	struct _BUFFER_HEAD_* bh;
	unsigned int _cluster;
	unsigned int off;
	unsigned int lba;

	if (cluster <= 1) return 0;
	off = cluster << 1;
	lba = off / fat->info.logical_sector_size;
	off &= fat->info.logical_sector_size - 1;
	bh = bread(fat->part, fat->fat_lba + lba);
	if (!bh) return 0;
	_cluster = ((unsigned short*)(bh->addr))[off >> 1];
	bfree(bh);
	if (_cluster >= 0x0fff0)
		return _cluster | 0xfffffff0;
	else return _cluster;
}
static uint32_t fat_next_cluster_32(struct _FAT_* fat,uint32_t cluster) {
	struct _BUFFER_HEAD_* bh;
	unsigned int _cluster;
	unsigned int off;
	unsigned int lba;

	if (cluster <= 1) return 0;
	off = cluster << 2;
	lba = off / fat->info.logical_sector_size;
	off &= fat->info.logical_sector_size - 1;
	bh = bread(fat->part, fat->fat_lba + lba);
	if (!bh) return 0;
	_cluster = ((unsigned int*)(bh->addr))[off >> 2];
	bfree(bh);
	if (_cluster >= 0x0ffffff0)
		return _cluster | 0xfffffff0;
	else return _cluster;
}
static uint32_t fat_find_next_free_cluster_32_info(struct _FAT_* fat,uint32_t old){
	struct _FAT_INFO_ * fat32_info;
	struct _BUFFER_HEAD_ * bh;
	uint32_t * fat32;
	uint64_t lba;
	uint32_t index;
	uint32_t index_max;

	fat32_info = fat->fs_info->addr;
	if(!buf_islock(fat->fs_info,0)){
		printk("BUG:fat_find_next_free_cluster_32_info(),must lock info before call.\n");
		stop();
	}
	if(fat32_info->next_free == 0xffffffff) {
		if(!fat32_info->free_count) return 0;
		old = 2;
	}
	else old = fat32_info->next_free;
	index_max = fat->info.logical_sector_size/4;
	index = old & (index_max - 1);
	lba = old / index_max;
	lba += fat->fat_lba;
	while(old < fat->cluster_count){
		bh = bread(fat->part,lba);
		fat32 = bh->addr;
		while(index < index_max && old < fat->cluster_count){
			if(!fat32[index]){
				bfree(bh);
				bdirty(fat->fs_info);
				fat32_info->next_free = old;
				return old;
			}
			old++;
		}
		bfree(bh);
		if(old >= fat->cluster_count){
			bdirty(fat->fs_info);
			fat32_info->next_free = 0xffffffff;
			fat32_info->free_count = 0;
			return 0;
		}
		lba++;
	}
	bdirty(fat->fs_info);
	fat32_info->next_free = 0xffffffff;
	fat32_info->free_count = 0;
	return 0;
}
static uint32_t fat_find_next_free_cluster_32(struct _FAT_ * fat,uint32_t old){
	struct _BUFFER_HEAD_ * bh;
	uint32_t * fat32;
	uint64_t lba;
	uint32_t index;
	uint32_t index_max;

	if(!fat->free_cluster_count) return 0;
	if(!old || old == 0xffffffff) old = 2;
	index_max = fat->info.logical_sector_size/4;
	index = old & (index_max - 1);
	lba = old / index_max;
	lba += fat->fat_lba;
	while(old < fat->cluster_count){
		bh = bread(fat->part,lba);
		fat32 = bh->addr;
		while(index < index_max && old < fat->cluster_count){
			if(!fat32[index]){
				bfree(bh);
				return old;
			}
			old++;
		}
		bfree(bh);
		if(old >= fat->cluster_count){
			fat->free_cluster_count = 0;
			return 0;
		}
		lba++;
	}
	fat->free_cluster_count = 0;
	return 0;
}
static uint32_t fat_find_next_free_cluster_16(struct _FAT_ * fat,uint32_t old){
	struct _BUFFER_HEAD_ * bh;
	uint16_t * fat16;
	uint64_t lba;
	uint32_t index;
	uint32_t index_max;

	if(!fat->free_cluster_count) return 0;
	if(!old || old == 0xffffffff) old = 2;
	index_max = fat->info.logical_sector_size/2;
	index = old & (index_max - 1);
	lba = old / index_max;
	lba += fat->fat_lba;
	while(old < fat->cluster_count){
		bh = bread(fat->part,lba);
		fat16 = bh->addr;
		while(index < index_max && old < fat->cluster_count){
			if(!fat16[index]){
				bfree(bh);
				return old;
			}
			old++;
		}
		bfree(bh);
		if(old >= fat->cluster_count){
			fat->free_cluster_count = 0;
			return 0;
		}
		lba++;
	}
	fat->free_cluster_count = 0;
	return 0;
}
static uint32_t fat_find_next_free_cluster_12(struct _FAT_* fat,uint32_t old) {
	struct _BUFFER_HEAD_ * bh;
	uint8_t * fat8;
	uint64_t lba;
	uint32_t half_index;
	uint32_t half_index_max;
	uint32_t fat_val;
	
	if(!fat->free_cluster_count) return 0;
	if(!old || old == 0xffffffff) old = 2;
	half_index_max = fat->info.logical_sector_size * 3;
	half_index = old * 3;
	lba = half_index / half_index_max;
	half_index %= half_index_max;
	lba += fat->fat_lba;
	bh = bread(fat->part,lba);
	fat8 = bh->addr;
	while(half_index < half_index_max && old < fat->cluster_count){
		if(half_index >= half_index_max - 2){
			fat_val = fat8[half_index >> 1];
			bfree(bh);
			lba++;
			bh = bread(fat->part,lba);
			fat8 = bh->addr;
			if(half_index == half_index_max - 2){
				half_index = 1;
				fat_val |= ((uint32_t)(fat8[0] & 0x0f)) << 8;
			}
			else {
				half_index = 2;
				fat_val >>= 4;
				fat_val |= ((uint32_t)(fat8[0])) << 4;
			}
		}
		else{
			if(half_index & 0x01){
				fat_val = (fat8[half_index >> 1] >> 4) & 0x0f;
				fat_val |= ((uint32_t)(fat8[(half_index >> 1) + 1])) << 4;
			}
			else {
				fat_val = fat8[half_index >> 1];
				fat_val |= ((uint32_t)(fat8[(half_index >> 1) + 1] & 0x0f)) << 8;
			}
			half_index += 3;
			if (half_index >= half_index_max) {
				half_index -= half_index_max;
				lba++;
				bfree(bh);
				bh = bread(fat->part, lba);
				fat8 = bh->addr;
			}
		}
		if (!fat_val) {
			bfree(bh);
			return old;
		}
		old++;
	}
	bfree(bh);
	fat->free_cluster_count = 0;
	return 0;
}
static uint32_t fat_alloc_cluster(struct _FAT_ * fat) {
	struct _FAT_INFO_* _info;
	uint32_t res;
	
	if(fat->comm_high){
		while(buf_lock(fat->fs_info,0,1)) wait(0);
		_info = fat->fs_info->addr;
		res = _info->next_free;
		if(res == 0xffffffff){
			fat->find_next_free_cluster(fat,0);
			res = _info->next_free;
		}
		fat->list_cluster(fat,res,0xffffffff);
		fat->find_next_free_cluster(fat,0);
		return res;
	}
	res = fat->first_free_cluster;
	if(res == 0xffffffff){
		fat->find_next_free_cluster(fat,0xffffffff);
		res = fat->first_free_cluster;
	}
	fat->list_cluster(fat,res,0xffffffff);
	fat->find_next_free_cluster(fat,res);
	return res;
}
static void fat_clean_cluster(struct _FAT_* fat, uint32_t cluster) {
	uint64_t lba;
	int sec_per_cluster;
	struct _BUFFER_HEAD_* bh;

	sec_per_cluster = fat->cluster_size / 512;
	lba = fat->cluster_lba + cluster * sec_per_cluster;
	while (sec_per_cluster) {
		sec_per_cluster--;
		bh = bread(fat->part, lba);
		if (!bh) return;
		bdirty(bh);
		memset(bh->addr, 0, 512);
		bfree(bh);
		lba++;
	}
}
int fat_list_cluster_32(struct _FAT_ * fat, uint32_t cluster, uint32_t to){
	struct _BUFFER_HEAD_* bh;
	unsigned int off;
	unsigned int lba;

	if(cluster <= 1 || cluster >= fat->cluster_count) return -1;
	off = cluster << 2;
	lba = off / 512;
	off &= 511;
	bh = bread(fat->part, fat->fat_lba + lba);
	if (!bh) return 0;
	bdirty(bh);
	((unsigned int*)(bh->addr))[off >> 2] = to & 0x0fffffff;
	bfree(bh);
	return 0;
}
int fat_list_cluster_16(struct _FAT_ * fat, uint32_t cluster, uint32_t to){
	struct _BUFFER_HEAD_* bh;
	unsigned int off;
	unsigned int lba;

	if(cluster <= 1 || cluster >= fat->cluster_count) return -1;
	off = cluster << 1;
	lba = off / 512;
	off &= 511;
	bh = bread(fat->part, fat->fat_lba + lba);
	if (!bh) return 0;
	bdirty(bh);
	((unsigned short*)(bh->addr))[off >> 1] = to;
	bfree(bh);
	return 0;
}
int fat_list_cluster_12(struct _FAT_ * fat, uint32_t cluster, uint32_t to){
	struct _BUFFER_HEAD_* bh;
	unsigned int _cluster;
	unsigned int off;
	unsigned int lba;

	if(cluster <= 1 || cluster >= fat->cluster_count) return -1;
	off = cluster + (cluster >> 1);
	lba = off / 512;
	off &= 511;
	bh = bread(fat->part, fat->fat_lba + lba);
	if (!bh) return 0;
	if (off == fat->info.logical_sector_size - 1) {
		bdirty(bh);
		if (cluster & 0x01) {
			_cluster = ((unsigned char*)(bh->addr))[off];
			_cluster &= 0x0f;
			_cluster |= to << 4;
			((unsigned char*)(bh->addr))[off] = _cluster;
			bfree(bh);
			bh = bread(fat->part, fat->fat_lba + lba + 1);
			if (!bh) return 0;
			bdirty(bh);
			((unsigned char*)(bh->addr))[0] = to >> 4;
		}
		else {
			((unsigned char*)(bh->addr))[off] = cluster;
			bfree(bh);
			bh = bread(fat->part, fat->fat_lba + lba + 1);
			if (!bh) return 0;
			_cluster = ((unsigned char*)(bh->addr))[0];
			_cluster &= 0xf0;
			_cluster |= (to >> 8) & 0x0f;
			bdirty(bh);
			((unsigned char*)(bh->addr))[0] = _cluster;
		}
		bfree(bh);
		return 0;
	}
	_cluster = ((unsigned char*)(bh->addr))[off];
	_cluster |= ((uint32_t)(((unsigned char*)(bh->addr))[off + 1])) << 8;
	if (cluster & 0x01) {
		_cluster &= 0x000f;
		_cluster |= (to << 4) & 0x0fff0;
	}
	else {
		_cluster &= 0xf000;
		_cluster |= to & 0x0fff;
	}
	bdirty(bh);
	((unsigned char*)(bh->addr))[off] = _cluster;
	((unsigned char*)(bh->addr))[off + 1] = _cluster >> 8;
	bfree(bh);
	return 0;
}
static struct _BUFFER_HEAD_* fat_off2bh(struct _FAT_* fat, struct _BUFFER_HEAD_* bh, uint64_t off) {
	uint32_t vcluster;
	uint32_t pcluster;
	uint32_t cluster_off;
	struct FAT_DIR* dir;
	uint64_t lba;
	struct _BUFFER_HEAD_* _bh;

	dir = bh->addr;
	if ((!(dir->attr & FAT_DIR_ATTR_DIRECTORY)) && (off > dir->size)) return NULL;
	vcluster = (uint32_t)(off / fat->cluster_size);
	cluster_off = (uint32_t)(off % fat->cluster_size);
	pcluster = dir->cluster_low;
	pcluster |= dir->cluster_hi << 16;
	while (vcluster) {
		vcluster--;
		pcluster = fat->next_cluster(fat, pcluster);
		if (!pcluster || pcluster >= 0xfffffff0) return NULL;
	}
	lba = pcluster * fat->cluster_size / 512 + fat->cluster_lba;
	bh = bread(fat->part, lba);
	if (!bh) return NULL;
	_bh = hl_blink(bh, cluster_off, 32);
	bfree(bh);
	return _bh;
}
static int fat_read_file(struct _FAT_ * fat,struct _BUFFER_HEAD_ * entry,uint32_t off,void * buf,uint32_t count) {
	uint32_t vcluster;
	uint32_t pcluster;
	uint32_t cluster_off;
	struct FAT_DIR* dir;
	uint64_t lba;
	struct _BUFFER_HEAD_* bh;
	uint32_t _size;

	dir = entry->addr;
	if ((!(dir->attr & FAT_DIR_ATTR_DIRECTORY)) && ((uint64_t)off + (uint64_t)count) > dir->size) return -1;
	vcluster = (uint32_t)(off / fat->cluster_size);
	cluster_off = (uint32_t)(off % fat->cluster_size);
	pcluster = dir->cluster_low;
	pcluster |= ((uint32_t)dir->cluster_hi) << 16;
	while (vcluster) {
		vcluster--;
		pcluster = fat->next_cluster(fat, pcluster);
		if (!pcluster || pcluster >= 0xfffffff0) return -1;
	}
	lba = pcluster * fat->cluster_size / 512 + fat->cluster_lba;
	if ((uint64_t)cluster_off + count <= fat->cluster_size) {
		lba += cluster_off >> 9;
		cluster_off &= 0x1ff;
		if (cluster_off + count <= 512) {
			bh = bread(fat->part, lba);
			memcpy(buf, cluster_off + (char*)(bh->addr), count);
			bfree(bh);
			return 0;
		}
		if (cluster_off) {
			bh = bread(fat->part, lba);
			memcpy(buf, cluster_off + (char*)(bh->addr), 512 - cluster_off);
			count -= 512 - cluster_off;
			buf = 512 - cluster_off + (char*)buf;
			lba++;
			bfree(bh);
		}
		while (count > 512) {
			bh = bread(fat->part, lba);
			memcpy(buf, bh->addr, 512);
			count -= 512;
			buf = 512 + (char*)buf;
			lba++;
			bfree(bh);
		}
		if (count) {
			bh = bread(fat->part, lba);
			memcpy(buf, bh->addr, count);
			bfree(bh);
		}
		return 0;
	}
	if (cluster_off) {
		_size = (uint32_t)(fat->cluster_size - cluster_off);
		count -= _size;
		lba += cluster_off >> 9;
		cluster_off &= 0x1ff;
		if (cluster_off + count <= 512) {
			bh = bread(fat->part, lba);
			memcpy(buf, cluster_off + (char*)(bh->addr), count);
			bfree(bh);
			return 0;
		}
		if (cluster_off) {
			bh = bread(fat->part,lba);
			memcpy(buf, cluster_off + (char*)(bh->addr), 512 - cluster_off);
			count -= 512 - cluster_off;
			buf = 512 - cluster_off + (char*)buf;
			lba++;
			bfree(bh);
		}
		while (_size >= 512) {
			bh = bread(fat->part, lba);
			memcpy(buf, bh->addr, 512);
			_size -= 512;
			buf = 512 + (char*)buf;
			lba++;
			bfree(bh);
		}
		pcluster = fat->next_cluster(fat, pcluster);
		if (!pcluster || pcluster >= 0xfffffff0) return -1;
		lba = pcluster * fat->cluster_size / 512 + fat->cluster_lba;
	}
	while (count > fat->cluster_size) {
		_size = (uint32_t)(fat->cluster_size);
		while (_size >= 512) {
			bh = bread(fat->part, lba);
			memcpy(buf, bh->addr, 512);
			_size -= 512;
			buf = 512 + (char*)buf;
			lba++;
			bfree(bh);
		}
		count -= (uint32_t)(fat->cluster_size);
		pcluster = fat->next_cluster(fat, pcluster);
		if (!pcluster || pcluster >= 0xfffffff0) return -1;
		lba = pcluster * fat->cluster_size / 512 + fat->cluster_lba;
	}
	while (count > 512) {
		bh = bread(fat->part, lba);
		memcpy(buf, bh->addr, 512);
		count -= 512;
		buf = 512 + (char*)buf;
		lba++;
		bfree(bh);
	}
	if (count) {
		bh = bread(fat->part, lba);
		memcpy(buf, bh->addr, count);
		bfree(bh);
	}
	return 0;
}
static int fat_write_file(struct _FAT_* fat, struct _BUFFER_HEAD_* entry, uint32_t off, void* buf, uint32_t count) {
	uint32_t vcluster;
	uint32_t pcluster,ppcluster;
	uint32_t cluster_off;
	struct FAT_DIR* dir;
	uint64_t lba;
	struct _BUFFER_HEAD_* bh;
	uint32_t _size;

	dir = entry->addr;
	if (((uint64_t)off + (uint64_t)count) > dir->size) {
		bdirty(entry);
		dir->size = off + count;
	}
	vcluster = (uint32_t)(off / fat->cluster_size);
	cluster_off = (uint32_t)(off % fat->cluster_size);
	pcluster = dir->cluster_low;
	pcluster |= dir->cluster_hi << 16;
	if (!pcluster) {
		pcluster = fat_alloc_cluster(fat);
		bdirty(entry);
		dir->cluster_low = pcluster;
		dir->cluster_hi = pcluster >> 16;
		fat_clean_cluster(fat, pcluster);
	}
	while (vcluster) {
		vcluster--;
		ppcluster = pcluster;
		pcluster = fat->next_cluster(fat, pcluster);
		if (!pcluster || pcluster >= 0xfffffff0) {
			pcluster = fat_alloc_cluster(fat);
			fat_clean_cluster(fat, pcluster);
			fat->list_cluster(fat, ppcluster, pcluster);
		}
	}
	lba = pcluster * fat->cluster_size / 512 + fat->cluster_lba;
	if ((uint64_t)cluster_off + count <= fat->cluster_size) {
		lba += cluster_off >> 9;
		cluster_off &= 0x1ff;
		if (cluster_off + count <= 512) {
			bh = bread(fat->part, lba);
			bdirty(bh);
			memcpy(cluster_off + (char*)(bh->addr), buf, count);
			bfree(bh);
			return 0;
		}
		if (cluster_off) {
			bh = bread(fat->part, lba);
			bdirty(bh);
			memcpy(cluster_off + (char*)(bh->addr), buf, 512 - cluster_off);
			count -= 512 - cluster_off;
			buf = 512 - cluster_off + (char*)buf;
			lba++;
			bfree(bh);
		}
		while (count > 512) {
			bh = bread(fat->part, lba);
			bdirty(bh);
			memcpy(bh->addr, buf, 512);
			count -= 512;
			buf = 512 + (char*)buf;
			lba++;
			bfree(bh);
		}
		if (count) {
			bh = bread(fat->part, lba);
			bdirty(bh);
			memcpy(bh->addr, buf, count);
			bfree(bh);
		}
		return 0;
	}
	if (cluster_off) {
		_size = (uint32_t)(fat->cluster_size - cluster_off);
		count -= _size;
		lba += cluster_off >> 9;
		cluster_off &= 0x1ff;
		if (cluster_off + count <= 512) {
			bh = bread(fat->part, lba);
			bdirty(bh);
			memcpy(cluster_off + (char*)(bh->addr), buf, count);
			bfree(bh);
			return 0;
		}
		if (cluster_off) {
			bh = bread(fat->part, lba);
			bdirty(bh);
			memcpy(cluster_off + (char*)(bh->addr), buf, 512 - cluster_off);
			count -= 512 - cluster_off;
			buf = 512 - cluster_off + (char*)buf;
			lba++;
			bfree(bh);
		}
		while (_size >= 512) {
			bh = bread(fat->part, lba);
			bdirty(bh);
			memcpy(bh->addr, buf, 512);
			_size -= 512;
			buf = 512 + (char*)buf;
			lba++;
			bfree(bh);
		}
		ppcluster = pcluster;
		pcluster = fat->next_cluster(fat, pcluster);
		if (!pcluster || pcluster >= 0xfffffff0) {
			pcluster = fat_alloc_cluster(fat);
			fat_clean_cluster(fat, pcluster);
			fat->list_cluster(fat, ppcluster, pcluster);
		}
		lba = pcluster * fat->cluster_size / 512 + fat->cluster_lba;
	}
	while (count > fat->cluster_size) {
		_size = (uint32_t)(fat->cluster_size);
		while (_size >= 512) {
			bh = bread(fat->part, lba);
			bdirty(bh);
			memcpy(bh->addr, buf, 512);
			_size -= 512;
			buf = 512 + (char*)buf;
			lba++;
			bfree(bh);
		}
		count -= (uint32_t)(fat->cluster_size);
		ppcluster = pcluster;
		pcluster = fat->next_cluster(fat, pcluster);
		if (!pcluster || pcluster >= 0xfffffff0) {
			pcluster = fat_alloc_cluster(fat);
			fat_clean_cluster(fat, pcluster);
			fat->list_cluster(fat, ppcluster, pcluster);
		}
		lba = pcluster * fat->cluster_size / 512 + fat->cluster_lba;
	}
	while (count > 512) {
		bh = bread(fat->part, lba);
		bdirty(bh);
		memcpy(bh->addr, buf, 512);
		count -= 512;
		buf = 512 + (char*)buf;
		lba++;
		bfree(bh);
	}
	if (count) {
		bh = bread(fat->part, lba);
		bdirty(bh);
		memcpy(buf, bh->addr, count);
		bfree(bh);
	}
	return 0;
}
static LPSTREAM fat_open_file(wchar_t* name, u64 mode, LPSIMPLE_PATH _path) {
	wchar_t* path, * tpath;
	struct _BUFFER_HEAD_* dir_bh,*bh;
	char short_name[11];
	struct FAT_DIR dir;
	struct _FAT_* fat;
	LPSTREAM file;
	wchar_t* point;
	int i;
	uint64_t off;
	stamp64_t time_stamp;
	struct tm _tm;
	short dos_date;
	short dos_time;
	uint32_t cluster;
	uint32_t cluster2;
	size_t name_len;

	fat = _path->fs->data;
	path = name;

	if (_path->idata0) {
		name = path;
		tpath = wcschr(path, L'\\');
		if (tpath) {
			*tpath = 0;
			name_len = tpath - path;
			path = tpath + 1;
			memset(short_name, 0x20, 11);
			if (name_len == 1 && name[0] == L'.') {
				short_name[0] = '.';
			}
			else if (name_len == 2 && name[0] == L'.' && name[1] == L'.') {
				short_name[0] = '.';
				short_name[1] = '.';
			}
			else {
				point = wcsrchr(name, L'.');
				if (!point) {
					for (i = 0;i < 8;i++) {
						if (!name[i]) break;
						if (name[i] > 255) break;
						short_name[i] = name[i];
					}
				}
				else {
					*point = 0;
					point++;
					for (i = 0;i < 8;i++) {
						if (!name[i]) break;
						if (name[i] > 255) break;
						short_name[i] = name[i];
					}
					for (i = 0;i < 3;i++) {
						if (!point[i]) break;
						if (point[i] > 255) break;
						short_name[i + 8] = point[i];
					}
				}
			}
			off = 0;
			while (1) {
				if (fat->read_root(fat, off, &dir, 32)) return NULL;
				if (!dir.short_name[0]) {
					//TODO:create a new folder
					if (fat->fs_type == FAT_TYPE_32) cluster2 = fat->root;
					else cluster2 = 0;
					dir_bh = fat->syn_rootoff2bh(fat, off);
					if (!dir_bh) return NULL;
					memset(&dir, 0, 32);
					memcpy(dir.short_name, short_name, 11);
					dir.attr = FAT_DIR_ATTR_DIRECTORY;
					time(&time_stamp);
					_tm = *localtime(&time_stamp);
					dos_time = _tm.tm_sec >> 1;
					dos_time &= 0x001f;
					dos_time |= _tm.tm_min << 5;
					dos_time &= 0x07ff;
					dos_time |= _tm.tm_hour << 11;
					dos_date = _tm.tm_mday;
					dos_date &= 0x1f;
					dos_date |= _tm.tm_mon << 5;
					dos_date &= 0x03ff;
					dos_date |= (_tm.tm_year - 1980) << 9;
					dir.access_date = dos_date;
					dir.create_date = dos_date;
					dir.modify_date = dos_date;
					dir.create_time = dos_time;
					dir.modify_time = dos_time;
					cluster = fat_alloc_cluster(fat);
					fat_clean_cluster(fat, cluster);
					dir.cluster_low = cluster;
					dir.cluster_hi = cluster >> 16;
					bdirty(dir_bh);
					memcpy(dir_bh->addr, &dir, 32);
					bh = bread(fat->part, cluster * fat->cluster_size / 512 + fat->cluster_lba);
					if (!bh) {
						bfree(dir_bh);
						return NULL;
					}
					memset(dir.short_name, 0x20, 11);
					bdirty(bh);
					dir.short_name[0] = '.';
					memcpy(bh->addr, &dir, 32);
					dir.cluster_low = cluster2;
					dir.cluster_hi = cluster2 >> 16;
					dir.short_name[1] = '.';
					memcpy(32 + (char*)bh->addr, &dir, 32);
					bfree(bh);
					break;
				}
				if (!(dir.attr & FAT_DIR_ATTR_DIRECTORY) ||
					dir.attr & FAT_DIR_ATTR_VPLUME_ID ||
					memcmp(dir.short_name, short_name, 11)) {
					off += 32;
					continue;
				}
				dir_bh = fat->rootoff2bh(fat, off);
				if (!dir_bh) return NULL;
				break;
			}
		}
		else {
			memset(short_name, 0x20, 11);
			point = wcsrchr(name, L'.');
			if (!point) {
				for (i = 0;i < 8;i++) {
					if (!name[i]) break;
					if (name[i] > 255) break;
					short_name[i] = name[i];
				}
			}
			else {
				*point = 0;
				point++;
				for (i = 0;i < 8;i++) {
					if (!name[i]) break;
					if (name[i] > 255) break;
					short_name[i] = name[i];
				}
				for (i = 0;i < 3;i++) {
					if (!point[i]) break;
					if (point[i] > 255) break;
					short_name[i + 8] = point[i];
				}
			}
			off = 0;
			while (1) {
				if (fat->read_root(fat, off, &dir, 32)) return NULL;
				if (!dir.short_name[0]) {
					if (mode & FS_OPEN_N_CRATE)  return NULL;
					//TODO:create a new file
					dir_bh = fat->rootoff2bh(fat, off);
					if (!dir_bh) return NULL;
					memset(&dir, 0, 32);
					memcpy(dir.short_name, short_name, 11);
					time(&time_stamp);
					_tm = *localtime(&time_stamp);
					dos_time = _tm.tm_sec >> 1;
					dos_time &= 0x001f;
					dos_time |= _tm.tm_min << 5;
					dos_time &= 0x07ff;
					dos_time |= _tm.tm_hour << 11;
					dos_date = _tm.tm_mday;
					dos_date &= 0x1f;
					dos_date |= _tm.tm_mon << 5;
					dos_date &= 0x03ff;
					dos_date |= (_tm.tm_year - 1980) << 9;
					dir.access_date = dos_date;
					dir.create_date = dos_date;
					dir.modify_date = dos_date;
					dir.create_time = dos_time;
					dir.modify_time = dos_time;
					bdirty(dir_bh);
					memcpy(dir_bh->addr, &dir, 32);
					break;
				}
				if (dir.attr & FAT_DIR_ATTR_DIRECTORY ||
					dir.attr & FAT_DIR_ATTR_VPLUME_ID ||
					memcmp(dir.short_name, short_name, 11)) {
					off += 32;
					continue;
				}
				if (mode & FS_OPEN_N_REPLACE) return NULL;
				dir_bh = fat->rootoff2bh(fat, off);
				if (!dir_bh) return NULL;
				break;
			}
			if (mode & FS_OPEN_MONOPOLIZE) {
				if (!(dir_bh->edata.idata[BUF_EDATA_ATTR] & MONOPOLIZE)
					&& spin_try_lock_bit(dir_bh->edata.idata + BUF_EDATA_ATTR,MONOPOLIZE_BIT)) {
					if (dir_bh != _path->data) bfree(dir_bh);
					return NULL;
				}
				if (dir_bh != _path->data) bfree(dir_bh);
				return NULL;
			}
			file = kmalloc(sizeof(STREAM),0);
			if (!file) {
				if (dir_bh != _path->data) bfree(dir_bh);
				return NULL;
			}
			memset(file, 0, sizeof(STREAM));
			file->edata.idata[FILE_TABLE_INDEX] = insert_file(fat->file_table,file);
			file->edata.pdata[FILE_EDATA_NODE] = dir_bh;
			file->fc = &fat->fc;
			file->mode = (uint32_t)mode;
			return file;
		}
	}
	else dir_bh = _path->data;
	while (1) {
		name = path;
		tpath = wcschr(path, L'\\');
		if (!tpath) break;
		*tpath = 0;
		name_len = tpath - path;
		path = tpath + 1;
		memset(short_name, 0x20, 11);
		if (name_len == 1 && name[0] == L'.') {
			short_name[0] = '.';
		}
		else if (name_len == 2 && name[0] == L'.' && name[1] == L'.') {
			short_name[0] = '.';
			short_name[1] = '.';
		}
		else {
			point = wcsrchr(name, L'.');
			if (!point) {
				for (i = 0;i < 8;i++) {
					if (!name[i]) break;
					if (name[i] > 255) break;
					short_name[i] = name[i];
				}
			}
			else {
				*point = 0;
				point++;
				for (i = 0;i < 8;i++) {
					if (!name[i]) break;
					if (name[i] > 255) break;
					short_name[i] = name[i];
				}
				for (i = 0;i < 3;i++) {
					if (!point[i]) break;
					if (point[i] > 255) break;
					short_name[i + 8] = point[i];
				}
			}
		}
		off = 0;
		while (1) {
			if (fat_read_file(fat, dir_bh, off, &dir, 32)) return NULL;
			if (!dir.short_name[0]) {
				//TODO:create a new folder
				cluster2 = ((struct FAT_DIR*)(dir_bh->addr))->cluster_low;
				cluster2 = ((uint32_t)((struct FAT_DIR*)(dir_bh->addr))->cluster_hi) << 16;
				bh = fat_off2bh(fat, dir_bh, off);
				if (dir_bh != _path->data) bfree(dir_bh);
				dir_bh = bh;
				if (!dir_bh) return NULL;
				memset(&dir, 0, 32);
				memcpy(dir.short_name, short_name, 11);
				dir.attr = FAT_DIR_ATTR_DIRECTORY;
				time(&time_stamp);
				_tm = *localtime(&time_stamp);
				dos_time = _tm.tm_sec >> 1;
				dos_time &= 0x001f;
				dos_time |= _tm.tm_min << 5;
				dos_time &= 0x07ff;
				dos_time |= _tm.tm_hour << 11;
				dos_date = _tm.tm_mday;
				dos_date &= 0x1f;
				dos_date |= _tm.tm_mon << 5;
				dos_date &= 0x03ff;
				dos_date |= (_tm.tm_year - 1980) << 9;
				dir.access_date = dos_date;
				dir.create_date = dos_date;
				dir.modify_date = dos_date;
				dir.create_time = dos_time;
				dir.modify_time = dos_time;
				cluster = fat_alloc_cluster(fat);
				fat_clean_cluster(fat, cluster);
				dir.cluster_low = cluster;
				dir.cluster_hi = cluster >> 16;
				bdirty(dir_bh);
				memcpy(dir_bh->addr, &dir, 32);
				bh = bread(fat->part, cluster * fat->cluster_size / 512 + fat->cluster_lba);
				if (!bh) {
					bfree(dir_bh);
					return NULL;
				}
				memset(dir.short_name, 0x20, 11);
				bdirty(bh);
				dir.short_name[0] = '.';
				memcpy(bh->addr, &dir, 32);
				dir.cluster_low = cluster2;
				dir.cluster_hi = cluster2 >> 16;
				dir.short_name[1] = '.';
				memcpy(32 + (char*)bh->addr, &dir, 32);
				bfree(bh);
				break;
			}
			if (!(dir.attr & FAT_DIR_ATTR_DIRECTORY) ||
				dir.attr & FAT_DIR_ATTR_VPLUME_ID ||
				memcmp(dir.short_name, short_name, 11)) {
				off += 32;
				continue;
			}
			bh = fat_off2bh(fat, dir_bh, off);
			if (dir_bh != _path->data) bfree(dir_bh);
			dir_bh = bh;
			if (!dir_bh) return NULL;
			break;
		}
	}
	memset(short_name, 0x20, 11);
	point = wcsrchr(name, L'.');
	if (!point) {
		for (i = 0;i < 8;i++) {
			if (!name[i]) break;
			if (name[i] > 255) break;
			short_name[i] = name[i];
		}
	}
	else {
		*point = 0;
		point++;
		for (i = 0;i < 8;i++) {
			if (!name[i]) break;
			if (name[i] > 255) break;
			short_name[i] = name[i];
		}
		for (i = 0;i < 3;i++) {
			if (!point[i]) break;
			if (point[i] > 255) break;
			short_name[i + 8] = point[i];
		}
	}
	off = 0;
	while (1) {
		if (fat_read_file(fat, dir_bh, off, &dir, 32)) return NULL;
		if (!dir.short_name[0]) {
			if (mode & FS_OPEN_N_CRATE) {
				if (dir_bh != _path->data) bfree(dir_bh);
				return NULL;
			}
			//TODO:create a new file
			bh = fat_off2bh(fat, dir_bh, off);
			if (dir_bh != _path->data) bfree(dir_bh);
			dir_bh = bh;
			if (!dir_bh) return NULL;
			memset(&dir, 0, 32);
			memcpy(dir.short_name, short_name, 11);
			time(&time_stamp);
			_tm = *localtime(&time_stamp);
			dos_time = _tm.tm_sec >> 1;
			dos_time &= 0x001f;
			dos_time |= _tm.tm_min << 5;
			dos_time &= 0x07ff;
			dos_time |= _tm.tm_hour << 11;
			dos_date = _tm.tm_mday;
			dos_date &= 0x1f;
			dos_date |= _tm.tm_mon << 5;
			dos_date &= 0x03ff;
			dos_date |= (_tm.tm_year - 1980) << 9;
			dir.access_date = dos_date;
			dir.create_date = dos_date;
			dir.modify_date = dos_date;
			dir.create_time = dos_time;
			dir.modify_time = dos_time;
			bdirty(dir_bh);
			memcpy(dir_bh->addr, &dir, 32);
			break;
		}
		if (dir.attr & FAT_DIR_ATTR_DIRECTORY ||
			dir.attr & FAT_DIR_ATTR_VPLUME_ID ||
			memcmp(dir.short_name, short_name, 11)) {
			off += 32;
			continue;
		}
		if (mode & FS_OPEN_N_REPLACE) {
			if (dir_bh != _path->data) bfree(dir_bh);
			return NULL;
		}
		bh = fat_off2bh(fat, dir_bh, off);
		if (dir_bh != _path->data) bfree(dir_bh);
		dir_bh = bh;
		if (!dir_bh) return NULL;
		break;
	}
	if (mode & FS_OPEN_MONOPOLIZE) {
		if (!(dir_bh->edata.idata[BUF_EDATA_ATTR] & MONOPOLIZE)
			&& spin_try_lock_bit(dir_bh->edata.idata + BUF_EDATA_ATTR,MONOPOLIZE_BIT)) {
			if (dir_bh != _path->data) bfree(dir_bh);
			return NULL;
		}
		if (dir_bh != _path->data) bfree(dir_bh);
		return NULL;
	}
	file = kmalloc(sizeof(STREAM),0);
	if (!file) {
		if (dir_bh != _path->data) bfree(dir_bh);
		return NULL;
	}
	memset(file, 0, sizeof(STREAM));
	file->edata.idata[FILE_TABLE_INDEX] = insert_file(fat->file_table,file);
	file->edata.pdata[FILE_EDATA_NODE] = dir_bh;
	file->fc = &fat->fc;
	file->mode = (uint32_t)mode;
	return file;
}

static int fat_close(LPSTREAM file) {
	struct _FAT_* fat;
	struct _BUFFER_HEAD_* bh;
	
	fat = file->fc->data;
	bh = file->edata.pdata[FILE_EDATA_NODE];
	remove_file(fat->file_table,file,file->edata.idata[FILE_TABLE_INDEX]);
	if (file->mode & FS_OPEN_MONOPOLIZE) lock_andd(bh->edata.idata + BUF_EDATA_ATTR,MONOPOLIZE);
	bfree(bh);
	kfree(file);
}
static int fat_read(LPSTREAM file, size_t count, void* buf) {
	int err = fat_read_file(file->fc->data, file->edata.pdata[FILE_EDATA_NODE], file->read_pos, buf, count);
	file->read_pos += count;
	return err;
}
static int fat_write(LPSTREAM file, size_t count, void* buf) {
	int err =  fat_write_file(file->fc->data, file->edata.pdata[FILE_EDATA_NODE], file->write_pos, buf, count);
	file->write_pos += count;
	return err;
}
static int fat_seek_get(LPSTREAM file, s64 pos, int org) {
	struct _BUFFER_HEAD_* node;
	struct FAT_DIR* dir;

	node = file->edata.pdata[FILE_EDATA_NODE];
	dir = node->addr;
	if (org == SEEK_SET) {
		file->read_pos = pos;
	}
	else if (org == SEEK_CUR) {
		file->read_pos += pos;
	}
	else {
		file->read_pos = dir->size + pos;
	}
	if (dir->size < file->read_pos) {
		bdirty(node);
		dir->size = file->read_pos;
	}
	return 0;
}
static int fat_seek_put(LPSTREAM file, s64 pos, int org) {
	struct _BUFFER_HEAD_* node;
	struct FAT_DIR* dir;

	node = file->edata.pdata[FILE_EDATA_NODE];
	dir = node->addr;
	if (org == SEEK_SET) {
		file->write_pos = pos;
	}
	else if (org == SEEK_CUR) {
		file->write_pos += pos;
	}
	else {
		file->write_pos = dir->size + pos;
	}
	if (dir->size < file->write_pos) {
		bdirty(node);
		dir->size = file->write_pos;
	}
	return 0;
}
static u64 fat_tell_get(LPSTREAM file) {
	return file->read_pos;
}
static u64 fat_tell_put(LPSTREAM file) {
	return file->write_pos;
}
static int fat_get(LPSTREAM file) {
	int element;
	int err;
	
	err = fat_read(file, 1, &element);
	if (err) return err;
	return element;
}
static int fat_put(LPSTREAM file,  int element) {
	return fat_write(file, 1, &element);
}

static FCPEB fat_fc = {
	.close = fat_close,
	.read = fat_read,
	.write = fat_write,
	.seek_put = fat_seek_put,
	.seek_get = fat_seek_get,
	.tell_put = fat_tell_put,
	.tell_get = fat_tell_get,
	.put = fat_put,
	.get = fat_get,
};
static FSCTRL fat_fs = {.open = fat_open_file};

static void open_fat_call_back(LPSTREAM part, struct _FAT_* fat) {
	struct _BUFFER_HEAD_ * bh;

	if (fat->vaild) {
		fat->vaild = 0;
		free_file_table(fat->file_table);
		if(fat->alter_lba){
			bh = bread(fat->part,fat->alter_lba);
			bdirty(bh);
			memcpy(bh->addr,fat->boot->addr,fat->info.logical_sector_size);
			bfree(bh);
			if(fat->comm_high){
				bh = bread(fat->part,fat->alter_lba + 1);
				bdirty(bh);
				memcpy(bh->addr,fat->fs_info->addr,fat->info.logical_sector_size);
				bfree(bh);
			}
		}
		buf_unlock(fat->boot, 0);
		bfree(fat->boot);
		if (fat->fs_info) bfree(fat->fs_info);
	}
	close(fat->part);
	kfree(fat);
}
static int raw_summon_fat(struct _FAT_* fat) {
	//struct _BUFFER_HEAD_* bh;
	uint32_t info_lba;
	uint32_t rvd_sectors;
	unsigned char* boot;
	struct _FAT_INFO_* _info;
	struct FAT_DIR* dir;
	struct _BUFFER_HEAD_* bh;
	uint64_t lba;
	int remind_size,sec_size;
	int i;
	int byte_count;
	char tmp;

	info(fat->part, sizeof(struct _PART_INFO_), &fat->info);
	fat->boot = bread(fat->part, 0);
	if (!fat->boot) return -1;
	if (buf_lock(fat->boot, 0, 512)) {
		bfree(fat->boot);
		return -1;
	}
	boot = fat->boot->addr;
	fat->sector_count = boot[BPB_SECTORS_COUNT_16] | (((uint32_t)(boot[BPB_SECTORS_COUNT_16 + 1])) << 8);
	if (!fat->sector_count) {
		fat->fs_type = FAT_TYPE_32;
		fat->sector_count = *(uint32_t*)(boot + BPB_SECTORS_COUNT_32);
		if (!fat->sector_count) return 0;
	}
	if (!fat->fs_type) {
		fat->fat_size = boot[BPB_FAT_SIZE_16] | (((uint32_t)(boot[BPB_FAT_SIZE_16 + 1])) << 8);
		if (!fat->fat_size) {
			fat->fat_size = *(uint32_t*)(boot + BPB_FAT_SIZE_32);
			fat->fs_type = FAT_TYPE_32;
		}
	}
	else fat->fat_size = *(uint32_t*)(boot + BPB_FAT_SIZE_32);
	if (!fat->fat_size) return 0;
	fat->fat_count = boot[BPB_FATS_NUMBER];
	if (!fat->fat_count) return 0;
	fat->cluster_size = boot[BPB_SECTORS_PER_CLUSTER];
	fat->cluster_size <<= 9;
	fat->root_ent_cnt = boot[BPB_ROOT_ENT_CNT];
	fat->root_ent_cnt |= ((uint32_t)(boot[BPB_ROOT_ENT_CNT + 1])) << 8;
	rvd_sectors = boot[BPB_RESERVED_SECTORS];
	rvd_sectors |= ((uint32_t)(boot[BPB_RESERVED_SECTORS + 1])) << 8;
	fat->fat_lba = rvd_sectors;
	memcpy(&(fat->fc), &fat_fc, sizeof(FCPEB));
	memcpy(&(fat->fs), &fat_fs, sizeof(FSCTRL));
	fat->fc.data = fat;
	fat->fs.data = fat;
	if (fat->fs_type == FAT_TYPE_32) {
        fat->read_root = fat_read_root_32;
        fat->write_root = fat_write_root_32;
		fat->rootoff2bh = fat_rootoff2bh_32;
		fat->syn_rootoff2bh = fat_syn_rootoff2bh_32;
		fat->next_cluster = fat_next_cluster_32;
		fat->list_cluster = fat_list_cluster_32;
		if (memcmp(BS_FILE_SYSTEM_TYPE_32 + boot, "FAT", 3)) return 0;
		if(memcmp(BS_FILE_SYSTEM_TYPE_32 + boot,"FAT32   ", 8) && 
			memcmp(BS_FILE_SYSTEM_TYPE_32 + boot,"FAT     ", 8)) return 0;
		fat->alter_lba = *(uint16_t*)(boot + BPB_BACK_BOOT_SECTOR);
		fat->cluster_count = (uint32_t)((fat->sector_count - fat->fat_size * fat->fat_count - rvd_sectors) / (fat->cluster_size/512));
		info_lba = *(uint16_t*)(boot + BPB_FS_INFO_32);
		if(info_lba) fat->fs_info = bread(fat->part, info_lba);
		else if(fat->alter_lba >= 2){
			fat->fs_info = bread(fat->part,1);
			bdirty(fat->boot);
			*(uint16_t*)(boot + BPB_FS_INFO_32) = 1;
		}
		fat->root = *(uint32_t*)(boot + BPB_ROOT_CLUSTER_32);
		fat->cluster_lba = rvd_sectors + (fat->fat_count * fat->fat_size) - 2 * fat->cluster_size / 512;
		if (fat->fs_info) {
			fat->find_next_free_cluster = fat_find_next_free_cluster_32_info;;
			_info = fat->fs_info->addr;
			if (_info->lead_sign != 0x41615252 ||
				_info->struct_sign != 0x61417272 ||
				_info->tail_sign != 0xaa550000) {
				bdirty(fat->fs_info);
				_info->lead_sign = 0x41615252;
				_info->struct_sign = 0x61517272;
				_info->tail_sign = 0xaa550000;
				_info->free_count = 0xffffffff;
				_info->next_free = 0xffffffff;
			}
		}
		else fat->find_next_free_cluster = fat_find_next_free_cluster_32;
	}
	else {
        fat->read_root = fat_read_root_12_16;
        fat->write_root = fat_write_root_12_16;
		fat->rootoff2bh = fat_rootoff2bh_12_16;
		fat->syn_rootoff2bh = fat_syn_rootoff2bh_12_16;
		if (memcmp(BS_FILE_SYSTEM_TYPE_16 + boot, "FAT", 3)) return 0;
		if (!memcmp(BS_FILE_SYSTEM_TYPE_16 + boot, "FAT12   ", 8)) fat->fs_type = FAT_TYPE_12;
		else if (!memcmp(BS_FILE_SYSTEM_TYPE_16 + boot, "FAT16   ", 8)) fat->fs_type = FAT_TYPE_16;
		fat->cluster_count = (uint32_t)((fat->sector_count - fat->fat_size * fat->fat_count - (fat->root_ent_cnt * 32 + fat->info.logical_sector_size - 1)/fat->info.logical_sector_size - rvd_sectors) / (fat->cluster_size/512));
		if (!fat->fs_type) {
			if (fat->cluster_count < 4085) fat->fs_type = FAT_TYPE_12;
			else fat->fs_type = FAT_TYPE_16;
		}
		if(fat->fs_type == FAT_TYPE_12) {
			fat->next_cluster = fat_next_cluster_12;
			fat->find_next_free_cluster = fat_find_next_free_cluster_12;
			fat->list_cluster = fat_list_cluster_12;
		}
		else {
			fat->next_cluster = fat_next_cluster_16;
			fat->find_next_free_cluster = fat_find_next_free_cluster_16;
			fat->list_cluster = fat_list_cluster_16;
		}
		fat->root = rvd_sectors + fat->fat_count * fat->fat_size;
		fat->cluster_lba = fat->root + fat->root_ent_cnt * 32 / 512 - 2 * fat->cluster_size / 512;
	}
	fat->vaild = 1;
	fat->my_path.data = fat;
	fat->my_path.fs = &fat->fs;
	fat->my_path.idata0 = 1;
	fat->name = fat->_name;
	wsprintf(fat->_name,24,L"/fs/%d/",fat->info.soft_id);
	fs_map(fat->name, &(fat->my_path), &(fat->f_path));
	return 0;
}
int fat_open(const wchar_t * path) {
	struct _FAT_* fat;

	fat = kmalloc(sizeof(struct _FAT_),0);
	if (!fat) return -1;
	memset(fat, 0, sizeof(struct _FAT_));
	fat->part = open(path, FS_OPEN_READ | FS_OPEN_WRITE | FS_OPEN_N_CRATE | FS_OPEN_MONOPOLIZE, fat, open_fat_call_back);
	if (!fat->part) {
		kfree(fat);
		return -1;
	}
	if (raw_summon_fat(fat)) {
		close(fat->part);
		kfree(fat);
		return -1;
	}
	return 0;
}
int unsummon_fat(HANDLE _fs) {
	struct _FAT_* fat;

	if (!_fs) return -1;
	fat = _fs;
	open_fat_call_back(fat->part, fat);
	return 0;
}
int esp_open(const wchar_t * path) {
	struct _FAT_* fat;

	fat = kmalloc(sizeof(struct _FAT_),0);
	if (!fat) return -1;
	memset(fat, 0, sizeof(struct _FAT_));
	fat->part = open(path, FS_OPEN_READ | FS_OPEN_WRITE | FS_OPEN_N_CRATE, fat, open_fat_call_back);
	if (!fat->part) {
		kfree(fat);
		return -1;
	}
	if (raw_summon_fat(fat)) {
		close(fat->part);
		kfree(fat);
		return -1;
	}
	return 0;
}
int unsummon_esp(HANDLE _fs) {
	struct _FAT_* fat;

	if (!_fs) return -1;
	fat = _fs;
	open_fat_call_back(fat->part, fat);
	return 0;
}
static void clear_fat(struct _FAT_* fat, uint64_t lba, int size) {
	struct _BUFFER_HEAD_* bh;

	while (size) {
		size--;
		bh = bread(fat->part, lba);
		if (!bh) return;
		bdirty(bh);
		memset(bh->addr, 0, 512);
		bfree(bh);
		lba++;
	}
}