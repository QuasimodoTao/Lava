//Lava OS
//PageFault
//20-04-11-15-08

#include <vm.h>
#include <config.h>
#include "ins.h"

#ifdef VM_DEBUG
const char * REG8[] = { "AL","CL","DL","BL","AH","CH","DH","BH" };
const char * REG16[] = { "AX","CX","DX","BX","SP","BP","SI","DI"};
const char * REG32[] = { "EAX","ECX","EDX","EBX","ESP","EBP","ESI","EDI" };
const char * SREG[] = { "ES","CS","SS","DS","FS","GS" };
const char * RM16[] = { "BX + SI","BX + DI","BP + SI","BP + DI","SI","DI","BP","BX" };
#endif

struct _OPC_ OPB[] = {
	//void (*RR16)(struct _INS_ *);//r,r/no/r,i/m,i/
	//void(*RR32)(struct _INS_ *);
	//void(*MR16)(struct _INS_ *);
	//void(*MR32)(struct _INS_ *);
	//char * Name16;
	//char * Name32;
	{ADD_RbRb,ADD_RbRb,ADD_MbRb,ADD_MbRb},//00 /r add r/m8,r8
	{ADD_RwRw,ADD_RdRd,ADD_MwRw,ADD_MdRd},//01 /r add r/m16/32,r16/32
	{ADD_RbRb,ADD_RbRb,ADD_RbMb,ADD_RbMb},//02 /r add r8,r/m8
	{ADD_RwRw,ADD_RdRd,ADD_RwMw,ADD_RdMd},//03 /r add r16/32,r/m16/32
	{ADD_AbIb,ADD_AbIb,ADD_AbIb,ADD_AbIb},//04 ib add al,imm8
	{ADD_AwIw,ADD_AdId,ADD_AwIw,ADD_AdId},//05 iw add [e]ax,imm16/32
	{PUSH_Sr,PUSH_Sr,PUSH_Sr,PUSH_Sr},//06 push es
	{POP_Sr,POP_Sr,POP_Sr,POP_Sr},//07 pop es
	{OR_RbRb,OR_RbRb,OR_MbRb,OR_MbRb},//08 /r or r/m8,r8
	{OR_RwRw,OR_RdRd,OR_MwRw,OR_MdRd},//09 /r or r/m16/32,r16/32
	{OR_RbRb,OR_RbRb,OR_RbMb,OR_RbMb},//0a /r or r8,r/m8
	{OR_RwRw,OR_RdRd,OR_RwMw,OR_RdMd},//0b /r or r16/32,r/m16/32
	{OR_AbIb,OR_AbIb,OR_AbIb,OR_AbIb},//0c ib or al,imm8
	{OR_AwIw,OR_AdId,OR_AwIw,OR_AdId},//0d iw or [e]ax,imm16/32
	{PUSH_Sr,PUSH_Sr,PUSH_Sr,PUSH_Sr},//0e push cs
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//0f 

	{ADC_RbRb,ADC_RbRb,ADC_MbRb,ADC_MbRb},//10 /r adc r/m8,r8
	{ADC_RwRw,ADC_RdRd,ADC_MwRw,ADC_MdRd},//11 /r adc r/m16/32,r16/32
	{ADC_RbRb,ADC_RbRb,ADC_RbMb,ADC_RbMb},//12 /r adc r8,r/m8
	{ADC_RwRw,ADC_RdRd,ADC_RwMw,ADC_RdMd},//13 /r adc r16/32,r/m16/32
	{ADC_AbIb,ADC_AbIb,ADC_AbIb,ADC_AbIb},//14 ib adc al,imm8
	{ADC_AwIw,ADC_AdId,ADC_AwIw,ADC_AdId},//15 iw adc [e]ax,imm16/32
	{PUSH_Sr,PUSH_Sr,PUSH_Sr,PUSH_Sr},//16 push ss
	{POP_Sr,POP_Sr,POP_Sr,POP_Sr},//17 pop ss
	{SBB_RbRb,SBB_RbRb,SBB_MbRb,SBB_MbRb},//18 /r sbb r/m8,r8
	{SBB_RwRw,SBB_RdRd,SBB_MwRw,SBB_MdRd},//19 /r sbb r/m16/32,r16/32
	{SBB_RbRb,SBB_RbRb,SBB_RbMb,SBB_RbMb},//1a /r sbb r8,r/m8
	{SBB_RwRw,SBB_RdRd,SBB_RwMw,SBB_RdMd},//1b /r sbb r16/32,r/m16/32
	{SBB_AbIb,SBB_AbIb,SBB_AbIb,SBB_AbIb},//1c ib sbb al,imm8
	{SBB_AwIw,SBB_AdId,SBB_AwIw,SBB_AdId},//1d iw sbb [e]ax,imm16/32
	{PUSH_Sr,PUSH_Sr,PUSH_Sr,PUSH_Sr},//1e push ds
	{ POP_Sr,POP_Sr,POP_Sr,POP_Sr},//1f pop ds

	{AND_RbRb,AND_RbRb,AND_MbRb,AND_MbRb},//20 /r and r/m8,r8
	{AND_RwRw,AND_RdRd,AND_MwRw,AND_MdRd},//21 /r and r/m16/32,r16/32
	{AND_RbRb,AND_RbRb,AND_RbMb,AND_RbMb},//22 /r and r8,r/m8
	{AND_RwRw,AND_RdRd,AND_RwMw,AND_RdMd},//23 /r and r16/32,r/m16/32
	{AND_AbIb,AND_AbIb,AND_AbIb,AND_AbIb},//24 ib and al,imm8
	{AND_AwIw,AND_AdId,AND_AwIw,AND_AdId},//25 iw and [e]ax,imm16/32
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//26 ES prex
	{DAA_No,DAA_No,DAA_No,DAA_No},//27 daa
	{SUB_RbRb,SUB_RbRb,SUB_MbRb,SUB_MbRb},//28 /r sub r/m8,r8
	{SUB_RwRw,SUB_RdRd,SUB_MwRw,SUB_MdRd},//29 /r sub r/m16/32,r16/32
	{SUB_RbRb,SUB_RbRb,SUB_RbMb,SUB_RbMb},//2a /r sub r8,r/m8
	{SUB_RwRw,SUB_RdRd,SUB_RwMw,SUB_RdMd},//2b /r sub r16/32,r/m16/32
	{SUB_AbIb,SUB_AbIb,SUB_AbIb,SUB_AbIb},//2c ib sub al,imm8
	{SUB_AwIw,SUB_AdId,SUB_AwIw,SUB_AdId},//2d iw sub [e]ax,imm16/32
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//2e CS prex
	{DAS_No,DAS_No,DAS_No,DAS_No},//2f das

