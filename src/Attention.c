#include "Attention.h"

Attention Attention_forgetEvent(Attention *taskAttention)
{
    return (Attention) { .priority   = taskAttention->priority * taskAttention->durability,
                         .durability = EVENT_DURABILITY };
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))
Attention Attention_forgetConcept(Attention *conceptAttention, Usage *conceptUsage, long currentTime)
{
    /*
     Usefulness affects absolute forgetting
     
     We use instead of a second queue in ALANN a lower priority barrier (very small) dependent on usefulness

     This way it only needs to order according to priority, and items with high usefulness stay even though not in the" attentional focus"
     (It's what Pei did for quality in OpenNARS)
     */
    
    double usefulness = Usage_usefulness(conceptUsage, currentTime);
    double lowerPriorityBarrier = usefulness * USEFULNESS_MAX_PRIORITY_BARRIER;
    return (Attention) { .priority = MAX(lowerPriorityBarrier, conceptAttention->priority * conceptAttention->durability),
                         .durability = CONCEPT_DURABILITY };
}

Attention Attention_activateConcept(Attention *conceptAttention, Attention *taskAttention)
{
    return (Attention) { .priority = or(conceptAttention->priority, taskAttention->priority),
                         .durability = CONCEPT_DURABILITY };
}

Attention Attention_deriveEvent(Attention *conceptAttention, Truth *truth)
{
    return (Attention) { .priority = conceptAttention->priority * Truth_Expectation(*truth),
                         .durability = EVENT_DURABILITY };
}

Attention Attention_inputEvent(Truth *truth)
{
    return (Attention) { .priority = Truth_Expectation(*truth),
                         .durability = EVENT_DURABILITY };
}
