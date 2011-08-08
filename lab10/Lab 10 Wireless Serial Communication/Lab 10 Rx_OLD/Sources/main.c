// filename ******** main.C ************** 
// Reagan Revisore, Michael May
// Date Created: 4/13/2011
// This Project implements the Reciever portion 
// of ZigBee communication
// Lab Number: 10
// TA: Justin Capogna 
// Date of Last Revision: 4/13/2011
// ***************************************


// Rx System Design
// The Rx DP512 initializes the virtual port, the XBee module, and the LCD display.  Data from the XBee module 
// is stored in the Rx FIFO.  The main program on the Rx DP512 continually polls the Rx FIFO and any available data is 
// displayed on the LCD using the LCD driver routines. 

#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"
//#include "derivative.h"      /* derivative-specific definitions */
#include "PLL.h"
#include "XBee.h"
#include "LCD.h"
#include "SCIb.h"

void main(void) {
  PLL_Init();
  XBee_Init();
  SCIb_Init(9600);
  LCD_Open();
  RxFifob_Init();
	EnableInterrupts;
  for(;;) {
   // Rx DP512 continually polls the Rx FIFO and any available data is 
   // displayed on the LCD using the LCD driver routines.
    unsigned char data;
    if(SCIb_InStatus()) {
      data = XBee_ReceiveRxFrame();
      LCD_OutChar(data);
    }
  }
}
