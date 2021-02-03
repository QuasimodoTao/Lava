CC = gcc -c -O2 $< -o $@ 
SHELL = cmd.exe

.PHONY : all clean

all : ../dop.exe

../dop.exe : buffer.c common.c dop.c fat.c fs.c lfs.c part.c vhd.c dop.h error.h fs.h
	gcc -o ../dop.exe buffer.c common.c dop.c fat.c fs.c lfs.c part.c vhd.c -municode

clean :