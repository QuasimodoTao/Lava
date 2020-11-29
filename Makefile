LD = ld
LM = lm/lm.o lib/lib.o
KER = init/init.o mm/mm.o lib/lib.o arch/arch.o dev/dev.o ker/ker.o fs/fs.o vm/vm.o graph/graph.o
SHELL = cmd.exe

.PHONY : all clean _boot _lib debug _ker _lm _itry tools

debug : all
	VirtualBoxVM -startvm E:\vpc\vpc.vbox --dbg

all : tools Lava.vhd _boot _lib _ker _lm lm.exe lava.exe

lava.exe : $(KER) 
	ld --image-base 0xffffc07fc0000000 -e entry_point -o lava.exe $(KER)
	dop -O Lava.vhd -f 1 lava.exe lava.sys

tools :
	cd tools && make all

_ker :
	cd vm && make all
	cd init && make all
	cd mm && make all
	cd arch && make all
	cd dev && make all
	cd ker && make all
	cd fs && make all
	cd graph && make all

_lm :
	cd lm && make all

_lib :
	cd lib && make all

_boot : boot.bin MBR.bin

lm.exe : $(LM) 
	ld --image-base 0x10000 -e EntryPoint -o lm.exe -s $(LM)
	dop -O Lava.vhd -f 1 lm.exe .LM64.SYS
	
boot.bin : boot/boot.asm 
	nasm -f bin -o boot.bin boot/boot.asm
	dop -O Lava.vhd -B 1 boot.bin

MBR.bin : boot/MBR.asm 
	nasm -f bin -o MBR.bin boot/MBR.asm
	dop -O Lava.vhd -M MBR.bin	

Lava.vhd :
	dop -C Lava.vhd 262144
	dop -O Lava.vhd -F
	dop -O Lava.vhd -c 65536 131071
	dop -O Lava.vhd -FP 1 LFS TestPartition
	-dop -O Lava.vhd -f 1 Font.fnt Font.fnt
	
clean :
	-del lm.exe
	-del boot.bin
	-del MBR.bin
	-del lava.exe
	cd lm && make clean
	cd lib && make clean
	cd vm && make clean
	cd init && make clean
	cd dev && make clean
	cd arch && make clean
	cd ker && make clean
	cd fs && make clean
	cd graph && make clean
	cd mm && make clean
	cd tools && make clean