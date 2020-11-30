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

#define FS_MAP_DIR	0
#define FS_MAP_DEV	1
#define FS_MAP_FS	3

#define FS_MAP_DIR_MIN_LEN	7

struct _DIR_ {
	union {
		struct {
			u8 len;
			u8 type;
			wchar_t name[FS_MAP_DIR_MIN_LEN];
		} min;
		struct {
			u8 len;
			u8 type;
			wchar_t * name;
		} max;
	};
	struct _DIR_ * next;
	union {
		LPFCPEB fc;
		LPPCPEB pc;
		struct _DIR_ * son;
	};
};

#define DirName(dir)	(((dir)->min.len <= FS_MAP_DIR_MIN_LEN) ? (dir)->min.name : (dir)->max.name)

static LPSTREAM root_open(wchar_t * name,u64 mode,FS_PATH * cur_path);
static int root_seek(wchar_t * path,struct _FS_PATH_ * dpath,struct _FS_PATH_ * cur_path);
static int root_catalog(wchar_t * path,LPCATALOG buf,struct _FS_PATH_ * cur_path);

static struct _DIR_ * root;
static int lock;
static PCPEB root_pc = {root_open,NULL,root_seek,root_catalog,NULL,NULL,NULL,NULL,NULL,NULL};
static FS_PATH root_path;

#define Lock()		spin_lock_bit(&lock,0)
#define TryLock()	spin_try_lock_bit(&lock,0)
#define Unlock()	spin_unlock_bit(&lock,0)

