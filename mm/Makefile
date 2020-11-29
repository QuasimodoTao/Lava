MM = page.o mmio.o memory.o swap.o
INC = ../inc/
CC = gcc -c -Os $< -I $(INC) -fno-builtin -o $@
SHELL = cmd.exe

.PHONY : all clean

all : mm.o

mm.o : $(MM)
	ld -r -o mm.o $(MM)
	
page.o : page.c $(INC)asm.h $(INC)stddef.h $(INC)lava.h $(INC)string.h $(INC)arch.h $(INC)int.h\
 $(INC)kernel.h $(INC)mm.h $(INC)config.h
	$(CC)
	
mmio.o : mmio.c $(INC)lava.h $(INC)stddef.h $(INC)mm.h $(INC)asm.h $(INC)string.h $(INC)stdio.h\
 $(INC)config.h
	$(CC)

memory.o : memory.c $(INC)stddef.h $(INC)asm.h $(INC)lava.h $(INC)mm.h $(INC)string.h $(INC)stdio.h\
$(INC)kernel.h $(INC)int.h $(INC)config.h
	$(CC)

swap.o : swap.c
	$(CC)
	
clean :
	-del mm.o $(MM)