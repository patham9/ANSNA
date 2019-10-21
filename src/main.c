#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "SDR.h"
#include "Memory.h"
#include "Encode.h"
#include "ANSNA.h"

void SDR_Test()
{
    puts(">>SDR Test Start");
    puts("testing encoding and permutation:");
    SDR mySDR = Encode_Term("term1");
    int previous = SDR_ReadBit(&mySDR, 0);
    SDR_WriteBit(&mySDR, 0, previous);
    assert(SDR_ReadBit(&mySDR, 0) == previous, "SDR was written to with same value, shouldn't have changed");
    assert(SDR_CountTrue(&mySDR) == TERM_ONES, "SDR term should have TERM_ONES ones");
    SDR sdr2 = SDR_PermuteByRotation(&mySDR, true);
    SDR mySDR_Recons1 = SDR_PermuteByRotation(&sdr2, false);
    assert(SDR_Equal(&mySDR_Recons1, &mySDR), "Inverse rotation should lead to original result");
    int perm[SDR_SIZE];
    int perm_inv[SDR_SIZE];
    SDR_GeneratePermutation(perm, perm_inv);
    SDR sdr3 = SDR_Permute(&mySDR, perm);
    SDR mySDR_recons2 = SDR_Permute(&sdr3, perm_inv);
    assert(SDR_Equal(&mySDR_recons2, &mySDR), "Inverse permutation should lead to original result");
    puts("testing tuples now:");
    SDR mySDR2 = Encode_Term("term2");
    fputs("recons:", stdout);
    SDR tuple = SDR_Tuple(&mySDR, &mySDR2);
    SDR SDR1Recons = SDR_TupleGetFirstElement(&tuple, &mySDR2);
    SDR SDR2Recons = SDR_TupleGetSecondElement(&tuple, &mySDR);
    Truth selfTest = SDR_Similarity(&mySDR, &mySDR);
    printf("sdr1 sdr1 similarity: %f %f\n", selfTest.frequency, selfTest.confidence);
    assert(selfTest.frequency == 1, "No negative evidence is allowed to be found when matching to itself");
    Truth t1 = SDR_Similarity(&mySDR, &SDR1Recons);
    assert(t1.frequency == 1, "Reconstructed tuple element1 should be almost the same as the original");
    Truth t2 = SDR_Similarity(&mySDR2, &SDR2Recons);
    assert(t2.frequency == 1, "Reconstructed tuple element2 should be almost the same as the original");
    Truth t3 = SDR_Similarity(&mySDR, &SDR2Recons);
    assert(t3.frequency < 0.5, "These elements should mostly differ");
    Truth t4 = SDR_Similarity(&mySDR2, &SDR1Recons);
    assert(t4.frequency < 0.5, "These elements should mostly differ too");
    puts("<<SDR Test successful");
}

void FIFO_Test()
{
    puts(">>FIFO test start");
    FIFO fifo = {0};
    //First, evaluate whether the fifo works, not leading to overflow
    for(int i=FIFO_SIZE*2; i>=1; i--) //"rolling over" once by adding a k*FIFO_Size items
    {
        Event event1 = (Event) { .sdr = Encode_Term("test"), 
                                 .type = EVENT_TYPE_BELIEF, 
                                 .truth = {.frequency = 1.0, .confidence = 0.9},
                                 .stamp = (Stamp) { .evidentalBase = {i} }, 
                                 .occurrenceTime = FIFO_SIZE*2-i*10 };
        FIFO_Add(&event1, &fifo);
    }
    for(int i=0; i<FIFO_SIZE; i++)
    {
        assert(FIFO_SIZE-i == fifo.array[0][i].stamp.evidentalBase[0], "Item at FIFO position has to be right");
    }
    //now see whether a new item is revised with the correct one:
    int i=3; //revise with item 10, which has occurrence time 10
    int newbase = FIFO_SIZE*2+1;
    Event event2 = (Event) { .sdr = Encode_Term("test"), 
                             .type = EVENT_TYPE_BELIEF, 
                             .truth = {.frequency = 1.0, .confidence = 0.9},
                             .stamp = (Stamp) { .evidentalBase = {newbase} }, 
                             .occurrenceTime = i*10+3 };
    FIFO fifo2 = {0};
    for(int i=0; i<FIFO_SIZE*2; i++)
    {
        SDR zero = (SDR) {0};
        event2.occurrenceTime += 1;
        FIFO_Add(&event2, &fifo2);
        if(i < FIFO_SIZE && i < MAX_SEQUENCE_LEN)
        {
            char buf[100]; 
            Event *ev = FIFO_GetKthNewestSequence(&fifo2, 0, i);
            sprintf(buf,"This event SDR is not allowed to be zero, sequence length=%d\n",i+1);
            assert(!SDR_Equal(&zero, &ev->sdr),buf);
        }
    }
    assert(fifo2.itemsAmount == FIFO_SIZE, "FIFO size differs");
    puts("<<FIFO Test successful");
}

void Stamp_Test()
{
    puts(">>Stamp test start");
    Stamp stamp1 = (Stamp) { .evidentalBase = {1,2} };
    Stamp_print(&stamp1);
    Stamp stamp2 = (Stamp) { .evidentalBase = {2,3,4} };
    Stamp_print(&stamp2);
    Stamp stamp3 = Stamp_make(&stamp1, &stamp2);
    fputs("zipped:", stdout);
    Stamp_print(&stamp3);
    assert(Stamp_checkOverlap(&stamp1,&stamp2) == true, "Stamp should overlap");
    puts("<<Stamp test successful");
}

