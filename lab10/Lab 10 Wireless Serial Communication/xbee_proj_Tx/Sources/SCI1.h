// filename  ******************* SCI1.H **************************
// Jonathan W. Valvano 8/10/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

// Modified by EE345L students Charlie Gough && Matt Hawk
// Modified by EE345M students Agustinus Darmawan + Mingjie Qiu
 
// standard ASCII symbols 
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20       
#define DEL  0x7F
 
//-------------------------SCI1_Init------------------------
// Initialize Serial port SCI
// Input: baudRate is tha baud rate in bits/sec
// Output: none
// assumes a module clock frequency of 24 MHz
void SCI1_Init(unsigned long baudRate);
 
//-------------------------SCI1_InStatus--------------------------
// Checks if new input is ready, TRUE if new input is ready
// Input: none
// Output: TRUE if a call to InChar will return right away with data
//         FALSE if a call to InChar will wait for input
char SCI1_InStatus(void);  

//-------------------------SCI1_InChar------------------------
// Wait for new serial port input, busy-waiting synchronization
// The input is not echoed
// Input: none
// Output: ASCII code for key typed
char SCI1_InChar(void);

//------------------------SCI1_InString------------------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until <enter> is typed 
//    or until max length of the string is reached.  
// It echoes each character as it is inputted.  
// If a backspace is inputted, the string is modified 
//    and the backspace is echoed
// terminates the string with a null character
// uses busy-waiting synchronization on RDRF
// Input: pointer to empty buffer, size of buffer
// Output: Null terminated string
void SCI1_InString(char *bufPt, unsigned short max);
 

//----------------------SCI1_InUDec-------------------------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 16 bit unsigned number
//     valid range is 0 to 65535
// Input: none
// Output: 16-bit unsigned number
// If you enter a number above 65535, it will truncate without an error
// Backspace will remove last digit typed
unsigned short SCI1_InUDec(void);   

//---------------------SCI1_InUHex----------------------------------------
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 16-bit unsigned number
// No '$' or '0x' need be entered, just the 1 to 4 hex digits
// It will convert lower case a-f to uppercase A-F
//     and converts to a 16 bit unsigned number
//     value range is 0 to FFFF
// If you enter a number above FFFF, it will truncate without an error
// Backspace will remove last digit typed
unsigned short SCI1_InUHex(void);  

//-----------------------SCI1_OutStatus----------------------------
// Checks if output data buffer is empty, TRUE if empty
// Input: none
// Output: TRUE if a call to OutChar will output and return right away
//         FALSE if a call to OutChar will wait for output to be ready
char SCI1_OutStatus(void);   

//-------------------------SCI1_OutChar------------------------
// Wait for buffer to be empty, output 8-bit to serial port
// busy-waiting synchronization
// Input: 8-bit data to be transferred
// Output: none
void SCI1_OutChar(char);  
 
//-----------------------SCI1_OutUDec-----------------------
// Output a 16-bit number in unsigned decimal format
// Input: 16-bit number to be transferred
// Output: none
// Variable format 1-5 digits with no space before or after
void SCI1_OutUDec(unsigned short);    

//-------------------------SCI1_OutString------------------------
// Output String (NULL termination), busy-waiting synchronization
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void SCI1_OutString(char *pt); 
 
//--------------------------SCI1_OutUHex----------------------------
// Output a 16 bit number in unsigned hexadecimal format
// Input: 16-bit number to be transferred
// Output: none
// Variable format 1 to 4 digits with no space before or after
void SCI1_OutUHex(unsigned short); 

