/*
	inc/fs.h
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
#include <stddef.h>
#include <spinlock.h>
#include <error.h>
#include <config.h>

#define FS_OPEN_READ		0x00000001
#define FS_OPEN_WRITE		0x00000002
#define FS_OPEN_N_CRATE		0x00000004
#define FS_OPEN_N_REPLACE	0x00000008
#define FS_OPEN_MONOPOLIZE	0x00000010


typedef void * LPCATALOG; 

struct _DISK_INFO_ {
	u32 size;
	u32 physical_sector_size;
	u32 logical_sector_size;
	u32 cache_block_size;
	u32 i_id;
	u64 logical_sector_count;
	wchar_t* path;
	GUID g_id;
};
struct _PART_INFO_ {
	u32 size;
	u32 physical_sector_size;
	u32 logical_sector_size;
	u32 cache_block_size;
	u32 i_disk;
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

typedef struct _STREAM_ {
	int gst;
	int mode;
	int lock;
	u64 read_pos;
	u64 write_pos;
	void* call_back_data;
	void(*call_back)(struct _DOPSTREAM_*, void*);
	struct _FCPEB_* fc;
	union{
		void * pdata[3];
		int64_t idata[3];
	} edata;
} STREAM,*LPSTREAM;

typedef struct _FSCTRL_{
	struct _DOPSTREAM_ * (*open)(wchar_t * name,int mode, struct _SIMPLE_PATH_* path);
	int (*open_path)(wchar_t * name,struct _SIMPLE_PATH_ * path);
	int (*close_path)(struct _SIMPLE_PAHT_ * path);
	int (*seek)(wchar_t* name, struct _SIMPLE_PATH_* dest_path,struct _SIMPLE_PATH_ * my_path);
	int (*exist)(wchar_t* name, struct _SIMPLE_PATH_* path);
	int (*get_attr)(wchar_t* name, void * attr, struct _SIMPLE_PATH_* path);
	int (*set_attr)(wchar_t* name, void* attr, struct _SIMPLE_PATH_* path);
	int (*create)(wchar_t* name, struct _SIMPLE_PATH_* path);
	int (*remove)(wchar_t* name, struct _SIMPLE_PATH_* path);
	int (*rename)(wchar_t* dname, wchar_t* sname, struct _SIMPLE_PATH_ * path);
	int (*cat)(LPCATALOG* buf, size_t size, struct _SIMPLE_PATH_* path);
	int (*link)(wchar_t* dname, wchar_t* sname, struct _SIMPLE_PATH_* path);
	void* data;
}FSCTRL,*LPFSCTRL;

typedef struct _SIMPLE_PATH_ {//a path,only kernel can hold this kind of file
	void* data;
	int idata0;
	int idata1;
	struct _FSCTRL_* fs;
} SIMPLE_PATH, * LPSIMPLE_PATH;

typedef struct _FCPEB_ {
	u64(*in)(LPSTREAM, u64 port);
	int (*out)(LPSTREAM, u64 port, u64 data);
	LPSTREAM(*open)(wchar_t* name, u64 mode, struct _FCPEB_*);
	int(*close)(LPSTREAM);
	int(*read)(LPSTREAM, size_t count, void* buf);
	int(*write)(LPSTREAM, size_t count, void* buf);
	int(*seek_get)(LPSTREAM, s64 pos, int org);
	int(*seek_put)(LPSTREAM, s64 pos, int org);
	u64(*tell_get)(LPSTREAM);
	u64(*tell_put)(LPSTREAM);
	int(*get)(LPSTREAM);
	int(*put)(LPSTREAM, int element);
	struct _BUFFER_HEAD_*(*bread)(LPSTREAM, u64 iblock);
	struct _BUFFER_HEAD_*(*cache_bwrite)(LPSTREAM, u64 iblock);
	int(*fresh_get)(LPSTREAM);
	int(*fresh_put)(LPSTREAM);
	u64(*get_size)(LPSTREAM);
	int(*set_size)(LPSTREAM, u64 size);
	u64(*get_count)(LPSTREAM);
	int(*set_count)(LPSTREAM, u64 count);
	int(*lock)(LPSTREAM);
	int(*unlock)(LPSTREAM);
	int(*lock_string)(LPSTREAM,uint64_t s,uint32_t l);
	int(*unlock_string)(LPSTREAM,uint64_t s);
	int(*info)(LPSTREAM,size_t size,void * info);
	void* data;
} FCPEB,*LPFCPEB;
int fs_map(const wchar_t * path,void * xc,struct _SIMPLE_PATH_ * f_path);
int fs_unmap(const wchar_t * path);

HANDLE summon_disk(const wchar_t * path);
int unsummon_disk(HANDLE _storage);

static u64 inline in(LPSTREAM file,u64 port){
	if(!file || !file->fc || !file->fc->in) return ERR_INVAILD_PTR;
	return file->fc->in(file,port);
}
static int inline out(LPSTREAM file,u64 port,u64 data){
	if(!file || !file->fc || !file->fc->out) return ERR_INVAILD_PTR;
	return file->fc->out(file,port,data);
}
static int close(LPSTREAM file){
	if(!file || !file->fc || !file->fc->close) return ERR_INVAILD_PTR;
	return file->fc->close(file);
}
static int inline read(LPSTREAM file,size_t count,void * buf){
	if(!file || !file->fc || !file->fc->read) return ERR_INVAILD_PTR;
	if(count && !buf) return -1;
	return file->fc->read(file,count,buf);
}
static int inline write(LPSTREAM file,size_t count,void * buf){
	if(!file || !file->fc || !file->fc->write) return ERR_INVAILD_PTR;
	if(count && !buf) return -1;
	return file->fc->write(file,count,buf);
}
static int inline seek_get(LPSTREAM file,s64 pos,int org){
	if(!file || !file->fc || !file->fc->seek_get) return ERR_INVAILD_PTR;
	return file->fc->seek_get(file,pos,org);
}
static int inline seek_put(LPSTREAM file,s64 pos,int org){
	if(!file || !file->fc || !file->fc->seek_put) return ERR_INVAILD_PTR;
	return file->fc->seek_put(file,pos,org);
}
static u64 inline tell_get(LPSTREAM file){
	if(!file || !file->fc || !file->fc->tell_get) return ERR_INVAILD_PTR;
	return file->fc->tell_get(file);
}
static u64 inline tell_put(LPSTREAM file){
	if(!file || !file->fc || !file->fc->tell_put) return ERR_INVAILD_PTR;
	return file->fc->tell_put(file);	
}
static int inline get(LPSTREAM file){
	if(!file || !file->fc || !file->fc->get) return ERR_INVAILD_PTR;
	return file->fc->get(file);
}
static int inline put(LPSTREAM file,int Element){
	if(!file || !file->fc || !file->fc->put) return ERR_INVAILD_PTR;
	return file->fc->put(file,Element);
}
static int inline fresh_get(LPSTREAM file){
	if(!file || !file->fc || !file->fc->fresh_get) return ERR_INVAILD_PTR;
	return file->fc->fresh_get(file);
}
static int inline fresh_put(LPSTREAM file){
	if(!file || !file->fc || !file->fc->fresh_put) return ERR_INVAILD_PTR;
	return file->fc->fresh_put(file);
}
static u64 inline get_size(LPSTREAM file){
	if(!file || !file->fc || !file->fc->get_size) return ERR_INVAILD_PTR;
	return file->fc->get_size(file);
}
static int inline set_size(LPSTREAM file,u64 size){
	if(!file || !file->fc || !file->fc->set_size) return ERR_INVAILD_PTR;
	return file->fc->set_size(file,size);
}
static u64 inline get_count(LPSTREAM file){
	if(!file || !file->fc || !file->fc->get_count) return ERR_INVAILD_PTR;
	return file->fc->get_count(file);	
}
static int inline set_count(LPSTREAM file,u64 count){
	if(!file || !file->fc || !file->fc->set_count) return ERR_INVAILD_PTR;
	return file->fc->set_count(file,count);
}
LPSTREAM open(const wchar_t * name,int mode,void * call_back_data, void(*call_back)(LPSTREAM, void*));
int seek(const wchar_t * name);
static int inline catalog(size_t size,LPCATALOG buf,LPSIMPLE_PATH cur_path){
	if(!cur_path || !cur_path->fs || !cur_path->fs->cat) return ERR_INVAILD_PTR;
	return cur_path->fs->cat(buf,size,cur_path);
}
static inline struct _BUFFER_HEAD_* bread(LPSTREAM dev, u64 iblock) {
	if (!dev || !(dev->fc) || !(dev->fc->bread)) return NULL;
	return dev->fc->bread(dev, iblock);
}
static inline struct _BUFFER_HEAD_* cache_bwrite(LPSTREAM dev, u64 iblock) {
	if (!dev || !(dev->fc) || !(dev->fc->cache_bwrite)) return NULL;
	return dev->fc->cache_bwrite(dev, iblock);
}
static int inline info(LPSTREAM file, size_t size, void* info) {
	if (!file || !file->fc || !file->fc->info) return -1;
	return file->fc->info(file, size, info);
}

LPSTREAM * alloc_file_table();
void free_file_table(LPSTREAM * table);
int insert_file(LPSTREAM * table,LPSTREAM file);
int remove_file(LPSTREAM * table,LPSTREAM file,int i);

#endif