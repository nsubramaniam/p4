#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//#include "counter.h"

volatile int balance=0;

typedef struct __spinlock
{
	volatile unsigned int flag;	
} spinlock_t;

typedef struct __counter
{
	spinlock_t lock;
	int value;
} counter_t;

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

void* spinlock_acquire(void* lock)
{
	spinlock_t* l_lock;
	l_lock=(spinlock_t *)lock;
	while(xchg(&(l_lock->flag),1));	
}

void* spinlock_release(void* lock)
{
	spinlock_t* l_lock;
	l_lock=(spinlock_t *)lock;
	l_lock->flag=0;
}

void Counter_Init(counter_t *c,int value)
{
	spinlock_acquire(&(c->lock));
	c->value=value;
	spinlock_release(&(c->lock));
}

int Counter_GetValue(counter_t *c)
{
	return c->value;
}

void Counter_Increment(counter_t *c)
{
	spinlock_acquire(&(c->lock));
	c->value+=1;
	spinlock_release(&(c->lock));
}

void Counter_Decrement(counter_t *c)
{
	spinlock_acquire(&(c->lock));
	c->value-=1;
	spinlock_release(&(c->lock));
}

int main(int argc, char *argv[])
{
	counter_t *c=malloc(sizeof(counter_t));
	Counter_Init(c,100);

	printf("c->value = %d\n",c->value);
	int i,count=atoi(argv[1]);

	for(i=0;i<count;i++)
		Counter_Increment(c);

	printf("c->value = %d\n",c->value);
	free(c);
	return 0;
}
