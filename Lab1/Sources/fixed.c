//******FIXED.C********
//contains implementations of functions that output fixed point numbers
//with different precisions and resolutions

#include "fixed.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char buffer[11];

//---------------------Fixed_uDecOut2--------------------- 
// create a fixed point number string with precision=65535
// and resolution=1/100, ranging from 0 to 65534
// Input: number contains an unsigned short to be converted into fixed point   
// Output: pointer to the beginning of a string containing the fixed point number
char* Fixed_uDecOut2(unsigned short number){
  unsigned short dec_part, frac_part;        //hold decimal part and fractional part
  
  if (number > 65534){                       //error condition
    return "***.**"; 
  }
  
  dec_part = number / 100;                      //calculate decimal part (quotient)
  frac_part = number % 100;                     //calculate fractional part (remainder)
  sprintf(buffer,"%d.%.2d",dec_part,frac_part); //put together fixed point number string
  return buffer;
}

//---------------------Fixed_sDecOut3--------------------- 
// create a fixed point number string with precision=19999
// and resolution=1/1000, ranging from -9999 to 9999
// Input: number contains an unsigned short to be converted into fixed point   
// Output: pointer to the beginning of a string containing the fixed point number
char* Fixed_sDecOut3(signed short number){
  unsigned short dec_part, frac_part;        //hold decimal and fractional parts
  
  if((number < -9999) || (number > 9999)){   //error conditions
    return "*.***";
  }
  
  dec_part = abs(number) / 1000;                    //calculate decimal part (quotient)
  frac_part = abs((int) number) % 1000;             //calculate fractional part (remainder)
  if ( number < 0 ){
    sprintf(buffer,"-%d.%.3d",dec_part, frac_part); //negative number case
  }else{
    sprintf(buffer,"%d.%.3d",dec_part, frac_part);  //positive number case  
  }
  return buffer;
}

//---------------------Fixed_uBinOut8--------------------- 
// create a fixed point number string with precision=65535
// and resolution=1/256, ranging from 0 to 255.99
// Input: number contains an unsigned short to be converted into fixed point   
// Output: pointer to the beginning of a string containing the fixed point number
char* Fixed_uBinOut8(unsigned short number){
  unsigned short dec_part, frac_part;         //hold decimal and fractional parts
  
  if(number > 65534){                         //error condition
    return "***.**";
  }
  
  dec_part = (number / 256);                      //calculate decimal part (quotient)
  frac_part = ((number % 256) * 100) / 256;       //calculate fractional part (remainder)
  sprintf(buffer,"%d.%.2d",dec_part, frac_part);  //put together fixed point number string
  
  return buffer;
}


