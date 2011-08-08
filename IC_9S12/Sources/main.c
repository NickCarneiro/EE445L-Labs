/* ************************ Main.c *****************************
 * AGM1264 example
 * ************************************************************ */
 
// 9S12DP512
// Jonathan W. Valvano 11/20/09 

// Copyright 2009 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

// Input on PAD4, from slide pot connected to to +5 and ground
// heartbeats, PP7 every 3000, PP5 at sampling rate

// AGM1264 graphics interface
// gnd    =  1- AGM1264F ground
// +5V    =  2- AGM1264F Vcc (with 0.1uF cap to ground)
// pot    =  3- AGM1264F Vo (center pin of 10k pot)
// PP2    =  4- AGM1264F D/I (0 for command, 1 for data)
// gnd    =  5- AGM1264F R/W (blind cycle synchronization)
// PP3    =  6- AGM1264F E   (1 to latch in data/command)
// PH0    =  7- AGM1264F DB0
// PH1    =  8- AGM1264F DB1
// PH2    =  9- AGM1264F DB2
// PH3    = 10- AGM1264F DB3
// PH4    = 11- AGM1264F DB4
// PH5    = 12- AGM1264F DB5
// PH6    = 13- AGM1264F DB6
// PH7    = 14- AGM1264F DB7
// PP0    = 15- AGM1264F CS1 (controls left half of LCD)
// PP1    = 16- AGM1264F CS2 (controls right half of LCD)
// +5V    = 17- AGM1264F RES (reset)
// pot    = 18- ADM1264F Vee (-10V)
// 10k pot from pin 18 to ground, with center to pin 3
// references   http://www.azdisplays.com/prod/g1264f.php
// sample code  http://www.azdisplays.com/PDF/agm1264f_code.pdf
// data sheet   http://www.azdisplays.com/PDF/agm1264f.pdf


// TCNT will become 1.5 MHz, calling LCD_Init, which calls Timer_Init

//Game mechanics based on the Allegro pong implementation written by Remi Bernotavicius
//http://blog.kunugiken.com/?p=66

#include <hidef.h>      /* common defines and macros */
#include <mc9s12c32.h>     /* derivative information */
#include <math.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12c32"
#include "PLL.H"

#include "PLL.h"
#include "adc.h"
#include "LCDG.h"
#include "Timer.h"

#define M_PI 3.141592654
#define UP 0
#define CENTER 1
#define DOWN 2
#define PADDLE_HEIGHT 16
#define paddle_width 4
#define BALL_SPEED 100
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64
#define paddle_speed 1
#define ball_size 4
#define FPS 20

//states
#define ST_MENU 0
#define ST_START 1
#define ST_PLAY 2

//sample 60 times per second
#define SAMPLING_FREQUENCY 60 

unsigned short ADCsample;	// ADC sample, 0 to 1023
unsigned short Voltage;   // 0.01 volts, 0 to 500
unsigned short ADCcount;  // 0 to 2999
unsigned short StartTime; // in seconds

unsigned short interruptRate = 1500000/SAMPLING_FREQUENCY;
//coordinate system starts at (0,0) in upper left corner
short ball_x = 0;
short ball_y = 0;
short ball_x_vel = 0;
short ball_y_vel = 0;
short start_x_vel = 2;
short start_y_vel =2;


short left_paddle_y = 0;
short right_paddle_y = 0;

short score_left = 0;
short score_right = 0;


char Start_Button;
char Select_Button;

unsigned char screen_buffer[1024] ={0};
char state = ST_MENU; //initialize game state

int powi(int base, int exponent){
    int i = 0;
    int result = 1;
    for(i = 0; i < exponent; i++){
        result *= base;    
    }
    return result;
}


void clearBuffer(){
  int i;
  for(i = 0; i < 1024; i++){
    screen_buffer[i] = 0x00;  
  }
  
}  
         
void updatePaddles(){
    unsigned short left;
    unsigned short right;
    left = ADC_In(0);
    right = ADC_In(1);
    
    //convert to 0-48
    left_paddle_y = left * 48 / 1003; 
    right_paddle_y = right * 48 / 1003;
           
}


void drawLeftPaddle(){
    unsigned short result1 = 0;
    unsigned short result2 = 0;
    unsigned short result3 = 0;
    unsigned short i = 0;
    unsigned short dest1 = 0;
    unsigned short dest2 = 0;
    unsigned short dest3 = 0;
    unsigned short width = 4;
    //left paddle
    if(left_paddle_y > 56){
       // left_paddle_y = 56;    
    }
    
    //contents of first bucket
    for(i = left_paddle_y % 8; i < 8; i++){
        result1 += powi(2,i);
    }
    
    //contents second bucket
    for(i = 0; i < 8; i++){
        result2 += powi(2,i);    
    }
    //contents of third bucket
    for(i = 0; i < left_paddle_y % 8; i++){
        result3 += powi(2,i);
    }
    
    //determine location of first bucket
    dest1 = (left_paddle_y / 8) * 64;
    dest2 = dest1 + 64;
    dest3 = dest2 + 64;
    for(i = 0; i < width; i++){
      screen_buffer[dest1 + i] = result1;
      screen_buffer[dest2 + i] = result2;
      screen_buffer[dest3 + i] = result3;
    }
    
    if(result2 > 0){
        //if left_paddle_y % 8 is zero, only one page is used
        
    } 
    
}


