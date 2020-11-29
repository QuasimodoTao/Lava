//Lava OS
//PageFault
//20-04-11-15-05

#include <stdio.h>
#include "lm.h"
#define F_FREE	0
#define F_OCCU	1
struct _MLOG_ {
	u64 Flag;
	u64 Size;
	struct _MLOG_ * Prev;
	struct _MLOG_ * Next;
};
static struct _MLOG_ * Root;
void * malloc(u32 Size) {		//从内核空间分配堆
	void * res;
	struct _MLOG_ * log;
	struct _MLOG_ * logt;

	Size = (Size + 0xf) & 0xfffffff0;
	
	for(log = Root;log;log = log->Next){	//循环查找内存块头
		if(log->Flag) continue;		//若此内存块已分配则忽略此块
		if(log->Size > Size){			//若内存块大小超过需要的大小，则分配此内存块，并建立新的内存块记录
			res = log+1;
			logt = (struct _MLOG_ *)((long)(log+1) + Size);
			logt->Next = log->Next;
			if(log->Next) log->Next->Prev = logt;
			log->Next = logt;
			logt->Prev = log;
			logt->Flag = F_FREE;	//设置空闲标识给新建的内存块头
			logt->Size = log->Size - Size - sizeof(struct _MLOG_);
			log->Size = Size;
			log->Flag = F_OCCU;	//设置占用标识给分配的内存块头
			return res;
		}
		if(log->Size == Size){		//若内存块与申请的大小相同，则直接分配，设置标识
			res = log+1;
			log->Flag = F_OCCU;
			return res;
		}
	}
	puts("No more memory area.\n");
	stop();
}
void free(void * P) {	//释放已被申请的内核空间块
	
	//printk("call by %X.\n",*((long*)(&P) - 1));
	
	struct _MLOG_ * log;
	struct _MLOG_ * logt;

	if ((int)P & 0xf) {	//若未对其则报错
		puts("Kernel Free Memory function error:\n\tAddress not aligment with 0xf.\n");
		stop();
	}
	log = ((struct _MLOG_ *)P) - 1;
	if(!log->Flag) {	//若块未被分配则报错
		printk("Kernel Free Memory function error:%X\n\tTry to free free memory or address not in log.\n",P);
		stop();
	}
	log->Flag = F_FREE;	//清除标识
	logt = log->Next;
	if ((logt) && (logt->Flag == F_FREE)) {	//若下一个块头存在且块空闲，则合并两块
		if(logt->Next) logt->Next->Prev = log;
		log->Next = logt->Next;
		log->Size += logt->Size + sizeof(struct _MLOG_);
	}
	logt = log->Prev;
	if ((logt) && (logt->Flag == F_FREE)) {	//若上一个块头存在且块空闲，则合并两块
		if(log->Next)log->Next->Prev = logt;
		logt->Next = log->Next;
		logt->Size += log->Size + sizeof(struct _MLOG_);
	}
}
int mem_init(u64 MemoryStart,u64 MemoryEnd){
	MemoryStart += 0x0f;
	MemoryStart &= 0xfffffffffffffff0;
	MemoryEnd &= 0xfffffffffffffff0;
	Root = (struct _MLOG_ *)MemoryStart;
	Root->Size = MemoryEnd - MemoryStart - sizeof(struct _MLOG_);
	Root->Flag = F_FREE;
	Root->Prev = NULL;
	Root->Next = NULL;
	return 0;
}