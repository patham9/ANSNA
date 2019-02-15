#include "Cycle.h"

int eventsSelected = 0, eventsDerived = 0;
Event** selectedEvents[EVENT_SELECTIONS]; //better to be global
Concept** selectedConcepts[CONCEPT_SELECTIONS]; //too large to be a local array
Event derivations[MAX_DERIVATIONS];

//by doing inference with the highest priority concepts
void temporalInference(Concept *B, Event *b, long currentTime)
{
    int conceptsSelected = Memory_selectHighestPriorityConcepts(CONCEPT_SELECTIONS, selectedConcepts);
    for(int j=0; j<conceptsSelected; j++)
    {   
        Concept *A = selectedConcepts[j];
        IN_DEBUG( printf("Concept was chosen as temporal inference partner (equals matched? %d):\n", A->id == B->id); Concept_Print(A); )
        RuleTable_Composition(A, B, b, currentTime); // deriving a =/> b
        //forget concept
        A->attention = Attention_forgetConcept(&A->attention, &A->usage, currentTime);
    }
}

void cycle(long currentTime)
{
    eventsSelected = eventsDerived = 0;
    //retrieve k events from event queue (attention buffer)
    eventsSelected = Memory_selectHighestPriorityEvents(EVENT_SELECTIONS, selectedEvents);
    for(int i=0; i<eventsSelected; i++)
    {
        Event *e = selectedEvents[i];
        Event_SetSDR(e, e->sdr); // TODO make sure that hash needs to be calculated once instead already
        IN_DEBUG( printf("Event was selected:\n"); Event_Print(e); )
        //determine the concept it is related to
        int closest_concept_i;
        Decision decision = (Decision) {0};
        if(Memory_getClosestConcept(e, &closest_concept_i))
        {
            Concept *c = &concepts[closest_concept_i];
            //Matched event, see https://github.com/patham9/ANSNA/wiki/SDR:-SDRInheritance-for-matching,-and-its-truth-value
            Event eMatch = *e;
            eMatch.sdr = c->sdr;
            eMatch.truth = Truth_Deduction(SDR_Inheritance(&e->sdr, &c->sdr), e->truth);
            if(eMatch.truth.confidence > MIN_CONFIDENCE)
            {
                Concept_SDRInterpolation(c, &e->sdr, eMatch.truth); 
                //apply decomposition-based inference: prediction/explanation
                //if(currentTime - c->usage.lastUsed > CONCEPT_LATENCY_PERIOD)
                {
                    RuleTable_Decomposition(c, &eMatch, currentTime);
                }
                c->usage = Usage_use(&c->usage, currentTime);
                //add event to the FIFO of the concept
                FIFO *fifo =  e->type == EVENT_TYPE_BELIEF ? &c->event_beliefs : &c->event_goals;
                Event revised = FIFO_AddAndRevise(&eMatch, fifo);
                Event *goal = e->type == EVENT_TYPE_GOAL ? &eMatch : NULL;
                if(revised.type != EVENT_TYPE_DELETED && revised.truth.confidence >= MIN_CONFIDENCE && revised.attention.priority >= MIN_PRIORITY)
                {
                    goal = revised.type == EVENT_TYPE_GOAL ? &revised : NULL;
                    IN_OUTPUT( printf("REVISED EVENT: "); Event_Print(&revised); )
                }
                if(goal != NULL)
                {
                    decision = Decision_PotentiallyExecute(c, goal, currentTime);
                    //if no operation matched, try motor babbling with a certain chance
                    if(!decision.matched && !decision.executed)
                    {
                        if(rand() % 1000000 < (int)(MOTOR_BABBLING_CHANCE*1000000.0))
                        {
                            decision = Decision_MotorBabbling();
                        } 
                    }
                    if(decision.executed)
                    {
                        Decision_MotorTagging(c, decision.op);
                    }
                }
                //activate concepts attention with the event's attention, but penalize for mismatch to concept
                //eMatch.attention.priority *= Truth_Expectation(eMatch.truth);
                c->attention = Attention_activateConcept(&c->attention, &eMatch.attention);
            }
            //send event to the highest prioriry concepts
            temporalInference(c, e, currentTime); //c pointer should not be used after this position, as it involves push operation to concepts
        }
        if(!decision.matched && !Memory_FindConceptBySDR(&e->sdr, NULL)) //not conceptualizing (&/,a,op())
        {   //add a new concept for e too at the end, as it does not exist already
            Concept *eNativeConcept = Memory_Conceptualize(&e->sdr, &e->attention, currentTime);
            if(eNativeConcept != NULL)
            {
                IN_DEBUG( printf("ADDED CONCEPT \n"); )
                FIFO_Add(e, (e->type == EVENT_TYPE_BELIEF ? &eNativeConcept->event_beliefs : &eNativeConcept->event_goals));
            }
        }
        //forget event
        e->attention = Attention_forgetEvent(&e->attention, currentTime);
    }
}
