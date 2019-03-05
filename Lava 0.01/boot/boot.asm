;boot.asm

;struct DataHeader{
;	word MinVer;
;	word MajVer;
;	dword BlockCnt;
;	qword SecCnt;
;	
;	dword Node0Block;
;	dword ?
;	dword ?
;	dword ?
;	
;	dword ?
;	dword LMNode;
;	dword FirstFreeBlock;
;	dword FirstFreeNode;
;	
;	qword GlobalBlockCnt;
;	qword GlobalFree;
;	
;	GUID FirstDisk;
;	GUID FirstPart;
;	GUID LastDisk;
;	GUID LastPart;
;	GUID PrevDisk;
;	GUID PrevPart;
;	GUID NextDisk;
;	GUID NextPart;
;	
;	wchar_t wVolName[];	
;};

;struct Node{
;	qword Create;
;	qword Access;
;
;	qword Modify;
;	qword Size;
;
;	GUID Auther;
;
;	short Attr;	//read,write,excuse,delete,autom data,system,hide,copy,link,dictionary
;	short Link;
;	union{
;		dword Block[19];
;		char data[76];
;	};
;};

;struct Dictionary{
;	byte NameLen;
;	byte DescLen;
;	word Attr;	//read,write,excuse,delete,system,hide,copy,dectionary
;	dword Node;
;	qword Create;
;	GUID Auther;
;	word Name[NameLen];
;	word Descript[DescLen];
;};

%define SecSize	512
%define Version	0x0001
%define LodOff	0x0000
%define LodSeg	0x9000
%define SecCnt	(____End - $$)/SecSize
%define SupDataStart 8



%define SecPerBlock (4096/SecSize)
%if SecPerBlock == 1
%define Sec2BlockShift	0
%elif SecPerBlock == 2
%define Sec2BlockShift	1
%elif SecPerBlock == 4
%define Sec2BlockShift	2
%elif SecPerBlock == 8
%define Sec2BlockShift	3
%elif SecPerBlock == 16
%define Sec2BlockShift	4
%else 
%error "Bad BlockSize or SectorSize."
%endif

[CPU 686]
[bits 16]
org 0x0000;0x1000:0x0000


BootMsgVer:		dw	Version
BootMsgSecCnt:	dw	SecCnt
BootMsgOff:		dw	LodOff
BootMsgSeg:		dw	LodSeg
BootMsgEntry:	dw	Entry
BootMsgSupData:	dw	SupDataStart
BootMsgRvd:		dw	0
BootMsgEvenXor:	dw	0xffff^Version^SecCnt^LodOff^LodSeg^(Entry-$$)^SupDataStart
align 16
DAP:				;用于LBA模式读取硬盘的数据结构
DAP_Size:		db	16
DAP_Rvd:		db	0
DAP_BlockCnt:	dw	0
DAP_Buf_Off:	dw	0
DAP_Buf_Seg:	dw	LodSeg
DAP_LBA:		dq	0

FirstLBA:		dq	0
align 8
GDTR:	dw	0x17,GDT,LodSeg >> 12
align 8
IDTR:	dw	0,0,0
align 8
GDT:	dq	0;
		db	0ffh,0ffh,000h,000h,000h,09ah,0cfh,000h ;代码段 4GB l0
		db	0ffh,0ffh,000h,000h,000h,092h,0cfh,000h	;数据段 4GB l0
align 2
CanNotOperateDiskStr:	db	"Can't read disk.",0x0a,0x0d,0
align 2
UnsupposeStr:			db	"Unsuppose LM32 file.",0x0a,0x0d,0
align 2
NoLMStr:				db	"LMNode not exist.",0x0a,0x0d,0
align 2
NotSuppostVESAStr:		db	"Unsuppose version of VESA.",0x0d,0x0a,0
align 16
BootNode:		times 128	db	0
BootBlock:		times 128	dw	0
align 4
BlockCnt:			dd	0
SmapCnt:			dd	0
SMAPAddr:			dd	0
VESAInfoAddr:		dd	0
VESAModeInfoAddr:	dd	0
VESAModeCnt:		dd	0


