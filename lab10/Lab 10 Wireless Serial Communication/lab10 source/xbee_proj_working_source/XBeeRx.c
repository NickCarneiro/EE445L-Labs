// ******** XBee.C ************** 
// EE 464 - FAR System, Xbee DP512 drivers
// Michael Davis, Gage Eads, Grace Gee, Trang Lee


#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#include "SCIb.h"


#pragma LINK_INFO DERIVATIVE "mc9s12dp512"

unsigned char reply[10];
unsigned char frame[50];
unsigned char status[7];
unsigned char frameID=0;//0xA1;
unsigned short dataLength;

//*********** mwait *********************
//Input: number of msec to wait
//Output: none
//assumes TCNT timer is running at 667ns
void mwait(unsigned short msec){
  unsigned short startTime;
  for(; msec > 0; msec--){
    startTime = TCNT;
    while((TCNT - startTime) <= 1500){
    }
  }  
}

//--------------sendATCommand-------------
// Send an AT command repeatedly until it receives 
// a reply that it was correctly received
void sendATCommand(char *pt){
  do {
    SCIb_OutString(pt);
    mwait(100);
    SCIb_InString(reply,2);
  } while((reply[0] != 'O' || reply[1] != 'K')&&(reply[1] != 'O' || reply[0] != 'K'));
  reply[0] = 0;
  reply[1] = 0;
}

// -------------XBee_Init------------------
// Initialize the XBee module
// Input: none
// Output: none
void XBee_Init(void){
  char ATDL[8] = {'A','T','D','L','4','E',0x0D};
  char ATMY[8] = {'A','T','M','Y','4','F',0x0D};
  char ATAP[8] = "ATAP 1\r";
  char ATCN[6] = {'A','T','C','N',0x0D};
  sendATCommand("+++");          // enter command mode
  sendATCommand(ATDL);  // sets destination address to 78
  sendATCommand(ATMY);  // sets my address to 79
  sendATCommand(ATAP);   // API mode 1 (sends/receive message packets)
  sendATCommand(ATCN);     // end command mode
}

//---------------stringLength---------------
// Determine the length of a string
// Input: string
// Output: the length of the string
unsigned short stringLength(char *string){
  unsigned short length = 0;
  while (string[length] != '\0'){
    length++;
  }
  return length;
}


typedef struct {
    unsigned short length;
    unsigned char api;
    unsigned short srcAddr;
    unsigned char rssi;
    unsigned char options;
    unsigned char data[20];    
    unsigned char checksum;
} RxFrame;

// ---------------XBee_ReceiveRxFrame----------
// Send an API frame
// Input: frame 
// Output: none
unsigned char * XBee_ReceiveRxFrame(unsigned char *data){
  unsigned char checksum;  
  RxFrame frame;
  unsigned int i;
  
  //while (SCIb_InChar() != 0x7E) ;
  SCIb_InChar();  
  frame.length = SCIb_InChar() << 8;
  checksum = frame.length >> 8;
  frame.length |= SCIb_InChar();
  checksum += frame.length & 0xFF;
  
  if(frame.length > 100) {
    *data = 0;
    return data;
  }     
  
  frame.api = SCIb_InChar();
  checksum += frame.api;
  
  frame.srcAddr = SCIb_InChar() << 8;
  checksum += frame.srcAddr >> 8; 
  frame.srcAddr |= SCIb_InChar();
  checksum += frame.srcAddr & 0xFF;
  
  frame.rssi = SCIb_InChar();
  checksum += frame.rssi;
  frame.options = SCIb_InChar();
  checksum += frame.options;
  
  for (i = 0; i < frame.length-5; i++) {
    frame.data[i] = SCIb_InChar();
    data[i] = frame.data[i];
    checksum += frame.data[i];    
  }
  data[i] = 0;
  frame.checksum = SCIb_InChar();  
  return data;
}

// ---------------XBee_RxStatus-------------
// Determine transmit status
// Input: none
// Output: 1 if successful; 0 otherwise
unsigned char XBee_RxStatus(void){
  unsigned short i;
  unsigned short checkSum;
  for(i=0;i<dataLength;i++){
    checkSum = checkSum+frame[i+9];
  }
  checkSum = checkSum + frame[3] +frame[4]+frame[5]
            +frame[6]+frame[7]+frame[8]+frame[9+i];
  if (checkSum==0xFF)
    return 1;
  else
    return 0;
}


