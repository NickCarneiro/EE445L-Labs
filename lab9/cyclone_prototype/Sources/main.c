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
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include "light_ring.h"
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"

#define ON_PERIOD 9500
#define WIN_PERIOD 50000
#define WIN_LED 4

void main2(void);
void PLL_Init(void);

unsigned short main_loc = 0;             //main lcd location
unsigned short interrupt_loc = 0;        //interrupt lcd location
unsigned short hour = 12, minute = 0, second = 0, mili_sec_count = 0;   //time counters
unsigned short al_minute = 0, al_hour = 12, alarm_set = 0, secret_alarm = 0;    //alarm time counters
unsigned short timeNow = 0;

short off_index = -1; 	//initialized to -1 since all lights are off
unsigned short on_index = 0;
void	(*lightRing[6])(void) = 
{
	ToggleLED_top,
	ToggleLED_b,
	ToggleLED_c,
	ToggleLED_d,
	ToggleLED_e,
	ToggleLED_f,
};


interrupt 8 void TOC0handler(void){ // executes at 100 Hz 
  TFLG1 = 0x01;         // acknowledge OC0
  TC0 = TC0 + ON_PERIOD;      // 10 ms
  //on_index turns new light on
  //off_index trails and turns off the light previously on
  if (off_index >= 0){
	  (*lightRing[off_index])();
  }
  (*lightRing[on_index])();
  off_index= (off_index+ 1) % 6;
  on_index = (on_index + 1) % 6;
}

//---------------------OC_Init0---------------------
// arm output compare 0 for 100Hz periodic interrupt
// Input: none
// Output: none 
void OC_Init0(){
  TIOS |= 0x01;   // activate TC0 as output compare
  TIE  |= 0x01;   // arm OC0    to be armed conditionally
/* Bottom three bits of TSCR2 (PR2,PR1,PR0) set TCNT period
    divide  FastMode(24MHz)    Slow Mode (8MHz)
000   1     42ns  TOF  2.73ms  125ns TOF  8.192ms
001   2     84ns  TOF  5.46ms  250ns TOF 16.384ms 
010   4    167ns  TOF  10.9ms  500ns TOF 32.768ms     
011   8    333ns  TOF  21.8ms    1us TOF 65.536ms
100  16    667ns  TOF  43.7ms    2us TOF 131.072ms
101  32   1.33us  TOF  87.4ms    4us TOF 262.144ns
110  64   2.67us  TOF 174.8ms    8us TOF 524.288ms
111 128   5.33us  TOF 349.5ms   16us TOF 1.048576s */
  TC0    = TCNT+50; // first interrupt right away
}

//---------------------OC_Init3---------------------
// arm output compare 3 for 1000Hz periodic interrupt
// Input: none    assumes another ritual enabled the TCNT
// Output: none 
void OC_Init3(){
  TIOS |= 0x08;    // activate TC3 as output compare
  TC3   = TCNT+50; // first interrupt right away
}

interrupt 11 void TOC3handler(void){ // executes at 1000 Hz 
  TFLG1 = 0x08;         // acknowledge OC3
  
  TC3 = TC3+WIN_PERIOD;       // 1 ms
  PTT ^= 0x3F;          // this is a celebration
  LCD_Clear();
}


/*
//---------------------OC_Init2---------------------
// arm output compare 3 for 1000Hz periodic interrupt
// Input: none    assumes another ritual enabled the TCNT
// Output: none 
void OC_Init2(){
  Count3 = 0;      // debugging monitor      
  //DDRT |= 0x08;    // debugging monitor
  TIOS |= 0x04;    // activate TC2 as output compare
  TIE  |= 0x04;    // arm OC2
  TC2   = TCNT+1500; // first interrupt right away
}

interrupt 10 void TOC2handler(void){ // executes at 100 Hz 
  TFLG1 = 0x04;         // acknowledge OC0
  TC2 = TC2+1500;      // 10 ms
  //PTT ^= 0x01;          // debugging monitor
  mili_sec_count = (mili_sec_count + 1) % 500;   //count milisecs
  if (mili_sec_count == 0){
    LCD_GoTo(2,interrupt_loc);
    printf("b");
    interrupt_loc = (interrupt_loc + 1) % 8;
    LCD_GoTo(1,main_loc);
  }
  //PTT ^= 0x80;
}*/
 
void main(void){

  unsigned char won = 0;
  unsigned short jackpot = 150;
  PLL_Init();
  LCD_Init();
  OC_Init3();
  OC_Init0();
  LCD_Clear();
  DDRT |= 0x3F;
  DDRP |= 0x80;
  DDRP &= ~0x03;
  PTT &= ~0x3F;
  printf("Push to start");
  while(PTP_PTP1 != 1); //wait for game to start
  LCD_Clear();
  printf("%u", jackpot);
  asm cli
  
  while(1){
    if (PTP_PTP0 == 1){
      TIE  &= ~0x01;   
      if (off_index == 4){
      on_index = 0;
      off_index = -1;
       TIE |= 0x08; 
       jackpot = 150;
       won = 1; 
      }
      while(PTP_PTP1 != 1){
      //wait for continue button to pushed
        if (won){
          printf("JACKPOT!");
        }
      }
      if (won){
        TIE &= ~0x08;
        PTT &= ~0x3F;
        won = 0;
      }else{
        jackpot++;
      }
      LCD_Clear();
      printf("%u", jackpot);
      TIE  |= 0x01;   // arm OC0    to be armed conditionally     
    }
  }
  return;
}