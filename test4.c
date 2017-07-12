/*#include "Heap.h"
int main(int argc, char const *argv[]) {
  memalloc(8000);
print_list();
printf("%lu\n",METADATA);	return 0;
}
*/
#include <omp.h>
#include <pthread.h>
#include <assert.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "Heap.h"
#define LOOPCNT 5000
#undef calculate
#undef getrusage
char *ptr[LOOPCNT]={NULL};
#define RAND() (drand48())
#define SEED(x) (srand48((x)))
#define MAX_ALLOC_SIZE (MAX_HEAP_SIZE/1000) /**/
double calculate(const struct rusage* b, const struct rusage* a);
int fail=0,success=0;

void threadFunc(int i)
{ int itr,size;  double randvar;
    size = (int)(RAND() * MAX_ALLOC_SIZE);
    while(size == 0)
        size = (int)(RAND() * MAX_ALLOC_SIZE);
    itr = (int)(RAND() * LOOPCNT/10);
    randvar = RAND();
    printf("Loop %i will malloc: %u\n", i,size);
		ptr[i]= memalloc(size);
		if(ptr[i] == NULL)
			++fail;
		else
		{
      //printf("Loop %i malloced: %u\n", i,size);
			++success;
		}
		if( ptr[itr] != NULL)
    {
			printf("Freeing ptr[%d]\n", itr);
			memfree(ptr[itr]);
			ptr[itr] = NULL;
		}
}
int main(void)
{ struct rusage begin, end;
  double time_spent;
  getrusage(RUSAGE_SELF, &begin);
  int thread_id, i=0,loop=0;
#pragma omp parallel for
        for(i=0;i<LOOPCNT;i++)
    {thread_id = omp_get_thread_num();
      printf("%i\n", i);
    threadFunc(i);
  //  printf("Thread %d performed \n", thread_id);
  }

	printf("main waiting for thread to terminate...\n");


	getrusage(RUSAGE_SELF, &end);
	for(i=0; i < LOOPCNT; i++)
  {
    if(ptr[i] != NULL) {
      memfree(ptr[i]);
      ptr[i] = NULL;
    }
  }
  time_spent= calculate(&begin, &end);
  //print_list();
  printf("\n");
  printf("Test case summary\n");
  printf("Loop count: %d, malloc successful: %d, malloc failed: %d, execution time: %g seconds\n\n", success+fail, success, fail, time_spent);
	return 0;
}

double calculate(const struct rusage* b, const struct rusage* a)
{
    if (b == NULL || a == NULL)
    {
        return 0.0;
    }
    else
    {
        return ((((a->ru_utime.tv_sec * 1000000 + a->ru_utime.tv_usec) -
                 (b->ru_utime.tv_sec * 1000000 + b->ru_utime.tv_usec))+
                ((a->ru_stime.tv_sec * 1000000 + a->ru_stime.tv_usec) -
                 (b->ru_stime.tv_sec * 1000000 + b->ru_stime.tv_usec)))/ 1000000.0);
    }
}
