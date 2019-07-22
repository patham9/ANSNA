#include "Cycle.h"

int eventsSelected = 0, eventsDerived = 0;
Event selectedEvents[EVENT_SELECTIONS]; //better to be global
Concept selectedConcepts[CONCEPT_SELECTIONS]; //too large to be a local array
Event derivations[MAX_DERIVATIONS];

//doing inference within the matched concept, returning the matched event
Event LocalInference(Concept *c, int closest_concept_i, Event *e, long currentTime)
{
    //Matched event, see https://github.com/patham9/ANSNA/wiki/SDR:-SDRInheritance-for-matching,-and-its-truth-value
    strcpy(c->debug, e->debug);
    Event eMatch = *e;
    eMatch.sdr = c->sdr;
    eMatch.truth = Truth_Deduction(SDR_Inheritance(&e->sdr, &c->sdr), e->truth);
    if(eMatch.truth.confidence > MIN_CONFIDENCE)
    {
        Concept_SDRInterpolation(c, &e->sdr, eMatch.truth); 
        //apply decomposition-based inference: prediction/explanation
        //RuleTable_Decomposition(c, &eMatch, currentTime); <- TODO, how to deal with derived events? I guess FIFO will need to support it
        c->usage = Usage_use(&c->usage, currentTime);          //given its new role it should be doable to add a priorization mechanism to it
        //add event to the FIFO of the concept
        FIFO *fifo =  e->type == EVENT_TYPE_BELIEF ? &c->event_beliefs : &c->event_goals;
        FIFO_Add(&eMatch, fifo); //TODO: Add and revise
        //activate concepts attention with the event's attention
        c->attention = Attention_activateConcept(&c->attention, &eMatch.attention); 
        PriorityQueue_IncreasePriority(&concepts, closest_concept_i, c->attention.priority); //priority was increased
    }
    return eMatch;
}

Event ProcessEvent(Event *e, long currentTime)
{
    e->processed = true;
    Event_SetSDR(e, e->sdr); // TODO make sure that hash needs to be calculated once instead already
    IN_DEBUG( printf("Event was selected:\n"); Event_Print(e); )
    //determine the concept it is related to
    int closest_concept_i;
    Concept *c = NULL;
    Event eMatch = {0};
    if(Memory_getClosestConcept(&e->sdr, e->sdr_hash, &closest_concept_i))
    {
        c = concepts.items[closest_concept_i].address;
        //perform concept-related inference
        eMatch = LocalInference(c, closest_concept_i, e, currentTime);
    }
    if(!Memory_FindConceptBySDR(&e->sdr, e->sdr_hash, NULL))
    {   
        //add a new concept for e too at the end, as it does not exist already
        Concept *eNativeConcept = Memory_Conceptualize(&e->sdr, e->attention);
        if(eNativeConcept != NULL && c != NULL)
        {
            //copy over all knowledge
            for(int i=0; i<OPERATIONS_MAX; i++)
            {
                Table_COPY(&c->precondition_beliefs[i],  &eNativeConcept->precondition_beliefs[i]);
                Table_COPY(&c->postcondition_beliefs[i], &eNativeConcept->postcondition_beliefs[i]);
            }
            FIFO_COPY(&c->event_beliefs, &eNativeConcept->event_beliefs);
            FIFO_COPY(&c->event_goals, &eNativeConcept->event_goals);
        }
    }
    return eMatch;
}

