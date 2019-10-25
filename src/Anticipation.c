#include "Anticipation.h"

int stampID = -1;
void Anticipation_Anticipate(int operationID, long currentTime)
{
    assert(operationID >= 0 && operationID < OPERATIONS_MAX, "Wrong operation id, did you inject an event manually?");
    for(int j=0; j<concepts.itemsAmount; j++)
    {
        Concept *postc = concepts.items[j].address;
        for(int  h=0; h<postc->precondition_beliefs[operationID].itemsAmount; h++)
        {
            Memory_RelinkImplication(&postc->precondition_beliefs[operationID].array[h]);
            Implication imp = postc->precondition_beliefs[operationID].array[h]; //(&/,a,op) =/> b.
            Concept *current_prec = imp.sourceConcept;
            Event *precondition = &current_prec->belief_spike; //a. :|:
            Event updated_precondition = Inference_EventUpdate(precondition, currentTime);
            if(precondition != NULL)
            {
                Event op = { .type = EVENT_TYPE_BELIEF,
                             .truth = ANSNA_DEFAULT_TRUTH,
                             .occurrenceTime = currentTime,
                             .operationID = operationID };
                Event seqop = Inference_BeliefIntersection(&updated_precondition, &op); //(&/,a,op). :|:
                Event result = Inference_BeliefDeduction(&seqop, &imp); //b. :/:
                if(Truth_Expectation(result.truth) > ANTICIPATION_THRESHOLD)
                {
                    Implication negative_confirmation = imp;
                    Truth TNew = { .frequency = 0.0, .confidence = ANTICIPATION_CONFIDENCE };
                    Truth TPast = Truth_Projection(precondition->truth, 0, imp.occurrenceTimeOffset);
                    negative_confirmation.truth = Truth_Eternalize(Truth_Induction(TPast, TNew));
                    negative_confirmation.stamp = (Stamp) { .evidentalBase = { -stampID } };
                    IN_DEBUG ( printf("ANTICIPATE %s, future=%ld \n", imp.debug, imp.occurrenceTimeOffset); )
                    assert(negative_confirmation.truth.confidence >= 0.0 && negative_confirmation.truth.confidence <= 1.0, "(666) confidence out of bounds");
                    for(int i=0; i<ANTICIPATIONS_MAX; i++)
                    {
                        if(postc->anticipations[i].deadline == 0 || currentTime + imp.occurrenceTimeOffsetMax <= postc->anticipations[i].deadline)
                        {
                            postc->anticipations[i].deadline = currentTime + imp.occurrenceTimeOffsetMax;
                            postc->anticipations[i].negative_confirmation = negative_confirmation;
                            postc->anticipations[i].operation_id = operationID;
                        }
                    }
                    stampID--;
                }
            }
        }
    }
}

void Anticipation_Disappoint(Concept *c, long currentTime)
{
    for(int j=0; j<ANTICIPATIONS_MAX; j++)
    {
        if(c->anticipations[j].deadline > 0)
        {
            IN_DEBUG ( printf("currentTime = %ld, deadline = %ld\n", currentTime, c->anticipations[j].deadline); )
            if(currentTime > c->anticipations[j].deadline)
            {
                Implication *added = Table_AddAndRevise(&c->precondition_beliefs[c->anticipations[j].operation_id], &c->anticipations[j].negative_confirmation, c->anticipations[j].negative_confirmation.debug);
                if(added != NULL)
                {
                    added->sourceConcept = c->anticipations[j].negative_confirmation.sourceConcept;
                    added->sourceConceptSDR = c->anticipations[j].negative_confirmation.sourceConceptSDR;
                }
                c->anticipations[j].deadline = 0;
                IN_DEBUG
                (
                    printf("DISAPPOINTED %s\n", c->anticipations[j].negative_confirmation.debug);
                    getchar();
                    puts("START");
                    for(int i=0; i<c->precondition_beliefs[c->anticipations[j].operation_id].itemsAmount; i++)
                    {
                        puts(c->precondition_beliefs[c->anticipations[j].operation_id].array[i].debug);
                        puts("");
                        Implication_Print(&c->precondition_beliefs[c->anticipations[j].operation_id].array[i]);
                    }
                    puts("ADDITION END");
                 )
            }   
        }
    }
}

void Anticipation_Confirm(Concept *c, Event *e)
{
    for(int i=0; i<ANTICIPATIONS_MAX; i++)
    {
        if(c->anticipations[i].deadline > 0 && e->type == EVENT_TYPE_BELIEF)
        {
            c->anticipations[i].deadline = 0;
            IN_DEBUG
            (
                printf("CONFIRMED %s\n", c->debug);
                getchar();
            )
        }
    }
}
