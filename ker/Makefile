KER = time.o timer.o schedule.o switch.o
INC = ../inc/
CC = gcc -c -Os $< -I $(INC) -fno-builtin -o $@
SHELL = cmd.exe

all : ker.o

ker.o : $(KER)
	ld -r -o ker.o $(KER)
	
time.o : time.c $(INC)time.h $(INC)lava.h $(INC)stddef.h
	$(CC)
	
timer.o : timer.c $(INC)stddef.h $(INC)timer.h $(INC)mm.h $(INC)asm.h $(INC)lava.h $(INC)mutex.h\
 $(INC)kernel.h $(INC)gst.h $(INC)int.h
	$(CC)
	
schedule.o : schedule.c $(INC)lava.h $(INC)stddef.h $(INC)kernel.h $(INC)mm.h $(INC)arch.h $(INC)asm.h\
 $(INC)timer.h $(INC)config.h $(INC)gst.h
	$(CC)
	
switch.o : switch.s
	gcc -c -o $@ -Os $<

clean :
	-del ker.o $(KER)