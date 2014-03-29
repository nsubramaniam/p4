#ifndef LIST_H
#define LIST_H
#include "spin.h"

typedef struct __list
{
	spinlock_t *lock;
	void *element;
	struct __list *next;
	unsigned int key;
} list_t;

void List_Init(list_t *list);
void List_Insert(list_t *list,void *element,unsigned int key);
void List_Delete(list_t *list, unsigned int key);
void *List_Lookup(list_t *list,unsigned int key);

#endif
