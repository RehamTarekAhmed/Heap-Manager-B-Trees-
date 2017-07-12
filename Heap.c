/****************************************************************************
 * Heap.c
 * The implementation of the Heap Manager's functionality
 * using the binary tree data structure.
 * Author: Reham Tarek <RehamTarekAhmed@gmail.com>
 ***************************************************************************/
#include "Heap.h"

block_t* head = NULL;
block_t* headf = NULL;

 /*-----------------------------------------------------------------------------------------------------------*/
bool memalloc_init()
{
  head = (block_t*)malloc(MAX_HEAP_SIZE); /*Or sbrk*/
  if(!head) return false;
  //head = (block_t*) sbrk(MAX_HEAP_SIZE); /*On  success,  sbrk() returns  the  previous program break.*/
   //if (head == (void *)-1) /*On error, (void *) -1 is returned and errno is set to ENOMEM*/
    //   return false;
  head->end = true;
  head->size = MAX_HEAP_SIZE-METADATA;
  head->free = true;
  head->prevsize=1;
  head->right=NULL;
  head->leftfree=1;
  head->freedirection= '+';
  insertf(head);
  return true;
}
/*-----------------------------------------------------------------------------------------------------------*/
void* memalloc(unsigned int reqsize)
{
    reqsize= ALIGN(reqsize);
    if(reqsize <= 0 || reqsize > MAX_HEAP_SIZE-METADATAF || ((head == NULL) && !memalloc_init()))
          return NULL;
    reqsize-=8; /*Because I can always afford the 8 bytes, i don't need the right pointer anymore*/
    pthread_mutex_lock(&lock);
   block_t*current= search(reqsize);
    if (current==NULL)
         { pthread_mutex_unlock(&lock); return NULL;} /*No suitable chunk was found*/
     //assert(reqsize <= current->size);
    removef(current); /* Remove the chunk you found from the free list */
    current->free = false;
    if (current->size >= (reqsize+METADATA)) /*Split if the chunk after spiltting supports the metadata and a minimal block*/
    {
      block_t* newblock=(block_t *)(((char*) current) + reqsize + METADATA);
      newblock->free = true;
      newblock->size = current->size-reqsize-METADATA;
      newblock->prevsize= reqsize;
      newblock->end=current->end;
      newblock->leftfree=1;
      newblock->freedirection='+';
      newblock->right = NULL;
      current->size = reqsize;
      current->end=false;
      block_t* next=set_Next(newblock);
      if(next)
          next->prevsize=newblock->size;
      insertf(newblock);
  } /*split complete*/
  pthread_mutex_unlock(&lock);
    return (void*)(((char*)current) + METADATAF); /* return a pointer to the allocated segment */
}
/*-----------------------------------------------------------------------------------------------------------*/
void memfree(void *ptr)
{
  if (ptr== NULL) /*Return if invalid block*/
    return;
  block_t* to_free = (block_t*)((char*)ptr - METADATAF);
  if ( to_free->free == true) return;
  pthread_mutex_lock(&lock);
  block_t* pre =set_prev(to_free); /*Obtain the previous block*/
  to_free->free = true;
  if(pre && pre->free)
  {
    removef(pre);
    to_free = fusion (pre);
  }
  block_t* next= set_Next(to_free); /*Obtain the next block*/
  if(next && next->free)
  {
    removef(next);
    fusion (to_free);
  }
  insertf(to_free);
  pthread_mutex_unlock(&lock);
}

/*-----------------------------------------------------------------------------------------------------------*/
block_t* search(unsigned int reqsize)
{
    if(!headf)
         return NULL;
    unsigned int bestfit=MAX_HEAP_SIZE;
    block_t* current=headf; block_t* pred=NULL;
    while(current && current->size!=reqsize)
    {
      if(current->size<=bestfit && current->size>reqsize )
      {pred=current; bestfit=current->size;} /*Update the first fit block */
      if(current->size>reqsize)
           current=set_left(current);
      else if (current->size<reqsize)
           current=current->right;
    }
    if (current!=NULL) /*found a fitting block*/
       return current;
    else /*return the best fit block*/
       return pred;
  }
