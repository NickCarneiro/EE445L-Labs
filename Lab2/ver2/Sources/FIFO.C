// filename *************Fifo.c********
// Two-Index implementation of a FIFO
// Jonathan W. Valvano 8/28/10

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2010 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

#include "Fifo.h"
#include "SCI0.h"


unsigned short volatile PutI;  // put next
unsigned short volatile GetI;  // get next
dataType static Fifo[FIFOSIZE];

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
  if((PutI-GetI) & ~(FIFOSIZE-1)){
    return(FIFOFAIL); // Failed, fifo full
  }
  Fifo[PutI&(FIFOSIZE-1)] = data; // put
  PutI++;  // Success, update
  return(FIFOSUCCESS);
}


// ******* Fifo_Get ********
// Get data from FIFO
// Inputs: pointer to memory into which to return data
// Outputs: FIFOSUCCESS if data properly removed
//          FIFOFAIL if not removed, because FIFO was empty
int Fifo_Get(dataType *datapt){
  if(PutI == GetI ){
    return(FIFOFAIL); // Empty if PutI=GetI
  }
  *datapt = Fifo[GetI&(FIFOSIZE-1)];
  GetI++;  // Success, update
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

// Version 2) with debugging print 
int Fifo_Get2(dataType *datapt){
  if(PutI == GetI ){
    return(FIFOFAIL); // Empty if PutI=GetI
  }
  *datapt = Fifo[GetI&(FIFOSIZE-1)];
  SCI0_OutUHex(GetI&(FIFOSIZE-1)); 
  SCI0_OutChar(32);
  SCI0_OutUDec((unsigned short)*datapt);
  SCI0_OutChar(13); SCI0_OutChar(10);
  GetI++;  // Success, update
  return(FIFOSUCCESS);
}  

// Version 3) with debugging dump
unsigned short ptBuf[10];
dataType dataBuf[10];
unsigned short Debug_n=0;
int Fifo_Get3(dataType *datapt){
  if(PutI == GetI ){
    return(FIFOFAIL); // Empty if PutI=GetI
  }
  *datapt = Fifo[GetI&(FIFOSIZE-1)];
  if(Debug_n<10){
    ptBuf[Debug_n] =  GetI&(FIFOSIZE-1);
    dataBuf[Debug_n] = *datapt;
    Debug_n++;
  }  
  GetI++;  // Success, update
  return(FIFOSUCCESS);
}  
