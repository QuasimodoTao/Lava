INC = ../inc/
CC = gcc -c -Os $< -I $(INC) -fno-builtin -o $@
DEV = out.o pci.o pit.o rtc.o apic.o ps2_kbd.o vga.o serial.o ahci.o ata.o
SHELL = cmd.exe

.PHONY : all clean

all : dev.o

dev.o : $(DEV)
	ld -r -o dev.o $(DEV)
	
out.o : out.c $(INC)stddef.h $(INC)string.h $(INC)asm.h $(INC)int.h $(INC)stdio.h
	$(CC)
	
pci.o : pci.c $(INC)lava.h $(INC)stddef.h $(INC)asm.h $(INC)pci.h $(INC)int.h $(INC)stdio.h	\
$(INC)string.h $(INC)mm.h 
	$(CC)

pit.o : pit.c $(INC)stddef.h $(INC)asm.h $(INC)int.h $(INC)kernel.h
	$(CC)
	
rtc.o : rtc.c $(INC)stddef.h $(INC)asm.h $(INC)time.h $(INC)int.h $(INC)lava.h
	$(CC)

apic.o : apic.c $(INC)stddef.h $(INC)asm.h $(INC)config.h $(INC)mp.h $(INC)arch.h $(INC)kernel.h $(INC)mm.h\
$(INC)lava.h $(INC)int.h $(INC)stdio.h
	$(CC)

ps2_kbd.o : ps2_kbd.c $(INC)stddef.h $(INC)asm.h $(INC)keyboard.h $(INC)int.h $(INC)semaphore.h
	$(CC)

vga.o : vga.c $(INC)vm.h $(INC)stddef.h $(INC)mm.h $(INC)string.h $(INC)stdio.h
	$(CC)

serial.o : serial.c $(INC)int.h $(INC)lava.h $(INC)stddef.h $(INC)asm.h $(INC)kernel.h $(INC)mm.h\
 $(INC)gst.h $(INC)semaphore.h $(INC)error.h $(INC)spinlock.h $(INC)stdio.h
	$(CC)
	
ahci.o : ahci.c $(INC)lava.h $(INC)mm.h $(INC)ahci.h $(INC)stddef.h $(INC)pci.h $(INC)spinlock.h
	$(CC)
	
ata.o : ata.c $(INC)ahci.h $(INC)ata.h $(INC)stddef.h $(INC)spinlock.h $(INC)lava.h $(INC)mm.h $(INC)gst.h\
 $(INC)stdio.h
	$(CC)

clean :
	-del dev.o $(DEV)