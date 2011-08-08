//*****Timer Header******

//---------------------Timer_Init---------------------
// initializes timer
// Input: none
// Output: none
void Timer_Init(void); 

//---------------------Wait---------------------
// time delay
// Input: time in 41.6667ns(1/24M)
// Output: none
void Wait(unsigned short delay);

//---------------------Wait1ms---------------------
// time delay
// Input: time in msec
// Output: none
void Wait1ms(unsigned short msec);