// filename ************** SCIb.H *****************************
// Interrupting I/O routines to 9S12DP512 virtual serial port 

// 9S12DP512 PT2 is RxD (input to 9S12DP512) 
// 9S12DP512 PT3 is TxD (output from 9S12DP512)
// 9S12DP512 channel 4 output compare interrupt used 
// baud rate can be from 1000 to 50000 bits/sec
// 1 start bit, 8 bit data, 1 stop
// Jonathan W. Valvano 11/12/09

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thompson, copyright (c) 2006,
//   "Introduction to Embedded Systems: Interfacing to the Freescale 9S12",
//        Cengage Publishing 2009, ISBN-10: 049541137X | ISBN-13: 9780495411376

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

// standard ASCII symbols 
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20       
#define DEL  0x7F
 
//-------------------------SCIb_Init------------------------
// Initialize virtual Serial port SCI
// baudrate can be from 1000 to 38400 bits/sec
// 1 start, 8 data, 1 stop bit
// Input: none
// Output: none
// assumes a module clock frequency of 24 MHz
void SCIb_Init(unsigned short baudrate);
 
//-------------------------SCIb_Clear------------------------
// Initialize buffers, halt activity
// Input: none
// Output: none
void SCIb_Clear(void);

//-------------------------SCIb_InStatus--------------------------
// Checks if new input is ready, TRUE if new input is ready
// Input: none
// Output: TRUE if a call to InChar will return right away with data
//         FALSE if a call to InChar will wait for input
int SCIb_InStatus(void);  

//-------------------------SCIb_InChar-----------------------
// Wait for new serial port input, interrupt synchronization
// Input: none
// Output: ASCII code for key typed
unsigned char SCIb_InChar(void);

//-------------------------SCIb_InChar2------------------------
// Wait for new serial port input, interrupt synchronization
// Input: none
// Output: ASCII code for key typed
// Worst case response time is 20 ms
// so it returns with 0xFE if no response in 20 ms
unsigned char SCIb_InChar2(void); 


//-----------------------SCIb_OutStatus----------------------------
// Checks if output data buffer is empty, TRUE if empty
// Input: none
// Output: TRUE if a call to OutChar will output and return right away
//         FALSE if a call to OutChar will wait for output to be ready
int SCIb_OutStatus(void);   

//-------------------------SCIb_OutChar------------------------
// Wait for buffer to be empty, output 8-bit to serial port
// interrupt synchronization
// Input: 8-bit data to be transferred
// Output: none
void SCIb_OutChar(unsigned char);
   

//-------------------------SCIb_OutString------------------------
// Output String (NULL termination), interrupt synchronization
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void SCIb_OutString(char *pt);


//----------------------SCIb_InUDec-------------------------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 16 bit unsigned number
//     valid range is 0 to 65535
// Input: none
// Output: 16-bit unsigned number
// If you enter a number above 65535, it will truncate without an error
// Backspace will remove last digit typed
unsigned short SCIb_InUDec(void);

//-----------------------SCIb_OutUDec-----------------------
// Output a 16-bit number in unsigned decimal format
// Input: 16-bit number to be transferred
// Output: none
// Variable format 1-5 digits with no space before or after
void SCIb_OutUDec(unsigned short n);


//---------------------SCIb_InUHex----------------------------------------
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 16-bit unsigned number
// No '$' or '0x' need be entered, just the 1 to 4 hex digits
// It will convert lower case a-f to uppercase A-F
//     and converts to a 16 bit unsigned number
//     value range is 0 to FFFF
// If you enter a number above FFFF, it will truncate without an error
// Backspace will remove last digit typed
unsigned short SCIb_InUHex(void);

//--------------------------SCIb_OutUHex----------------------------
// Output a 16 bit number in unsigned hexadecimal format
// Input: 16-bit number to be transferred
// Output: none
// Variable format 1 to 4 digits with no space before or after
void SCIb_OutUHex(unsigned short number);

//------------------------SCIb_InString------------------------
// This function accepts ASCII characters from the serial port
//    and adds them to a string until a carriage return is inputted 
//    or until max length of the string is reached.  
// It echoes each character as it is inputted.  
// If a backspace is inputted, the string is modified 
//    and the backspace is echoed
// InString terminates the string with a null character
// -- Modified by Agustinus Darmawan + Mingjie Qiu --
void SCIb_InString(char *string, unsigned short max) ; 

