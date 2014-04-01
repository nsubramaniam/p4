#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "hash.h"

#define SEED 2654435761
unsigned int hashTableSize=1;

unsigned int hashfunction(int value)
{
	return (value*SEED)%hashTableSize;
}

//Hash Functions
void Hash_Init(hash_t *hash,int buckets)
{
	hashTableSize=buckets;
	int i;
	for(i=0;i<buckets;i++)
	{	
		hash_t *currentPtr=(void *)hash+(sizeof(list_t)*i);
		currentPtr->list=malloc(sizeof(list_t));
		List_Init(currentPtr->list);
	}
}

void Hash_Insert(hash_t *hash, void *element, unsigned int key)
{
	unsigned int hashValue=hashfunction(key);
	hash_t *currentPtr=(void *)hash+(sizeof(list_t)*hashValue);
	List_Insert(currentPtr->list,element,key);
}

void Hash_Delete(hash_t *hash, unsigned int key)
{
	unsigned int hashValue=hashfunction(key);
	hash_t *currentPtr=(void *)hash+(sizeof(list_t)*hashValue);
	List_Delete(currentPtr->list,key);
}

void *Hash_Lookup(hash_t *hash, unsigned int key)
{
	unsigned int hashValue=hashfunction(key);
	void *element;
	hash_t *currentPtr=(void *)hash+(sizeof(list_t)*hashValue);
	element = List_Lookup(currentPtr->list,key);
	return element;
}
