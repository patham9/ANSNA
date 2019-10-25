#ifndef ANTICIPATION_H
#define ANTICIPATION_H

//////////////////////////
//  ANSNA Anticipation  //
//////////////////////////
//Anticipation of future events, plus feedback

//References//
//-----------//
#include "Memory.h"

//Parameters//
//----------//
#define ANTICIPATION_THRESHOLD 0.501
#define ANTICIPATION_CONFIDENCE 0.001
#define ANSNA_DEFAULT_FREQUENCY  1.0
#define ANSNA_DEFAULT_CONFIDENCE 0.9
#define ANSNA_DEFAULT_TRUTH ((Truth) { .frequency = ANSNA_DEFAULT_FREQUENCY, .confidence = ANSNA_DEFAULT_CONFIDENCE })

//Methods//
//-------//
//Predict and expect an outcome to happen
void Anticipation_Anticipate(int operationID, long currentTime);
//Negative evidence for the predictive hypothesis that made an unconfirmed prediction
void Anticipation_Disappoint(Concept *c, long currentTime);
//Confirm the anticipation with an event
void Anticipation_Confirm(Concept *c, Event *e);

#endif
