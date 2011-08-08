// filename *************************heap_test.c ************************
// Test for memory heap for dynamic memory allocation.

// Jacob Egner 2008-07-31
// modified 8/31/08 Jonathan Valvano for style

// You may use, edit, run or distribute this file 
//    as long as the above notices remain
#include <hidef.h>      /* common defines and macros */
#include <mc9s12c32.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12c32"


#include "heap.h"

// global so easier to see with the debugger
// Proper style would be to make these variables local to main.
short status; 
short* ptr;
short* p1;
short* p2;
short* p3;
unsigned char* q1;
unsigned char* q2;
unsigned char* q3;
unsigned char* q4;
unsigned char* q5;
unsigned char* q6;
short maxBlockSize;
unsigned char* bigBlock;
heap_stats_t stats;

void main(){
  
  short i;
  

  status = Heap_Init();

  ptr = Heap_Malloc(sizeof(short));
  *ptr = 0x1111;
  status = Heap_Test();

  status = Heap_Free(ptr);
  status = Heap_Test();

  ptr = Heap_Malloc(1);
  status = Heap_Test();

  status = Heap_Free(ptr);
  status = Heap_Test();

  p1 = (short*) Heap_Malloc(1 * sizeof(short));
  p2 = (short*) Heap_Malloc(2 * sizeof(short));
  p3 = (short*) Heap_Malloc(3 * sizeof(short));
  p1[0] = 0xAAAA;
  p2[0] = 0xBBBB;
  p2[1] = 0xBBBB;
  p3[0] = 0xCCCC;
  p3[1] = 0xCCCC;
  p3[2] = 0xCCCC;
  status = Heap_Test();
  stats = Heap_Stats();

  status = Heap_Free(p1);
  status = Heap_Free(p3);
  status = Heap_Test();

  status = Heap_Free(p2);
  status = Heap_Test();
  stats = Heap_Stats();

  for(i = 0; i <= HEAP_SIZE_WORDS; i++){
    ptr = Heap_Malloc(sizeof(short));
  }
  status = Heap_Test();
  stats = Heap_Stats();

  status = Heap_Init();
  q1 = Heap_Malloc(1);
  q2 = Heap_Malloc(2);
  q3 = Heap_Malloc(3);
  q4 = Heap_Malloc(4);
  q5 = Heap_Malloc(5);
  status = Heap_Test();

  *q1 = 0xDD;
  q6 = Heap_Realloc(q1, 6);
  status = Heap_Test();
  stats = Heap_Stats();

  //q1 should point to freed space
  status = Heap_Free(q1);
  status = Heap_Test();

  for(i = 0; i < 6; i++){
    q6[i] = 0xEE;
  }
  q1 = Heap_Realloc(q6, 2);
  status = Heap_Test();

  status = Heap_Init();
  maxBlockSize = HEAP_SIZE_BYTES - 2 * sizeof(short);
  bigBlock = Heap_Malloc(maxBlockSize);
  for(i = 0; i < maxBlockSize; i++){
    bigBlock[i] = 0xFF;
  }
  status = Heap_Test();
  stats = Heap_Stats();
  status = Heap_Free(bigBlock);
  bigBlock = Heap_Calloc(maxBlockSize);
  status = Heap_Test();
  stats = Heap_Stats();
  for(;;){
  } 
}

