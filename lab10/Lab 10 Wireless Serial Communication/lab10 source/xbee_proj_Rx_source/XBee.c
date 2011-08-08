//******XBEE.C******
//contains xbee drivers
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"
#include "XBee.h"
#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#include "SCIb.h"
#include "timer.h"

//static char [] frame;
#define LEN 6
#define OK 0
#define TWENTY_MS 20
#define THREE_SEC 3000

const unsigned short rxID = 63;
const unsigned short txID = 65;
static unsigned char Id = 1;
char response[10];

unsigned short stringLength(char *string);

//--------------sendATCommand-------------
// Send an AT command repeatedly until it receives 
// a reply that it was correctly received
void sendATCommand(char *command){
  while(1){
    SCIb_OutString(command);
    Wait1ms(TWENTY_MS);
    SCIb_InString(response,2);
    if ((response[0] == 'O' && response[1] == 'K') || (response[0] == 'K' && response[1] == 'O')){
      break;
    }
  }  
    response[0] = 0;
    response[1] = 0;  
}

//---------------------XBee_Init--------------------- 
//initialize XBee
//Input: none   
//Output: none
void XBee_Init(void){

  sendATCommand("+++");          // enter command mode
  sendATCommand("ATDL2E\r");  // sets destination address to 78
  sendATCommand("ATMY3C\r");  // sets my address to 79
  sendATCommand("ATAP 1\r");   // API mode 1 (sends/receive message packets)
  sendATCommand("ATCN\r");     // end command mode  
}       

//---------------------XBee_CreateTxFrame--------------------- 
//creates frame to send to xbee
//Input: frame params
//Output: frame
unsigned char* XBee_CreateTxFrame(char* message){
  unsigned char frame[50];
  char *msg_count = &message[0];
  unsigned short i = 0, j = 0;
  unsigned short length = 0; 
  unsigned char check_sum = 0;
  
  //calculating message length
  while(*msg_count){
    length++;
    msg_count++;  
  }
  length += 5;
  
  //creating frame of variable length
  frame[0] = 0x7E;
  frame[1] = (char) (length >> 8);
  frame[2] = (char) (length);
  frame[3] = 0x01;    //api
  check_sum += frame[3];
  frame[4] = Id++;
  check_sum += frame[4];
  frame[5] = (char) (0x2E >> 8);
  check_sum += frame[5];
  frame[6] = (char) 0x2E;
  check_sum += frame[6];
  frame[7] = 0;       //opt
  
  //populate message portion
  for (i = 8, j = 0; i < length - 5 ; i++, j++){
    frame[i] = message[j];
    check_sum += frame[i];  
  }
  frame[i + 1] = 0xFF - check_sum;
  if (Id == 0){Id = 1;}
  return frame;
}

#define delimiter frame[0]
#define lengthMS frame[1]
#define lengthLS frame[2]
#define api frame[3]
#define id frame[4]
#define status frame[5]
//---------------------XBee_TxStatus--------------------- 
//gets the status of the xbee
//Input: none
//Output: 1 if successful, 0 otherwise
short XBee_TxStatus(void){
  unsigned char frame[7]; 
  unsigned char receiveI, checkSum;
  unsigned short subtractor; 
  //everything except checkSum(known sizes)
  for(receiveI = 0; receiveI < 6; receiveI++){   
    frame[receiveI] = SCIb_InChar2(); 
  }
  if(api != 0x89) return 0; //not a status check frame
  subtractor = api + id + status; 
  //receive checksum and verify
  checkSum = 0xFF - (subtractor & 0x00FF);
  if(checkSum != SCIb_InChar2() || (status != 0)) return 0; 
  return 1; //status=0->successful transmission
}

//---------------------XBee_SendTxFrame---------------------
//send frame to the xbee
//Input: frame to send
//Output: true if successful
unsigned char XBee_SendTxFrame(unsigned char* f){
  unsigned short i;
  unsigned short size = stringLength(f);
  /*unsigned char *frame_count = f;
  unsigned short frame_size = 0;
  while(*frame_count){
    frame_size++; 
    frame_count++;
  }           */
  for (i = 0 ; i < size ; i++){  
    SCIb_OutChar(f[i]);          //put bytes into fifo
  }
  //return XBee_TxStatus();
  return 1;
}

