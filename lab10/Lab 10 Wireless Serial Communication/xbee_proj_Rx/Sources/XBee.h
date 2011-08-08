//*******XBee.H********
//xbee drivers

extern const unsigned short txID;
extern const unsigned short rxID;

//---------------------XBee_Init--------------------- 
//initialize XBee
//Input: none   
//Output: none
void XBee_Init(void);//unsigned short boardID);

//---------------------XBee_CreateTxFrame--------------------- 
//creates frame to send to xbee
//Input: frame params
//Output: frame
unsigned char* XBee_CreateTxFrame(char* message);

//---------------------XBee_SendTxFrame---------------------
//send frame to the xbee
//Input: frame to send
//Output: true if successful
unsigned char XBee_SendTxFrame(unsigned char* f);

//---------------------XBee_TxStatus--------------------- 
//gets the status of the xbee
//Input: none
//Output: 1 if successful, 0 otherwise
short XBee_TxStatus(void);

//---------------------XBee_ReceiveRxFrame--------------------- 
//gets a frame from the xbee
//Input: none 
//Output: frame received
unsigned char* XBee_ReceiveRxFrame(unsigned char*);

//---------------------SendATCommand--------------------- 
// Send an AT command repeatedly until it receives 
// a reply that it was correctly received
void sendATCommand(char *);

