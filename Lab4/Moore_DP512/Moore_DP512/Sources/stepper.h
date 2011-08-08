//**********STEPPER.H********
//stepper functions
//authors: Aaron Alaniz (aa28877)
//         Denis Sokolov (dks574)

#ifndef STEPPER_H
#define STEPPER_H

//---------Stepper_Init------------------
// initialize stepper motor state machine and port
// inputs: none 
// outputs: none
void Stepper_Init(void);

//---------Stepper_Step------------------
// Steps the stepper motor forward or backward
// inputs: dir is the direction to step (0 for backward, 1 for forward) 
// outputs: none
void Stepper_Step(void);

//state construct
const struct State{
  unsigned char output;            //output to pins
  unsigned short time;             //hold time
  const struct State* next[8];     //next states
};

#endif