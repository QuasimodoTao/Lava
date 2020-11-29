CC = gcc -O2
SHELL = cmd.exe

.PHONY : all clean

all : bin2c.exe

bin2c.exe : bin2c.c
	$(CC) -o bin2c.exe bin2c.c

clean :
	-del bin2c.exe