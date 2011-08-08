// filename *************************heap.c ************************
// Implements memory heap for dynamic memory allocation.
// Follows standard malloc/calloc/realloc/free interface
// for allocating/unallocating memory.

// Jacob Egner 2008-07-31
// modified 8/31/08 Jonathan Valvano for style

// You may use, edit, run or distribute this file 
//    as long as the above notices remain

// Implementation Notes:
// This is a Knuth Heap. Each block has a header and a trailer, which I shall
// call the meta-sections.  The meta-sections are each a single short that tells
// how many shorts/words can be stored between the header and trailer.
// If the block is used, the meta-sections record the room as a positive
// number.  If the block is unused, the meta-sections record the room as a
// negative number.

#include "heap.h"

#define HEAP_START (Heap)
#define HEAP_END (HEAP_START + HEAP_SIZE_WORDS)

//The actual heap is just a big array.
static short Heap[HEAP_SIZE_WORDS];

static short inHeapRange(short* address);
static short blockUsed(short* block);
static short blockUnused(short* block);
static short blockRoom(short* block);
static short blockSize(short* block);
static short* blockHeader(short* blockEnd);
static short* blockTrailer(short* blockStart);
static short* nextBlockHeader(short* blockStart);
static short* previousBlockHeader(short* blockStart);
static short markBlockUsed(short* blockStart);
static short markBlockUnused(short* blockStart);
static short splitAndMarkBlockUsed(short* upperBlockStart, short desiredRoom);
static void mergeBlockWithBelow(short* upperBlockStart);
static short byteIndex(short* ptr);

//******** Heap_Init *************** 
// Initialize the Heap
// input: none
// output: always HEAP_OK
// notes: Initializes/resets the heap to a clean state where no memory
//  is allocated.
short Heap_Init(void){
  short* blockStart = HEAP_START;
  short* blockEnd = (HEAP_START + HEAP_SIZE_WORDS - 1);
  *blockStart = -(short)(HEAP_SIZE_WORDS - 2);  
  *blockEnd = -(short)(HEAP_SIZE_WORDS - 2);
  return HEAP_OK;
}


//******** Heap_Malloc *************** 
// Allocate memory, data not initialized
// input: 
//   desiredBytes: desired number of bytes to allocate
// output: void* pointing to the allocated memory or will return NULL
//   if there isn't sufficient space to satisfy allocation request
void* Heap_Malloc(short desiredBytes){
  short desiredWords = (desiredBytes + sizeof(short) - 1) / sizeof(short);
  short* blockStart = HEAP_START;
  if(desiredWords <= 0){
    return 0; //NULL
  }
  while(inHeapRange(blockStart)){
    if(blockUnused(blockStart) && desiredWords <= blockRoom(blockStart)){
      if(splitAndMarkBlockUsed(blockStart, desiredWords)){
        return 0; //NULL
      }
      return blockStart + 1;
    }
    blockStart = nextBlockHeader(blockStart);
  }
  return 0; //NULL
}


//******** Heap_Calloc *************** 
// Allocate memory, data are initialized to 0
// input:
//   desiredBytes: desired number of bytes to allocate
// output: void* pointing to the allocated memory block or will return NULL
//   if there isn't sufficient space to satisfy allocation request
//notes: the allocated memory block will be zeroed out
void* Heap_Calloc(short desiredBytes){  
  short* blockPtr;
  short wordsToClear;
  short i;
  
  //malloc a block
  blockPtr = Heap_Malloc(desiredBytes);
  //did malloc fail?
  if(blockPtr == 0){
    return 0; //NULL
  }
  wordsToClear = *(blockPtr - 1); //get room from header
  //clear out block
  for(i = 0; i < wordsToClear; i++){
    blockPtr[i] = 0;
  }
  return blockPtr;
}


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
void* Heap_Realloc(void* oldBlock, short desiredBytes){
  short* oldBlockPtr;
  short* oldBlockStart;
  short* newBlockPtr;
  short oldBlockRoom;
  short newBlockRoom;
  short wordsToCopy;
  short i;
  
  oldBlockPtr = (short*) oldBlock;
  // error if...
  // 1) oldBlockPtr doesn't point in the heap
  // 2) oldBlockPtr points to an unused block
  oldBlockStart = oldBlockPtr - 1;
  if(!inHeapRange(oldBlockStart) || blockUnused(oldBlockStart)){
    return 0; // NULL
  }

  newBlockPtr = Heap_Malloc(desiredBytes);
  // did Malloc fail?
  if(newBlockPtr == 0){
    return 0; // NULL
  }
  
  oldBlockRoom = blockRoom(oldBlockStart);
  newBlockRoom = blockRoom(newBlockPtr - 1);
  if(oldBlockRoom < newBlockRoom){
    wordsToCopy = oldBlockRoom;
  }
  else{
    wordsToCopy = newBlockRoom;
  }  
  for(i = 0; i < wordsToCopy; i++){
    newBlockPtr[i] = oldBlockPtr[i];
  }
  if(Heap_Free(oldBlockPtr)){
    return 0; // NULL Free failed
  }
  return newBlockPtr;
}


