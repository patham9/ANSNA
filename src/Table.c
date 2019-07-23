#include "Table.h"

void Table_Add(Table *table, Implication *imp)
{
    double impTruthExp = Truth_Expectation(imp->truth);
    for(int i=0; i<TABLE_SIZE; i++)
    {
        bool same_term = (table->array[i].sdr_hash == imp->sdr_hash && SDR_Equal(&table->array[i].sdr,&imp->sdr));
        //either it's not yet full and we reached a new space,
        //or the term is different and the truth expectation is higher
        //or the term is the same and the confidence is higher
        if(i==table->itemsAmount || (!same_term && impTruthExp > Truth_Expectation(table->array[i].truth)) || (same_term && imp->truth.confidence > table->array[i].truth.confidence))
        {
            //ok here it has to go, move down the rest, evicting the last element if we hit TABLE_SIZE-1.
            for(int j=MIN(table->itemsAmount, TABLE_SIZE-1); j>i; j--)
            {
                table->array[j] = table->array[j-1];
            }
            table->array[i] = *imp;
            table->itemsAmount = MIN(table->itemsAmount+1, TABLE_SIZE);
            return;
        }
    }
}

Implication Table_AddAndRevise(Table *table, Implication *imp, char *debug)
{
    Implication RetRevised = (Implication) {0};
    //1. revise with items of same SDR only
    int best_i = -1;
    double best_confidence = 0.0;
    for(int i=0; i<table->itemsAmount; i++)
    {
        double cur_confidence = table->array[i].truth.confidence;
        if(cur_confidence > best_confidence && SDR_Equal(&imp->sdr, &table->array[i].sdr) && !Stamp_checkOverlap(&imp->stamp, &table->array[i].stamp))
        {
            best_i = i;
            best_confidence = cur_confidence;
        }
    }
    //2. if there was one, revise with it, and add the revised element, removing the old implication from the table
    if(best_i != -1)
    {
        Implication* OldImp = &table->array[best_i];
        Implication revised = Inference_ImplicationRevision(OldImp, imp);
        strcpy(revised.debug, debug);
        Implication_SetSDR(&revised, imp->sdr); //update sdr hash
        //printf("AAA %s  %.02f,%.02f\n", revised.debug, revised.truth.frequency, revised.truth.confidence); //++
        Table_Add(table, &revised);
        RetRevised = revised;
    }
    else //only if not revised for now
    {
        //3. add imp too:
        strcpy(imp->debug, debug);
        Table_Add(table, imp);
    }
    return RetRevised;
}

Implication Table_PopHighestTruthExpectationElement(Table *table)
{
    Implication result = table->array[0];
    for(int i=1; i<table->itemsAmount; i++)
    {
        table->array[i-1] = table->array[i];
    }
    table->itemsAmount--;
    return result;
}

void Table_COPY(Table *src, Table *target)
{
    target->itemsAmount = src->itemsAmount;
    for(int i=0; i<src->itemsAmount; i++)
    {
        target->array[i] = src->array[i];
    }
}
