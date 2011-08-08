// filename ******** IC4.C ************** 
// Interrupt Input Capture program for the 9S12C32   
// Jonathan W. Valvano 6/23/04
// 2 by 2 matrix keyboard
// PT3,PT2 are inputs from a keyboard column, with internal +5pullup
// PT7,PT6 are outputs to keyboard row
// uses OC4 interrupt to debounce

#include <hidef.h>         /* common defines and macros */
#include <mc9s12c32.h>     /* derivative information */

#pragma LINK_INFO DERIVATIVE "mc9s12c32"
#include "PLL.H"
//---------Wait------------------
// fixed time delay
// inputs: time to wait in 667ns cycles
// outputs: none
void Wait(unsigned short delay){ 
unsigned short startTime;
  startTime = TCNT;
  while((TCNT-startTime) <= delay){} 
}

unsigned char Count;
unsigned char LastKey;
// -------------Key_Init--------------
// enable input capture 2,3 interrupts on falling edge
// inputs: none
// outputs: none
// will enable IC3.IC2 interrupts
// enable OC4, disarm OC4
// assumes all of PORTT used for this interface
// so it will not be friendly accessing PORTT 
void Key_Init(void){  
asm sei           // make atomic
  DDRT = 0xC0;    // PT7,PT6 all rows are output
  PERT = 0x0C;    // internal pullup on PT3,PT2
  PTT  = 0x00;    // PT7=PT6=0 enable all rows
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
  TCTL4 = 0xA0;   // falling edges IC3,IC2
  TIE = 0x0C;     // Arm only IC3,IC2
  Count = 0;  
  LastKey = 0;
  TIOS = 0x10;   // PT4 output compare,  PT3,PT2 input capture
asm cli
}

void main(void) {	
  PLL_Init();       // running at 24MHz
  Key_Init(); // start IC thread
  for(;;) {	
  }
}
/* Returns ASCII code for key pressed, 
   return zero if no key pressed
   returns 0xFF if more than one is pressed */
unsigned char KeyScan(void){
unsigned char numKey;
unsigned char key,column; 
  numKey = 0;           // no key pressed yet
  key = 0;              // default value for no key
//*********** Row 3 *******************************
  DDRT = 0x80;  	      // select row 3 
  Wait(5);              // 4 us
  column = PTT;         // read columns
  if((column&0x08)==0){  
    key = '1';          // row 3, column 3
    numKey++;
  }
  if((column&0x04)==0){  
    key = '2';          // row 3, column 2
    numKey++;
  }
//*********** Row 2 *******************************
  DDRT = 0x40;  	      // select row 2 
  Wait(5);              // 4 us
  column = PTT;         // read columns
  if((column&0x08)==0){  
    key = '4';          // row 2, column 3
    numKey++;
  }
  if((column&0x04)==0){  
    key = '5';          // row 2, column 2
    numKey++;
  }
  if(numKey>1){
    key = 0xFF;         // multiple key error
  }
  return key;
}

void interrupt 10 IC2Han(void){ 
  TC4 = TCNT+15000; // 10 ms later
  TIE = 0x10;       // arm OC4, disarm IC3,IC2
  TFLG1 = 0xFF;     // clear all flags
}
void interrupt 11 IC3Han(void){ 
  TC4 = TCNT+15000; // 10 ms later
  TIE = 0x10;       // arm OC4, disarm IC3,IC2
  TFLG1 = 0xFF;     // clear all flags
}
void interrupt 12 OC4Han(void){
unsigned char mykey;
  mykey = KeyScan();
  if(mykey==0xFF){     // more than one
    TC4 = TCNT+10000;   // wait for there to be one
  } else{            
    TIE = 0x0C;        // arm IC3,IC2, disarm OC4
    if(mykey){         // exactly one
      LastKey = mykey; // should be fifo Put
      Count++;
    }
  }
  DDRT = 0xC0;         // all rows zero
  TFLG1 = 0xFF;        // clear all flags
}
