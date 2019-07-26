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
    int closest_postcon_concept_i;
    if(Memory_getClosestConcept(&goal->sdr, goal->sdr_hash, &closest_postcon_concept_i))
    {
        Concept *postcon_c = concepts.items[closest_postcon_concept_i].address;
        double bestTruthExpectation = 0;
        double bestValue = 0;
        Implication bestImp = {0};
        Concept *precon_concept;
        for(int i=1; i<OPERATIONS_MAX; i++)
        {
            if(operations[i-1].action == 0)
            {
                break;
            }
            for(int j=0; j<postcon_c->precondition_beliefs[i].itemsAmount; j++)
            {
                Implication imp = postcon_c->precondition_beliefs[i].array[j];
                IN_DEBUG
                (
                    printf("CONSIDERED IMPLICATION: %s\n", imp.debug);
                    SDR_PrintWhereTrue(&imp.sdr);
                )
                //now look at how much the precondition is fulfilled
                int closest_precon_concept_i;
                if(Memory_getClosestConcept(&imp.sdr, imp.sdr_hash, &closest_precon_concept_i))
                {
                    Concept * current_precon_c = concepts.items[closest_precon_concept_i].address;
                    Event * precondition = &current_precon_c->belief_spike; //a. :|:
                    if(precondition != NULL)
                    {
                        double context_penalty = 1.0;
                        if(LINK_CONTEXT)
                        {
                            context_penalty = Truth_Expectation(SDR_Inheritance(&precondition->context, &imp.context));
                        }
                        Event ContextualOperation = Inference_GoalDeduction(goal, &imp); //(&/,a,op())!
                        ContextualOperation.truth = Truth_Projection(ContextualOperation.truth, ContextualOperation.occurrenceTime, currentTime);
                        double operationGoalTruthExpectation = Truth_Expectation(Truth_Deduction(ContextualOperation.truth, Truth_Projection(precondition->truth, precondition->occurrenceTime, currentTime))); //op()! //TODO project to now
                        double value = context_penalty * operationGoalTruthExpectation;
                        if(value > bestValue)
                        {
                            IN_DEBUG
                            (
                                printf("CONSIDERED PRECON: %s\n", current_precon_c->debug);
                                fputs("CONSIDERED PRECON truth ", stdout);
                                Truth_Print(&precondition->truth);
                                fputs("CONSIDERED goal truth ", stdout);
                                Truth_Print(&goal->truth);
                                fputs("CONSIDERED imp truth ", stdout);
                                Truth_Print(&imp.truth);
                                printf("CONSIDERED time %d\n", (int)precondition->occurrenceTime);
                                SDR_PrintWhereTrue(&current_precon_c->sdr);
                                SDR_PrintWhereTrue(&precondition->sdr);
                            )
                            precon_concept = current_precon_c;
                            bestImp = imp;
                            decision.operationID = i;
                            bestTruthExpectation = operationGoalTruthExpectation;
                            bestValue = value;
                        }
                    }
                }
            }
        }
        printf("decision expectation %f impTruth=(%f, %f): %s\n", bestTruthExpectation, bestImp.truth.frequency, bestImp.truth.confidence, bestImp.debug);
        if(bestTruthExpectation < DECISION_THRESHOLD)
        {
            return decision;
        }
        //ANTICIPATON (neg. evidence numbers for now)
        for(int i=0; i<ANTICIPATIONS_MAX; i++)
        {
            if(postcon_c->anticipation_deadline[i] == 0)
            {
                postcon_c->anticipation_deadline[i] = currentTime + bestImp.occurrenceTimeOffset * ANTICIPATION_FORWARD + bestImp.variance * ANTICIPATION_WINDOW;
                postcon_c->anticipation_negative_confirmation[i] = bestImp;
                postcon_c->anticipation_negative_confirmation[i].truth = (Truth) { .frequency = 0.0, .confidence = ANTICIPATION_CONFIDENCE };
                postcon_c->anticipation_negative_confirmation[i].stamp = (Stamp) { .evidentalBase = {-stampID} };
                postcon_c->anticipation_operation_id[i] = decision.operationID;
                IN_DEBUG( printf("ANTICIPATE future=%ld variance=%ld\n", bestImp.occurrenceTimeOffset,bestImp.variance); )
                stampID--;
            }
        }
        //EMD anticipation
        IN_DEBUG
        (
            printf("%s %f,%f",bestImp.debug, bestImp.truth.frequency, bestImp.truth.confidence);
            puts("");
            printf("SELECTED PRECON: %s\n", precon_concept->debug);
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
