// filename *************************heap.h ************************
// Implements memory heap for dynamic memory allocation.
// Follows standard malloc/calloc/realloc/free interface
// for allocating/unallocating memory.

// Jacob Egner 2008-07-31
// modified 8/31/08 Jonathan Valvano for style

// You may use, edit, run or distribute this file 
//    as long as the above notices remain


#ifndef HEAP_H
#define HEAP_H


// feel free to change HEAP_SIZE_BYTES to however
// big you want the heap to be
#define HEAP_SIZE_BYTES (256)
#define HEAP_SIZE_WORDS (HEAP_SIZE_BYTES / sizeof(short))

#define HEAP_OK 0
#define HEAP_ERROR_CORRUPTED_HEAP 1
#define HEAP_ERROR_POINTER_OUT_OF_RANGE 2

// struct for holding statistics on the state of the heap
typedef struct heap_stats {
  short wordsAllocated;
  short wordsAvailable;
  short wordsOverhead;
  short blocksUsed;
  short blocksUnused;
} heap_stats_t;

//******** Heap_Init *************** 
// Initialize the Heap
// input: none
// output: always HEAP_OK
// notes: Initializes/resets the heap to a clean state where no memory
//  is allocated.
short Heap_Init(void);


//******** Heap_Malloc *************** 
// Allocate memory, data not initialized
// input: 
//   desiredBytes: desired number of bytes to allocate
// output: void* pointing to the allocated memory or will return NULL
//   if there isn't sufficient space to satisfy allocation request
void* Heap_Malloc(short desiredBytes);


//******** Heap_Calloc *************** 
// Allocate memory, data are initialized to 0
// input:
//   desiredBytes: desired number of bytes to allocate
// output: void* pointing to the allocated memory block or will return NULL
//   if there isn't sufficient space to satisfy allocation request
//notes: the allocated memory block will be zeroed out
void* Heap_Calloc(short desiredBytes);


//******** Heap_Realloc *************** 
// Reallocate buffer to a new size
//input: 
//  oldBlock: pointer to a block
//  desiredBytes: a desired number of bytes for a new block
//    where the contents of the old block will be copied to
// output: void* pointing to the new block or will return NULL
//   if there is any reason the reallocation can't be completed
// notes: the given block will be unallocated after its contents
//   are copied to the new block
void* Heap_Realloc(void* oldBlock, short desiredBytes);


//******** Heap_Free *************** 
// return a block to the heap
// input: pointer to memory to unallocate
// output: HEAP_OK if everything is ok;
//  HEAP_ERROR_POINTER_OUT_OF_RANGE if pointer points outside the heap;
//  HEAP_ERROR_CORRUPTED_HEAP if heap has been corrupted or trying to
//  unallocate memory that has already been unallocated;
short Heap_Free(void* pointer);


//******** Heap_Test *************** 
// Test the heap
// input: none
// output: validity of the heap - either HEAP_OK or HEAP_ERROR_HEAP_CORRUPTED
short Heap_Test(void);


//******** Heap_Stats *************** 
// return the current status of the heap
// input: none
// output: a heap_stats_t that describes the current usage of the heap
heap_stats_t Heap_Stats(void);


#endif //#ifndef HEAP_H
