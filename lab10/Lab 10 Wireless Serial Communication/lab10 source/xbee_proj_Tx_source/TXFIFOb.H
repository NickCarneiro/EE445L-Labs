// filename ************ TxFifob.H ***************
// header for Pointer implementation of a FIFO  
// Jonathan W. Valvano 10/25/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 
   
#define TXFIFOSIZEb 20
// the number of 8 bit bytes in the Fifo
//   the FIFO is full when it has FifoSize-1 bytes 

/*-----------------------TxFifob_Init----------------------------
  Initialize fifo to be empty
  Inputs: none
  Outputs: none */
void TxFifob_Init(void);

/*-----------------------TxFifob_Put----------------------------
  Enter one character into the fifo
  Inputs: 8 bit data
  Outputs: true if data is properly saved */
// returns TRUE=1 if successful, 
// FALSE=0 if full and data not saved
int TxFifob_Put (unsigned char data);  

/*-----------------------TxFifob_Get----------------------------
  Remove one character from the fifo
  Inputs: pointer to place to save 8 bit data
  Outputs: true if data is valid */
// returns TRUE=1 if successful, 
// FALSE=0 if empty and data not removed
int TxFifob_Get (unsigned char *datapt);


/*-----------------------TxFifob_Status----------------------------
  Check the status of the fifo
  Inputs: none
  Outputs: true if there is room in the FIFO */
int TxFifob_Status (void);
