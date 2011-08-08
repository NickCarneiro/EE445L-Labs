// filename ************* RxFifob.h ********
// Header file for the receive FIFO
// Jonathan W. Valvano 10/25/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

#define RXFIFOSIZEb 20
/* Number of characters in the Fifo
     the FIFO is full when it has FifoSize-1 characters */
void RxFifob_Init(void);

/*-----------------------RxFifob_Put----------------------------
  Enter one character into the fifo
  Inputs: 8-bit data
  Outputs: true if data is properly saved
  Since this is called by interrupt handlers no sei,cli*/
int RxFifob_Put(unsigned char data); 

/*-----------------------RxFifob_Get----------------------------
  Remove one character from the fifo
  Inputs: pointer to place to save 8-bit data
  Outputs: true if data is valid */
int RxFifob_Get(unsigned char *datapt);

/*-----------------------RxFifob_Status----------------------------
  Check the status of the fifo
  Inputs: none
  Outputs: true if there is any data in the fifo */
int RxFifob_Status(void); 

