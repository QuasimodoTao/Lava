ARCH = sdt.o int.o private.o syscall.o ap_entry.o asm.o interrupt.o
INC = ../inc/
CC = gcc -c -Os $< -I $(INC) -fno-builtin -o $@
SHELL = cmd.exe

.PHONY : all clean

all : arch.o

arch.o : $(ARCH)
	ld -r -o arch.o $(ARCH)

ap_entry.o : ap_entry.asm
	nasm -f bin -o ap_entry.bin ap_entry.asm
	../tools/bin2c.exe ap_entry.bin ap_entry.c
	gcc -c -Os ap_entry.c -I $(INC) -fno-builtin -o $@
	
switchcs.o : switchcs.s
	$(CC)
	
init.o : init.c $(INC)stddef.h $(INC)lava.h $(INC)string.h $(INC)arch.h $(INC)asm.h $(INC)mm.h
	$(CC)
	
int.o : int.c $(INC)arch.h $(INC)asm.h $(INC)stddef.h $(INC)stdio.h
	$(CC)
	
sdt.o : sdt.c $(INC)stddef.h $(INC)asm.h $(INC)string.h $(INC)arch.h $(INC)config.h $(INC)mm.h
	$(CC)
	
private.o : private.c $(INC)stddef.h $(INC)arch.h $(INC)config.h $(INC)kernel.h $(INC)lava.h $(INC)mm.h
	$(CC)
	
syscall.o : syscall.c $(INC)stddef.h $(INC)arch.h $(INC)config.h $(INC)string.h $(INC)stdio.h $(INC)int.h
	$(CC)
	
asm.o : asm.c $(INC)stddef.h $(INC)asm.h
	$(CC)

interrupt.o : interrupt.s
	$(CC)

clean :
	-del arch.o $(ARCH) ap_entry.c ap_entry.bin