align 16
Entry:
	cli
	mov bx,LodSeg
	mov ds,bx
	mov es,bx
	mov ss,bx
	mov sp,0xfbfe
	sti
	mov [FirstLBA],ax		;保存当前分区起始逻辑扇区号，cx:dx:ax
	mov [FirstLBA + 2],dx
	mov [FirstLBA + 4],cx
	push 1
	push ____End
	push 1
	call ReadBlock
	mov bx,[____End + 36]
	mov ax,[____End + 16]
	push 1
	shl bx,7
	mov si,____End
	push ____End
	add si,bx
	mov di,BootNode
	push ax
	call ReadBlock
	mov cx,64
	rep movsw

	mov bp,sp
	sub sp,4096
	mov cx,16
	mov di,BootBlock
	mov si,BootNode + 52
	xor dx,dx
GetAllBlockl1:
	lodsw
	test ax,ax
	jz GetAllBlockl0
	add si,2
	stosw
	inc dx
	loop GetAllBlockl1
	lodsw
	test ax,ax
	jz GetAllBlockl0
	push dx
	push 1
	lea si,[bp - 4096]
	push si
	push ax
	call ReadBlock
	pop dx
	mov cx,1024
GetAllBlockl2:
	lodsw
	test ax,ax
	jz GetAllBlockl0
	add si,2
	stosw
	inc dx
	loop GetAllBlockl2
GetAllBlockl0:
	mov sp,bp
	
	test dx,dx
	jz NoLM
	mov [BlockCnt],dx
	mov di,dx
	mov bx,BootBlock
	mov word [DAP_Buf_Seg],0x1000
LoadFilel1:
	test di,di
	jz LoadFilel0
	dec di
	push 1
	push 0
	push word [bx]
	call ReadBlock
	add bx,2
	add word [DAP_Buf_Seg],0x100
	jmp LoadFilel1
LoadFilel0:
	mov word [DAP_Buf_Seg],LodSeg
	
	mov ax,0x1000
	mov es,ax
	mov bx,[es:0x3c]
	cmp word [es:bx],0x4550
	jne Unsuppose
	cmp word [es:bx + 2],0x0000
	jne Unsuppose
	cmp word [es:bx + 0x18],0x010b
	jne Unsuppose
	
;	xor si,si
;	mov word [DAP_Buf_Seg],0
;SaveNext:
;	push 1
;	push 0
;	push si
;	call WriteBlock
;	inc si
;	cmp si,256
;	jnb SaveFileEnd
;	add word [DAP_Buf_Seg],0x0100;
;	jmp SaveNext
;SaveFileEnd:
;	hlt
;	jmp SaveFileEnd

;get SMAP
	mov ax,0x50
	mov es,ax
	mov ds,ax
	xor di,di
	xor ebx,ebx
	xor bp,bp
	mov edx,0x534d4150
	mov eax,0xe820
	mov dword [di + 20],1
	mov ecx,24
	int 0x15
	jc Failed
	mov edx,0x534d4150
	cmp eax,edx
	jne Failed
	test ebx,ebx
	je Failed
	jmp Jmpin
E820lp:
	mov eax,0xe820
	mov dword [di],1
	mov ecx,24
	int 0x15
	jc E820F
	mov edx,0x534d4150
Jmpin:
	jcxz SkipEnt
	cmp cl,20
	jbe NotExt
	test byte [di],1
	je SkipEnt
NotExt:
	mov ecx,[di + 8]
	or ecx,[di + 12]
	jz SkipEnt
	inc bp
	add di,24
SkipEnt:
	test ebx,ebx
	jne E820lp
E820F:
	mov ax,LodSeg
	mov ds,ax
	mov [SmapCnt],bp
Failed:
	mov dword [SMAPAddr],0x00000500
	shl bp,5
	add bp,0x0500
	mov [VESAInfoAddr],bp
	mov bx,bp
	add bx,0x200
	mov [VESAModeInfoAddr],bx

;	char BitPerPixel
;	char Size;
;	short XResolution
;	short YResolution
;	short BytePerScanLine
;	long PhysicalAddr
;	short ModeID;
	
;Get VESA screen mode
	shr bp,4
	mov es,bp
	mov bx,[VESAModeInfoAddr]
	sub bx,0x0500
	xor di,di
	mov ax,0x50
	mov fs,ax
	mov ax,0x4f00
	int 0x10
	xor bp,bp
	cmp ax,0x004f
	jne Failed1
	cmp dword [es:di],'VESA'
	je Then
	cmp dword [es:di],'VBE2'
	jne Failed1
