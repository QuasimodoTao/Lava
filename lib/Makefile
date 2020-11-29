INC = ../inc/
LIB = crc32.o vsprintf.o string.o
CC = gcc -c -Os $< -I $(INC) -fno-builtin -o $@
SHELL = cmd.exe

.PHONY : all clean

all : lib.o

lib.o : $(LIB)
	ld -r -o lib.o $(LIB)
	
crc32.o : crc32.c
	$(CC)
	
vsprintf.o : vsprintf.c $(INC)stddef.h
	$(CC)
	
string.o : string.c $(INC)stddef.h
	$(CC)
	
clean :
	-del $(LIB) lib.o