//0-3 use #define's from TxStatus
/*
#define sourceMS frame[4] 
#define sourceLS frame[5]
#define rssi frame[6]      //signal strength
#define opt frame[7]
#define message Message[0]+Message[1]
//---------------------XBee_ReceiveRxFrame--------------------- 
//gets a frame from the xbee
//Input: none 
//Output: frame received
unsigned char* XBee_ReceiveRxFrame(void){ 
  unsigned char frame[11];
  unsigned char Message[50]; 
  unsigned char receiveI, checkSum;
  unsigned short length, subtractor, source;  
  unsigned short msg_length = 0;
  //everything up to message(known sizes)
  /*for(receiveI = 0; receiveI < 8; receiveI++){   
    frame[receiveI] = SCIb_InChar(); 
  }
  source = (sourceMS<<8) + sourceLS;
  if(api != 0x81 || source != txID) return 0; //not a receive frame
  subtractor = api + source + rssi + opt; 
  length = (lengthMS<<8) + lengthLS;
  //receive message
  for(receiveI = 0; receiveI < (length - 5); receiveI++){  //for loop ends when receiveI+8 points at checkSum
    frame[receiveI+8] = SCIb_InChar();
    subtractor += frame[receiveI+8]; 
  }
  
  //receive checksum and verify
  checkSum = 0xFF - (subtractor & 0x00FF);
  if(checkSum != SCIb_InChar2()) return 0;//message incorrectly received
  unsigned short i = 0;
  SCIb_InChar();
  length = ((short)SCIb_InChar() << 8); 
  length += SCIb_InChar();
  msg_length = length - 5;
  api = SCIb_InChar();
  subtractor = api;
  sourceMS = SCIb_InChar();
  subtractor += sourceMS;
  sourceLS = SCIb_InChar();
  subtractor += sourceLS;
  rssi = SCIb_InChar();
  subtractor += rssi;
  opt = SCIb_InChar();
  subtractor += opt;
  for (i = 0 ; i < msg_length ; i++){
    Message[i] = SCIb_InChar();  
    subtractor += Message[i];  
  }
  checkSum = SCIb_InChar();
  
  return Message; 
}
*/
//---------------stringLength---------------
// Determine the length of a string
// Input: string
// Output: the length of the string
unsigned short stringLength(char *c){
  unsigned short len = 0;
  while (c[len] != '\0'){
    len++;
  }
  return len;
}


typedef struct {
    unsigned short len;
    unsigned char API;
    unsigned short src;
    unsigned char rssi;
    unsigned char opt;
    unsigned char data[20];    
    unsigned char checksum;
} RxFrame;

// ---------------XBee_ReceiveRxFrame----------
// Send an API frame
// Input: frame 
// Output: none
unsigned char * XBee_ReceiveRxFrame(unsigned char *data){
  unsigned char checksum = 0;  
  RxFrame rframe;
  unsigned short i;
  
  SCIb_InChar();
  SCIb_InChar();
  SCIb_InChar();  //length and delimeter  

  if(rframe.len > 100) {
    *data = 0;
    return data;
  }     
  
  rframe.API = SCIb_InChar();
  checksum += rframe.API;
  
  rframe.src = (unsigned short)SCIb_InChar() << 8;
  checksum += rframe.src >> 8; 
  rframe.src |= (unsigned short)SCIb_InChar();
  checksum += rframe.src & 0xFF;
  
  rframe.rssi = SCIb_InChar();
  checksum += rframe.rssi;
  rframe.opt = SCIb_InChar();
  checksum += rframe.opt;
  
  for (i = 0; i < rframe.len-5; i++) {
    rframe.data[i] = SCIb_InChar();
    data[i] = rframe.data[i];
    checksum += rframe.data[i];    
  }
  data[i+1] = 0;
  rframe.checksum = SCIb_InChar();  
  return data;
}