Then:
	cmp word [es:di + 4],0x0300
	je VESASuppose
	cmp word [es:di + 4],0x0200
	jne NotSuppostVESA
VESASuppose:
	lds si,[es:di + 14]
	push ss
	pop es
	mov di,____End
VESANext:
	mov cx,[si]
	cmp cx,0xffff
	je VESAF
	mov ax,0x4f01
	int 0x10
	test byte [es:di],0x010
	jz VESAText
;	mov ax,[si]
;	call ShowHex
	mov al,[es:di + 25]	;BitsPerPixel
	mov cx,[es:di + 16]	;BytePerScalLine
	mov dx,[es:di + 18]	;XResolution
	mov [fs:bx + 0],al
	mov [fs:bx + 6],cx
	mov [fs:bx + 2],dx
	mov ax,[es:di + 20]	;YResolution
	mov cx,[es:di + 40]	;PhysicalAddr
	mov dx,[es:di + 42]
	mov [fs:bx + 4],ax
	mov [fs:bx + 8],cx
	mov [fs:bx + 10],dx
	mov byte [fs:bx + 1],16
	lodsw
	mov [fs:bx + 12],ax
	add bx,16
	inc bp
	jmp VESANext
VESAText:
	inc si
	inc si
	jmp VESANext
VESAF:	
Failed1:
	mov [ss:VESAModeCnt],bp
	
	
	cli
	mov	al,0x11				; initialization sequence
	out	0x20,al				; send it to 8259A-1
	call delay
	out	0xA0,al				; and to 8259A-2
	call delay
	mov	al,0x20				; start of hardware int's (0x20)
	out	0x21,al
	call delay
	mov	al,0x28				; start of hardware int's 2 (0x28)
	out	0xA1,al
	call delay
	mov	al,0x04				; 8259-1 is master
	out	0x21,al
	call delay
	mov	al,0x02				; 8259-2 is slave
	out	0xA1,al
	call delay
	mov	al,0x01				; 8086 mode for both
	out	0x21,al
	call delay
	out	0xA1,al
	call delay
	mov	al,0xFb				; mask off all interrupts for now
	out	0x21,al
	call delay
	mov al,0xff
	out	0xA1,al
	lidt [ss:IDTR]		;加载IDTR，不是必须的，
	lgdt [ss:GDTR]		;加载GDTR，进入32必须的
	
	in al,092h		;打开A20
	or al,2
	out 092h,al
    
	;call Stop
	
	mov	ax,1
	lmsw ax			;设置PE位，cr0寄存器的第0位
	jmp	dword 0x8:(LodSeg << 4) + _In32Bit	;跳转到32位，

;memcmp:	;int stdcall memcmp(void * s,void * d,int n);
;	push bp
;	mov bp,sp
;	push si
;	push di
;	mov cx,[bp + 8]
;	mov si,[bp + 6]
;	mov di,[bp + 4]
;	repe cmpsb
;	mov ax,cx
;	pop di
;	pop si
;	pop bp
;	ret 8

;HexStr:	db	"0123456789ABCDEF"
;ShowHex:
;	push bx
;	push ds
;	push ss
;	pop ds
;	mov bx,HexStr
;	mov cx,ax
;	mov al,ah
;	mov ah,0x0e
;	shr al,4
;	mov bx,HexStr
;	and al,0x0f
;	xlatb
;	int 0x10
;	mov al,ch
;	mov bx,HexStr
;	and al,0x0f
;	xlatb
;	int 0x10
;	mov al,cl
;	shr al,4
;	mov bx,HexStr
;	and al,0x0f
;	xlatb
;	int 0x10
;	mov al,cl
;	mov bx,HexStr
;	and al,0x0f
;	xlatb
;	int 0x10
;	pop ds
;	pop bx
;	ret

Stop:
	hlt
	jmp Stop
	
;PutChar:
;	mov ah,0x0e
;	int 0x10
;	ret
	
Print:	;void stdcall Print(char * Str)
	push bp
	mov bp,sp
	push si
	mov si,[bp + 4]
	mov ah,0x0e
