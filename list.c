#include <stdio.h>
#include <stdlib.h>
//#include <pthread.h>

#include "list.h"

//pthread_mutex_t lock;

//List functions
void List_Init(list_t *list)
{
	list->element=NULL;
	list->next=NULL;
	//pthread_mutex_init(&lock,NULL);
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
	spinlock_acquire(list->lock);
	//pthread_mutex_lock(&lock);
	new_element->next=currentPtr->next;
	currentPtr->next=new_element;
	spinlock_release(list->lock);
	//pthread_mutex_unlock(&lock);
}

void List_Delete(list_t *list, unsigned int key)
{
	list_t *prevPtr=list;
	spinlock_acquire(list->lock);
	//pthread_mutex_lock(&lock);
	list_t *currentPtr=list->next;
	while(currentPtr)
	{
		if(currentPtr->key==key)
		{
			prevPtr->next=currentPtr->next;
			free(currentPtr);
			break;
		}
		else
		{	
			prevPtr=currentPtr;
			currentPtr=currentPtr->next;
		}
	}
	spinlock_release(list->lock);
	//pthread_mutex_unlock(&lock);
}	

void *List_Lookup(list_t *list,unsigned int key)
{
	list_t *currentPtr=list->next;
	void *element=NULL;
	spinlock_acquire(list->lock);
	//pthread_mutex_lock(&lock);
	while(currentPtr)
	{
		if(currentPtr->key==key)
		{
			element = currentPtr->element;
			break;
		}
		else
		{	
			currentPtr=currentPtr->next;
		}
	}
	spinlock_release(list->lock);
	//pthread_mutex_unlock(&lock);

	return element;
}
