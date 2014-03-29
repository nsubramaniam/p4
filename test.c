/* Simple hash delete
*/

#include "p4.h"
#include "hash.c"
#include "list.c"
#include "spin.c"

int main()
{
    int element = 255;
    unsigned int key = 127;
    hash_t hs;
    Hash_Init(&hs, 32);
    Hash_Delete(&hs, key);
return 0;
}
/*
    void *val = Hash_Lookup(&hs, key);
    if (!val)
	return 0;
    else
	return -1;
}*/