void drawRightPaddle(){
    unsigned short result1 = 0;
    unsigned short result2 = 0;
    unsigned short result3 = 0;
    unsigned short i = 0;
    unsigned short dest1 = 0;
    unsigned short dest2 = 0;
    unsigned short dest3 = 0;
    unsigned short width = 4;
    
    
    //contents of first bucket
    for(i = right_paddle_y % 8; i < 8; i++){
        result1 += powi(2,i);
    }
    
    //contents second bucket
    for(i = 0; i < 8; i++){
        result2 += powi(2,i);    
    }
    //contents of third bucket
    for(i = 0; i < right_paddle_y % 8; i++){
        result3 += powi(2,i);
    }
    
    //determine location of first bucket
    dest1 = (right_paddle_y / 8) * 64 + 572;
    dest2 = dest1 + 64;
    dest3 = dest2 + 64;
    for(i = 0; i < width; i++){
      screen_buffer[dest1 + i] = result1;
      screen_buffer[dest2 + i] = result2;
      screen_buffer[dest3 + i] = result3;
    }
    
    if(result2 > 0){
        //if left_paddle_y % 8 is zero, only one page is used
        
    } 
    
}

void drawBall(){
  unsigned short dest = 0;
  unsigned short result = 0;
  result = powi(2,ball_y % 8);
  if(ball_x < 64){
    dest = ball_y / 8 * 64 + ball_x;
  } else {
    dest = ball_y /8 * 64 + ball_x + 512 - 64;
  }
  
  screen_buffer[dest] = result;
}

void updateBall(){

  //ball hit left paddle
  if(ball_x < 4 && (left_paddle_y <= ball_y && left_paddle_y + PADDLE_HEIGHT >= ball_y)){
      //negative x vel
      ball_x_vel = -1 * ball_x_vel; 
  } else if(ball_x > 122 && (right_paddle_y <= ball_y && right_paddle_y + PADDLE_HEIGHT >= ball_y)){
  //ball hit right paddle
    //negative x vel
    ball_x_vel = -1 * ball_x_vel;
  } else if(ball_x <4){
  //ball hit left wall
    score_right++;
    LCD_GoTo(4,5);
    LCD_OutString("Player 2 Scores!");
    Timer_Wait1ms(1000);  
    ball_x = 63;
    ball_y = 31;
    ball_x_vel = start_x_vel;
    ball_y_vel = start_x_vel; 
  
  
  } else if(ball_x > 122){
   //ball hit right wall
    score_left++;
    LCD_GoTo(4,5);
    LCD_OutString("Player 1 Scores!");
    Timer_Wait1ms(1000);
    
    ball_x = 63;
    ball_y = 31;
    ball_x_vel = -1 * start_x_vel;
    ball_y_vel = start_y_vel; 
  } else if(ball_y <= 0){
   //ball hit top
   //negative y vel
   ball_y_vel = -1 * ball_y_vel;
  } else if(ball_y >= 63){
   //ball hit bottom
   ball_y_vel = -1 * ball_y_vel;
  }
  
  
  
  
  
  
  //apply velocity
  ball_x += ball_x_vel;
  ball_y += ball_y_vel;
}

void drawScore(){
  LCD_GoTo(1,3);
  LCD_OutDec(score_left);
  
  LCD_GoTo(1,10);
  LCD_OutDec(score_right);
}
 void main(void){
 
    
    
    
    //start ball in middle
    ball_x = 63;
    ball_y = 31;
    ball_x_vel = start_x_vel;
    ball_y_vel = start_y_vel;
    
    PLL_Init();
    LCD_Init();
    ADC_Init();
    DDRP &= ~(0xC0);
    
    LCD_Clear(0);
    LCD_GoTo(1,3);
    LCD_OutString("#Distortion Pong#");
    LCD_GoTo(3,4);
    LCD_OutString("Tim Osborne");
    LCD_GoTo(4,3);
    LCD_OutString("Nick Carneiro");
    LCD_GoTo(6,1);
    LCD_OutString("Press RED to start");
    
    while(PTP_PTP6 ==0){
    }
    LCD_Clear(0);
    LCD_GoTo(4,1);
    LCD_OutString("Starting...");
    Timer_Wait1ms(1000);
    while (1){ 
      updatePaddles(); //read in paddle values from lcd and update state variables
       
      //draw paddles, ball, score, render to lcd
       //draw paddles
      drawLeftPaddle();
      drawRightPaddle();
      
      
      //draw ball
      //find new position
      updateBall();
      drawBall();
      
                     
  
      
      
      //render buffer to lcd
      LCD_DrawImage(screen_buffer);
      drawScore();
      clearBuffer();

  //check for winner
  if(score_left > 9){
      LCD_Clear(0);
      LCD_GoTo(4,3);
      LCD_OutString("Player 1 wins!");
      
      score_left = 0;
      score_right = 0;
      ball_x = 63;
      ball_y = 31;
      LCD_GoTo(5,1);
      LCD_OutString("Press RED for rematch");
      
      while(PTP_PTP6 == 0){
      }
    } else if(score_right > 9){
      LCD_Clear(0);
      LCD_GoTo(4,3);
      LCD_OutString("Player 2 wins!");
      
      score_left = 0;
      score_right = 0;
      ball_x = 63;
      ball_y = 31;
      LCD_GoTo(5,1);
      LCD_OutString("Press RED for rematch");
      while(PTP_PTP6 == 0){
      }
    }
     
     //check for pause
     if(PTP_PTP7 == 0){
      while(PTP_PTP6 == 0){
      }
     }
      Timer_Wait1ms(1000/FPS); //20 fps
      
     
    }
     
}
 



        
            
                

                
         
        
    