	{XOR_RbRb,XOR_RbRb,XOR_MbRb,XOR_MbRb},//30 /r xor r/m8,r8
	{XOR_RwRw,XOR_RdRd,XOR_MwRw,XOR_MdRd},//31 /r xor r/m16/32,r16/32
	{XOR_RbRb,XOR_RbRb,XOR_RbMb,XOR_RbMb},//32 /r xor r8,r/m8
	{XOR_RwRw,XOR_RdRd,XOR_RwMw,XOR_RdMd},//33 /r xor r16/32,r/m16/32
	{XOR_AbIb,XOR_AbIb,XOR_AbIb,XOR_AbIb},//34 ib xor al,imm8
	{XOR_AwIw,XOR_AdId,XOR_AwIw,XOR_AdId},//35 iw xor [e]ax,imm16/32
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//36 SS prex
	{AAA_No,AAA_No,AAA_No,AAA_No},//37 aaa
	{CMP_RbRb,CMP_RbRb,CMP_MbRb,CMP_MbRb},//38 /r cmp r/m8,r8
	{CMP_RwRw,CMP_RdRd,CMP_MwRw,CMP_MdRd},//39 /r cmp r/m16/32,r16/32
	{CMP_RbRb,CMP_RbRb,CMP_RbMb,CMP_RbMb},//3a /r cmp r8,r/m8
	{CMP_RwRw,CMP_RdRd,CMP_RwMw,CMP_RdMd},//3b /r cmp r16/32,r/m16/32
	{CMP_AbIb,CMP_AbIb,CMP_AbIb,CMP_AbIb},//3c ib cmp al,imm8
	{CMP_AwIw,CMP_AdId,CMP_AwIw,CMP_AdId},//3d iw cmp [e]ax,imm16/32
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//3e DS prex
	{AAS_No,AAS_No,AAS_No,AAS_No},//3f das

	{INC_Rw,INC_Rd,INC_Rw,INC_Rd},//40 inc [e]ax
	{INC_Rw,INC_Rd,INC_Rw,INC_Rd},//41 inc [e]cx
	{INC_Rw,INC_Rd,INC_Rw,INC_Rd},//42 inc [e]dx
	{INC_Rw,INC_Rd,INC_Rw,INC_Rd},//43 inc [e]bx
	{INC_Rw,INC_Rd,INC_Rw,INC_Rd},//44 inc [e]sp
	{INC_Rw,INC_Rd,INC_Rw,INC_Rd},//45 inc [e]bp
	{INC_Rw,INC_Rd,INC_Rw,INC_Rd},//46 inc [e]si
	{INC_Rw,INC_Rd,INC_Rw,INC_Rd},//47 inc [e]di
	{DEC_Rw,DEC_Rd,DEC_Rw,DEC_Rd},//48 dec [e]ax
	{DEC_Rw,DEC_Rd,DEC_Rw,DEC_Rd},//49 dec [e]cx
	{DEC_Rw,DEC_Rd,DEC_Rw,DEC_Rd},//4a dec [e]dx
	{DEC_Rw,DEC_Rd,DEC_Rw,DEC_Rd},//4b dec [e]bx
	{DEC_Rw,DEC_Rd,DEC_Rw,DEC_Rd},//4c dec [e]sp
	{DEC_Rw,DEC_Rd,DEC_Rw,DEC_Rd},//4d dec [e]bp
	{DEC_Rw,DEC_Rd,DEC_Rw,DEC_Rd},//4e dec [e]si
	{DEC_Rw,DEC_Rd,DEC_Rw,DEC_Rd},//4f dec [e]di

	{PUSH_Rw,PUSH_Rd,PUSH_Rw,PUSH_Rd},//50 push [e]ax
	{PUSH_Rw,PUSH_Rd,PUSH_Rw,PUSH_Rd},//51 push [e]cx
	{PUSH_Rw,PUSH_Rd,PUSH_Rw,PUSH_Rd},//52 push [e]dx
	{PUSH_Rw,PUSH_Rd,PUSH_Rw,PUSH_Rd},//53 push [e]bx
	{PUSH_Rw,PUSH_Rd,PUSH_Rw,PUSH_Rd},//54 push [e]sp
	{PUSH_Rw,PUSH_Rd,PUSH_Rw,PUSH_Rd},//55 push [e]bp
	{PUSH_Rw,PUSH_Rd,PUSH_Rw,PUSH_Rd},//56 push [e]si
	{PUSH_Rw,PUSH_Rd,PUSH_Rw,PUSH_Rd},//57 push [e]di
	{POP_Rw,POP_Rd,POP_Rw,POP_Rd},//58 pop [e]ax
	{POP_Rw,POP_Rd,POP_Rw,POP_Rd},//59 pop [e]cx
	{POP_Rw,POP_Rd,POP_Rw,POP_Rd},//5a pop [e]dx
	{POP_Rw,POP_Rd,POP_Rw,POP_Rd},//5b pop [e]bx
	{POP_Rw,POP_Rd,POP_Rw,POP_Rd},//5c pop [e]sp
	{POP_Rw,POP_Rd,POP_Rw,POP_Rd},//5d pop [e]bp
	{POP_Rw,POP_Rd,POP_Rw,POP_Rd},//5e pop [e]si
	{POP_Rw,POP_Rd,POP_Rw,POP_Rd},//5f pop [e]di

	{PUSHA_No,PUSHAD_No,PUSHA_No,PUSHAD_No},//60 pusha[d]
	{POPA_No,POPAD_No,POPA_No,POPAD_No},//61 popa[d]
	{BOUND_RwMd,BOUND_RdMq,BOUND_RwMd,BOUND_RdMq},//62 /r bound r16/32,m32/64
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//63 
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//64 FS prex
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//65 GS prex
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//66 OS prex
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//67 AS prex
	{PUSH_Iw,PUSH_Id,PUSH_Iw,PUSH_Id},//68 push imm16/32 
	{IMUL_RwRwIw,IMUL_RdRdId,IMUL_RwMwIw,IMUL_RdMdId},//69 /r id/iw imul r32/16,r/m32/16,imm32/16
	{PUSH_Ib,PUSH_Ib,PUSH_Ib,PUSH_Ib},//6a push imm8
	{IMUL_RwRwIb,IMUL_RdRdIb,IMUL_RwMwIb,IMUL_RdMdIb},//6b /r ib imul r16/32,r/m16/32,imm8
	{INSB_No,INSB_No,INSB_No,INSB_No},//6c insb
	{INSW_No,INSD_No,INSW_No,INSD_No},//6d insw/insd
	{OUTSB_No,OUTSB_No,OUTSB_No,OUTSB_No},//6e outsb
	{OUTSW_No,OUTSD_No,OUTSW_No,OUTSD_No},//6f outsw/outsd

