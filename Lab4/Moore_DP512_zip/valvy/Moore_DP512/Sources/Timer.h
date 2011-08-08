// filename ******** Timer.h ************** 
// TCNT increments at a fixed rate
// Jonathan W. Valvano 10/11/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
// You may use, edit, run or distribute this file 
//    as long as the above copyright notices remain

//---------Timer_Init------------------
// activate TCNT timer at 1.5 MHz,667ns cycles
// inputs: none 
// outputs: none
void Timer_Init(void);

//---------Timer_Wait------------------
// fixed time delay
// inputs: time to wait in 667ns cycles
// outputs: none
void Timer_Wait(unsigned short time);

//---------Timer_Wait1ms------------------
// fixed time delay
// inputs: time to wait in ms
// outputs: none
// 1500 cycles equals 1ms
void Timer_Wait1ms(unsigned short delay);

//---------Timer_Wait10ms------------------
// fixed time delay
// inputs: time to wait in 10ms
// outputs: none
// 15000 cycles equals 10ms
void Timer_Wait10ms(unsigned short delay);
