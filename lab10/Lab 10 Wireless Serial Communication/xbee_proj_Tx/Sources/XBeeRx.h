//**************XBee.h*************

#ifndef __XBEE_H
#define __XBEE_H

#include "SCIb.h"		// virtual SCI port driver

// maximum number of bytes in the API frame identifier-specific data block
#define XBEE_MAX_DATA_LEN 128
#define XBEE_MAX_RFDATA_LEN 100

//*********** mwait *********************
//Input: number of msec to wait
//Output: none
//assumes TCNT timer is running at 667ns
void mwait(unsigned short msec);

//--------------sendATCommand-------------
// Send an AT command repeatedly until it receives 
// a reply that it was correctly received
void sendATCommand(char *pt);

// -------------XBee_Init------------------
// Initialize the XBee module
// Input: none
// Output: none
void XBee_Init(void);

//---------------stringLength---------------
// Determine the length of a string
// Input: string
// Output: the length of the string
unsigned short stringLength(char *string);

//---------------XBee_CreateTxFrame---------
// Create an API frame
// Input: data
// Output: an array containing the frame
unsigned short XBee_CreateTxFrame(unsigned char *data);

// ---------------XBee_SendTxFrame----------
// Send an API frame
// Input: frame length
// Output: none
void XBee_SendTxFrame(unsigned short length);

// ---------------XBee_TxStatus-------------
// Determine transmit status
// Input: none
// Output: 1 if successful; 
//         0 if no ACK
//         2 if CCA failure
//         3 if Purged 
unsigned char XBee_TxStatus(unsigned char frameID);

// ---------------XBee_ReceiveRxFrame----------
// Send an API frame
// Input: frame 
// Output: none
unsigned char* XBee_ReceiveRxFrame();//unsigned char* data);

#endif
