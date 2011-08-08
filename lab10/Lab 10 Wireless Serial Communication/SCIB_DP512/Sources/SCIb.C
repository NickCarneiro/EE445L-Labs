// filename ************** SCIb.C *****************************
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

#include "derivative.h"      /* derivative-specific definitions */
#include "SCIb.h"
#include "RxFifob.h"  
#include "TxFifob.h" 
 
// Tx interrupt rate at the baud rate
// Rx interrupt rate at the baud rate
// change the following two defines in order to adjust baud rate
unsigned short BitTime;     // 24000000/baudrate
unsigned short HalfBitTime; // 24000000/baudrate/2
// E.g., for 9600, BitTime=2500 

static unsigned short TxCount,TxData; // transmit shift register
static unsigned char RxCount,RxData;  // receive shift register
unsigned char RxFull,NoStart,NoStop;  // number of receiver errors

//-------------------------SCIb_Init------------------------
// Initialize virtual Serial port SCI
// baudrate can be from 1000 to 50000 bits/sec
// 1 start, 8 data, 1 stop bit
// Input: none
// Output: none
// assumes a module clock frequency of 24 MHz
void SCIb_Init(unsigned short baudrate){
  RxFifob_Init();
  TxFifob_Init();
  BitTime = (unsigned short)(24000000L/(unsigned long)baudrate);
  HalfBitTime = BitTime/2;
  DDRT |= 0x08;     // PT3 is TxD output
  DDRT &= ~0x04;    // PT2 RxD input
  PERT |= 0x04;     // internal pullup on PT2
  PTT  |= 0x08;     // PT3 high TxD idle
  RxFull = 0; NoStart = 0; NoStop = 0; 
  RxCount =0;       // means receiver is idle
  TxCount =0;       // means transmitter is idle
  TIOS  &= ~0x04;   // activate TC2 as input capture
  TIOS  |= 0x18;    // activate TC3,TC4 as output compares
  TSCR1 = 0x80;     // Enable TCNT, 24MHz 
  TSCR2 = 0x00;     // divide by 1 TCNT prescale, TOI disarm
  PACTL = 0;        // timer prescale used for TCNT
  TCTL4 = 0x20;     // falling edge IC2
  TIE  |= 0x0C;     // arm OC3, IC2
  TC3   = TCNT+50;  // first interrupt right away
  asm cli   /* enable interrupts */
}

//-------------------------SCIb_Clear------------------------
// Initialize buffers, halt activity
// Input: none
// Output: none
void SCIb_Clear(void){
  RxFifob_Init();
  TxFifob_Init();
  PTT  |= 0x08;     // PT3 high TxD idle
  RxFull = 0; NoStart = 0; NoStop = 0; 
  RxCount =0;       // means receiver is idle
  TxCount =0;       // means transmitter is idle
}  
  
//-------------------------SCIb_InChar-----------------------
// Wait for new serial port input, interrupt synchronization
// Input: none
// Output: ASCII code for key typed
unsigned char SCIb_InChar(void){ 
unsigned char letter;
  while (RxFifob_Get(&letter) == 0){};
  return(letter);
}

//-------------------------SCIb_InChar2------------------------
// Wait for new serial port input, interrupt synchronization
// Input: none
// Output: ASCII code for key typed
// Worst case response time is 20 ms
// so it returns with 0xFE if no response in 20 ms
unsigned char SCIb_InChar2(void){ 
unsigned char letter;
short TimeLeft,EndT;
unsigned short timeOut=2000; // 20ms
  while (RxFifob_Get(&letter) == 0){
    EndT = TCNT+240;       // 240 counts (42ns cycles) in 10us 
    do{
      TimeLeft = (short)TCNT-EndT;
    }
    while((TimeLeft<0)||(TimeLeft>100));   // wait 10us
    timeOut--;
    if(timeOut == 0){
      return(0xFE);
    }
  }
  return(letter);

}


//-------------------------SCIb_OutChar------------------------
// Wait for buffer to be empty, output 8-bit to serial port
// interrupt synchronization
// Input: 8-bit data to be transferred
// Output: none
void SCIb_OutChar(unsigned char data){
  while (TxFifob_Put(data) == 0){}; // spin if TxFifo is full
}
    
//-------------------------SCIb_InStatus--------------------------
// Checks if new input is ready, TRUE if new input is ready
// Input: none
// Output: TRUE if a call to InChar will return right away with data
//         FALSE if a call to InChar will wait for input
int SCIb_InStatus(void){  
  return(RxFifob_Status());
}

//-----------------------SCIb_OutStatus----------------------------
// Checks if there is room in the FIFO, 
// TRUE if a call to OutChar will not spin
int SCIb_OutStatus(void){
  return(TxFifob_Status());
}

/*----------------Input Capture 2 Handler---------------*/
// used to detect start bit on RxD input
// PT2=RxD input
// ______			 ____ ____ ____ ____ ____ ____ ____ ____ ______
//       |  S | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | Stop  
//       |____|____|____|____|____|____|____|____|____|
// 			
// IC2   ^
// OC4      ^    ^    ^    ^    ^    ^    ^    ^    ^    ^
interrupt 10 void T2Handler(void){
  TC4 = TC2+HalfBitTime;   // 1/2 bit time later
  TIE  |= 0x10;            // arm OC4
  TIE  &= ~0x04;           // disarm IC2
  RxCount = 10;            // 10 bits/frame
  TFLG1 = 0x14;            // acknowledge OC4,IC2
}

