////////////Hamlib Module//////////////////////
//> Hinput - HamModule
//> A template file for a new hammodule
//+ Creator "Patrick Hammer"
//+ License "GPL" Language "C"
//+ Changedate "06.09.2008" State "10%"
///////////////////////////////////////////////
#ifndef Hinput
#define Hinput
///////////////////////////////////////////////

//---------------DEFINES---------------------//
#pragma region DEFINES
#define MODE 0 //0=speed 1=safeness/quality
#define EXT  1 //not only std
//::
#define MaxEventHandlers 32

#define KEY_UP 283
#define KEY_DOWN 284
#define KEY_LEFT 285
#define KEY_RIGHT 286
#define KEY_MINUS 314
#define KEY_PLUS 315

#pragma endregion
//-------------------------------------------//

//+++++++++++++++DEPENCIES+++++++++++++++++++//
#pragma region DEPENCIES
//STD:
#include <stdio.h>
#include <stdlib.h>
//OTHER:
#if EXT==1
#ifndef GLFW_DLL
#define GLFW_DLL
#define GLFW_BUILD_DLL
#include <GL/glfw.h>
#endif
#ifndef GLuint
typedef unsigned int GLuint;
#endif
#endif
#pragma endregion
//+++++++++++++++++++++++++++++++++++++++++++//

/////////////////MODULE-DATA///////////////////
#ifndef DefEventArgs
typedef struct
{
	int mx;
	int my;
	int mk; //mousekey
}EventArgs;
#define DefEventArgs
#endif
typedef void* (*EventHandler)(EventArgs*);

struct Hinput
{
	int mode; //will be set to MODE on init 
	EventHandler MouseDowns[MaxEventHandlers];
	int nMouseDowns; // 1   0 reserved for GUI
	EventHandler MouseUps[MaxEventHandlers];
	int nMouseUps; // 0
	EventHandler MouseDraggeds[MaxEventHandlers];
	int nMouseDraggeds; // 1
	EventHandler MouseMoveds[MaxEventHandlers];
	int nMouseMoveds; // 0
	EventHandler MouseScrolls[MaxEventHandlers];
	int nMouseScrolls; // 0
	EventHandler KeyDowns[MaxEventHandlers];
	int nKeyDowns; // 0
	EventHandler KeyUps[MaxEventHandlers];
	int nKeyUps; // 0
	int init; //0
}
hinput;
///////////////////////////////////////////////

//:::::::::::::::MODULE-IO::::::::::::::::::://
void hinput_INIT();
void hinput_UPDATE();
void hinput_CHECK();
//::
void hinput_Specific();
void hinput_AddMouseDown(EventHandler Routine); //0
void hinput_AddMouseUp(EventHandler Routine);	//1
void hinput_AddMouseMoved(EventHandler Routine); //2
void hinput_AddMouseDragged(EventHandler Routine); //3
void hinput_AddMouseScroll(EventHandler Routine); //4
void hinput_AddKeyDown(EventHandler Routine);
void hinput_AddKeyUp(EventHandler eh);
void hinput_SetGUIMouseDragged(EventHandler eh);
void hinput_SetGUIMouseDown(EventHandler eh);

//::
int hinput_GetLastMousePosX(); //polling (JAVA C#....)
int hinput_GetLastMousePosY(); 
int hinput_GetLastMouseEvent();
//::::::::::::::::::::::::::::::::::::::::::://
#endif
/*................COMMENTS...................//
>>todo:

>>done:

>>others:

//..........................................*/
