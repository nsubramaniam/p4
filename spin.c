#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//#include "hash.h"
#include "list.h"
//#include "counter.h"

#define KEY 5373
#define SEED 2654435761
unsigned int hashTableSize=1;

unsigned int hashfunction(int value)
{
	return (value*SEED)%hashTableSize;
}

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

//Counter functions
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

//List functions
void List_Init(list_t *list)
{
	list->lock=malloc(sizeof(spinlock_t));
	list->lock->flag=0;
	list->next=NULL;
}

void List_Insert(list_t *list,void *element,unsigned int key)
{
	list_t *new_element=malloc(sizeof(list_t));
	new_element->element=element;
	new_element->key=key;
	new_element->lock=malloc(sizeof(spinlock_t));
	new_element->lock->flag=0;
	spinlock_acquire(list->lock);
	if(list==NULL)
	{
		new_element->next=list;
		list=new_element;
	}
	else
	{
		new_element->next=list->next;
		list->next=new_element;
	}
	spinlock_release(list->lock);
}

void List_Delete(list_t *list, unsigned int key)
{
	list_t *currentPtr=list,*prevPtr=NULL;
	while(currentPtr)
	{
		//I think this should work - NAS
		spinlock_acquire(list->lock);
		if(currentPtr->key==key)
		{
			if(currentPtr==list)
			{
				list=currentPtr->next;
				free(currentPtr);
			}
			else
			{
				prevPtr->next=currentPtr->next;
				free(currentPtr);
				spinlock_release(list->lock);
			}
			break;
		}
		else
		{	
			spinlock_release(list->lock);
			prevPtr=currentPtr;
			currentPtr=currentPtr->next;
		}
	}
}	

void *List_Lookup(list_t *list,unsigned int key)
{
	list_t *currentPtr=list;
	while(currentPtr)
	{
		if(currentPtr->key==key)
		{
			return currentPtr->element;
		}
		else
		{	
			currentPtr=currentPtr->next;
		}
	}

	return NULL;
}
/*
// Should change
//Hash Functions
void Hash_Init(hash_t *hash,int buckets)
{
	hashTableSize=buckets;
	hash=malloc(sizeof(hash_t)*buckets); //Need to allocate memory for all buckets?
}

void Hash_Insert(hash_t *hash, void *element, unsigned int key)
{
	unsigned int hashValue=hashfunction(key);
	//Needs code.
}

void Hash_Delete(hash_t *hash, unsigned int key)
{
	//Needs code
}

void *Hash_Lookup(hash_t *hash, unsigned int key)
{
	//Needs code
}
*/
void *testProg(void *c)
{
	int i;
	list_t *list=(list_t *)c;
	for(i=0;i<100;i++)
		List_Insert(list,list->element,list->key);
}

int main(int argc, char *argv[])
{
	if(argc<2)
	{	
		printf("Enter correct arguments\n");
		return -1;
	}
	int n=atoi(argv[1]);
	pthread_t t1,t2;
	list_t *l1;
	List_Init(l1);
	
	pthread_create(&t1,NULL,testProg,(void *)l1);	
	pthread_create(&t2,NULL,testProg,(void *)l1);

	pthread_join(t1,NULL);	
	pthread_join(t2,NULL);	

	int count=0;
	while(l1)
	{
		count++;
		l1=l1->next;
	}
	printf("Number of elements in list : %d\n",count);
	
	
	return 0;
}
