/*
	inc/error.h
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

#ifndef _ERROR_H_
#define _ERROR_H_

enum ERROR_CODE {
	ERR_INVAILD_PTR	=	1,
	ERR_BAD_TYPE,
	ERR_NO_MORE_MEMORY,
	ERR_OUT_OF_RANGE,
	ERR_UNSUPPOSE_CHAR,
	ERR_UNSUPPOSE_MAP_TYPE,
	ERR_RESOURCE_BUSY,
	ERR_OUT_OF_TIME,
    ERR_BE_DESTORY,
    ERR_UNVAILD_SYSCALL
	
	
};

#define IsInvaildPtr(ptr)	(((u64)(ptr)) >= 0x100000ULL)



#endif