void PriorityQueue_Test()
{
    puts(">>PriorityQueue test start");
    PriorityQueue queue;
    int n_items = 10;
    Item items[n_items];
    for(int i=0; i<n_items; i++)
    {
        items[i].address = (void*) ((long) i+1);
        items[i].priority = 0;
    }
    PriorityQueue_RESET(&queue, items, n_items);
    for(int i=0, evictions=0; i<n_items*2; i++)
    {
        PriorityQueue_Push_Feedback feedback = PriorityQueue_Push(&queue, 1.0/((double) (n_items*2-i)));
        if(feedback.added)
        {
            printf("item was added %f %ld\n", feedback.addedItem.priority, (long)feedback.addedItem.address);
        }
        if(feedback.evicted)
        {
            printf("evicted item %f %ld\n", feedback.evictedItem.priority, (long)feedback.evictedItem.address);
            assert(evictions>0 || feedback.evictedItem.priority == ((double)(1.0/((double) (n_items*2)))), "the evicted item has to be the lowest priority one");
            assert(queue.itemsAmount < n_items+1, "eviction should only happen when full!");
            evictions++;
        }
    }
    puts("<<PriorityQueue test successful");
}

void Table_Test()
{
    puts(">>Table test start");
    Table table = {0};
    for(int i=TABLE_SIZE*2; i>=1; i--)
    {
        Implication imp = (Implication) { .sdr = Encode_Scalar(1,TABLE_SIZE*2,i), 
                                          .truth = (Truth) { .frequency = 1.0, .confidence = 1.0/((double)(i+1)) },
                                          .stamp = (Stamp) { .evidentalBase = {i} },
                                          .occurrenceTimeOffset = 10,
                                          .revisions = 1 };
        Table_Add(&table, &imp);
    }
    for(int i=0; i<TABLE_SIZE; i++)
    {
        assert(i+1 == table.array[i].stamp.evidentalBase[0], "Item at table position has to be right");
    }
    Implication imp = (Implication) { .sdr = Encode_Term("test"), 
                                      .truth = (Truth) { .frequency = 1.0, .confidence = 0.9},
                                      .stamp = (Stamp) { .evidentalBase = {TABLE_SIZE*2+1} },
                                      .occurrenceTimeOffset = 10,
                                      .revisions = 1 };
    assert(table.array[0].truth.confidence==0.5, "The highest confidence one should be the first.");
    Table_AddAndRevise(&table, &imp, "");
    assert(table.array[0].truth.confidence>0.5, "The revision result should be more confident than the table element that existed.");
    puts("<<Table test successful");
}

void Memory_Test()
{
    ANSNA_INIT();
    puts(">>Memory test start");
    Event e = Event_InputEvent(Encode_Term("a"), 
                               EVENT_TYPE_BELIEF, 
                               (Truth) {.frequency = 1, .confidence = 0.9}, 
                               1337);
    Memory_addEvent(&e);
    assert(belief_events.array[0][0].truth.confidence == (double) 0.9, "event has to be there"); //identify
    int returnIndex;
    assert(!Memory_getClosestConcept(&e.sdr, e.sdr_hash, &returnIndex), "a concept doesn't exist yet!");
    Memory_Conceptualize(&e.sdr);
    int concept_i;
    assert(Memory_FindConceptBySDR(&e.sdr, SDR_Hash(&e.sdr), &concept_i), "Concept should have been created!");
    Concept *c = concepts.items[concept_i].address;
    assert(Memory_FindConceptBySDR(&e.sdr, e.sdr_hash, &returnIndex), "Concept should be found!");
    assert(c == concepts.items[returnIndex].address, "e should match to c!");
    assert(Memory_getClosestConcept(&e.sdr, e.sdr_hash, &returnIndex), "Concept should be found!");
    assert(c == concepts.items[returnIndex].address, "e should match to c!");
    Event e2 = Event_InputEvent(Encode_Term("b"), 
                               EVENT_TYPE_BELIEF, 
                               (Truth) {.frequency = 1, .confidence = 0.9}, 
                               1337);
    Memory_addEvent(&e2);
    Memory_Conceptualize(&e2.sdr);
    assert(Memory_FindConceptBySDR(&e2.sdr, SDR_Hash(&e2.sdr), &concept_i), "Concept should have been created!");
    Concept *c2 = concepts.items[concept_i].address;
    Concept_Print(c2);
    assert(Memory_getClosestConcept(&e2.sdr, e2.sdr_hash, &returnIndex), "Concept should be found!");
    assert(c2 == concepts.items[returnIndex].address, "e2 should closest-match to c2!");
    assert(Memory_getClosestConcept(&e.sdr, e.sdr_hash, &returnIndex), "Concept should be found!");
    assert(c == concepts.items[returnIndex].address, "e should closest-match to c!");
    puts("<<Memory test successful");
}

void ANSNA_Alphabet_Test()
{
    ANSNA_INIT();
    puts(">>ANSNA Alphabet test start");
    ANSNA_AddInput(Encode_Term("a"), EVENT_TYPE_BELIEF, ANSNA_DEFAULT_TRUTH);
    for(int i=0; i<50; i++)
    {
        int k=i%10;
        if(i % 3 == 0)
        {
            char c[2] = {'a'+k,0};
            ANSNA_AddInput(Encode_Term(c), EVENT_TYPE_BELIEF, ANSNA_DEFAULT_TRUTH);
        }
        ANSNA_Cycles(1);
        puts("TICK");
    }
    puts("<<ANSNA Alphabet test successful");
}

bool ANSNA_Procedure_Test_Op_executed = false;
void ANSNA_Procedure_Test_Op()
{
    puts("op executed by ANSNA");
    ANSNA_Procedure_Test_Op_executed = true;
}
void ANSNA_Procedure_Test()
{
    ANSNA_INIT();
    puts(">>ANSNA Procedure test start");
    ANSNA_AddOperation(Encode_Term("op"), ANSNA_Procedure_Test_Op); 
    ANSNA_AddInputBelief(Encode_Term("a"));
    ANSNA_Cycles(10);
    puts("---------------");
    ANSNA_AddInputBelief(Encode_Term("op"));
    ANSNA_Cycles(10);
    puts("---------------");
    ANSNA_AddInputBelief(Encode_Term("result"));
    ANSNA_Cycles(10);
    puts("---------------");
    ANSNA_AddInputBelief(Encode_Term("a"));
    ANSNA_Cycles(10);
    puts("---------------");
    ANSNA_AddInputGoal(Encode_Term("result"));
    ANSNA_Cycles(10);
    puts("---------------");
    assert(ANSNA_Procedure_Test_Op_executed, "ANSNA should have executed op!");
    puts("<<ANSNA Procedure test successful");
}

