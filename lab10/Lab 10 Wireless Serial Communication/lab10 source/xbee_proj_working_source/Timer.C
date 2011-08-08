// File **********Timer.C***********
// Timer wait routines, 9S12DP512
// assumes PLL is active and E clock is 24 MHz
// TCNT will become 1.5MHz
// Jonathan W. Valvano 7/14/07

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//         Thomson Engineering, copyright (c) 2006,
//   "Introduction to Embedded Microcomputer Systems: 
//    Motorola 6811 and 6812 Simulation", Thomson, copyright (c) 2002

// Copyright 2007 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

#include <mc9s12dp512.h>     /* derivative information */

 

//---------------------Timer_Init---------------------
// activate TCNT at 1.5 MHz
// inputs: none
// outputs: none
void Timer_Init(void){
  asm sei      // make ritual atomic 
  TSCR1 = 0x80;   // Enable TCNT, 24MHz E clock
  TSCR2 = 0x04;   // divide by 16 TCNT prescale, TOI disarm
  PACTL = 0;      // timer prescale used for TCNT
/* Bottom three bits of TSCR2 (PR2,PR1,PR0) determine TCNT period
    divide  FastMode(24MHz)    Slow Mode (4MHz)
000   1     42ns  TOF  2.73ms  250ns TOF 16.384ms
001   2     84ns  TOF  5.46ms  500ns TOF 32.768ms 
010   4    167ns  TOF  10.9ms    1us TOF 65.536ms   
011   8    333ns  TOF  21.8ms 	 2us TOF 131.072ms
100  16    667ns  TOF  43.7ms 	 4us TOF 262.144ns
101  32   1.33us  TOF  87.4ms		 8us TOF 524.288ms
110  64   2.67us  TOF 174.8ms   16us TOF 1.048576s
111 128   5.33us  TOF 349.5ms   32us TOF 2.097152s */ 
}


//---------Timer_Wait------------------
// fixed time delay
// inputs: time to wait in 667ns cycles
// outputs: none
void Timer_Wait(unsigned short delay){ 
unsigned short startTime;
  startTime = TCNT;
  while((TCNT-startTime) <= delay){} 
}

//---------Timer_Wait1ms------------------
// fixed time delay
// inputs: time to wait in ms
// outputs: none
// 1500 cycles equals 1ms
void Timer_Wait1ms(unsigned short delay){ 
  for(;delay>0;delay--){
    Timer_Wait(1500);
  }
}

//---------Timer_Wait10ms------------------
// fixed time delay
// inputs: time to wait in 10ms
// outputs: none
// 15000 cycles equals 10ms
void Timer_Wait10ms(unsigned short delay){
  for(;delay>0;delay--){
    Timer_Wait(15000);
  }
}