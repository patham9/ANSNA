#include "Cycle.h"

static Event Increased_Action_Potential(Event *existing_potential, Event *incoming_spike, long currentTime)
{
    if(existing_potential->type == EVENT_TYPE_DELETED)
    {
        return *incoming_spike;
    }
    else
    {
        double expExisting = Truth_Expectation(Inference_EventUpdate(existing_potential, currentTime).truth);
        double expIncoming = Truth_Expectation(Inference_EventUpdate(incoming_spike, currentTime).truth);
        //check if there is evidental overlap
        bool overlap = Stamp_checkOverlap(&incoming_spike->stamp, &existing_potential->stamp);
        //if there is, apply choice, keeping the stronger one:
        if(overlap)
        {
            if(expIncoming > expExisting)
            {
                return *incoming_spike;
            }
        }
        else
        //and else revise, increasing the "activation potential"
        {
            Event revised_spike = Inference_EventRevision(existing_potential, incoming_spike);
            if(revised_spike.truth.confidence >= existing_potential->truth.confidence)
            {
                return revised_spike;
            }
            //lower, also use choice
            if(expIncoming > expExisting)
            {
                return *incoming_spike;
            }
        }
    }
    return *existing_potential;
}

static Event MatchEventToConcept(Concept *c, Event *e)
{
    Event eMatch = *e;
    eMatch.sdr = c->sdr;
    eMatch.truth = Truth_Deduction(SDR_Inheritance(&e->sdr, &c->sdr), e->truth);
    return eMatch;
}

//doing inference within the matched concept, returning the matched event
static Event LocalInference(Concept *c, Event *e, long currentTime)
{
    Concept_ConfirmAnticipation(c, e);
    //Matched event, see https://github.com/patham9/ANSNA/wiki/SDR:-SDRInheritance-for-matching,-and-its-truth-value
    Event eMatch = MatchEventToConcept(c, e);
    if(eMatch.truth.confidence > MIN_CONFIDENCE)
    {
        c->usage = Usage_use(&c->usage, currentTime);          //given its new role it should be doable to add a priorization mechanism to it
        //add event as spike to the concept:
        if(eMatch.type == EVENT_TYPE_BELIEF)
        {
            c->incoming_belief_spike = eMatch;
        }
        else
        {
            c->incoming_goal_spike = eMatch;
        }
    }
    return eMatch;
}

static Event ProcessEvent(Event *e, long currentTime)
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
        //perform concept-related inference
        eMatch = LocalInference(c, e, currentTime);
    }
    if(!Memory_FindConceptBySDR(&e->sdr, e->sdr_hash, NULL))
    {   
        //if different enough
        bool different_enough = true;
        if(c != NULL)
        {
            double novelty = 1.0 - Truth_Expectation(SDR_Similarity(&e->sdr, &eMatch.sdr));
            if(novelty < CONCEPT_FORMATION_NOVELTY)
            {
                different_enough = false;
            }
        }
        if(different_enough)
        {
            //add a new concept for e too at the end, as it does not exist already
            Concept *specialConcept = Memory_Conceptualize(&e->sdr);
            if(specialConcept != NULL && c != NULL)
            {
                //copy over all knowledge
                for(int i=0; i<OPERATIONS_MAX; i++)
                {
                    Table_COPY(&c->precondition_beliefs[i],  &specialConcept->precondition_beliefs[i]);
                    Table_COPY(&c->postcondition_beliefs[i], &specialConcept->postcondition_beliefs[i]);
                }
            }
        }
    }
    return eMatch;
}

