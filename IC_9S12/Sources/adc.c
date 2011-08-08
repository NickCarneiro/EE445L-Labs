/* ************************ adc.c *****************************
 * Simple I/O routines ADC port 
 * ************************************************************ */
 
// 9S12DP512
// Jonathan W. Valvano 7/14/07

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thomson Engineering, copyright (c) 2006,
//   "Introduction to Embedded Microcomputer Systems: 
//    Motorola 6811 and 6812 Simulation", Brooks-Cole, copyright (c) 2002

// Copyright 2007 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 

#include <mc9s12c32.h>     /* derivative information */
#include <hidef.h>      /* common defines and macros */
#include "ADC.h"

//******** ADC_In *************** 
// perform 10-bit analog to digital conversion
// input: chan is 0 to 7 specifying analog channel to sample
// output: 10-bit ADC sample (left justified) 
// analog input    left justified   right justified
//  0.000               0	               0
//  0.005            0040	               1
//  0.010            0080                2
//  1.250            4000              100
//  2.500            8000							 200
//  5.000            FFC0						   3FF
// uses busy-wait synchronization
// bit 7 DJM Result Register Data Justification
//       1=right justified, 0=left justified
// bit 6 DSGN Result Register Data Signed or Unsigned Representation
//       1=signed, 0=unsigned
// bit 5 SCAN Continuous Conversion Sequence Mode
//       1=continuous, 0=single
// bit 4 MULT Multi-Channel Sample Mode
//       1=multiple channel, 0=single channel
// bit 3 0
// bit 2-0 CC,CB,CA channel number 0 to 7
// example  data = ADC_In(0x82); // samples right-justified channel 2
unsigned short ADC_In(unsigned short chan){ 
  ATDCTL5 = (unsigned char)chan + 0x80;  // start sequence
  while((ATDSTAT1&0x01)==0){};    // wait for CCF0 
  return ATDDR0; 
}

//******** ADC_Init *************** 
// Initialize ADC, sequence length=1, 10-bit mode
// input: none
// output: none
// errors: none
void ADC_Init(void){
  ATDCTL2 = 0x80; // enable ADC
  // bit 7 ADPU=1 enable
  // bit 6 AFFC=0 ATD Fast Flag Clear All
  // bit 5 AWAI=0 ATD Power Down in Wait Mode
  // bit 4 ETRIGLE=0 External Trigger Level/Edge Control
  // bit 3 ETRIGP=0 External Trigger Polarity
  // bit 2 ETRIGE=0 External Trigger Mode Enable
  // bit 1 ASCIE=0 ATD Sequence Complete Interrupt Enable
  // bit 0 ASCIF=0 ATD Sequence Complete Interrupt Flag
  
  //ATD0CTL3 = 0x08; 
  // bit 6 S8C =0 Sequence length = 1
  // bit 5 S4C =0 
  // bit 4 S2C =0 
  // bit 3 S1C =1
  // bit 2 FIFO=0 no FIFO mode
  // bit 1 FRZ1=0 no freeze
  // bit 0 FRZ0=0 
  
  //ATD0CTL4 = 0x05; // enable ADC
  // bit 7 SRES8=0 A/D Resolution Select
  //      1 => n=8 bit resolution
  //      0 => n=10 bit resolution
  // bit 6 SMP1=0 Sample Time Select 
  // bit 5 SMP0=0 s=4
  // bit 4 PRS4=0 ATD Clock Prescaler m=5
  // bit 3 PRS3=0 ATD Clock Prescaler
  // bit 2 PRS2=1 ATD Clock Prescaler
  // bit 1 PRS1=0 ATD Clock Prescaler
  // bit 0 PRS0=1 ATD Clock Prescaler
  // Prescale = 2(m+1) = 12 cycles
  // ADC clock period = 2(m+1)/24MHz = 0.5us
  // choose m so 2(m+1)/Eperiod is between 0.5 to 2us
  // Sample time s=4
  // SMP1 SMP0  s sample time
  //  0    0    4 ADC clock periods
  //  0    1    6 ADC clock periods
  //  1    0   10 ADC clock periods
  //  1    1   18 ADC clock periods
  // ADC conversion time  2(m+1)(s+n)/24MHz = 7us
  // if multiple samples are selected, then each requires 7us
  
}
extern unsigned short interruptRate;
void OC_Start  (void){ 
  DisableInterrupts;  
  TimerInit();
  TIOS |= 0x01;   // activate TC0 as output compare
  TIE  |= 0x01;   // arm OC0     
  TC0 = TCNT + 50; //First interrupt = immediate
  EnableInterrupts;
}
 /*
unsigned short ADC0_In (unsigned short channel){
  //Sample Given channel
  ATD0CTL5 = channel;
  while ((ATD0STAT1&0x01) == 0);
  return ATD0DR0;
}  
 
interrupt 8 void TOC0handler(void){ // executes at Hz defined in main
  DisableInterrupts; 
  TFLG1 = 0x01;         // acknowledge OC0

//we don't need a fifo. we just want the most recent value for paddle position.
  Update_Left_Paddle(ADC0_In(0x80));      // function that samples channel 0
  Update_Right_Paddle(ADC0_In(0x82));      //channel 1
  TC0 += interruptRate;     
  
  EnableInterrupts; 
}
     */