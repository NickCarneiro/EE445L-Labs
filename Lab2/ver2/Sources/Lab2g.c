// filename ******** Lab2g_v2.C ************** 
// output compare interrupt program for the 9S12DP512, Lab2g
// Starter project for EE445L Lab2g_v2, DP512 version
// Jonathan W. Valvano 8/28/10

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2010 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

// how to run programs that use the SCI port
// 1) connect 9S12DP512 to PC's COM port, 
//     set 9S12DP512 to LOAD mode
//     reset 9S12DP512
// 2) execute Project->Debug (compiles and downloads code to 9S12DP512)
// 3) quit MW debugger once programming complete 
//    quitting the debugger will release the COM port
// 4) start a terminal program (like HyperTerminal)
//    specify proper COM port, 115200 bits/sec, no flow control
// 5) set to RUN mode and hit reset
// 6) when done, quit terminal program
//    quitting the terminal program will release the COM port
//     set 9S12DP512 to LOAD mode
//     reset 9S12DP512

#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"

#include "pll.h"
#include "SCI0.h"
#include "Fifo.h"

//---------------------OutCRLF---------------------
// Output a CR,LF to SCI to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  SCI0_OutChar(CR);
  SCI0_OutChar(LF);
}

dataType ForeData;
void main(void){ 
unsigned short First;
unsigned short Delay = 0;	 
EnableInterrupts;
  PLL_Init();       // running at 24MHz
  DDRT = 0xff;      // debugging outputs
  PTT = 0x00;
  SCI0_Init(115200);	// fastest standard baud rate on run mode 9S12DP512
  
  SCI0_OutString("EE445L Lab2g -JWV"); OutCRLF();
  TSCR1 = 0x80;     // Enable TCNT, 24MHz assuming PLL is active
  TSCR2 = 0;
// calibration offset  
  First = TCNT;
  //nothing here
  Delay = TCNT - First - 9;
  SCI0_OutString("Time to execute nothing "); 
  SCI0_OutUDec(Delay);
  SCI0_OutString(" cycles"); OutCRLF();

// measurement version 1, no debugging instruments 
  Fifo_Init();    // Initialize fifo
  Fifo_Put(1);    // make sure there is something in the fifo
  First = TCNT;
  Fifo_Get(&ForeData);
  Delay = TCNT-First-9;
  SCI0_OutString("Time to execute Fifo_Get with no debugging instruments is "); 
  SCI0_OutUDec(Delay);
  SCI0_OutString(" cycles"); OutCRLF();

// measurement version 2, print debugging instruments 
/*  Fifo_Init();    // Initialize fifo
  Fifo_Put(1);    // make sure there is something in the fifo
  First = TCNT;
  Fifo_Get2(&ForeData);
  Delay = TCNT-First-9;
  SCI0_OutString("Time to execute Fifo_Get2 with print debugging instruments is "); 
  SCI0_OutUDec(Delay);
  SCI0_OutString(" cycles"); OutCRLF();
  
// measurement version 3, dump debugging instruments 
  Fifo_Init();    // Initialize fifo
  Fifo_Put(1);    // make sure there is something in the fifo
  First = TCNT;
  Fifo_Get3(&ForeData);
  Delay = TCNT-First-9;
  SCI0_OutString("Time to execute Fifo_Get3 with dump debugging instruments is "); 
  SCI0_OutUDec(Delay);
  SCI0_OutString(" cycles"); OutCRLF();*/  
  for(;;){}
}
