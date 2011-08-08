//*********** Main.C ********************
//Transmit Side

#include <hidef.h>
#include "derivative.h"
#include "SCIb.H"
#include "PLL.H"
#include "XBeeRx.h"
#include "TXFIFOb.h"
#include "SCI1.h"

//---------------------OutCRLF---------------------
// Output a CR,LF to SCI to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  SCI1_OutChar(CR);
  SCI1_OutChar(LF);
  PTP_PTP7 ^= 1;
}

void main(void){
  unsigned short phrase = 0;
  unsigned char data[50];
  unsigned short check = 0;
  unsigned short length  = 0;
  int i = 0;
  
  SCIb_Init(9600);
  DDRP |= 0x80;       //Port P bit 7 is output to LED
  PLL_Init();         //Running at 24 MHz
  SCI1_Init(115200);    // fastest standard baud rate on run mode 9S12DP512 
  XBee_Init();
  asm cli
  
  SCI1_OutString("XBee Test"); OutCRLF();
  
  for(;;){
      SCI1_OutString("XBee Output: ");
      XBee_ReceiveRxFrame(data);
      SCI1_OutString(data);OutCRLF();   
  }
} 
    