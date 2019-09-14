#include "Concept.h"

void Concept_SetSDR(Concept *concept, SDR sdr)
{
    concept->sdr = sdr;
    //Generate hash too:
    concept->sdr_hash = SDR_Hash(&sdr);
    //Initialize counter to what the sdr has:
    int k = 0;
    ITERATE_SDR_BITS(i,j,
        concept->sdr_bit_counter[k] = SDR_ReadBitInBlock(&sdr, i, j) ? 1.0 : -1.0;
        concept->sdr_bit_counter[k] *= CONCEPT_INTERPOLATION_INIT_STRENGTH;
        k++;
    )
}

void Concept_Print(Concept *concept)
{
    puts("Concept:");
    SDR_Print(&concept->sdr);
    Usage_Print(&concept->usage);
    puts("");
}

void Concept_SDRInterpolation(int layer, Concept *concept, SDR *eventSDR)
{
    int k = 0;
    bool rehash = false;
    double mul = CONCEPT_INTERPOLATION_STRENGTH;
    for(int i=0; i<((CONCEPT_LAYERS-1)-layer); i++)
    {
        mul *= CONCEPT_INTERPOLATION_LAYER_DECAY;
    }
    ITERATE_SDR_BITS(i,j,
        double oldValue = concept->sdr_bit_counter[k];
        double count = SDR_ReadBitInBlock(eventSDR,i,j) ? 1.0 : -1.0;
        concept->sdr_bit_counter[k] = MIN(CONCEPT_INTERPOLATION_COUNTER_MAX, MAX(CONCEPT_INTERPOLATION_COUNTER_MIN, concept->sdr_bit_counter[k]+mul * count));
        double newValue = concept->sdr_bit_counter[k];
        if(oldValue<0.5 && newValue >= 0.5)
        {
            SDR_WriteBit(&concept->sdr,k,1);
            rehash = true;
        }
        else
        if(oldValue>=0.5 && newValue < 0.5)
        {
            SDR_WriteBit(&concept->sdr,k,0);
            rehash = true;
        }
        k++;
    )
    if(rehash)
    {
        concept->sdr_hash = SDR_Hash(&concept->sdr);
    }
}