bool ANSNA_Follow_Test_Left_executed = false;
void ANSNA_Follow_Test_Left()
{
    puts("left executed by ANSNA");
    ANSNA_Follow_Test_Left_executed = true;
}
bool ANSNA_Follow_Test_Right_executed = false;
void ANSNA_Follow_Test_Right()
{
    puts("right executed by ANSNA");
    ANSNA_Follow_Test_Right_executed = true;
}
void ANSNA_Follow_Test()
{
    OUTPUT = 0;
    ANSNA_INIT();
    puts(">>ANSNA Follow test start");
    ANSNA_AddOperation(Encode_Term("op_left"), ANSNA_Follow_Test_Left); 
    ANSNA_AddOperation(Encode_Term("op_right"), ANSNA_Follow_Test_Right); 
    int simsteps = 1000000;
    int LEFT = 0;
    int RIGHT = 1;
    int BALL = RIGHT;
    int score = 0;
    int goods = 0;
    int bads = 0;
    for(int i=0;i<simsteps; i++)
    {
        puts(BALL == LEFT ? "LEFT" : "RIGHT");
        ANSNA_AddInputBelief(BALL == LEFT ? Encode_Term("ball_left") : Encode_Term("ball_right"));
        ANSNA_AddInputGoal(Encode_Term("good_ansna"));
        if(ANSNA_Follow_Test_Right_executed)
        {
            if(BALL == RIGHT)
            {
                ANSNA_AddInputBelief(Encode_Term("good_ansna"));
                printf("(ball=%d) good\n",BALL);
                score++;
                goods++;
            }
            else
            {
                //ANSNA_AddInput(Encode_Term("good_ansna"), EVENT_TYPE_BELIEF, (Truth) {.frequency = 0, .confidence = 0.9}, "good_ansna");
                printf("(ball=%d) bad\n",BALL);
                score--;
                bads++;
            }
            ANSNA_Follow_Test_Right_executed = false;
        }
        if(ANSNA_Follow_Test_Left_executed)
        {        
            if(BALL == LEFT)
            {
                ANSNA_AddInputBelief(Encode_Term("good_ansna"));
                printf("(ball=%d) good\n",BALL);
                score++;
                goods++;
            }
            else
            {
                //ANSNA_AddInput(Encode_Term("good_ansna"), EVENT_TYPE_BELIEF, (Truth) {.frequency = 0, .confidence = 0.9}, "good_ansna");
                printf("(ball=%d) bad\n",BALL);
                score--;
                bads++;
            }
            ANSNA_Follow_Test_Left_executed = false;
        }
        BALL = rand() % 2;
        printf("Score %i step%d=\n", score,i);
        assert(score > -100, "too bad score");
        assert(bads < 500, "too many wrong trials");
        if(score >= 500)
            break;
        ANSNA_Cycles(10);
    }
    printf("<<ANSNA Follow test successful goods=%d bads=%d\n",goods,bads);
}