int stampID = 0;
void Cycle_Perform(long currentTime)
{    
    IN_DEBUG
    (
        for(int i=0; i<belief_events.itemsAmount; i++)
        {
            Event *ev = FIFO_GetKthNewestElement(&belief_events, i);
            printf(ev->debug);
            printf("\n");
        }
        printf("items amount: %d",belief_events.itemsAmount);
        getchar();
    )
    //1. process newest event
    if(belief_events.itemsAmount > 0)
    {
        Event *toProcess = FIFO_GetNewestElement(&belief_events);
        if(!toProcess->processed)
        {
            //the matched event becomes the postcondition
            Event postcondition = ProcessEvent(toProcess, currentTime);
            //Mine for <(&/,precondition,operation) =/> postcondition> patterns in the FIFO:     
            if(postcondition.operationID != 0)
            {
                return;
            }
            for(int k=1; k<belief_events.itemsAmount; k++)
            {
                Event *precondition = FIFO_GetKthNewestElement(&belief_events, k);
                //if it's an operation find the real precondition and use the current one as action
                int operationID = precondition->operationID;
                if(operationID != 0)
                {
                    if(k >= SEQUENCE_SPAN)
                    {
                        continue;
                    }
                    for(int j=k+1; j<belief_events.itemsAmount; j++)
                    {
                        precondition = FIFO_GetKthNewestElement(&belief_events, j);
                        if(precondition->operationID == 0)
                        {
                            RuleTable_Composition(currentTime, precondition, &postcondition, operationID);
                        }
                    }
                }
                else
                {
                    //RuleTable_Composition(currentTime, precondition, &postcondition, operationID); (A =/> B) not used yet
                    //anticipation below not supported yet
                }
            }
        }
    }
    //2. process oldest event
    if(belief_events.itemsAmount == FIFO_SIZE)
    {
        int k = belief_events.itemsAmount-1;
        Event *precondition = FIFO_GetKthNewestElement(&belief_events, k);
        int closest_concept_i;
        Concept *c = NULL;
        if(Memory_getClosestConcept(&precondition->sdr, precondition->sdr_hash, &closest_concept_i))
        {
            c = concepts.items[closest_concept_i].address;
            
        }
        for(int j=k-1; j>=0; j--)
        {
            for(int op_i=1; op_i<OPERATIONS_MAX; op_i++)
            {
                for(int prec_i=0; prec_i<c->precondition_beliefs[op_i].itemsAmount; prec_i++)
                {
                    Implication link = c->precondition_beliefs[op_i].array[prec_i];
                    //search for operator
                    for(int h=j-1; h>=j-1-SEQUENCE_SPAN; h--)
                    {
                        Event *potential_op = FIFO_GetKthNewestElement(&belief_events, h);
                        if(potential_op->operationID == op_i)
                        {
                            //make sure consequent does not exist
                            for(int l=h-1; l>=0; l--)
                            {
                                Event *postcondition = FIFO_GetKthNewestElement(&belief_events, l);
                                if(postcondition->sdr_hash == link.sdr_hash && SDR_Equal(&link.sdr, &postcondition->sdr))
                                {
                                    goto CONFIRM;
                                }
                            }
                            //Disappointment!
                            Implication negConfirmation = link;
                            negConfirmation.truth = (Truth) { .frequency = 0.0, .confidence = 0.001 };
                            negConfirmation.stamp = (Stamp) { .evidentalBase = {-stampID} };
                            Implication ret = Table_AddAndRevise(&c->precondition_beliefs[op_i], &negConfirmation, negConfirmation.debug);
                            IN_DEBUG(
                                printf("DISAPPOINTED %s\n", c->debug);
                                //for(int u = 0; u<c->precondition_beliefs[op_i].itemsAmount; u++)
                                //{
                                //    Implication_Print(&c->precondition_beliefs[op_i].array[u]);
                                //}
                                Implication_Print(&ret);
                                printf("END\n");
                                printf("______\n");
                                getchar();
                            )
                            
                            break; //disappoint once only for each precondition going out of scope
                        }
                    }
                    //Confirmation, nothing to be done
                    CONFIRM:;
                    IN_DEBUG(
                        printf("CONFIRMED %s\n", c->debug);
                        getchar();
                    )
                }
            }
        }
    }
    //invoke decision making for goals
    if(goal_events.itemsAmount > 0)
    {
        Event *goal = FIFO_GetNewestElement(&goal_events);
        if(!goal->processed)
        {
            ProcessEvent(goal, currentTime);
            Decision_Making(goal, currentTime);
        }
    }
}