static LPSTREAM root_open(wchar_t * name,u64 mode,FS_PATH * cur_path){
	wchar_t *path,*tpath;
	struct _DIR_ * cur,*first;
	size_t name_len;
	
	cur = cur_path->data;
	path = name;
	SD();Lock();
	while(1){
			
		name = path;
		tpath = wcschr(path,L'\\');
		if(!tpath) break;
		name_len = tpath - path;
		first = cur;
		path = tpath + 1;
		//printk("L0,%P,%P,%d.\n",root,name,name_len);
		while(1){
			if(cur->min.len == name_len && !wcsncmp(DirName(cur),name,name_len)){
				if(cur->min.type == FS_MAP_FS) {
					Unlock(); SE();
					if(!cur->pc || !cur->pc->open1) return NULL;
					else return cur->pc->open1(path,mode,cur->pc);
				}
				else if(cur->min.type == FS_MAP_DEV){
					Unlock(); SE();
					if(!cur->fc || !cur->fc->open)return NULL;
					else return cur->fc->open(path,mode,cur->fc);
				}
				else break;
			}
			if(cur->next == first){ Unlock(); return NULL; }
			cur = cur->next;
		}
		cur = cur->son;
	}
	name_len = wcslen(path);
	first = cur;
	path = tpath + 1;
	while(1){
		if(cur->min.len == name_len && !wcsncmp(DirName(cur),name,name_len)){
			if(cur->min.type == FS_MAP_FS) {
				Unlock(); SE();
				if(!cur->pc || !cur->pc->open1) return NULL;
				else return cur->pc->open1(NULL,mode,cur->pc);
			}
			else if(cur->min.type == FS_MAP_DEV){
				Unlock(); SE();
				if(!cur->fc || !cur->fc->open) return NULL;
				else return cur->fc->open(NULL,mode,cur->fc);
			}
			else { SE(); Unlock(); return NULL; }
		}
		if(cur->next == first){SE(); Unlock(); return NULL; }
		cur = cur->next;
	}
	Unlock(); SE();
	return NULL;
}
static int root_seek(wchar_t * path,struct _FS_PATH_ * dpath,struct _FS_PATH_ * cur_path){
	wchar_t *tpath,*name;
	struct _DIR_ * cur,*prev,*first;
	size_t name_len;
	int ret;
	
	cur = cur_path->data;
	SD(); Lock();
	while(1){
		name = path;
		tpath = wcschr(path,L'\\');
		if(!tpath) break;
		name_len = tpath - path;
		first = cur;
		path = tpath + 1;
		while(1){
			if(cur->min.len == name_len && !wcsncmp(DirName(cur),name,name_len)){
				if(cur->min.type == FS_MAP_FS) {
					Unlock(); SE();
					if(!cur->pc || !cur->pc->seek) return -1;
					else return cur->pc->seek(path,dpath,cur->pc);
				}
				else if(cur->min.type == FS_MAP_DIR) break;
			}
			if(cur->next == first){ Unlock(); SE(); return -1; }
			cur = cur->next;
		}
		cur = cur->son;
	}
	name_len = wcslen(path);
	first = cur;
	path = tpath + 1;
	while(1){
		if(cur->min.len == name_len && !wcsncmp(DirName(cur),name,name_len)){
			if(cur->min.type == FS_MAP_FS) {
				Unlock(); SE();
				if(!cur->pc || !cur->pc->seek) return -1;
				else return cur->pc->seek(NULL,dpath,cur->pc);
			}
			else if(cur->min.type == FS_MAP_DIR){
				Unlock(); SE();
				dpath->data = cur->son;
				dpath->pc = &root_pc;
				return 0;
			}
			else {SE(); Unlock(); return -1;}
		}
		if(cur->next == first){SE(); Unlock(); return -1; }
		cur = cur->next;
	}
	Unlock();SE(); return -1;
}
static int root_catalog(wchar_t * path,LPCATALOG buf,struct _FS_PATH_ * cur_path){
	wchar_t *name,*tpath;
	struct _DIR_ * cur,*first;
	size_t name_len;
	
	cur = cur_path->data;
	SD(); Lock();
	while(1){
		name = path;
		tpath = wcschr(path,L'\\');
		if(!tpath) break;
		name_len = tpath - path;
		first = cur;
		path = tpath + 1;
		while(1){
			if(cur->min.len == name_len && !wcsncmp(DirName(cur),name,name_len)){
				if(cur->min.type == FS_MAP_FS) {
					Unlock(); SE();
					if(!cur->pc || !cur->pc->catalog1) return -1;
					else return cur->pc->catalog1(path,buf,cur->pc);
				}
				else if(cur->min.type == FS_MAP_DEV){
					Unlock();
					SE();
					return -1;
				}
				else break;
			}
			if(cur->next == first){ Unlock(); SE(); return -1; }
			cur = cur->next;
		}
		cur = cur->son;
	}
	name_len = wcslen(path);
	first = cur;
	path = tpath + 1;
	while(1){
		if(cur->min.len == name_len && !wcsncmp(DirName(cur),name,name_len)){
			if(cur->min.type == FS_MAP_FS) {
				Unlock(); SE();
				if(!cur->pc || !cur->pc->catalog1) return NULL;
				else return cur->pc->catalog1(NULL,buf,cur->pc);
			}
			else if(cur->min.type == FS_MAP_DIR){
				Unlock(); SE();
				return 0;
			}
			else { Unlock(); SE();return NULL; }
		}
		if(cur->next == first){ Unlock(); SE();return NULL; }
		cur = cur->next;
	}
	Unlock();
	SE();
	return NULL;
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
int fs_map(const wchar_t * _path,void * xc){
	wchar_t * path_heap;
	wchar_t * path,*tpath,*name,*tname;
	size_t path_len,name_len;
	struct _DIR_ * cur,*first,*ent,*ents,*ents2;
	int is_fs = 0;
	
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
	cur = root;
	SD(); Lock();
	while(1){
		name = path;
		tpath = wcschr(path,L'\\');
		if(!tpath) break;
		name_len = tpath - path;
		path = tpath + 1;
		first = cur;
		while(1){
			
			if(cur->min.len == name_len && 
				!wcsncmp(DirName(cur),name,name_len)){
				if(cur->min.type == FS_MAP_DIR) break;
				if(cur->min.type == FS_MAP_FS){
					Unlock(); SE();
					kfree(path_heap);
					return ERR_UNSUPPOSE_MAP_TYPE;
				}
			}
			if(cur->next == first){
				ent = kmalloc(sizeof(struct _DIR_),0);
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
				ent->next = cur->next;
				cur->next = ent;
			}
			else cur = cur->next;
		}
		cur = cur->son;
	}
	name_len = wcslen(name);
	first = cur;
	while(1){
		if(cur->min.len == name_len &&
			!wcsncmp(DirName(cur),name,name_len)){
			if(is_fs){
				if(cur->min.type != FS_MAP_DEV){
					Unlock(); SE();
					kfree(path_heap);
					return ERR_UNSUPPOSE_MAP_TYPE;
				}
			}
			else{
				if(cur->min.type == FS_MAP_DEV){
					Unlock(); SE();
					kfree(path_heap);
					return ERR_UNSUPPOSE_MAP_TYPE;
				}
			}
		}
		if(cur->next == first) break;
		else cur = cur->next;
	}
	ent = kmalloc(sizeof(struct _DIR_),0);
	ent->min.len = name_len;
	if(is_fs) ent->min.type = FS_MAP_FS;
	else ent->min.type = FS_MAP_DEV;
	if(name_len <= FS_MAP_DIR_MIN_LEN) tname = ent->min.name;
	else tname = ent->max.name = kmalloc(sizeof(wchar_t) * name_len,0);
	wcsncpy(tname,name,name_len);
	if(is_fs) ent->fc = xc;
	else ent->pc = xc;
	ent->next = first;
	cur->next = ent;
	Unlock(); SE();
	wprintf(L"FS map:\\%s\n",path_heap[0] == L'\\' ? path_heap + 1 : path_heap);
	kfree(path_heap);
	return 0;
}
void * fs_unmap(const wchar_t * _path){
	wchar_t * path_heap,*path,*tpath,*name;
	struct _DIR_ * cur,*prev,*first;
	size_t name_len;
	int is_fs = 0;
	void * xc;
	
	name_len = wcslen(path);
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
	cur = root;
	SD(); Lock();
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
				Unlock(); SE();
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
			Unlock(); SE();
			kfree(path_heap);
			return -1;
		}
		prev = cur;
		cur = cur->next;
	}
