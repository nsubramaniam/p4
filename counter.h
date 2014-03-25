#include <stdio.h>
#include <stdlib.h>

#ifndef __SPINLOCK_H
#define __SPINLOCK_H

typedef struct __spinlock
{
	volatile unsigned int flag;	
} spinlock_t;

#endif

typedef struct __counter
{
	spinlock_t *lock;
	int value;
} counter_t;

void Counter_Init(counter_t *c, int value);
int Counter_GetValue(counter_t *c);
void Counter_Increment(counter_t *c);
void Counter_Decrement(counter_t *c);

