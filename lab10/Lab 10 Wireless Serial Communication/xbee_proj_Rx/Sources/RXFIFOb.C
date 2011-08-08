// filename *************RxFifob.c********
// Pointer implementation of the receive FIFO
// Jonathan W. Valvano 10/25/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

#include "RxFifob.h"
/* Number of characters in the Fifo
     the FIFO is full when it has FifoSize-1 characters */
unsigned char static volatile *RxPutPtb;    /* Pointer of where to put next */
unsigned char static volatile *RxGetPtb;    /* Pointer of where to get next */
                  /* FIFO is empty if PutPt=GetPt */
                  /* FIFO is full  if PutPt+1=GetPt */
char unsigned static RxFifob[RXFIFOSIZEb];     /* The statically allocated fifo data */
 
/*-----------------------RxFifob_Init----------------------------
  Initialize fifo to be empty
  Inputs: none
  Outputs: none */
void RxFifob_Init(void){unsigned char SaveCCR;
/* save previous interrupt enable */
asm  tpa  
asm  staa SaveCCR 
asm  sei          /* make atomic */
  RxPutPtb=RxGetPtb=&RxFifob[0];   /* Empty when PutPt=GetPt */
asm  ldaa SaveCCR
asm  tap          /* end critical section */
}

/*-----------------------RxFifob_Put----------------------------
  Enter one character into the fifo
  Inputs: 8-bit data
  Outputs: true if data is properly saved
  Since this is called by interrupt handlers no sei,cli*/
int RxFifob_Put(unsigned char data){
unsigned char volatile *tempPt;
  tempPt = RxPutPtb;
  tempPt++;              // place for next time
  if(tempPt==&RxFifob[RXFIFOSIZEb]){ // need to wrap?
    tempPt = &RxFifob[0];
  }
  if(tempPt == RxGetPtb ){
    return(0);             // Failed, fifo was full 
  }     
  else{
    (*RxPutPtb) = data;    // put
    RxPutPtb = tempPt;     // Success, so update pointer 
    return(1);
  }
}

/*-----------------------RxFifob_Get----------------------------
  Remove one character from the fifo
  Inputs: pointer to place to save 8-bit data
  Outputs: true if data is valid */
int RxFifob_Get(unsigned char *datapt){ 
  if(RxPutPtb == RxGetPtb ){
    return(0);     /* Empty if PutPt=GetPt */
  }
  else{
    *datapt = *(RxGetPtb++);
    if(RxGetPtb==&RxFifob[RXFIFOSIZEb]){ 
       RxGetPtb = &RxFifob[0];
    }
    return(1); 
  }
}

/*-----------------------RxFifbo_Status----------------------------
  Check the status of the fifo
  Inputs: none
  Outputs: true if there is any data in the fifo */
int RxFifob_Status(void){ 
  return (RxPutPtb != RxGetPtb);
}
