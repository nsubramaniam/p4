/* Simple hash delete
*/

#include <sys/time.h>

#include "p4.h"
#include "hash.c"
#include "list.c"
#include "counter.c"
#include "spin.c"

void *testProg(void *c)
{
	int i;
	for(i=0;i<1000000;i++)
		Counter_Increment((counter_t *)c);
}

int main(int argc, char *argv[])
{
	int i;
	struct timeval start,end;
	counter_t c;
	Counter_Init(&c,0);
	pthread_t thread[20];
	gettimeofday(&start,NULL);
	
	for(i=0;i<atoi(argv[1]);i++)
		pthread_create(&thread[i],NULL,testProg,(void *)&c);

	for(i=0;i<atoi(argv[1]);i++)
		pthread_join(thread[i],NULL);

	gettimeofday(&end,NULL);

	double time=((end.tv_sec-start.tv_sec)*1000000.00 + (end.tv_usec-start.tv_usec))/1000000.00;
	printf("%d,%d,%f\n",atoi(argv[1]),c.value,time);
	return 0;
}
