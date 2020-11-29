//Lava OS
//PageFault
//20-03-07-15-17

#include <stddef.h>
#include <asm.h>
#include <lava.h>
#include <mm.h>
#include <string.h>
#include <stdio.h>
#include <kernel.h>
#include <int.h>
#include <spinlock.h>

/*
I	Size  Max	M	A	total  free
1	16	256 254	32 16	4096   0 0
2	32	128 127	16 32	4096   0 16
3	48	 85 85	11 16	4080  16 5
4	64	 64 63	8  64	4096   0 56
5	80	 51 51	7  16	4080  16 9
6	96	 42 42	7  32	4032  64 57
7	112	 36 36	5  16	4032  64 48
8	128	 32 31	4  128	4096   0 124
9	144	 28 28	4  16	4032  64 60
10	160	 25 25	4  32	4000  96 92
11	176	 23 23	3  16	4048  48 45
12	192	 21 21	3  64	4032  64 61
13	208	 19 19	3  16	3952 144 144//
14	224	 18 18	3  32	4032  64 64
15	240	 17 17	3  16	4080  16 16
16	256	 16 16	2  256	4096   0 0
17	272	 15 15	2  16	4080  16 16
18	288	 14 14	2  32	4032  64 64
19	304	 13 13	2  8	3952  40 40
20	336	 12 12	2  16	4032  64 64
21	368	 11 11	2  16	4048  48 48
22	408	 10 10	2  16	4080  16 16
23	448	  9 9	2  32	4032  64 64
24	512	  8 8	1  512	4096   0 0
25	584	  7 7	1  8	4032   8 7
26	680	  6 6	1  8	4080  16 16
27	816	  5 5	1  16	4080  16 16
28	1024  4 4	1  1024	4096   0 0
29	1360  3 3	1  16	4080  16 16
30	2048  2 2	1  2048	4096   0 0
*/
struct KMCB {
	u32 ctrl:24;
	u32 index:5;
	u32 lock:1;
	u32 vaild:1;
	u32 :1;
};
struct SupBlk {
	u8 size;//qword count
	u8 blks;//block count
};
static const struct SupBlk mem_sup[] = {
	{0,0  },//none
	{1,254},{3,127},{5,85 },{7,63 },{9,51 },{11,42},//mask on page
	{13,36},{15,31},{17,28},{19,25},{21,23},{23,21},//mask on page
	{25,19},{27,18},{29,17},{31,16},{33,15},{35,14},//mask on control
	{38,13},{41,12},{45,11},{50,10},{55,9 },{63,8 },//mask on control
	{72,7 },{84,6 },{101,5},{127,4},{169,3},{255,2} //mask on control
};

#define first_block		((struct KMCB *)MMHMCBB)
#define __last_block	((struct KMCB *)MMHMCBL)
#define ker_heap_base	MMHSB
#define kernel_unswapable_stack_mask		((u8*)KSBA)
#define kernel_unswapable_stack_vaild_mask	((u8*)KSBV)

static struct KMCB * first_useable[sizeof(mem_sup)/sizeof(struct SupBlk)];
static struct KMCB * first_empty;
static u32 lock;
static u32 first_lock;
static u32 stack_first_unswapable;
static struct KMCB * last_block;

static int stack_busy_count;
static int heap_busy_count;

#define HEAP_BUSY		0
#define HEAP_LIST_BUSY	1
#define HEAP_EMPTY_BUSY	2
#define STACK_BUSY		3

spin_optr_bit(Block,29);
spin_optr_def_bit(Heap,&lock,HEAP_BUSY);
spin_optr_def_bit(List,&lock,HEAP_LIST_BUSY);
spin_optr_def_bit(Empty,&lock,HEAP_EMPTY_BUSY);
spin_optr_def_arg_bit(First,&first_lock,0);
spin_optr_def_bit(Stack,&lock,STACK_BUSY);