bool ANSNA_Pong_Left_executed = false;
void ANSNA_Pong_Left()
{
    ANSNA_Pong_Left_executed = true;
}
bool ANSNA_Pong_Right_executed = false;
void ANSNA_Pong_Right()
{
    ANSNA_Pong_Right_executed = true;
}
bool ANSNA_Pong_Stop_executed = false;
void ANSNA_Pong_Stop()
{
    ANSNA_Pong_Stop_executed = true;
}
void ANSNA_Pong2(bool useNumericEncoding)
{
    OUTPUT = 0;
    ANSNA_INIT();
    puts(">>ANSNA Pong start");
    ANSNA_AddOperation(Encode_Term("op_left"), ANSNA_Pong_Left); 
    ANSNA_AddOperation(Encode_Term("op_right"), ANSNA_Pong_Right); 
    ANSNA_AddOperation(Encode_Term("op_stop"), ANSNA_Pong_Stop); 
    int szX = 50;
    int szY = 20;
    int ballX = szX/2;
    int ballY = szY/5;
    int batX = 20;
    int batVX = 0;
    int batWidth = 6; //"radius", batWidth from middle to the left and right
    int vX = 1;
    int vY = 1;
    int hits = 0;
    int misses = 0;
    int t=0;
    while(1)
    {
        t++;
        //if(t%10000 == 0)
        //    getchar();
        fputs("\033[1;1H\033[2J", stdout); //POSIX clear screen
        for(int i=0; i<batX-batWidth+1; i++)
        {
            fputs(" ", stdout);
        }
        for(int i=0; i<batWidth*2-1+MIN(0,batX) ;i++)
        {
            fputs("@", stdout);
        }
        puts("");
        for(int i=0; i<ballY; i++)
        {
            for(int k=0; k<szX; k++)
            {
                fputs(" ", stdout);
            }
            puts("|");
        }
        for(int i=0; i<ballX; i++)
        {
            fputs(" ", stdout);
        }
        fputs("#", stdout);
        for(int i=ballX+1; i<szX; i++)
        {
            fputs(" ", stdout);
        }
        puts("|");
        for(int i=ballY+1; i<szY; i++)
        {
            for(int k=0; k<szX; k++)
            {
                fputs(" ", stdout);
            }
            puts("|");
        }
        if(useNumericEncoding)
        {
            SDR sdrX = Encode_Scalar(0-batWidth*2, 2*szX+batWidth*2, szX+(ballX-batX));
            //SDR_PrintWhereTrue(&sdrX);
            ANSNA_AddInputBelief(sdrX);
        }
        else
        {
            if(batX <= ballX - batWidth)
            {
                ANSNA_AddInputBelief(Encode_Term("ball_right"));
            }
            else
            if(ballX + batWidth < batX)
            {
                ANSNA_AddInputBelief(Encode_Term("ball_left"));
            }
            else
            {
                ANSNA_AddInputBelief(Encode_Term("ball_equal"));
            }
        }
        ANSNA_AddInputGoal(Encode_Term("good_ansna"));
        if(ballX <= 0)
        {
            vX = 1;
        }
        if(ballX >= szX-1)
        {
            vX = -1;
        }
        if(ballY <= 0)
        {
            vY = 1;
        }
        if(ballY >= szY-1)
        {
            vY = -1;
        }
        if(t%2 == -1)
        {
            ballX += vX;
        }
        ballY += vY;
        if(ballY == 0)
        {
            if(abs(ballX-batX) <= batWidth)
            {
                ANSNA_AddInputBelief(Encode_Term("good_ansna"));
                puts("good");
                hits++;
            }
            else
            {
                //ANSNA_AddInput(Encode_Term("good_ansna"), EVENT_TYPE_BELIEF, (Truth) {.frequency = 0, .confidence = 0.9});
                puts("bad");
                misses++;
            }
        }
        if(ballY == 0 || ballX == 0 || ballX >= szX-1)
        {
            ballY = szY/2+rand()%(szY/2);
            ballX = rand()%szX;
            vX = rand()%2 == 0 ? 1 : -1;
        }
        if(ANSNA_Pong_Left_executed)
        {
            ANSNA_Pong_Left_executed = false;
            puts("Exec: op_left");
            batVX = -3;
        }
        if(ANSNA_Pong_Right_executed)
        {
            ANSNA_Pong_Right_executed = false;
            puts("Exec: op_right");
            batVX = 3;
        }
        if(ANSNA_Pong_Stop_executed)
        {
            ANSNA_Pong_Stop_executed = false;
            puts("Exec: op_stop");
            batVX = 0;
        }
        batX=MAX(-batWidth*2,MIN(szX-1+batWidth,batX+batVX*batWidth/2));
        printf("Hits=%d misses=%d ratio=%f time=%ld\n", hits, misses, (float) (((float) hits) / ((float) misses)), currentTime);
        nanosleep((struct timespec[]){{0, 20000000L}}, NULL); //POSIX sleep
        //ANSNA_Cycles(10);
    }
}
//int t=0;
void ANSNA_Pong(bool useNumericEncoding)
{
    OUTPUT = 0;
    ANSNA_INIT();
    puts(">>ANSNA Pong start");
    ANSNA_AddOperation(Encode_Term("op_left"), ANSNA_Pong_Left); 
    ANSNA_AddOperation(Encode_Term("op_right"), ANSNA_Pong_Right); 
    int szX = 50;
    int szY = 20;
    int ballX = szX/2;
    int ballY = szY/5;
    int batX = 20;
    int batVX = 0;
    int batWidth = 4; //"radius", batWidth from middle to the left and right
    int vX = 1;
    int vY = 1;
    int hits = 0;
    int misses = 0;
    while(1)
    {
        //t++;
        //if(t%10000 == 0)
        //    getchar();
        fputs("\033[1;1H\033[2J", stdout); //POSIX clear screen
        for(int i=0; i<batX-batWidth+1; i++)
        {
            fputs(" ", stdout);
        }
        for(int i=0; i<batWidth*2-1 ;i++)
        {
            fputs("@", stdout);
        }
        puts("");
        for(int i=0; i<ballY; i++)
        {
            for(int k=0; k<szX; k++)
            {
                fputs(" ", stdout);
            }
            puts("|");
        }
        for(int i=0; i<ballX; i++)
        {
            fputs(" ", stdout);
        }
        fputs("#", stdout);
        for(int i=ballX+1; i<szX; i++)
        {
            fputs(" ", stdout);
        }
        puts("|");
        for(int i=ballY+1; i<szY; i++)
        {
            for(int k=0; k<szX; k++)
            {
                fputs(" ", stdout);
            }
            puts("|");
        }
        if(useNumericEncoding)
        {
            SDR sdrX = Encode_Scalar(0, 2*szX, szX+(ballX-batX));
            //SDR_PrintWhereTrue(&sdrX);
            ANSNA_AddInputBelief(sdrX);
        }
        else
        {
            if(batX < ballX)
            {
                ANSNA_AddInputBelief(Encode_Term("ball_right"));
            }
            if(ballX < batX)
            {
                ANSNA_AddInputBelief(Encode_Term("ball_left"));
            }
        }
        ANSNA_AddInputGoal(Encode_Term("good_ansna"));
        if(ballX <= 0)
        {
            vX = 1;
        }
        if(ballX >= szX-1)
        {
            vX = -1;
        }
        if(ballY <= 0)
        {
            vY = 1;
        }
        if(ballY >= szY-1)
        {
            vY = -1;
        }
        ballX += vX;
        ballY += vY;
        if(ballY == 0)
        {
            if(abs(ballX-batX) <= batWidth)
            {
                ANSNA_AddInputBelief(Encode_Term("good_ansna"));
                puts("good");
                hits++;
            }
            else
            {
                //ANSNA_AddInput(Encode_Term("good_ansna"), EVENT_TYPE_BELIEF, (Truth) {.frequency = 0, .confidence = 0.9}, "good_ansna");
                puts("bad");
                misses++;
            }
        }
        if(ballY == 0 || ballX == 0 || ballX >= szX-1)
        {
            ballY = szY/2+rand()%(szY/2);
            ballX = rand()%szX;
            vX = rand()%2 == 0 ? 1 : -1;
        }
        if(ANSNA_Pong_Left_executed)
        {
            ANSNA_Pong_Left_executed = false;
            puts("Exec: op_left");
            batVX = -2;
        }
        if(ANSNA_Pong_Right_executed)
        {
            ANSNA_Pong_Right_executed = false;
            puts("Exec: op_right");
            batVX = 2;
        }
        batX=MAX(0,MIN(szX-1,batX+batVX*batWidth/2));
        printf("Hits=%d misses=%d ratio=%f time=%ld\n", hits, misses, (float) (((float) hits) / ((float) misses)), currentTime);
        nanosleep((struct timespec[]){{0, 20000000L}}, NULL); //POSIX sleep
        //ANSNA_Cycles(10);
    }
}

