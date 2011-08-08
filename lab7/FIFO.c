#include <hidef.h>      /* common defines and macros */
#include <mc9s12dp512.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dp512"

#define FIFOSIZE 16   // must be a power of 2
#define FIFOSUCCESS 1
#define FIFOFAIL    0

unsigned short volatile PutI;  // put next
unsigned short volatile GetI;  // get next



typedef unsigned short dataType;
dataType static Fifo[FIFOSIZE];
// ******* Fifo_Init ********
// Initialize FIFO, making it empty
// This function must be called before FIFO is used,
// Can be called again to empty FIFO
// Inputs: none
// Outputs: none
void Fifo_Init(void){ unsigned char SaveCCR;
asm  tpa  
asm  staa SaveCCR 
asm  sei          // make atomic 
  PutI = GetI = 0;      // Empty
asm  ldaa SaveCCR
asm  tap          // end critical section 
}

// ******* Fifo_Put ********
// Put data into FIFO
// Inputs: data to save in FIFO
// Outputs: FIFOSUCCESS if saved
//          FIFOFAIL if not saved, because FIFO was full
int Fifo_Put(dataType data){
asm sei
  PTT_PTT3 = 1;
  if((PutI-GetI) & ~(FIFOSIZE-1)){
  PTT_PTT3 = 0;
    return(FIFOFAIL); // Failed, fifo full
  }
  Fifo[PutI&(FIFOSIZE-1)] = data; // put
  PutI++;  // Success, update
  PTT_PTT3 = 0;
asm cli
  return(FIFOSUCCESS);
}


// ******* Fifo_Get ********
// Get data from FIFO
// Inputs: pointer to memory into which to return data
// Outputs: FIFOSUCCESS if data properly removed
//          FIFOFAIL if not removed, because FIFO was empty
int Fifo_Get(dataType *datapt){
asm sei
  PTT_PTT2 = 1;
  if(PutI == GetI ){
  PTT_PTT2 = 0;
    return(FIFOFAIL); // Empty if PutI=GetI
  }
  *datapt = Fifo[GetI&(FIFOSIZE-1)];
  GetI++;  // Success, update
  PTT_PTT2 = 0;
asm cli  
  return(FIFOSUCCESS);
}

// ******* Fifo_Size ********
// Check the status of the FIFO
// Inputs: none
// Outputs: number of elements currently stored
// 0 to FIFOSIZE (0 means empty, FIFOSIZE means full)
unsigned short Fifo_Size(void){
 return ((unsigned short)(PutI-GetI));
}
