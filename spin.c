#include <stdio.h>
#include <stdlib.h>

#include "spin.h"
static inline uint xchg(volatile unsigned int *addr, unsigned int newval)
{
    	uint result;
    	asm volatile("lock; xchgl %0, %1" : "+m" (*addr), "=a" (result) : "1" (newval) : "cc");
    	//xchg: exchange the two source operands
    	//two outputs: *addr and result
	//"+m", m means the operand is directly from memory, + means read and write
    	//"=a": suggest gcc to put the operand value into eax register; `=' means write only
    	//one input: newval, "1" means it uses the same constraint as the earlier 1th, i.e., it 
    	// will be put into eax and then be overwritten
    	//"cc" means the condition register might be altered
    	return result;
}

//Spinlock functions
void* spinlock_acquire(void* lock)
{
	spinlock_t *l_lock;
	l_lock=(spinlock_t*)lock;
	while(xchg(&(l_lock->flag),1));
}

void* spinlock_release(void* lock)
{
	spinlock_t *l_lock;
	l_lock=(spinlock_t*)lock;
	l_lock->flag=0;
}
