#include "Implication.h"

void Implication_SetSDR(Implication *implication, SDR sdr)
{
    implication->sdr = sdr;
    //Generate hash too:
    implication->sdr_hash = SDR_Hash(&sdr);
}

void Implication_Print(Implication *implication)
{
    puts("Implication:");
    SDR_PrintWhereTrue(&implication->sdr);
    Truth_Print(&implication->truth);
    Stamp_print(&implication->stamp);
    printf("occurrenceTimeOffset=%ld\n\n", implication->occurrenceTimeOffset);
}

void Implication_ContextSDRInterpolation(Implication *target, Implication *a, Implication *b)
{
    double u1 = Truth_Expectation(a->truth);
    double u2 = Truth_Expectation(a->truth);
    int k = 0;
    ITERATE_SDR_BITS(i,j,
        target->context_sdr_bit_counter[k] = (u1*a->context_sdr_bit_counter[k] + u2*b->context_sdr_bit_counter[k])/(u1+u2);
        double newValue = target->context_sdr_bit_counter[k];
        if(newValue >= 0.5)
        {
            SDR_WriteBit(&target->context, k, 1);
        }
        else
        {
            SDR_WriteBit(&target->context, k, 0);
        }
        k++;
    )
}