bool ANSNA_Lightswitch_GotoSwitch_executed = false;
void ANSNA_Lightswitch_GotoSwitch()
{
    ANSNA_Lightswitch_GotoSwitch_executed = true;
    puts("ANSNA invoked goto switch");
}
bool ANSNA_Lightswitch_ActivateSwitch_executed = false;
void ANSNA_Lightswitch_ActivateSwitch()
{
    ANSNA_Lightswitch_ActivateSwitch_executed = true;
    puts("ANSNA invoked activate switch");
}
void ANSNA_Multistep_Test()
{
    MOTOR_BABBLING_CHANCE = 0;
    puts(">>ANSNA Multistep test start");
    OUTPUT = 0;
    ANSNA_INIT();
    ANSNA_AddOperation(Encode_Term("op_goto_switch"), ANSNA_Lightswitch_GotoSwitch); 
    ANSNA_AddOperation(Encode_Term("op_activate_switch"), ANSNA_Lightswitch_ActivateSwitch); 
    for(int i=0; i<5; i++)
    {
        ANSNA_AddInputBelief(Encode_Term("start_at"));
        ANSNA_AddInputBelief(Encode_Term("op_goto_switch"));
        ANSNA_Cycles(1);
        ANSNA_AddInputBelief(Encode_Term("switch_at"));
        ANSNA_AddInputBelief(Encode_Term("op_activate_switch"));
        ANSNA_AddInputBelief(Encode_Term("switch_active"));
        ANSNA_Cycles(1);
        ANSNA_AddInputBelief(Encode_Term("light_active"));
        ANSNA_Cycles(10);
    }
    ANSNA_Cycles(10);
    ANSNA_AddInputBelief(Encode_Term("start_at"));
    ANSNA_AddInputGoal(Encode_Term("light_active"));
    ANSNA_Cycles(10);
    assert(ANSNA_Lightswitch_GotoSwitch_executed && !ANSNA_Lightswitch_ActivateSwitch_executed, "ANSNA needs to go to the switch first");
    ANSNA_Lightswitch_GotoSwitch_executed = false;
    puts("ANSNA arrived at the switch");
    ANSNA_AddInputBelief(Encode_Term("switch_at"));
    ANSNA_AddInputGoal(Encode_Term("light_active"));
    assert(!ANSNA_Lightswitch_GotoSwitch_executed && ANSNA_Lightswitch_ActivateSwitch_executed, "ANSNA needs to activate the switch");
    ANSNA_Lightswitch_ActivateSwitch_executed = false;
    puts("<<ANSNA Multistep test successful");
}
void ANSNA_Multistep2_Test()
{
    MOTOR_BABBLING_CHANCE = 0;
    puts(">>ANSNA Multistep2 test start");
    OUTPUT = 0;
    ANSNA_INIT();
    ANSNA_AddOperation(Encode_Term("op_goto_switch"), ANSNA_Lightswitch_GotoSwitch); 
    ANSNA_AddOperation(Encode_Term("op_activate_switch"), ANSNA_Lightswitch_ActivateSwitch); 
    for(int i=0; i<5; i++)
    {
        ANSNA_AddInputBelief(Encode_Term("start_at"));
        ANSNA_AddInputBelief(Encode_Term("op_goto_switch"));
        ANSNA_Cycles(1);
        ANSNA_AddInputBelief(Encode_Term("switch_at"));
        ANSNA_Cycles(10);
    }
    ANSNA_Cycles(1000);
    for(int i=0; i<5; i++)
    {
        ANSNA_AddInputBelief(Encode_Term("switch_at"));
        ANSNA_AddInputBelief(Encode_Term("op_activate_switch"));
        ANSNA_AddInputBelief(Encode_Term("switch_active"));
        ANSNA_Cycles(1);
        ANSNA_AddInputBelief(Encode_Term("light_active"));
        ANSNA_Cycles(10);
    }
    ANSNA_Cycles(10);
    ANSNA_AddInputBelief(Encode_Term("start_at"));
    ANSNA_AddInputGoal(Encode_Term("light_active"));
    ANSNA_Cycles(10);
    assert(ANSNA_Lightswitch_GotoSwitch_executed && !ANSNA_Lightswitch_ActivateSwitch_executed, "ANSNA needs to go to the switch first (2)");
    ANSNA_Lightswitch_GotoSwitch_executed = false;
    puts("ANSNA arrived at the switch");
    ANSNA_AddInputBelief(Encode_Term("switch_at"));
    ANSNA_AddInputGoal(Encode_Term("light_active"));
    assert(!ANSNA_Lightswitch_GotoSwitch_executed && ANSNA_Lightswitch_ActivateSwitch_executed, "ANSNA needs to activate the switch (2)");
    ANSNA_Lightswitch_ActivateSwitch_executed = false;
    puts("<<ANSNA Multistep2 test successful");
}

