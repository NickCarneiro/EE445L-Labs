//******XBEE.C******
//contains xbee drivers
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"
#include "XBee.h"
#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#include "SCIb.h"
#include "SCI1.h"
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
  sendATCommand("ATDL3C\r");  // sets destination address to __
  sendATCommand("ATMY2E\r");  // sets my address to __
  sendATCommand("ATAP 1\r");   // API mode 1 (sends/receive message packets)
  sendATCommand("ATCN\r");     // end command mode  
}       

//---------------------XBee_CreateTxFrame--------------------- 
//creates frame to send to xbee
//Input: frame params
//Output: frame
unsigned char* XBee_CreateTxFrame(char* message){
  unsigned char sframe[75];
  char *msg_count = &message[0];
  unsigned short i = 0, j = 0;
  unsigned short length = 0; 
  unsigned short check_sum = 0;
  
  //calculating message length
  while(*msg_count){
    length++;
    msg_count++;  
  }
  length += 5;
  
  //creating frame of variable length
  sframe[0] = 0x7E;
  sframe[1] = (char) (length >> 8);
  sframe[2] = (char) (length);
  sframe[3] = 0x01;    //api
  check_sum += sframe[3];
  sframe[4] = Id++;
  check_sum += sframe[4];
  sframe[5] = (char) (0x3C >> 8);
  sframe[6] = (char) 0x3C;
  check_sum += (sframe[5]>>8) + sframe[6];
  sframe[7] = 0;       //opt
  check_sum += sframe[7];
  //populate message portion
  for (i = 8, j = 0; j < length - 5 ; i++, j++){
    sframe[i] = message[j];
    check_sum += sframe[i];  
  }
  sframe[i] = 0xFF - (check_sum & 0x00FF);//sframe[i+1]=CheckSum
  if (Id == 0){Id = 1;}
  return &sframe[0];
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
  //unsigned short frame_size = stringLength(f);
  unsigned char *frame_count = f+8;
  unsigned short frame_size = ((*(f+1)<<8) + *(f+2)) + 4;

  for (i = 0 ; i < 8; i++){
    SCIb_OutChar(f[i]);
  } 
  
  for (; i < frame_size; i++){  
    SCIb_OutChar(f[i]);          //put bytes into fifo
  }
  return XBee_TxStatus();
}    