/*-----------------------------------------------------------------------------------------------------------*/
block_t* fusion (block_t* b)
  {
    block_t* next= set_Next(b);
    b->size += METADATA + next->size;
    b->end=next->end;
    if (!b->end)
      {next=set_Next(b);
      next->prevsize= b->size;}
  return (b);
  }
  /*-----------------------------------------------------------------------------------------------------------*/
  bool removef(block_t* freeblock)
  {  if ((freeblock && !freeblock->free)||headf==NULL)
          return false;
     block_t* temp=NULL; block_t*prev;
    if(freeblock->right && freeblock->leftfree!=1)  /* Replace the node with the minimum element in the right sub tree*/
          {
            temp = FindMin(freeblock->right);
            prev= FindPrevnode(freeblock);
            removef(temp); /* As we're going to replace it with some other node, we have to delete that node */
            if(prev==NULL)
                 headf=temp;
            else if(prev->right==freeblock)
                 prev->right=temp;
            else
                set_dir( prev,temp);
            set_dir(temp,set_left(freeblock));
            temp->right=freeblock->right;
          }
      else
          {/* If there is only 1 or 0 children, we can directly remove it & connect its parent to its child */
             if(set_left(freeblock) != NULL)
                   temp = set_left(freeblock);
             else if(freeblock->right != NULL)
                   temp= freeblock->right;
             prev= FindPrevnode(freeblock);
             if(prev==NULL)
                    headf=temp;
             else if (prev->right==freeblock)
                    prev->right=temp;
              else
                    set_dir(prev,temp);
            }
            freeblock->right=NULL;
            freeblock->leftfree=1;
            return true;
         }
/*-----------------------------------------------------------------------------------------------------------*/
bool insertf(block_t* freeblock)
{   if (!freeblock->free)
          return false;
    freeblock->leftfree=1;
    freeblock->right = NULL;
    block_t*node=headf;
    if(headf==NULL)
        {headf=freeblock; return true;}
   while(node!= NULL) /*else*/
  {
    if( freeblock->size >= node->size)
          if(node->right==NULL)
              {node->right=freeblock; node=NULL;}
           else
              node=node->right;
    else
          if(node->leftfree==1)
              {set_dir(node,freeblock);  node=NULL;}
          else
              node = set_left(node);
  }
  return true;
}
/*-----------------------------------------------------------------------------------------------------------*/
void set_dir(block_t* p,block_t* n)
{ if (!p || p==n)
      return;
  else if(n==NULL)
  {   p->leftfree= 1;
      return;
  }
  p->leftfree= abs((char*)p-(char*)n);
  if((p-n)>0)
      p->freedirection='+';
  else
      p->freedirection='-';
}
/*-----------------------------------------------------------------------------------------------------------*/
block_t* set_left(block_t* p)
{ if (!p || p->leftfree==1 ) return NULL;
  if (p->freedirection=='-')
    return ((block_t*)((char*)p + p->leftfree));
  else
    return ((block_t*)((char*)p - p->leftfree));
}
block_t* set_prev(block_t* p)
{
 if(p && p->prevsize!=1)
    return (block_t*)((char*)p - METADATA- p->prevsize);
  else
    return NULL;
}
block_t* set_Next(block_t* p)
{
   if (p && !(p->end))
    return (block_t*)((char*)p + METADATA+ p->size);
  else
    return NULL;
}
/*-----------------------------------------------------------------------------------------------------------*/
block_t* FindMin(block_t*node)
{  if(node==NULL)
        return NULL;
   if(set_left(node)!=NULL) /* Go to the left sub tree to find the min element */
         return FindMin(set_left(node));
   else
        return node;
}

 block_t* FindPrevnode(block_t *node)
{  if(!headf)
        return NULL;
   block_t* current=headf; block_t* pred=NULL;
   while(current && current!=node)
   {  pred=current;
     if(node->size < current->size)
          current=set_left(current);
     else if (node->size >= current->size)
          current=current->right;
   }
   if (current==node)
      return pred;
   else
      return NULL;
 }
/*-----------------------------------------------------------------------------------------------------------*/
void print_list()
{     unsigned int total=0;
      block_t *blocklist_head = head;
    //  rlock;
      while(blocklist_head)
      {
         block_t* next= set_Next(blocklist_head);
         block_t* prev= set_prev(blocklist_head);
         printf( "\x1b[35m \t blocklist Size:%u, Head:%p, Prev:%p, Next:%p, Free:%d\t\n \x1b[0m",blocklist_head->size+8,blocklist_head,prev,next,blocklist_head->free);
         blocklist_head = next;
      }
      total=inOrderTraverse(headf,total);
    //  //unlock;
      printf("TOTAL FREE: %u\t",total);
      printf("\n");
  }


unsigned int inOrderTraverse(block_t* bt,unsigned int total)
  {   if (headf==NULL)
              return 0;
      if (bt == NULL)
              return total;
      total=inOrderTraverse(set_left(bt),total);
      printf( "\x1b[34m \t blocklist Size:%u, Head:%p, Prev Free:%p, Next Free:%p, Free:%d\t\n \x1b[0m",bt->size+8,bt,set_left(bt),bt->right,bt->free);
      total += (bt->size+8);
      total=inOrderTraverse(bt->right,total);
      return total;
  }

  /*-----------------------------------------------------------------------------------------------------------*/
  void calledFirst()
  {
    //pthread_rwlock_init(&lock, NULL);
    memalloc_init();
  }

void calledLast()
{
  free(head);
  //sbrk(-MAX_HEAP_SIZE);
  head=NULL;
  //pthread_rwlock_destroy(&lock);
}
