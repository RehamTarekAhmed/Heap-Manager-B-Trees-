/****************************************************************************
* Version: 1.0
* Created: 18/6/2017
* Author: Reham Tarek <RehamTarekAhmed@gmail.com>
* Test3: Obtain the avg time for both functions
 ***************************************************************************/
 #include <assert.h>
 #include <sys/time.h>
 #include <time.h>
 #include "Heap.h"
 #define RAND() (drand48())
 #define SEED(x) (srand48((x)))
 #define PSEUDO_RANDOM_SEED 0 /*toggle it for random results for every run*/
 #define LOOPCNT 50000
 #define MAX_ALLOC_SIZE (MAX_HEAP_SIZE/10000) /**/

 double time_diff(struct timeval x , struct timeval y);

 int main(int argc, char *argv[])
{  struct timeval begin, end, begin2, end2;
  int size, itr, i, fail= 0, success=0, j=0;
  void *ptr[LOOPCNT]={NULL};
  double randvar,time_spent=0.0,time_spent2=0.0;;
  if(PSEUDO_RANDOM_SEED)   /* Set the PSEUDO_RANDOM_SEED for pseduo random seed initialization based on time */
    SEED(time(NULL));
  for(i = 0; i < LOOPCNT; i++)
  {
    size = (int)(RAND() * MAX_ALLOC_SIZE);
    randvar = RAND();
    itr = (int)(RAND() * LOOPCNT);
    while(size == 0)
        size = (int)(RAND() * MAX_ALLOC_SIZE);
    gettimeofday(&begin , NULL);
    ptr[i] = memalloc(size);
	  gettimeofday(&end , NULL);

    if(ptr[i] == NULL)
      ++fail;
    else
    {    ++success;
         time_spent+= time_diff(begin, end);
    }

    if( ptr[itr] != NULL)
    {
      gettimeofday(&begin2 , NULL);
      memfree(ptr[itr]);
			gettimeofday(&end2 , NULL);
			time_spent2 += time_diff(begin2, end2);
			j++;
      ptr[itr] = NULL;
    }
  }
  for(i=0; i < LOOPCNT; i++) {
    if(ptr[i] != NULL) {
      gettimeofday(&begin2 , NULL);
      memfree(ptr[i]);
      gettimeofday(&end2 , NULL);
      time_spent2 += time_diff(begin2, end2);
      j++;
      ptr[i] = NULL;
    }
  }
  printf("Test case summary\n");
  printf("Loop count: %d, malloc successful: %d, malloc failed: %d\n", success+fail, success, fail);
  printf("Average memalloc execution time: %g microseconds\t Average memfree execution time:%g microseconds\n", time_spent/success,time_spent2/j);
  return 0;
}

double time_diff(struct timeval x , struct timeval y)
{
    double diff = (y.tv_sec - x.tv_sec)*1000000 + (y.tv_usec - x.tv_usec);
    return diff;
}
