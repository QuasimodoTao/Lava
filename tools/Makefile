CC = gcc -O2
SHELL = cmd.exe

.PHONY : all clean dop

all : bin2c.exe dop

dop : 
	cd dop && make all

bin2c.exe : bin2c.c
	$(CC) -o bin2c.exe bin2c.c

clean :
	-del bin2c.exe
	-del dop.exe
	cd dop && make clean