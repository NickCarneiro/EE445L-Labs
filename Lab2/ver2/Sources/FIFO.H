// filename ************* Fifo.h ********
// Two-Index header of a FIFO
// Jonathan W. Valvano 8/28/10

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2010, edit, run or distribute this file 
//    as long as the above copyright notice remains 

// Two-index implementation of the FIFO
// can hold 0 to FIFOSIZE elements
#define FIFOSIZE 16   // must be a power of 2
#define FIFOSUCCESS 1
#define FIFOFAIL    0

typedef unsigned char dataType;


// ******* Fifo_Init ********
// Initialize FIFO, making it empty
// This function must be called before FIFO is used,
// Can be called again to empty FIFO
// Inputs: none
// Outputs: none
void Fifo_Init(void);

// ******* Fifo_Put ********
// Put data into FIFO
// Inputs: data to save in FIFO
// Outputs: FIFOSUCCESS if saved
//          FIFOFAIL if not saved, because FIFO was full
int Fifo_Put(dataType data); 

// ******* Fifo_Get ********
// Get data from FIFO
// Inputs: pointer to memory into which to return data
// Outputs: FIFOSUCCESS if data properly removed
//          FIFOFAIL if not removed, because FIFO was empty
int Fifo_Get(dataType *datapt);

// ******* Fifo_Size ********
// Check the status of the FIFO
// Inputs: none
// Outputs: number of elements currently stored
// 0 to FIFOSIZE (0 means empty, FIFOSIZE means full)
unsigned short Fifo_Size(void); 


// Version 2) with debugging print
int Fifo_Get2(dataType *datapt);

// Version 3) with debugging dump
int Fifo_Get3(dataType *datapt); 
