#ifndef CONCEPT_H
#define CONCEPT_H

///////////////////
//  SDR Concept  //
///////////////////
//A concept named by a SDR

//References//
//-----------//
#include "FIFO.h"
#include "Table.h"
#include "Usage.h"

//Parameters//
//----------//
#define OPERATIONS_MAX 10
#define MIN_CONFIDENCE 0.01
#define CONCEPT_INTERPOLATION_STRENGTH 1.0
#define CONCEPT_INTERPOLATION_INIT_STRENGTH 1.0
#define CONCEPT_INTERPOLATION_LAYER_DECAY 0.8
#define CONCEPT_INTERPOLATION_COUNTER_MAX 10.0
#define CONCEPT_INTERPOLATION_COUNTER_MIN -10.0
#define CONCEPT_LAYERS 10 //with 2^10=1024 

//Data structure//
//--------------//
typedef struct {
    int id;
    Usage usage;
    SDR sdr;
    SDR_HASH_TYPE sdr_hash;
    Event belief_spike;
    Event incoming_goal_spike;
    Event goal_spike;
    Table precondition_beliefs[OPERATIONS_MAX];
    //Concept interpolation:
    double sdr_bit_counter[SDR_SIZE];
    //For debugging:
    char debug[50];
} Concept;

//Methods//
//-------//
//Assign a new name to a concept
void Concept_SetSDR(Concept *concept, SDR sdr);
//print a concept
void Concept_Print(Concept *concept);
//Interpolate concepts, see https://github.com/patham9/ANSNA/wiki/Concept:-Conceptual-Interpolation
void Concept_SDRInterpolation(int layer, Concept *concept, SDR *eventSDR);
//Local inference: confirming anticipations, firing spikes, matching event, adjusting Usage
Event Concept_LocalInference(int layer, Concept *c, Event *e, long currentTime);

#endif
