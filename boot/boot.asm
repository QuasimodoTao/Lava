;
;	boot/boot.asm
;	Copyright (C) 2020  Quasimodo
;
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <https://www.gnu.org/licenses/>.
;


;struct DataHeader{
;	word MinVer;
;	word MajVer;
;	dword BlockCnt;
;	qword SecCnt;
;	
;	dword Node0Block;
;	dword Rvd0;
;	dword Rvd1;
;	dword Rvd2;
;	
;	dword Rvd3
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

%define SecSize	512
%define Version	0x0001
%define LodOff	0x0000
%define LodSeg	0x9000
%define SecCnt	(____End - $$)/SecSize
%define SupDataStart 8

%define VESAInfoSeg		0x0080
%define CurModeInfoSeg	0x00a0
%define sp16			0xfbfe
%define sp32			0x9fbfc
%define sp64			0x0fff8
%define _SMAPSeg		0x00b0
%define DefMode			0x0118
%define OrginalData		0x00000500

%define Sec2BlockShift	3

%define BootNode 			4096;128
%define BootBlock 			4096+128;256
%define FirstLBA			4096+256+128;8分区的第一个扇区号
%define ImageBase			4096+256+128+8;8
%define EntryPoint			4096+256+128+16;8
%define BlockCnt 			4096+256+128+24;4
%define SectionRemind		4096+256+128+28;2
%define SectionCount		4096+256+128+30;2
%define SectionTableBase	4096+256+128+34;2

[CPU X64]
[bits 16]
org 0x90000;0x9000:0x0000

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
align 8
GDTR:	dw	0x27,GDT,LodSeg >> 12
align 8
IDTR:	dw	0,0,0
align 8		
GDT:	dq	0
		db	0x00,0x00,0x00,0x00,0x00,0x9a,0x20,0x00;64-bit DPL:0 Code
		db	0x00,0x00,0x00,0x00,0x00,0x92,0x20,0x00;64-bit DPL:0 Data
		db	0xff,0xff,0x00,0x00,0x00,0x9a,0xcf,0x00;32-bit DPL:0 Code
		db	0xff,0xff,0x00,0x00,0x00,0x92,0xcf,0x00;32-bit DPL:0 Data
align 8
DiskGUID:			dq	0,0
PartGUID:			dq	0,0
SmapCnt:			dw	0
SMAPSeg:			dw	_SMAPSeg
ModeSeg:			dw	CurModeInfoSeg
FirstFreeSeg:		dw	0x01000
PCIBIOSEnt:			dd	0
PCIax:				dw	0xffff
PCIbx:				dw	0
PCIcx:				dw	0
_VESAInfoSeg:		dw	VESAInfoSeg

align 2
CanNotOperateDiskStr:	db	"Can't read disk.",0x0a,0x0d,0
align 2
UnsupposeStr:			db	"Unsuppose LM32 file.",0x0a,0x0d,0
align 2
NoLMStr:				db	"LMNode not exist.",0x0a,0x0d,0
align 2
NotSuppostVESAStr:		db	"Unsuppose version of VESA.",0x0d,0x0a,0
align 2
VideoModeFailStr:		db	"Get or set VESA mode fail.",0x0d,0x0a,0
align 2
OverSizeStr:			db	"Boot file over size,must less than 256KB.",0x0d,0x0a,0
align 2
UnsupposeImageStr:		db	"Unsuppose excutive image.",0x0d,0x0a,0
align 2
RelocationName:			db	".reloc",0,0
align 2
UnsupposeSMAPStr:		db	"Muchine not suppose SMAP.",0x0d,0x0a,0
align 2
Unsuppose8086Str:		db	"Lava OS can not run on 8086 processor.",0x0d,0x0a,0
align 2
Unsuppose32CPUStr:		db	"Lava OS can not run on 32-bits processor(s).",0x0d,0x0a,0
align 2
UnsupposeCPUStr:		db	"Lava OS can not run on this processor(s).",0x0d,0x0a,0
align 2
HexStr:	db	"0123456789ABCDEF"


align 16
Entry:
	cli
	mov bx,LodSeg;初始化堆栈空间和段寄存器
	mov es,bx
	mov ss,bx
	mov sp,sp16;堆栈指针
	sti
	
	mov [es:FirstLBA],ax		;保存当前分区起始逻辑扇区号，0x00:cx:dx:ax
	mov [es:FirstLBA + 2],dx
	mov [es:FirstLBA + 4],cx
	mov word [es:FirstLBA + 6],0
	xor ax,ax	;复制磁盘和分区的GUID
	mov ds,ax
	mov si,0x7c00 + 480
	mov cx,16
	mov di,DiskGUID
	rep movsw
	mov ds,bx
	
	pushf
	pop ax
	and ax,0x9000
	cmp ax,0x9000
	je Unsuppose8086
	
	pushfd
	pop eax
	btr eax,21
	push eax
	popfd
	pushfd
	pop eax
	bts eax,21
	jc Unsuppose32CPU
	push eax
	popfd
	pushfd
	pop eax
	bt eax,21
	jnc Unsuppose32CPU
	
	xor eax,eax
	inc eax
	cpuid
	and eax,0x00000f00
	cmp eax,0x00000600
	jne UnsupposeCPU
	
	in al,0x92		;打开A20
	or al,2
	out 0x92,al
	
	lgdt [ss:GDTR]		;加载GDTR，进入32必须的

	mov esi,0
	mov edi,0x100000
	mov ecx,4096

	push 1;Count
	push 8192;pBlock
	push 0;iBlock
	push 1;ReadBlock(1,8192,1);
	call ReadBlock;读取块1到0x9000:0x2000
	mov ebx,[8192 + 36];LMNode
	test ebx,ebx
	jz NoLM;若LMNode为0，则表示不存在LM
	mov eax,[8192 + 16];Node0Block
	push eax
	push 1
	push 8192
	push eax;ReadBlock(eax,8192,1)
	call ReadBlock;读取Node0所在的块
	mov di,BootNode;复制Node0到BootNode，实际上为临时的
	mov si,8192
	mov cx,64
	rep movsw;复制Node0到0x9000:0x1000
	mov eax,ebx
	call ComputeBootNodeBlock;eax内为BootNode所在的块
	pop ecx
	cmp eax,ecx
	je SameBlock
	push 1
	push 8192
	push eax
	call ReadBlock;ReadBlock(eax,8192,1)
SameBlock:
	mov si,bx
	and si,0x1f
	shl si,7
	add si,8192
	mov di,BootNode;获取BootNode
	mov cx,64
	rep movsw
	
	mov eax,[BootNode + 24];测试文件大小
	mov edx,[BootNode + 28]
	test edx,edx
	jnz BootOverSize
	cmp eax,262144;256KB
	ja BootOverSize
	test eax,eax
	jz NoLM
	call GetBootiBlock
	call LoadBootFile
	call DepressImage
	
;get SMAP
	mov ax,_SMAPSeg
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
	mov [ss:SmapCnt],bp
	jmp SMAPFin
Failed:
	push UnsupposeSMAPStr
	call Print
	call Stop
SMAPFin:
	mov ax,LodSeg
	mov ds,ax
	
	mov ax,0x4f02
	mov bx,DefMode
	int 0x10

	mov ax,CurModeInfoSeg
	mov es,ax
	mov ax,0x4f01
	mov cx,DefMode
	xor di,di
	int 0x10
	
	mov ax,VESAInfoSeg
	mov es,ax
	xor di,di
	mov ax,0x4f00
	int 0x10

;	call Stop
		
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
		
	mov	ax,1
	lmsw ax			;设置PE位，cr0寄存器的第0位
	jmp	dword 0x18:_In32Bit	;跳转到32位，

Unsuppose8086:
	push Unsuppose8086Str
	call Print
	call Stop
	
Unsuppose32CPU:
	push Unsuppose32CPUStr
	call Print
	call Stop

UnsupposeCPU:
	push UnsupposeCPUStr
	call Print
	call Stop


memclr:	;void stdcall memset(u16 off,u16 seg,u32 count);
	;call Stop
	push bp
	mov bp,sp
	push es
	push eax
	push edx
	push di
	push cx
	
	movzx edx,word [bp + 6]
	movzx eax,word [bp + 4]
	shl edx,4
	add edx,eax
	mov di,dx
	and di,0x0f
	shr edx,4
	mov es,dx
	mov edx,[bp +8]
	inc edx
	shr edx,1
	mov bp,di
	xor ax,ax
memclrnext:
	cmp edx,16284
	jnz memclrnorem
	sub edx,16384
	mov cx,16384
	mov di,bp
	rep stosw
	mov cx,es
	add cx,2048
	mov es,cx
	jmp memclrnext
memclrnorem:
	test edx,edx
	jz memclrend
	mov di,bp
	mov cx,dx
	rep stosw
memclrend:
	pop cx
	pop di
	pop edx
	pop eax
	pop es
	pop bp
	ret 8

Failed1:
	push VideoModeFailStr
	call Print
	call Stop

BootOverSize:
	push OverSizeStr
	call Print
	call Stop
Stop:
	hlt
	jmp Stop
	
UnsupposeImage:
	push UnsupposeImageStr
	call Print
	call Stop
	
Print:	;void stdcall Print(char * Str)
	push bp
	mov bp,sp
	push si
	push ax
	push ds
	mov si,[bp + 4]
	mov ax,LodSeg
	mov ds,ax
	mov ah,0x0e
_PrintNext:
	lodsb
	test al,al
	jz _PrintEnd
	int 0x10
	jmp _PrintNext
_PrintEnd:
	pop ds
	pop ax
	pop si
	pop bp
	ret 2
	
PutChar:	;void fastcall PutChar(char ch);
	push ax
	mov ah,0x0e
	int 0x10
	pop ax
	ret

ReadBlock:	;int stdcall ReadBlock(long iBlock,void * pBlock,unsigned char Count);
	push bp
	mov bp,sp
	push si
	push eax
	push edx
	push ecx
	mov cx,Sec2BlockShift
	mov eax,[bp + 4] ;iBlock
	xor edx,edx	
_ReadBlockl1:
	test cx,cx
	jz _ReadBlockl0
	shl eax,1
	rol edx,1
	dec cx
	jmp _ReadBlockl1
_ReadBlockl0:
	add eax,[FirstLBA]
	adc edx,[FirstLBA + 4]
	mov [DAP_LBA],eax
	mov [DAP_LBA + 4],edx
	mov ax,[bp + 8];pBlock
	movzx dx,byte [bp + 10];Count
	mov cx,Sec2BlockShift
	mov [DAP_Buf_Off],ax
	shl dx,cl
	mov [DAP_BlockCnt],dx
	mov ah,0x42
	mov dl,0x80
	mov si,DAP
	int 0x13
	jc _CanNotOperateDisk
	pop ecx
	pop edx
	pop eax
	pop si
	pop bp
	ret 8	
	
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
	
ComputeBootNodeBlock:;计算LMNode所在的块号
	;int32_t fastcall ComputeBootNodeBlock(int32_t iNode:eax)
	push esi
	push ecx
	shr eax,5
	cmp eax,16
	jb ComputeNodeBlockLev0;直接指针
	sub eax,16
	cmp eax,1024
	jb ComputeNodeBlockLev1;一级指针
	sub eax,1024
	cmp eax,1024*1024
	jb ComputeNodeBlockLev2;二级指针
	sub eax,1024*1024
	push eax
	push 1
	push 8192
	mov ecx,[BootNode + 52 + 16 * 4 + 8];三级指针
	push ecx
	call ReadBlock;ReadBlock(BootNode.iBlock[18],8192,1)
	pop eax
	mov esi,eax
	shr esi,18
	and si,0xfffc
	push eax
	push 1
	push 8192
	mov ecx,[8192 + si]
	push ecx
	call ReadBlock;ReadBlock(((int32_t*)8192)[(eax >> 20) & 0x3ff],8192,1)
	pop eax
	mov esi,eax
	shr esi,10
	and esi,0x1ff
	shl si,2
	push eax
	push 1
	push 8192
	mov ecx,[8192 + si]
	push ecx
	call ReadBlock;ReadBlock(((int32_t*)8192)[(eax >> 10) & 0x3ff],8192,1)
	pop eax
	mov si,ax
	and si,0x1ff
	shl si,2
	mov eax,[8192 + si];eax = ((int32_t*)8192)[eax & 0x3ff]
	pop ecx
	pop esi
	ret
ComputeNodeBlockLev2:
	push eax
	push 1
	push 8192
	mov ecx,[BootNode + 52 + 16 * 4 + 4]
	push ecx
	call ReadBlock;ReadBlock(BootNode.iBlock[17],8192,1)
	pop eax
	mov esi,eax
	shr esi,10
	shl si,2
	mov ecx,[8192 + si]
	push eax
	push 1
	push 8192
	push ecx
	call ReadBlock;ReadBlock(((int32_t*)8192)[(eax >> 10) & 0x3ff],8192,1)
	pop eax
	mov si,ax
	and si,0x1ff
	shl si,2
	mov eax,[8192 + si];eax = ((int32_t*)8192)[eax & 0x3ff]
	pop ecx
	pop esi
	ret
ComputeNodeBlockLev1:
	mov ecx,[BootNode + 52 + 16 * 4];ReadBlock(BootNode.iBlock[16],8192,1)
	push eax
	push 1
	push 8192
	push ecx
	call ReadBlock
	pop eax
	mov si,ax
	shl si,2
	mov eax,[8192 + si];eax = ((int32_t*)8192)[eax & 0x3ff]
	pop ecx
	pop esi
	ret
ComputeNodeBlockLev0:
	shl eax,2
	mov si,ax
	mov eax,[BootNode + 52 + si];eax = BootNode.iBlock[eax]
	pop ecx
	pop esi
	ret
	
GetBootiBlock:
	;int GetBootiBlock();
	push si
	push di
	push cx
	push edx
	mov si,BootNode + 52
	mov di,BootBlock
	mov cx,32
	rep movsw;0-15Block
	lodsd
	test eax,eax
	jz GetAllBlockEnd
	push 1
	push 8192
	push eax
	call ReadBlock
	mov si,8192
	mov cx,96
	rep movsw
GetAllBlockEnd:
	mov edx,[BootNode + 24]
	add edx,8191
	shr edx,12
	mov ax,dx
	pop edx
	pop cx
	pop di
	pop si
	ret
	
LoadBootFile:
	;void fastcall LoadBootFile(int BlockCount);
	push di
	push bx
	push ax
	mov [BlockCnt],ax
	mov di,ax
	mov bx,BootBlock;装载LM到内存，但不展开到对齐
	mov word [DAP_Buf_Seg],0x5000
LoadFilel1:
	test di,di
	jz LoadFilel0
	dec di
	push 1
	push 0
	push word [bx + 2]
	push word [bx]
	call ReadBlock
	add bx,4
	add word [DAP_Buf_Seg],0x100
	jmp LoadFilel1
LoadFilel0:
	mov word [DAP_Buf_Seg],LodSeg
	pop ax
	pop bx
	pop di
	ret
	
DepressImage:
	;void DepressImage();
	pushad
	
	mov ax,0x5000
	mov ds,ax
	xor si,si
	mov si,[si + 0x3c]
	mov ax,[si + 6]
	mov [ss:SectionRemind],ax
	mov [ss:SectionCount],ax
	mov ax,[si + 0x18]
	cmp ax,0x020b
	jne UnsupposeImage
	mov ax,[si + 4]
	cmp ax,0x8664
	jne UnsupposeImage
	mov eax,[si + 0x30];PE32+
	mov edx,[si + 0x34]
	mov [ss:ImageBase],eax
	mov [ss:ImageBase + 4],edx
	mov eax,[si + 0x28]
	add eax,[ss:ImageBase]
	mov [ss:EntryPoint],eax
	adc edx,0
	mov [ss:EntryPoint + 4],edx
	add si,0x108
	mov bx,si
	mov [ss:SectionTableBase],bx
NextSection:
	mov bp,[ss:SectionRemind]
	
	test bp,bp
	jz LoadSectionEnd
	dec bp
	mov [ss:SectionRemind],bp
	mov ax,0x5000
	mov ds,ax
	
	mov eax,[ds:bx + 12];VirtualAddress
	mov edx,[ds:bx + 20];PointerToRawData
	mov ecx,[ds:bx + 16];SizeOfRawData
	
	add eax,[ss:ImageBase]
	
	push dword [ds:bx + 8]
	
	mov ebp,eax
	add ebp,ecx
	add ebp,0x0f
	shr ebp,4
	cmova bp,[ss:FirstFreeSeg]
	mov [ss:FirstFreeSeg],bp
	
	add edx,0x50000
	mov si,dx
	and si,0x0f
	shr edx,4
	mov ds,dx;di:si
	
	mov di,ax
	and di,0x0f
	shr eax,4
	mov es,ax;es:di
	
	push es
	push di
	call memclr
	
	add bx,40
	mov ebp,ecx
	inc ebp
	shr ebp,1
NextSeg:
	cmp ebp,16384
	jna NoRem
	sub ebp,16384
	mov cx,16384
	mov ax,si
	mov dx,di
	rep movsw
	mov si,ax
	mov di,dx
	mov cx,ds
	mov ax,es
	add cx,2048
	add ax,2048
	mov ds,cx
	mov es,ax
	jmp NextSeg
NoRem:
	mov cx,bp
	test cx,cx
	jz NextSection
	rep movsw
	jmp NextSection
LoadSectionEnd:	
	mov ax,LodSeg
	mov ds,ax
	mov es,ax
	popad
	ret

align 16
[bits 32]
_In32Bit:		;在此处进入32bit
	mov eax,0x20		
	mov ds,eax	;Load Selector
	mov es,eax
	mov fs,eax
	mov gs,eax
	mov ss,eax
	mov esp,sp32	;Stack point
	
	mov esi,0x000dfff0
SBIOS32l1:
	add esi,0x10
	cmp esi,0x000ffff0
	jae TestPCIEnd
	cmp dword [esi],0x5f32335f
	jne SBIOS32l1
	cmp byte [esi + 9],1
	jne SBIOS32l1
	xor eax,eax
	mov ecx,0x10
SBIOS32l2:
	add al,[esi + ecx - 1]
	loop SBIOS32l2
	test al,al
	jnz SBIOS32l1
	cmp byte [esi + 8],0
	jne TestPCIEnd
	mov ebp,[esi + 4]
	
	mov eax,0x49435024
	xor ebx,ebx
	push 0x18
	push ebp
	call far [esp]
	test eax,0xff
	jnz TestPCIEnd
	mov ebp,ebx
	add ebp,edx
	pop eax
	mov ax,0xb101
	push ebp
	mov [PCIBIOSEnt],ebp
	call far [esp]
	add esp,8
	cmp edx,0x20494350
	jne TestPCIEnd
	mov word [PCIax],ax
	mov word [PCIbx],bx
	mov word [PCIcx],cx
TestPCIEnd:
	mov esi,DiskGUID
	mov edi,OrginalData
	mov ecx,64
	rep movsd
	
	mov edi,0x2000
	mov cr3,edi
	xor eax,eax
	mov ecx,2*4096/4
	rep stosd
	mov dword [0x2000],0x3003
	mov dword [0x2800],0x3003
	mov dword [0x3000],0x4003
	mov dword [0x3008],0x5003
	mov dword [0x3010],0x6003
	mov dword [0x3018],0x7003
	
	mov eax,0x00000083
	mov edi,0x00004000
	mov ecx,2048
next_2M_page:
	mov [edi],eax
	mov dword [edi + 4],0
	add eax,0x200000
	lea edi,[edi + 8]
	loop next_2M_page
	
	mov ecx,cr4
	or ecx,0x20
	mov cr4,ecx
	
	mov ecx,0xc0000080
	rdmsr
	or eax,0x100
	wrmsr
	
	mov edx,cr0
	and edx,0x1fffffff
	or edx,0x80000000
	mov cr0,edx
	jmp dword 0x08:in64bit
	
align 16
[bits 64]
in64bit:
	mov esp,sp64
	mov ax,0x10
	mov ss,ax
	mov rax,EntryPoint + (LodSeg << 4)
	mov rax,[rax]
	mov ecx,OrginalData
	;hlt
	jmp rax
	
align 512
____End:
