#ifndef SPINLOCK_H
#define SPINLOCK_H

typedef struct __spinlock
{
	volatile unsigned int flag;	
} spinlock_t;

void* spinlock_acquire(void* lock);
void* spinlock_release(void* lock);

#endif
