//******XBEE.C******
//contains xbee drivers

#include "XBee.h"
#include <mc9s12dp512.h>
#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#include "SCIb.h"
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"
#include "timer.h"

//static char [] frame;
#define LEN 7
#define OK 0
#define TWENTY_MS 20
#define ONE_1S 1100
unsigned char Id = 1;

//---------------------XBee_Init--------------------- 
//initialize XBee
//Input: none   
//Output: none
void XBee_Init(void){
//X  wait 1.1s  +++  wait 1.1s OK<CR>  Enter command mode 
XBee_SendTxFrame(XBee_CreateTxFrame(2,0,(unsigned short)'X'));
Timer_Wait_onems(ONE_1S);
XBee_SendTxFrame(XBee_CreateTxFrame(2,0,(unsigned short)'+'));
XBee_SendTxFrame(XBee_CreateTxFrame(2,0,(unsigned short)'+'));
XBee_SendTxFrame(XBee_CreateTxFrame(2,0,(unsigned short)'+'));
Timer_Wait_onems(ONE_1S);
XBee_SendTxFrame(XBee_CreateTxFrame(2,0,0x4F4B));
//ATDL4F<CR>  wait 20ms    OK<CR>  Destination address to 79 
sendATCommand("DL4F\r");
Timer_Wait_onems(TWENTY_MS);
//ATDH0<CR>  wait 20ms    OK<CR>  Destination high address to 0 
sendATCommand("DH0\r");
Timer_Wait_onems(TWENTY_MS);
//ATMY4E<CR> wait 20ms    OK<CR>  Sets my address to 78 
sendATCommand("MY4E\r");
Timer_Wait_onems(TWENTY_MS);
//ATAP1<CR>  wait 20ms    OK<CR>  API mode 1 (packets) 
sendATCommand("AP1\r");
Timer_Wait_onems(TWENTY_MS);
//ATCN<CR>  wait 20ms    OK<CR>  Ends command mod
sendATCommand("CN\r");
Timer_Wait_onems(TWENTY_MS);  
}       

//---------------------XBee_CreateTxFrame--------------------- 
//creates frame to send to xbee
//Input: frame params
//Output: frame
unsigned char* XBee_CreateTxFrame(unsigned short dest,unsigned char opt,unsigned short message){
  unsigned char frame[12]; 
  char checksum = 0xFF - (0x01 + Id + (char)(dest >> 8) + (char)(dest) + opt + (char)(message >> 8) + (char)(message));
  frame[0] = 0x7E;
  frame[1] = (char) (LEN >> 8);
  frame[2] = (char) (LEN);
  frame[3] = 0x01;
  frame[4] = Id++;
  frame[5] = (char) (dest >> 8);
  frame[6] = (char) dest;
  frame[7] = opt;
  frame[8] = (char) (message >> 8);
  frame[9] = (char) message;
  frame[10] = checksum;
  if (Id == 0){Id = 1;}
  return frame;
}

//---------------------XBee_SendTxFrame---------------------
//send frame to the xbee
//Input: frame to send
//Output: true if successful
unsigned char XBee_SendTxFrame(unsigned char* f){
  unsigned short i;
  for (i = 0 ; i < 11 ; i++){
    SCIb_OutChar(f[i]);   //put bytes in the FIFO  
  }
  return 0;
}

//---------------------XBee_TxStatus--------------------- 
//gets the status of the xbee
//Input: none
//Output: 1 if successful, 0 otherwise
short XBee_TxStatus(void){
   unsigned char message;
   
   message = XBee_ReceiveRxFrame();
   if (message == OK)
    return 1;
   else 
    return 0;
}

//---------------------XBee_ReceiveRxFrame--------------------- 
//gets a frame from the xbee
//Input: none 
//Output: frame received
unsigned char XBee_ReceiveRxFrame(void){
  return SCIb_InChar();
}

//---------------------sendATCommand--------------------- 
//continuously sends AT command to xbee until receives confirmation
//Input: command   
//Output: true if successful
short sendATCommand(char *string){
  unsigned char done = 0;
  unsigned short i = 0;
  unsigned char* f;
  do{
    while(!done){
      f = XBee_CreateTxFrame(2,0,string[i]);
      XBee_SendTxFrame(f);  
      if (string[i] == (unsigned char)NULL){done = 1;}
    }
    i = 0;
    done = 0;
  } while(!XBee_TxStatus());
  return 0;
}