FS = open.o buffer.o part.o lfs.o fat.o file_table.o
INC = ../inc/
CC = gcc -c -Os $< -I $(INC) -fno-builtin -o $@
SHELL = cmd.exe

all : fs.o

fs.o : $(FS)
	ld -r -o fs.o $(FS)
	
open.o : open.c $(INC)mm.h $(INC)string.h $(INC)stddef.h $(INC)asm.h $(INC)stdio.h $(INC)kernel.h $(INC)config.h
	$(CC)
	
buffer.o : buffer.c $(INC)stddef.h $(INC)config.h $(INC)buffer.h $(INC)spinlock.h $(INC)mm.h $(INC)kernel.h $(INC)asm.h
	$(CC)

part.o : part.c $(INC)stddef.h $(INC)fs.h $(INC)spinlock.h $(INC)buffer.h $(INC)mm.h $(INC)string.h $(INC)lava.h
	$(CC)

file_table.o : file_table.c $(INC)config.h $(INC)stddef.h $(INC)mm.h $(INC)string.h $(INC)spinlock.h $(INC)fs.h $(INC)asm.h $(INC)lava.h
	$(CC)

lfs.o : lfs.c $(INC)stddef.h $(INC)lfs.h $(INC)fs.h $(INC)config.h $(INC)mm.h $(INC)buffer.h $(INC)time.h
	$(CC)

fat.o : fat.c
	$(CC)

clean :
	-del fs.o $(FS)