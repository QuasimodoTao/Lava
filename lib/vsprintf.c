//Lava OS
//PageFault
//20-02-10-17-20

#include <stddef.h>

#define ZERO		0x00001
#define SIZE		0x00002
#define UPPER		0x00004
#define INT8		0x00008
#define INT16		0x00010
#define INT32		0x00020
#define INT64		0x00040

static const char acu[] = "0123456789ABCDEF";
static const char acl[] = "0123456789abcdef";
static const char * getnumber(const char * fmt, int * value) {
	int res = 0;
	char tmp;

	while (*fmt) {
		tmp = *fmt;
		tmp -= '0';
		if (tmp > 9 || tmp < 0)  break;
		res *= 10;
		res += tmp;
		fmt++;
	}
	*value = res;
	return fmt;
}
static char * tounumber(char * buf, size_t bsize, uint64_t data, int base, int flags, int size) {
	char s[20];
	int i = 0;
	const char * sx = acl;
	static int Count = 0;

	Count++;
	if (flags & UPPER) sx = acu;
	if (data) {
		for (i = 0;data;i++) {
			s[i] = sx[data%base];
			data /= base;
		}
	}
	else {
		s[i] = '0';
		i++;
	}
	if (flags & SIZE) {
		if((i > size ? i : size) > bsize){
			*buf = 0;
			return NULL;
		}
		if (i >= size) {
			while (i > 0) {
				*buf = s[i - 1];
				i--;
				buf++;
			}
		}
		else {
			while (i < size) {
				size--;
				if (flags & ZERO) *buf = '0';
				else *buf = ' ';
				buf++;
			}
			while (i > 0) {
				*buf = s[i - 1];
				i--;
				buf++;
			}
		}
	}
	else {
		if(i > bsize){
			*buf = 0;
			return NULL;
		}
		while (i > 0) {
			*buf = s[i - 1];
			i--;
			buf++;
		}
	}
	return buf;
}
static char * tonumber(char * buf, size_t bsize, int64_t data, int base, int flags, int size) {
	char s[20];
	int i = 0;
	const char * sx = acl;
	int64_t datao;

	datao = data;
	if (flags & UPPER) sx = acu;
	if(data < 0) data = -data;
	if (data) {
		for (;data;i++) {
			s[i] = sx[data%base];
			data /= base;
		}
	}
	else {
		s[i] = '0';
		i++;
	}
	if(datao < 0){
		s[i] = '-';
		i++;
	}
	//asm("hlt"::"a"(bsize),"c"(i),"d"(size));
	if (flags & SIZE) {
		if((i > size ? i : size) > bsize){
			*buf = 0;
			return NULL;
		}
		if (i >= size) {
			while (i > 0) {
				*buf = s[i - 1];
				i--;
				buf++;
			}
		}
		else {
			while (i < size) {
				size--;
				if (flags & ZERO) *buf = '0';
				else *buf = ' ';
				buf++;
			}
			while (i > 0) {
				*buf = s[i - 1];
				i--;
				buf++;
			}
		}
	}
	else {
		if(i > bsize){
			*buf = 0;
			return NULL;
		}
		while (i > 0) {
			*buf = s[i - 1];
			i--;
			buf++;
		}
	}
	return buf;
}
int vsprintf(char * _buf, size_t bsize, const char * fmt, int64_t * argvlist) {
	char * buf;
	int flags = 0;
	int size;
	char * tmp;
	
	buf = _buf;
	while (*fmt) {
		if (*fmt != '%') {
			*buf = *fmt;
			buf++;
			fmt++;
			if(buf - _buf >= bsize){
				*buf = 0;
				return buf - _buf;
			}
			continue;
		}
		fmt++;//% exist
nextchar:
		switch (*fmt) {
		case '0'://使用0填充不足的长度
			fmt++;
			flags |= ZERO;
			goto nextchar;
		case 'l':
			fmt++;
			if (*fmt == 'l') {//64位数
				fmt++;
				flags |= INT64;
			}
			else flags |= INT32;//32位数
			goto nextchar;
		case 'h':
			fmt++;
			if (*fmt == 'h') {
				fmt++;
				flags |= INT8;//8位整数
			}
			else flags |= INT16;//16位整数
			goto nextchar;
		case 'I':
			fmt++;
			if (fmt[0] == '6' && fmt[1] == '4') {
				fmt += 2;
				flags |= INT64;//64位整数
			}
			else flags |= INT32;//32位整数
			goto nextchar;
		case '%'://显示百分号
			*buf = '%';
			buf++;
			fmt++;
			if(buf - _buf >= bsize){
				*buf = 0;
				return buf - _buf;
			}
			break;
		case 'd'://十进制显示有符号数
			tmp = buf;
			if (flags & INT64) buf = tonumber(buf, _buf + bsize - buf, *(int64_t*)argvlist, 10, flags, size);
			else if (flags & INT16) buf = tonumber(buf, _buf + bsize - buf, *(int16_t*)argvlist, 10, flags, size);
			else if (flags & INT8) buf = tonumber(buf, _buf + bsize - buf, *(int8_t*)argvlist, 10, flags, size);
			else buf = tonumber(buf, _buf + bsize - buf, *(int32_t*)argvlist, 10, flags, size);
			if(!buf) return tmp - _buf;
			if(buf - _buf >= bsize){
				*buf = 0;
				return buf - _buf;
			}
			argvlist++;
			fmt++;
			break;
		case 'u'://十进制显示无符号数
			tmp = buf;
			if (flags & INT64) buf = tounumber(buf, _buf + bsize - buf, *(uint64_t*)argvlist, 10, flags, size);
			else if (flags & INT16) buf = tounumber(buf, _buf + bsize - buf, *(uint16_t*)argvlist, 10, flags, size);
			else if (flags & INT8) buf = tounumber(buf, _buf + bsize - buf, *(uint8_t*)argvlist, 10, flags, size);
			else buf = tounumber(buf, _buf + bsize - buf, *(uint32_t*)argvlist, 10, flags, size);
			if(!buf) return tmp - _buf;
			if(buf - _buf >= bsize){
				*buf = 0;
				return buf - _buf;
			}
			argvlist++;
			fmt++;
			break;
		case 'X'://十六进制显示整数，大写
			flags |= UPPER;
		case 'x'://十六进制显示整数，小写
			tmp = buf;
			if (flags & INT64) buf = tounumber(buf, _buf + bsize - buf, *(uint64_t*)argvlist, 16, flags, size);
			else if (flags & INT16) buf = tounumber(buf, _buf + bsize - buf, *(uint16_t*)argvlist, 16, flags, size);
			else if (flags & INT8) buf = tounumber(buf, _buf + bsize - buf, *(uint8_t*)argvlist, 16, flags, size);
			else buf = tounumber(buf, _buf + bsize - buf, *(uint32_t*)argvlist, 16, flags, size);
			if(!buf) return tmp - _buf;
			if(buf - _buf >= bsize){
				*buf = 0;
				return buf - _buf;
			}
			argvlist++;
			fmt++;
			break;
		case 's'://字符串
			tmp = *(char**)argvlist;
			if(tmp) while (*tmp) {
					*buf = *tmp;
					buf++;
					tmp++;
					if(buf - _buf >= bsize){
						*buf = 0;
						return buf - _buf;
					}
				}
			argvlist++;
			fmt++;
			break;
		case 'c'://字符
			*buf = *(char*)argvlist;
			argvlist++;
			fmt++;
			buf++;
			break;
		case 'P'://十六进制显示指针，大写
			flags |= UPPER;
		case 'p'://十六进制显示指针，小写
			flags |= SIZE | ZERO;
			tmp = buf;
			buf = tounumber(buf, _buf + bsize - buf, *(uint64_t*)argvlist, 16, flags, 16);
			if(!buf) return tmp - _buf;
			if(buf - _buf >= bsize){
				*buf = 0;
				return buf - _buf;
			}
			argvlist++;
			fmt++;
			break;
		case 'n'://传递当前长度，无符号数指针
			if(flags & INT8) *(uint8_t*)(*argvlist) = buf - _buf;
			else if (flags & INT16) *(uint16_t*)(*argvlist) = buf - _buf;
			else if(flags & INT64) *(uint64_t*)(*argvlist) = buf - _buf;
			else *(uint32_t*)(*argvlist) = buf - _buf;
			argvlist++;
			fmt++;
			break;
		default:
			if (*fmt >= '1' && *fmt <= '9') {//第一定长参数
				fmt = getnumber(fmt, &size);
				flags |= SIZE;
				goto nextchar;
			}
		}
		flags = 0;
	}
	*buf = 0;
	return buf - _buf;
}
int sprintf(char * _buf, size_t size, const char * fmt,...){
	return vsprintf(_buf,size,fmt,((int64_t*)&fmt) + 1);
}

