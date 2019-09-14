#define NULL 0
#define pGPU 1 //programmable GPU?
#include "HamBuildControl.h"
//////GLOBAL VARS//////////

///////////////////////////
#if HAMOS!=1
#include <GL/glew.h>

///STD///
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/////////
//OTHER//
//#include <windows.h> //++
//#include <GL/glew.h>
//#include <png.h>
/////////
//**#include <gtk/gtk.h>
#include "Hutil.h"

//**#include "HtcpSv.h"
//**#include "HtcpCl.h"

//**#include "Hvb.h"
//**#include "Hhashmapping.h"
#include "Hfio.h"
//**#include "Hshade.h"
#include "Hrend.h"

#include "Hinput.h"
#include "Hgui.h"
//**#include "Hsound.h"
//**#include "Hcom.h"

//***#include "Hnn.h"
//***#include "Hql.h"
//***#include "Hev.h"
//***#include "Hsom.h"
//***#include "Hadap.h"

//***#include "Hsim.h"
#endif
//libs:
//glfwdll.lib glu32.lib OpenGL32.Lib glew32.lib glew32s.lib OpenAL32.lib EFX-Util.lib alut.lib
//päprozessor:
//_CRT_SECURE_NO_WARNINGS

///////////////////////////

//++ Übergangslösung
//-- vorrübergehend auskommentiert
//## unvollständig*/
//** Linux Port Änderungen
