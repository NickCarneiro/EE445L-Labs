//filename  ***************  LCD.C ****************************** 
//LCD Display (HD44780) on Port H for the 9S12DP512   
//Jonathan W. Valvano 9/18/09

// This example accompanies the books
//  "Embedded Microcomputer Systems: Real Time Interfacing",
//       Thompson, copyright (c) 2006,
//  "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//       Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

//Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//   You may use, edit, run or distribute this file 
//   as long as the above copyright notice remains 

/*   
  size is 1*16 
  if do not need to read busy, then you can tie R/W=ground 
  ground = pin 1    Vss
  power  = pin 2    Vdd   +5V
  ground = pin 3    Vlc   grounded for highest contrast
  PH4    = pin 4    RS    (1 for data, 0 for control/status)
  PH5    = pin 5    R/W   (1 for read, 0 for write)
  PH6    = pin 6    E     (enable)
  PH3    = pin 14   DB7   (4-bit data)
  PH2    = pin 13   DB6
  PH1    = pin 12   DB5
  PH0    = pin 11   DB4
16 characters are configured as 2 rows of 8
addr  00 01 02 03 04 05 06 07 40 41 42 43 44 45 46 47
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MC9S12DP512.h>
#include "fixed.h"
#include "lcd.h"

#define HUNDRED_MICROS 150
#define NINETY_MICROS 135
#define ONESIXFOUR_MS 2460
#define ONE_MS 1500
#define CR 13
#define TAB 9
#define LF 10

static unsigned short OpenFlag = 0;

//---------------------Timer_Wait---------------------
//time delay
//Input: time in 0.667usec increments
//Output: none
static void Timer_Wait(unsigned short delay){ 
unsigned short startTime;
  startTime = TCNT;
  while((TCNT-startTime) <= delay){}  
}
//---------------------Timer_Wait_1ms---------------------
//time delay
//Input: time in msec increments
//Output: none
static void Timer_Wait_1ms(unsigned short msec){ 
  for(;msec;msec--){
    Timer_Wait(ONE_MS);    //1ms wait
  }
}
  
//---------------------outCsrNibble--------------------- 
//execute a 4-bit lcd command
//Input: command contains the 4-bit lcd command   
//Output: none
static void outCsrNibble(unsigned char command){
  PTH = (PTH&0x80)+(command&0x0F);    //nibble, E=0, RS=0
  PTH |= 0x40;             //E goes 0->1
  asm nop
  asm nop                  //5 cycles wide = 208ns
  PTH &= ~0x40;            //E goes 1->0
}

//---------------------outCsr--------------------- 
//executes an 8-bit lcd command via two intervalled calls to outCsrNibble
//Input: command contains the 8-bit lcd command   
//Output: none
static void outCsr(unsigned char command){
  outCsrNibble(command>>4);   //ms nibble, E=0, RS=0
  outCsrNibble(command);      //ls nibble, E=0, RS=0
  Timer_Wait(NINETY_MICROS);  //blind cycle 90 us wait
}

//---------------------LCD_Clear---------------------
//clear the LCD display, send cursor to home
//Input: none
//Output: true if successful
unsigned short LCD_Clear(void){
  if(OpenFlag==0){
    return 0;  //not open
  }
  letter_cnt = 0;   //zero letters after clear
  outCsr(0x01);        //Clear Display
  Timer_Wait(ONESIXFOUR_MS);  //1.64ms wait
  outCsr(0x02);        //Cursor to home
  Timer_Wait(ONESIXFOUR_MS);  //1.64ms wait
  return 1;		         //success
}
#define LCDINC 2
#define LCDDEC 0
#define LCDSHIFT 1
#define LCDNOSHIFT 0
#define LCDCURSOR 2
#define LCDNOCURSOR 0
#define LCDBLINK 1
#define LCDNOBLINK 0
#define LCDSCROLL 8
#define LCDNOSCROLL 0
#define LCDLEFT 0
#define LCDRIGHT 4
#define LCD2LINE 8
#define LCD1LINE 0
#define LCD10DOT 4
#define LCD7DOT 0

//---------------------LCD_Init---------------------
//initialize the LCD display, called once at beginning
//Input: none
//Output: true if successful
unsigned short LCD_Init(void){
  if(OpenFlag){
    return 0;      //error if already open
  }
  DDRH |= 0x7F;       //PH6-0 output to LCD
  PTH &= ~0x20;       //PH5=R/W=0 means write
  TSCR1 = 0x80;       //Enable TCNT, 24MHz boot mode, 4MHz in run mode
  TSCR2 = 0x04;       //divide by 16 TCNT prescale, TCNT at 667nsec
  PACTL = 0;          //timer prescale used for TCNT
  Timer_Wait_1ms(20);        //20ms wait to allow LCD powerup
  outCsrNibble(0x03); //(DL=1 8-bit mode)
  Timer_Wait_1ms(5);          //blind cycle 5ms wait
  outCsrNibble(0x03); //(DL=1 8-bit mode)
  Timer_Wait(HUNDRED_MICROS); //blind cycle 100us wait
  outCsrNibble(0x03); //(DL=1 8-bit mode)
  Timer_Wait(HUNDRED_MICROS); //blind cycle 100us wait (not called for, but do it anyway)
  outCsrNibble(0x02); //(DL=0 4-bit mode)
  Timer_Wait(HUNDRED_MICROS); //blind cycle 100us wait
  outCsr(0x04+LCDINC+LCDNOSHIFT);        //I/D=1 Increment, S=0 no displayshift
  outCsr(0x0C+LCDNOCURSOR+LCDNOBLINK);   //D=1 displayon, C=0 cursoroff, B=0 blink off
  outCsr(0x10+LCDNOSCROLL+LCDRIGHT);     //S/C=0 cursormove, R/L=1 shiftright
  outCsr(0x20+LCD2LINE+LCD7DOT);         //DL=0 4bit, N=1 2 line, F=0 5by7 dots
  OpenFlag = 1;       //device open
  return 1;           //clear display
}

//---------------------LCD_OutChar---------------------
//sends one ASCII to the LCD display
//Input: letter is ASCII code
//Output: true if successful
unsigned short LCD_OutChar(unsigned char letter){
  if(OpenFlag==0){
    return 0;  //not open
  }
  PTH = (PTH&0x80)+(0x10+(0x0F&(letter>>4)));  //ms nibble, E=0, RS=1
  PTH |= 0x40;       //E goes 0->1
  asm nop
  asm nop            //5 cycles wide = 208ns
  PTH &= ~0x40;      //E goes 1->0
  PTH = (PTH&0x80)+(0x10+(letter&0x0F));  //ls nibble, E=0, RS=1
  PTH |= 0x40;       //E goes 0->1
  asm nop
  asm nop            //5 cycles wide = 208ns
  PTH &= ~0x40;      //E goes 1->0
  Timer_Wait(NINETY_MICROS);   //90 us wait
  return 1;	         //success
}

//---------------------LCD_OutString--------------------- 
//outputs an ASCII string to the LCD display 
//Input: string is a pointer to the beginning of the string to output   
//Output: true if successful
unsigned short LCD_OutString(char *pt){ 
  if(OpenFlag==0){
    return 0;   //not open
  }
  while(*pt){
    if(LCD_OutChar((unsigned char)*pt)==0){
      return 0; //not open for an LCD_OutChar operations
    }
    pt++;       //move to next letter
  }
  return 1;	    //success
}

//---------------------LCD_GoTo--------------------- 
//moves cursor to desired location 
//Input: string is a pointer to the beginning of the string to output   
//Output: true if successful
unsigned short LCD_GoTo(unsigned short cur_loc){
  
  if(cur_loc >= 0x08 && cur_loc <= 0x3F || cur_loc >= 0x48 && cur_loc <= 0xFF){
    return 0;
  }
  outCsr(cur_loc + 0x80);
  return 1;   
}

//---------------------TERMIO_PutChar--------------------- 
//sends one ASCII to the LCD display 
//Input: letter is ASCII code 
//handles at least two special characters, like CR LF or TAB  
//Output: true if successful
short TERMIO_PutChar(unsigned char letter){
  if(letter == 0x0A){       //if \n, clear display
    LCD_GoTo(0);
  } else if(letter == 0x09){      //if \t, output 3 spaces
    if( letter_cnt >= 8){
      LCD_GoTo(0x40);   //goto next lcd line
      letter_cnt = 0;
    }
    letter_cnt += 3;    //3 more letters
    LCD_OutChar(' ');
    LCD_OutChar(' ');
    LCD_OutChar(' ');   //tab outputs 3 spaces
  } else{
    if( letter_cnt >= 8){
      LCD_GoTo(0x40);
      letter_cnt = 0;
    }
    letter_cnt++;
    LCD_OutChar(letter); //output letter
    return 1;            //assume it worked
  }
}
