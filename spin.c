#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "counter.h"
#include "hash.h"

#define SEED 2654435761
unsigned int hashTableSize=1;
unsigned short flag=0;

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
	int value;
	spinlock_acquire(&(c->lock));
	value = c->value;
	spinlock_acquire(&(c->lock));
	return value;
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
	list->element=NULL;
	list->next=NULL;
	list->lock=malloc(sizeof(spinlock_t));
	list->lock->flag=0;
}

void List_Insert(list_t *list,void *element,unsigned int key)
{
	list_t *new_element=malloc(sizeof(list_t)),*currentPtr=list;
	new_element->element=element;
	new_element->key=key;
	new_element->lock=malloc(sizeof(spinlock_t));
	new_element->lock->flag=0;
	while(currentPtr)
	{
		if(currentPtr->lock->flag==0)
		{
			spinlock_acquire(currentPtr->lock);
			new_element->next=currentPtr->next;
			currentPtr->next=new_element;
			spinlock_release(currentPtr->lock);
			break;
		}
		else
		{
			if(currentPtr->next!=NULL)
				currentPtr=currentPtr->next;
			else
				currentPtr=list;
		}
	}
}

void List_Delete(list_t *list, unsigned int key)
{
	list_t *currentPtr=list->next,*prevPtr=list;
	while(currentPtr)
	{
		if(currentPtr->key==key)
		{
			spinlock_acquire(currentPtr->lock);
			prevPtr->next=currentPtr->next;
			spinlock_release(currentPtr->lock);
			free(currentPtr);
			break;
		}
		else
		{	
			prevPtr=currentPtr;
			currentPtr=currentPtr->next;
		}
	}
}	

void *List_Lookup(list_t *list,unsigned int key)
{
	list_t *currentPtr=list->next;
	void *element=NULL;
	while(currentPtr)
	{
		if(currentPtr->key==key)
		{
			spinlock_acquire(currentPtr->lock);
			element = currentPtr->element;
			spinlock_release(currentPtr->lock);
			break;
		}
		else
		{	
			currentPtr=currentPtr->next;
		}
	}

	return element;
}

//Hash Functions
void Hash_Init(hash_t *hash,int buckets)
{
	hashTableSize=buckets;
	hash_t *currentPtr=hash;
	int i;
	for(i=0;i<buckets;i++)
	{
		currentPtr->list=malloc(sizeof(list_t));
		currentPtr->list->next=NULL;
		currentPtr->list->element=NULL;
		currentPtr->list->key=0;
		currentPtr->list->lock=malloc(sizeof(spinlock_t));
		currentPtr->list->lock->flag=0;
		currentPtr+=sizeof(hash_t);
	}
}

void Hash_Insert(hash_t *hash, void *element, unsigned int key)
{
	unsigned int hashValue=hashfunction(key);
	hash_t *currentPtr=hash+(sizeof(hash_t)*hashValue);
	List_Insert(currentPtr->list,element,key);
}

void Hash_Delete(hash_t *hash, unsigned int key)
{
	unsigned int hashValue=hashfunction(key);
	hash_t *currentPtr=hash+(sizeof(hash_t)*hashValue);
	List_Delete(currentPtr->list,key);
}

void *Hash_Lookup(hash_t *hash, unsigned int key)
{
	unsigned int hashValue=hashfunction(key);
	void *element;
	hash_t *currentPtr=hash+(sizeof(hash_t)*hashValue);
	element = List_Lookup(currentPtr->list,key);
	return element;
}


//Test Programs
void *testProg(void *c)
{
	int i;
	for(i=0;i<13;i++)
		Hash_Insert((hash_t *)c,"hi",i);
}

void *testProg2(void *c)
{
	int i;
	for(i=0;i<10;i++)
		Hash_Delete((hash_t *)c,10);
}


int main(int argc, char *argv[])
{
	int buckets=10;
	hash_t *hash=malloc(sizeof(hash_t)*buckets);
	Hash_Init(hash,buckets);
	hash_t *currentPtr=hash;
	int i;
	for(i=0;i<buckets;i++)
	{
		printf("Lock = %d\n",currentPtr->list->lock->flag);	
		currentPtr+=sizeof(hash_t);
	}
/*
	pthread_t t1,t2;
	pthread_create(&t1,NULL,testProg,hash);
	pthread_create(&t2,NULL,testProg,hash);
	
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
*/
	return 0;
}