#ifdef DEBUG
	if(!prev) print("BUG:fs_unmap().\n");
#endif
	prev->next = cur->next;
	Unlock(); SE();
	kfree(path_heap);
	if(cur->min.len > FS_MAP_DIR_MIN_LEN) kfree(cur->max.name);
	if(is_fs) xc = cur->fc;
	else xc = cur->pc;
	kfree(cur);
	return xc;
}
int close(LPSTREAM file){
	struct _PROCESS_ * process;
#ifdef CHECK
	if(!file || !file->fc || !file->fc->close) return ERR_INVAILD_PTR;
#endif
	LockProcessField(process = GetCurProcess());
	if(file->next) file->next->prev = file->prev;
	if(file->prev) file->prev->next = file;
	if(file == process->file) process->file = file->next;
	UnlockProcessField(process);
	return file->fc->close(file);
}
LPSTREAM open(const wchar_t * _name,u64 mode){
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
	if(path_heap[0] != L'\\'){
		if(!GetCurProcess()->cur_path.pc || 
			!GetCurProcess()->cur_path.pc->open0) {
			kfree(path_heap);
			return NULL;
		}
		else stream = GetCurProcess()->cur_path.pc->open0(path_heap,mode,&(GetCurProcess()->cur_path));
	}
	else stream = root_open(path_heap,mode,&root_path);
	kfree(path_heap);
	if(stream){
		stream->prev = NULL;
		LockProcessField(process = GetCurProcess());
		if(process->file){
			stream->next = process->file;
			process->file->prev = stream;
			process->file = stream;
		}
		else{
			process->file = stream;
			stream->next = NULL;
		}
		UnlockProcessField(process);
	}
	return stream;
}
int seek(const wchar_t * _name,struct _FS_PATH_ * dpath){
	wchar_t * path_heap;
	size_t name_len;
	int ret;

	if(!dpath) dpath = &(GetCurProcess()->cur_path);
	name_len = wcslen(_name);
	path_heap = kmalloc(sizeof(wchar_t) * name_len + sizeof(wchar_t),0);
	name_len = prep_path(path_heap,_name,NULL,NULL);
	if(!name_len) {
		kfree(path_heap);
		return 0;
	}
	if(name_len == 1 && *path_heap == L'\\'){
		dpath->pc = &root_pc;
		dpath->data = root;
		kfree(path_heap);
		return 0;
	}
	if(path_heap[name_len - 1] == L'\\') path_heap[name_len - 1] = 0;
	if(path_heap[0] == L'\\') ret = root_seek(path_heap + 1,dpath,&root_path);
	else {
		if(!GetCurProcess()->cur_path.pc || 
			!GetCurProcess()->cur_path.pc->seek)ret = root_seek(path_heap,dpath,&root_path);
		else GetCurProcess()->cur_path.pc->seek(path_heap,dpath,&(GetCurProcess()->cur_path));
	}
	kfree(path_heap);
	return ret;
}
int catalog(const wchar_t * _name,LPCATALOG buf){
	wchar_t * path_heap;
	size_t name_len;
	int ret;
	
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
	if(path_heap[0] != L'\\'){
		if(!GetCurProcess()->cur_path.pc || 
			!GetCurProcess()->cur_path.pc->catalog0) {
			kfree(path_heap);
			return NULL;
		}
		else ret = GetCurProcess()->cur_path.pc->catalog0(path_heap,buf,&(GetCurProcess()->cur_path));
	}
	else ret = root_catalog(path_heap + 1,buf,&root_path);
	kfree(path_heap);
	return ret;
}
void fs_init(){
	root = kmalloc(sizeof(struct _DIR_),8);
	root->min.len = 1;
	root->min.type = FS_MAP_DIR;
	root->min.name[0] = L'.';
	root->next = root;
	root->son = root;
	root_path.data = root;
	root_path.pc = &root_pc;
	lock = 0;
}

