/*
	tools/dop/fs.c
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



#include <stdlib.h>
#define kmalloc(x,a)	malloc(x)
#define kfree(x)		free(x)
#include "fs.h"
#include "error.h"
#include <string.h>
#include <stddef.h>
#include <stdio.h>

#define FS_MAP_DIR_MIN_LEN	7

#define FS_MAP_DIR				0
#define FS_MAP_DEV				1
#define FS_MAP_FS				2
#define FS_MAP_INVAILD			3
#define FS_MAP_FREE				4


struct _DIR_ {
	union {
		struct {
			unsigned char len;
			unsigned char type;
			wchar_t name[FS_MAP_DIR_MIN_LEN];
		} min;
		struct {
			unsigned char len;
			unsigned char type;
			wchar_t* name;
		} max;
	};
	struct _DIR_* next;
	union {
		LPFCPEB fc;
		LPSIMPLE_FILE sf;
		struct _DIR_* son;
	};
};

#define DirName(dir)	(((dir)->min.len <= FS_MAP_DIR_MIN_LEN) ? (dir)->min.name : (dir)->max.name)

static LPDOPSTREAM root_open(wchar_t* name, u64 mode, LPSIMPLE_FILE path);

static struct _DIR_ root;
static struct _SIMPLE_FILE_ root_path;
static struct _DOPSTREAM_* stream_list = NULL;
static FSCTRL root_ctrl = {.open = root_open};

static LPDOPSTREAM root_open(wchar_t* _name, u64 mode, LPSIMPLE_FILE  _path) {
	wchar_t* path, * tpath,*name;
	struct _DIR_* cur, * first;
	size_t name_len;

	cur = _path->data;
	path = _name;
	while (1) {
		name = path;
		tpath = wcschr(path, L'\\');
		if (!tpath) break;
		name_len = tpath - path;
		first = cur;
		path = tpath + 1;
		while (1) {
			if (cur->min.type != FS_MAP_INVAILD && 
				cur->min.type != FS_MAP_FREE && 
				cur->min.len == name_len && 
				!wcsncmp(DirName(cur), name, name_len)) {
				if (cur->min.type == FS_MAP_FS) {
					if (!cur->sf || !cur->sf->fs || !cur->sf->fs->open) 
						return NULL;
					else return 
						cur->sf->fs->open(path, mode, cur->sf);
				}
				else if (cur->min.type == FS_MAP_DEV) {
					if (!cur->fc || !cur->fc->open)
						return NULL;
					else 
						return cur->fc->open(path, mode, cur->fc);
				}
				else break;
			}
			if (cur->next == first) 
				return NULL; 
			cur = cur->next;
		}
		cur = cur->son;
	}
	name_len = wcslen(path);
	first = cur;
	while (1) {
		if (cur->min.type != FS_MAP_INVAILD &&
			cur->min.type != FS_MAP_FREE && 
			cur->min.len == name_len && 
			!wcsncmp(DirName(cur), name, name_len)) {
			if (cur->min.type == FS_MAP_FS) {
				if (!cur->sf || !cur->sf->fs || !cur->sf->fs->open) 
					return NULL;
				else 
					return cur->sf->fs->open(path, mode, cur->sf);
			}
			else if (cur->min.type == FS_MAP_DEV) {
				if (!cur->fc || !cur->fc->open) 
					return NULL;
				else 
					return cur->fc->open(NULL, mode, cur->fc);
			}
			else 
				return NULL; 
		}
		if (cur->next == first) 
			return NULL; 
		cur = cur->next;
	}
	return NULL;
}
static size_t prep_path(wchar_t* d, const wchar_t* s) {
	wchar_t* _d;

	if (!s) return 0;
	_d = d;
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
							d += 3;
						}
						else {
							d--;
							while (*d != L'\\' && d != _d) d--;
						}
						break;
					}
				}
				else if (s[2] == L'/' || s[2] == L'\\') {// /./
					s += 2;
					break;
				}
			}
			*d++ = L'\\';
			s++;
			break;
		case L'\"':
			s++;
			break;
		case L',':
		case L';':
			return 0;
		default:
			*d++ = *s++;
		}
	}
	*d = 0;
	return d - _d;
}
int fs_map(const wchar_t* _path, void* xc, LPSIMPLE_FILE f_path) {
	wchar_t path_heap[PATH_MAX_LENGTH];
	wchar_t* path, * tpath, * name, * tname;
	size_t path_len, name_len;
	struct _DIR_* cur, * first, * ent, * ents, * ents2;
	int is_fs = 0;
	struct _DIR_* invaild = NULL;

	path_len = wcslen(_path);
	if (!path_len || path_len >= PATH_MAX_LENGTH) return -1;
	path_len = prep_path(path_heap, _path);
	if (!path_len) return -1;
	if (path_heap[path_len - 1] == L'\\') {
		path_heap[path_len - 1] = 0;
		is_fs = 1;
		if (!f_path) return -1;
	}
	path_heap[path_len] = 0;
	if (path_heap[0] == L'\\') path = path_heap + 1;
	else path = path_heap;
	cur = &root;
	while (1) {
		name = path;
		tpath = wcschr(path, L'\\');
		if (!tpath) break;
		name_len = tpath - path;
		path = tpath + 1;
		first = cur;
		while (1) {
			if (cur->min.type == FS_MAP_FREE && !invaild) {//need cirtal section
				cur->min.type = FS_MAP_INVAILD;
				invaild = cur;
			}
			else if (cur->min.type != FS_MAP_INVAILD) {
				if (cur->min.len == name_len &&
					!wcsncmp(DirName(cur), name, name_len)) {
					if (cur->min.type == FS_MAP_DIR) break;
					if (cur->min.type == FS_MAP_FS) return -1;
				}
				if (cur->next == first) {
					if (!invaild) { 
						if(!(ent = kmalloc(sizeof(struct _DIR_), 0))) 
							return -1; 
					}
					else ent = invaild;
					ent->min.len = (uint8_t)name_len;
					if (name_len <= FS_MAP_DIR_MIN_LEN) tname = ent->min.name;
					else if (!(tname = ent->max.name = kmalloc(sizeof(wchar_t) * name_len, 0))) { 
						if (!invaild) kfree(ent);
						else invaild->min.type = FS_MAP_INVAILD;
						return -1; 
					}
					wcsncpy(tname, name, name_len);
					if (!(ents = kmalloc(sizeof(struct _DIR_), 0))) {
						if (name_len > FS_MAP_DIR_MIN_LEN) kfree(ent->max.name);
						if (!invaild) kfree(ent);
						else invaild->min.type = FS_MAP_INVAILD;
						return -1;
					}
					ent->son = ents;
					ents->min.type = FS_MAP_DIR;
					ents->min.len = 2;
					ents->min.name[0] = ents->min.name[1] = L'.';
					ents->son = first;
					if (!(ents2 = kmalloc(sizeof(struct _DIR_), 0))) {
						kfree(ents);
						if (name_len > FS_MAP_DIR_MIN_LEN) kfree(ent->max.name);
						if (!invaild) kfree(ent);
						else invaild->min.type = FS_MAP_INVAILD;
						return -1;
					}
					ents->next = ents2;
					ents2->min.type = FS_MAP_DIR;
					ents2->min.len = 1;
					ents2->min.name[0] = L'.';
					ents2->next = ents;
					ents2->son = ents;
					if (!invaild) {
						ent->next = cur->next;
						cur->next = ent;
					}
					ent->min.type = FS_MAP_DIR;
					cur = ent;
					break;
				}
			}
			cur = cur->next;
		}
		invaild = NULL;
		cur = cur->son;
	}
	name_len = wcslen(name);
	first = cur;
	while (1) {
		if (cur->min.type == FS_MAP_FREE && !invaild) {
			cur->min.type = FS_MAP_INVAILD;
			invaild = cur;
		}
		else if (cur->min.type != FS_MAP_INVAILD) {
			if (cur->min.len == name_len &&
				!wcsncmp(DirName(cur), name, name_len)) {
				if (is_fs) {
					if (cur->min.type != FS_MAP_FS) return -1;
				}
				else {
					if (cur->min.type == FS_MAP_DEV) return -1;
				}
			}
		}
		if (cur->next == first) break;
		else cur = cur->next;
	}
	if (!invaild) { 
		if (!(ent = kmalloc(sizeof(struct _DIR_), 0))) return -1; 
	}
	else ent = invaild;
	ent->min.len = (uint8_t)name_len;
	if (name_len <= FS_MAP_DIR_MIN_LEN) tname = ent->min.name;
	else if (!(tname = ent->max.name = kmalloc(sizeof(wchar_t) * name_len, 0))) {
		if (invaild) invaild->min.type = FS_MAP_INVAILD;
		else kfree(ent);
		return -1;
	}
	wcsncpy(tname, name, name_len);
	if (is_fs) ent->fc = xc;
	else ent->sf = xc;
	if (!invaild) {
		ent->next = first;
		cur->next = ent;
	}
	if (is_fs) { 
		ent->min.type = FS_MAP_FS; 
		f_path->data = ent;
		f_path->fs = &root_ctrl;
	}
	else ent->min.type = FS_MAP_DEV;
	//wprintf(L"FS map:\\%s\n", path_heap[0] == L'\\' ? path_heap + 1 : path_heap);
	return 0;
}
int fs_unmap(const wchar_t* _path) {
	wchar_t path_heap[PATH_MAX_LENGTH];
	wchar_t* path, * tpath, * name;
	struct _DIR_* cur, * first;
	size_t name_len;
	int is_fs = 0;

	name_len = wcslen(_path);//wchar_t string len
	if (!name_len || name_len >= PATH_MAX_LENGTH) return -1;
	name_len = prep_path(path_heap, _path);
	if (!name_len) return -1;
	if (path_heap[name_len - 1] == L'\\') {//test weather path is a FS path
		path_heap[name_len - 1] = 0;
		is_fs = 1;
	}
	if (path_heap[0] == L'\\') path = path_heap + 1;
	else path = path_heap;
	cur = &root;
	while (1) {
		name = path;
		tpath = wcschr(path, L'\\');
		if (!tpath)
			break;
		name_len = tpath - path;
		first = cur;
		path = tpath + 1;
		while (1) {
			if (cur->min.type != FS_MAP_INVAILD && 
				cur->min.type != FS_MAP_FREE && 
				cur->min.len == name_len &&
				!wcsncmp(DirName(cur), name, name_len) && 
				cur->min.type == FS_MAP_DIR) break;
			if (cur->next == first) 
				return -1;
			cur = cur->next;
		}
		cur = cur->son;
	}
	name_len = wcslen(path);
	first = cur;
	while (1) {
		if (cur->min.len == name_len &&
			!wcsncmp(DirName(cur), name, name_len)) {
			if ((is_fs && cur->min.type == FS_MAP_FS) ||
				cur->min.type == FS_MAP_DEV) 
				break;
		}
		if (cur->next == first) return -1;
		cur = cur->next;
	}
	cur->min.type = FS_MAP_INVAILD;
	if (cur->min.len > FS_MAP_DIR_MIN_LEN) kfree(cur->max.name);
	cur->min.type = FS_MAP_FREE;
	return 0;
}
int close(LPDOPSTREAM file) {
	if (!file || !file->fc || !file->fc->close) return ERR_INVAILD_PTR;
	RemoveList(stream_list, file, p_prev, p_next);
	return file->fc->close(file);
}
LPDOPSTREAM open(const wchar_t* _name, u64 mode,void * call_back_data,void(*call_back)(LPDOPSTREAM,void*)) {
	wchar_t path_heap[PATH_MAX_LENGTH];
	size_t name_len;
	LPDOPSTREAM stream = NULL;

	name_len = wcslen(_name);
	if (!name_len || name_len >= PATH_MAX_LENGTH) return NULL;
	name_len = prep_path(path_heap, _name);
	if (!name_len) return NULL;
	if (path_heap[name_len - 1] == L'\\') return NULL;//can not open a path/dictionary
	if (path_heap[0] == L'\\') 
		stream = root_open(path_heap + 1, mode, &root_path);
	else if (cur_path && cur_path->fs && cur_path->fs->open) 
		stream = cur_path->fs->open(path_heap, mode, cur_path);
	if (stream) {
		InsertList(stream_list, stream, p_prev, p_next);
		stream->call_back = call_back;
		stream->call_back_data = call_back_data;
	}
	return stream;
}
void fs_init() {
	root.min.len = 1;
	root.min.type = FS_MAP_DIR;
	root.min.name[0] = L'.';
	root.next = &root;
	root.son = &root;
	root_path.data = &root;
	root_path.fs = &root_ctrl;
	cur_path = &root_path;
}

