// filename ******** IC3.C ************** 
// Interrupt Input Capture program for the 9S12C32   
// Jonathan W. Valvano 3/30/04
// PT0,PT1 are debugging LEDs
// PT3 is input from a keyboard column, with internal +5pullup
// PT7 is output to keyboard row
// uses OC4 interrupt to debounce

#include <hidef.h>         /* common defines and macros */
#include <mc9s12c32.h>     /* derivative information */

#pragma LINK_INFO DERIVATIVE "mc9s12c32"
#include "PLL.H"

unsigned char Count;
// -------------IC_Init--------------
// enable input capture 3 interrupts on falling edge
// inputs: none
// outputs: none
// will enable IC3 interrupts
// enable OC4, disarm OC4
void IC_Init(void){ 
asm sei           // make atomic
  DDRT |= 0x80;   // PT7 output
  PERT |= 0x08;   // internal pullup on PT3
  DDRT &=~0x08;   // PT3 is input
  DDRT |= 0x03;   // PT0,PT1 debugging
  PTT &= ~0x80;   // PT7=0 enable row
  PTT &= ~0x03;   // PT1=PT0=0
  TSCR1 = 0x80;   // enable TCNT, 667ns
  TSCR2 = 0x04;   // divide by 16 TCNT prescale, TOI disarm
  PACTL = 0;      // timer prescale used for TCNT
/* Bottom three bits of TSCR2 (PR2,PR1,PR0) determine TCNT period
    divide TCNT    at 24MHz  
000   1     42ns  TOF  2.73ms 
001   2     84ns  TOF  5.46ms  
010   4    167ns  TOF  10.9ms      
011   8    333ns  TOF  21.8ms 	 
100  16    667ns  TOF  43.7ms 
101  32   1.33us  TOF  87.4ms		
110  64   2.67us  TOF 174.8ms  
111 128   5.33us  TOF 349.5ms  */ 
  TIOS &= ~0x08;  // PT3 input capture
  TCTL4 = (TCTL4&0x3F)|0x80; // falling edge
  TIE |= 0x08;    // Arm IC3
  Count = 0;  
  TIOS |= 0x10;   // PT4 output compare
  TIE &= ~0x10;   // disarm OC4
asm cli
}

void main(void) {	
  PLL_Init();       // running at 24MHz
  IC_Init();        // start IC thread
  for(;;) {	
  }
}
//**interrupt on switch touch PT3
interrupt 11 void IC3Han(void){
  PTT ^= 0x01;      // profile
  TIE &= ~0x08;     // disarm IC3
  TC4 = TCNT+15000; // 10 ms later
  TIE |= 0x10;      // arm OC4
  TFLG1 = 0x10;     // clear C4F
}
void interrupt 12 OC4Han(void){ 
  PTT ^= 0x02;      // profile
  TIE &= ~0x10;     // disarm OC4
  TIE |= 0x08;      // arm IC3
  TFLG1 = 0x08;     // clear C3F
  if((PTT&0x08)==0){
    Count++;
  }
}  
