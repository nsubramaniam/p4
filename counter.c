#include <stdio.h>
#include <stdlib.h>
//#include <pthread.h>

#include "counter.h"

//pthread_mutex_t lock;

//Counter Functions
void Counter_Init(counter_t *c,int value)
{
	//pthread_mutex_init(&lock,NULL);
	c->lock=malloc(sizeof(spinlock_t));
	c->lock->flag=0;
	spinlock_acquire(c->lock);
	//pthread_mutex_lock(&lock);
	c->value=value;
	spinlock_release(c->lock);
	//pthread_mutex_unlock(&lock);
}

int Counter_GetValue(counter_t *c)
{
	int value;
	spinlock_acquire(c->lock);
	//pthread_mutex_lock(&lock);
	value = c->value;
	spinlock_release(c->lock);
	//pthread_mutex_unlock(&lock);
	return value;
}

void Counter_Increment(counter_t *c)
{
	spinlock_acquire(c->lock);
	//pthread_mutex_lock(&lock);
	c->value+=1;
	spinlock_release(c->lock);
	//pthread_mutex_unlock(&lock);
}

void Counter_Decrement(counter_t *c)
{
	spinlock_acquire(c->lock);
	//pthread_mutex_lock(&lock);
	c->value-=1;
	spinlock_release(c->lock);
	//pthread_mutex_unlock(&lock);
}
