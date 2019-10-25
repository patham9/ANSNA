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
#define ANTICIPATIONS_MAX 50
#define MIN_CONFIDENCE 0.01

//Data structure//
//--------------//
typedef struct {
    Implication negative_confirmation;
    long deadline;
    long operation_id;
} AnticipationEntry;

typedef struct {
    int id;
    Usage usage;
    SDR sdr;
    SDR_HASH_TYPE sdr_hash;
    Event belief_spike;
    Event incoming_goal_spike;
    Event goal_spike;
    Table precondition_beliefs[OPERATIONS_MAX];
    //For debugging:
    char debug[50];
    //Anticipation:
    AnticipationEntry anticipations[ANTICIPATIONS_MAX];
} Concept;

//Methods//
//-------//
//Assign a new name to a concept
void Concept_SetSDR(Concept *concept, SDR sdr);
//print a concept
void Concept_Print(Concept *concept);

#endif
