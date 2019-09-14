////////////Hamlib Module//////////////////////
//> Hgui - HamModule
//> openGL GUI and GTK management
//+ Creator "Patrick Hammer"
//+ License "GPL" Language "C"
//+ Changedate "06.09.2008" State "90%"
///////////////////////////////////////////////
#ifndef Hgui
#define Hgui
///////////////////////////////////////////////

//---------------DEFINES---------------------//
#pragma region DEFINES
#define MODE 0 //0=speed 1=safeness/quality
#define EXT  1 //not only std
#pragma endregion
//-------------------------------------------//

//+++++++++++++++DEPENCIES+++++++++++++++++++//
#include "Hutil.h"
#pragma region DEPENCIES
//STD:
#include <stdio.h>
#include <stdlib.h>
//OTHER:
//**#include <gtk/gtk.h>
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
#ifndef Hinput
#include "Hinput.h"
#endif
#ifndef Hrend
#include "Hrend.h"
#endif
#ifndef Hfio
#include "Hfio.h"
#endif
#pragma endregion
//+++++++++++++++++++++++++++++++++++++++++++//

/////////////////MODULE-DATA///////////////////
typedef void (*Hgui_Event)(int);
struct Hgui
{
	int mode; //will be set to MODE on init 
	int init; //0
	void (*CallbackUpdate)();
}
hgui;
///////////////////////////////////////////////

//:::::::::::::::MODULE-IO::::::::::::::::::://
void hgui_INIT();
void hgui_UPDATE();
void hgui_CHECK();
void hgui_RENDER();
//::
unsigned int hgui_AddElem(float px,float py,float width,float height,char * text,float textsize,float textdist,float textoffsetX,Hgui_Event OnClick,unsigned char visible,int INFO);
//TODO: SIMPEL MACHEN
void hgui_SetActiveGUI(int gui);
int hgui_GetLastClickedElem(); //Polling

//**void hgui_ShowWindow(GtkWidget *Window);
void hgui_ActivateElem(unsigned int button);
void hgui_DeactivateElem(unsigned int button);
void hgui_SetElemText(unsigned int button,char *text);
void hgui_SetElemInfo(unsigned int button,int INFO);
void hgui_SetElemOnDrag(unsigned int button,Hgui_Event ev);
void hgui_SetElemColor(unsigned int button,float r,float g,float b,float a);
char *hgui_GetElemText(unsigned int button);
struct float4 hgui_GetElemColor(unsigned int button);
int hgui_GetElemInfo(unsigned int button);
unsigned int hgui_AddSimpleElem(float px,float py,float width,float height,char *text,Hgui_Event OnClick,int INFO);
void hgui_setCallBack(void (*CallBackGUIFunc)());
//::::::::::::::::::::::::::::::::::::::::::://
#endif
/*................COMMENTS...................//
>>todo:

>>done:

>>others:

//..........................................*/
