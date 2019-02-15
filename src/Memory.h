#ifndef MEMORY_H
#define MEMORY_H

///////////////////
//  ANSNA Memory //
///////////////////
//The conce-based memory of ANSNA
//See voting mechanism to make it fast:
//https://github.com/patham9/ANSNA/wiki/Voting-Schema

//References//
//////////////
#include "Concept.h"

//Parameters//
//----------//
#define CONCEPTS_MAX 1000 //TODO statically alloc once on INIT, as can lead in value too large for the compiler to handle
#define EVENTS_MAX 64
#define OPERATIONS_MAX 1000

//Data structure//
//--------------//
//Data structures
extern Concept *concepts;
extern Event *events;
typedef void (*Action)(void);
typedef void (*EventInspector)(Event *);
typedef struct
{
    SDR sdr;
    Action action;
}Operation;
Operation operations[OPERATIONS_MAX];
EventInspector event_inspector;

//Methods//
//-------//
//Init memory
void Memory_INIT();
//Find a concept
bool Memory_FindConceptBySDR(SDR *sdr, int *returnIndex);
//Create a new concept
Concept* Memory_Conceptualize(SDR *sdr, Attention *attention, long currentTime);
//Return closest concept
bool Memory_getClosestConcept(Event *event, int *returnIndex);
//Add event to memory
void Memory_addEvent(Event *event);
//Add operation to memory
void Memory_addOperation(Operation op);
//Reset events
void Memory_ResetEvents();
//Reset concepts
void Memory_ResetConcepts();
//Select highest priority concepts
int Memory_selectHighestPriorityConcepts(int k, Concept **selectedConcepts);
//Select highest priority events
int Memory_selectHighestPriorityEvents(int k, Event **selectedEvents);
#endif
