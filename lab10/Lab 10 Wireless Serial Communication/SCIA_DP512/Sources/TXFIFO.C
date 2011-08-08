// filename ************ TxFIFO.C ***************
// Pointer implementation of a FIFO  
// Jonathan W. Valvano 9/1/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 


#include "TxFifo.h"  
   
char static volatile *TxPutPt;    /* Pointer of where to put next */
char static volatile *TxGetPt;    /* Pointer of where to get next */
                  /* FIFO is empty if PutPt=GetPt */
                  /* FIFO is full  if PutPt+1=GetPt */
char static TxFifo[TXFIFOSIZE];     /* The statically allocated fifo data */


  
/*-----------------------TxFifo_Init----------------------------
  Initialize fifo to be empty
  Inputs: none
  Outputs: none */
void TxFifo_Init(void) {unsigned char SaveCCR;
/* save previous interrupt enable */
asm  tpa  
asm  staa SaveCCR 
asm  sei          /* make atomic */
  TxPutPt=TxGetPt=&TxFifo[0];   /* Empty when TxPutPt==TxGetPt */
asm  ldaa SaveCCR
asm  tap          /* end critical section */
}

/*-----------------------TxFifo_Put----------------------------
  Enter one character into the fifo
  Inputs: 8 bit data
  Outputs: true if data is properly saved */
int TxFifo_Put (char data) {  
char volatile *tempPt;
  tempPt = TxPutPt;
  tempPt++;
  if(tempPt == &TxFifo[TXFIFOSIZE]){   /* need to wrap?*/
    tempPt = &TxFifo[0];
  }
  if(tempPt == TxGetPt){
    return(0);             /* Failed, fifo was full */
  }
  else{
    *(TxPutPt) = data;     /* Put data into fifo */
    TxPutPt = tempPt;      /* Success, so update pointer */
    return(1);
  }
}

/*-----------------------TxFifo_Get----------------------------
  Remove one character from the fifo
  Inputs: pointer to place to save 8 bit data
  Outputs: true if data is valid */
int TxFifo_Get(char *datapt){
  if(TxPutPt == TxGetPt ){
    return(0);     /* Empty if PutPt=GetPt */
  }
  else {
    *datapt = *(TxGetPt);
    TxGetPt++;
    if(TxGetPt == &TxFifo[TXFIFOSIZE]){ 
      TxGetPt = &TxFifo[0];
    }
    return(1); 
  }
}

/*-----------------------TxFifo_Status----------------------------
  Check the status of the fifo
  Inputs: none
  Outputs: true if there is room in the FIFO */
int TxFifo_Status(void) { 
char volatile *nextPt;
  nextPt = TxPutPt;
  nextPt++;                         /* next pointer */
  if(nextPt == &TxFifo[TXFIFOSIZE]){ /* need to wrap?*/
    nextPt = &TxFifo[0];
  }
  return (nextPt != TxGetPt);
}

/*-----------------------TxFifo_Size----------------------------
  Check the status of the fifo
  Inputs: none
  Outputs: number of elements currently stored in the FIFO */
unsigned short TxFifo_Size(void) { 
unsigned short size;
  if(TxPutPt>=TxGetPt){
    size = TxPutPt-TxGetPt;
  } else{
    size = TXFIFOSIZE+TxPutPt-TxGetPt;
  }
  return size;
}