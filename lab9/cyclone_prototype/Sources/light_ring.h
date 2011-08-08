//******LED toggle Functions******
//Purpose(all): Toggle specified LED in ring. Accessed via function pointer.
//Input: none
//Output: toggles desired LED 	

#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#include <stdio.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"

void ToggleLED_top(void);
void ToggleLED_b(void);
void ToggleLED_c(void);
void ToggleLED_d(void);
void ToggleLED_e(void);
void ToggleLED_f(void);