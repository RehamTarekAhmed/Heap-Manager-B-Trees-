/****************************************************************************
* Created: 18/6/2017
* Author: Reham Tarek <RehamTarekAhmed@gmail.com>
* Test1: checking the sainty of the code
* 0-> allocated 1-> free
*The main list is represented in magenta and the Free list is represented in blue.
 ***************************************************************************/
 #include <assert.h>
 #include "Heap.h"

 int main(int argc, char *argv[])
{
	void *ptr[6];
  int size = MAX_HEAP_SIZE/ 4;

  /* Allocate 3 consecutive chunks 0001*/
  int i;
  for(i=0;i<3;i++)
    {
    ptr[i] = memalloc(size);
    assert(ptr[i]);
    }
  /* Free the middle 0101*/
   memfree(ptr[1]);
   print_list();
   /*allocate a new small chunk, it should be allocated within the smaller free chunk 0100001 */
   ptr[3] = memalloc(50000);
   ptr[4] = memalloc(50000);
   ptr[5] = memalloc(50000);
   memfree(ptr[4]); /*0100101*/
   print_list();
   ptr[4] = memalloc(5000); /*01000101*/
   print_list();
   /*Free the all chunks to check that fusion works*/
   memfree(ptr[2]);
   memfree(ptr[0]);
   memfree(ptr[4]);
   memfree(ptr[5]);
   memfree(ptr[3]);
   print_list();
  return 0;
}
