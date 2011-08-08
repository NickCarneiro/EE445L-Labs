// filename ************* RxFifo.h ********
// Header file for the receive FIFO
// Jonathan W. Valvano 8/10/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

#define RXFIFOSIZE 24
/* Number of characters in the Fifo
     the FIFO is full when it has FifoSize-1 characters */
void RxFifo_Init(void);

/*-----------------------RxFifo_Put----------------------------
  Enter one character into the fifo
  Inputs: 8-bit data
  Outputs: true if data is properly saved,
           false if data not saved because it was previously full*/
int RxFifo_Put(unsigned char data); 

/*-----------------------RxFifo_Get----------------------------
  Remove one character from the fifo
  Inputs: pointer to place to return 8-bit data
  Outputs: true if data is valid, 
           false if fifo was empty at the time of the call*/
int RxFifo_Get(unsigned char *datapt);

/*-----------------------RxFifo_Status----------------------------
  Check the status of the fifo
  Inputs: none
  Outputs: true if there is any data in the fifo */
int RxFifo_Status(void); 

// Version 2) with debugging print
int RxFifo_Get2(unsigned char *datapt);

// Version 3) with debugging dump
int RxFifo_Get3(unsigned char *datapt); 
