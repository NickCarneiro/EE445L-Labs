// filename ******** main.C ************** 
// Busy-wait SCI0 driver for the 9S12DP512,
// Jonathan W. Valvano 8/10/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

// how to run programs that use the SCI port
// 1) connect SCI0 of 9S12DP512 to PC's COM port, 
//     set 9S12DP512 to LOAD mode
//     reset 9S12DP512
// 2) execute Project->Debug (compiles and downloads code to 9S12DP512)
// 3) quit MW debugger once programming complete 
//    quitting the debugger will release the COM port
// 4) start a terminal program (like HyperTerminal)
//    specify proper COM port, 115200 bits/sec, no flow control
// 5) set to RUN mode and hit reset
// 6) when done, quit terminal program
//    quitting the terminal program will release the COM port
//     set 9S12DP512 to LOAD mode
//     reset 9S12DP512