//******** Heap_Free *************** 
// return a block to the heap
// input: pointer to memory to unallocate
// output: HEAP_OK if everything is ok;
//  HEAP_ERROR_POINTER_OUT_OF_RANGE if pointer points outside the heap;
//  HEAP_ERROR_CORRUPTED_HEAP if heap has been corrupted or trying to
//  unallocate memory that has already been unallocated;
short Heap_Free(void* pointer){
  short* blockStart;
  short* blockEnd;
  short* nextBlockStart;
  
  blockStart = ((short*)pointer) - 1;

  //-----Begin error checking-------
  if(!inHeapRange(blockStart)){
    return HEAP_ERROR_POINTER_OUT_OF_RANGE;
  }
  if(blockUnused(blockStart)){
    return HEAP_ERROR_CORRUPTED_HEAP;
  }
  blockEnd = blockTrailer(blockStart);
  if(!inHeapRange(blockEnd) || blockUnused(blockEnd)){
    return HEAP_ERROR_CORRUPTED_HEAP;
  }
  //-----End error checking-------

  if(markBlockUnused(blockStart)){
    return HEAP_ERROR_CORRUPTED_HEAP;
  }

  // time to possibly merge with block above
  // first, make sure there IS a block above us
  if(blockStart > HEAP_START){ 
    short* previousBlockStart = previousBlockHeader(blockStart);
    // second, make sure we only merge with an unused block
    if(blockUnused(previousBlockStart)){
      mergeBlockWithBelow(previousBlockStart);
      blockStart = previousBlockStart; // start of block has moved
    }
  }

  // possibly merge with block below
  nextBlockStart = nextBlockHeader(blockStart);
  if(inHeapRange(nextBlockStart) && blockUnused(nextBlockStart)){
    mergeBlockWithBelow(blockStart);
  }
  return HEAP_OK;
}


//******** Heap_Test *************** 
// Test the heap
// input: none
// output: validity of the heap - either HEAP_OK or HEAP_ERROR_HEAP_CORRUPTED
short Heap_Test(void){
  short lastBlockWasUnused = 0;
  short* blockStart = HEAP_START;
  while(inHeapRange(blockStart)){
    short* blockEnd;
    
    //shouldn't have any blocks holding zero words
    if(*blockStart == 0){
      return HEAP_ERROR_CORRUPTED_HEAP;
    }
    blockEnd = blockTrailer(blockStart);
    //error if blockEnd is not in the heap or blockend disagrees with blockStart
    if(!inHeapRange(blockEnd) || *blockStart != *blockEnd){
      return HEAP_ERROR_CORRUPTED_HEAP;
    }
    //error if we have two adjacent unused blocks
    if(lastBlockWasUnused && blockUnused(blockStart)){
      return HEAP_ERROR_CORRUPTED_HEAP;
    }
    lastBlockWasUnused = blockUnused(blockStart);
    blockStart = blockEnd + 1;
  }
  //traversing the heap should end exactly where the heap ends
  if(blockStart != HEAP_END){
    return HEAP_ERROR_CORRUPTED_HEAP;
  }
  return HEAP_OK;
}


//******** Heap_Stats *************** 
// return the current status of the heap
// input: none
// output: a heap_stats_t that describes the current usage of the heap
heap_stats_t Heap_Stats(void){
  short* blockStart;
  heap_stats_t stats;
  
  stats.wordsAllocated = 0;
  stats.wordsAvailable = 0;
  stats.blocksUsed = 0;
  stats.blocksUnused = 0;

  //just go through each block to get stats on heap usage
  blockStart = HEAP_START;
  while(inHeapRange(blockStart)){
    if(blockUsed(blockStart)){
      stats.wordsAllocated += blockRoom(blockStart);
      stats.blocksUsed++;
    }
    else{
      stats.wordsAvailable += blockRoom(blockStart);
      stats.blocksUnused++;
    }
    blockStart = nextBlockHeader(blockStart);
  }
  stats.wordsOverhead = HEAP_SIZE_WORDS - stats.wordsAllocated - stats.wordsAvailable;
  return stats;
}


// inHeapRange
// input: a pointer
// output: whether or not the pointer points inside the heap
static short inHeapRange(short* address){
  return address >= HEAP_START && address < HEAP_END;
}


// blockUsed
// input: pointer to the header or trailer of a block
// output: whether or not the block is marked as used/allocated
static short blockUsed(short* block){
  return *block > 0;
}


