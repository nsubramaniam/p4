#include<stdio.h>
#include<stdlib.h>
typedef struct __hash
{
	spinlock_t *lock;
	void *element;
	struct __hash *next;
	unsigned int key;
} hash_t;

void Hash_Init(hash_t *hash,int buckets);
void Hash_Insert(hash_t *hash, void *element, unsigned int key);
void Hash_Delete(hash_t *hash, unsigned int key);	
void *Hash_Lookup(hash_t *hash, unsigned int key);	
