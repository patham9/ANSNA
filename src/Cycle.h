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
#define PROPAGATE_GOAL_SPIKES true
//this can be seen as some kind of attention mechanism:
#define PROPAGATION_TRUTH_EXPECTATION_THRESHOLD 0.8
//how often a child concept needs to be the best match to become alive
//but it's not fixed, it's multiplied with familiarity to become become_alive_threshold
#define CONCEPT_USE_TO_BE_ALIVE_THRESHOLD 5

//Methods//
//-------//
//Apply one operating cyle
void Cycle_Perform(long currentTime);

#endif
