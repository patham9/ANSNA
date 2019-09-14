////////////Hamlib Module//////////////////////
//> Hnav - HamSimulation basic template module
//for 2D simulations
//+ Creator "Patrick Hammer"
//+ License "GPL" Language "C80"
//+ Changedate "23.12.2008" State "50%"
///////////////////////////////////////////////
#ifndef Hnav_H
#define Hnav_H

#include "Hinput.h"
#include "Hrend.h"

struct Hnav
{
	int mode; //will be set to MODE on init
	float difx; //0
	float dify; //0
	float zoom; //1
}
hnav;

void hnav_INIT();
void hnav_UPDATE();
float hnav_MouseToWorldCoordX(int x);
float hnav_MouseToWorldCoordY(int y);
void hnav_SetRendFunc(void (*RenderFunc)(void));
void hnav_Transform();
#endif
