#include <stdio.h>
#include <stdlib.h>

#include "list.h"

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
	list_t *prevPtr=list;
	spinlock_acquire(list->lock);
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
