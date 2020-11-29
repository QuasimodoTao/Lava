INC = ../inc/
CC = gcc -c -Os $< -I $(INC) -fno-builtin -o $@
SHELL = cmd.exe

.PHONY : all clean

all : init.o

init.o : init.c $(INC)stddef.h $(INC)lava.h $(INC)kernel.h $(INC)arch.h $(INC)fs.h $(INC)asm.h $(INC)stdio.h $(INC)mm.h $(INC)int.h $(INC)keyboard.h
	$(CC)
	
clean :
	-del init.o init.s
