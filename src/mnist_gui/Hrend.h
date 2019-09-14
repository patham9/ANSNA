////////////Hamlib Module//////////////////////
//> Hrend - HamRenderer
//> The renderer of the HFramework
//+ Creator "Patrick Hammer"
//+ License "GPL" Language "C"
//+ Changedate "15.11.2008" State "10%"
///////////////////////////////////////////////
#ifndef Hrend
#define Hrend
///////////////////////////////////////////////

//---------------DEFINES---------------------//
#pragma region DEFINES
#define MODE 0 //0=speed 1=safeness/quality
#define EXT  1 //not only std

#define MaxRenderRoutines 32
#define GLFW
#pragma endregion
//-------------------------------------------//

//+++++++++++++++DEPENCIES+++++++++++++++++++//
#include "HamBuildControl.h"
#if HAMOS==0
//STD:
#include <stdio.h>
#include <stdlib.h>
//OTHER:
//#if EXT==1 //renderer uses always EXT
//####include <windows.h> //++
#ifndef GLFW_DLL
#include <GL/glew.h>
#define GLFW_DLL
#define GLFW_BUILD_DLL
#include <GL/glfw.h>
#endif
#ifndef GLuint
typedef unsigned int GLuint;
#endif
//#endif
//SAME:
#if HAMOS==0
//#include "Hshade.h"
#include "Hcam.h"
#include "Hnav.h"
#include "Hgui.h"
#endif
#endif
#define NULL 0
#ifndef GLuint
typedef unsigned int GLuint;
#define __security_check_cookie 0
#endif
#define icon GLuint
//+++++++++++++++++++++++++++++++++++++++++++//


/////////////////MODULE-DATA///////////////////
typedef void (*RenderRoutine)();
struct Hrend
{
	int mode; //will be set to MODE on init 
	int nRenderRoutines; // 1, 0 is reserved for GUI
	RenderRoutine RenderRoutines[MaxRenderRoutines];
	int width; //1024
	int height; //768
	float pitch;
	float roll;
	float px;
	float py;	//camera
	float pz;
	int init; //0
	int rendermode;
	float clrR;
	float clrG;
	float clrB;
	float sunpos[4];
}
hrend;
///////////////////////////////////////////////

//:::::::::::::::MODULE-IO::::::::::::::::::://
void hrend_INIT(int width, int height,int fullscreen);
void hrend_UPDATE();
void hrend_CHECK();
//::
void hrend_DrawPixelValue(float val,float px,float py,float size,float distance);
void hrend_DrawSegmentValue(float val,float px,float py,float size,float distance);
void hrend_TakeImage(char *image,void (*RenderFunc)(void*),void *data,int resolutionX,int resolutionY,float posx,float posy,float posz,float rotx,float roty);
void hrend_drawpolygon(float px0,float py0,float pz0,float px1,float py1,float pz1,float px2,float py2,float pz2);
void hrend_AddRenderRoutine(RenderRoutine rend);
void hrend_SetGUIRenderRoutine(RenderRoutine rend); //for high priority rendering like GUI
void hrend_DrawIcon(float px, float py,float ang, float size, float lenght,GLuint icon);
void hrend_DrawObj(float px, float py,float ang, float size, float lenght,GLuint icon);
unsigned char hrend_DrawPixelString(char *ch, float px, float py, float size, float distance);
unsigned char hrend_DrawSegmentString(char *ch, float px, float py, float size, float distance);
void hrend_DrawLine(float x1,float y1,float z1,float x2,float y2,float z2);
void hrend_SelectColor(float r,float g,float b,float a);
void hrend_SetCamera(float px, float py, float pz, float roll, float pitch);
void hrend_2Dmode(float r,float g,float b);
void hrend_3Dmode(float r,float g,float b);
void hrend_Begin(); //Java C# etc instead of AddRenderRoutine with FuncPointer
void hrend_End();
void hrend_ResetMatrix();
void hrend_Translate(float x,float y,float z);
void hrend_Rotate(float x,float y,float z);
void hrend_Scale(float x,float y,float z);
void hrend_EnableTex();
void hrend_DisableTex();
void hrend_EnableSmooth();
void hrend_DisableSmooth();
void hrend_Shadow(void(*RenderFunc)(void*),void* data,float colR,float colG,float colB,float colA);
void hrend_LightShadow(float ground[4],float light[4],void(*RenderFunc)(void*),void* data,float colR,float colG,float colB,float colA);
void hrend_TakeTextureFromImage(GLuint *tex,char *image,int resolutionX,int resolutionY);
void hrend_InitTextureFromImage(GLuint *tex,char *image,int resolutionX,int resolutionY);

void hrend_CMD_DrawConsole(char **inarr,int size);
void hrend_CMD_InsertChar(char **inarr,int px,int py,int size,char Char);
char **hrend_CMD_InitCharArray(int size);
void hrend_CMD_ClrCharArray(char **inarr,int size);
//::::::::::::::::::::::::::::::::::::::::::://
#endif
/*................COMMENTS...................//
>>todo:
new 3d part
>>done:

>>others:

//..........................................*/
