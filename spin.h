#include <stdio.h>
#include <stdlib.h>

typedef struct __spinlock
{
	short key;	
	short value;
}spinlock_t;
