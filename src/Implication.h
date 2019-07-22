#ifndef BELIEF_H
#define BELIEF_H

//References//
//-----------//
#include "SDR.h"
#include "Stamp.h"

//Data structure//
//--------------//
typedef struct {
    SDR sdr;
    SDR_HASH_TYPE sdr_hash;
    Truth truth;
    Stamp stamp;
    long occurrenceTimeOffset;
    char debug[50]; //++ DEBUG
} Implication;

//Methods//
//-------//
//Assign a new name to an implication
void Implication_SetSDR(Implication *implication, SDR sdr);
void Implication_Print(Implication *implication);
double Implication_Currentness(Implication *implication);

#endif


