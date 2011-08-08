// filename ******** main.C ************** 
// Busy-wait SCI1 driver for the 9S12DP512,
// Jonathan W. Valvano 8/10/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

// how to run programs that use the SCI1 port
// 1) connect SCI0 on 9S12DP512 to PC's COM port, 
//    connect SCI1 on 9S12DP512 to a different COM port on PC, 
//     set 9S12DP512 to LOAD mode
//     reset 9S12DP512
// 2) execute Project->Debug (compiles and downloads code to 9S12DP512)
// 3) start a terminal program (like HyperTerminal)
//    specify proper COM port, 115200 bits/sec, no flow control
// 4) run either in debugger or set to RUN mode and hit reset

#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"
#include "derivative.h"
#include "SCI1.h"
#include "PLL.h"
#include "Xbee.h"
#include "SCIb.h"
#include "TXFIFOb.h"
#include "lcd.h"
#include "timer.h"
#include <stdio.h>

// If you want to communicate from one 9S12DP512 to another, then you can build the following 4-pin 
// cable to connect the RS232 SCI1 ports between the two 9S12 systems. 
// You would need two 4-pin female headers (All Electronics part number CON-244)
// 9S12 Computer 1                                             9S12 Computer 2
//   1 RxD ------ black ------------------------- yellow ------  3 TxD
//   2 Ground --- green ------------------------- green -------  2 Ground
//   3 TxD 	----- yellow ------------------------ black -------  1 RxD
//   4 NC                                                        4 NC

// If you wish to connect SCI1 to a personal computer, you could build this cable. 
// You would need a 4-pin female header (All Electronics part number CON-244) and 
// a 9-pin female DP9 header (All Electronic part number DB-9S). 
// Three wires must be connected in the cable, they are:
// 9S12 J2                                           COM port on a PC
//   1 RxD ------------------------------------------   3 TxD
//   2 Ground ---------------------------------------   5 Ground
//   3 TxD 	------------------------------------------  2 RxD

//---------------------OutCRLF---------------------
// Output a CR,LF to SCI to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  SCI1_OutChar(CR);
  SCI1_OutChar(LF);
  PTP_PTP7 ^= 1;
}

#define checkSum rxFrame[9]
unsigned char rxFrame[20];
//---------------------mainRx---------------------
// main used for receiver
// Input: none
// Output: none
void main(void) {  
  PLL_Init();
  SCIb_Init(9600);
  XBee_Init();
  LCD_Open();
  LCD_Clear();
  LCD_OutString("Xbee: ");
	EnableInterrupts;
  while(1) {
   // Rx DP512 continually polls the Rx FIFO and any available data is 
   // displayed on the LCD using the LCD driver routines.
    XBee_ReceiveRxFrame(&rxFrame[0]);
    /*if(SCIb_InStatus() && (rxFrame != 0)) {
      unsigned short messageLength = ((rxFrame[1]<<8) + rxFrame[2])-5;//message length only
      unsigned short i;
      for(i = 0; i < messageLength; i++){
        LCD_OutChar(rxFrame[i+8]);
      }
      printf("\n"); 
    }*/
    LCD_GoTo(1,1);
    LCD_OutString(&rxFrame[0]);
  }
}