static const wchar_t wcu[] = L"0123456789ABCDEF";
static const wchar_t wcl[] = L"0123456789abcdef";
static const wchar_t * getnumberws(const wchar_t * fmt, int * value) {
	int res = 0;
	wchar_t tmp;

	while (*fmt) {
		tmp = *fmt;
		tmp -= L'0';
		if (tmp > 9 || tmp < 0)  break;
		res *= 10;
		res += tmp;
		fmt++;
	}
	*value = res;
	return fmt;
}
static wchar_t * tounumberws(wchar_t * buf, size_t bsize, uint64_t data, int base, int flags, int size) {
	wchar_t s[20];
	int i = 0;
	const wchar_t * sx = wcl;
	static int Count = 0;

	Count++;
	if (flags & UPPER) sx = wcu;
	if (data) {
		for (i = 0;data;i++) {
			s[i] = sx[data%base];
			data /= base;
		}
	}
	else {
		s[i] = L'0';
		i++;
	}
	if (flags & SIZE) {
		if((i > size ? i : size) > bsize){
			*buf = 0;
			return NULL;
		}
		if (i >= size) {
			while (i > 0) {
				*buf = s[i - 1];
				i--;
				buf++;
			}
		}
		else {
			while (i < size) {
				size--;
				if (flags & ZERO) *buf = L'0';
				else *buf = L' ';
				buf++;
			}
			while (i > 0) {
				*buf = s[i - 1];
				i--;
				buf++;
			}
		}
	}
	else {
		if(i > bsize){
			*buf = 0;
			return NULL;
		}
		while (i > 0) {
			*buf = s[i - 1];
			i--;
			buf++;
		}
	}
	return buf;
}
static wchar_t * tonumberws(wchar_t * buf, size_t bsize, int64_t data, int base, int flags, int size) {
	wchar_t s[20];
	int i = 0;
	const wchar_t * sx = wcl;
	int64_t datao;

	datao = data;
	if (flags & UPPER) sx = wcu;
	if(data < 0) data = -data;
	if (data) {
		for (;data;i++) {
			s[i] = sx[data%base];
			data /= base;
		}
	}
	else {
		s[i] = L'0';
		i++;
	}
	if(datao < 0){
		s[i] = L'-';
		i++;
	}
	if (flags & SIZE) {
		if((i > size ? i : size) > bsize){
			*buf = 0;
			return NULL;
		}
		if (i >= size) {
			while (i > 0) {
				*buf = s[i - 1];
				i--;
				buf++;
			}
		}
		else {
			while (i < size) {
				size--;
				if (flags & ZERO) *buf = L'0';
				else *buf = L' ';
				buf++;
			}
			while (i > 0) {
				*buf = s[i - 1];
				i--;
				buf++;
			}
		}
	}
	else {
		if(i > bsize){
			*buf = 0;
			return NULL;
		}
		while (i > 0) {
			*buf = s[i - 1];
			i--;
			buf++;
		}
	}
	return buf;
}
int vwsprintf(wchar_t * _buf, size_t bsize, const wchar_t * fmt, int64_t * argvlist) {
	wchar_t * buf;
	int flags = 0;
	int size;
	wchar_t * tmp;

	//*_buf = 0;
	//return 0;
	
	buf = _buf;
	while (*fmt) {
		if (*fmt != L'%') {
			*buf = *fmt;
			buf++;
			fmt++;
			if(buf - _buf >= bsize){
				*buf = 0;
				return buf - _buf;
			}
			continue;
		}
		fmt++;//% exist
nextchar:
		switch (*fmt) {
		case L'0'://使用0填充不足的长度
			fmt++;
			flags |= ZERO;
			goto nextchar;
		case L'l':
			fmt++;
			if (*fmt == L'l') {//64位数
				fmt++;
				flags |= INT64;
			}
			else flags |= INT32;//32位数
			goto nextchar;
		case L'h':
			fmt++;
			if (*fmt == L'h') {
				fmt++;
				flags |= INT8;//8位整数
			}
			else flags |= INT16;//16位整数
			goto nextchar;
		case L'I':
			fmt++;
			if (fmt[0] == L'6' && fmt[1] == L'4') {
				fmt += 2;
				flags |= INT64;//64位整数
			}
			else flags |= INT32;//32位整数
			goto nextchar;
		case L'%'://显示百分号
			*buf = L'%';
			buf++;
			fmt++;
			break;
		case L'd'://十进制显示有符号数
			tmp = buf;
			if (flags & INT64) buf = tonumberws(buf, _buf + bsize - buf, *(int64_t*)argvlist, 10, flags, size);
			else if (flags & INT16) buf = tonumberws(buf, _buf + bsize - buf, *(int16_t*)argvlist, 10, flags, size);
			else if (flags & INT8) buf = tonumberws(buf, _buf + bsize - buf, *(int8_t*)argvlist, 10, flags, size);
			else buf = tonumberws(buf, _buf + bsize - buf, *(int32_t*)argvlist, 10, flags, size);
			if(!buf) return tmp - _buf;
			if(buf - _buf >= bsize){
				*buf = 0;
				return buf - _buf;
			}
			argvlist++;
			fmt++;
			break;
		case L'u'://十进制显示无符号数
			tmp = buf;
			if (flags & INT64) buf = tounumberws(buf, _buf + bsize - buf, *(uint64_t*)argvlist, 10, flags, size);
			else if (flags & INT16) buf = tounumberws(buf, _buf + bsize - buf, *(uint16_t*)argvlist, 10, flags, size);
			else if (flags & INT8) buf = tounumberws(buf, _buf + bsize - buf, *(uint8_t*)argvlist, 10, flags, size);
			else buf = tounumberws(buf, _buf + bsize - buf, *(uint32_t*)argvlist, 10, flags, size);
			if(!buf) return tmp - _buf;
			if(buf - _buf >= bsize){
				*buf = 0;
				return buf - _buf;
			}
			argvlist++;
			fmt++;
			break;
		case L'X'://十六进制显示整数，大写
			flags |= UPPER;
		case L'x'://十六进制显示整数，小写
			tmp = buf;
			if (flags & INT64) buf = tounumberws(buf, _buf + bsize - buf, *(uint64_t*)argvlist, 16, flags, size);
			else if (flags & INT16) buf = tounumberws(buf, _buf + bsize - buf, *(uint16_t*)argvlist, 16, flags, size);
			else if (flags & INT8) buf = tounumberws(buf, _buf + bsize - buf, *(uint8_t*)argvlist, 16, flags, size);
			else buf = tounumberws(buf, _buf + bsize - buf, *(uint32_t*)argvlist, 16, flags, size);
			if(!buf) return tmp - _buf;
			if(buf - _buf >= bsize){
				*buf = 0;
				return buf - _buf;
			}
			argvlist++;
			fmt++;
			break;
		case L's'://字符串
			tmp = *(wchar_t**)argvlist;
			if(tmp) while (*tmp) {
					*buf = *tmp;
					buf++;
					tmp++;
					if(buf - _buf >= bsize){
						*buf = 0;
						return buf - _buf;
					}
				}
			argvlist++;
			fmt++;
			break;
		case L'c'://字符
			*buf = *(wchar_t*)argvlist;
			argvlist++;
			fmt++;
			buf++;
			break;
		case L'P'://十六进制显示指针，大写
			flags |= UPPER;
		case L'p'://十六进制显示指针，小写
			flags |= SIZE | ZERO;
			tmp = buf;
			buf = tounumberws(buf, _buf + bsize - buf, *(uint64_t*)argvlist, 16, flags, 16);
			if(!buf) return tmp - _buf;
			if(buf - _buf >= bsize){
				*buf = 0;
				return buf - _buf;
			}
			argvlist++;
			fmt++;
			break;
		case L'n'://传递当前长度，无符号数指针
			if(flags & INT8) *(uint8_t*)(*argvlist) = buf - _buf;
			else if (flags & INT16) *(uint16_t*)(*argvlist) = buf - _buf;
			else if(flags & INT64) *(uint64_t*)(*argvlist) = buf - _buf;
			else *(uint32_t*)(*argvlist) = buf - _buf;
			argvlist++;
			fmt++;
			break;
		default:
			if (*fmt >= L'1' && *fmt <= L'9') {//第一定长参数
				fmt = getnumberws(fmt, &size);
				flags |= SIZE;
				goto nextchar;
			}
		}
		flags = 0;
	}
	*buf = 0;
	return buf - _buf;
}
int wsprintf(wchar_t * _buf, size_t size, const wchar_t * fmt,...){
	return vwsprintf(_buf,size,fmt,((int64_t*)&fmt) + 1);
}
//Quasimodo