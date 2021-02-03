/*
	tools/bin2c.c
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,const char * argv[]){
	const char * in_file = NULL,* out_file = NULL;
	char * _out_file = NULL,* off;
	FILE * in,*out;
	int name_len;
	struct tm * time_struct;
	time_t time_stamp;
	unsigned int byte = 0;
	long long magic;
	int array_count;
	int struct_ptr_pos;
	int struct_pos;
	int size_count;
	int array_start_pos;
	int start_name_pos;
	int array_size,_array_size;
	int size_name_pos;
	char name[64];
	
	if(argc <= 1){
		printf("InFileName [OutFileName].\n");
		return -1;
	}
	if(argc >= 2) in_file = argv[1];
	if(argc >= 3) out_file = argv[2];
	if(!out_file){
		_out_file = malloc((name_len = strlen(in_file)) + 3);
		strcpy(_out_file,in_file);
		off = strrchr(_out_file,'\\');
		if(!off) off = strrchr(_out_file,'/');
		if(!off) off = _out_file;
		off = strrchr(off,'.');
		if(!off){
			_out_file[name_len] = '.';
			_out_file[name_len + 1] = 'c';
			_out_file[name_len + 2] = 0;
		}
		else{
			off++; *off = 'c';
			off++; *off = 0;
		}
		out_file = _out_file;
	}
	in = fopen(in_file,"rb");
	fseek(in,0,SEEK_SET);
	fread(&magic,8,1,in);
	if(magic != 0x00000043324e4942){
		fclose(in);
		if(_out_file) free(_out_file);
		printf("Input file is not correct format.\n");
		return -1;
	}
	out = fopen(out_file,"w");
	time_stamp = time(&time_stamp);
	time_struct = localtime(&time_stamp);
	fprintf(out,"//Lava OS\n//PageFault\n//%02d-%02d-%02d-%02d-%02d",time_struct->tm_year%100,
		time_struct->tm_mon,time_struct->tm_mday,time_struct->tm_hour,time_struct->tm_min);
	fprintf(out,"\n\n#include <stddef.h>\n");
	fseek(in,12,SEEK_SET);
	fread(&array_count,4,1,in);
	struct_ptr_pos = 0x10;
	while(array_count){
		array_count--;
		fseek(in,struct_ptr_pos,SEEK_SET);
		fread(&struct_pos,4,1,in);
		fseek(in,struct_pos,SEEK_SET);
		fread(&size_count,4,1,in);
		
		fread(&array_start_pos,4,1,in); fread(&start_name_pos,4,1,in);
		fread(&array_size,4,1,in); fread(&size_name_pos,4,1,in);
		
		struct_pos = ftell(in);
		fseek(in,start_name_pos,SEEK_SET);
		fread(name,1,64,in);
		fprintf(out,"\nu8 %s[] = {",name);
		fseek(in,array_start_pos,SEEK_SET);
		for(_array_size = 0;_array_size < array_size-1;_array_size++){
			if(!(_array_size % 8)) fprintf(out,"\n\t");
			fread(&byte,1,1,in);
			fprintf(out,"0x%02x,",byte);
		}
		if(!(_array_size % 8)) fprintf(out,"\n\t");
		fread(&byte,1,1,in);
		fprintf(out,"0x%02x\n};\n",byte);
		fseek(in,size_name_pos,SEEK_SET);
		fread(name,1,64,in);
		fprintf(out,"int %s = %d;\n",name,array_size);
		while(size_count){
			fseek(in,struct_pos,SEEK_SET);
			size_count--;
			fread(&array_size,4,1,in); fread(&size_name_pos,4,1,in);
			struct_pos = ftell(in);
			fseek(in,size_name_pos,SEEK_SET);
			fread(name,1,64,in);
			fprintf(out,"int %s = %d;\n",name,array_size);
		}
		struct_ptr_pos += 4;
	}
	fclose(in);
	fclose(out);
	if(_out_file) free(_out_file);
	return 0;
}