#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <mc9s12c32.h>
#include "lcd.h"
//#include "timer.h"
#include "light_ring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REG_PERIOD 2000
#define HARD_PERIOD 19000
#define REG 1
#define HARD 2
#define WIN_PERIOD 30000
#define WIN_LED_ONE 5
#define WIN_LED_TWO 13

//void main2(void);
void PLL_Init(void);

/*
unsigned short main_loc = 0;             //main lcd location
unsigned short interrupt_loc = 0;        //interrupt lcd location
unsigned short hour = 12, minute = 0, second = 0, mili_sec_count = 0;   //time counters
unsigned short al_minute = 0, al_hour = 12, alarm_set = 0, secret_alarm = 0;    //alarm time counters
unsigned short timeNow = 0;
*/

short off_index = -1; 	//initialized to -1 since all lights are off
unsigned short on_index = 0;
short prev_index = -1;
unsigned short cur_index = 0;
unsigned char mode = REG;
void	(*lightRing[17])(void) = 
{
	ToggleLED_top,
	ToggleLED_b,
	ToggleLED_c,
	ToggleLED_d,
	ToggleLED_e,
	ToggleLED_f,
	ToggleLED_g,
	ToggleLED_h,
	ToggleLED_i,
	ToggleLED_j,
	ToggleLED_k,
	ToggleLED_l,
	ToggleLED_m,
	ToggleLED_o,
	ToggleLED_p,
	ToggleLED_q,
	ToggleLED_r,
};


interrupt 8 void TOC0handler(void){ // executes at 100 Hz 
  TFLG1 = 0x01;         // acknowledge OC0
  
  if (mode == REG){
    TC0 = TC0 + REG_PERIOD;      // 10 ms
  }else{
    TC0 = TC0 + HARD_PERIOD;      // 10 ms 
  }
  //on_index turns new light on
  //off_index trails and turns off the light previously on
  if (mode == REG){
    if (off_index >= 0){
  	  (*lightRing[off_index])();
    }
    (*lightRing[on_index])();
    off_index= (off_index+ 1) % 17;
    on_index = (on_index + 1) % 17;
  }else{
    if (prev_index != -1){
      (*lightRing[prev_index])(); 
    }
      cur_index = rand() % 17;    
      (*lightRing[cur_index])(); 
      prev_index = cur_index;     
  }
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

  //this is the celebration -- toggle all led's
  PORTB ^= 0xBD;
  PTT ^= 0xF9;
  PORTA ^= 0x81;
  PTP ^= 0x87;
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
 
  DDRB |= 0xBD;	//init led's
  DDRT |= 0xF9;	//init more led's
  DDRA |= 0x81;	//init more led's
  DDRA &= ~0x2C;   //init buttons
  DDRP |= 0x87;	//init more led's

  LCD_Init();
  PLL_Init();
  OC_Init3();
  OC_Init0();
  LCD_Clear();
  //LCD_GoTo(0x00);
 // LCD_OutChar('a');
  
  //turn led's off
  PORTB &= ~0xBD;
  PTT &= ~0xF9;
  PORTA &= ~0x81;
  PTP &= ~0x87;
    
  //LCD_OutString("Push to start");                                                           
  printf("Push to start");
  while(!(PORTA & 0x20)); //wait for game to start
  LCD_Clear();
  printf("Reg <- -> Hard");
  while(!(PORTA & 0x0C));
  if ((PORTA & 0x08)){
    mode = HARD;
  }else{
    mode = REG; 
  }
  while((PORTA & 0x0C));
  LCD_Clear();
  printf("%u", jackpot);
  //LCD_OutString("Go!");
  asm cli
  
  while(1){
    if ((PORTA & 0x0C)){
      TIE  &= ~0x01;   
      if (( (off_index == WIN_LED_ONE) && (PORTA & 0x08) ) || ( (off_index == WIN_LED_TWO) && (PORTA & 0x04) ) || ( (prev_index == WIN_LED_ONE) && (PORTA & 0x08) ) || ( (prev_index == WIN_LED_TWO) && (PORTA & 0x04) )){
       on_index = 0;
       off_index = -1;
       prev_index = -1;
       cur_index = 0;
       TIE |= 0x08; 
       jackpot = 150;
       won = 1; 
      }
      while(!(PORTA & 0x20)){
      //wait for continue button to pushed
        if (won){
          //LCD_OutString("JACKPOT!");
          printf("JACKPOT!");
        }
      }
      if (won){
        TIE &= ~0x08;
        PORTB &= ~0xBD;
        PTT &= ~0xF9;
        PORTA &= ~0x81;
        PTP &= ~0x87;
        won = 0;
        LCD_Clear();      
        printf("Reg <- -> Hard");
        while(!(PORTA & 0x0C));
        if ((PORTA & 0x08)){
          mode = HARD;
        }else{
          mode = REG; 
        }
        while((PORTA & 0x0C));
        LCD_Clear();      
      }else{
        if (mode == REG){
          jackpot++;
        }else{
          jackpot += 10; 
        }
      }
      LCD_Clear();
      printf("%u", jackpot);
      //LCD_OutString("Go!");
      TIE  |= 0x01;   // arm OC0    to be armed conditionally     
    }
  }
  return;
}
