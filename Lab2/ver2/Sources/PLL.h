/*************************PLL.h***************************
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


//********* PLL_Init ****************
// Set 9S12DP512 PLL clock to 24 MHz
// Inputs: none
// Outputs: none
// Errors: will hang if PLL does not stabilize 
void PLL_Init(void);
