// filename ******** main.C ************** 
// Busy-wait SCI0 driver for the 9S12DP512,
// Jonathan W. Valvano 8/10/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

// how to run programs that use the SCI port
// 1) connect SCI0 of 9S12DP512 to PC's COM port, 
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
#include "SCI0.h"
#include "PLL.h"


//---------------------OutCRLF---------------------
// Output a CR,LF to SCI to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  SCI0_OutChar(CR);
  SCI0_OutChar(LF);
  PTP_PTP7 ^= 1;
}

char string[20];  // global to assist in debugging  
unsigned short n;  
void main(void) {		
  DDRP |= 0x80;     // PortP bit 7 is output to LED
  PLL_Init();       // running at 24 MHz
  SCI0_Init(115200);	// fastest standard baud rate on run mode 9S12DP512
  asm cli
  SCI0_OutString("TechArts DP512, SCI demo 8/10/09 -JWV"); OutCRLF();
  for(;;) {
    SCI0_OutString("InString: "); 
    SCI0_InString(string,19);   
    SCI0_OutString(" OutString="); SCI0_OutString(string); OutCRLF();
    
    SCI0_OutString("InUDec: ");  n=SCI0_InUDec(); 
    SCI0_OutString(" OutUDec="); SCI0_OutUDec(n); OutCRLF();
     
    SCI0_OutString("InUHex: ");  n=SCI0_InUHex(); 
    SCI0_OutString(" OutUHex="); SCI0_OutUHex(n); OutCRLF();
  }

}
