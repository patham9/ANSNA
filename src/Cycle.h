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
#define CONCEPT_FORMATION_NOVELTY 0.2
#define PROPAGATE_GOAL_SPIKES true
//this can be seen as some kind of attention mechanism:
#define PROPAGATION_TRUTH_EXPECTATION_THRESHOLD 0.8
//how often a child concept needs to be the best match to become alive
#define CONCEPT_USE_TO_BE_ALIVE_THRESHOLD 10

//Methods//
//-------//
//Apply one operating cyle
void Cycle_Perform(long currentTime);

#endif
