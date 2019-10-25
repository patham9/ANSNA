#ifndef ANSNA_H
#define ANSNA_H

////////////////////////////////////////////////////
//  ANSNA - Adaptive Neuro-Symbolic Network Agent //
////////////////////////////////////////////////////

//References//
//-----------//
#include "Cycle.h"

//Parameters//
//----------//
extern long currentTime;

//Callback function types//
//-----------------------//
//typedef void (*Action)(void); //already defined in Memory

//Methods//
//-------//
//Init/Reset system
void ANSNA_INIT();
//Run the system for a certain amount of cycles
void ANSNA_Cycles(int cycles);
//Add input
Event ANSNA_AddInput(SDR sdr, char type, Truth truth);
Event ANSNA_AddInputBelief(SDR sdr);
Event ANSNA_AddInputGoal(SDR sdr);
//Add an operation
void ANSNA_AddOperation(SDR sdr, Action procedure);

#endif
