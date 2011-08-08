//**********STEPPER.C********
//stepper motor drivers
//implements a stepper motor state machine
//and functions for moving the motor
//authors: Aaron Alaniz (aa28877)
//         Denis Sokolov (dks574)

#include <mc9s12dp512.h>
#include "stepper.h"
#include "switches.h"

#define STATE_HOLD 20000
#define FAST_HOLD 5000
#define JERK_HOLD 8000

#define S0 &stepper_fsm[0]
#define S1 &stepper_fsm[1]
#define S2 &stepper_fsm[2]
#define S3 &stepper_fsm[3]
#define S4 &stepper_fsm[4]
#define S5 &stepper_fsm[5]
#define S6 &stepper_fsm[6]
#define S7 &stepper_fsm[7]
#define S8 &stepper_fsm[8]
#define S9 &stepper_fsm[9]
#define S10 &stepper_fsm[10]
#define S11 &stepper_fsm[11]
#define S12 &stepper_fsm[12]
#define S13 &stepper_fsm[13]
#define S14 &stepper_fsm[14]
#define S15 &stepper_fsm[15]
#define S16 &stepper_fsm[16]
#define S17 &stepper_fsm[17]
#define S18 &stepper_fsm[18]
#define S19 &stepper_fsm[19]
#define S20 &stepper_fsm[20]
#define S21 &stepper_fsm[21]
#define S22 &stepper_fsm[22]

const struct State stepper_fsm[] = 
                  // *************Switch Combinations*************    Enumerated States
{                 //000     001   010   011   100   101   110   111
{0,   STATE_HOLD,   {S1,    S1,   S1,   S1,   S1,   S1,   S1,   S1}},     //S0   
{5,   STATE_HOLD,   {S1,    S10,  S8,   S8,   S2,   S2,   S4,   S4}},     //S1
{6,   STATE_HOLD,   {S2,    S11,  S5,   S5,   S3,   S3,   S1,   S1}},     //S2
{10,  STATE_HOLD,   {S3,    S12,  S6,   S6,   S4,   S4,   S2,   S2}},     //S3
{9,   STATE_HOLD,   {S4,    S9,   S7,   S7,   S1,   S1,   S3,   S3}},     //S4
{6,   STATE_HOLD,   {S1,    S1,   S5,   S5,   S1,   S1,   S5,   S5}},     //S5
{10,  STATE_HOLD,   {S2,    S2,   S6,   S6,   S2,   S2,   S6,   S6}},     //S6
{9,   STATE_HOLD,   {S3,    S3,   S7,   S7,   S3,   S3,   S7,   S7}},     //S7
{5,   STATE_HOLD,   {S4,    S4,   S8,   S8,   S4,   S4,   S8,   S8}},     //S8
{5,   JERK_HOLD,    {S9,    S10,  S8,   S9,   S2,   S9,   S2,   S9}},     //S9
{6,   FAST_HOLD,    {S10,   S11,  S5,   S10,  S3,   S10,  S3,   S10}},    //S10
{10,  FAST_HOLD,    {S11,   S12,  S6,   S11,  S4,   S11,  S4,   S11}},    //S11
{9,   FAST_HOLD,    {S12,   S13,  S7,   S12,  S1,   S12,  S1,   S12}},    //S12
{5,   FAST_HOLD,    {S13,   S14,  S8,   S13,  S2,   S13,  S2,   S13}},    //S13
{6,   FAST_HOLD,    {S14,   S15,  S5,   S14,  S3,   S14,  S3,   S14}},    //S14
{10,  JERK_HOLD,    {S15,   S16,  S6,   S15,  S4,   S15,  S4,   S15}},    //S15
{6,   FAST_HOLD,    {S16,   S17,  S5,   S16,  S3,   S16,  S3,   S16}},    //S16
{5,   FAST_HOLD,    {S17,   S18,  S8,   S17,  S2,   S17,  S2,   S17}},    //S17
{9,   FAST_HOLD,    {S18,   S19,  S7,   S18,  S1,   S18,  S1,   S18}},    //S18
{10,  FAST_HOLD,    {S19,   S20,  S6,   S19,  S4,   S19,  S4,   S19}},    //S19
{6,   FAST_HOLD,    {S20,   S21,  S5,   S20,  S3,   S20,  S3,   S20}},    //S20
{5,   FAST_HOLD,    {S21,   S22,  S8,   S21,  S2,   S21,  S2,   S21}},    //S21
{9,   JERK_HOLD,    {S22,   S9,   S7,   S22,  S4,   S22,  S4,   S22}}     //S22
};  

const struct State* Cur_State;                                                                                                   
                                                                                                                      
//---------Stepper_Init------------------
// initialize stepper motor state machine and port
// inputs: none 
// outputs: none
void Stepper_Init(void)
{
  DDRT |= 0x0F;   //low four pins output to motor
  Cur_State = S0;  //goto default state (5)
}

//---------Stepper_Step------------------
// Steps the stepper motor forward or backward
// inputs: dir is the direction to step (0 for backward, 1 for forward) 
// outputs: none
void Stepper_Step(void)
{
  PTT = (PTT & 0xF0) + (Cur_State->output);          //set output
  Cur_State = Cur_State->next[Get_Switches()];       //step to next state
}