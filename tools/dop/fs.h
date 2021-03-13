
/*
	tools/dop/fs.h
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



#ifndef _FS_H_
#define _FS_H_
#include "dop.h"
#include <objbase.h>
#include "error.h"

#ifndef SEEK_SET
#define SEEK_SET	0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR	1
#endif
#ifndef SEEK_END
#define SEEK_END	2
#endif

#define FS_OPEN_READ		0x00000001
#define FS_OPEN_WRITE		0x00000002
#define FS_OPEN_N_CRATE		0x00000004
#define FS_OPEN_N_REPLACE	0x00000008
#define FS_OPEN_MONOPOLIZE	0x00000010

#define EATTR_MONOPOLIZE	0x1

#define F_KILLING			0x00000001

#define LL_BLOCK_DEV_MAX	256


typedef void* LPCATALOG;

typedef struct _DOPSTREAM_ {
	int gst;
	int mode;
	u64 read_pos;
	u64 write_pos;
	void* data;
	void* call_back_data;
	void(*call_back)(struct _DOPSTREAM_*, void*);
	struct _FCPEB_* fc;
	struct _DOPSTREAM_* p_prev;//list of process file list
	struct _DOPSTREAM_* p_next;//list of process file list
	struct _DOPSTREAM_* d_prev;//list of device access list
	struct _DOPSTREAM_* d_next;//list of device access list
} DOPSTREAM,*LPDOPSTREAM;

typedef struct _FSCTRL_{
	struct _DOPSTREAM_ * (*open)(wchar_t * name,u64 mode, struct _SIMPLE_FILE_* path);
	int (*seek)(wchar_t* name, struct _SIMPLE_FILE_* path);
	int (*exist)(wchar_t* name, struct _SIMPLE_FILE_* path);
	int (*get_attr)(wchar_t* name, void * attr, struct _SIMPLE_FILE_* path);
	int (*set_attr)(wchar_t* name, void* attr, struct _SIMPLE_FILE_* path);
	int (*create)(wchar_t* name, struct _SIMPLE_FILE_* path);
	int (*remove)(wchar_t* name, struct _SIMPLE_FILE_* path);
	int (*rename)(wchar_t* dname, wchar_t* sname, struct _SIMPLE_FILE_ * path);
	int (*cat)(LPCATALOG* buf, int* count, struct _SIMPLE_FILE_* path);
	int (*link)(wchar_t* dname, wchar_t* sname, struct _SIMPLE_FILE_* path);
	void* data;
}FSCTRL,*LPFSCTRL;

struct _SIMPLE_FILE_ * cur_path;

typedef struct _SIMPLE_FILE_ {//a path,only kernel can hold this kind of file
	void* data;
	int idata0;
	int idata1;
	struct _FSCTRL_* fs;
} SIMPLE_FILE, * LPSIMPLE_FILE;
static inline int cat(LPCATALOG* buf, int* count) {
	if (cur_path && cur_path->fs && cur_path->fs->cat) return cur_path->fs->cat(buf, count, cur_path);
	return -1;
}
int seek(const wchar_t* name);

typedef struct _FCPEB_ {
	u64(*in)(LPDOPSTREAM, u64 port);
	int (*out)(LPDOPSTREAM, u64 port, u64 data);
	LPDOPSTREAM(*open)(wchar_t* name, u64 mode, struct _FCPEB_*);
	int(*close)(LPDOPSTREAM);
	int(*read)(LPDOPSTREAM, size_t count, void* buf);
	int(*write)(LPDOPSTREAM, size_t count, void* buf);
	int(*seek_get)(LPDOPSTREAM, s64 pos, int org);
	int(*seek_put)(LPDOPSTREAM, s64 pos, int org);
	u64(*tell_get)(LPDOPSTREAM);
	u64(*tell_put)(LPDOPSTREAM);
	int(*get)(LPDOPSTREAM);
	int(*put)(LPDOPSTREAM, int element);
	struct _BUFFER_HEAD_* (*bread)(LPDOPSTREAM, u64 iblock);
	struct _BUFFER_HEAD_* (*cache_bwrite)(LPDOPSTREAM, u64 iblock);
	int(*fresh_get)(LPDOPSTREAM);
	int(*fresh_put)(LPDOPSTREAM);
	u64(*get_size)(LPDOPSTREAM);
	int(*set_size)(LPDOPSTREAM, u64 size);
	u64(*get_count)(LPDOPSTREAM);
	int(*set_count)(LPDOPSTREAM, u64 count);
	int (*info)(LPDOPSTREAM,size_t size,void * info);
	void* data;
} FCPEB, * LPFCPEB;

static int inline info(LPDOPSTREAM file, size_t size, void* info) {
	if (!file || !file->fc || !file->fc->info) return -1;
	return file->fc->info(file, size, info);
}
static u64 inline in(LPDOPSTREAM file, u64 port) {
	if (!file || !file->fc || !file->fc->in) return ERR_INVAILD_PTR;
	return file->fc->in(file, port);
}
static int inline out(LPDOPSTREAM file, u64 port, u64 data) {
	if (!file || !file->fc || !file->fc->out) return ERR_INVAILD_PTR;
	return file->fc->out(file, port, data);
}
int close(LPDOPSTREAM file);
static int inline read(LPDOPSTREAM file, size_t count, void* buf) {
	if (!file || !file->fc || !file->fc->read) return ERR_INVAILD_PTR;
	if (count && !buf) return -1;
	if (!(file->mode & FS_OPEN_READ)) return ERR_OUT_OF_PERMIT;
	return file->fc->read(file, count, buf);
}
static int inline write(LPDOPSTREAM file, size_t count, void* buf) {
	if (!file || !file->fc || !file->fc->write) return ERR_INVAILD_PTR;
	if (count && !buf) return -1;
	if (!(file->mode & FS_OPEN_WRITE)) return ERR_OUT_OF_PERMIT;
	return file->fc->write(file, count, buf);
}
static int inline seek_get(LPDOPSTREAM file, s64 pos, int org) {
	if (!file || !file->fc || !file->fc->seek_get) return ERR_INVAILD_PTR;
	return file->fc->seek_get(file, pos, org);
}
static int inline seek_put(LPDOPSTREAM file, s64 pos, int org) {
	if (!file || !file->fc || !file->fc->seek_put) return ERR_INVAILD_PTR;
	return file->fc->seek_put(file, pos, org);
}
static u64 inline tell_get(LPDOPSTREAM file) {
	if (!file || !file->fc || !file->fc->tell_get) return ERR_INVAILD_PTR;
	return file->fc->tell_get(file);
}
static u64 inline tell_put(LPDOPSTREAM file) {
	if (!file || !file->fc || !file->fc->tell_put) return ERR_INVAILD_PTR;
	return file->fc->tell_put(file);
}
static int inline get(LPDOPSTREAM file) {
	if (!file || !file->fc || !file->fc->get) return ERR_INVAILD_PTR;
	if (!(file->mode & FS_OPEN_READ)) return ERR_OUT_OF_PERMIT;
	return file->fc->get(file);
}
static int inline put(LPDOPSTREAM file, int Element) {
	if (!file || !file->fc || !file->fc->put) return ERR_INVAILD_PTR;
	if (!(file->mode & FS_OPEN_WRITE)) return ERR_OUT_OF_PERMIT;
	return file->fc->put(file, Element);
}
static int inline fresh_get(LPDOPSTREAM file) {
	if (!file || !file->fc || !file->fc->fresh_get) return ERR_INVAILD_PTR;
	return file->fc->fresh_get(file);
}
static int inline fresh_put(LPDOPSTREAM file) {
	if (!file || !file->fc || !file->fc->fresh_put) return ERR_INVAILD_PTR;
	return file->fc->fresh_put(file);
}
static u64 inline get_size(LPDOPSTREAM file) {
	if (!file || !file->fc || !file->fc->get_size) return ERR_INVAILD_PTR;
	return file->fc->get_size(file);
}
static int inline set_size(LPDOPSTREAM file, u64 size) {
	if (!file || !file->fc || !file->fc->set_size) return ERR_INVAILD_PTR;
	return file->fc->set_size(file, size);
}
static u64 inline get_count(LPDOPSTREAM file) {
	if (!file || !file->fc || !file->fc->get_count) return ERR_INVAILD_PTR;
	return file->fc->get_count(file);
}
static int inline set_count(LPDOPSTREAM file, u64 count) {
	if (!file || !file->fc || !file->fc->set_count) return ERR_INVAILD_PTR;
	return file->fc->set_count(file, count);
}
int fs_map(const wchar_t* path, void* xc, struct _SIMPLE_FILE_* f_path);
int fs_unmap(const wchar_t* _path);
LPDOPSTREAM open(const wchar_t* _name, u64 mode, void* call_back_data, void(*call_back)(LPDOPSTREAM, void*));
void fs_init();

int close_vhd(HANDLE _vhd);
HANDLE create_vhd(const wchar_t* name, uint64_t count);
HANDLE open_vhd(const wchar_t* name);

HANDLE summon_part_table(const wchar_t* path);
int unsummon_part_table(HANDLE);
int create_part_table(HANDLE);

HANDLE summon_esp();
int unsummon_esp(HANDLE);
int format_esp(HANDLE);
int esp_write_boot(HANDLE, const wchar_t*);

HANDLE summon_fat();
int unsummon_fat(HANDLE);
int format_fat(HANDLE);
int fat_write_boot(HANDLE, const wchar_t*);

HANDLE summon_lfs();
int unsummon_lfs(HANDLE);
int format_lfs(HANDLE);
int lfs_write_boot(HANDLE, const wchar_t*);

#define BH_LOCKED		1
#define BH_UPDATEING	2
#define BH_DIRTY		4
#define BH_VAILD		8
#define BH_24BITS		0x00
#define BH_32BITS		0x10
#define BH_64BITS		0x20
#define BH_LOCK_LIST_SIZE	14

struct _BUFFER_LOCK_ARRAY_ {
	int mask;//4
	int attr;//4
	unsigned int start[BH_LOCK_LIST_SIZE];//24
	unsigned int end[BH_LOCK_LIST_SIZE];//24
	struct _BUFFER_LOCK_ARRAY_* next;//8
};
struct _BUFFER_HEAD_ {
	volatile int status;
		//bit0:locked
		//bit1:updataing
		//bit2:dirty
		//bit3:vaild
	unsigned int ref_count;
	unsigned int buf_size;
	unsigned int byte_off;
	void* addr;
	iblock_t iblock;
	struct _LL_BLOCK_DEV_ * dev;
	//LPTHREAD * p;
	//LPTHREAD wait;
	struct _BUFFER_HEAD_* prev,* next;
	struct _BUFFER_HEAD_* parent;
	struct _BUFFER_LOCK_ARRAY_* lock_list;
	void * data;
};
struct _DISK_INFO_ {
	u32 size;
	u32 physical_sector_size;
	u32 logical_sector_size;
	u32 cache_block_size;
	u64 logical_sector_count;
	wchar_t* path;
	GUID g_id;
};
struct _PART_INFO_ {
	u32 size;
	u32 physical_sector_size;
	u32 logical_sector_size;
	u32 cache_block_size;
	u64 logical_sector_count;
	wchar_t* path;
	GUID g_id;
	GUID g_type;
	GUID g_disk;
	u64 first_lba;
};
struct _FILE_INFO_ {
	uint32_t size;
	uint32_t attr;
	uint64_t file_size;
	wchar_t* path;
	GUID owner;
};
struct _LL_BLOCK_DEV_ {
	void* data;
	int32_t cache_size;
	int(*read_block)(struct _BUFFER_HEAD_*);
	int(*write_block)(struct _BUFFER_HEAD_*);
};

static inline struct _BUFFER_HEAD_* bread(LPDOPSTREAM dev, u64 iblock) {
	if (!dev || !(dev->fc) || !(dev->fc->bread)) return NULL;
	if (!(dev->mode & FS_OPEN_READ)) return NULL;
	return dev->fc->bread(dev, iblock);
}
static inline struct _BUFFER_HEAD_* cache_bwrite(LPDOPSTREAM dev, u64 iblock) {
	if (!dev || !(dev->fc) || !(dev->fc->cache_bwrite)) return NULL;
	if (!(dev->mode & FS_OPEN_WRITE)) return NULL;
	return dev->fc->cache_bwrite(dev, iblock);
}

struct _BUFFER_HEAD_* ll_bread(struct _LL_BLOCK_DEV_* dev, iblock_t iblock);
struct _BUFFER_HEAD_* ll_balloc(struct _LL_BLOCK_DEV_* dev, iblock_t iblock);
struct _BUFFER_HEAD_* ml_blink(struct _LL_BLOCK_DEV_* dev, iblock_t iblock, uint32_t byte_off,uint32_t size);
struct _BUFFER_HEAD_* hl_blink(struct _BUFFER_HEAD_* _bh, uint32_t byte_off, uint32_t size);
int buf_lock(struct _BUFFER_HEAD_* bh, uint32_t start, uint32_t end);
int buf_try_lock(struct _BUFFER_HEAD_* bh, uint32_t start, uint32_t end);
int buf_unlock(struct _BUFFER_HEAD_* bh, uint32_t start);
int bfree(struct _BUFFER_HEAD_* bh);
int bsync(struct _LL_BLOCK_DEV_* dev);
u64 bclean();
static inline void bdirty(struct _BUFFER_HEAD_* bh) {
	if (bh->parent) bh = bh->parent;
	bh->status |= BH_DIRTY;
}



#endif