static bool goto_s0 = false;
static bool goto_s1 = false;
static bool goto_s2 = false;
static bool goto_s3 = false;
static bool goto_l0 = false;
static bool goto_l1 = false;
static bool activate = false;
static bool deactivate = false;
void ANSNA_TestChamber_goto_s0()
{
    goto_s0 = true;
    puts("ANSNA goto s0");
}
void ANSNA_TestChamber_goto_s1()
{
    goto_s1 = true;
    puts("ANSNA goto s1");
}
void ANSNA_TestChamber_goto_s2()
{
    goto_s2 = true;
    puts("ANSNA goto s2");
}
void ANSNA_TestChamber_goto_s3()
{
    goto_s3 = true;
    puts("ANSNA goto s3");
}
void ANSNA_TestChamber_goto_l0()
{
    goto_l0 = true;
    puts("ANSNA goto l0");
}
void ANSNA_TestChamber_goto_l1()
{
    goto_l1 = true;
    puts("ANSNA goto l1");
}
void ANSNA_TestChamber_activate()
{
    activate = true;
    puts("ANSNA activate");
}
void ANSNA_TestChamber_deactivate()
{
    deactivate = true;
    puts("ANSNA deactivate");
}
void ANSNA_TestChamber()
{
    CONCEPT_FORMATION_NOVELTY = 0.0;
    OUTPUT = 0;
    ANSNA_INIT();
    MOTOR_BABBLING_CHANCE = 0;
    ANSNA_AddOperation(Encode_Term("op_goto_s0"), ANSNA_TestChamber_goto_s0); 
    ANSNA_AddOperation(Encode_Term("op_goto_s1"), ANSNA_TestChamber_goto_s1); 
    ANSNA_AddOperation(Encode_Term("op_goto_s2"), ANSNA_TestChamber_goto_s2); 
    ANSNA_AddOperation(Encode_Term("op_goto_s3"), ANSNA_TestChamber_goto_s3); 
    ANSNA_AddOperation(Encode_Term("op_goto_l0"), ANSNA_TestChamber_goto_l0); 
    ANSNA_AddOperation(Encode_Term("op_goto_l1"), ANSNA_TestChamber_goto_s1); 
    ANSNA_AddOperation(Encode_Term("op_activate"), ANSNA_TestChamber_activate); 
    ANSNA_AddOperation(Encode_Term("op_deactivate"), ANSNA_TestChamber_deactivate); 
    int size = 7;
    char world[7][13] = { "_________    ",
                          "| l0  s2| s1 ",
                          "| (0)-0 |-0  ",
                          "|_______|    ",
                          "          s0 ",
                          " l1  s3   .< ",
                          " (0)-0       " };
    //positions:
    const int pos_s0 = 0;
    const int pos_s1 = 1;
    const int pos_s2 = 2;
    const int pos_s3 = 3;
    const int pos_l0 = 4;
    const int pos_l1 = 5;
    //states:
    int pos = pos_s0; //agent position
    bool s1 = false;
    bool s2 = false;
    bool s3 = false;
    bool l0 = false;
    bool l1 = false;
    bool door = false; //door closed
    puts("at_s0");
    ANSNA_AddInputBelief(Encode_Term("at_s0"));
    while(1)
    {
        //movement
        if(goto_s0)
        {
            pos = pos_s0;
        }
        else
        if(goto_s1)
        {
            pos = pos_s1;
        }
        else
        if(goto_s3)
        {
            pos = pos_s3;
        }
        else
        if(goto_l1)
        {
            pos = pos_l1;
        }
        else
        if(door) //door must be open else it couldn't be in the room or move to a position inside
        {
            if(goto_l0)
            {
                pos = pos_l0;
            }
            else
            if(goto_s2)
            {
                pos = pos_s2;
            }
        }
        //inform ANSNA about current location
        if(pos == pos_s0)
        {
            puts("at_s0.");
            ANSNA_AddInputBelief(Encode_Term("at_s0"));
        }
        if(pos == pos_s1)
        {
            puts("at_s1.");
            ANSNA_AddInputBelief(Encode_Term("at_s1"));
        }
        if(pos == pos_s2)
        {
            puts("at_s2.");
            ANSNA_AddInputBelief(Encode_Term("at_s2"));
        }
        if(pos == pos_s3)
        {
            puts("at_s3.");
            ANSNA_AddInputBelief(Encode_Term("at_s3"));
        }
        if(pos == pos_l0)
        {
            puts("at_l0.");
            ANSNA_AddInputBelief(Encode_Term("at_l0"));
        }
        if(pos == pos_l1)
        {
            puts("at_l1.");
            ANSNA_AddInputBelief(Encode_Term("at_l1"));
        }
        //manipulation
        if(pos == pos_s1 && deactivate)
        {
            s1 = false;
            puts("s1_is_0.");
            ANSNA_AddInputBelief(Encode_Term("s1_is_0"));
            //s1 also closes the door:
            door = false;
            puts("door_is_closed.");
            ANSNA_AddInputBelief(Encode_Term("door_is_closed"));
        }
        else
        if(pos == pos_s2 && deactivate)
        {
            s2 = false;
            puts("s2_is_0.");
            ANSNA_AddInputBelief(Encode_Term("s2_is_0"));
            //s2 also deactivates l0:
            l0 = false;
            puts("l0_is_0.");
            ANSNA_AddInputBelief(Encode_Term("l0_is_0"));
        }
        else
        if(pos == pos_s3 && deactivate)
        {
            s3 = false;
            puts("s3_is_0.");
            ANSNA_AddInputBelief(Encode_Term("s3_is_0"));
            //s3 also deactivates l1
            l1 = false;
            puts("l1_is_0.");
            ANSNA_AddInputBelief(Encode_Term("l1_is_0"));
        }
        else
        if(pos == pos_s1 && activate)
        {
            s1 = true;
            puts("s1_is_1.");
            ANSNA_AddInputBelief(Encode_Term("s1_is_1"));
            //s1 also opens the door:
            door = true;
            puts("door_is_open.");
            ANSNA_AddInputBelief(Encode_Term("door_is_open"));
        }
        else
        if(pos == pos_s2 && activate)
        {
            s2 = true;
            puts("s2_is_1.");
            ANSNA_AddInputBelief(Encode_Term("s2_is_1"));
            //s2 also activates l0:
            l0 = true;
            puts("l0_is_1.");
            ANSNA_AddInputBelief(Encode_Term("l0_is_1"));
        }
        else
        if(pos == pos_s3 && activate)
        {
            s3 = true;
            puts("s3_is_1.");
            ANSNA_AddInputBelief(Encode_Term("s3_is_1"));
            //s3 also activates l1
            l1 = true;
            puts("l1_is_1.");
            ANSNA_AddInputBelief(Encode_Term("l1_is_1"));
        }
        //change char array to draw:
        world[6][6] = world[6][0] = world[5][11] = world[2][11] = world[2][7] = world[2][1] = ' ';
        if(pos == pos_s3)
        {
             world[6][6] = '<';
        }
        if(pos == pos_l1)
        {
             world[6][0] = '<';
        }
        if(pos == pos_s0)
        {
             world[5][11] = '<';
        }
        if(pos == pos_s1)
        {
             world[2][11] = '<';
        }
        if(pos == pos_s2)
        {
             world[2][7] = '<';
        }
        if(pos == pos_l0)
        {
             world[2][1] = '<';
        }
        if(l0)
        {
            world[2][3] = '1';
        }
        else
        {
            world[2][3] = '0';
        }
        if(s2)
        {
            world[2][6] = '1';
        }
        else
        {
            world[2][6] = '0';
        }
        if(s1)
        {
            world[2][10] = '1';
        }
        else
        {
            world[2][10] = '0';
        }
        if(l1)
        {
            world[6][2] = '1';
        }
        else
        {
            world[6][2] = '0';
        }
        if(s3)
        {
            world[6][5] = '1';
        }
        else
        {
            world[6][5] = '0';
        }
        if(door)
        {
            world[2][8] = ' ';
        }
        else
        {
            world[2][8] = '|';
        }
        activate = deactivate = goto_l0 = goto_l1 = goto_s0 = goto_s1 = goto_s2 = goto_s3 = false;
        //fputs("\033[1;1H\033[2J", stdout); //POSIX clear screen
        puts("\n---------------\nNew iteration\nCommands:");
        puts("a ... goto s0");
        puts("b ... goto s1");
        puts("c ... goto s2");
        puts("d ... goto s3");
        puts("e ... goto l0");
        puts("f ... goto l1");
        puts("g ... activate");
        puts("h ... deactivate");
        puts("i ... door should be open!");
        puts("j ... door should be closed!");
        puts("k ... s1 should be 1!");
        puts("l ... s1 should be 0!");
        puts("m ... s2 should be 1!");
        puts("n ... s2 should be 0!");
        puts("o ... s3 should be 1!");
        puts("p ... s3 should be 0!");
        puts("q ... l0 should be 1!");
        puts("r ... l0 should be 0!");
        puts("s ... l1 should be 1!");
        puts("t ... l1 should be 0!");
        puts("u ... you should be at s0!");
        puts("v ... you should be at s1!");
        puts("w ... you should be at s2!");
        puts("x ... you should be at s3!");
        puts("y ... you should be at l0!");
        puts("z ... you should be at l1!");
        puts("other ... Next timestep\n");
        //nanosleep((struct timespec[]){{0, 20000000L}}, NULL); //POSIX sleep
        for(int i=0; i<size; i++)
        {
            for(int j=0; j<13; j++)
            {
                putchar(world[i][j]);
            }
            puts("");
            //puts(&world[i]);
        }
        puts("\nCommand:");
        char c = getchar();
        if(c == 'a')
        {
            goto_s0 = true;
            puts("op_goto_s0.");
            ANSNA_AddInputBelief(Encode_Term("op_goto_s0"));
        }
        if(c == 'b')
        {
            goto_s1 = true;
            puts("op_goto_s1.");
            ANSNA_AddInputBelief(Encode_Term("op_goto_s1"));
        }
        if(c == 'c')
        {
            goto_s2 = true;
            puts("op_goto_s2.");
            ANSNA_AddInputBelief(Encode_Term("op_goto_s2"));
        }
        if(c == 'd')
        {
            goto_s3 = true;
            puts("op_goto_s3.");
            ANSNA_AddInputBelief(Encode_Term("op_goto_s3"));
        }
        if(c == 'e')
        {
            goto_l0 = true;
            puts("op_goto_l0.");
            ANSNA_AddInputBelief(Encode_Term("op_goto_l0"));
        }
        if(c == 'f')
        {
            goto_l1 = true;
            puts("op_goto_l1.");
            ANSNA_AddInputBelief(Encode_Term("op_goto_l1"));
        }
        if(c == 'g')
        {
            activate = true;
            puts("op_activate.");
            ANSNA_AddInputBelief(Encode_Term("op_activate"));
        }
        if(c == 'h')
        {
            deactivate = true;
            puts("op_deactivate.");
            ANSNA_AddInputBelief(Encode_Term("op_deactivate"));
        }
        if(c == 'i')
        {
            puts("door_is_open!");
            ANSNA_AddInputGoal(Encode_Term("door_is_open"));
            //door should be open
        }
        if(c == 'j')
        {
            puts("door_is_closed!");
            ANSNA_AddInputGoal(Encode_Term("door_is_closed"));
            //door should be closed
        }
        if(c == 'k')
        {
            puts("s1_is_1!");
            ANSNA_AddInputGoal(Encode_Term("s1_is_1"));
            //s1 should be 1
        }
        if(c == 'l')
        {
            puts("s1_is_0!");
            ANSNA_AddInputGoal(Encode_Term("s1_is_0"));
            //s1 should be 0
        }
        if(c == 'm')
        {
            puts("s2_is_1!");
            ANSNA_AddInputGoal(Encode_Term("s2_is_1"));
            //s2 should be 1
        }
        if(c == 'n')
        {
            puts("s2_is_0!");
            ANSNA_AddInputGoal(Encode_Term("s2_is_0"));
            //s2 should be 0
        }
        if(c == 'o')
        {
            puts("s3_is_1!");
            ANSNA_AddInputGoal(Encode_Term("s3_is_1"));
            //s3 should be 1
        }
        if(c == 'p')
        {
            puts("s3_is_0!");
            ANSNA_AddInputGoal(Encode_Term("s3_is_0"));
            //s3 should be 0
        }
        if(c == 'q')
        {
            puts("l0_is_1!");
            ANSNA_AddInputGoal(Encode_Term("l0_is_1"));
            //l0 should be 1
        }
        if(c == 'r')
        {
            puts("l0_is_1!");
            ANSNA_AddInputGoal(Encode_Term("l0_is_0"));
            //l0 should be 0
        }
        if(c == 's')
        {
            puts("l1_is_1!");
            ANSNA_AddInputGoal(Encode_Term("l1_is_1"));
            //l1 should be 1
        }
        if(c == 't')
        {
            puts("l1_is_0!");
            ANSNA_AddInputGoal(Encode_Term("l1_is_0"));
            //l1 should be 0
        }
        if(c == 'u')
        {
            puts("at_s0!");
            ANSNA_AddInputGoal(Encode_Term("at_s0"));
            //you should be at s0!
        }
        if(c == 'v')
        {
            puts("at_s1!");
            ANSNA_AddInputGoal(Encode_Term("at_s1"));
            //you should be at s1!
        }
        if(c == 'w')
        {
            puts("at_s2!");
            ANSNA_AddInputGoal(Encode_Term("at_s2"));
            //you should be at s2!
        }
        if(c == 'x')
        {
            puts("at_s3!");
            ANSNA_AddInputGoal(Encode_Term("at_s3"));
            //you should be at s3!
        }
        if(c == 'y')
        {
            puts("at_l0!");
            ANSNA_AddInputGoal(Encode_Term("at_l0"));
            //you should be at l0!
        }
        if(c == 'z')
        {
            puts("at_l1!");
            ANSNA_AddInputGoal(Encode_Term("at_l1"));
            //you should be at l1!
        }
    }
}