	{JO_Ib,JO_Ib,JO_Ib,JO_Ib},//70 jo rel8
	{JNO_Ib,JNO_Ib,JNO_Ib,JNO_Ib},//71 jno rel8
	{JC_Ib,JC_Ib,JC_Ib,JC_Ib},//72 jc rel8
	{JNC_Ib,JNC_Ib,JNC_Ib,JNC_Ib},//73 jnc rel8
	{JZ_Ib,JZ_Ib,JZ_Ib,JZ_Ib},//74 jz rel8
	{JNZ_Ib,JNZ_Ib,JNZ_Ib,JNZ_Ib},//75 jnz rel8
	{JNA_Ib,JNA_Ib,JNA_Ib,JNA_Ib},//76 jna rel8
	{JA_Ib,JA_Ib,JA_Ib,JA_Ib},//77 ja rel8
	{JS_Ib,JS_Ib,JS_Ib,JS_Ib},//78  js rel8
	{JNS_Ib,JNS_Ib,JNS_Ib,JNS_Ib},//79 jns rel8
	{JP_Ib,JP_Ib,JP_Ib,JP_Ib},//7a jp rel8
	{JNP_Ib,JNP_Ib,JNP_Ib,JNP_Ib},//7b jnp rel8
	{JL_Ib,JL_Ib,JL_Ib,JL_Ib},//7c jl rel8
	{JNL_Ib,JNL_Ib,JNL_Ib,JNL_Ib},//7d jnl rel8
	{JNG_Ib,JNG_Ib,JNG_Ib,JNG_Ib},//7e jng rel8
	{JG_Ib,JG_Ib,JG_Ib,JG_Ib},//7f jg rel8

	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//group 0x80
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//group 0x81
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//82
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//group 0x83
	{TEST_RbRb,TEST_RbRb,TEST_MbRb,TEST_MbRb},//84 /r test r/m8,r8
	{TEST_RwRw,TEST_RdRd,TEST_MwRw,TEST_MdRd},//85 /r test r/m16/32,r16/32
	{XCHG_RbRb,XCHG_RbRb,XCHG_MbRb,XCHG_MbRb},//86 /r xchg r8,r/m8
	{XCHG_RwRw,XCHG_RdRd,XCHG_MwRw,XCHG_MdRd},//87 /r xchg r16/32,r/m16/32
	{MOV_RbRb,MOV_RbRb,MOV_MbRb,MOV_MbRb},//88 /r mov r/m8,r8
	{MOV_RwRw,MOV_RdRd,MOV_MwRw,MOV_MdRd},//89 /r mov r/m16/32,r16/32
	{MOV_RbRb,MOV_RbRb,MOV_RbMb,MOV_RbMb},//8a /r mov r8,r/m8
	{MOV_RwRw,MOV_RdRd,MOV_RwMw,MOV_RdMd},//8b /r mov r16/32,r/m16/32
	{MOV_RwSw,MOV_RwSw,MOV_MwSw,MOV_MwSw},//8c /r mov r/m16,sreg
	{LEA_RwMu,LEA_RdMu,LEA_RwMu,LEA_RdMu},//8d /r lea r16/32,m
	{MOV_SwRw,MOV_SwRw,MOV_SwMw,MOV_SwMw},//8e /r mov sreg,r/m16
	{POP_Rw,POP_Rd,POP_Mw,POP_Md},//8f /0 pop r/m16/32

	{NOP_No,NOP_No,NOP_No,NOP_No},//90 nop
	{XCHG_RwRw,XCHG_RdRd,XCHG_RwRw,XCHG_RdRd},//91 xchg [e]ax,[e]cx
	{XCHG_RwRw,XCHG_RdRd,XCHG_RwRw,XCHG_RdRd},//92 xchg [e]ax,[e]dx
	{XCHG_RwRw,XCHG_RdRd,XCHG_RwRw,XCHG_RdRd},//93 xchg [e]ax,[e]bx
	{XCHG_RwRw,XCHG_RdRd,XCHG_RwRw,XCHG_RdRd},//94 xchg [e]ax,[e]sp
	{XCHG_RwRw,XCHG_RdRd,XCHG_RwRw,XCHG_RdRd},//95 xchg [e]ax,[e]bp
	{XCHG_RwRw,XCHG_RdRd,XCHG_RwRw,XCHG_RdRd},//96 xchg [e]ax,[e]si
	{XCHG_RwRw,XCHG_RdRd,XCHG_RwRw,XCHG_RdRd},//97 xchg [e]ax,[e]di
	{CBW_No,CWDE_No,CBW_No,CWDE_No},//98 cbw/cwde
	{CWD_No,CDQ_No,CWD_No,CDQ_No},//99 cwd/cdq
	{CALL_IPw,CALL_IPd,CALL_IPw,CALL_IPd},//9a call 16:16/16:32
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//9b
	{PUSHF_No,PUSHFD_No,PUSHF_No,PUSHFD_No},//9c pushf[d]
	{POPF_No,POPFD_No,POPF_No,POPFD_No},//9d popf[d]
	{SAHF_No,SAHF_No,SAHF_No,SAHF_No},//9e sahf
	{LAHF_No,LAHF_No,LAHF_No,LAHF_No},//9f lahf

	{MOV_AbMb,MOV_AbMb,MOV_AbMb,MOV_AbMb},//a0 mov al,off8
	{MOV_AwMw,MOV_AdMd,MOV_AwMw,MOV_AdMd},//a1
	{MOV_MbAb,MOV_MbAb,MOV_MbAb,MOV_MbAb},//a2
	{MOV_MwAw,MOV_MdAd,MOV_MwAw,MOV_MdAd},//a3
	{MOVSB_No,MOVSB_No,MOVSB_No,MOVSB_No},//a4 movsb
	{MOVSW_No,MOVSD_No,MOVSW_No,MOVSD_No},//a5 movsw/movsd
	{CMPSB_No,CMPSB_No,CMPSB_No,CMPSB_No},//a6 cmpsb
	{CMPSW_No,CMPSD_No,CMPSW_No,CMPSD_No},//a7 cmpsw/cmpsd
	{TEST_AbIb,TEST_AbIb,TEST_AbIb,TEST_AbIb},//a8 test al,imm8
	{TEST_AwIw,TEST_AdId,TEST_AwIw,TEST_AdId},//a9 test [e]ax,imm16/32
	{STOSB_No,STOSB_No,STOSB_No,STOSB_No},//aa stosb
	{STOSW_No,STOSD_No,STOSW_No,STOSD_No},//ab stosw/stosd
	{LODSB_No,LODSB_No,LODSB_No,LODSB_No},//ac lodsb
	{LODSW_No,LODSD_No,LODSW_No,LODSD_No},//ad lodsw/lodsd
	{SCASB_No,SCASB_No,SCASB_No,SCASB_No},//ae scasb
	{SCASW_No,SCASD_No,SCASW_No,SCASD_No},//af scasw/scasd

