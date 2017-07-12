/****************************************************************************
* Version: 1.0
* Created: 18/6/2017
* Author: Reham Tarek <RehamTarekAhmed@gmail.com>
* Test2: Checking efficiency
 ***************************************************************************/
 #include <assert.h>
 #include <sys/resource.h>
 #include <sys/time.h>
 #include <time.h>
 #include "Heap.h"
 #define RAND() (drand48())
 #define SEED(x) (srand48((x)))
 #define PSEUDO_RANDOM_SEED 0 /*toggle it for random results for every run*/
 #define LOOPCNT 5000
 #define MAX_ALLOC_SIZE (MAX_HEAP_SIZE/1000) /**/
 #undef calculate
 #undef getrusage

double calculate(const struct rusage* b, const struct rusage* a);
void occupy(int size, char*a) /*Modify data*/
 {
   int j;
   for(j=0; j < size-1; j++)
    {
        a[j] = 'a';
    }
   a[size-1] = '\0';
 //print_list();
}
 int main(int argc, char *argv[])
{
  struct rusage begin, end;   char *ptr[LOOPCNT]={NULL};
  int size, itr,i,fail = 0, success=0;   double randvar,time_spent;
  if(PSEUDO_RANDOM_SEED)   /* Set the PSEUDO_RANDOM_SEED for pseduo random seed initialization based on time */
    SEED(time(NULL));
  getrusage(RUSAGE_SELF, &begin); /*Start calculating time*/
  for(i = 0; i < LOOPCNT; i++)
  {
    size = (int)(RAND() * MAX_ALLOC_SIZE);   randvar = RAND();
    itr = (int)(RAND() * LOOPCNT);
    while(size == 0)
         size = (int)(RAND() * MAX_ALLOC_SIZE);
    ptr[i] = memalloc(size);
    if(ptr[i] == NULL)
      ++fail;
    else
    { ++success;  occupy(size,ptr[i]); } /*A function that writes to the whole of the allocated size*/
    if( ptr[itr] != NULL) /*Random memory free to introduce fargmentation*/
     { memfree(ptr[itr]);  ptr[itr] = NULL;}
  }
  getrusage(RUSAGE_SELF, &end); /*Stop calculating time*/
  for(i=0; i < LOOPCNT; i++) /*Free all pointers*/
  {
    if(ptr[i] != NULL)
    { memfree(ptr[i]);  ptr[i] = NULL; }
  }
  time_spent= calculate(&begin, &end);
	print_list();
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
