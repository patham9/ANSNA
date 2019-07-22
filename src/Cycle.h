#ifndef H_CYCLE
#define H_CYCLE

/////////////////////////////////////
//  ALANN Control Cycle            //
/////////////////////////////////////
//A FIFO-like structure, that only supports put in and overwrites
//the oldest task when full

//References//
//-----------//
#include "Globals.h"
#include "RuleTable.h"
#include "Decision.h"

//Parameters//
//----------//
#define EVENT_SELECTIONS 5
//for temporal induction:
#define CONCEPT_SELECTIONS 5
#define SEQUENCE_SPAN FIFO_SIZE/2

//Methods//
//-------//
//Apply one operating cyle
void Cycle_Perform(long currentTime);

#endif
