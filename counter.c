#include <stdio.h>
#include <stdlib.h>

#include "counter.h"

//Counter Functions
void Counter_Init(counter_t *c,int value)
{
	c->lock=malloc(sizeof(spinlock_t));
	c->lock->flag=0;
	spinlock_acquire(c->lock);
	c->value=value;
	spinlock_release(c->lock);
}

int Counter_GetValue(counter_t *c)
{
	int value;
	spinlock_acquire(c->lock);
	value = c->value;
	spinlock_release(c->lock);
	return value;
}

void Counter_Increment(counter_t *c)
{
	spinlock_acquire(c->lock);
	c->value+=1;
	spinlock_release(c->lock);
}

void Counter_Decrement(counter_t *c)
{
	spinlock_acquire(c->lock);
	c->value-=1;
	spinlock_release(c->lock);
}
