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

typedef struct __hash
{
	spinlock_t *lock;
	void *element;
	struct __hash *next;
	unsigned int key;
} hash_t;

void Counter_Init(counter_t *c, int value);
int Counter_GetValue(counter_t *c);
void Counter_Increment(counter_t *c);
void Counter_Decrement(counter_t *c);

void List_Init(list_t *list);
void List_Insert(list_t *list,void *element,unsigned int key);
void List_Delete(list_t *list, unsigned int key);
void *List_Lookup(list_t *list,unsigned int key);

void Hash_Init(hash_t *hash,int buckets);
void Hash_Insert(hash_t *hash, void *element, unsigned int key);
void Hash_Delete(hash_t *hash, unsigned int key);	
void *Hash_Lookup(hash_t *hash, unsigned int key);	
