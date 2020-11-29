FS = open.o
INC = ../inc/
CC = gcc -c -Os $< -I $(INC) -fno-builtin -o $@
SHELL = cmd.exe

all : fs.o

fs.o : $(FS)
	ld -r -o fs.o $(FS)
	
open.o : open.c $(INC)mm.h $(INC)string.h $(INC)stddef.h $(INC)asm.h $(INC)stdio.h $(INC)kernel.h
	$(CC)
	
clean :
	-del fs.o $(FS)