void Cycle_Perform(long currentTime)
{    
    IN_DEBUG
    (
        for(int i=0; i<belief_events.itemsAmount; i++)
        {
            Event *ev = FIFO_GetKthNewestSequence(&belief_events, i, 0);
            puts(ev->debug);
            puts("");
        }
        printf("items amount: %d",belief_events.itemsAmount);
        getchar();
    )
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
                Event postcondition = ProcessEvent(toProcess, currentTime);
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
                                            RuleTable_Composition(precondition, &postcondition, operationID);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                RuleTable_Composition(precondition, &postcondition, operationID);
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
            ProcessEvent(goal, currentTime);
            Decision_Making(goal, currentTime);
            goal->processed = true;
        }
    }
    //process spikes
    if(PROPAGATE_GOAL_SPIKES)
    {
        //pass goal spikes on to the next
        for(int i=0; i<concepts.itemsAmount; i++)
        {
            Concept *c = concepts.items[i].address;
            if(c->goal_spike.type != EVENT_TYPE_DELETED || c->belief_spike.type != EVENT_TYPE_DELETED)
            {
                for(int opi=0; opi<OPERATIONS_MAX; opi++)
                {
                    for(int j=0; j<c->precondition_beliefs[opi].itemsAmount; j++)
                    {
                        SDR *preSDR = &c->precondition_beliefs[opi].array[j].sdr;
                        int closest_concept_i;
                        if(Memory_getClosestConcept(preSDR, SDR_Hash(preSDR), &closest_concept_i)) //todo cache it, maybe in the function
                        {
                            Concept *pre = concepts.items[closest_concept_i].address;
                            if(c->goal_spike.type != EVENT_TYPE_DELETED)
                            {
                                Event incoming_spike = Inference_GoalDeduction(&c->goal_spike, &c->precondition_beliefs[opi].array[j]);
                                pre->incoming_goal_spike = Increased_Action_Potential(&pre->incoming_goal_spike, &incoming_spike, currentTime);
                            }
                        }
                    }
                    for(int j=0; j<c->postcondition_beliefs[opi].itemsAmount; j++)
                    {
                        SDR *postSDR = &c->postcondition_beliefs[opi].array[j].sdr;
                        int closest_concept_i;
                        if(Memory_getClosestConcept(postSDR, SDR_Hash(postSDR), &closest_concept_i)) //todo cache it, maybe in the function
                        {
                            Concept *post = concepts.items[closest_concept_i].address;
                            if(c->goal_spike.type != EVENT_TYPE_DELETED)
                            {
                                Event incoming_spike = Inference_GoalAbduction(&c->goal_spike, &c->postcondition_beliefs[opi].array[j]);
                                post->incoming_goal_spike = Increased_Action_Potential(&post->incoming_goal_spike, &incoming_spike, currentTime);
                            }
                        }
                    }
                }
            }
        }
        //process incoming goal spikes, invoking potential operations
        for(int i=0; i<concepts.itemsAmount; i++)
        {
            Concept *c = concepts.items[i].address;
            if(c->incoming_goal_spike.type != EVENT_TYPE_DELETED)
            {
                c->goal_spike = Increased_Action_Potential(&c->goal_spike, &c->incoming_goal_spike, currentTime);
                if(c->goal_spike.type != EVENT_TYPE_DELETED && !c->goal_spike.processed)
                {
                    c->goal_spike.processed = true;
                    Decision_Making(&c->goal_spike, currentTime);
                }
            }
            if(c->incoming_belief_spike.type != EVENT_TYPE_DELETED)
            {
                c->belief_spike = Increased_Action_Potential(&c->belief_spike, &c->incoming_belief_spike, currentTime);
                if(c->belief_spike.type != EVENT_TYPE_DELETED && !c->belief_spike.processed)
                {
                    c->belief_spike.processed = true;
                }
            }
            c->incoming_goal_spike = (Event) {0};
        }
    }

    //TODO same for belief spikes, but no triggering of decision making, just updating beliefSpike with same policy
    //This will allow it to be better prepared for the future (for instance when observations are missing), though for simple experiments it might not make much difference,
    //but without goal spikes, multistep procedure learning is difficult
    //Re-sort queue
    for(int i=0; i<concepts.itemsAmount; i++)
    {
        Concept *concept = concepts.items[i].address;
        //usefulness was changed, 
        PriorityQueue_PopAt(&concepts, i, NULL);
        Memory_addConcept(concept, currentTime);
    }
}
