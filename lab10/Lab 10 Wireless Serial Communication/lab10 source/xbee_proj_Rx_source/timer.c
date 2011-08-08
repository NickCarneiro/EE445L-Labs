//**********Timer Functions*******
#include "timer.h"
#include <MC9S12DP512.h>

//---------------------Timer_Init---------------------
// initializes timer
// Input: none
// Output: none
void Timer_Init(void){
    TSCR1 = 0x80; //enable TCNT
    TSCR2 = 0x00; //divide by 1
    PACTL = 0;        // timer prescale used for TCNT  
}

//---------------------Wait---------------------
// time delay
// Input: time in 41.6667ns(1/24M)
// Output: none
void Wait(unsigned short delay){ 
unsigned short startTime;
  startTime = TCNT;
  while((TCNT-startTime) <= delay){}  
}

//---------------------Wait1ms---------------------
// time delay
// Input: time in msec
// Output: none
void Wait1ms(unsigned short msec){ 
  for(;msec;msec--){
    Wait(24000);    // 1ms wait
  }
}