	{MOV_RbIb,MOV_RbIb,MOV_RbIb,MOV_RbIb},//b0 mov al,imm8
	{MOV_RbIb,MOV_RbIb,MOV_RbIb,MOV_RbIb},//b1 mov cl,imm8
	{MOV_RbIb,MOV_RbIb,MOV_RbIb,MOV_RbIb},//b2 mov dl,imm8
	{MOV_RbIb,MOV_RbIb,MOV_RbIb,MOV_RbIb},//b3 mov bl,imm8
	{MOV_RbIb,MOV_RbIb,MOV_RbIb,MOV_RbIb},//b4 mov ah,imm8
	{MOV_RbIb,MOV_RbIb,MOV_RbIb,MOV_RbIb},//b5 mov ch,imm8
	{MOV_RbIb,MOV_RbIb,MOV_RbIb,MOV_RbIb},//b6 mov dh,imm8
	{MOV_RbIb,MOV_RbIb,MOV_RbIb,MOV_RbIb},//b7 mov bh,imm8
	{MOV_RwIw,MOV_RdId,MOV_RwIw,MOV_RdId},//b8 mov [e]ax,imm16/32
	{MOV_RwIw,MOV_RdId,MOV_RwIw,MOV_RdId},//b9 mov [e]cx,imm16/32
	{MOV_RwIw,MOV_RdId,MOV_RwIw,MOV_RdId},//ba mov [e]dx,imm16/32
	{MOV_RwIw,MOV_RdId,MOV_RwIw,MOV_RdId},//bb mov [e]bx,imm16/32
	{MOV_RwIw,MOV_RdId,MOV_RwIw,MOV_RdId},//bc mov [e]sp,imm16/32
	{MOV_RwIw,MOV_RdId,MOV_RwIw,MOV_RdId},//bd mov [e]bp,imm16/32
	{MOV_RwIw,MOV_RdId,MOV_RwIw,MOV_RdId},//be mov [e]si,imm16/32
	{MOV_RwIw,MOV_RdId,MOV_RwIw,MOV_RdId},//bf mov [e]di,imm16/32

	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//group 0xc0
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//group 0xc1
	{RET_NIw,RET_NIw,RET_NIw,RET_NIw},//c2 retn imm16
 	{RET_NNo,RET_NNo,RET_NNo,RET_NNo},//c3 retn
	{LES_RwMPw,LES_RdMPd,LES_RwMPw,LES_RdMPd},//c4 les r16/32,m16:16/32
	{LDS_RwMPw,LDS_RdMPd,LDS_RwMPw,LDS_RdMPd},//c5 lds r16/32,m16:16/32
	{MOV_RbRb,MOV_RbRb,MOV_MbRb,MOV_MbRb},//c6 mov r/m8,r8
	{MOV_RwIw,MOV_RdId,MOV_MwIw,MOV_MdId},//c7 mov r/m16/32,imm16/32
	{ENTER_IwIb,ENTER_IwIb,ENTER_IwIb,ENTER_IwIb},//c8 enter iw,ib
	{LEAVE_No,LEAVE_No,LEAVE_No,LEAVE_No},//c9 leave
	{RET_FIw,RET_FIw,RET_FIw,RET_FIw},//ca retf imm16
	{RET_FNo,RET_FNo,RET_FNo,RET_FNo},//cb retf
	{INT3_No,INT3_No,INT3_No,INT3_No},//cc int 3
	{INT_Ib,INT_Ib,INT_Ib,INT_Ib},//cd ib int imm8
	{INTO_No,INTO_No,INTO_No,INTO_No},//ce into
	{IRET_No,IRETD_No,IRET_No,IRETD_No},//cf iret/iretd

	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//group 0xd0
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//group 0xd1
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//group 0xd2
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//group 0xd3
	{AAM_Ib,AAM_Ib,AAM_Ib,AAM_Ib},//d4 aam ib
	{AAD_Ib,AAD_Ib,AAD_Ib,AAD_Ib},//d5 ib aad ib
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d6
	{XLATB_No,XLATB_No,XLATB_No,XLATB_No},//d7 xlatb
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d8
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d9
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//da
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//db
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//dc
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//dd
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//de
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//df

	{LOOPNE_Ib,LOOPNE_Ib,LOOPNE_Ib,LOOPNE_Ib},//e0 cb loopne rel8
	{LOOPE_Ib,LOOPE_Ib,LOOPE_Ib,LOOPE_Ib},//e1 cb loope rel8
	{LOOP_Ib,LOOP_Ib,LOOP_Ib,LOOP_Ib},//e2 loop rel8
	{JCXZ_Ib,JECXZ_Ib,JCXZ_Ib,JECXZ_Ib},//e3 j[e]cxz rel8
	{IN_AbIb,IN_AbIb, IN_AbIb, IN_AbIb},//e4 in al,imm8
	{IN_AwIb,IN_AdIb,IN_AwIb,IN_AdIb},//e5 in [e]ax,imm8
	{OUT_IbAb,OUT_IbAb, OUT_IbAb, OUT_IbAb},//e6 out imm8,al
	{OUT_IbAw,OUT_IbAd,OUT_IbAw,OUT_IbAd},//e7 out imm8 [e]ax
	{CALL_Iw,CALL_Id,CALL_Iw,CALL_Id},//e8 iw/id call rel16/32
	{JMP_Iw,JMP_Id,JMP_Iw,JMP_Id},//e9 jmp rel16/32
	{JMP_IPw,JMP_IPd,JMP_IPw,JMP_IPd},//ea jmp 16:16/16:32
	{JMP_Ib,JMP_Ib,JMP_Ib,JMP_Ib},//eb jmp rel8
	{IN_AbDw,IN_AbDw,IN_AbDw,IN_AbDw},//ec in al,dx
	{IN_AwDw,IN_AdDw,IN_AwDw,IN_AdDw},//ed in [e]ax,dx
	{OUT_DwAb,OUT_DwAb,OUT_DwAb,OUT_DwAb},//ee out dx,al
	{OUT_DwAw,OUT_DwAd,OUT_DwAw,OUT_DwAd},//ef out dx,[e]ax

	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f0 LOCK prex
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f1 unknow
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f2 repne prex
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f3 repe/rep prex
	{HLT_No,HLT_No,HLT_No,HLT_No},//f4 hlt
	{CMC_No,CMC_No,CMC_No,CMC_No},//f5 cmc
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f6 group 0xf6
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f7 group 0xf7
	{CLC_No,CLC_No,CLC_No,CLC_No},//f8 clc
	{STC_No,STC_No,STC_No,STC_No},//f9 stc
	{CLI_No,CLI_No,CLI_No,CLI_No},//fa cli
	{STI_No,STI_No,STI_No,STI_No},//fb sti
	{CLD_No,CLD_No,CLD_No,CLD_No},//fc cld
	{STD_No,STD_No,STD_No,STD_No},//fd std
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//fe group 0xfe
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR}//ff group 0xff
};
struct _OPC_ OPE[] = {
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//00 group 0x0f,0x00
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//01 group 0x0f,0x01
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//02
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//03
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//04
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//05
	{CLTS_No,CLTS_No,CLTS_No,CLTS_No},//06 clts
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//07
	{INVD_No,INVD_No,INVD_No,INVD_No},//08
	{WBINVD_No,WBINVD_No,WBINVD_No,WBINVD_No},//09 wbinvd
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//0a
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//0b
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//0c
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//0d
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//0e
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//0f

	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//10
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//11
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//12
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//13
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//14
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//15
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//16
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//17
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//18
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//19
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//1a
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//1b
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//1c
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//1d
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//1e
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//1f

	{MOV_RdCd,MOV_RdCd,MOV_RdCd,MOV_RdCd},//20 /r mov r32,crn
	{MOV_RdDd,MOV_RdDd,MOV_RdDd,MOV_RdDd},//21 /r mov r32,drn
	{MOV_CdRd,MOV_CdRd,MOV_CdRd,MOV_CdRd},//22 /r mov crn,r32
	{MOV_DdRd,MOV_DdRd,MOV_DdRd,MOV_DdRd},//23 /r mov drn,r32
	{MOV_RdTd,MOV_RdTd,MOV_RdTd,MOV_RdTd},//24 /r mov r32,trn
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//25
	{MOV_TdRd,MOV_TdRd,MOV_TdRd,MOV_TdRd},//26 /r mov trn,r32
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//27
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//28
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//29
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//2a
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//2b
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//2c
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//2d
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//2e
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//2f

	{WRMSR_No,WRMSR_No,WRMSR_No,WRMSR_No},//30 wrmsr
	{RDTSC_No,RDTSC_No,RDTSC_No,RDTSC_No},//31 rdtsc
	{RDMSR_No,RDMSR_No,RDMSR_No,RDMSR_No},//32 rdmsr
	{RDPMC_No,RDPMC_No,RDPMC_No,RDPMC_No},//33 rdpmc
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//34
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//35
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//36
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//37
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//38
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//39
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//3a
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//3b
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//3c
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//3d
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//3e
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//3f

	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//40
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//41
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//42
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//43
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//44
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//45
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//46
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//47
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//48
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//49
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//4a
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//4b
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//4c
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//4d
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//4e
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//4f

	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//50
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//51
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//52
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//53
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//54
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//55
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//56
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//57
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//58
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//59
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//5a
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//5b
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//5c
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//5d
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//5e
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//5f

	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//60
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//61
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//62
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//63
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//64
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//65
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//66
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//67
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//68
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//69
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//6a
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//6b
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//6c
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//6d
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//6e
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//6f

	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//70
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//71
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//72
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//73
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//74
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//75
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//76
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//77
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//78
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//79
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//7a
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//7b
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//7c
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//7d
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//7e
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//7f

