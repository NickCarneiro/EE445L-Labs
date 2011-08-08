// filename *************Fifo.c********
// Two-Index implementation of a FIFO
// Jonathan W. Valvano 8/29/10

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2010 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

#include "Fifo.h"

#include <hidef.h>           /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"

// set Good equal to 0 to use a FIFO with a critical section
// set Good equal to 1 to use a FIFO without a critical section
#define Good 0
dataType static Fifo[FIFOSIZE];

#if Good 

unsigned short volatile PutI;  // put next
unsigned short volatile GetI;  // get next

// ******* Fifo_Init ********
// Initialize FIFO, making it empty
// This function must be called before FIFO is used,
// Can be called again to empty FIFO
// Inputs: none
// Outputs: none
void Fifo_Init(void){ unsigned char SaveCCR;
asm  tpa  
asm  staa SaveCCR 
asm  sei          // make atomic 
  PutI = GetI = 0;      // Empty
asm  ldaa SaveCCR
asm  tap          // end critical section 
}

// ******* Fifo_Put ********
// Put data into FIFO
// Inputs: data to save in FIFO
// Outputs: FIFOSUCCESS if saved
//          FIFOFAIL if not saved, because FIFO was full
int Fifo_Put(dataType data){
  PTT_PTT3 = 1;
  if((PutI-GetI) & ~(FIFOSIZE-1)){
    return(FIFOFAIL); // Failed, fifo full
  }
  Fifo[PutI&(FIFOSIZE-1)] = data; // put
  PutI++;  // Success, update
  PTT_PTT3 = 0;
  return(FIFOSUCCESS);
}


// ******* Fifo_Get ********
// Get data from FIFO
// Inputs: pointer to memory into which to return data
// Outputs: FIFOSUCCESS if data properly removed
//          FIFOFAIL if not removed, because FIFO was empty
int Fifo_Get(dataType *datapt){
  PTT_PTT2 = 1;
  if(PutI == GetI ){
    return(FIFOFAIL); // Empty if PutI=GetI
  }
  *datapt = Fifo[GetI&(FIFOSIZE-1)];
  GetI++;  // Success, update
  PTT_PTT2 = 0;
  return(FIFOSUCCESS);
}

// ******* Fifo_Size ********
// Check the status of the FIFO
// Inputs: none
// Outputs: number of elements currently stored
// 0 to FIFOSIZE (0 means empty, FIFOSIZE means full)
unsigned short Fifo_Size(void){
 return ((unsigned short)(PutI-GetI));
}


#else
//***************Second Implementation with critical section****
unsigned short static volatile Size;   /* number of elements in FIFO */
dataType static volatile *PutPt;    /* Pointer of where to put next */
dataType static volatile *GetPt;    /* Pointer of where to get next */

/*-----------------------Fifo_Init----------------------------
  Initialize fifo to be empty
  Inputs: none
  Outputs: none */
void Fifo_Init(void){
unsigned char SaveCCR;
asm  tpa  
asm  staa SaveCCR 
asm  sei          // make atomic 
  PutPt=GetPt=&Fifo[0];   // Empty when PutPt=GetPt
  Size = 0; 
asm  ldaa SaveCCR
asm  tap          // end critical section 
}


/*-----------------------Fifo_Put----------------------------
  Enter one character into the fifo
  Inputs: data
  Outputs: true if data is properly saved,
           false if data not saved because it was previously full*/
int Fifo_Put(dataType data){
  PTT_PTT3 = 1;
  if(Size==FIFOSIZE){
    return 0;     // Failed, fifo was previously full
  }
  *(PutPt) = data;      // try to Put data into fifo 
  PutPt++;              
  if(PutPt == &Fifo[FIFOSIZE]){ // need to wrap?
    PutPt = &Fifo[0];
  }
  Size++;   // one more element
  PTT_PTT3 = 0;
  return(1);
}

/*-----------------------Fifo_Get----------------------------
  Remove one character from the fifo
  Inputs: pointer to place to return data
  Outputs: true if data is valid, 
           false if fifo was empty at the time of the call*/
int Fifo_Get(dataType *datapt){ 
  PTT_PTT2 = 1;
  if(Size == 0){
    return(0);     // Empty if no elements in FIFO 
  }
  *datapt = *(GetPt);  // return by reference
  GetPt++;             // removes data from fifo
  if(GetPt == &Fifo[FIFOSIZE]){ 
    GetPt = &Fifo[0];  // wrap
  }
  Size--;   // one less element
  PTT_PTT2 = 0;
  return(1); 
}

//-----------------------Fifo_Size----------------------------
// Check the status of the FIFO
// Inputs: none
// Outputs: number of elements currently stored
// 0 to FIFOSIZE (0 means empty, FIFOSIZE-1 means full)
unsigned short Fifo_Size(void){ 
  return Size;
}
#endif