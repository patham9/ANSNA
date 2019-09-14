////////////Hamlib Module//////////////////////
//> Hcam - HamModule
//> A template file for a new hammodule
//+ Creator "Patrick Hammer"
//+ License "GPL" Language "C"
//+ Changedate "06.09.2008" State "90%"
///////////////////////////////////////////////
#ifndef Hcam
#define Hcam
///////////////////////////////////////////////

//---------------DEFINES---------------------//
#pragma region DEFINES
#define MODE 0 //0=speed 1=safeness/quality
#define EXT  1 //not only std
#pragma endregion
//-------------------------------------------//

//+++++++++++++++DEPENCIES+++++++++++++++++++//
#pragma region DEPENCIES
//STD:
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//OTHER:
#if EXT==1
///--GLFW--///
#ifndef GLFW_DLL
#define GLFW_DLL
#define GLFW_BUILD_DLL
#include <GL/glfw.h>
#endif
#ifndef GLuint
typedef unsigned int GLuint;
#endif
//----------//
#endif
//SAME:
#include "Hutil.h"
#include "Hinput.h"
#pragma endregion
//+++++++++++++++++++++++++++++++++++++++++++//

/////////////////MODULE-DATA///////////////////
struct Hcam
{
	int mode; //will be set to MODE on init 
	int init; //0
	int mouse_x;
	int mouse_y;

	//const char* id;
	float xpos;
	float ypos;
	float zpos;
	float xrot;
	float yrot;
	float angle;
	float camzahigkeit;
}
hcam;
///////////////////////////////////////////////

//:::::::::::::::MODULE-IO::::::::::::::::::://
void hcam_INIT();
void hcam_UPDATE();
//void hcam_CHECK();
void hcam_SaveCamPos();
void hcam_SetCamPos(float x,float y,float z,float xrot,float yrot);
void hcam_LoadCamPos();
//::::::::::::::::::::::::::::::::::::::::::://
#endif
/*................COMMENTS...................//
>>todo:

>>done:

>>others:

//..........................................*/
