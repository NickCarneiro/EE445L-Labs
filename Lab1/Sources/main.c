//******Main Program*******
//Used to test the fixed.c functions and verify
//proper functionality sending output to lcd
//Authors: Aaron Alaniz (aa28877), Denis Sokolov (dks574)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MC9S12DP512.h>
#include "fixed.h"
#include "lcd.h"

#define NUMUDECTESTS 11
#define NUMSDECTESTS 11
#define NUMUBINTESTS 13

void PLL_Init(void);
                                            
//Data used to verify uDecOut2 and uBinOut8 in fixed.c
typedef const struct {
  unsigned short test_input;
  char *test_output;
}uFixed_C_Test;

//Data used to verify sDecOut3 in fixed.c
typedef const struct {
  signed short test_input;
  char *test_output;
}sFixed_C_Test;

//test data for Fixed_uDecOut2
uFixed_C_Test uDecOutTests[] = { 
  {0,"0.00"}, 
  {1,      "0.01"},
  {15,     "0.15"},
  {99,     "0.99"},
  {110,    "1.10"},
  {999,    "9.99"},
  {1200,   "12.00"},
  {9999,   "99.99"},
  {10000,  "100.00"},
  {65534,  "655.34"},
  {65535,  "***.**"}
};

//test data for Fixed_sDecOut3
sFixed_C_Test sDecOutTests[] = {
  {-32768,        "*.***"},
  {-10000,        "*.***"},
  {-9999,         "-9.999"},
  {-999,          "-0.999"},
  {-10,           "-0.010"},
  {-1,            "-0.001"},
  {0,             "0.000"},
  {123,           "0.123"},
  {1234,          "1.234"},
  {9999,          "9.999"},
  {32767,         "*.***"}
};

//test data for Fixed_uBinOut8
uFixed_C_Test uBinOut8Tests[] = {
  {0,           "0.00"},
  {4,           "0.01"},
  {10,          "0.03"},
  {200,         "0.78"},
  {254,         "0.99"},
  {505,         "1.97"},
  {1070,        "4.17"},
  {26000,       "101.56"},
  {32767,       "127.99"},
  {32768,       "128.00"},
  {34567,       "135.02"},
  {65534,       "255.99"},
  {65535,       "***.**"} 
};

unsigned short i;                //loop counter     
char *converted_input;  //holds fixed point output
  
int main(void){                                             
  
  letter_cnt = 0;   //init letter_cnt
  PLL_Init();
  LCD_Init();       //start the lcd and timer
  DDRT &= 0xFE;     //PT0 is a button input
  LCD_Clear();      //clears lcd display
    
  asm cli       
  for(i = 0;;){
    printf("uDecOut2 Tests");
    while (!(PTT & 0x01));   //wait for button push
    while(PTT & 0x01);       //wait for button release
    printf("\n");
    for(i = 0; i < NUMUDECTESTS; i++){
      converted_input = Fixed_uDecOut2(uDecOutTests[i].test_input);  //retrieve fixed point output
      printf("%u",uDecOutTests[i].test_input);
      LCD_GoTo(0x40);
      letter_cnt = 0;
      printf("%s",converted_input);
      while (!(PTT & 0x01));   //wait for button push
      while(PTT & 0x01);       //wait for button release
      LCD_Clear();             //clear lcd
    }
    printf("sDecOut3 Tests");
    while (!(PTT & 0x01));   //wait for button push
    while(PTT & 0x01);       //wait for button release
    LCD_Clear();    
    for(i = 0; i < NUMSDECTESTS; i++){
      converted_input = Fixed_sDecOut3(sDecOutTests[i].test_input);  //retrieve fixed point output
      printf("%d",sDecOutTests[i].test_input);
      LCD_GoTo(0x40);
      letter_cnt = 0;
      printf("%s",converted_input);
      while (!(PTT & 0x01));   //wait for button push
      while(PTT & 0x01);       //wait for button release
      LCD_Clear();             //clear lcd
    }
    printf("uBinOut8 Tests");
    while (!(PTT & 0x01));   //wait for button push
    while(PTT & 0x01);       //wait for button release
    LCD_Clear();
    for(i = 0; i < NUMUBINTESTS; i++){
      converted_input = Fixed_uBinOut8(uBinOut8Tests[i].test_input);  //retrieve fixed point output
      printf("%u",uBinOut8Tests[i].test_input);
      LCD_GoTo(0x40);
      letter_cnt = 0;
      printf("%s",converted_input);
      while (!(PTT & 0x01));   //wait for button push
      while(PTT & 0x01);       //wait for button release
      LCD_Clear();             //clear lcd
    }
    printf("\tend");   //tab demo
    while (!(PTT & 0x01));   //wait for button push
    while(PTT & 0x01);       //wait for button release
    LCD_Clear();
  }
  return 0;  
}


