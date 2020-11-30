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
#include <kernel.h>
#include <spinlock.h>
#include <error.h>
typedef void * LPCATALOG; 

typedef struct _STREAM_ {
	int gst;
	int flags;
	u64 read_pos;
	u64 write_pos;
	void * data;
	struct _FCPEB_ * fc;
	struct _STREAM_ * prev;
	struct _STREAM_ * next;
} STREAM,*LPSTREAM;

typedef struct _FS_PATH_ {
	void * data;
	struct _PCPEB_ * pc;
} FS_PATH;
typedef struct _PCPEB_ {
	LPSTREAM (*open0)(wchar_t * name,u64 mode,struct _FS_PATH_ *);//cur_process->cur_path有效且访问相对路径使使用此入口
	LPSTREAM (*open1)(wchar_t * name,u64 mode,struct _PCPEB_*);//访问绝对路径时用此入口
	int (*seek)(wchar_t * path,struct _FS_PATH_ * dpath,struct _FS_PATH_ * cur_path);
	int (*catalog0)(wchar_t * path,LPCATALOG buf,struct _FS_PATH_ * cur_path);
	int (*catalog1)(wchar_t * path,LPCATALOG buf,struct _PCPEB_*);
	int (*remove)(wchar_t * path,const wchar_t * name);
	int (*rename)(wchar_t * path,const wchar_t * dest_name,const wchar_t * scr_name);
	int (*Link)(wchar_t * path,const wchar_t * dest_name,const wchar_t * scr_Name);
	int (*Unlink)(wchar_t * path,const wchar_t * name);
	void * data;
} PCPEB,*LPPCPEB;


typedef struct _FCPEB_ {
	u64 (*in)(LPSTREAM,u64 port);
	int (*out)(LPSTREAM,u64 port,u64 data);
	LPSTREAM (*open)(wchar_t * name,u64 mode,struct _FCPEB_ *);
	int(*close)(LPSTREAM);
	int(*read)(LPSTREAM,size_t count,void * buf);
	int(*write)(LPSTREAM,size_t count,void * buf);
	int(*seek_get)(LPSTREAM,s64 pos,int org);
	int(*seek_put)(LPSTREAM,s64 pos,int org);
	u64(*tell_get)(LPSTREAM);
	u64(*tell_put)(LPSTREAM);
	int(*get)(LPSTREAM);
	int(*put)(LPSTREAM,int element);
	int(*read_block)(LPSTREAM,u64 iblock,void * pblock,size_t count);
	int(*write_block)(LPSTREAM,u64 iblock,void * pblock,size_t count);
	int(*fresh_get)(LPSTREAM);
	int(*fresh_put)(LPSTREAM);
	u64(*get_size)(LPSTREAM);
	int(*set_size)(LPSTREAM,u64 size);
	u64(*get_count)(LPSTREAM);
	int(*set_count)(LPSTREAM,u64 count);
	void * data;
} FCPEB,*LPFCPEB;

u64 inline in(LPSTREAM file,u64 port){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->in) return ERR_INVAILD_PTR;
#endif
	return file->fc->in(file,port);
}
int inline out(LPSTREAM file,u64 port,u64 data){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->out) return ERR_INVAILD_PTR;
#endif
	return file->fc->out(file,port,data);
}
int close(LPSTREAM file);
static int inline read(LPSTREAM file,size_t count,void * buf){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->read) return ERR_INVAILD_PTR;
	if(count && !buf) return -1;
#endif
	return file->fc->read(file,count,buf);
}
static int inline write(LPSTREAM file,size_t count,void * buf){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->write) return ERR_INVAILD_PTR;
	if(count && !buf) return -1;
#endif
	return file->fc->write(file,count,buf);
}
int inline seek_get(LPSTREAM file,s64 pos,int org){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->seek_get) return ERR_INVAILD_PTR;
#endif
	return file->fc->seek_get(file,pos,org);
}
int inline seek_put(LPSTREAM file,s64 pos,int org){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->seek_put) return ERR_INVAILD_PTR;
#endif
	return file->fc->seek_put(file,pos,org);
}
u64 inline tell_get(LPSTREAM file){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->tell_get) return ERR_INVAILD_PTR;
#endif
	return file->fc->tell_get(file);
}
u64 inline tell_put(LPSTREAM file){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->tell_put) return ERR_INVAILD_PTR;
#endif
	return file->fc->tell_put(file);	
}
int inline get(LPSTREAM file){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->get) return ERR_INVAILD_PTR;
#endif
	return file->fc->get(file);
}
int inline put(LPSTREAM file,int Element){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->put) return ERR_INVAILD_PTR;
#endif
	return file->fc->put(file,Element);
}
int inline read_block(LPSTREAM file,u64 iblock,void * pblock,size_t count){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->read_block) return ERR_INVAILD_PTR;
	if(count && !pblock) return -1;
#endif
	return file->fc->read_block(file,iblock,pblock,count);
}
int inline write_block(LPSTREAM file,u64 iblock,void * pblock,size_t count){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->write_block) return ERR_INVAILD_PTR;
	if(count && !pblock) return -1;
#endif
	return file->fc->write_block(file,iblock,pblock,count);
}
int inline fresh_get(LPSTREAM file){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->fresh_get) return ERR_INVAILD_PTR;
#endif
	return file->fc->fresh_get(file);
}
int inline fresh_put(LPSTREAM file){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->fresh_put) return ERR_INVAILD_PTR;
#endif
	return file->fc->fresh_put(file);
}
u64 inline get_size(LPSTREAM file){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->get_size) return ERR_INVAILD_PTR;
#endif
	return file->fc->get_size(file);
}
int inline set_size(LPSTREAM file,u64 size){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->set_size) return ERR_INVAILD_PTR;
#endif
	return file->fc->set_size(file,size);
}
u64 inline get_count(LPSTREAM file){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->get_count) return ERR_INVAILD_PTR;
#endif
	return file->fc->get_count(file);	
}
int inline set_count(LPSTREAM file,u64 count){
#ifdef CHECK
	if(!file || !file->fc || !file->fc->set_count) return ERR_INVAILD_PTR;
#endif
	return file->fc->set_count(file,count);
}
int fs_map(const wchar_t * path,void * xc);
void * fs_unmap(const wchar_t * path);
LPSTREAM open(const wchar_t * name,u64 mode);
int seek(const wchar_t * name,struct _FS_PATH_ * dpath);
int catalog(const wchar_t * name,LPCATALOG buf);

#define STREAM_FIELD_BUSY	0
#define STREAM_LINK_BUSY	1

spin_optr_struct_member_bit(StreamField,struct _STREAM_,flags,STREAM_FIELD_BUSY);
spin_optr_struct_member_bit(StreamList,struct _STREAM_,flags,STREAM_LINK_BUSY);


#endif