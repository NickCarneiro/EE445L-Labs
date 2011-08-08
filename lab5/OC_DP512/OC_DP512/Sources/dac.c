//**********DAC.C************

#include <mc9s12dp512.h>
#include "dac.h"
#include "music.h"
#include "timer.h"
#include "switches.h"

void Debug_Profile(unsigned short thePlace);

//pitch constants
#define E_FREQUENCY 568
#define G_FREQUENCY 479
#define G_SHARP_FREQUENCY 451
#define C_SHARP_FREQUENCY 676
#define G_SHARP_PRIME_FREQUENCY 451*2
#define D_FREQUENCY 638
#define C_FREQUENCY 716
#define B_FREQUENCY 760
#define F_SHARP_FREQUENCY 507
#define A_FREQUENCY 426
#define F_FREQUENCY 537

//sine wave array structure
/*unsigned short voltage_ladder [] = {
  1024,  1219,  1407,  1580,  1731,
  1855,  1948,  2005,  2024,  2005,
  1948,  1855,  1731,  1580,  1407,
  1219,  1024,  829,   641,   468,
  317,   193,   100,   43,    24,
  43,    100,   193,   317,   468,
  641,   829
};*/
unsigned short voltage_ladder [] = {
1007,
1088,
1156,
1194,
1067,
789,
303,
99,
789,
1510,
1476,
1173,
1067,
1037,
1084,
1062,
1011,
1015,
1045,
1062,
1011,
1011,
1058,
1113,
1084,
1075,
1079,
1105,
1088,
1049,
1015,
1045
};
unsigned short DAC_Data = 0;
unsigned short volt_index = 0;
unsigned char Note;
//unsigned char volt_change = TRUE;
extern Envelope envelope[];
extern unsigned short note_index;

//array accessed in interrupt to output certain pitches
unsigned short frequency_array [] = {
  100000,E_FREQUENCY,G_FREQUENCY,D_FREQUENCY,C_FREQUENCY,B_FREQUENCY,F_SHARP_FREQUENCY, A_FREQUENCY, F_FREQUENCY, C_SHARP_FREQUENCY, G_SHARP_FREQUENCY, G_SHARP_PRIME_FREQUENCY
};

//--------DAC_Init------
// initialized the DAC
// inputs: none
// outputs: none
void DAC_Init(void){
  DDRS |= 0xE0;   //PS5-7 are output
  //DDRS &= ~0x10;  //PS4 is input
  SPI0CR1 = 0x5C;
  SPI0CR2 = 0x00;
  SPI0BR = 0x00;
  PTS_PTS7 = 1;
}

//---------------------OC_Init0---------------------
// arm output compare 0 for 100Hz periodic interrupt
// Input: none
// Output: none 
void OC_Init0(){
  TIOS |= 0x01;   // activate TC0 as output compare
  TC0 = TCNT + envelope[2].Duration; // first interrupt right away
}

//--------Send------
// Sends data to the DAC 
// inputs: data/command
// outputs: none
void Send(unsigned char code){
  unsigned char dummy_var;
  while((!(SPI0SR & 0x20)));  //wait SPTEF
  SPI0DR = code;
  while((!(SPI0SR & 0x80)));  //wait SPIF
  dummy_var = SPI0DR;
}
                
unsigned char old_note = 0;
unsigned short thingy = 256;
                
//--------DAC_Out------
// outputs to the DAC
// inputs: none
// outputs: none
void DAC_Out(unsigned char output_note){
  Note = output_note;
  PTS_PTS7 = 0;
  Send(0x09);
  Send((char) thingy >> 8);//(voltage_ladder[volt_index] >> 8));
  Send(0x09);
  Send((char) thingy);//voltage_ladder[volt_index]);
  PTS_PTS7 = 1;
  if(old_note != output_note){
    Debug_Profile(2);
    old_note = output_note;
  }
}                  

static unsigned short counter = 320;
interrupt 8 void TOC0handler(void){ // executes at 100 Hz 
  TFLG1 = 0x01;                     // acknowledge OC0
  //PTP ^= 0x80;
  TC0 = TC0 + frequency_array[Note];
  
  if (Note != 0){
    volt_index = (volt_index + 1) & 0x1F;
    if(counter == 0){
      Debug_Profile(1);
      counter = 320;
    } else{
      counter--;
    }
    //volt_change = TRUE;
    //DAC_Data = voltage_ladder[volt_index]; 
  }
  
  return;
}