_PrintNext:
	lodsb
	test al,al
	jz _PrintEnd
	int 0x10
	jmp _PrintNext
_PrintEnd:
	pop si
	pop bp
	ret 2

ReadBlock:	;int stdcall ReadBlock(short Block,void * Off,int Cnt);
	push bp
	mov bp,sp
	push si
	;mov ax,[bp + 4]
	;call ShowHex
	;mov al,','
	;call PutChar
	mov cx,Sec2BlockShift
	mov ax,[bp + 4]
	xor dx,dx
_ReadBlockl1:
	test cx,cx
	jz _ReadBlockl0
	shl ax,1
	rol dx,1
	dec cx
	jmp _ReadBlockl1
_ReadBlockl0:
	add ax,[FirstLBA]
	adc dx,[FirstLBA + 2]
	mov [DAP_LBA],ax
	mov [DAP_LBA + 2],dx
	mov ax,[bp + 6]
	mov dx,[bp + 8]
	mov cx,Sec2BlockShift
	mov [DAP_Buf_Off],ax
	shl dx,cl
	mov [DAP_BlockCnt],dx
	mov ah,0x42
	mov dl,0x80
	mov si,DAP
	int 0x13
	jc _CanNotOperateDisk
	pop si
	pop bp
	ret 6	
	
;WriteBlock:	;int stdcall WriteBlock(short Block,void * Off,int Cnt);
;	push bp
;	mov bp,sp
;	push si
;	;mov ax,[bp + 4]
;	;call ShowHex
;	;mov al,','
;	;call PutChar
;	mov cx,Sec2BlockShift
;	mov ax,[bp + 4]
;	xor dx,dx
;_WriteBlockl1:
;	test cx,cx
;	jz _WriteBlockl0
;	shl ax,1
;	rol dx,1
;	dec cx
;	jmp _WriteBlockl1
;_WriteBlockl0:
;	add ax,[FirstLBA]
;	adc dx,[FirstLBA + 2]
;	mov [DAP_LBA],ax
;	mov [DAP_LBA + 2],dx
;	mov ax,[bp + 6]
;	mov dx,[bp + 8]
;	mov cx,Sec2BlockShift
;	mov [DAP_Buf_Off],ax
;	shl dx,cl
;	mov [DAP_BlockCnt],dx
;	mov ah,0x43
;	mov dl,0x80
;	mov si,DAP
;	int 0x13
;	jc _CanNotOperateDisk
;	pop si
;	pop bp
;	ret 6		
	
	
_CanNotOperateDisk:
	push CanNotOperateDiskStr
	call Print
	call Stop
	
Unsuppose:
	push UnsupposeStr
	call Print
	call Stop
	
delay:
	nop
	nop
	nop
	nop
	ret	

NoLM:
	push NoLMStr
	call Print
	call Stop
	
NotSuppostVESA:
	push NotSuppostVESAStr
	call Print
	call Stop
	
align 16
[bits 32]
_In32Bit:		;在此处进入32bit
	mov eax,0x10		
	mov ds,eax	;Load Selector
	mov es,eax
	mov fs,eax
	mov gs,eax
	mov ss,eax
	mov esp,0x9effc	;Stack point
	mov eax,[0x1003c]
	add eax,0x10000
	mov ecx,[eax + 0x28]
	add ecx,0x10000
	movzx eax,word [BlockCnt + (LodSeg << 4)]
	push eax		;BlockCount
	push dword [(LodSeg << 4) + VESAModeCnt]
	push dword [(LodSeg << 4) + VESAModeInfoAddr]
	push dword [(LodSeg << 4) + VESAInfoAddr]
	push dword [(LodSeg << 4) + SmapCnt]
	push dword [(LodSeg << 4) + SMAPAddr]
	push 0x7c00 + 496
	push 0x7c00 + 480
	call ecx	;void EntryPoint(GUID * Disk,GUID * Partition,struct SMAP * SMAPAddr, int SMAPCnt
					;struct VESAInfo * VESAInfoAddr,struct Mode * VESAModeInfoAddr,
					;int VESAModeCnt,int BlockCnt);	
stop1:			;实际上是不会执行到这里的，因为loder manger是不会返回的，当然如果它出错了那就可能返回到这个地址了。
	hlt
	jmp stop1
	
	
align 512
____End:
;Quasimodo
