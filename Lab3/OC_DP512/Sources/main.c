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
#include "lcd.h"
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"

#define WAVE_PERIOD 3400

void main2(void);
void PLL_Init(void);

unsigned short static volatile Count0;   //count oc0 interrupts
unsigned short main_loc = 0;             //main lcd location
unsigned short interrupt_loc = 0;        //interrupt lcd location
unsigned short hour = 12, minute = 0, second = 0, mili_sec_count = 0;   //time counters
unsigned short al_minute = 0, al_hour = 12, alarm_set = 0, secret_alarm = 0;    //alarm time counters
unsigned short timeNow = 0;


interrupt 8 void TOC0handler(void){ // executes at 100 Hz 
  TFLG1 = 0x01;         // acknowledge OC0
  Count0++;
  TC0 = TC0+WAVE_PERIOD;      // 10 ms
  PTT ^= 0x80;
}

//---------------------OC_Init0---------------------
// arm output compare 0 for 100Hz periodic interrupt
// Input: none
// Output: none 
void OC_Init0(){
  Count0 = 0;     // debugging monitor
  DDRT |= 0x01;   // debugging monitor
  TIOS |= 0x01;   // activate TC0 as output compare
  //TIE  |= 0x01;   // arm OC0    to be armed conditionally
  //TSCR1 = 0x80;   // Enable TCNT, 24MHz boot mode, 8MHz in run mode
  //TSCR2 = 0x03;   // divide by 8 TCNT prescale, TOI disarm            //timer set in lcd init
  //PACTL = 0;      // timer prescale used for TCNT
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

unsigned short static volatile Count3;
interrupt 11 void TOC3handler(void){ // executes at 1000 Hz 
  TFLG1 = 0x08;         // acknowledge OC3
  Count3++;
  TC3 = TC3+1500;       // 1 ms
  //PTT ^= 0x08;          // debugging monitor
  
  mili_sec_count = (mili_sec_count + 1) % 1000;   //count milisecs
  if (mili_sec_count == 0){
    second++;       //1000 mili_secs into seconds
    PTP ^= 0x80; // flash LED
    if ( second == 60 ){
      minute++;    //60 seconds into minutes
      second = 0;
      secret_alarm = 1;
    }
    if ( minute == 60 ){
      hour = (hour % 12) + 1;   //60 minutes into hour
      minute = 0;
    }
  }
    
}

//---------------------OC_Init3---------------------
// arm output compare 3 for 1000Hz periodic interrupt
// Input: none    assumes another ritual enabled the TCNT
// Output: none 
void OC_Init3(){
  Count3 = 0;      // debugging monitor      
  //DDRT |= 0x08;    // debugging monitor
  TIOS |= 0x08;    // activate TC3 as output compare
  TIE  |= 0x08;    // arm OC3
  TC3   = TCNT+50; // first interrupt right away
}

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
}

void main2(void) {
  PLL_Init();
  LCD_Init();
  OC_Init2();
  TIE  |= 0x04;
  LCD_Clear();
  
  DDRP |= 0x80;  // LED
  asm cli

  while(1) {
    PTP ^= 0x80; // flash LED
    printf("a");   //output a on first line
    main_loc = (main_loc + 1) % 8;  //update location on main line
    Timer_Wait_1ms(500);
    if ( main_loc == 0 )
      LCD_GoTo(1,1);    //wrap around
  } 
}

#define ALARM_STOP (PTP & 0x10)
#define TIME_CHANGE (PTP & 0x04)
#define ALARM_CHANGE (PTP & 0x08)
#define HOUR_CHANGE (PTP & 0x01)
#define MIN_CHANGE (PTP & 0x02)
#define ALARM_ARM (PTP & 0x20)          //button definitions
#define ALARM_TRIGGER ((al_hour == hour) && (al_minute == minute) && (alarm_set == 1) && (secret_alarm == 1))  //alarm condition    

 
void main(void){

  PLL_Init();
  //LCD_Init();
  //OC_Init3();
  //OC_Init0();
  //LCD_Clear();
  //DDRT |= 0xC0;
  //DDRP &= 0xE0;
  //DDRP |= 0x80;
  DDRT &= ~0x01;
  DDRP |= 0x01;
  asm cli
  while(1){
  if (PTT_PTT0 == 1){
    while(PTT_PTT0 == 1);
    PTP_PTP0 ^= 1; 
  }
  
    /*timeNow = TCNT - 9;
    printf("\n%2u:%.2u:%.2u", hour, minute, second);  //print time
    timeNow = TCNT - timeNow;
    
    if(ALARM_STOP || !ALARM_TRIGGER){
      //PTT &= ~0x80; //turn off alarm
      TIE  &= ~0x01;  //unarm interrupt, turning off the alarm
      secret_alarm = 0; //allows for alarm to be set but not go off after turned off
    }
    
    if(ALARM_ARM){
      while(ALARM_ARM);
      alarm_set = 1 - alarm_set;  //toggle alarm_set
      secret_alarm = 1;           //arm secret alarm
      PTT ^= 0x40;                //toggle LED
    }
    
    while(TIME_CHANGE){         //change the time
      printf("\n%2u:%.2u:%.2u", hour, minute, second);  //print time

      if(HOUR_CHANGE){
        while(HOUR_CHANGE);
        hour = (hour % 12) + 1;   //increment hour
      }
      
      if(MIN_CHANGE){
        while(MIN_CHANGE);
        minute = (minute + 1) % 60; //increment minute
      }
    }
    
    while(ALARM_CHANGE){        //change the alarm time
      printf("\n%2u:%.2u:%.2u", al_hour, al_minute, second);   //echo alarm time
      if(HOUR_CHANGE){
        while(HOUR_CHANGE);
        al_hour = (al_hour % 12) + 1;   //increment hour
      }
      
      if(MIN_CHANGE){
        while(MIN_CHANGE);
        al_minute = (al_minute + 1) % 60; //increment minute
      }
    }
    
    if(ALARM_TRIGGER){
      //PTT |= 0x80;       //give power to alarm speaker
      TIE  |= 0x01;   //turn on interrupt sounding the alarm
    }  */
  }
  return;
}