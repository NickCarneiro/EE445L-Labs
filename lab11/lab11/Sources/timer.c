//**********Timer Functions*******
#include "timer.h"
#include <MC9S12C32.h>

void Timer_Init(void){
  
    TSCR1 = 0x80; //enable TCNT
    TSCR2 = 0x07; 
}

void Timer_Wait(unsigned short cycles){
  
  unsigned short startTime = TCNT;
  while ((TCNT - startTime) <= cycles){}
}

void Timer_Wait_onems(unsigned short delay){
  
  unsigned short i;
  
  for (i = 0 ; i < delay ; i++){
    Timer_Wait(188);
  }
}