void * alloc_stack(){
	u64 res, size, page;
	u32 i;
	
	ID();//屏蔽中断避免中断发生
	LockStack();//竞争以进入操作
	if(stack_first_unswapable >= KST) {
		UnlockStack();
		IE();//允许中断
		return NULL;
	}
	i = stack_first_unswapable;
	res = KSB + stack_first_unswapable * KSS;
	bts(kernel_unswapable_stack_mask,stack_first_unswapable);
	size = KSS - 0x2000;
	stack_first_unswapable++;
	while(stack_first_unswapable < KST){
		if(!bt(kernel_unswapable_stack_mask,stack_first_unswapable)) break;
		stack_first_unswapable++;
	}
	UnlockStack();
	if(bt(kernel_unswapable_stack_vaild_mask,i)) {
		IE();//允许中断
		return (void*)res;
	}
	allocate_area(NULL,res + size,0x2000,PAGE_EXIST | PAGE_WRITE);
	allocate_area(NULL,res,size,PAGE_WRITE);
	IE();//允许中断
	return (void*)res;
}
void free_stack(void * _stack){
	u64 stack;
	
	stack = (u64)_stack;
	ID();
	if(stack >= KSB && stack < KSB + KSS * KST){
		//unswapable
		stack -= KSB;
		stack /= KSS;
		LockStack();//竞争以进入操作
		btr(kernel_unswapable_stack_mask,stack);
		if(stack < stack_first_unswapable) stack_first_unswapable = stack;
		UnlockStack();//释放锁
	}
	IE();
}
void ker_stack_init(){
	put_page(get_free_page(0,0),NULL,kernel_unswapable_stack_mask);
	memset(kernel_unswapable_stack_mask,0,KST/8);
	memset(kernel_unswapable_stack_vaild_mask,0,KST/8);
	stack_first_unswapable = 0;
	stack_busy_count = 0;
	UnlockStack();
}
static void search_next(){
	struct KMCB * cur;
	u64 count;
	u64 page;
	u64 i;
	struct KMCB * first;
	
	cur = first_empty + 1;
	while(cur <= last_block){
		if(TryLockBlock(cur)){
			if(cur->index != 31) cur++;
			continue;
		}
		else{
			if(!cur->index) {
				first_empty = cur;
				UnlockBlock(cur);
				return;
			}
			if(cur->index == 31){
				first = cur;
				count = 0;
				while(cur->ctrl == 0x00ffffff){
					count += 0xfffffd;
					cur++;
				}
				count += cur->ctrl;
				cur += count;
				UnlockBlock(first);
				continue;
			}
			UnlockBlock(cur);
			cur++;
		}
	}
	//TODO:Extern block control 
	if(last_block < __last_block){
		cur = first_empty = last_block + 1;
		last_block += PAGE_SIZE/sizeof(struct KMCB);
		page = get_free_page(0,0);
		put_page(page,NULL,cur);
		memset(cur,0,PAGE_SIZE);
	}
	else first_empty = NULL;
}
void * kmalloc(size_t size,unsigned int align){
	int i, j;
	void * ret;
	struct KMCB * cur, * first;
	u8 * mask, * _mask;
	u64 t, count, page;
	
	
	{//检查参数是否合法
		if(!size) return NULL;
		if(align){//检查对齐
			for(i = 0;i < 32;i++) if(align & (1 << i)) break;
			for(j = 31;j >= 0;j--) if(align & (1 << j)) break;
			if(i != j){
				print("Alignment mumber must be powwer of 2.\n");
				return NULL;
			}
		}
		if(!align) align = 8;
		if(align <= 8){//检查对齐
			size += 0x07;
			size >>= 3;
			if(size <= 1) size = 2;
		}
		else if(align > PAGE_SIZE){
			print("Unsuppose alignment number,up to page size.\n");
			return NULL;
		}
		else{
			size += align - 1;
			size &= ~(align - 1);
			size >>= 3;
		}
	}
	if(size <= 256){//此分配将页面分块
		size--;//搜索最小的大于等于申请的大小的支持块，这可能会造成内零头
		for(i = 0;i < sizeof(mem_sup)/sizeof(struct SupBlk);i++) 
			if(mem_sup[i].size >= size) break;
		size = (mem_sup[i].size + 1) << 3;
		ID();
		LockHeap(); heap_busy_count++; UnlockHeap();//记录进入操作
		LockFirst(i);
		if(i <= 13){
			if(!first_useable[i]){//
				LockEmpty();//判断前就进行互斥避免后面的search_next造成first_empty变成NULL
				if(!first_empty){//没有空闲的块了
					UnlockEmpty();
					UnlockFirst(i);
					LockHeap(); heap_busy_count--; UnlockHeap();//记录退出操作
					IE();
					return NULL;
				}
				cur = first_useable[i] = first_empty;
				cur->index = i;
				cur->lock = 0;
				search_next();//搜索下一个可用的块
				UnlockEmpty();
				if(!cur->vaild){
					page = get_free_page(0,0);
					put_page(page,NULL,(void*)((cur - first_block) * PAGE_SIZE + ker_heap_base));
					cur->vaild = 1;
				}
				cur->ctrl = mem_sup[i].blks - 1;
				if(i == 1) {
					memset((void*)((cur - first_block) * PAGE_SIZE + ker_heap_base + PAGE_SIZE - 32),0,32);
					*(u8*)((cur - first_block) * PAGE_SIZE + ker_heap_base + PAGE_SIZE - 32) = 1;
				}
				else {
					memset((void*)((cur - first_block) * PAGE_SIZE + ker_heap_base + PAGE_SIZE - 16),0,16);
					*(u8*)((cur - first_block) * PAGE_SIZE + ker_heap_base + PAGE_SIZE - 16) = 1;
				}
				ret = (void*)((cur - first_block) * PAGE_SIZE + ker_heap_base);
				UnlockFirst(i);
				LockHeap(); heap_busy_count--; UnlockHeap();//记录退出操作
				IE();
				return ret;
			}
			else{
				cur = first_useable[i];
				if(i == 1) _mask = mask = (u8*)((cur - first_block) * PAGE_SIZE + ker_heap_base + PAGE_SIZE - 32);
				else _mask = mask = (u8*)((cur - first_block) * PAGE_SIZE + ker_heap_base + PAGE_SIZE - 16);
				while(*mask == 0xff) mask++;
				for(j = 0;j < 8;j++) if(!(*mask & (1 << j))) break;
				j += (mask - _mask) * 8;
				if(j >= mem_sup[i].blks){
					print("BUG:memory.c::kmalloc().\n");
					stop();
				}
				*mask |= 1 << (j & 0x07);
				ret = (void*)((cur - first_block) * PAGE_SIZE + ker_heap_base + j * size);
				cur->ctrl--;
				if(!cur->ctrl) first_useable[i] = NULL;
				UnlockFirst(i);
				LockHeap(); heap_busy_count--; UnlockHeap();//记录退出操作
				IE();
				return ret;
			}
		}
		else{
			if(!first_useable[i]){
				LockEmpty();
				if(!first_empty){
					UnlockEmpty();
					UnlockFirst(i);
					LockHeap(); heap_busy_count--; UnlockHeap();//记录退出操作
					IE();
					return NULL;
				}
				cur = first_useable[i] = first_empty;
				search_next();
				UnlockEmpty();
				if(!cur->vaild){
					page = get_free_page(0,0);
					put_page(page,NULL,(void*)((cur - first_block) * PAGE_SIZE + ker_heap_base));
					cur->vaild = 1;
				}
				cur->ctrl = mem_sup[i].blks - 1;
				cur->ctrl |= 0x20;
				ret = (void*)((cur - first_block) * PAGE_SIZE + ker_heap_base);
				cur->index = i;
				cur->lock = 0;
				UnlockFirst(i);
				LockHeap(); heap_busy_count--; UnlockHeap();//记录退出操作
				IE();
				return ret;
			}
			else{
				cur = first_useable[i];
				for(j = 5;j < 24;j++) if(!(*(u32*)cur & (1 << j))) break;
				if((j - 5) >= mem_sup[i].blks){
					print("BUG:memory.c::kmalloc().\n");
					stop();
				}
				*(u32*)cur |= 1 << j;
				ret = (void*)((cur - first_block) * PAGE_SIZE + ker_heap_base + (j - 5) * size);
				j = cur->ctrl;
				*(u32*)cur &= 0xffffffe0;
				j--;
				*(u32*)cur |= j & 0x1f;
				if(!(j & 0x1f)) first_useable[i] = NULL;
				UnlockFirst(i);
				LockHeap(); heap_busy_count--; UnlockHeap();//记录退出操作
				IE();
				return ret;
			}
		}
	}
	else{
		size += 0x1ff;//qword count
		size >>= 9;//page count
		ID();
		LockHeap();//记录进入操作,锁定则只能进行一个进行以页分配的堆的线程
		for(cur = first_empty;cur <= last_block;cur++){
			while(last_block - cur < size){//扩展区域
				if(last_block >= __last_block) {
					UnlockHeap();
					IE();
					return NULL;
				}
				last_block++;
				page = get_free_page(0,0);
				put_page(page,NULL,last_block);
				memset(last_block,0,PAGE_SIZE);
				last_block += 1023;
			}
			if(cur->index) {//检查到已分配块则跳过
				if(cur->index == 31){
					count = 0;
					while(cur->ctrl == 0x00ffffff){
						count += 0xfffffd;
						cur++;
					}
					count += cur->ctrl;
					count--;
					cur += count;
				}
				continue;
			}
			ret = (void*)((cur - first_block) * PAGE_SIZE + ker_heap_base);//检查到可用块
			first = cur;
			for(t = 0;t < size && cur <= last_block;t++,cur++) if(cur->index) break;//检查连续块是否足够
			if(t == size){//块足够则分配
				first->index = 31;
				LockBlock(first);
				cur = first;
				while(size >= 0x00fffffe){//记录分配的块数
					cur->index = 31;
					cur->ctrl = 0x00ffffff;
					size -= 0x00fffffe;
					cur++;
				}
				cur->index = 31;
				cur->ctrl = size;
				UnlockBlock(first);
				cur = first;
				for(t = 0;t < size;t++){//逐个检查块是否可用
					if(!cur[t].vaild){
						page = get_free_page(0,0);
						put_page(page,NULL,(void*)((cur - first_block) * PAGE_SIZE + ker_heap_base + t * PAGE_SIZE));
						cur[t].vaild = 1;
					}
				}
				if(first_empty == first) {
					first_empty--;
					search_next();
				}
				UnlockHeap();
				IE();
				return ret;
			}
		}
	}
}
void kfree(void * addr){
	struct KMCB * cur;
	int i,index;
	struct KMCB * first;
	
	first = cur = (struct KMCB *)((((((u64)addr) & ~PAGE_IN_ADDR_MASK) - ker_heap_base)/PAGE_SIZE) + first_block);
	if(cur->index == 31) {
		//锁定控制块，仅允许读
		//锁定多个连续块（即以页大小分配的堆），避免过滤时
		LockHeap();//记录进入操作
		LockBlock(cur);
		while(first->index == 31){
			*(u32*)first = 0x40000000;//vaild = 1,lock = 0;
			first++;
		}
		if(first_empty > cur) first_empty = cur;
		*(u32*)cur = 0x40000000;//解锁并清零控制块值:vaild = 1,lock = 0;
		UnlockHeap();
		return;
	}
	else{
		LockHeap();//记录进入操作
		heap_busy_count++;
		UnlockHeap();
		index = cur->index;
		LockFirst(index);
		i = (((u64)addr) & PAGE_IN_ADDR_MASK) / ((mem_sup[index].size + 1) << 3);
		if(index <= 13){
			if(index == 1) btr((u8*)((cur - first_block) * PAGE_SIZE + ker_heap_base + PAGE_SIZE - 32),i);
			else btr((u8*)((cur - first_block) * PAGE_SIZE + ker_heap_base + PAGE_SIZE - 16),i);
			cur->ctrl++;
		}
		else{
			btr(cur,5 + i);
			i = cur->ctrl & 0x1f;
			i++;
			cur->ctrl &= 0x00ffffe0;
			cur->ctrl |= i & 0x1f;
		}
		if(cur->ctrl == mem_sup[index].blks) {
			LockBlock(cur);
			*(u32*)cur = 0x60000000;//vaild = 1,lock = 1
			LockEmpty();
			if(first_empty > cur) first_empty = cur;
			UnlockEmpty();
			first_useable[index] = NULL;
			UnlockBlock(cur);
		}
		UnlockFirst(index);
		LockHeap();//记录退出操作
		heap_busy_count--;
		UnlockHeap();
	}
}
u64 ker_heap_clean(){
	struct KMCB * cur;
	u64 free_count = 0;
	
	ID();
	while(1){
		LockHeap();
		if(!heap_busy_count) break;
		UnlockHeap();
	}
	for(cur = first_block;cur < last_block;cur++){
		if(!cur->index && cur->vaild){
			free_vaddr(0,(void*)((cur - first_block) * 4096 + ker_heap_base));
			cur->vaild = 0;
			free_count += 0x1000;
		}
	}
	UnlockHeap();
	IE();
	return free_count;
}
void * vmalloc(size_t size){
	
}
void vfree(void * addr){
	
}
void ker_heap_init(){
	u64 page;
	
	page = get_free_page(0,0);
	put_page(page,NULL,first_block);
	memset(first_block,0,0x1000);
	first_empty = first_block;
	last_block = first_block + 0x1000/sizeof(struct KMCB) - 1;
	memset(first_useable,0,sizeof(first_useable));
	lock = 0;
	first_lock= 0;
}




