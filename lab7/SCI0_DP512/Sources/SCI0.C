// filename  ***************  SCI0.C ****************************** 
// Simple I/O routines to 9S12DP512 serial port   
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
 
#include <mc9s12dp512.h>     /* derivative information */
#include "SCI0.h"
#define RDRF 0x20   // Receive Data Register Full Bit
#define TDRE 0x80   // Transmit Data Register Empty Bit

//-------------------------SCI0_Init------------------------
// Initialize Serial port SCI0
// Input: baudRate is tha baud rate in bits/sec
// Output: none
// assumes a module clock frequency of 24 MHz
void SCI0_Init(unsigned long baudRate){
unsigned long br;
  SCI0BDH = 0;   
  br=(1500000+baudRate/2)/baudRate; // MCLK/(16*baudRate) 
  SCI0BDL= (unsigned char) br; 
  SCI0CR1 = 0;
/* bit value meaning
    7   0    LOOPS, no looping, normal
    6   0    WOMS, normal high/low outputs
    5   0    RSRC, not appliable with LOOPS=0
    4   0    M, 1 start, 8 data, 1 stop
    3   0    WAKE, wake by idle (not applicable)
    2   0    ILT, short idle time (not applicable)
    1   0    PE, no parity
    0   0    PT, parity type (not applicable with PE=0) */ 
  SCI0CR2 = 0x0C; 
/* bit value meaning
    7   0    TIE, no transmit interrupts on TDRE
    6   0    TCIE, no transmit interrupts on TC
    5   0    RIE, no receive interrupts on RDRF
    4   0    ILIE, no interrupts on idle
    3   1    TE, enable transmitter
    2   1    RE, enable receiver
    1   0    RWU, no receiver wakeup
    0   0    SBK, no send break */ 
}
    
//-------------------------SCI0_InChar------------------------
// Wait for new serial port input, busy-waiting synchronization
// The input is not echoed
// Input: none
// Output: ASCII code for key typed
char SCI0_InChar(void){
  while((SCI0SR1 & RDRF) == 0){};
  return(SCI0DRL);
}
        
//-------------------------SCI0_OutChar------------------------
// Wait for buffer to be empty, output 8-bit to serial port
// busy-waiting synchronization
// Input: 8-bit data to be transferred
// Output: none
void SCI0_OutChar(char data){
  while((SCI0SR1 & TDRE) == 0){};
  SCI0DRL = data;
}

   
//-------------------------SCI0_InStatus--------------------------
// Checks if new input is ready, TRUE if new input is ready
// Input: none
// Output: TRUE if a call to InChar will return right away with data
//         FALSE if a call to InChar will wait for input
char SCI0_InStatus(void){
  return(SCI0SR1 & RDRF);
}

//-----------------------SCI0_OutStatus----------------------------
// Checks if output data buffer is empty, TRUE if empty
// Input: none
// Output: TRUE if a call to OutChar will output and return right away
//         FALSE if a call to OutChar will wait for output to be ready
char SCI0_OutStatus(void){
  return(SCI0SR1 & TDRE);
}


//-------------------------SCI0_OutString------------------------
// Output String (NULL termination), busy-waiting synchronization
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void SCI0_OutString(char *pt){
  while(*pt){
    SCI0_OutChar(*pt);
    pt++;
  }
}

//----------------------SCI0_InUDec-------------------------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 16 bit unsigned number
//     valid range is 0 to 65535
// Input: none
// Output: 16-bit unsigned number
// If you enter a number above 65535, it will truncate without an error
// Backspace will remove last digit typed
unsigned short SCI0_InUDec(void){	
unsigned short number=0, length=0;
char character;
  character = SCI0_InChar();	
  while(character != CR){ // accepts until <enter> is typed
// The next line checks that the input is a digit, 0-9.
// If the character is not 0-9, it is ignored and not echoed
    if((character>='0') && (character<='9')) {
      number = 10*number+(character-'0');   // this line overflows if above 65535
      length++;
      SCI0_OutChar(character);
    } 
// If the input is a backspace, then the return number is
// changed and a backspace is outputted to the screen
    else if((character==BS) && length){
      number /= 10;
      length--;
      SCI0_OutChar(character);
    }
    character = SCI0_InChar();	
  }
  return number;
}


//-----------------------SCI0_OutUDec-----------------------
// Output a 16-bit number in unsigned decimal format
// Input: 16-bit number to be transferred
// Output: none
// Variable format 1-5 digits with no space before or after
void SCI0_OutUDec(unsigned short n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string 
  if(n >= 10){
    SCI0_OutUDec(n/10);
    n = n%10;
  }
  SCI0_OutChar(n+'0'); /* n is between 0 and 9 */
}



//---------------------SCI0_InUHex----------------------------------------
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 16-bit unsigned number
// No '$' or '0x' need be entered, just the 1 to 4 hex digits
// It will convert lower case a-f to uppercase A-F
//     and converts to a 16 bit unsigned number
//     value range is 0 to FFFF
// If you enter a number above FFFF, it will truncate without an error
// Backspace will remove last digit typed
unsigned short SCI0_InUHex(void){	
unsigned short number=0, digit, length=0;
char character;
  character = SCI0_InChar();
  while(character != CR){	
    digit = 0x10; // assume bad
    if((character>='0') && (character<='9')){
      digit = character-'0';
    }
    else if((character>='A') && (character<='F')){ 
      digit = (character-'A')+0xA;
    }
    else if((character>='a') && (character<='f')){ 
      digit = (character-'a')+0xA;
    }
// If the character is not 0-9 or A-F, it is ignored and not echoed
    if(digit <= 0xF){	
      number = number*0x10+digit;
      length++;
      SCI0_OutChar(character);
    }
// Backspace outputted and return value changed if a backspace is inputted
    else if((character==BS) && length){
      number /= 0x10;
      length--;
      SCI0_OutChar(character);
    }
    character = SCI0_InChar();
  }
  return number;
}

//--------------------------SCI0_OutUHex----------------------------
// Output a 16 bit number in unsigned hexadecimal format
// Input: 16-bit number to be transferred
// Output: none
// Variable format 1 to 4 digits with no space before or after
void SCI0_OutUHex(unsigned short number){
// This function uses recursion to convert the number of 
//   unspecified length as an ASCII string
  if(number >= 0x10){
    SCI0_OutUHex(number/0x10);
    SCI0_OutUHex(number%0x10);
  }
  else{
    if(number < 0xA){
      SCI0_OutChar(number+'0');
     }
    else{
      SCI0_OutChar((number-0x0A)+'A');
    }
  }
}

//------------------------SCI0_InString------------------------
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
// -- Modified by Agustinus Darmawan + Mingjie Qiu --
void SCI0_InString(char *bufPt, unsigned short max) {	
int length=0;
char character;
  character = SCI0_InChar();
  while(character != CR){
    if(character == BS){
      if(length){
        bufPt--;
        length--;
        SCI0_OutChar(BS);
      }
    }
    else if(length < max){
      *bufPt = character;
      bufPt++;
      length++; 
      SCI0_OutChar(character);
    }
    character = SCI0_InChar();
  }
  *bufPt = 0;
}



