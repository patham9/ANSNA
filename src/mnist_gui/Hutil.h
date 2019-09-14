////////////Hamlib Module//////////////////////
//> Hutil - Hutil
//> A base of most modules
//+ Creator "Patrick Hammer"
//+ License "GPL" Language "C"
//+ Changedate "06.09.2008" State "10%"
///////////////////////////////////////////////

#ifndef Hutil_H
#define Hutil_H

//---------------DEFINES---------------------//
#pragma region DEFINES
typedef int Thread;
#define ThreadEntry __stdcall
//**typedef void (ThreadEntry * ThreadFunc)(void *);
#pragma endregion
//-------------------------------------------//

//+++++++++++++++DEPENCIES+++++++++++++++++++//
#include "HamBuildControl.h"
//STD:
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
//OTHER:
#if HAMOS==0
#ifndef GLFW_DLL
#define GLFW_DLL
#define GLFW_BUILD_DLL
#include <GL/glfw.h>
#endif
#endif
//+++++++++++++++++++++++++++++++++++++++++++//

/////////////////MODULE-DATA///////////////////
struct Hutil
{
	int mode; //will be set to MODE on init 
	int init; //0
}
hutil;

struct float3
{
	float x;
	float y;
	float z;
};
struct float4
{
	float x;
	float y;
	float z;
	float w;
};
typedef struct 
{
	float *data;
	int *coords;
	int nDimensions;
	int *szDimensions;
}array;
///////////////////////////////////////////////

//:::::::::::::::MODULE-IO::::::::::::::::::://
#define foreach(x,y) for(x;x<sizeof(y)/sizeof(int);x++)

char * ftoa(float f);
char * dtoa(int d);
char * ctoa(char d);
#define PI 3.1415
#define PI_2 PI/2
#define PI_4 PI/4
#define min(a,b) ((a)<(b))?(a):(b)
#define max(a,b) ((a)>(b))?(a):(b)
float deg(float value);
void *ToVoid(float value);
float ToFloat(void *value);
double drnd();
float frnd();
float srnd();
float RadAngleRange(float ret);

//**Thread Thread_NEW(ThreadFunc fun,void *arg);
//**void Thread_DEL(Thread thr);
//**#define §(x,y) Thread_NEW(x,y)
//**//#define $(x) Thread_DEL(x)
float ** malloc2Df(int nRows,int nCols);
void free2Df(float **array2D,int nRows);
void memcpy2Df(float **To,float **From,int nRows,int nCols);
double CurTime();
void Wait(double time);
//**void strprs(char *str,char *delim,void (*parser)(char*));
//ARRAY:
//**int array_Size(array *arr);
//**array *array_NEW(int nDimensions,int *szDimensions);
//**array *array_NEW2(int nDimensions,...);
//**void array_SetVal(array *arr,float value,int *coords);
//**float array_GetVal(array *arr,int *coords);
//**void array_SetVal2(array *arr,float value, ...);
//**float array_GetVal2(array *arr, ...);

//::::::::::::::::::::::::::::::::::::::::::://
#endif
/*................COMMENTS...................//
>>todo:

>>done:

>>others:

//..........................................*/
