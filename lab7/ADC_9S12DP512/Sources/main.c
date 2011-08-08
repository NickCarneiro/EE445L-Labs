// filename ******** Main.C ************** 
// This example illustrates the ADC, 
//     analog input connected to PAD2
// It also interfaces an LCD in 8-bit mode, and uses the PLL.
// 9S12DP512
// Jonathan W. Valvano 7/18/07

//  This example accompanies the books
//   "Embedded Microcomputer Systems: Real Time Interfacing",
//        Thomson Engineering, copyright (c) 2006,
//   "Introduction to Embedded Microcomputer Systems: 
//    Motorola 6811 and 6812 Simulation", Brooks-Cole, copyright (c) 2002

// Copyright 2007 by Jonathan W. Valvano, valvano@mail.utexas.edu 
//    You may use, edit, run or distribute this file 
//    as long as the above copyright notice remains 
/*   
  physically the LCD size is 1*16 
  but, programming-wise it is 2 rows of 8
  addr  00 01 02 03 04 05 06 07 $40 $41 $42 $43 $44 $45 $46 $47
  for best LCD contrast, use a 10k or 20k potentiometer
     pot pin1 to +5V 
     pot pin2 to LCD pin3 
     pot pin3 to ground 
  ground = pin 1    Vss   ground
  power  = pin 2    Vdd   +5V
  ground = pin 3    Vlc   Vee   
  PP1    = pin 4    RS    (1 for data, 0 for control/status)
  ground = pin 5    R/W   (1 for read, 0 for write)
  PP0    = pin 6    E     (enable)
  PH7    = pin 14   DB7   (8-bit data)
  PH6    = pin 13   DB6
  PH5    = pin 12   DB5
  PH4    = pin 11   DB4
  PH3    = pin 10   DB3   
  PH2    = pin  9   DB2
  PH1    = pin  8   DB1
  PH0    = pin  7   DB0
  if do need to read busy, then you can connect an output to R/W
*/

#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"

#include "Timer.h"
#include "lcd.h"
#include "PLL.h"
#include "adc.h"
#include "SCI0.H"
#include "FIFO.H"
#include "fixed.h"

#define TEN_MS 188

void OC_Init0(void);

unsigned short Data;		// ADC sample

