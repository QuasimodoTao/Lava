INC = ../inc/
OBJ = main.o stdout.o pci.o sata.o lfs.o memory.o int.o inte.o draw.o
SHELL = cmd.exe
CC = gcc -c -Os $< -I $(INC) -fno-builtin -o $@

.PHONY : all clean

all : lm.o

lm.o : $(OBJ)
	ld -r -o lm.o $(OBJ)
	
main.o : main.c lm.h $(INC)stddef.h $(INC)asm.h
	$(CC)
	
stdout.o : stdout.c lm.h
	$(CC)

pci.o : pci.c lm.h
	$(CC)

sata.o : sata.c lm.h
	$(CC)

lfs.o : lfs.c lm.h
	$(CC)

memory.o : memory.c lm.h
	$(CC)

int.o : int.c
	$(CC)
	
inte.o : inte.s
	gcc -c -o inte.o inte.s
	
draw.o : draw.c $(INC)stddef.h $(INC)asm.h $(INC)string.h
	$(CC)
	
clean :
	-del $(OBJ) lm.o