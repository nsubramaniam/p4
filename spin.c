#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//#include "counter.h"

volatile int balance=0;

typedef struct __counter
{
	volatile unsigned int lock;
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

void Counter_Init(counter_t *c,int value)
{
	c->value=value;
}

int Counter_GetValue(counter_t *c)
{
	return c->value;
}

void Counter_Increment(counter_t *c)
{
	while(xchg(&(c->lock),1));
	c->value+=1; //Don't need to use xchg here
	c->lock=0;
}

void Counter_Decrement(counter_t *c)
{
	while(xchg(&(c->lock),1));
	c->value+=-1; //Don't need to use xchg here
	c->lock=0;
}

int main(int argc, char *argv[])
{
	pthread_t thread1,thread2;
	counter_t *c=malloc(sizeof(counter_t));
	Counter_Init(c,100);

	if(argc<2)
	{
		printf("Error; Usage: ./spin.o <count>\n");
		return -1;
	}

	int i,count=atoi(argv[1]);

	for(i=0;i<count;i++)
	{	
		pthread_create(&thread1,NULL,Counter_Increment,(void *)c);
		pthread_create(&thread2,NULL,Counter_Increment,(void *)c);
		pthread_join(thread1,NULL);
		pthread_join(thread2,NULL);
	}

	printf("c->value = %d\n",c->value);

	free(c);
	return 0;
}
