// filename ******** Main.C ************** 
// This example illustrates the output compare interrupts, 
// 9S12DP512
// Jonathan W. Valvano 9/7/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
// You may use, edit, run or distribute this file 
//    as long as the above copyright notices remain


#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#include "music.h"
#include "timer.h"
#include "switches.h"
#include "dac.h"
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"

void PLL_Init(void);
void main2(void);
void Debug_Profile(unsigned short thePlace);
extern unsigned char volt_change; 
extern unsigned short note_index;

//------------Debug_Profile------------
// saves place and time
// inputs: 8-bit number identifying where the software is executing
// outputs: none
unsigned short timeBuf[100];
unsigned short placeBuf[100];
unsigned static short Debug_n = 0;
//unsigned short Debug_n=0;
void Debug_Profile(unsigned short thePlace){
  if(Debug_n>99) return;
  timeBuf[Debug_n] = TCNT;        // record current time
  placeBuf[Debug_n] = thePlace;   // record place from which it is called
  Debug_n++;
}

//*******Iron Man*******
//
//
Envelope envelope [SONG_SIZE] = {
	{ONE_HALF,B}, 
	{ONE_QUARTER,D},
	{ONE_QUARTER,REST},
	{ONE_QUARTER,D},
	{ONE_EIGHTH,E},
	{ONE_EIGHTH,REST},
	{ONE_HALF,E},
	{ONE_EIGHTH,G},
	{ONE_EIGHTH,F_SHARP},
	{ONE_EIGHTH,G},
	{ONE_EIGHTH,F_SHARP},
	{ONE_EIGHTH,G},
	{ONE_EIGHTH,F_SHARP},
	{ONE_EIGHTH,D},
	{ONE_EIGHTH,REST},
	{ONE_QUARTER,D},
	{ONE_EIGHTH,E},
	{ONE_EIGHTH,REST},
	{ONE_QUARTER,E},
	{ONE_QUARTER,REST}	  
};  

//unsigned short old_note = 0;

void main(void){
  PLL_Init();
  Timer_Init();
  DAC_Init();
  asm cli
  for(;;){
    DAC_Out(0);
  }
}

void main2(void) {
  PLL_Init();
  Timer_Init();
  OC_Init3();
  OC_Init0();
  DAC_Init();
  Switches_Init(); 
  DDRP |= 0x80;
  asm cli
  Debug_Profile(0);

  while(1) {
    
    //wait for play button to be pushed to start song
    if (Get_Switches() == PLAY){
      while (1){
        //old_note = note_index;
        //if stop button is pushed exit and wait for the play button to be pushed again
        if (Get_Switches() == STOP){
          Music_Stop();
          break;
        }
        if (Get_Switches() == REWIND){
          Music_Rewind();
          continue; 
        }
        Music_Play(envelope[note_index].frequency_pitch);
      }
    }
    
    if (Get_Switches() == REWIND){
      Music_Rewind();
    }
  }
}

