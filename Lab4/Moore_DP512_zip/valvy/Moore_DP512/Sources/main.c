// filename ******** Main.C ************** 
// MC9S12DP512 Moore finite state machine example   
// TCNT increments at a fixed rate
// PT1 and PT0 are outputs to LEDs
// PH1 and PH0 are inputs from switches
// if both buttons not pushed, fast alternate LED toggle
// if PH0 button pushed, both LEDs are on
// if PH1 button pushed, both LEDs are off
// if both buttons pushed, slow together LED toggle
// Jonathan W. Valvano 10/11/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
// You may use, edit, run or distribute this file 
//    as long as the above copyright notices remain

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "pll.h"
#include "timer.h"


const struct State{
  unsigned char Out;            // Output to Port T
  unsigned short Time;          // Time in msec to wait in this state
  const struct State *Next[4];  // Next state if input=0,1,2,3
};
typedef const struct State StateType;
typedef StateType *	StatePtr;

#define SA &fsm[0]
#define SB &fsm[1]
#define SC &fsm[2]
#define SD &fsm[3]
#define SE &fsm[4]
#define SF &fsm[5]
StateType fsm[6]={
	{0x01,2,{SB,SC,SD,SE}},   // SA,SB fast alternate toggle
	{0x02,2,{SA,SC,SD,SE}},   // SB
	{0x03,1,{SA,SC,SD,SE}},   // SC both on
	{0x00,1,{SA,SC,SD,SE}},   // SD both off
	{0x00,10,{SA,SC,SD,SF}},  // SE,SF together toggle
	{0x03,10,{SA,SC,SD,SE}}   // SF
};

StatePtr Pt;  // Current State 
unsigned char Input;

//---------FSMInit------------------
// initialize FSM, clock, initial state SA
// inputs: none
// outputs: none
void FSMInit(void){
  Pt = SA;        // Initial State
  DDRT |= 0x03;    // PT1,PT0 outputs
  DDRH &= ~0x03;   // PH1,PH0 inputs
}

void main(void) {
  PLL_Init();     // 24 MHz
  Timer_Init();   // TCNT at 1.5 MHz
  FSMInit();
  asm cli
 
  for(;;) { 
    PTT = (PTT&0xFC)+Pt->Out; // Output depends on the current state
    Timer_Wait1ms(Pt->Time);  // Time to wait in this state
    Input = PTH&0x03;         // Input=0,1,2,or 3
    Pt = Pt->Next[Input];     // Next state depends on the input 
  }
}
