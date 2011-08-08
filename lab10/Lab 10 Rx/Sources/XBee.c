// filename ******** XBee.C ************** 
// Reagan Revisore, Michael May
// Date Created: 4/13/2011
// ZigBee Communication Receiver Driver
// Lab Number: 10
// TA: Justin Capogna 
// Date of Last Revision: 4/13/2011
// ***************************************
#include "derivative.h"      /* derivative-specific definitions */
#include "lcd.h"

void sendATCommand(char *command);

// initialize the XBee module
void XBee_Init(){
  sendATCommand("X");
  wait1ms(2980);
  sendATCommand("+++"); 
  wait1ms(2980);
  sendATCommand("ATDL4F\r");   
  sendATCommand("ATDH0\r");     // Sets destination high address to 0
  sendATCommand("ATMY4E\r");    // Sets my address to 78
  sendATCommand("ATAP1\r");     // API mode 1 (sends/receive packets)
  sendATCommand("ATCN\r");      // Ends command mode
  
}


#define MFRAME_SIZE 64
//accepts messages into the receiver 
//When the XBee module receives an API data frame it sends a message to the 9S12
unsigned char* XBee_ReceiveRxFrame(){ 
  unsigned char i;
  unsigned char message[MFRAME_SIZE];
  unsigned short length, source;
  unsigned char frameID, api, rssi, opt, checkSum;
  frameID = SCIb_InChar2();//FrameID
  length = SCIb_InChar2()*256; //MS length
  length += (SCIb_InChar2()-5);    //LS length
  api = SCIb_InChar2();//API
  source = SCIb_InChar2()*256;//SourceMSB
  source += SCIb_InChar2();//SourceLSB
  rssi = SCIb_InChar2();//RSSI
  opt = SCIb_InChar2();//Opt
  checkSum = 0xFF - (api + source + rssi + opt);
  for(i = 0; i < length; i++){ //Message
    message[i] = SCIb_InChar2();  
    checkSum -= message[i];  
  }
  if(checkSum != SCIb_InChar2()) return 0;//message incorrectly received 
  return message; 
}


// create an API frame 
// This routine creates an API transmit data frame consisting of a 
// start delimiter, frame length, frame data, and checksum fields.
// The frame data field contains destination address and 
// transmission options information
void XBee_CreateTxFrame() {
  
}

// send an API frame 
// This routine transmits the API transmit data frame 
// to the XBee module via the  virtual port.
void XBee_SendTxFrame(){

}

// determine transmit status
void XBee_TxStatus(){

}

// sends an AT command repeatedly until it receives 
// a reply that it was correctly received
void sendATCommand(char *command){
  SCIb_OutString(command);
  wait1ms(20);
  while(SCIb_InChar2() != 'O'){ 
  }  
  while(SCIb_InChar2() != 'K'){ 
  }  
  while(SCIb_InChar2() != '\r'){ 
  }    
}

