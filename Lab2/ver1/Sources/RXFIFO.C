// filename *************RxFifo.c********
// Pointer implementation of the receive FIFO
// Jonathan W. Valvano 8/9/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

#include "RxFifo.h"
/* Number of characters in the Fifo
     the FIFO is full when it has FifoSize-1 characters */
unsigned char static volatile *RxPutPt;    /* Pointer of where to put next */
unsigned char static volatile *RxGetPt;    /* Pointer of where to get next */
                  /* FIFO is empty if PutPt=GetPt */
                  /* FIFO is full  if PutPt+1=GetPt */
unsigned char static RxFifo[RXFIFOSIZE];     /* The statically allocated fifo data */
 
/*-----------------------RxFifo_Init----------------------------
  Initialize fifo to be empty
  Inputs: none
  Outputs: none */
void RxFifo_Init(void){
unsigned char SaveCCR;
asm  tpa  
asm  staa SaveCCR 
asm  sei          // make atomic 
  RxPutPt=RxGetPt=&RxFifo[0];   // Empty when PutPt=GetPt 
asm  ldaa SaveCCR
asm  tap          // end critical section 
}

/*-----------------------RxFifo_Put----------------------------
  Enter one character into the fifo
  Inputs: 8-bit data
  Outputs: true if data is properly saved,
           false if data not saved because it was previously full*/
int RxFifo_Put(unsigned char data){
unsigned char volatile *tempPt;
  tempPt = RxPutPt+1;
  if(tempPt == &RxFifo[RXFIFOSIZE]){ // need to wrap?
    tempPt = &RxFifo[0];
  }
  if(tempPt == RxGetPt){
    return(0);        // Failed, fifo was previously full
  }     
  else{
    *(RxPutPt) = data; // Put data into fifo 
    RxPutPt = tempPt;  // Success, so update pointer 
    return(1);
  }
}

/*-----------------------RxFifo_Get----------------------------
  Remove one character from the fifo
  Inputs: pointer to place to return 8-bit data
  Outputs: true if data is valid, 
           false if fifo was empty at the time of the call*/
int RxFifo_Get(unsigned char *datapt){ 
  if(RxPutPt == RxGetPt){
    return(0);     // Empty if PutPt=GetPt 
  }
  else{
    *datapt = *(RxGetPt);  // return by reference
    RxGetPt++;             // removes data from fifo
    if(RxGetPt == &RxFifo[RXFIFOSIZE]){ 
      RxGetPt = &RxFifo[0];  // wrap
    }
    return(1); 
  }
}

/*-----------------------RxFifo_Status----------------------------
  Check the status of the fifo
  Inputs: none
  Outputs: true if there is any data in the fifo */
int RxFifo_Status(void){ 
  return (RxPutPt != RxGetPt);
}
