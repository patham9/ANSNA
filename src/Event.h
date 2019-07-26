#ifndef EVENT_H
#define EVENT_H

///////////////////
//  SDR Event    //
///////////////////
//also see https://github.com/patham9/ANSNA/wiki/Input

//References//
//-----------//
#include "SDR.h"
#include "Stamp.h"

//Data structure//
//--------------//
#define EVENT_TYPE_GOAL 1
#define EVENT_TYPE_BELIEF 2
#define EVENT_TYPE_DELETED 0
typedef struct {
    SDR sdr;
    SDR_HASH_TYPE sdr_hash;
    char type; //either JUDGMENT or GOAL
    Truth truth;
    Stamp stamp;
    long occurrenceTime;
    int operationID; //automatically obtained from SDR on input
    bool processed;
    char debug[30];
    SDR context;
} Event;

//Methods//
//-------//
//Init/Reset module
void Event_INIT();
//Assign a new name to an event
void Event_SetSDR(Event *event, SDR sdr);
//construct an input event
Event Event_InputEvent(SDR sdr, char type, Truth truth, long currentTime);
//print event
void Event_Print(Event *event);

#endif
