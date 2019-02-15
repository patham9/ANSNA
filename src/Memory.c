#include "Memory.h"

int operations_index = 0;
Concept *concepts = NULL;
Event *events = NULL;

void Memory_ResetEvents()
{
    if(events != NULL)
    {
        free(events);
    }
    events = calloc(EVENTS_MAX, sizeof(Event)); 
    if(events == NULL)
    {
        printf("OUT OF MEMORY: Can't allocate %f Gigabytes for events!\n", 
              ((float)EVENTS_MAX*sizeof(Event))/(1000000000.0f));
        exit(1);
    }
}

void Memory_ResetConcepts()
{
    if(concepts != NULL)
    {
        free(concepts);
    }
    concepts = calloc(CONCEPTS_MAX, sizeof(Concept));
    if(concepts == NULL)
    {
        printf("OUT OF MEMORY: Can't allocate %f Gigabytes for concepts!\n",
               ((float)CONCEPTS_MAX*sizeof(Concept))/(1000000000.0f));
        exit(1);
    }
}

long concept_id = 1;
void Memory_INIT()
{
    Memory_ResetConcepts();
    Memory_ResetEvents();
    for(int i=0; i<OPERATIONS_MAX; i++)
    {
        operations[i] = (Operation) {0};
    }
    operations_index = 0;
    concept_id = 1;
}

bool Memory_FindConceptBySDR(SDR *sdr, int *returnIndex)
{
    for(int i=0; i<CONCEPTS_MAX; i++)
    {
        Concept *existing = &concepts[i];
        if(Concept_Exists(existing)) //it exists
        {
            if(SDR_Equal(&existing->sdr, sdr))
            {
                if(returnIndex != NULL)
                {
                    *returnIndex = i;
                }
                return true;
            }
        }
    }
    return false;
}

Concept* Memory_Conceptualize(SDR *sdr, Attention *attention, long currentTime)
{
    //1. find concept of lowest usefulness or, if available, an empty concept
    int leastUsefulIndex = 0;
    double leastUsefulness = 1.0;
    for(int i=0; i<CONCEPTS_MAX; i++)
    {
        Concept *existing = &concepts[i];
        if(Concept_Exists(existing))
        {
            double usefulness = Usage_usefulness(&existing->usage, currentTime);
            if(usefulness <= leastUsefulness)
            {
                leastUsefulIndex = i;
                leastUsefulness = usefulness;
            }
        }
        else
        {
            leastUsefulIndex = i;
            break;
        }
    }
    //2. assign concept to that position, setting SDR, attention, id, then return it
    Concept *addedConcept = &concepts[leastUsefulIndex];
    *addedConcept = (Concept) {0};
    Concept_SetSDR(addedConcept, *sdr);
    addedConcept->attention = *attention;
    addedConcept->id = concept_id++;
    return addedConcept;
}

bool Memory_getClosestConcept(Event *event, int *returnIndex)
{
    SDR *eventSDR = &(event->sdr);
    if(Memory_FindConceptBySDR(&event->sdr, returnIndex)) //speed improvement
    {
        return true;
    }
    int best_i = -1;
    double bestValSoFar = -1;
    for(int i=0; i<CONCEPTS_MAX; i++)
    {
        Concept *existing = &concepts[i];
        if(Concept_Exists(existing))
        {
            double curVal = Truth_Expectation(SDR_Inheritance(eventSDR, &(existing->sdr)));
            if(curVal > bestValSoFar)
            {
                bestValSoFar = curVal;
                best_i = i;
            }
        }
    }
    *returnIndex = best_i;
    return best_i != -1;
}

void Memory_addEvent(Event *event)
{
    if(event_inspector != NULL)
    {
        (*event_inspector)(event);
    }
    //1. find event of lowest priority or, if available, an empty event
    int leastPriorityIndex = 0;
    double leastPriority = 1.0;
    for(int i=0; i<CONCEPTS_MAX; i++)
    {
        Event *existing = &events[i];
        if(Event_Exists(existing))
        {
            double priority = (event->attention).priority;
            if(priority <= leastPriority)
            {
                leastPriorityIndex = i;
                leastPriority = priority;
            }
        }
        else
        {
            leastPriorityIndex = i;
            break;
        }
    }
    //2. assign event to that position, setting SDR, attention, id, then return it
    Event *addedEvent = &events[leastPriorityIndex];
    *addedEvent = *event;
}

void Memory_addOperation(Operation op)
{
    operations[operations_index%OPERATIONS_MAX] = op;
    operations_index++;
}

int Memory_selectHighestPriorityEvents(int k, Event **selectedEvents)
{
    int selected = 0;
    for(int h=0; h<k; h++)
    {
        selectedEvents[h] = NULL;
    }
    for(int i=0; i<EVENTS_MAX; i++) //go through all the events that exist
    {
        Event *existing = &events[i];
        if(Event_Exists(existing))
        {
            for(int j=0; j<k; j++) //go from left to right in the selectedEvents to see where it belongs
            {
                //ok this place is empty we can just insert it here
                if(!Event_Exists(selectedEvents[j]))
                {
                    selectedEvents[j] = existing; //inserted
                    selected++;
                    break;
                }
                //if it's higher priority, we have to insert here, but shift to the right first
                if(existing->attention.priority > selectedEvents[j]->attention.priority)
                {
                    for(int j2=k-1; j2>j; j2--)
                    {
                        selectedEvents[j2] = selectedEvents[j2-1];
                    }
                    selectedEvents[j] = existing; //inserted
                    break;
                }
                //if it's lower priority, we cannot insert it here, move to the right
            }
        }
    }
    return selected;
}

int Memory_selectHighestPriorityConcepts(int k, Concept **selectedConcepts)
{
    int selected = 0;
    for(int h=0; h<k; h++)
    {
        selectedConcepts[h] = NULL;
    }
    for(int i=0; i<CONCEPTS_MAX; i++) //go through all the concepts that exist
    {
        Concept *existing = &concepts[i];
        if(Concept_Exists(existing))
        {
            for(int j=0; j<k; j++) //go from left to right in the selectedEvents to see where it belongs
            {
                //ok this place is empty we can just insert it here
                if(!Concept_Exists(selectedConcepts[j]))
                {
                    selectedConcepts[j] = existing; //inserted
                    selected++;
                    break;
                }
                //if it's higher priority, we have to insert here, but shift to the right first
                if(existing->attention.priority > selectedConcepts[j]->attention.priority)
                {
                    for(int j2=k-1; j2>j; j2--)
                    {
                        selectedConcepts[j2] = selectedConcepts[j2-1];
                    }
                    selectedConcepts[j] = existing; //inserted
                    break;
                }
                //if it's lower priority, we cannot insert it here, move to the right
            }
        }
    }
    return selected;
}