/*----------------Output Compare 3 Handler---------------*/
// used to output virtual TxData
// PT3=TxD output
// ______			 ____ ____ ____ ____ ____ ____ ____ ____ ______
//       |  S | 0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | Stop  
//       |____|____|____|____|____|____|____|____|____|
// 			
// OC3   ^    ^    ^    ^    ^    ^    ^    ^    ^    ^
interrupt 11 void T3Handler(void){  char data;
  TFLG1 = 0x08;         // acknowledge OC3
  TC3 = TC3+BitTime;    
  if(TxCount==0){
    if(TxFifob_Get(&data)){
      TxData = 0x100+(0xFF&data); // one stop bit+data
      PTT &= ~0x08;    // start bit
      TxCount = 9;
    }
  }
  else {
    if(TxData&0x0001){
      PTT |= 0x08;  // data bit high
    } else{
      PTT &= ~0x08; // data bit low
    }
    TxData = TxData>>1;
    TxCount--;
  }
}
/*----------------Output Compare 4 Handler---------------*/
// used to input virtual RxData
interrupt 12 void T4Handler(void){ 
  TC4 = TC4+BitTime;    
  RxCount--;
  if(RxCount==9){       // check start bit
    if(PTT&0x04){
      NoStart++;        // framing error, no start
    }
  }
  else{
    if(RxCount){	 // RxCount is 8,7,6,5,4,3,2,1
      RxData = RxData >>1;
      if(PTT&0x04){
        RxData |= 0x80;	 // data bit is high
      }
    } 
    else{				        // RxCount is 0
      if((PTT&0x04)==0){
        NoStop++;       // framing error, no stop
      }
      if(RxFifob_Put(RxData)==0){
        PTT &= ~0x01;   // PT0 LED on means lost data
        RxFull++;       // RxFifo full
      }
      TIE  |= 0x04;     // arm IC2
      TIE  &= ~0x10;    // disarm OC4
		}
	}

  TFLG1 = 0x14;         // acknowledge IC2,OC4

}




//-------------------------SCIb_OutString------------------------
// Output String (NULL termination), interrupt synchronization
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void SCIb_OutString(char *pt){
  while(*pt){
    SCIb_OutChar(*pt);
    pt++;
  }
}


//----------------------SCIb_InUDec-------------------------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 16 bit unsigned number
//     valid range is 0 to 65535
// Input: none
// Output: 16-bit unsigned number
// If you enter a number above 65535, it will truncate without an error
// Backspace will remove last digit typed
unsigned short SCIb_InUDec(void){	
unsigned short number=0, length=0;
char character;
  character = SCIb_InChar();	
  while(character!=CR){ // accepts until carriage return input
// The next line checks that the input is a digit, 0-9.
// If the character is not 0-9, it is ignored and not echoed
    if((character>='0') && (character<='9')) {
      number = 10*number+(character-'0');   // this line overflows if above 65535
      length++;
      SCIb_OutChar(character);
    } 
// If the input is a backspace, then the return number is
// changed and a backspace is outputted to the screen
    else if((character==BS) && length){
      number /= 10;
      length--;
      SCIb_OutChar(character);
    }
    character = SCIb_InChar();	
  }
  return number;
}



//-----------------------SCIb_OutUDec-----------------------
// Output a 16-bit number in unsigned decimal format
// Input: 16-bit number to be transferred
// Output: none
// Variable format 1-5 digits with no space before or after
void SCIb_OutUDec(unsigned short n){
// This function uses recursion to convert decimal number
//   of unspecified length as an ASCII string 
  if(n >= 10){
    SCIb_OutUDec(n/10);
    n = n%10;
  }
  SCIb_OutChar(n+'0'); /* n is between 0 and 9 */
}



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
unsigned short SCIb_InUHex(void){	
unsigned short number=0, digit, length=0;
char character;
  character = SCIb_InChar();
  while(character!=CR){	
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
    if(digit<=0xF ){	
      number = number*0x10+digit;
      length++;
      SCIb_OutChar(character);
    }
// Backspace outputted and return value changed if a backspace is inputted
    else if(character==BS && length){
      number /=0x10;
      length--;
      SCIb_OutChar(character);
    }
    character = SCIb_InChar();
  }
  return number;
}

//--------------------------SCIb_OutUHex----------------------------
// Output a 16 bit number in unsigned hexadecimal format
// Input: 16-bit number to be transferred
// Output: none
// Variable format 1 to 4 digits with no space before or after
void SCIb_OutUHex(unsigned short number){
// This function uses recursion to convert the number of 
//   unspecified length as an ASCII string
  if(number>=0x10)	{
    SCIb_OutUHex(number/0x10);
    SCIb_OutUHex(number%0x10);
  }
  else if(number<0xA){
    SCIb_OutChar(number+'0');
  }
  else{
    SCIb_OutChar((number-0x0A)+'A');
  }
}

//------------------------SCIb_InString------------------------
// This function accepts ASCII characters from the serial port
//    and adds them to a string until a carriage return is inputted 
//    or until max length of the string is reached.  
// It echoes each character as it is inputted.  
// If a backspace is inputted, the string is modified 
//    and the backspace is echoed
// InString terminates the string with a null character
// -- Modified by Agustinus Darmawan + Mingjie Qiu --
void SCIb_InString(char *string, unsigned short max) {	
int length=0;
char character;
  character = SCIb_InChar();
  while(character!=CR){
    if(character==BS){
      if(length){
        string--;
        length--;
        SCIb_OutChar(BS);
      }
    }
    else if(length<max){
      *string++=character;
      length++; 
      SCIb_OutChar(character);
    }
    character = SCIb_InChar();
  }
  *string = 0;
}


