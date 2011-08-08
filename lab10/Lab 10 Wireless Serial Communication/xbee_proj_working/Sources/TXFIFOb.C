// filename ************ TxFIFOb.C ***************
// Pointer implementation of a FIFO  
// Jonathan W. Valvano 10/25/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 


#include "TxFifob.h"  
   
unsigned char static volatile *TxPutPtb;    /* Pointer of where to put next */
unsigned char static volatile *TxGetPtb;    /* Pointer of where to get next */
                  /* FIFO is empty if PutPt=GetPt */
                  /* FIFO is full  if PutPt+1=GetPt */
unsigned char static TxFifob[TXFIFOSIZEb];     /* The statically allocated fifo data */

/*-----------------------TxFifob_Init----------------------------
  Initialize fifo to be empty
  Inputs: none
  Outputs: none */
void TxFifob_Init(void) {unsigned char SaveCCR;
/* save previous interrupt enable */
asm  tpa  
asm  staa SaveCCR 
asm  sei          /* make atomic */
  TxPutPtb=TxGetPtb=&TxFifob[0];   /* Empty when TxPutPt==TxGetPt */
asm  ldaa SaveCCR
asm  tap          /* end critical section */
}

/*-----------------------TxFifob_Put----------------------------
  Enter one character into the fifo
  Inputs: 8 bit data
  Outputs: true if data is properly saved */
int TxFifob_Put (unsigned char data) {  
unsigned char volatile *tempPt;
  tempPt = TxPutPtb;
  tempPt++;                  // place for next time
  if (tempPt==&TxFifob[TXFIFOSIZEb]){   // need to wrap?
    tempPt = &TxFifob[0];
  }
  if (tempPt == TxGetPtb ){
    return(0);             // Failed, fifo was full
  }
  else {
    (*TxPutPtb) = data;    // put
    TxPutPtb = tempPt;     // Success, so update pointer 
    return(1);
  }
}

/*-----------------------TxFifob_Get----------------------------
  Remove one character from the fifo
  Inputs: pointer to place to save 8 bit data
  Outputs: true if data is valid */
int TxFifob_Get(unsigned char *datapt){
  if (TxPutPtb == TxGetPtb )
    return(0);     // Empty if PutPt=GetPt 
  else {
    *datapt = *(TxGetPtb++);
    if (TxGetPtb==&TxFifob[TXFIFOSIZEb]){ 
      TxGetPtb = &TxFifob[0];
    }
    return(1); 
  }
}

/*-----------------------TxFifob_Status----------------------------
  Check the status of the fifo
  Inputs: none
  Outputs: true if there is room in the FIFO */
int TxFifob_Status (void) { 
unsigned char volatile *nextPt;
  nextPt = TxPutPtb;
  nextPt++;                         /* next pointer */
  if (nextPt==&TxFifob[TXFIFOSIZEb]){ /* need to wrap?*/
    nextPt = &TxFifob[0];
  }
  return (nextPt != TxGetPtb);
}
