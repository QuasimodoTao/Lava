LD = ld
LM = lm/lm.o lib/lib.o
KER = init/init.o mm/mm.o lib/lib.o arch/arch.o dev/dev.o ker/ker.o fs/fs.o vm/vm.o graph/graph.o
SHELL = cmd.exe

.PHONY : all clean _boot _lib debug _ker _lm _itry tools

debug : all
	VirtualBoxVM -startvm E:\vpc\vpc.vbox --dbg

all : tools Lava.vhd _boot _lib _ker _lm lm.exe lava.exe

lava.exe : $(KER) Makefile tools/dop.exe
	ld --image-base 0xffff800000100000 -e entry_point -o lava.exe $(KER)
	tools/dop.exe -O Lava.vhd -c /fs/lfs/lava.sys lava.exe 

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
	tools/dop.exe -O Lava.vhd -c /fs/lfs/lm.sys lm.exe
	
boot.bin : boot/boot.asm 
	nasm -f bin -o boot.bin boot/boot.asm
	tools/dop.exe -O Lava.vhd -m lfs boot.bin

MBR.bin : boot/MBR.asm 
	nasm -f bin -o MBR.bin boot/MBR.asm
	tools/dop.exe -O Lava.vhd -M MBR.bin

Lava.vhd :
	tools/dop.exe -C Lava.vhd 262144
	-tools/dop.exe -O Lava.vhd -c /fs/lfs/Font.fnt Font.fnt
	
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