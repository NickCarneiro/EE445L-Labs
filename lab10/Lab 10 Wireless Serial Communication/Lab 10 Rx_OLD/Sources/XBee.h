// filename ******** XBee.H ************** 
// Reagan Revisore, Michael May
// Date Created: 4/13/2011
// ZigBee Communication Receiver Driver
// Lab Number: 10
// TA: Justin Capogna 
// Date of Last Revision: 4/14/2011
// ***************************************

// initialize the XBee module
void XBee_Init();

//accepts messages into the receiver 
//When the XBee module receives an API data frame it sends a message to the 9S12
int XBee_ReceiveRxFrame();