bool op_1_executed = false;
void op_1()
{
    op_1_executed = true;
}
bool op_2_executed = false;
void op_2()
{
    op_2_executed = true;
}
bool op_3_executed = false;
void op_3()
{
    op_3_executed = true;
}
void Sequence_Test()
{
    OUTPUT=0;
    ANSNA_INIT();
    CONCEPT_FORMATION_NOVELTY = 0;
    MOTOR_BABBLING_CHANCE = 0;
    puts(">>Sequence test start");
    ANSNA_AddOperation(Encode_Term("op_1"), op_1); 
    ANSNA_AddOperation(Encode_Term("op_2"), op_2); 
    ANSNA_AddOperation(Encode_Term("op_3"), op_3); 
    for(int i=0;i<100;i++)
    {
        ANSNA_AddInputBelief(Encode_Term("a")); //0 2 4 5
        ANSNA_AddInputBelief(Encode_Term("b"));
        ANSNA_AddInputBelief(Encode_Term("op_1"));
        ANSNA_AddInputBelief(Encode_Term("g"));
        ANSNA_Cycles(100);
    }
    for(int i=0;i<100;i++)
    {
        ANSNA_AddInputBelief(Encode_Term("a"));
        ANSNA_AddInputBelief(Encode_Term("op_1"));
        ANSNA_Cycles(100);
    }
    for(int i=0;i<100;i++)
    {
        ANSNA_AddInputBelief(Encode_Term("b"));
        ANSNA_AddInputBelief(Encode_Term("op_1"));
        ANSNA_Cycles(100);
    }
    /*for(int i=0;i<10;i++)
    {
        ANSNA_AddInputBelief(Encode_Term("b"));
        ANSNA_AddInputBelief(Encode_Term("op_2"));
        ANSNA_AddInputBelief(Encode_Term("g"));
        ANSNA_Cycles(100);
    }
    for(int i=0;i<10;i++)
    {
        ANSNA_AddInputBelief(Encode_Term("a"));
        ANSNA_AddInputBelief(Encode_Term("op_3"));
        ANSNA_AddInputBelief(Encode_Term("g"));
        ANSNA_Cycles(100);
    }*/
    ANSNA_AddInputBelief(Encode_Term("a"));
    ANSNA_AddInputBelief(Encode_Term("b"));
    ANSNA_AddInputGoal(Encode_Term("g"));
    assert(op_1_executed && !op_2_executed && !op_3_executed, "Expected op1 execution");
    op_1_executed = op_2_executed = op_3_executed = false;
    exit(0);
    ANSNA_Cycles(100);
    ANSNA_AddInputBelief(Encode_Term("b"));
    ANSNA_AddInputGoal(Encode_Term("g"));
    assert(!op_1_executed && op_2_executed && !op_3_executed, "Expected op2 execution");
    op_1_executed = op_2_executed = op_3_executed = false;
    ANSNA_Cycles(100);
    ANSNA_AddInputBelief(Encode_Term("a"));
    ANSNA_AddInputGoal(Encode_Term("g"));
    assert(!op_1_executed && !op_2_executed && op_3_executed, "Expected op3 execution");
    op_1_executed = op_2_executed = op_3_executed = false;
    MOTOR_BABBLING_CHANCE = MOTOR_BABBLING_CHANCE_INITIAL;
    CONCEPT_FORMATION_NOVELTY = CONCEPT_FORMATION_NOVELTY_INITIAL;
    puts(">>Sequence Test successul");
}


