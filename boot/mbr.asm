;Lava OS
;PageFault
;20-04-11-14-58

;MBR for GPT solution.

;word Off		偏移
;word Seg		段
;word entry		入口
;word SupData	分区数据头对应逻辑扇区号，对齐到4096字节
;word rvd		保留
;word OddXor	奇校验

;此段代码将加载启动分区以逻辑扇区0开始的代码到其指定的地址上
;并交予控制权，代码不应超过32KB，即64个扇区

;此段代码仅用于硬盘，不可用于软盘

%define SectorSizePow	9


[CPU 686]
[bits 16]
org 0x7c00	;0x0000:0x7c00

	cli
	xor ax,ax
	mov ds,ax
	mov es,ax
	mov ss,ax
	mov sp,0xfffe
	sti
	mov ah,0x41		;测试是否支持LBA模式读取硬盘
	mov dl,0x80
	mov bx,0x55aa
	int 0x13
	jc _NotSupposeLBAMode
	
	mov ah,0x42		;读取GPT Header
	mov si,DAP
	int 0x13
	jc _CanNotReadDisk
	
	mov si,0x8000	;测试GPT头是否可用，即GPT头标识是否合法
	mov di,GRUBMagic
	mov cx,0x04
	repe cmpsw
	test cx,cx
	jnz _BadGPTHeader
	
	mov si,72 + 0x8000	;获得分区表第一个逻辑扇区号
	mov di,DAP_LBA
	mov cl,4
	rep movsw
	
	mov si,0x8000 + 56	;保存磁盘GUID，以确定启动的磁盘
	mov di,BootDiskGUID
	mov cl,8
	rep movsw
	
	mov ax,[0x8000 + 80]	;计算已存在的分区表共有过大，即多少个扇区;项数
	cmp dword [0x8000 + 84],0x0080	;项大小，都是128
	jne _UnsupposeVersion
	mov [PartitionCnt],al	;保存分区个数，用于在后面搜索启动分区用
	shr ax,SectorSizePow - 7	;项大小，都是128
	inc ax
	mov [DAP_BlockCnt],ax

	mov si,DAP				;读取分区表
	mov ah,0x42
	int 0x13
	jc _CanNotReadDisk

	mov bx,128
	mov di,0x8000			;逐个测试分标识，判断是否是启动分区
	mov dl,[PartitionCnt]
NextPartition:
	mov cx,8
	mov si,BPGUID
	push di
	repe cmpsw
	test cx,cx
	jz FindIt
	test dl,dl
	jz _CanNotFindPartition
	dec dl
	pop di
	add di,bx
	jmp NextPartition
	
FindIt:						;找到启动分区
	pop si

	mov cl,8				;保存启动分区GUID，以确定启动的扇区
	add si,16
	mov di,BootPartitionGUID
	rep movsw
	mov di,DAP_LBA			;读取一个扇区
	mov cl,4
	rep movsw
	mov word [DAP_BlockCnt],4096 >> SectorSizePow
	mov si,DAP
	mov ah,0x42
	mov dl,0x80
	int 0x13
	jc _CanNotReadDisk
	
	mov di,0x8000			;奇校验8个字
	mov cx,8
	xor ax,ax
NextWord:
	xor ax,[di]
	inc di
	inc di
	loop NextWord
	not ax
	test ax,ax
	jnz _BadBootPartition	;校验失败则说明数据损坏
	mov si,0x8000
	mov di,[si + 4]
	mov es,[si + 6]
	mov dx,[si + 8]	;代码入口
	mov cx,2048
	rep movsw
	push es
	push dx
	mov ax,[DAP_LBA]
	mov dx,[DAP_LBA + 2]
	mov cx,[DAP_LBA + 4]
	retf
	
_UnsupposeVersion:
	mov si,UnsupposeVersionStr
	jmp Print
_BadBootPartition:
	mov si,BadBootPartitionStr
	jmp Print
_CanNotFindPartition:
	mov si,CanNotFindPartitionStr
	jmp Print
_BadGPTHeader:
	mov si,BadGPTHeaderStr
	jmp Print
_NotSupposeLBAMode:
	mov si,NotSupposeLBAModeStr
	jmp Print
_CanNotReadDisk:
	mov si,CanNotReadDiskStr
Print:
	;ds:si
	mov ah,0x0e
_PrintNext:
	lodsb
	test al,al
	jz _PrintEnd
	int 0x10
	jmp _PrintNext
_PrintEnd:
Stop:
	hlt
	jmp Stop

PartitionCnt:	db	0
align 16
DAP:				;用于LBA模式读取硬盘的数据结构
DAP_Size:		db	16
DAP_Rvd:		db	0
DAP_BlockCnt:	dw	1
DAP_Buf_Off:	dw	0x8000
DAP_Buf_Seg:	dw	0
DAP_LBA:		dq	1


NotSupposeLBAModeStr:	db	"Not suppose LBA mode",0
CanNotReadDiskStr:		db	"Can't read disk",0
BadGPTHeaderStr:		db	"Bad GPT",0
CanNotFindPartitionStr:	db	"No boot partition",0
BadBootPartitionStr:	db	"Bad boot partition",0
UnsupposeVersionStr:	db	"Unsuppose GPT version",0
GRUBMagic:				db	0x45,0x46,0x49,0x20,0x50,0x41,0x52,0x54
BPGUID:					db	0x32,0x4c,0x3a,0xa2,0x50,0x56,0xc6,0x47
						db	0xaf,0x40,0x5c,0xd9,0x48,0x78,0xc6,0x71

times 440 - ($-$$) db 0
	db	0,0,0,0,0,0
	db	0x00,0x00,0x02,0x00,0xee,0xff,0xff,0xff
	db	0x01,0x00,0x00,0x00,0xff,0xff,0xff,0xff
times 480 - ($-$$) db 0
BootDiskGUID:
times 496 - ($-$$) db 0
BootPartitionGUID:
times 510 - ($ - $$) db 0
db 0x55,0xaa	
	
;Quasimodo