//output temperature in degrees celcius with index as 10-bit adc value (0 to 1024)
//output values range from 0.00 degC to 39.96 degC
short lookupTable [] = {
3796,	3791,	3787,	3782,	3778,	3773,	3769,	3764,	3760,	3755,	3751,	3746,	3742,	3737,	3733,
3728,	3724,	3720,	3715,	3711,	3706,	3702,	3697,	3693,	3688,	3684,	3679,	3675,	3670,	3666,
3661,	3657,	3652,	3648,	3644,	3639,	3635,	3630,	3626,	3621,	3617,	3612,	3608,	3603,	3599,
3594,	3590,	3585,	3581,	3576,	3572,	3568,	3563,	3559,	3554,	3550,	3545,	3541,	3536,	3532,
3527,	3523,	3518,	3514,	3509,	3505,	3500,	3496,	3492,	3487,	3483,	3478,	3474,	3469,	3465,
3460,	3456,	3451,	3447,	3442,	3438,	3433,	3429,	3424,	3420,	3416,	3411,	3407,	3402,	3398,
3393,	3389,	3384,	3380,	3375,	3371,	3366,	3362,	3357,	3353,	3349,	3344,	3340,	3335,	3331,
3326,	3322,	3317,	3313,	3308,	3304,	3299,	3295,	3290,	3286,	3281,	3277,	3273,	3268,	3264,
3259,	3255,	3250,	3246,	3241,	3237,	3232,	3228,	3223,	3219,	3214,	3210,	3205,	3201,	3197,
3192,	3188,	3183,	3179,	3174,	3170,	3165,	3161,	3156,	3152,	3147,	3143,	3138,	3134,	3129,
3125,	3121,	3116,	3112,	3107,	3103,	3098,	3094,	3089,	3085,	3080,	3076,	3071,	3067,	3062,
3058,	3053,	3049,	3045,	3040,	3036,	3031,	3027,	3022,	3018,	3013,	3009,	3004,	3000,	2995,
2991,	2986,	2982,	2977,	2973,	2969,	2964,	2960,	2955,	2951,	2946,	2942,	2937,	2933,	2928,
2924,	2919,	2915,	2910,	2906,	2902,	2897,	2893,	2888,	2884,	2879,	2875,	2870,	2866,	2861,
2857,	2852,	2848,	2843,	2839,	2834,	2830,	2826,	2821,	2817,	2812,	2808,	2803,	2799,	2794,
2790,	2785,	2781,	2776,	2772,	2767,	2763,	2758,	2754,	2750,	2745,	2741,	2736,	2732,	2727,
2723,	2718,	2714,	2709,	2705,	2700,	2696,	2691,	2687,	2682,	2678,	2674,	2669,	2665,	2660,
2656,	2651,	2647,	2642,	2638,	2633,	2629,	2624,	2620,	2615,	2611,	2606,	2602,	2598,	2593,
2589,	2584,	2580,	2575,	2571,	2566,	2562,	2557,	2553,	2548,	2544,	2539,	2535,	2530,	2526,
2522,	2517,	2513,	2508,	2504,	2499,	2495,	2490,	2486,	2481,	2477,	2472,	2468,	2463,	2459,
2455,	2450,	2446,	2441,	2437,	2432,	2428,	2423,	2419,	2414,	2410,	2405,	2401,	2396,	2392,
2387,	2383,	2379,	2374,	2370,	2365,	2361,	2356,	2352,	2347,	2343,	2338,	2334,	2329,	2325,
2320,	2316,	2311,	2307,	2303,	2298,	2294,	2289,	2285,	2280,	2276,	2271,	2267,	2262,	2258,
2253,	2249,	2244,	2240,	2235,	2231,	2227,	2222,	2218,	2213,	2209,	2204,	2200,	2195,	2191,
2186,	2182,	2177,	2173,	2168,	2164,	2159,	2155,	2151,	2146,	2142,	2137,	2133,	2128,	2124,
2119,	2115,	2110,	2106,	2101,	2097,	2092,	2088,	2083,	2079,	2075,	2070,	2066,	2061,	2057,
2052,	2048,	2043,	2039,	2034,	2030,	2025,	2021,	2016,	2012,	2008,	2003,	1999,	1994,	1990,
1985,	1981,	1976,	1972,	1967,	1963,	1958,	1954,	1949,	1945,	1940,	1936,	1932,	1927,	1923,
1918,	1914,	1909,	1905,	1900,	1896,	1891,	1887,	1882,	1878,	1873,	1869,	1864,	1860,	1856,
1851,	1847,	1842,	1838,	1833,	1829,	1824,	1820,	1815,	1811,	1806,	1802,	1797,	1793,	1788,
1784,	1780,	1775,	1771,	1766,	1762,	1757,	1753,	1748,	1744,	1739,	1735,	1730,	1726,	1721,
1717,	1712,	1708,	1704,	1699,	1695,	1690,	1686,	1681,	1677,	1672,	1668,	1663,	1659,	1654,
1650,	1645,	1641,	1636,	1632,	1628,	1623,	1619,	1614,	1610,	1605,	1601,	1596,	1592,	1587,
1583,	1578,	1574,	1569,	1565,	1561,	1556,	1552,	1547,	1543,	1538,	1534,	1529,	1525,	1520,
1516,	1511,	1507,	1502,	1498,	1493,	1489,	1485,	1480,	1476,	1471,	1467,	1462,	1458,	1453,
1449,	1444,	1440,	1435,	1431,	1426,	1422,	1417,	1413,	1409,	1404,	1400,	1395,	1391,	1386,
1382,	1377,	1373,	1368,	1364,	1359,	1355,	1350,	1346,	1341,	1337,	1333,	1328,	1324,	1319,
1315,	1310,	1306,	1301,	1297,	1292,	1288,	1283,	1279,	1274,	1270,	1265,	1261,	1257,	1252,
1248,	1243,	1239,	1234,	1230,	1225,	1221,	1216,	1212,	1207,	1203,	1198,	1194,	1189,	1185,
1181,	1176,	1172,	1167,	1163,	1158,	1154,	1149,	1145,	1140,	1136,	1131,	1127,	1122,	1118,
1114,	1109,	1105,	1100,	1096,	1091,	1087,	1082,	1078,	1073,	1069,	1064,	1060,	1055,	1051,
1046,	1042,	1038,	1033,	1029,	1024,	1020,	1015,	1011,	1006,	1002,	997,	993,	988,	984,
979,	975,	970,	966,	962,	957,	953,	948,	944,	939,	935,	930,	926,	921,	917,
912,	908,	903,	899,	894,	890,	886,	881,	877,	872,	868,	863,	859,	854,	850,
845,	841,	836,	832,	827,	823,	818,	814,	810,	805,	801,	796,	792,	787,	783,
778,	774,	769,	765,	760,	756,	751,	747,	742,	738,	734,	729,	725,	720,	716,
711,	707,	702,	698,	693,	689,	684,	680,	675,	671,	667,	662,	658,	653,	649,
644,	640,	635,	631,	626,	622,	617,	613,	608,	604,	599,	595,	591,	586,	582,
577,	573,	568,	564,	559,	555,	550,	546,	541,	537,	532,	528,	523,	519,	515,
510,	506,	501,	497,	492,	488,	483,	479,	474,	470,	465,	461,	456,	452,	447,
443,	439,	434,	430,	425,	421,	416,	412,	407,	403,	398,	394,	389,	385,	380,
376,	371,	367,	363,	358,	354,	349,	345,	340,	336,	331,	327,	322,	318,	313,
309,	304,	300,	295,	291,	287,	282,	278,	273,	269,	264,	260,	255,	251,	246,
242,	237,	233,	228,	224,	220,	215,	211,	206,	202,	197,	193,	188,	184,	179,
175,	170,	166,	161,	157,	152,	148,	144,	139,	135,	130,	126,	121,	117,	112,
108,	103,	99,	94,	90,	85,	81,	76,	72,	68,	63,	59,	54,	50,	45,
41,	36,	32,	27,	23,	18,	14,	9,	5,	0,	-3,	-7,	-12,	-16,	-21,
-25,	-30,	-34,	-39,	-43,	-48,	-52,	-57,	-61,	-66,	-70,	-75,	-79,	-83,	-88,
-92,	-97,	-101,	-106,	-110,	-115,	-119,	-124,	-128,	-133,	-137,	-142,	-146,	-151,	-155,
-159,	-164,	-168,	-173,	-177,	-182,	-186,	-191,	-195,	-200,	-204,	-209,	-213,	-218,	-222,
-226,	-231,	-235,	-240,	-244,	-249,	-253,	-258,	-262,	-267,	-271,	-276,	-280,	-285,	-289,
-294,	-298,	-302,	-307,	-311,	-316,	-320,	-325,	-329,	-334,	-338,	-343,	-347,	-352,	-356,
-361,	-365,	-370,	-374,	-378,	-383,	-387,	-392,	-396,	-401,	-405,	-410,	-414,	-419,	-423,
-428,	-432,	-437,	-441,	-446,	-450,	-454,	-459,	-463,	-468,	-472,	-477,	-481,	-486,	-490,
-495,	-499,	-504,	-508,	-513,	-517,	-522,	-526,	-530,	-535,	-539,	-544,	-548,	-553,	-557,
-562,	-566,	-571,	-575,	-580,	-584,	-589,	-593,	-598,	-602,	-606,	-611,	-615,	-620,	-624,
-629,	-633,	-638,	-642,	-647,	-651,	-656,	-660,	-665,	-669,	-673,	-678,	-682,	-687,	-691,
-696,	-700,	-705,	-709,	-714,	-718,	-723,	-727,	-732,	-736,	-741,	-745,	-749,	-754,	-758,
-763,	-767,	-772,	-776		
};

      /***********************************************************************************************************************
      *          Initiating ADC conversion                                                                                   *
      *                                                                                                                      *
      *  3 ways to initialize conversion process:                                                                            *
      *    1) write to ATD0CTL5                                                                                              *
      *    2) continuous mode: software starts the ADC, but it return same value over and over again                         *
      *    3) connect an external trigger to the digital input on PAD07 to use busy wait or interrupt-driven synchronization *
      *                                                                                                                      *
      *  2 ways to know the conversion process has finished:                                                                 *
      *    1) SCF flag becomes set after conversion starts                                                                   *
      *    2) Hardware triggers interrupt or busy signal goes away (depending on type of synchronization)                    *
      ***********************************************************************************************************************/


