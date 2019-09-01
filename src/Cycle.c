#include "Cycle.h"

//doing inference within the matched concept, returning the matched event
static Event Cycle_ActivateConcept(Concept *c, Event *e, long currentTime)
{
    Concept_ConfirmAnticipation(c, e);
    //Matched event, see https://github.com/patham9/ANSNA/wiki/SDR:-SDRInheritance-for-matching,-and-its-truth-value
    Event eMatch = Memory_MatchEventToConcept(c, e);
    if(eMatch.truth.confidence > MIN_CONFIDENCE)
    {
        c->usage = Usage_use(&c->usage, currentTime);          //given its new role it should be doable to add a priorization mechanism to it
        //add event as spike to the concept:
        if(eMatch.type == EVENT_TYPE_BELIEF)
        {
            c->belief_spike = eMatch;
        }
        else
        {
            //pass spike if the concept doesn't have a satisfying motor command
            if(!Decision_Making(&eMatch, currentTime))
            {
                c->incoming_goal_spike = eMatch;
            }
            else
            {
                e->propagated = true;
            }
        }
    }
    return eMatch;
}

//Process an event, by crearting a concept, or activating an existing
static Event Cycle_ProcessEvent(Event *e, long currentTime)
{
    e->processed = true;
    Event_SetSDR(e, e->sdr); // TODO make sure that hash needs to be calculated once instead already
    IN_DEBUG( puts("Event was selected:"); Event_Print(e); )
    //determine the concept it is related to
    int closest_concept_i;
    Concept *c = NULL;
    Event eMatch = {0};
    if(Memory_getClosestConcept(&e->sdr, e->sdr_hash, &closest_concept_i))
    {
        c = concepts.items[closest_concept_i].address;
        eMatch = Cycle_ActivateConcept(c, e, currentTime);
    }
    Concept *specialConceptCreated = NULL;
    if(Memory_EventIsNovel(e, c))
    {
        //add a new concept for e too at the end, as it does not exist already
        specialConceptCreated = Memory_Conceptualize(&e->sdr);
        if(c != NULL && specialConceptCreated != NULL)
        {
            //copy over all knowledge
            for(int i=0; i<OPERATIONS_MAX; i++)
            {
                Table_COPY(&c->precondition_beliefs[i], &specialConceptCreated->precondition_beliefs[i]);
            }
        }
    } 
    if(c != NULL && !SDR_Equal(&e->sdr, &c->sdr))
    {
        //generalization by creating intersection sdr
        SDR general_sdr = SDR_Intersection(&e->sdr, &c->sdr);
        //then creating event with it to see whether it is novel
        //if so link to new, else to existing
        Event e_common = *e;
        e_common.sdr = general_sdr;
        int concept_i;
        Concept *general_c = NULL;
        if(specialConceptCreated != NULL &&
           Memory_getClosestConcept(&general_sdr, SDR_Hash(&general_sdr), &concept_i))
        {
            Concept *general_c_existing = concepts.items[concept_i].address;
            if(specialConceptCreated != general_c_existing)
            {
                general_c = general_c_existing;
                if(Memory_EventIsNovel(&e_common, general_c))
                {
                    Concept *general_c_created = Memory_Conceptualize(&general_sdr);
                    if(general_c_created != NULL)
                    {
                        general_c = general_c_created;
                    }
                }
                if(specialConceptCreated != general_c)
                {
                    specialConceptCreated->parent_c = general_c;
                }
            }
        }
        //now pass "spike" up the hierachy
        while(general_c != NULL && e->type == EVENT_TYPE_BELIEF)
        {
            e_common.sdr = general_c->sdr;
            Memory_addEvent(&e_common); //same evidental base, but SDR changes
            Cycle_Perform(currentTime);
            general_c = general_c->parent_c;
        }
    }
    return eMatch;
}

//Propagate spikes for subgoal processing, generating anticipations and decisions
static void Cycle_PropagateSpikes(long currentTime)
{
    //process spikes
    if(PROPAGATE_GOAL_SPIKES)
    {
        //pass goal spikes on to the next
        for(int i=0; i<concepts.itemsAmount; i++)
        {
            Concept *postc = concepts.items[i].address;
            if(postc->goal_spike.type != EVENT_TYPE_DELETED && !postc->goal_spike.propagated && Truth_Expectation(postc->goal_spike.truth) > PROPAGATION_TRUTH_EXPECTATION_THRESHOLD)
            {
                for(int opi=0; opi<OPERATIONS_MAX; opi++)
                {
                    for(int j=0; j<postc->precondition_beliefs[opi].itemsAmount; j++)
                    {
                        Implication *imp = &postc->precondition_beliefs[opi].array[j];
                        Concept *pre = imp->sourceConcept; //concepts.items[closest_concept_i].address; //TODO check if still the same concept!
                        if(pre->incoming_goal_spike.type == EVENT_TYPE_DELETED || pre->incoming_goal_spike.processed)
                        {
                            if(SDR_Equal(&pre->sdr, &imp->sourceConceptSDR))
                            {
                                pre->incoming_goal_spike = Inference_GoalDeduction(&postc->goal_spike, &postc->precondition_beliefs[opi].array[j]);
                            }
                            else
                            {
                                Table_Remove(&postc->precondition_beliefs[opi], j);
                                j--; //repeat iteration, with re-checking loop condition
                            }
                        }
                    }
                }
            }
            postc->goal_spike.propagated = true;
        }
        //process incoming goal spikes, invoking potential operations
        for(int i=0; i<concepts.itemsAmount; i++)
        {
            Concept *c = concepts.items[i].address;
            if(c->incoming_goal_spike.type != EVENT_TYPE_DELETED)
            {
                c->goal_spike = Inference_IncreasedActionPotential(&c->goal_spike, &c->incoming_goal_spike, currentTime);
                if(c->goal_spike.type != EVENT_TYPE_DELETED && !c->goal_spike.processed && Truth_Expectation(c->goal_spike.truth) > PROPAGATION_TRUTH_EXPECTATION_THRESHOLD)
                {
                    Cycle_ProcessEvent(&c->goal_spike, currentTime);
                }
            }
            c->incoming_goal_spike = (Event) {0};
        }
    }
}