	{JO_Iw,JO_Id,JO_Iw,JO_Id},//80 jo rel16/32
	{JNO_Iw,JNO_Id,JNO_Iw,JNO_Id},//81 jno rel16/32
	{JC_Iw,JC_Id,JC_Iw,JC_Id},//82 jc rel16/32
	{JNC_Iw,JNC_Id,JNC_Iw,JNC_Id},//83 jnc rel16/32
	{JZ_Iw,JZ_Id,JZ_Iw,JZ_Id},//84 jz rel16/32
	{JNZ_Iw,JNZ_Id,JNZ_Iw,JNZ_Id},//85 jnz rel16/32
	{JNA_Iw,JNA_Id,JNA_Iw,JNA_Id},//86 jna rel16/32
	{JA_Iw,JA_Id,JA_Iw,JA_Id},//87 ja rel16/32
	{JS_Iw,JS_Id,JS_Iw,JS_Id},//88  js rel16/32
	{JNS_Iw,JNS_Id,JNS_Iw,JNS_Id},//89 jns rel16/32
	{JP_Iw,JP_Id,JP_Iw,JP_Id},//8a jp rel16/32
	{JNP_Iw,JNP_Id,JNP_Iw,JNP_Id},//8b jnp rel16/32
	{JL_Iw,JL_Id,JL_Iw,JL_Id},//8c jl rel16/32
	{JNL_Iw,JNL_Id,JNL_Iw,JNL_Id},//8d jnl rel16/32
	{JNG_Iw,JNG_Id,JNG_Iw,JNG_Id},//8e jng rel16/32
	{JG_Iw,JG_Id,JG_Iw,JG_Id},//8f jg rel16/32

	{SETO_Rb,SETO_Rb,SETO_Mb,SETO_Mb},//90 seto r/m8
	{SETNO_Rb,SETNO_Rb,SETNO_Mb,SETNO_Mb},//91 setno r/m8
	{SETC_Rb,SETC_Rb,SETC_Mb,SETC_Mb},//92 setc r/m8
	{SETNC_Rb,SETNC_Rb,SETNC_Mb,SETNC_Mb},//93 setnc r/m8
	{SETZ_Rb,SETZ_Rb,SETZ_Mb,SETZ_Mb},//94 setz r/m8
	{SETNZ_Rb,SETNZ_Rb,SETNZ_Mb,SETNZ_Mb},//95 setnz r/m8
	{SETNA_Rb,SETNA_Rb,SETNA_Mb,SETNA_Mb},//96 setna r/m8
	{SETA_Rb,SETA_Rb,SETA_Mb,SETA_Mb},//97 seta r/m8
	{SETS_Rb,SETS_Rb,SETS_Mb,SETS_Mb},//98  sets r/m8
	{SETNS_Rb,SETNS_Rb,SETNS_Mb,SETNS_Mb},//99 setns r/m8
	{SETP_Rb,SETP_Rb,SETP_Mb,SETP_Mb},//9a setp r/m8
	{SETNP_Rb,SETNP_Rb,SETNP_Mb,SETNP_Mb},//9b setnp r/m8
	{SETL_Rb,SETL_Rb,SETL_Mb,SETL_Mb},//9c setl r/m8
	{SETNL_Rb,SETNL_Rb,SETNL_Mb,SETNL_Mb},//9d setnl r/m8
	{SETNG_Rb,SETNG_Rb,SETNG_Mb,SETNG_Mb},//9e setng r/m8
	{SETG_Rb,SETG_Rb,SETG_Mb,SETG_Mb},//9f setg r/m8

	{PUSH_Sr,PUSH_Sr,PUSH_Sr,PUSH_Sr},//a0 push fs
	{POP_Sr,POP_Sr,POP_Sr,POP_Sr},//a1 pop fs
	{CPUID_No,CPUID_No,CPUID_No,CPUID_No},//a2 cpuid
	{BT_RwRw,BT_RdRd,BT_MwRw,BT_MdRd},//a3 /r bt r/m16/32,r16/32
	{SHLD_RwRwIb,SHLD_RdRdIb,SHLD_MwRwIb,SHLD_MdRdIb},//a4 shld r/m16/32,r16/32,imm8
	{SHLD_RwRwCL,SHLD_RdRdCL,SHLD_MwRwCL,SHLD_MdRdCL},//a5 shld r/m16/32,r16/32,cl
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//a6
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//a7
	{PUSH_Sr, PUSH_Sr, PUSH_Sr, PUSH_Sr},//a8 push gs
	{POP_Sr,POP_Sr,POP_Sr,POP_Sr},//a9 pop gs
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//aa
	{BTS_RwRw,BTS_RdRd,BTS_MwRw,BTS_MdRd},//ab bts r/16/32,r16/32
	{SHRD_RwRwIb,SHRD_RdRdIb,SHRD_MwRwIb,SHRD_MdRdIb},//ac shrd r/m16/32,r16/32,imm8
	{SHRD_RwRwCL,SHRD_RdRdCL,SHRD_MwRwCL,SHRD_MdRdCL},//ad shrd r/m16/32,r16/32,cl
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//ae
	{IMUL_RwRw,IMUL_RdRd,IMUL_RwMw,IMUL_RdMd},//af imul r16/32,r/m16/32

	{CMPXCHG_RbRb,CMPXCHG_RbRb,CMPXCHG_MbRb,CMPXCHG_MbRb},//b0 cmpxchg r/m8,r8
	{CMPXCHG_RwRw,CMPXCHG_RdRd,CMPXCHG_MwRw,CMPXCHG_MdRd},//b1 cmpxchg r/m16/32,r16/32
	{LSS_RwMPw,LSS_RdMPd,LSS_RwMPw,LSS_RdMPd},//b2 lss r16/32,m16:16/m16:32
	{BTR_RwRw,BTR_RdRd,BTR_MwRw,BTR_MdRd},//b3 btr r/m16/32,r16/32
	{LFS_RwMPw,LFS_RdMPd,LFS_RwMPw,LFS_RdMPd},//b4 lfs r16/32,m16:16/m16:32
	{LGS_RwMPw,LGS_RdMPd,LGS_RwMPw,LGS_RdMPd},//b5 lgs r16/32,m16:16/m16:32
	{MOVZX_RwRb,MOVZX_RdRb,MOVZX_RwMb,MOVZX_RdMb},//b6 movzx r16/32,r/m8
	{MOVZX_RdRw,MOVZX_RdRw,MOVZX_RdMw,MOVZX_RdMw},//b7 movzx r32,r/m16
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//b8
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//b9
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//ba group 0x0f,0xba
	{BTC_RwRw,BTC_RdRd,BTC_MwRw,BTC_MdRd},//b3 btc r/m16/32,r16/32
	{BSF_RwRw,BSF_RdRd,BSF_RwMw,BSF_RdMd},//bc bsf r16/32,r/m16,32
	{BSR_RwRw,BSR_RdRd,BSR_RwMw,BSR_RdMd},//bd bsr r16/32,r/m16,32
	{MOVSX_RwRb,MOVSX_RdRb,MOVSX_RwMb,MOVSX_RdMb},//be movsx r16/32,r/m8
	{MOVSX_RdRw,MOVSX_RdRw,MOVSX_RdMw,MOVSX_RdMw},//bf movsx r32,r/m16