dataType Foredata;
void main(void){
  
  char *fixed_point;   //holds output to lcd
  dataType raw;        //holds raw adc data
  
  PLL_Init();          //24MHz clock
  ADC_Init();          //adc ready on PAD5
  //Timer_Init();        //timer at 5.33us
  OC_Init0();          //oc0 at 100Hz
  LCD_Init();
  LCD_Clear();
  OC_Init0();          //oc0 at 100Hz
  asm cli
  
  while(1){
    Fifo_Get(&Foredata);                       //get ADC data
    fixed_point = Fixed_uDecOut2(lookupTable[Foredata]);  //convert to temperature (find in look-up table above)
    //printf("%s C", fixed_point);                       //print to lcd
    printf("%u", Foredata);
    LCD_GoTo(0x00);
    letter_cnt = 0;
  }
}
  

//                                    (LAB MANUAL !!!!!!!!)
/*unsigned short DataBuffer[100];
unsigned short Count = 0;
unsigned short data;

void main(void){
  unsigned short i;
  PLL_Init();
  SCI0_Init(115200);
  ADC_Init();
  Timer_Init();
  OC_Init0();
  asm cli
  
  for(i = 0; i < 1000; i++);
  while(Count < 100);   //wait for buffer to fill
  
  for(i = 0; i < 100; i++){
    SCI0_OutUDec(DataBuffer[i]); //display to screen
    SCI0_OutChar(10);  //newline
    SCI0_OutChar(13);  //line feed?
  }
  
  while(1){
    ADC_In(0x80);
  }//infinite loop
}
   */


