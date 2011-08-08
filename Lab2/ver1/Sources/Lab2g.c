// filename ******** Lab2g.C ************** 
// output compare interrupt program for the 9S12DP512, Lab2g
// Starter project for EE445L Lab2g
// Jonathan W. Valvano 8/29/10

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2010 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 
  
#include <hidef.h>           /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"


#include "pll.h"
#include "Fifo.h"

// background variables
dataType BackData;           // set in the background
unsigned short NumLost;      // number of data points lost, FIFO full
unsigned short BackPeriod;   // variable interrupt period
unsigned long NumInterrupts;

// foreground variables
dataType ForeData;          // set in the foreground
dataType ForeExpected;      // expected data
unsigned short Errors;      // number of out-of-sequence points 
  
//---------OC0_Init--------------------------
// start output compare interrupt
// 10kHz making the TCNT timer 24 MHz 
// Inputs: none
// Ouputs: none
void OC0_Init(void){
  asm sei         // Make ritual atomic
  TIOS  |= 0x01;  // activate TC0 as output compare
  TSCR1 = 0x80;   // Enable TCNT, 24MHz assumming PLL is active
  TSCR2 = 0x00;   // divide by 1 TCNT prescale, TOI disarm
  PACTL = 0;      // timer prescale used for TCNT
/* Bottom three bits of TSCR2 (PR2,PR1,PR0) determine TCNT period
    divide  FastMode(24MHz)    Slow Mode (8MHz)
000   1     42ns  TOF  2.73ms  125ns TOF 8.192ms
001   2     84ns  TOF  5.46ms  250ns TOF 16.384ms 
010   4    167ns  TOF  10.9ms  500ns TOF 32.768ms   
011   8    333ns  TOF  21.8ms 	 1us TOF 65.536ms
100  16    667ns  TOF  43.7ms 	 2us TOF 131.072ms
101  32   1.33us  TOF  87.4ms		 4us TOF 262.144ns
110  64   2.67us  TOF 174.8ms    8us TOF 524.288ms
111 128   5.33us  TOF 349.5ms   16us TOF 1.048576s */ 
  TIE  |= 0x01;     // arm OC0
  TC0   = TCNT+50;  // first interrupt right away
  BackData = 0;   
  BackPeriod = 200;  // 8us
  NumLost = 0;
  NumInterrupts = 0;
  asm cli
}

//------------Debug_Profile------------
// saves place and time
// inputs: 8-bit number identifying where the software is executing
// outputs: none
unsigned short timeBuf[100];
unsigned short placeBuf[100];
//unsigned short Debug_n=0;
void Debug_Profile(unsigned short thePlace){
  if(Debug_n>99) return;
  timeBuf[Debug_n] = TCNT;        // record current time
  placeBuf[Debug_n] = thePlace;   // record place from which it is called
  Debug_n++;
}

unsigned short Delay,First;
void main(void){  	 
  PLL_Init();         // running at 24MHz
  DDRT |= 0x1F;       // debugging outputs
  PTT &= ~0x1F;
  Debug_Profile(0);   // 0 means initialization phase
  Fifo_Init();        // Initialize fifo
  OC0_Init();         // variable rate interrupt
  ForeExpected = 0;   // expected data
  for(;;){
    Debug_Profile(1); // 1 means start of foreground waiting 
    PTT_PTT1 = 0;     // falling edge of PT1 means start of foreground waiting
    while(Fifo_Get(&ForeData)==FIFOFAIL){
    }
    Debug_Profile(2); // 2 means foreground has new data
    PTT_PTT1 = 1;     // rising edge of PT1 means start of foreground processing
    if(ForeExpected != ForeData){
      Errors++;
      PTT ^= 0x10;                  // critical section found
      ForeExpected = ForeData+1; // resych to lost/bad data
    }  
    else{
      ForeExpected++;  // sequence is 0,1,2,3,...,254,255,0,1,...
    }
    if((ForeData%10)==0){
      Debug_Profile(3); // 3 means foreground has 10th data
   }
   }
}          
interrupt 8 void OC0Han(void){ // periodic interrupt
  Debug_Profile(4);  // 4 means background thread active
  PTT_PTT0 = 1;      // rising edge of PT0 means start of interrupt
  TFLG1 = 0x01;      // acknowledge OC0
  TC0 = TC0 +BackPeriod;   // varies from 10us to 1ms
  if(Fifo_Put(BackData)==FIFOFAIL){
    NumLost++;
  }
  BackData++; // sequence is 0,1,2,3,...,254,255,0,1,...
  if(BackPeriod > 500){
    BackPeriod = 200;
  } else{
    BackPeriod = BackPeriod+23;
  }
  NumInterrupts++;
  PTT_PTT0 = 0;	    // falling edge of PT0 means end of interrupt
}

// main program for Procedure part D
void main2(void){
dataType data;
  DDRT |= 0x01;       
  Fifo_Init();    // initialize 
  TSCR1 = 0x80;   // Enable TCNT, 24MHz assumming PLL is active
  First = TCNT;
  Delay = TCNT-First;
asm cli     
  for(;;){
    Fifo_Put(1);
    PTT_PTT0 = 1;     
    Fifo_Get(&data);
    PTT_PTT0 = 0;   
  }
}