//Reinforce link between concept a and b (creating it if non-existent)
static void Cycle_ReinforceLink(Event *a, Event *b, int operationID)
{   
    if(a->type != EVENT_TYPE_BELIEF || b->type != EVENT_TYPE_BELIEF)
    {
        return;
    }
    int AConceptIndex;
    int BConceptIndex;
    if(Memory_getClosestConcept(&a->sdr, a->sdr_hash, &AConceptIndex) &&
       Memory_getClosestConcept(&b->sdr, b->sdr_hash, &BConceptIndex))
    {
        Concept *A = concepts.items[AConceptIndex].address;
        Concept *B = concepts.items[BConceptIndex].address;
        if(A != B)
        {
            //temporal induction
            if(!Stamp_checkOverlap(&a->stamp, &b->stamp))
            {
                
                Implication precondition_implication = Inference_BeliefInduction(a, b);
                precondition_implication.sourceConcept = A;
                precondition_implication.sourceConceptSDR = A->sdr;
                if(precondition_implication.truth.confidence >= MIN_CONFIDENCE)
                {
                    char debug[100];
                    sprintf(debug, "<(&/,%s,^op%d()) =/> %s>.",a->debug, operationID, b->debug);
                    IN_DEBUG ( if(operationID != 0) { puts(debug); Truth_Print(&precondition_implication.truth); puts("\n"); getchar(); } )
                    IN_OUTPUT( fputs("Formed implication: ", stdout); Implication_Print(&precondition_implication); )
                    Implication *revised_precon = Table_AddAndRevise(&B->precondition_beliefs[operationID], &precondition_implication, debug);
                    if(revised_precon != NULL)
                    {
                        revised_precon->sourceConcept = A;
                        revised_precon->sourceConceptSDR = A->sdr;
                        IN_OUTPUT( if(revised_precon->sdr_hash != 0) { fputs("REVISED pre-condition implication: ", stdout); Implication_Print(revised_precon); } )
                    }
                }
            }
        }
    }
}

void Cycle_Perform(long currentTime)
{    
    //process anticipation
    for(int i=0; i<concepts.itemsAmount; i++)
    {
        Concept_CheckAnticipationDisappointment(concepts.items[i].address, currentTime);
    }
    //1. process newest event
    if(belief_events.itemsAmount > 0)
    {
        //form concepts for the sequences of different length
        for(int len=0; len<MAX_SEQUENCE_LEN; len++)
        {
            Event *toProcess = FIFO_GetNewestSequence(&belief_events, len);
            if(!toProcess->processed)
            {
                //the matched event becomes the postcondition
                Event postcondition = Cycle_ProcessEvent(toProcess, currentTime);
                //Mine for <(&/,precondition,operation) =/> postcondition> patterns in the FIFO:
                if(len == 0) //postcondition always len1
                {  
                    if(postcondition.operationID != 0)
                    {
                        return;
                    }
                    for(int k=1; k<belief_events.itemsAmount; k++)
                    {
                        for(int len2=0; len2<MAX_SEQUENCE_LEN; len2++)
                        {
                            Event *precondition = FIFO_GetKthNewestSequence(&belief_events, k, len2);
                            //if it's an operation find the real precondition and use the current one as action
                            int operationID = precondition->operationID;
                            if(operationID != 0) //also meaning len2==0
                            {
                                for(int j=k+1; j<belief_events.itemsAmount; j++)
                                {
                                    for(int len3=0; len3<MAX_SEQUENCE_LEN; len3++)
                                    {
                                        precondition = FIFO_GetKthNewestSequence(&belief_events, j, len3);
                                        if(precondition->operationID == 0)
                                        {
                                            Cycle_ReinforceLink(precondition, &postcondition, operationID);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                Cycle_ReinforceLink(precondition, &postcondition, operationID);
                            }
                        }
                    }
                }
            }
        }
    }
    //process goals
    if(goal_events.itemsAmount > 0)
    {
        Event *goal = FIFO_GetNewestSequence(&goal_events, 0);
        if(!goal->processed)
        {
            Cycle_ProcessEvent(goal, currentTime);
        }
    }
    Cycle_PropagateSpikes(currentTime);
    //Re-sort queue
    for(int i=0; i<concepts.itemsAmount; i++)
    {
        Concept *concept = concepts.items[i].address;
        //usefulness was changed, 
        PriorityQueue_PopAt(&concepts, i, NULL);
        Memory_addConcept(concept, currentTime);
    }
}
