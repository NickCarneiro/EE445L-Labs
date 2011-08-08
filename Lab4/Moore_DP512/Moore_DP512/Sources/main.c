// filename ******** Main.C ************** 
// Authors: Aaron Alaniz  (aa28877)
//          Denis Sokolov (dks574)

#include <hidef.h>           /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "pll.h"
#include "timer.h"
#include "stepper.h"
#include "switches.h"

extern const struct State* Cur_State;
static unsigned char old_switches = 0x00;
static unsigned char input = 0;
  
//---------------------OC_Init0---------------------
// arm output compare 0 for periodic interrupt
// Input: none
// Output: none
void OC0_Init(void)
{
  DDRP |= 0x80;     // debugging monitor (heartbeat)
  TIOS |= 0x01;     // activate TC0 as output compare
  TIE |= 0x01;      // arm the interrupt
  TC0 = TCNT+100;   // first interrupt right away
}

//handle OC0 interrupts
interrupt 8 void TOC0handler(void)
{ 
  TFLG1 = 0x01;                                 // acknowledge OC0
  TC0 = TC0 + Cur_State->time;                  // 300 ms
  PTT = (PTT & 0xF0) + (Cur_State->output);     //set output
  Cur_State = Cur_State->next[input];           //step to next state
  PTP ^= 0x80;
}

void main(void){
  
  PLL_Init();       // 24 MHz
  Timer_Init();     // TCNT at 5.33us period
  OC0_Init();       // OC0 interrupts every 300ms
  Switches_Init();  // set switch directions
  Stepper_Init();   // initialize state machine
  asm cli
 
  while(1) 
  { 
    //state machine runs in back ground
   
    old_switches = Get_Switches();
    Timer_Wait(1876);                      //debounce switches ~10 micro seconds
    if (old_switches == Get_Switches()){
      input = old_switches;                //if switches still same, then update input to state machine
    }
   
  }
}
