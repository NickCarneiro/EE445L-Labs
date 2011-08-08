//******LED toggle Functions******
//Purpose(all): Toggle specified LED in ring. Accessed via function pointer.
//Input: none
//Output: toggles desired LED 	

#include "light_ring.h"

void ToggleLED_top(){
  //PTP ^= 0x04;          // debugging monitor
	PORTB ^= 0x10;  //PB4
}
void ToggleLED_b(){
  //PTP ^= 0x04;          // debugging monitor
	PORTB ^= 0x20;  //PB5
}
void ToggleLED_c(){
  //PTP ^= 0x04;          // debugging monitor
	PORTA ^= 0x01;  //PA0
}
void ToggleLED_d(){
  //PTP ^= 0x04;          // debugging monitor
	PORTB ^= 0x80;  //PB7
}
void ToggleLED_e(){
  //PTP ^= 0x04;          // debugging monitor
	PTT_PTT0 ^= 1;  //PT0
}
void ToggleLED_f(){
  //PTP ^= 0x04;          // debugging monitor
	PTT_PTT3 ^= 1;  //PT3
}
void ToggleLED_g(){
  //PTP ^= 0x04;          // debugging monitor
	PTT_PTT4 ^= 1;  //PT4
}
void ToggleLED_h(){
  //PTP ^= 0x04;          // debugging monitor
	PTT_PTT6 ^= 1;   //PT6
}
void ToggleLED_i(){
  //PTP ^= 0x04;          // debugging monitor
	PTT_PTT5 ^= 1;  //PT5
}
void ToggleLED_j(){
  //PTP ^= 0x04;          // debugging monitor
	PTT_PTT7 ^= 1;  //PT7
}
void ToggleLED_k(){
  //PTP ^= 0x04;          // debugging monitor
	PTP_PTP2 ^= 1;  //PP2
}
void ToggleLED_l(){
  //PTP ^= 0x04;          // debugging monitor
	PTP_PTP1 ^= 1;  //PP1
}
void ToggleLED_m(){
  //PTP ^= 0x04;          // debugging monitor
	PTP_PTP0 ^= 1;  //PP0
}
void ToggleLED_n(){
  //PTP ^= 0x04;          // debugging monitor
	PORTA ^= 0x80;  //PA7
}
void ToggleLED_o(){
  //PTP ^= 0x04;          // debugging monitor
	PTP_PTP7 ^= 1;  //PP7
}
void ToggleLED_p(){
  //PTP ^= 0x04;          // debugging monitor
	PORTB ^= 0x01;  //PB0
}
void ToggleLED_q(){
  //PTP ^= 0x04;          // debugging monitor
	PORTB ^= 0x04;  //PB2
}
void ToggleLED_r(){
  //PTP ^= 0x04;          // debugging monitor
	PORTB ^= 0x08;  //PB3
}

