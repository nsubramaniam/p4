#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "spin.h"

volatile int balance=0;

typedef struct __spinlock
{
	short key;
	int count;	
	volatile unsigned int value;
} spinlock_t;

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

void init(spinlock_t *lock)
{
	lock->value=0;
}

void spinlock_acquire(spinlock_t *lock)
{
	while(xchg(&(lock->value),1));
}


void spinlock_release(spinlock_t *lock)
{
	xchg(&(lock->value),0);
}

void *increment(void *n)
{
	int i;
	spinlock_t *lock=(spinlock_t *)n;
	printf("Count = %d\n",lock->count);
	for(i=0;i<lock->count;i++)
	{	
		spinlock_acquire(lock);
		balance++;
		spinlock_release(lock);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t thread1,thread2;
	spinlock_t *lock=malloc(sizeof(spinlock_t));
	init(lock);

	if(argc<2)
	{
		printf("Error; Usage: ./spin.o <count>\n");
		return -1;
	}

	lock->count=atoi(argv[1]);

	printf("Balance = %d\n",balance);
	
	pthread_create(&thread1,NULL,increment,(void *)lock);
	pthread_create(&thread2,NULL,increment,(void *)lock);
	
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);

	printf("Balance = %d\n",balance);

	free(lock);
	return 0;
}