	{XADD_RbRb,XADD_RbRb,XADD_MbRb,XADD_MbRb},//c0 xadd r/m8,r8
	{XADD_RwRw,XADD_RdRd,XADD_MwRw,XADD_MdRd},//c1 xadd r/m16/32,r16/32
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//c2
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//c3
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//c4
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//c5
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//c6
	{CMPXCHG8B_Mq,CMPXCHG8B_Mq,CMPXCHG8B_Mq,CMPXCHG8B_Mq},//c7 cmpxchg8b m64
	{BSWAP_Rd,BSWAP_Rd,BSWAP_Rd,BSWAP_Rd},//c8 bswap eax
	{BSWAP_Rd,BSWAP_Rd,BSWAP_Rd,BSWAP_Rd},//c9 bswap ecx
	{BSWAP_Rd,BSWAP_Rd,BSWAP_Rd,BSWAP_Rd},//ca bswap edx
	{BSWAP_Rd,BSWAP_Rd,BSWAP_Rd,BSWAP_Rd},//cb bswap ebx
	{BSWAP_Rd,BSWAP_Rd,BSWAP_Rd,BSWAP_Rd},//cc bswap esp
	{BSWAP_Rd,BSWAP_Rd,BSWAP_Rd,BSWAP_Rd},//cd bswap ebp
	{BSWAP_Rd,BSWAP_Rd,BSWAP_Rd,BSWAP_Rd},//ce bswap esi
	{BSWAP_Rd,BSWAP_Rd,BSWAP_Rd,BSWAP_Rd},//cf bswap edi

	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d0
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d1
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d2
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d3
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d4
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d5
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d6
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d7
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d8
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//d9
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//da
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//db
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//dc
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//dd
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//de
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//df

	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//e0
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//e1
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//e2
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//e3
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//e4
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//e5
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//e6
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//e7
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//e8
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//e9
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//ea
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//eb
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//ec
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//ed
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//ee
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//ef

	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f0
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f1
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f2
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f3
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f4
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f5
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f6
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f7
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f8
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f9
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//fa
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//fb
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//fc
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//fd
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//fe
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//ff

};
struct _OPC_ GP0F00[] = {
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//00 /0
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//00 /1
	{LLDT_Rw,LLDT_Rw,LLDT_Mw,LLDT_Rw},//00 /2 lldt r/m16
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//00 /3
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//00 /4
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//00 /5
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//00 /6
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//00 /7
};
struct _OPC_ GP0F01[] = {
	{SGDT_MPd,SGDT_MPd,SGDT_MPd,SGDT_MPd},//01 /0 sgdt m16:32
	{SIDT_MPd,SIDT_MPd,SIDT_MPd,SIDT_MPd},//01 /1 sidt m16:32
	{LGDT_MPd,LGDT_MPd,LGDT_MPd,LGDT_MPd},//01 /2 lgdt m16:32
	{LIDT_MPd,LIDT_MPd,LIDT_MPd,LIDT_MPd},//01 /3 lidt m16:32
	{SMSW_Rw,SMSW_Rd,SMSW_Mw,SMSW_Md},//01 /4 smsw r/m16/32
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//01 /5
	{LMSW_Rw,LMSW_Rw,LMSW_Mw,LMSW_Mw},//01 /6 lmsw r/m16
	{INVLPG_Mu,INVLPG_Mu,INVLPG_Mu,INVLPG_Mu}//01 /7 invlpg m
};
struct _OPC_ GP0FBA[] = {
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//ba /0
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//ba /1
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//ba /2
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//ba /3
	{BT_RwIb,BT_RdIb,BT_MwIb,BT_MdIb},//ba /4 bt r/m16/32,imm8
	{BTS_RwIb,BTS_RdIb,BTS_MwIb,BTS_MdIb},//ba /5 bts r/m16/32,imm8
	{BTR_RwIb,BTR_RdIb,BTR_MwIb,BTR_MdIb},//ba /6 btr r/m16/32,imm8
	{BTC_RwIb,BTC_RdIb,BTC_MwIb,BTC_MdIb},//ba /7 btc r/m16/32,imm8
};
struct _OPC_ GP80[] = {
	{ADD_RbIb,ADD_RbIb,ADD_MbIb,ADD_MbIb},//80 /0 add r/m8,imm8
	{OR_RbIb,OR_RbIb,OR_MbIb,OR_MbIb},//80 /1 or r/m8,imm8
	{ADC_RbIb,ADC_RbIb,ADC_MbIb,ADC_MbIb},//80 /2 adc r/m8,imm8
	{SBB_RbIb,SBB_RbIb,SBB_MbIb,SBB_MbIb},//80 /3 sbb r/m8,imm8
	{AND_RbIb,AND_RbIb,AND_MbIb,AND_MbIb},//80 /4 and r/m8,imm8
	{SUB_RbIb,SUB_RbIb,SUB_MbIb,SUB_MbIb},//80 /5 sub r/m8,imm8
	{XOR_RbIb,XOR_RbIb,XOR_MbIb,XOR_MbIb},//80 /6 xor r/m8,imm8
	{CMP_RbIb,CMP_RbIb,CMP_MbIb,CMP_MbIb}//80 /7 cmp r/m8,imm8
};
struct _OPC_ GP81[] = {
	{ADD_RwIw,ADD_RdId,ADD_MwIw,ADD_MdId},//81 /0 add r/m16/32,imm16/32
	{OR_RwIw,OR_RdId,OR_MwIw,OR_MdId},//81 /1 or r/m16/32,imm16/32
	{ADC_RwIw,ADC_RdId,ADC_MwIw,ADC_MdId},//81 /2 adc r/m16/32,imm16/32
	{SBB_RwIw,SBB_RdId,SBB_MwIw,SBB_MdId},//81 /3 sbb r/m16/32,imm16/32
	{AND_RwIw,AND_RdId,AND_MwIw,AND_MdId},//81 /4 and r/m16/32,imm16/32
	{SUB_RwIw,SUB_RdId,SUB_MwIw,SUB_MdId},//81 /5 sub r/m16/32,imm16/32
	{XOR_RwIw,XOR_RdId,XOR_MwIw,XOR_MdId},//81 /6 xor r/m16/32,imm16/32
	{CMP_RwIw,CMP_RdId,CMP_MwIw,CMP_MdId}//81 /7 cmp r/m16/32,imm16/32
};
struct _OPC_ GP83[] = {
	{ADD_RwIb,ADD_RdIb,ADD_MwIb,ADD_MdIb},//83 /0 add r/m16/32,imm8
	{OR_RwIb,OR_RdIb,OR_MwIb,OR_MdIb},//83 /1 or r/m16/32,imm8
	{ADC_RwIb,ADC_RdIb,ADC_MwIb,ADC_MdIb},//83 /2 adc r/m16/32,imm8
	{SBB_RwIb,SBB_RdIb,SBB_MwIb,SBB_MdIb},//83 /3 sbb r/m16/32,imm8
	{AND_RwIb,AND_RdIb,AND_MwIb,AND_MdIb},//83 /4 and r/m16/32,imm8
	{SUB_RwIb,SUB_RdIb,SUB_MwIb,SUB_MdIb},//83 /5 sub r/m16/32,imm8
	{XOR_RwIb,XOR_RdIb,XOR_MwIb,XOR_MdIb},//83 /6 xor r/m16/32,imm8
	{CMP_RwIb,CMP_RdIb,CMP_MwIb,CMP_MdIb}//83 /7 cmp r/m16/32,imm8
};
struct _OPC_ GPC0[] = {
	{ROL_Rb,ROL_Rb,ROL_Mb,ROL_Mb},//c0 /0 rol r/m8,imm8
	{ROR_Rb,ROR_Rb,ROR_Mb,ROR_Mb},//c0 /1 ror r/m8,imm8
	{RCL_Rb,RCL_Rb,RCL_Mb,RCL_Mb},//c0 /2 rcl r/m8,imm8
	{RCR_Rb,RCR_Rb,RCR_Mb,RCR_Mb},//c0 /3 rcr r/m8,imm8
	{SHL_Rb,SHL_Rb,SHL_Mb,SHL_Mb},//c0 /4 shl/sal r/m8,imm8
	{SHR_Rb,SHR_Rb,SHR_Mb,SHR_Mb},//c0 /5 shr r/m8,imm8
	{SAL_Rb,SAL_Rb,SAL_Mb,SAL_Mb},//c0 /6 sal r/m8,imm8
	{SAR_Rb,SAR_Rb,SAR_Mb,SAR_Mb}	//c0 /7 sar r/m8,imm8
};
struct _OPC_ GPC1[] = {
	{ROL_Rw,ROL_Rd,ROL_Mw,ROL_Md},//c1 /0 rol r/m16/32,imm8
	{ROR_Rw,ROR_Rd,ROR_Mw,ROR_Md},//c1 /1 ror r/m16/32,imm8
	{RCL_Rw,RCL_Rd,RCL_Mw,RCL_Md},//c1 /2 rcl r/m16/32,imm8
	{RCR_Rw,RCR_Rd,RCR_Mw,RCR_Md},//c1 /3 rcr r/m16/32,imm8
	{SHL_Rw,SHL_Rd,SHL_Mw,SHL_Md},//c1 /4 shl/sal r/m16/32,imm8
	{SHR_Rw,SHR_Rd,SHR_Mw,SHR_Md},//c1 /5 shr r/m16/32,imm8
	{SAL_Rw,SAL_Rd,SAL_Mw,SAL_Md},//c1 /6 sal r/m16/32,imm8
	{SAR_Rw,SAR_Rd,SAR_Mw,SAR_Md}	//c1 /7 sar r/m16/32,imm8
};
struct _OPC_ GPD0[] = {
	{ROL_Rb,ROL_Rb,ROL_Mb,ROL_Mb},//d0 /0 rol r/m8,1
	{ROR_Rb,ROR_Rb,ROR_Mb,ROR_Mb},//d0 /1 ror r/m8,1
	{RCL_Rb,RCL_Rb,RCL_Mb,RCL_Mb},//d0 /2 rcl r/m8,1
	{RCR_Rb,RCR_Rb,RCR_Mb,RCR_Mb},//d0 /3 rcr r/m8,1
	{SHL_Rb,SHL_Rb,SHL_Mb,SHL_Mb},//d0 /4 shl/sal r/m8,1
	{SHR_Rb,SHR_Rb,SHR_Mb,SHR_Mb},//d0 /5 shr r/m8,1
	{SAL_Rb,SAL_Rb,SAL_Mb,SAL_Mb},//d0 /6 sal r/m8,1
	{SAR_Rb,SAR_Rb,SAR_Mb,SAR_Mb}	//d0 /7 sar r/m8,1
};
struct _OPC_ GPD1[] = {
	{ROL_Rw,ROL_Rd,ROL_Mw,ROL_Md},//d1 /0 rol r/m16/32,1
	{ROR_Rw,ROR_Rd,ROR_Mw,ROR_Md},//d1 /1 ror r/m16/32,1
	{RCL_Rw,RCL_Rd,RCL_Mw,RCL_Md},//d1 /2 rcl r/m16/32,1
	{RCR_Rw,RCR_Rd,RCR_Mw,RCR_Md},//d1 /3 rcr r/m16/32,1
	{SHL_Rw,SHL_Rd,SHL_Mw,SHL_Md},//d1 /4 shl/sal r/m16/32,1
	{SHR_Rw,SHR_Rd,SHR_Mw,SHR_Md},//d1 /5 shr r/m16/32,1
	{SAL_Rw,SAL_Rd,SAL_Mw,SAL_Md},//d1 /6 sal r/m16/32,1
	{SAR_Rw,SAR_Rd,SAR_Mw,SAR_Md}	//d1 /7 sar r/m16/32,1
};
struct _OPC_ GPD2[] = {
	{ROL_Rb,ROL_Rb,ROL_Mb,ROL_Mb},//d2 /0 rol r/m8,cl
	{ROR_Rb,ROR_Rb,ROR_Mb,ROR_Mb},//d2 /1 ror r/m8,cl
	{RCL_Rb,RCL_Rb,RCL_Mb,RCL_Mb},//d2 /2 rcl r/m8,cl
	{RCR_Rb,RCR_Rb,RCR_Mb,RCR_Mb},//d2 /3 rcr r/m8,cl
	{SHL_Rb,SHL_Rb,SHL_Mb,SHL_Mb},//d2 /4 shl/sal r/m8,cl
	{SHR_Rb,SHR_Rb,SHR_Mb,SHR_Mb},//d2 /5 shr r/m8,cl
	{SAL_Rb,SAL_Rb,SAL_Mb,SAL_Mb},//d2 /6 sal r/m8,cl
	{SAR_Rb,SAR_Rb,SAR_Mb,SAR_Mb}	//d2 /7 sar r/m8,cl
};
struct _OPC_ GPD3[] = {
	{ROL_Rw,ROL_Rd,ROL_Mw,ROL_Md},//d3 /0 rol r/m16/32,cl
	{ROR_Rw,ROR_Rd,ROR_Mw,ROR_Md},//d3 /1 ror r/m16/32,cl
	{RCL_Rw,RCL_Rd,RCL_Mw,RCL_Md},//d3 /2 rcl r/m16/32,cl
	{RCR_Rw,RCR_Rd,RCR_Mw,RCR_Md},//d3 /3 rcr r/m16/32,cl
	{SHL_Rw,SHL_Rd,SHL_Mw,SHL_Md},//d3 /4 shl/sal r/m16/32,cl
	{SHR_Rw,SHR_Rd,SHR_Mw,SHR_Md},//d3 /5 shr r/m16/32,cl
	{SAL_Rw,SAL_Rd,SAL_Mw,SAL_Md},//d3 /6 sal r/m16/32,cl
	{SAR_Rw,SAR_Rd,SAR_Mw,SAR_Md}	//d3 /7 sar r/m16/32,cl
};
struct _OPC_ GPF6[] = {
	{TEST_RbIb,TEST_RbIb,TEST_MbIb,TEST_MbIb},//f6 /0 ib test r/m8,imm8
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f6 /1
	{NOT_Rb,NOT_Rb,NOT_Mb,NOT_Mb},//f6 /2 not r/m8
	{NEG_Rb,NEG_Rb,NEG_Mb,NEG_Mb},//f6 /3 neg r/m8
	{MUL_Rb,MUL_Rb,MUL_Mb,MUL_Mb},//f6 /4 mul r/m8
	{IMUL_Rb,IMUL_Rb,IMUL_Mb,IMUL_Mb},//f6 /5 imul r/m8
	{DIV_Rb,DIV_Rb,DIV_Mb,DIV_Mb},//f6 /6 div r/m8
	{IDIV_Rb,IDIV_Rb,IDIV_Mb,IDIV_Mb}//f6 /7 idiv r/m8
};
struct _OPC_ GPF7[] = {
	{TEST_RwIw,TEST_RdId,TEST_MwIw,TEST_MdId},//f6 /0 ib test r/m16/32,imm16/32
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//f6 /1
	{NOT_Rw,NOT_Rd,NOT_Mw,NOT_Md},//f6 /2 not r/m16/32
	{NEG_Rw,NEG_Rd,NEG_Mw,NEG_Md},//f6 /3 neg r/m16/32
	{MUL_Rw,MUL_Rd,MUL_Mw,MUL_Md},//f6 /4 mul r/m16/32
	{IMUL_Rw,IMUL_Rd,IMUL_Mw,IMUL_Md},//f6 /5 imul r/m16/32
	{DIV_Rw,DIV_Rd,DIV_Mw,DIV_Md},//f6 /6 div r/m16/32
	{IDIV_Rw,IDIV_Rd,IDIV_Mw,IDIV_Md}//f6 /7 idiv r/m16/32
};
struct _OPC_ GPFE[] = {
	{INC_Rb,INC_Rb,INC_Mb,INC_Mb},//fe /0 inc r/m8
	{DEC_Rb,DEC_Rb,DEC_Mb,DEC_Mb},//fe /1 dec r/m8
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//fe /2
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//fe /3
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//fe /4
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//fe /5
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR},//fe /6
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR}//fe /7
};
struct _OPC_ GPFF[] = {
	{INC_Rw,INC_Rd,INC_Mw,INC_Md},//ff /0 inc r/m16/32
	{DEC_Rw,DEC_Rd,DEC_Mw,DEC_Md},//ff /1 dec r/m16/32
	{CALL_Rw,CALL_Rd,CALL_Mw,CALL_Md},//ff /2 call r/m16/32
	{CALL_MPw,CALL_MPd,CALL_MPw,CALL_MPd},//ff /3 call m16:16/m16:32
	{JMP_Rw,JMP_Rd,JMP_Mw,JMP_Md},//ff /4 jmp r/m16/32
	{JMP_MPw,JMP_MPd,JMP_MPw,JMP_MPd},//ff /5 jmp m16:16/m16:32
	{PUSH_Rw,PUSH_Rd,PUSH_Mw,PUSH_Md},//ff /6 push r/m16/32
	{INS_ERR,INS_ERR,INS_ERR,INS_ERR}//ff /7
}; 
char RMB[] = {//RMB
	0x0f,0x0f,//1,1,1,1,0,0,0,0, 1,1,1,1,0,0,0,0,
	0x0f,0x0f,//1,1,1,1,0,0,0,0, 1,1,1,1,0,0,0,0,
	0x0f,0x0f,//1,1,1,1,0,0,0,0, 1,1,1,1,0,0,0,0,
	0x0f,0x0f,//1,1,1,1,0,0,0,0, 1,1,1,1,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x04,0x0a,//0,0,1,0,0,0,0,0, 0,1,0,1,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0xfb,0xff,//1,1,0,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0xf3,0x00,//1,1,0,0,1,1,1,1, 0,0,0,0,0,0,0,0,
	0x0f,0x00,//1,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0xc0,0xc0 //0,0,0,0,0,0,1,1, 0,0,0,0,0,0,1,1
};
char RME[] = {//RME
	0x03,0x00,//1,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x5f,0x00,//1,1,1,1,1,0,1,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0xff,0xff,//1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	0x38,0xb8,//0,0,0,1,1,1,0,0, 0,0,0,1,1,1,0,1,
	0xff,0xfc,//1,1,1,1,1,1,1,1, 0,0,1,1,1,1,1,1,
	0x83,0x00,//1,1,0,0,0,0,0,1, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00 //0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0
};
char BTB[] = {//BTB
	0x2a,0x2a,//0,1,0,1,0,1,0,0, 0,1,0,1,0,1,0,0,
	0x2a,0x2a,//0,1,0,1,0,1,0,0, 0,1,0,1,0,1,0,0,
	0x2a,0x2a,//0,1,0,1,0,1,0,0, 0,1,0,1,0,1,0,0,
	0x2a,0x2a,//0,1,0,1,0,1,0,0, 0,1,0,1,0,1,0,0,
	0xff,0xff,//1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	0xff,0xff,//1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	0x07,0xab,//1,1,1,0,0,0,0,0, 1,1,0,1,0,1,0,1,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0xaa,0xaa,//0,1,0,1,0,1,0,1, 0,1,0,1,0,1,0,1,
	0xfe,0x77,//0,1,1,1,1,1,1,1, 1,1,1,0,1,1,1,0,
	0xa0,0xaa,//0,0,0,0,0,1,0,1, 0,1,0,1,0,1,0,1,
	0x00,0xff,//0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,
	0xb2,0x00,//0,1,0,0,1,1,0,1, 0,0,0,0,0,0,0,0,
	0x0a,0x00,//0,1,0,1,0,0,0,0, 0,0,0,0,0,0,0,0,
	0xa0,0xa7,//0,0,0,0,0,1,0,1, 1,1,1,0,0,1,0,1,
	0x80,0x80 //0,0,0,0,0,0,0,1, 0,0,0,0,0,0,0,1
};
char BTE[] = {//BTE
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0xff,0xff,//1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x38,0xb8,//0,0,0,1,1,1,0,0, 0,0,0,1,1,1,0,1,
	0x7e,0x7c,//0,1,1,1,1,1,1,0, 0,0,1,1,1,1,1,0,
	0x02,0x00,//0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0
};
//char NAB[] = {//NAB
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x03,0xa0,//1,1,0,0,0,0,0,0, 0,0,0,0,0,1,0,1,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x33,//0,0,0,0,0,0,0,0, 1,1,0,0,1,1,0,0,
//	0xa0,0xa8,//0,0,0,0,0,1,0,1, 0,0,0,1,0,1,0,1,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x80,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x08,0x00,//0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00 //0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0
// };
//char NAE[] = {//NAE
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
//	0x00,0x00//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
// };
char RMB2[] = {
	0x03,0x03,//1,1,0,0,0,0,0,0, 1,1,0,0,0,0,0,0,
	0x03,0x03,//1,1,0,0,0,0,0,0, 1,1,0,0,0,0,0,0,
	0x03,0x03,//1,1,0,0,0,0,0,0, 1,1,0,0,0,0,0,0,
	0x03,0x03,//1,1,0,0,0,0,0,0, 1,1,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0xfb,0x93,//1,1,0,1,1,1,1,1, 1,1,0,0,1,0,0,1,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0xc3,0x00,//1,1,0,0,0,0,1,1, 0,0,0,0,0,0,0,0,
	0x0f,0x00,//1,1,1,1,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0xc0,0xc0 //0,0,0,0,0,0,1,1, 0,0,0,0,0,0,1,1
};
char RME2[] = {
	0x03,0x00,//1,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x13,0x00,//1,1,0,0,1,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0xff,0xff,//1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,
	0x38,0xb8,//0,0,0,1,1,1,0,0, 0,0,0,1,1,1,0,1,
	0x0b,0x0c,//1,1,0,1,0,0,0,0, 0,0,1,1,0,0,0,0,
	0x83,0x00,//1,1,0,0,0,0,0,1, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00,//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	0x00,0x00//0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
};