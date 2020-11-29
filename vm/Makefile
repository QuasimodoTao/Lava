OBJ = access.o arith.o bcd.o bt.o ctrl.o flags.o if.o io.o logic.o op.o other.o shift.o stack.o transd.o
INC = ../inc/
CC = gcc -c -Os $< -I $(INC) -fno-builtin -o $@
SHELL = cmd.exe

.PHONY : all clean

all : vm.o

vm.o : $(OBJ)
	ld -r -o $@ $(OBJ)

access.o : access.c $(INC)vm.h $(INC)stddef.h $(INC)config.h $(INC)stdio.h
	$(CC)

arith.o : arith.c $(INC)vm.h $(INC)stddef.h $(INC)config.h $(INC)stdio.h
	$(CC)

bcd.o : bcd.c $(INC)vm.h $(INC)stddef.h $(INC)config.h $(INC)stdio.h
	$(CC)

bt.o : bt.c $(INC)vm.h $(INC)stddef.h $(INC)config.h $(INC)stdio.h
	$(CC)

ctrl.o : ctrl.c $(INC)vm.h $(INC)stddef.h $(INC)config.h $(INC)stdio.h
	$(CC)

flags.o : flags.c $(INC)vm.h $(INC)config.h $(INC)stdio.h
	$(CC)

if.o : if.c $(INC)vm.h $(INC)asm.h $(INC)string.h $(INC)stddef.h $(INC)config.h $(INC)stdio.h $(INC)keyboard.h $(INC)gst.h
	$(CC)

io.o : io.c $(INC)vm.h $(INC)asm.h $(INC)stddef.h $(INC)config.h $(INC)stdio.h
	$(CC)

logic.o : logic.c $(INC)vm.h $(INC)config.h $(INC)stdio.h
	$(CC)

op.o : op.c $(INC)vm.h ins.h $(INC)stddef.h $(INC)config.h
	$(CC)

other.o : other.c $(INC)vm.h $(INC)stddef.h $(INC)config.h $(INC)stdio.h
	$(CC)

shift.o : shift.c $(INC)vm.h $(INC)stddef.h $(INC)config.h $(INC)stdio.h
	$(CC)

stack.o : stack.c $(INC)vm.h $(INC)stddef.h $(INC)config.h $(INC)stdio.h
	$(CC)

transd.o : transd.c $(INC)vm.h $(INC)stddef.h $(INC)config.h $(INC)stdio.h
	$(CC)
	
clean :
	-del $(OBJ) vm.o