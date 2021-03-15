/*
	fs/open.c
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

#include <mm.h>
#include <string.h>
#include <stddef.h>
#include <asm.h>
#include <stdio.h>
#include <kernel.h>
#include <config.h>
#include <fs.h>
#include <fctrl.h>

#define FS_MAP_DIR				0
#define FS_MAP_DEV				1
#define FS_MAP_FS				2
#define FS_MAP_INVAILD			3
#define FS_MAP_FREE				4

#define FS_MAP_DIR_MIN_LEN	7

struct _DIR_ {
	union {
		struct {
			u8 len, type;
			wchar_t name[FS_MAP_DIR_MIN_LEN];
		} min;
		struct {
			u8 len, type;
			wchar_t * name;
		} max;
	};
	volatile struct _DIR_ * next;
	union {
		LPFCPEB fc;
		LPSIMPLE_PATH path;
		struct _DIR_ * son;
	};
};

#define DirName(dir)	(((dir)->min.len <= FS_MAP_DIR_MIN_LEN) ? (dir)->min.name : (dir)->max.name)

static LPSTREAM root_open(wchar_t * name,u64 mode,LPSIMPLE_PATH cur_path);
static int root_seek(wchar_t * path,LPSIMPLE_PATH dest_path,LPSIMPLE_PATH my_path);
static int root_cat(LPCATALOG buf,size_t size,LPSIMPLE_PATH cur_path);
static int root_exist(wchar_t * path,LPSIMPLE_PATH cur_path);

static LPSTREAM * stream_zone = NULL;
static struct _DIR_ root = {
	.min.len = 1,
	.min.type = FS_MAP_DIR,
	.min.name[0] = L'.',
	.next = &root,
	.son = &root
};
static FSCTRL root_ctrl = {.open = root_open,.seek = root_seek,.exist = root_exist,.cat = root_cat};
static SIMPLE_PATH root_path = {.data = &root,.fs = &root_ctrl};
static volatile uint16_t create_lock = 0;

static LPSTREAM root_open(wchar_t * name,u64 mode,LPSIMPLE_PATH cur_path){
	wchar_t *path,*tpath;
	struct _DIR_ * cur,*first;
	size_t name_len;
	
	cur = cur_path->data;
	path = name;
	while(1){
		name = path;
		tpath = wcschr(path,L'\\');
		if(!tpath) break;
		name_len = tpath - path;
		first = cur;
		path = tpath + 1;
		while(1){
			if(cur->min.type != FS_MAP_FREE && 
				cur->min.type != FS_MAP_INVAILD && 
				cur->min.len == name_len && 
				!wcsncmp(DirName(cur),name,name_len)){
				if(cur->min.type == FS_MAP_FS) {
					if(!cur->path || !cur->path->fs || !cur->path->fs->open) return NULL;
					else return cur->path->fs->open(path,mode,cur->path);
				}
				else if(cur->min.type == FS_MAP_DEV){
					if(!cur->fc || !cur->fc->open) return NULL;
					else return cur->fc->open(path,mode,cur->fc);
				}
				else break;
			}
			if(cur->next == first) return NULL; 
			cur = cur->next;
		}
		cur = cur->son;
	}
	name_len = wcslen(path);
	first = cur;
	path = tpath + 1;
	while(1){
		if(cur->min.type != FS_MAP_FREE && 
			cur->min.type != FS_MAP_INVAILD && 
			cur->min.len == name_len && 
			!wcsncmp(DirName(cur),name,name_len)){
			if(cur->min.type == FS_MAP_FS) {
				if(!cur->path || !cur->path->fs || !cur->path->fs->open) return NULL;
				else return cur->path->fs->open(NULL,mode,cur->path);
			}
			else if(cur->min.type == FS_MAP_DEV){
				if(!cur->fc || !cur->fc->open) return NULL;
				else return cur->fc->open(NULL,mode,cur->fc);
			}
			else return NULL; 
		}
		if(cur->next == first) return NULL; 
		cur = cur->next;
	}
	return NULL;
}
static int root_seek(wchar_t * path,LPSIMPLE_PATH dest_path,LPSIMPLE_PATH my_path){
	wchar_t *tpath,*name;
	struct _DIR_ * cur,*prev,*first;
	size_t name_len;
	int ret;
	
	cur = my_path->data;
	while(1){
		name = path;
		tpath = wcschr(path,L'\\');
		if(!tpath) break;
		name_len = tpath - path;
		first = cur;
		path = tpath + 1;
		while(1){
			if(cur->min.type != FS_MAP_FREE &&
				cur->min.type != FS_MAP_INVAILD &&
				cur->min.len == name_len && 
				!wcsncmp(DirName(cur),name,name_len)){
				if(cur->min.type == FS_MAP_FS) {
					if(!cur->path || !cur->path->fs || !cur->path->fs->seek) return -1;
					else return cur->path->fs->seek(path,dest_path,cur->path);
				}
				else if(cur->min.type == FS_MAP_DIR) break;
			}
			if(cur->next == first) return -1; 
			cur = cur->next;
		}
		cur = cur->son;
	}
	name_len = wcslen(path);
	first = cur;
	path = tpath + 1;
	while(1){
		if(cur->min.type != FS_MAP_FREE &&
			cur->min.type != FS_MAP_INVAILD &&
			cur->min.len == name_len && 
			!wcsncmp(DirName(cur),name,name_len)){
			if(cur->min.type == FS_MAP_FS) {
				if(!cur->path || !cur->path || !cur->path->fs || !cur->path->fs->seek) return -1;
				else return cur->path->fs->seek(NULL,dest_path,cur->path);
			}
			else if(cur->min.type == FS_MAP_DIR){
				if(dest_path->fs && dest_path->fs->close_path) 
					dest_path->fs->close_path(dest_path);
				dest_path->data = cur->son;
				dest_path->fs = &root_path;
				return 0;
			}
			else return -1;
		}
		if(cur->next == first) return -1; 
		cur = cur->next;
	}
	return -1;
}
static int root_cat(LPCATALOG buf,size_t size,LPSIMPLE_PATH cur_path){
	
	return 0;
}
static int root_exist(wchar_t * path,LPSIMPLE_PATH cur_path){

	return 0;
}
static size_t prep_path(wchar_t * d,const wchar_t * _s0,const wchar_t * _s1,const wchar_t * _s2){
	wchar_t * _d;
	int i;
	wchar_t * s;
	
	_d = d;
	for(i = 0;i < 3;i++){
		if(i == 0) s = _s0;
		else if(i == 1) s = _s1;
		else s = _s2;
		if(!s) break;
		while (*s) {
			switch (*s) {
			case L'\\':
			case L'/':
				if (s[1] == L'.') {
					if (s[2] == L'.') {
						if (s[3] == L'/' || s[3] == L'\\') {// /../
							s += 3;
							if (d == _d) {
								d[0] = L'\\';
								d[1] = d[2] = L'.';
								d+=3;
							}
							else d--;
							while (*d != L'/') {
								if (d == _d) {
									d[0] = L'\\';
									d[1] = d[2] = L'.';
									d += 3;
								}
								d--;
							}
							break;
						}
					}
					else if (s[2] == L'/' || s[2] == L'\\') {// /./
						s += 2;
						break;
					}
				}
				*d = L'\\';
				d++;
				s++;
				break;
			case L'\"':
				s++;
				break;
			case L',':
			case L';':
				return 0;
			default:
				*d = *s;
				s++;
				d++;
			}
		}	
	}
	*d = 0;
	return d - _d;
}
int fs_map(const wchar_t * _path,void * xc,struct _SIMPLE_PATH_ * f_path){
	wchar_t * path_heap;
	wchar_t * path,*tpath,*name,*tname;
	size_t path_len,name_len;
	struct _DIR_ * cur,*first,*ent,*ents,*ents2;
	int is_fs = 0;
	struct _DIR_ * invaild = NULL;
	int hash;
	
	path_len = wcslen(_path);
	path_heap = kmalloc(sizeof(wchar_t) * path_len + sizeof(wchar_t),0);
	path_len = prep_path(path_heap,_path,NULL,NULL);
	if(!path_len){
		kfree(path_heap);
		return ERR_UNSUPPOSE_CHAR;
	}
	if(path_heap[path_len - 1] == L'\\') {
		path_heap[path_len - 1] = 0;
		is_fs = 1;
	}
	if(path_heap[0] == L'\\') path = path_heap + 1;
	else path = path_heap;
	cur = &root;
	while(1){
		name = path;
		hash = name[0];
		hash ^= hash >> 8;
		hash ^= hash >> 4;
		hash &= 0x0f;
		tpath = wcschr(path,L'\\');
		if(!tpath) break;
		name_len = tpath - path;
		path = tpath + 1;
		first = cur;
retry1:
		while(1){
			if(cur->min.type != FS_MAP_INVAILD){
				if (cur->min.type == FS_MAP_FREE && !invaild) {//need cirtal section
					if(!cmpxchg1b(&cur->min.type,FS_MAP_FREE,FS_MAP_INVAILD,NULL))
						invaild = cur;
				}
				else{
					if(cur->min.len == name_len && 
						!wcsncmp(DirName(cur),name,name_len)){
						if(cur->min.type == FS_MAP_DIR) break;
						if(cur->min.type == FS_MAP_FS){
							kfree(path_heap);
							return ERR_UNSUPPOSE_MAP_TYPE;
						}
					}
				}
			}
			if(cur->next == first){
				while(spin_try_lock_bit(&create_lock,hash)){
					while(create_lock & (1 << hash)) pause();
					if(first != cur->next){
						cur = first;
						goto retry1;
					}
				}
				if(!invaild) ent = kmalloc(sizeof(struct _DIR_),0);
				else ent = invaild;
				ent->min.len = name_len;
				ent->min.type = FS_MAP_DIR;
				if(name_len <= FS_MAP_DIR_MIN_LEN) tname = ent->min.name;
				else tname = ent->max.name = kmalloc(sizeof(wchar_t) * name_len,0);
				wcsncpy(tname,name,name_len);
				ent->son = ents = kmalloc(sizeof(struct _DIR_),0);
				ents->min.type = FS_MAP_DIR;
				ents->min.len = 2;
				ents->min.name[0] = ents->min.name[1] = L'.';
				ents->son = first;
				ents->next = ents2 = kmalloc(sizeof(struct _DIR_),0);
				ents2->min.type = FS_MAP_DIR;
				ents2->min.len = 1;
				ents2->min.name[0] = L'.';
				ents2->next = ents;
				ents2->son = ents;
				if(invaild) invaild = NULL;
				else {
					ent->next = first;
					xchgq(&cur->next,ent);
				}
				spin_unlock_bit(&create_lock,hash);
				cur = ent;
				break;
			}
			cur = cur->next;
		}
		cur = cur->son;
		if(invaild) {
			invaild->min.type = FS_MAP_FREE;
			invaild = NULL;
		}
	}
	name_len = wcslen(name);
	first = cur;
retry2:
	while(1){
		if(cur->min.type != FS_MAP_INVAILD){
			if (cur->min.type == FS_MAP_FREE && !invaild) {//need cirtal section
				if(!cmpxchg1b(&cur->min.type,FS_MAP_FREE,FS_MAP_INVAILD,NULL))
					invaild = cur;
			}
			else{
				if(cur->min.len == name_len && 
					!wcsncmp(DirName(cur),name,name_len)){
					if(is_fs){
						if(cur->min.type == FS_MAP_FS ||
							cur->min.type == FS_MAP_DIR){
							kfree(path_heap);
							return ERR_UNSUPPOSE_MAP_TYPE;
						}
						break;
					}
					if(cur->min.type == FS_MAP_DIR || 
						cur->min.type == FS_MAP_FS) break;
					kfree(path_heap);
					return ERR_UNSUPPOSE_MAP_TYPE;
				}
			}
		}
		if(cur->next == first) break;
	 	cur = cur->next;
	}
	while(spin_try_lock_bit(&create_lock,hash)){
		while(create_lock & (1 << hash)) pause();
		if(first != cur->next){
			cur = first;
			goto retry2;
		}
	}
	if(invaild) ent = invaild;
	else ent = kmalloc(sizeof(struct _DIR_),0);
	ent->min.len = name_len;
	if(is_fs) ent->min.type = FS_MAP_FS;
	else ent->min.type = FS_MAP_DEV;
	if(name_len <= FS_MAP_DIR_MIN_LEN) tname = ent->min.name;
	else tname = ent->max.name = kmalloc(sizeof(wchar_t) * name_len,0);
	wcsncpy(tname,name,name_len);
	if(is_fs) ent->fc = xc;
	else ent->path = xc;
	if(!invaild) {
		ent->next = first;
		xchgq(&cur->next,ent);
	}
	spin_unlock_bit(&create_lock,hash);
	wprintk(L"FS map:\\%s\n",path_heap[0] == L'\\' ? path_heap + 1 : path_heap);
	kfree(path_heap);
	return 0;
}
int fs_unmap(const wchar_t * _path){
	wchar_t * path_heap,*path,*tpath,*name;
	struct _DIR_ * cur,*first;
	size_t name_len;
	int is_fs = 0;
	
	name_len = wcslen(_path);
	path_heap = kmalloc(sizeof(wchar_t) * name_len + sizeof(wchar_t),0);
	name_len = prep_path(path_heap,_path,NULL,NULL);
	if(!name_len) {
		kfree(path_heap);
		return NULL;
	}
	if(path_heap[name_len - 1] == L'\\') {
		path_heap[name_len - 1] = 0;
		is_fs = 1;
	}
	if(path_heap[0] == L'\\') path = path_heap + 1;
	else path = path_heap;
	cur = &root;
	while(1){
		name = path;
		tpath = wcschr(path,L'\\');
		if(!tpath) break;
		name_len = tpath - path;
		first = cur;
		path = tpath + 1;
		while(1){
			if(cur->min.len == name_len &&
				!wcsncmp(DirName(cur),name,name_len)){
				if(cur->min.type == FS_MAP_DIR) break;		
			}
			if(cur->next == first){
				kfree(path_heap);
				return -1;
			}
			cur = cur->next;
		}
		cur = cur->son;
	}
	name_len = wcslen(path);
	first = cur;
#ifdef DEBUG
	prev = NULL;
#endif
	while(1){
		if(cur->min.len == name_len && 
			!wcsncmp(DirName(cur),name,name_len)){
			if((is_fs && cur->min.type == FS_MAP_FS) ||
				cur->min.type == FS_MAP_DEV) break;
		}
		if(cur->next == first){
			kfree(path_heap);
			return -1;
		}
		cur = cur->next;
	}
#ifdef DEBUG
	if(!prev) print("BUG:fs_unmap().\n");
#endif
	cur->min.type = FS_MAP_FREE;
	kfree(path_heap);
	if(cur->min.len > FS_MAP_DIR_MIN_LEN) kfree(cur->max.name);
	kfree(cur);
	return 0;
}
LPSTREAM open(const wchar_t * _name,int mode,void * call_back_data, void(*call_back)(LPSTREAM, void*)){
	wchar_t * path_heap;
	size_t name_len;
	LPSTREAM stream;
	LPPROCESS process;
	
	name_len = wcslen(_name);
	path_heap = kmalloc(sizeof(wchar_t) * name_len + sizeof(wchar_t),0);
	name_len = prep_path(path_heap,_name,NULL,NULL);
	if(!name_len) {
		kfree(path_heap);
		return NULL;
	}
	if(path_heap[name_len - 1] == L'\\') {
		kfree(path_heap);
		return NULL;
	}
	process =  GetCurProcess();
	if(path_heap[0] != L'\\'){
		if(!process->cur_path.fs || 
			!process->cur_path.fs->open) {
			kfree(path_heap);
			return NULL;
		}
		else {
			stream = process->cur_path.fs->open(path_heap,mode,&process->cur_path);
		}
	}
	else {
		stream = root_open(path_heap + 1,mode,&root_path);
	}
	if(stream){
		stream->call_back = call_back;
		stream->call_back_data = call_back_data;
	}
	kfree(path_heap);
	return stream;
}
int seek(const wchar_t * _name){
	wchar_t * path_heap;
	size_t name_len;
	LPPROCESS process;
	SIMPLE_PATH my_path;
	int ret;

	process = GetCurProcess();
	name_len = wcslen(_name);
	path_heap = kmalloc(sizeof(wchar_t) * name_len + sizeof(wchar_t),0);
	name_len = prep_path(path_heap,_name,NULL,NULL);
	if(!name_len) {
		kfree(path_heap);
		return 0;
	}
	if(name_len == 1 && *path_heap == L'\\'){
		if(process->cur_path.fs && process->cur_path.fs->close_path)
			process->cur_path.fs->close_path(&process->cur_path);
		process->cur_path.fs = &root_ctrl;
		process->cur_path.data = &root;
		kfree(path_heap);
		return 0;
	}
	if(path_heap[name_len - 1] == L'\\') path_heap[name_len - 1] = 0;
	if(path_heap[0] == L'\\') ret = root_seek(path_heap + 1,&process->cur_path,&root_path);
	else {
		if(!process->cur_path.fs || 
			!process->cur_path.fs->seek)
			ret = root_seek(path_heap,&process->cur_path,&root_path);
		else ret = process->cur_path.fs->seek(path_heap,&process->cur_path,&process->cur_path);
	}
	kfree(path_heap);
	return ret;
}

