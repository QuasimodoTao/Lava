GRAPH = draw.o
INC = ../inc/
CC = gcc -c -Os $< -I $(INC) -fno-builtin -o $@
SHELL = cmd.exe

.PHONY : all clean

all : graph.o

graph.o : $(GRAPH)
	ld -r -o graph.o $(GRAPH)
	
draw.o : draw.c $(INC)stddef.h $(INC)string.h $(INC)asm.h $(INC)vbe.h $(INC)lava.h
	$(CC)

clean :
	-del graph.o $(GRAPH)