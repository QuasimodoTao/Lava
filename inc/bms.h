/*
	inc/bms.h
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

#ifndef _BMS_H_
#define _BMS_H_
#include <stddef.h>
#include <asm.h>
#include <string.h>

typedef void * BMS;

inline BMS bmsncpy(BMS d,const BMS s,size_t count){
	u8 mask;
	
	memcpy(d,s,count >> 3);
	mask = 2;
	mask <<= count & 0x07;
	count >>= 3;
	mask--;
	((u8*)d)[count] &= ~mask;
	mask &= ((u8*)s)[count];
	((u8*)d)[count] |= mask;
	return d;
}
inline BMS bmsclr(BMS bit_map,size_t start,size_t count){
	u8 mask,mask2;
	size_t end;
	end = start + count - 1;
	if(start == end) {
		btr(bit_map,start);
		return bit_map;
	}
	if(start & 0x07 == end & 0x07){
		((u8*)bit_map)[start >> 3] &= ~((0xff << (start & 0x07)) & ((2 << (end & 0x07)) - 1));
		return bit_map;
	}
	if(start & 0x07){
		((u8*)bit_map)[start >> 3] &= ~(0xff << (mask & 0x07));
		start += 8;
	}
	if(end & 0x07) ((u8*)bit_map)[end >> 3] &= ~((2 << (end & 0x07)) - 1);
	start >>= 3;
	end >>= 3;
	if(start == end) return bit_map;
	memset(((u8*)bit_map) + start,0,end - start);
	return bit_map;
}
inline BMS bmsset(BMS bit_map,size_t start,size_t count){
	u8 mask,mask2;
	size_t end;
	end = start + count - 1;
	if(start == end) {
		btr(bit_map,start);
		return bit_map;
	}
	if(start & 0x07 == end & 0x07){
		((u8*)bit_map)[start >> 3] |= (0xff << (start & 0x07)) & ((2 << (end & 0x07)) - 1);
		return bit_map;
	}
	if(start & 0x07){
		((u8*)bit_map)[start >> 3] |= 0xff << (mask & 0x07);
		start += 8;
	}
	if(end & 0x07) ((u8*)bit_map)[end >> 3] |= (2 << (end & 0x07)) - 1;
	start >>= 3;
	end >>= 3;
	if(start == end) return bit_map;
	memset(((u8*)bit_map) + start,0xff,end - start);
	return bit_map;
}
inline BMS bmscat(BMS _d,size_t len,BMS _s,size_t start,size_t count){
	size_t byte_off, bit_off;
	u8 *s,*d;
	u8 byte;
	u8 mask1,mask2;
	
	s = (void*)_s;
	d = (void*)_d;
	while(start & 0x07 && count){
		if(bt(s,start)) bts(d,len);
		else btr(d,len);
		len++;
		start++;
		count--;
	}
	if(!count) return _d;
	byte_off = len >> 3;
	bit_off = len & 0x07;
	s += start >> 3;
	d += byte_off;
	mask1 = 0xff << bit_off;
	mask2 = ~mask1;
	*d &= ~mask1;
	while(count > 8){
		*d |= (*s & mask1) << bit_off;
		d++;
		*d = 0;
		*d |= (*s & mask2) >> bit_off;
		count -= 8;
		s++;
	}
	while(count){
		count--;
		if(bt(s,count)) bts(d,bit_off + count);
		else btr(d,bit_off + count);
	}
	return _d;
}
inline size_t bms0(BMS _s,size_t start,size_t count){
	size_t ele_off;
	size_t bit_off;
	u8 * s;
	size_t i;
	
	s = _s;
	s += start >> 3;
	start &= 0x07;
	while(start & 0x07 && count) {
		if(!bt(s,start)) return start + (s - (u8*)_s) * 8;
		count--;
	}
	if(!count) return 0xffffffffffffffff;
	s++;
	while((u64)s & 0x07 && count >= 8){
		if(~*s){
			for(i = 0;i < 8 && count;i++){
				if(!bt(s,i)) return i + (s - (u8*)_s) * 8;
				count--;
			}
			if(!count) return 0xffffffffffffffff;
		}
		count -= 8;
		s++;
	}
	if(!count) return 0xffffffffffffffff;
	while(count >= 64){
		if(~*(u64*)s){
			for(i = 0;i < 64 && count;i++){
				if(!bt(s,i)) return i + (s - (u8*)_s) * 8;
				count--;
			}
			if(!count) return 0xffffffffffffffff;
		}
		count -= 64;
		s+=8;
	}
	for(i = 0;i < count;i++){
		if(!bt(s,i)) return i + (s - (u8*)_s) * 8;
	}
	return 0xffffffffffffffff;
}
inline size_t bms1(BMS _s,size_t start,size_t count){
	size_t ele_off;
	size_t bit_off;
	u8 * s;
	size_t i;
	
	s = _s;
	s += start >> 3;
	start &= 0x07;
	while(start & 0x07 && count) {
		if(bt(s,start)) return start + (s - (u8*)_s) * 8;
		count--;
	}
	if(!count) return 0xffffffffffffffff;
	s++;
	while((u64)s & 0x07 && count >= 8){
		if(*s){
			for(i = 0;i < 8 && count;i++){
				if(bt(s,i)) return i + (s - (u8*)_s) * 8;
				count--;
			}
			if(!count) return 0xffffffffffffffff;
		}
		count -= 8;
		s++;
	}
	if(!count) return 0xffffffffffffffff;
	while(count >= 64){
		if(*(u64*)s){
			for(i = 0;i < 64 && count;i++){
				if(bt(s,i)) return i + (s - (u8*)_s) * 8;
				count--;
			}
			if(!count) return 0xffffffffffffffff;
		}
		count -= 64;
		s+=8;
	}
	for(i = 0;i < count;i++){
		if(bt(s,i)) return i + (s - (u8*)_s) * 8;
	}
	return 0xffffffffffffffff;
}
inline size_t bmsr0(BMS _s,size_t start,size_t count){
	size_t end;
	u8 * s;
	
	end = start + count;
	s = _s;
	while(end & 0x07 && end > start){
		if(!bt(_s,end)) return end;
		end--;
		count--;
	}
	if(!count) return 0xffffffffffffffff;
	end >>= 3;
	end--;
	s += end;
	while((u64)s & 0x07 && count >= 8){
		if(~*s){
			i = 8;
			while(i && count){
				i--;
				if(!bt(s,i)) return  i + (s - (u8*)_s) * 8;
				count--;
			}
			if(!count) return 0xffffffffffffffff;
		}
		count -= 8;
		s--;
	}
	if(!count) return 0xffffffffffffffff;
	s-=8;
	while(count >= 64){
		if(~*(u64*)s){
			i = 64;
			while(i && count){
				if(!bt(s,i)) return i + (s - (u8*)_s) * 8;
				count--;
			}
			if(!count) return 0xffffffffffffffff;
		}
		count -= 64;
		s-=8;
		
	}
	i = count;
	while(i){
		i--;
		if(!bt(s,i)) return i + (s - (u8*)_s) * 8;
	}
	return 0xffffffffffffffff;
}
inline size_t bmsr1(BMS _s,size_t start,size_t count){
	size_t end;
	u8 * s;
	
	end = start + count;
	s = _s;
	while(end & 0x07 && end > start){
		if(bt(_s,end)) return end;
		end--;
		count--;
	}
	if(!count) return 0xffffffffffffffff;
	end >>= 3;
	end--;
	s += end;
	while((u64)s & 0x07 && count >= 8){
		if(*s){
			i = 8;
			while(i && count){
				i--;
				if(bt(s,i)) return  i + (s - (u8*)_s) * 8;
				count--;
			}
			if(!count) return 0xffffffffffffffff;
		}
		count -= 8;
		s--;
	}
	if(!count) return 0xffffffffffffffff;
	s-=8;
	while(count >= 64){
		if(*(u64*)s){
			i = 64;
			while(i && count){
				if(bt(s,i)) return i + (s - (u8*)_s) * 8;
				count--;
			}
			if(!count) return 0xffffffffffffffff;
		}
		count -= 64;
		s-=8;
		
	}
	i = count;
	while(i){
		i--;
		if(bt(s,i)) return i + (s - (u8*)_s) * 8;
	}
	return 0xffffffffffffffff;
}


#endif