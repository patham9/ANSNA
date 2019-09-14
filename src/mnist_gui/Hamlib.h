#ifndef HAMLIB_H
#define HAMLIB_H

#define PI 3.14159265f
#include "HamBuildControl.h"	 

#include "Hinclude.h"
#ifndef NULL
#define NULL 0
#endif
//#pragma comment(lib,"Hamlib.lib")

#pragma comment(lib,"Ws2_32.lib")

#pragma comment(lib,"glfwdll.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"OpenGL32.Lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glew32s.lib")
#pragma comment(lib,"OpenAL32.lib")
#pragma comment(lib,"EFX-Util.lib")
#pragma comment(lib,"alut.lib")
#pragma comment(lib,"cv.lib")
#pragma comment(lib,"cvcam.lib")
#pragma comment(lib,"cvaux.lib")
#pragma comment(lib,"cxcore.lib")
#pragma comment(lib,"cxts.lib")
#pragma comment(lib,"highgui.lib")

#pragma comment(lib,"gtk-win32-2.0.lib")
#pragma comment(lib,"glib-2.0.lib")
#pragma comment(lib,"gobject-2.0.lib")
#pragma comment(lib,"gmodule-2.0.lib")
#pragma comment(lib,"gladeui-1.lib")
#pragma comment(lib,"libindent.lib")
#pragma comment(lib,"libgladegtk.dll.a")

//Hamlib.lib glfwdll.lib glu32.lib OpenGL32.Lib glew32.lib glew32s.lib OpenAL32.lib EFX-Util.lib alut.lib cv.lib cvcam.lib cvaux.lib cxcore.lib cxts.lib highgui.lib cudart.lib cutil32D.lib cuda.lib cudart.lib
//gtk-win32-2.0.lib glib-2.0.lib gobject-2.0.lib gmodule-2.0.lib gladeui-1.lib libindent.lib libgladegtk.dll.a
#endif
int Hamlib_CHECK(){ return 0; } //systemcheck
//not needed at the moment cause every function has errorcheck

//init entweder über CYCLIC oder über INIT und anschließendem manuellem UPDATE
int Hamlib_CYCLIC(void (*initfunction)(),void *(*updatefunction)(),char *init5Modules) 
//hrend hsound hinput hgui hfio hcaminput
{
	int error=Hamlib_CHECK();
	const char * init=init5Modules;

	system("color 80"); //78

	if(init[0]=='1') 
	{
		if(init[1]=='1') //window?
		{
			hrend_INIT(1024,768,0);
		}
		else
		{
			hrend_INIT(1024,768,1);
		}
	}
	srand((unsigned int)CurTime()*1000);
	if(init[2]=='1')
	{
		//**hsound_INIT();
	}
	if(init[3]=='1')
	{
		hinput_INIT();
	}
	if(init[4]=='1')
	{
		hgui_INIT();
	}
	if(init[5]=='1')
	{
		hfio_INIT();
	}
	if(init[6]=='0')
	{
		//**hvb_INIT();
	}
	if(init[7]=='1')
	{
		//**hcom_INIT();
	}
	//other modules:
	//**hsom_INIT();

	if(initfunction!=NULL)
	{
		initfunction();
	}
	for(;;)
	{
		if(updatefunction!=NULL)
		{
			updatefunction();
		}

		hrend_UPDATE();
		//**hsound_UPDATE();
		hinput_UPDATE();
		hgui_UPDATE();
		hfio_UPDATE();

	}
	return error;
}

int Hamlib_UPDATE()
{
		hrend_UPDATE();
		//**hsound_UPDATE();
		hinput_UPDATE();
		hgui_UPDATE();
		hfio_UPDATE();

		return 0;
}
int Hamlib_INIT(char * init5Modules)
{
	int error=Hamlib_CHECK();
	const char * init=init5Modules;

	system("color 80"); //78

	if(init[0]=='1') 
	{
		if(init[1]=='1') //window?
		{
			hrend_INIT(1024,768,0);
		}
		else
		{
			hrend_INIT(1024,768,1);
		}
	}
	srand(CurTime()*1000);
	if(init[2]=='1')
	{
		//**hsound_INIT();
	}
	if(init[3]=='1')
	{
		hinput_INIT();
	}
	if(init[4]=='1')
	{
		hgui_INIT();
	}
	if(init[5]=='1')
	{
		hfio_INIT();
	}
	if(init[6]=='0')
	{
		//**hvb_INIT(); //wird erneuert
	}
	if(init[7]=='1')
	{
		//**hcom_INIT();
	}

	return error;
}

