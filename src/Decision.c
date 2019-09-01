#include "Decision.h"

double MOTOR_BABBLING_CHANCE = MOTOR_BABBLING_CHANCE_INITIAL;
//Inject action event after execution or babbling
static void Decision_InjectActionEvent(Decision *decision)
{
    assert(decision->operationID > 0, "Operation 0 is reserved for no action");
    decision->op = operations[decision->operationID-1]; //TODO put into InjectActionEvent
    (*decision->op.action)();
    ANSNA_AddInputBelief(decision->op.sdr);
}

//"reflexes" to try different operations, especially important in the beginning
static Decision Decision_MotorBabbling()
{
    Decision decision = (Decision) {0};
    int n_ops = 0;
    for(int i=0; i<OPERATIONS_MAX && operations[i].action != 0; i++)
    {
        n_ops = i+1;
    }
    if(n_ops > 0)
    {
        decision.operationID = 1+(rand() % (n_ops));
        IN_DEBUG (
            printf(" ANSNA BABBLE %d\n", decision.operationID);
        )
        decision.execute = true;
    }
    return decision;
}

int stampID = -1;
Decision Decision_RealizeGoal(Event *goal, long currentTime)
{
    Decision decision = (Decision) {0};
    int closest_postc_i;
    if(Memory_getClosestConcept(&goal->sdr, goal->sdr_hash, &closest_postc_i))
    {
        Concept *postc = concepts.items[closest_postc_i].address;
        double bestTruthExpectation = 0;
        Implication bestImp = {0};
        Concept *prec;
        for(int opi=1; opi<OPERATIONS_MAX; opi++)
        {
            if(operations[opi-1].action == 0)
            {
                break;
            }
            for(int j=0; j<postc->precondition_beliefs[opi].itemsAmount; j++)
            {
                Implication imp = postc->precondition_beliefs[opi].array[j];
                IN_DEBUG
                (
                    printf("CONSIDERED IMPLICATION: %s\n", imp.debug);
                    SDR_Print(&imp.sdr);
                )
                //now look at how much the precondition is fulfilled
                Concept *current_prec = imp.sourceConcept;
                if(SDR_Equal(&current_prec->sdr, &imp.sourceConceptSDR))
                {
                    Event *precondition = &current_prec->belief_spike; //a. :|:
                    if(precondition != NULL)
                    {
                        Event ContextualOperation = Inference_GoalDeduction(goal, &imp); //(&/,a,op())! :\:
                        double operationGoalTruthExpectation = Truth_Expectation(Inference_OperationDeduction(&ContextualOperation, precondition, currentTime).truth); //op()! :|:
                        Inference_OperationDeduction(&ContextualOperation, precondition, currentTime);
                        if(operationGoalTruthExpectation > bestTruthExpectation)
                        {
                            IN_DEBUG
                            (
                                printf("CONSIDERED PRECON: %s\n", current_prec->debug);
                                fputs("CONSIDERED PRECON truth ", stdout);
                                Truth_Print(&precondition->truth);
                                fputs("CONSIDERED goal truth ", stdout);
                                Truth_Print(&goal->truth);
                                fputs("CONSIDERED imp truth ", stdout);
                                Truth_Print(&imp.truth);
                                printf("CONSIDERED time %ld\n", precondition->occurrenceTime);
                                SDR_Print(&current_prec->sdr);
                                SDR_Print(&precondition->sdr);
                            )
                            prec = current_prec;
                            bestImp = imp;
                            decision.operationID = opi;
                            bestTruthExpectation = operationGoalTruthExpectation;
                        }
                    }
                }
                else
                {
                    Table_Remove(&postc->precondition_beliefs[opi], j);
                    j--; //repeat iteration, with re-checking loop condition
                }
            }
        }
        IN_DEBUG ( printf("decision expectation %f impTruth=(%f, %f): %s\n", bestTruthExpectation, bestImp.truth.frequency, bestImp.truth.confidence, bestImp.debug); )
        if(bestTruthExpectation < DECISION_THRESHOLD)
        {
            return decision;
        }
        //ANTICIPATON (neg. evidence numbers for now)
        for(int i=0; i<ANTICIPATIONS_MAX; i++)
        {
            if(postc->anticipation_deadline[i] == 0)
            {
                //"compute amount of negative evidence based on current evidence" (Robert's estimation)
                //"we just take the counter and don't add one because we want to compute a w "unit" which will be revised"
                long countWithNegativeEvidence = bestImp.revisions;
                double negativeEvidenceRatio = 1.0 / ((double) countWithNegativeEvidence);
                //compute confidence by negative evidence
                double w = Truth_c2w(bestImp.truth.confidence) * negativeEvidenceRatio;
                double c = Truth_w2c(w);
                //deadline: predicted time + tolerance
                long pessimistic_stddev_estimate = bestImp.maxOccurrenceTimeOffset - bestImp.minOccurrenceTimeOffset;
                postc->anticipation_deadline[i] = currentTime + MAX(ANTICIPATION_MIN_WINDOW, ANTICIPATION_FORWARD_MUL * bestImp.occurrenceTimeOffset + pessimistic_stddev_estimate * ANTICIPATION_VARIANCE_MUL);
                postc->anticipation_negative_confirmation[i] = bestImp;
                postc->anticipation_negative_confirmation[i].truth = (Truth) { .frequency = 0.0, .confidence = c };
                postc->anticipation_negative_confirmation[i].stamp = (Stamp) { .evidentalBase = { -stampID } };
                postc->anticipation_operation_id[i] = decision.operationID;
                IN_DEBUG( printf("ANTICIPATE future=%ld tolerance=%ld\n", bestImp.occurrenceTimeOffset, pessimistic_stddev_estimate); )
                stampID--;
                break;
            }
        }
        //EMD anticipation
        IN_DEBUG
        (
            printf("%s %f,%f",bestImp.debug, bestImp.truth.frequency, bestImp.truth.confidence);
            puts("");
            printf("SELECTED PRECON: %s\n", prec->debug);
            puts(bestImp.debug); //++
            printf(" ANSNA TAKING ACTIVE CONTROL %d\n", decision.operationID);
        )
        decision.execute = true;
    }
    return decision;
}

bool Decision_Making(Event *goal, long currentTime)
{
    Decision decision = {0};
    //try motor babbling with a certain chance
    if(!decision.execute && rand() % 1000000 < (int)(MOTOR_BABBLING_CHANCE*1000000.0))
    {
        decision = Decision_MotorBabbling();
    }
    //try matching op if didn't motor babble
    if(!decision.execute)
    {
        decision = Decision_RealizeGoal(goal, currentTime);
    }
    if(decision.execute && decision.operationID)
    {
        Decision_InjectActionEvent(&decision);
    }
    return decision.execute;
}