int main(int argc, char *argv[])
{
    
    //Sequence_Test();
    //if(true)
    //    return 0;
    //printf("sizeof concept %d\n",(int) sizeof(Concept));
    //exit(0);
    if(argc == 2) //pong
    {
        if(!strcmp(argv[1],"pong"))
        {
            ANSNA_Pong(false);
        }
        if(!strcmp(argv[1],"pong2"))
        {
            ANSNA_Pong2(false);
        }
        if(!strcmp(argv[1],"numeric-pong"))
        {
            ANSNA_Pong(true);
        }
        if(!strcmp(argv[1],"numeric-pong2"))
        {
            ANSNA_Pong2(true);
        }
        if(!strcmp(argv[1],"testchamber"))
        {
            ANSNA_TestChamber();
        }
    }
    srand(1337);
    ANSNA_INIT();
    SDR_Test();
    Stamp_Test();
    FIFO_Test();
    PriorityQueue_Test();
    Table_Test();
    ANSNA_Alphabet_Test();
    ANSNA_Procedure_Test();
    Memory_Test();
    ANSNA_Follow_Test();
    ANSNA_Multistep_Test();
    ANSNA_Multistep2_Test();
    puts("\nAll tests ran successfully, if you wish to run examples now, just pass the corresponding parameter:");
    puts("ANSNA pong (starts Pong example)");
    puts("ANSNA numeric-pong (starts Pong example with numeric input)");
    puts("ANSNA pong2 (starts Pong2 example)");
    puts("ANSNA numeric-pong2 (starts Pong2 example with numeric input)");
    puts("ANSNA testchamber (starts Test Chamber multistep procedure learning example)");
    return 0;
}

