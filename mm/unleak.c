#include <mm.h>
#include <kernel.h>

typedef struct _UNLEAK_LOG_{
	struct _LOG_ * prev;
	struct _LOG_ * next;
	void * data;
	void (*call_back)(void*data,void*block);
} UNLEAK_LOG,*LPUNLEAK_LOG,UNLEAK,*LPUNLEAK;

void * unleak_malloc(int size,void (*call_back)(void*),void * data){
	void * result;
	LPUNLEAK log;
	LPPROCESS process;
	
	process = GetCurProcess();
	size += sizeof(UNLEAK);
	LockProcessField(process);
	result = kmalloc(size + sizeof(UNLEAK),0);
	log = result;
	log->data = data;
	log->call_back = call_back;
	if(process->unleak_memory_list){
		log->prev = NULL;
		process->unleak_memory_list->prev = log;
		log->next = process->unleak_memory_list;
		process->unleak_memory_list = log;
	}
	else{
		log->next = log->prev = NULL;
		process->unleak_memory_list = log;
	}
	UnlockProcessField(process);
	return ((u8*)result) + sizeof(UNLEAK);
}
void unleak_free(void * ptr){
	LPUNLEAK log;
	LPPROCESS process;
	
	log = (LPUNLEAK)((u8*)ptr + sizeof(UNLEAK));
	if(log->prev) log->prev = log->next;
	if(log->next) log->next = log->prev;
	process = GetCurProcess();
	if(process->unleak_memory_list == log){
		process->unleak_memory_list = log->next;
	}
	kfree(ptr);
}


