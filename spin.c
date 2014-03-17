#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

volatile int balance=0;
volatile unsigned int lock=0;

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

void init()
{
	lock=0;
}

void spin_lock()
{
	while(xchg(&lock,1));
}


void spin_unlock()
{
	xchg(&lock,0);
}


void *increment(void *n)
{
	int i,count=(int)n;
	printf("Count = %d\n",count);
	for(i=0;i<count;i++)
	{	
		spin_lock();
		balance++;
		spin_unlock();
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t thread1,thread2;
	if(argc<2)
	{
		printf("Error; Usage: ./spin.o <count>\n");
		return -1;
	}

	int n=atoi(argv[1]);

	printf("Balance = %d\n",balance);
	
	pthread_create(&thread1,NULL,increment,(void *)n);
	pthread_create(&thread2,NULL,increment,(void *)n);
	
	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);

	printf("Balance = %d\n",balance);

	return 0;
}
