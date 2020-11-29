//Lava OS
//PageFault
//20-04-11-15-02

#ifndef _VM_H_
#define _VM_H_

#include <stddef.h>
#include <config.h>

#define CF	0x00000001
#define PF	0x00000004
#define AF	0x00000010
#define ZF	0x00000040
#define SF	0x00000080
#define TF	0x00000100
#define IF	0x00000200
#define DF	0x00000400
#define OF	0x00000800
#define NT	0x00004000
#define RF	0x00010000
#define VM	0x00020000
#define AC	0x00040000
#define VF	0x00080000
#define VP	0x00100000

#define _ESOP 0x26
#define _CSOP 0x2e
#define _SSOP 0x36
#define _DSOP 0x3e
#define _FSOP 0x64
#define _GSOP 0x65
#define _OSOP 0x66
#define _ASOP 0x67
#define _LOCKOP 0xf0
#define _REPNEOP 0xf2
#define _REPEOP 0xf3

#define _OSP 0x01
#define _ASP 0x02
#define _LOCKP 0x04
#define _REPEP 0x08
#define _REPNEP 0x10
#define _SREGP 0x20
#define _SIB 0x40
#define _DISP8 0x80

#define GETF(FLAGS) (cpu->eflags.erx & ((FLAGS) ? 1:0))
#define CHEF(FLAGS) (cpu->eflags.erx & (FLAGS))
#define CLRF(FLAGS) (cpu->eflags.erx &= ~(FLAGS))
#define SETF(FLAGS) (cpu->eflags.erx |= (FLAGS))

#define MAX(v1,v2) (v1 > v2 ? v1 : v2)
#define MIN(v1,v2) (v1 > v2 ? v2 : v1)

#define GETR8(n) (n >= 4 ? cpu->gr[n - 4].rh : cpu->gr[n].rl)
#define GETR16(n) (cpu->gr[n].rx)
#define GETR32(n) (cpu->gr[n].erx)

#define SETR8(n,v) (n >= 4 ? (cpu->gr[n - 4].rh = (v)):(cpu->gr[n].rl = (v)))
#define SETR16(n,v) (cpu->gr[n].rx = (v))
#define SETR32(n,v) (cpu->gr[n].erx = (v))

#define VMRead8(a)		((unsigned char)vm_read(a, 1))
#define VMRead16(a)		((unsigned short)vm_read(a, 2))
#define VMRead32(a)		vm_read(a, 4)
#define VMWrite8(a,v) 	vm_write(a, v, 1)
#define VMWrite16(a,v) 	vm_write(a, v, 2)
#define VMWrite32(a,v) 	vm_write(a, v, 4)
#define ModRM1(cpu) 	(cpu->ins.addr)
#define ModRM0(cpu) 	(cpu->ins.addr + cpu->base[cpu->ins.sreg])
#define VMPush16(x)		vm_push(cpu,2, x)
#define VMPush32(x)		vm_push(cpu,4, x)
#define VMPop16()		(unsigned short)vm_pop(cpu,2)
#define VMPop32()		vm_pop(cpu,4)
#define vm_memory_base	PMEMSTART

enum {AL,CL,DL,BL,AH,CH,DH,BH};
enum {AX,CX,DX,BX,SP,BP,SI,DI};
enum {EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI};
enum {ES,CS,SS,DS,FS,GS};

#ifdef VM_DEBUG
extern const char * SREG[];
extern const char * REG8[];
extern const char * REG16[];
extern const char * REG32[];
extern const char * RM16[];
#endif

struct _REG_ {
	union {
		u32 erx;
		struct {
			union {
				u16 rx;
				struct {
					u8 rl;
					u8 rh;
				};
			};
		};
	};
};

struct _IMM_ {
	union {
		u8 byte;
		u16 word;
		u32 dword;
	};
};

struct _INS_ {
	u8 op0;
	u8 op1;
	u8 src;//operator 1
	u8 des;//operator 2
	struct {
		union {
			u8 modrm;
			struct{
				u8 rm:3;
				u8 reg:3;
				u8 mod:2;
			};
		};
	};
	struct {
		union{
			u8 sib;
			struct{
				u8 base:3;
				u8 index:3;
				u8 scale:2;
			};
		};
	};
	u8 sreg;
	u8 muchine_flag;
	u8 counter;
	struct _IMM_ imm1;
	struct _IMM_ imm2;
	u32 disp;
	u32 addr;
};

typedef struct _VM_CPU_ {
	struct _REG_ gr[8];
	u16 sr[6];
	u64 limit[6];
	u64 base[6];
	struct _REG_ eip;
	struct _REG_ eflags;
	struct _INS_ ins;
	struct CPU * cpu;
#ifdef VM_DEBUG
	int (*cond)(struct CPU *,struct _VM_CPU_ *);
	char rm_str[16];
	char cs_ip_str[16];
	char ins_str[64];
	struct _REG_ cur_eip;
	u16 cur_cs;
	u16 ins_len;
#endif
} VCPU, * LPVCPU;
struct CPU{
	u32 gr[8];
	u16 sr[6];
	u32 flags;
};

struct _OPC_ {
	void (*RR16)(LPVCPU);//r,r/no/r,i/m,i/
	void (*RR32)(LPVCPU);
	void (*MR16)(LPVCPU);
	void (*MR32)(LPVCPU);
};
u32 vm_read(u32 addr, u32 len);
void vm_write(u32 addr, u32 data, u32 len);
void vm_push(LPVCPU cpu,u32 len, u32 data);
u32 vm_pop(LPVCPU cpu,u32 len);
int get_ins(LPVCPU cpu);
u8 vm_read_byte(u32 vm_addr);
u16 vm_read_short(u32 vm_addr);
u32 vm_read_long(u32 vm_addr);
void vm_write_byte(u32 vm_addr,u8 data);
void vm_write_short(u32 vm_addr,u16 data);
void vm_write_long(u32 vm_addr,u32 data);
void vm_read_string(u32 vm_addr,u32 count,void * dest);
void vm_write_string(u32 vm_addr,u32 count,void * srco);

int vm_alloc_memory(u32 size);
void vm_free_memory(int base);

int int86(u8 n,struct CPU * cpu);

inline void * vm_addr2v(u32 addr){
	return (void*)(vm_memory_base + addr);
}
inline void * vm_saddr2v(u16 seg,u16 base){
	return (void*)(vm_memory_base + base + (((u64)seg) << 4));
}

#endif
//Quasimodo