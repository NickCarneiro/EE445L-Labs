/*************************PLL.C***************************
*   boosts the CPU clock to 24 MHz                       *
*                                                        *
*   Created by Theodore Deden on 20 January 2004.        *
*   Modified by Theodore Deden on 9 February 2004.       *
*   Last Modified by Jonathan Valvano 7/13/07.           *
*                                                        *
*   Distributed underthe provisions of the GNU GPL ver 2 *
*   Copying, redistributing, modifying is encouraged.    *
*                                                        *
*********************************************************/

#include <hidef.h>         /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */

//********* PLL_Init ****************
// Set 9S12DP512 PLL clock to 24 MHz
// Inputs: none
// Outputs: none
// Errors: will hang if PLL does not stabilize 
void PLL_Init(void){  
  SYNR = 0x02;
  REFDV = 0x01;
  
  /*PLLCLK = 2 * OSCCLK * (SYNR + 1) / (REFDV + 1)
             2 *   16   * (  2  + 1) / (  1   + 1)
	Values above give E clock of 24 MHz with 16 MHz crystal. 
	(OSCCLK is Crystal Clock Frequency)                    */
  
  CLKSEL = 0x00;
  
  /*Meaning for CLKSEL:
  Bit 7: PLLSEL = 0 Keep using OSCCLK until we are ready to switch to PLLCLK
  Bit 6: PSTP   = 0 Do not need to go to Pseudo-Stop Mode
  Bit 5: SYSWAI = 0 In wait mode system clocks stop.
  But 4: ROAWAI = 0 Do not reduce oscillator amplitude in wait mode.
  Bit 3: PLLWAI = 0 Do not turn off PLL in wait mode
  Bit 2: CWAI	= 0 Do not stop the core during wait mode
  Bit 1: RTIWAI = 0 Do not stop the RTI in wait mode
  Bit 0: COPWAI = 0 Do not stop the COP in wait mode
  */
  
  
  PLLCTL = 0xD1;
  
  /*Meaning for PLLCTL:
  Bit 7: CME   = 1; Clock monitor enable - reset if bad clock when set
  Bit 6: PLLON = 1; PLL On bit
  Bit 5: AUTO  = 0; No automatic control of bandwidth, manual through ACQ
  But 4: ACQ   = 1; 1 for high bandwidth filter (acquisition); 0 for low (tracking)
  Bit 3:            (Not Used by 9s12c32)
  Bit 2: PRE   = 0; RTI stops during Pseudo Stop Mode
  Bit 1: PCE   = 0; COP diabled during Pseudo STOP mode
  Bit 0: SCME  = 1; Crystal Clock Failure -> Self Clock mode NOT reset.
  
  */
  
  while((CRGFLG&0x08) == 0){ 	  // Wait for PLLCLK to stabilize.
  }  
  CLKSEL_PLLSEL = 1;  // Switch to PLL clock
}