// blockUnused
// input: pointer to the header or trailer of a block
// output: whether or not the block is marked as unused/unallocated
static short blockUnused(short* block){
  return *block < 0;
}


// blockRoom
// input: pointer to the header or trailer of a block
// output: how many words of data the block can hold
static short blockRoom(short* block){
  if(*block > 0){
    return *block;
  }
  return -*block;
}


// blockSize
// input: pointer to the header or trailer of a block
// output: the size of a block in words, including header and trailer
static short blockSize(short* block){
  if(*block > 0){
    return *block + 2;
  }
  return -*block + 2;
}


// blockHeader
// input: pointer to the trailer of a block
// output: pointer to the header of the same block
static short* blockHeader(short* blockEnd){
  return blockEnd - blockRoom(blockEnd) - 1;
}


// blockTrailer
// input: pointer to the header of a block
// output: pointer to the trailer of the same block
static short* blockTrailer(short* blockStart){
  return blockStart + blockRoom(blockStart) + 1;
}


// nextBlockHeader
// input: pointer to the header of a block
// output: pointer the the header of the next block in the heap
// notes: given the header of the last block in the heap, will point to HEAP_END,
//   which is not a valid block; be careful
static short* nextBlockHeader(short* blockStart){
  return blockTrailer(blockStart) + 1;
}


// previousBlockHeader
// input: pointer to the header of a block
// output: pointer the the header of the previous block in the heap
// notes: given the header of the first block in the heap, this function
//   will go crazy and return a proportionally crazy address!
static short* previousBlockHeader(short* blockStart){
  return blockHeader(blockStart - 1);
}


// markBlockUsed
// input: pointer to the header of a block
// output: a heap flag - HEAP_OK if everything is ok or HEAP_ERROR_CORRUPTEDHEAP
//   if there is something obviously wrong with the block
//notes: marks the block as used/allocated
static short markBlockUsed(short* blockStart){
  short* blockEnd = blockTrailer(blockStart);
  if(blockUsed(blockStart) || *blockStart != *blockEnd){
    return HEAP_ERROR_CORRUPTED_HEAP;
  }
  *blockStart = -*blockStart;
  *blockEnd = -*blockEnd;
  return HEAP_OK;
}


// markBlockUnused
// input: pointer to the header of a block
// output: a heap flag - HEAP_OK if everything is ok or HEAP_ERROR_CORRUPTEDHEAP
//  if there is something obviously wrong with the block
// notes: marks the block as unused/unallocated
static short markBlockUnused(short* blockStart){
  short* blockEnd = blockTrailer(blockStart);
  if(blockUnused(blockStart) || *blockStart != *blockEnd){
    return HEAP_ERROR_CORRUPTED_HEAP;
  }
  *blockStart = -*blockStart;
  *blockEnd = -*blockEnd;
  return HEAP_OK;
}


// splitAndMarkBlockUsed
// input: 
//  uppterBlockStart: header of a block 
//  desiredRoom: desired amount of words to be in the new upper block
// output: none
// notes: splits the block given so that the new upper block holds desiredRoom
//  words (or more).  Marks the upper block as used, lower block as unused.
//  Will not split a block if the leftover room is insufficient to make another
//  useful block.
static short splitAndMarkBlockUsed(short* upperBlockStart, short desiredRoom){
  short leftoverRoom = blockRoom(upperBlockStart) - desiredRoom - 2;
  // only split block if leftovers could actually make another useful block
  if(leftoverRoom > 0){
    short* upperBlockEnd = upperBlockStart + desiredRoom + 1;
    short* lowerBlockStart = upperBlockEnd + 1;
    short* lowerBlockEnd = blockTrailer(upperBlockStart);
    *upperBlockStart = desiredRoom; // marked used
    *upperBlockEnd = desiredRoom;
    *lowerBlockStart = -leftoverRoom; // marked unused
    *lowerBlockEnd = -leftoverRoom;
  }
  // can't split block - just mark it at used
  else{
    if(markBlockUsed(upperBlockStart)){
      return 0; // NULL Free failed
    }
  }
  return HEAP_OK;
}


// mergeBlockWithBelow
// input: pointer to the header of a block
// output: none
// notes: will merge the given block with the block below it.
//  WARNING: Does not check that the block below actually exists.
static void mergeBlockWithBelow(short* upperBlockStart){
  short* upperBlockEnd = blockTrailer(upperBlockStart);
  short* lowerBlockStart = upperBlockEnd + 1;
  short* lowerBlockEnd = blockTrailer(lowerBlockStart);

  short room = lowerBlockEnd - upperBlockStart - 1;
  *upperBlockStart = -room;
  *lowerBlockEnd = -room;
  return;
}


