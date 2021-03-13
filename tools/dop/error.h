#ifndef _ERROR_H_
#define _ERROR_H_

/*
	tools/dop/error.h
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

enum {
	IllPathPrex = 1,
	FailPrepPath,
	UnsuppInMap,
	DiskSeekOverflow,
	ReadDiskFail,
	ERR_UNSUPPOSE_CHAR,
	ERR_UNSUPPOSE_MAP_TYPE,
	ERR_INVAILD_PTR,
	ERR_OUT_OF_PERMIT
};


#endif