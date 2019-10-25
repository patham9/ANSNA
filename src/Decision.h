#ifndef DECISION_H
#define DECISION_H

//////////////////////
//  ANSNA Decision  //
//////////////////////
//Realization of goals

//References//
//----------//
#include <stdbool.h>
#include <stdio.h>
#include "Memory.h"
#include "ANSNA.h"

//Parameters//
//----------//
//truth expectation needed for executions
#define DECISION_THRESHOLD 0.501
//motor babbling chance
#define MOTOR_BABBLING_CHANCE_INITIAL 0.2
extern double MOTOR_BABBLING_CHANCE;

//Data structure//
//--------------//
typedef struct
{
    double desire;
    bool execute;
    int operationID;
    Operation op;
}Decision;

//Methods//
//-------//
//Find the best operation to execute for the goal to realize
Decision Decision_Suggest(Event *goal, long currentTime);
//Execute the decision, invoking the procedure and returning feedback
void Decision_Execute(Decision *decision);
#endif
