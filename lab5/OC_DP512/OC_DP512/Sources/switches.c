//**********SWITCHES.C********
//switch drivers
//assign ports and direction registers
//authors: Aaron Alaniz (aa28877)
//         Denis Sokolov (dks574)

#include <mc9s12dp512.h>
#include "music.h"
#include "timer.h"
#include "switches.h"
#include "dac.h"

extern unsigned char input;

//---------Switches_Init------------------
// initialize switches
// inputs: none 
// outputs: none
void Switches_Init(void) 
{
  DDRP &= ~0x07;   //PP0-3 inputs
}

//--------Get_Switches------
// return switch values
// inputs: none
// outputs: switch value
unsigned char Get_Switches(void)
{
  return (PTP & 0x07);
}
  