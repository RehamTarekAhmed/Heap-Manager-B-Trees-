/****************************************************************************
 * Heap.h
 * Declares the Heap Manager's functionality.
 * Author: Reham Tarek <RehamTarekAhmed@gmail.com>
 ***************************************************************************/
#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_HEAP_SIZE (1<<27) /*134217728*/
#define ALIGNMENT 	8 /*64-bit systems */
#define ALIGN(x) (((x)+(ALIGNMENT-1)) & ~(ALIGNMENT-1)) /* A macro that rounds up to the nearest multiple of ALIGNMENT */

typedef struct block {/*contains the Metadata for each block, arranged for a better alignment*/
    bool free; /* 1 byte*/
    bool end; /*like our old fashoned NULL indicates the end of the list*/
    char freedirection; /*Linked with leftfree*/
    unsigned int size; /*contains the size of the data object or the amount of free bytes, 4 bytes*/
    unsigned int prevsize; /* 1 means no previous block*/
    unsigned int leftfree;
    struct block* right; /*8 bytes*/
 } block_t;

#define METADATA (sizeof(block_t)) /*Aligned*/
#define METADATAF (sizeof(block_t)-8) /*Aligned*/

void* memalloc(unsigned int reqsize); /*Allocates a contiguous block of memory of at least reqsize & returns a ptr of type void*/
void memfree(void *ptr); /* marks the block of memory pointed to by ptr as free*/
bool memalloc_init();
bool insertf(block_t* freeblock);
bool removef(block_t* freeblock);
block_t* fusion (block_t* b);
void set_dir(block_t* p,block_t* n); /*usage: 2nd,1st*/
block_t* set_left(block_t* p);
block_t* set_Next(block_t* p);
block_t* set_prev(block_t* p);
block_t* FindPrevnode(block_t *node);
block_t* FindMin(block_t*node);
block_t* search(unsigned int reqsize);
void __attribute__((constructor)) calledFirst();
void __attribute__((destructor)) calledLast();
/*DEBUGGING*/
void print_list();
unsigned int inOrderTraverse(block_t* bt,unsigned int total);
/*For thread safety*/
pthread_mutex_t lock;
/*
pthread_rwlock_t lock;
#define rlock  (pthread_rwlock_rdlock(&lock))
#define wlock  (pthread_rwlock_wrlock(&lock))
#define unlock (pthread_rwlock_unlock(&lock))
*/
#endif
