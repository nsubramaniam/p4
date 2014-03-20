#include <stdio.h>
#include <stdlib.h>

typedef struct __spinlock
{
	volatile unsigned int flag;	
} spinlock_t;

typedef struct __counter
{
	spinlock_t *lock;
	int value;
} counter_t;

typedef struct __list
{
	spinlock_t *lock;
	void *element;
	struct __list *next;
	unsigned int key;
} list_t;

void Counter_Init(counter_t *c, int value);
int Counter_GetValue(counter_t *c);
void Counter_Increment(counter_t *c);
void Counter_Decrement(counter_t *c);

void List_Init(list_t *list);
void List_Insert(list_t *list,void *element,unsigned int key);
void List_Delete(list_t *list, unsigned int key);
void *List_Lookup(list_t *list,unsigned int key);	
