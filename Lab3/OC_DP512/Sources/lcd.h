//*******LCD.H********
//LCD display on PTH for dp512 (lcd: hd44780)
//contains implementations of functions that 
//manipulate the lcd display

extern unsigned short letter_cnt; //counts letters output to lcd

//---------------------Timer_Wait---------------------
//time delay
//Input: time in 0.667usec increments
//Output: none
static void Timer_Wait(unsigned short delay);

//---------------------Timer_Wait_1ms---------------------
//time delay
//Input: time in msec increments
//Output: none
void Timer_Wait_1ms(unsigned short msec);

//---------------------outCsrNibble--------------------- 
//execute a 4-bit lcd command
//Input: command contains the 4-bit lcd command   
//Output: none
static void outCsrNibble(unsigned char command);

//---------------------outCsr--------------------- 
//executes an 8-bit lcd command via two intervalled calls to outCsrNibble
//Input: command contains the 8-bit lcd command   
//Output: none
static void outCsr(unsigned char command);

//---------------------LCD_Clear---------------------
//clear the LCD display, send cursor to home
//Input: none
//Output: true if successful
unsigned short LCD_Clear(void);

//---------------------LCD_OutChar--------------------- 
//outputs an ASCII character to the LCD display 
//Input: letter is the character to be output   
//Output: true if successful
unsigned short LCD_OutChar(unsigned char letter);

//---------------------LCD_OutString--------------------- 
//outputs an ASCII string to the LCD display 
//Input: string is a pointer to the beginning of the string to output   
//Output: true if successful
unsigned short LCD_OutString(char *string);

//---------------------TERMIO_PutChar--------------------- 
//sends one ASCII to the LCD display 
//Input: letter is ASCII code 
//handles at least two special characters, like CR LF or TAB  
//Output: true if successful
short TERMIO_PutChar(unsigned char letter);

//---------------------LCD_Init---------------------
//initialize the LCD display, called once at beginning
//Input: none
//Output: true if successful
unsigned short LCD_Init(void);

//---------------------LCD_Clear---------------------
//clear the LCD display, send cursor to home
//Input: none
//Output: true if successful
unsigned short LCD_Clear(void);

//---------------------readBusy--------------------- 
//checks to see if the lcd is busy with a process 
//Input: none   
//Output: true if busy and false if not busy
unsigned short readBusy(void);

//---------------------LCD_ErrorCheck--------------------- 
//check for time-out 
//Input: none   
//Output: error code
unsigned short LCD_ErrorCheck(void);
    
//---------------------LCD_GoTo--------------------- 
//moves cursor to desired location 
//Input: string is a pointer to the beginning of the string to output   
//Output: true if successful
unsigned short LCD_GoTo(unsigned short row, unsigned short column);    
     