//---------------------OC_Init0---------------------
// arm output compare 0 for 100Hz periodic interrupt
// Input: none    assumes another ritual enabled the TCNT
// Output: none 
void OC_Init0(void){
  DDRP |= 0x80;
  TIOS |= 0x01;        // activate TC3 as output compare
  TIE |= 0x01;         // arm
  TC0   = TCNT + 100;  // first interrupt right away
}


//******TC0Handler****** 
// input: none
// output: none
// puts 100 samples into DataBuffer
interrupt 8 void TC0Handler(void){
  TFLG1 = 0x01;         //clear interrupt
  PTP_PTP7 ^= 1;
  TC0 = TC0 + TEN_MS;   //100Hz = 10*10Hz (10 times the max frequency of the signal)
                        //change macro to sample at a different rate
  Fifo_Put(ADC_In(0x80));    //for use in foreground
}


/*

//******TC0Handler****** 
// input: none
// output: none
// puts 100 samples into DataBuffer
interrupt 8 void TC0Handler(void){
  TFLG1 = 0x01;
  TC0 = TC0 + TEN_MS;   //100Hz = 10*10Hz (10 times the max frequency of the signal)
  PTP ^= 0x80;
  
  if(Count < 100){
    data = ADC_In(0x80);
    DataBuffer[Count++] = data;
  }
} */ 
