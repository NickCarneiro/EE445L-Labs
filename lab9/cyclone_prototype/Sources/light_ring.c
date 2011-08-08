//******LED toggle Functions******
//Purpose(all): Toggle specified LED in ring. Accessed via function pointer.
//Input: none
//Output: toggles desired LED 	

#include "light_ring.h"

void ToggleLED_top(){
  PTP ^= 0x80;          // debugging monitor
	PTT_PTT4 ^= 1;
}
void ToggleLED_b(){
  PTP ^= 0x80;          // debugging monitor
	PTT_PTT1 ^= 1;
}
void ToggleLED_c(){
  PTP ^= 0x80;          // debugging monitor
	PTT_PTT2 ^= 1;
}
void ToggleLED_d(){
  PTP ^= 0x80;          // debugging monitor
	PTT_PTT5 ^= 1;
}
void ToggleLED_e(){
  PTP ^= 0x80;          // debugging monitor
	PTT_PTT3 ^= 1;
}
void ToggleLED_f(){
  PTP ^= 0x80;          // debugging monitor
	PTT_PTT0 ^= 1;
}
