//*********MUSIC.C*********
#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#include "music.h"
#include "timer.h"
#include "switches.h"
#include "dac.h"
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"

void Debug_Profile(unsigned short thePlace);

unsigned short note_index = 0;
unsigned short delay = 100;
extern Envelope envelope[];
//---------------------OC_Init3---------------------
// arm output compare 3 for 1000Hz periodic interrupt
// Input: none    assumes another ritual enabled the TCNT
// Output: none 
void OC_Init3(){
  TIOS |= 0x08;    // activate TC3 as output compare
  //TIE |= 0x08;
  TC3   = TCNT + envelope[note_index].Duration; // first interrupt right away
}

//--------Music_Play------
// play the music
// inputs: pointer to song envelope
// outputs: harmonious melody
void Music_Play(unsigned char pitch){//Envelope *song_envelope[]){
  if (!TIE & 0x09){
    TIE |= 0x09;  
  }
  DAC_Out(pitch);//song_envelope[note_index]->frequency_pitch);
}

//--------Music_Stop------
// stops the music
// inputs: none
// outputs: none
void Music_Stop(void){
  TIE &= ~0x09;   //disarm the interrupts  
}

//--------Music_Rewind------
// Plays music from the beginning
// inputs: none
// outputs: none
void Music_Rewind(void){
  note_index = 0;   
}

interrupt 11 void TOC3handler(void){ // executes at 1000 Hz 
  TFLG1 = 0x08;         // acknowledge OC3
  //PTP ^= 0x80;
  TC3 = TC3 + envelope[note_index].Duration;
  
  if (delay == 0){
    PTP ^= 0x80;
    note_index++;
    if ( note_index == SONG_SIZE ){note_index = 0;}
    delay = 100;
    Debug_Profile(3);
    return;
  }
  
  delay--;
  return;  
}