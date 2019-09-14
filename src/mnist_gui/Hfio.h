////////////Hamlib Module//////////////////////
//> Hfio - HamFileInputOutput
//> A HamEngine file for file input/output
//> created for simple use
//+ Creator "Patrick Hammer"
//+ License "GPL" Language "C"
//+ Changedate "06.09.2008" State "90%"
///////////////////////////////////////////////
#ifndef Hfio
#define Hfio
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
#include <string.h>
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
struct Hfio
{
	int mode; //will be set to MODE on init 
	int init; //0
}
hfio;
///////////////////////////////////////////////

//:::::::::::::::MODULE-IO::::::::::::::::::://
void hfio_INIT();
void hfio_UPDATE();
void hfio_CHECK();
//::
char *hfio_textFileRead(const char *fn);
int hfio_textFileWrite(char *fn, char *s);
void hfio_LoadTex(const char *datei,GLuint *ziel);
//::::::::::::::::::::::::::::::::::::::::::://
#endif
/*................COMMENTS...................//
>>todo:
png load/saving
>>done:
text-IO, tga-loading
>>others:
//..........................................*/
