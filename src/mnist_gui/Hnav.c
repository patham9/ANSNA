#include "Hnav.h"

float savepx=0;
float savepy=0;
int selID=0;

float hnav_MouseToWorldCoordX(int x)
{
	return 1/hnav.zoom*(((float)x)/hrend.width-(hnav.difx+0.5f));
}
float hnav_MouseToWorldCoordY(int y)
{
	return 1/hnav.zoom*(1.0f-((float)y)/hrend.height-(hnav.dify+0.5f));
}
int md=0;
void mousedown(EventArgs *e)
{
	md=1;
	if(e->mk==3 || e->mk==1)
	{
		savepx=e->mx; savepy=e->my;
		hrend_SetCamera(savepx,savepy,0,0,0);
	}
}
void mouseup(EventArgs *e)
{
	md=0;	
}
int newposx=-999;
int newposy=-999;
void mousemoved(EventArgs *e)
{
	newposx=e->mx;
	newposy=e->my;
}
void mousedragged(EventArgs *e)
{
	if(e->mk==3)
	{
		hnav.difx+=(e->mx-savepx)/hrend.width;
		hnav.dify+=-(e->my-savepy)/hrend.height;
		savepx=e->mx; savepy=e->my;
	}
	if(md)
	{
		mousedown(e);
	}
}
int lastscr=0;
#define scrollcamspeed 1.3
void mousescroll(EventArgs *e)
{
	float zoomBefore=hnav.zoom;
	if(e->mk-lastscr>0)
	{
		hnav.zoom*=scrollcamspeed;
	}
	else
	{
		hnav.zoom/=scrollcamspeed;
	}

	lastscr=e->mk;

	hnav.difx=(hnav.difx)*(hnav.zoom/zoomBefore);
	hnav.dify=(hnav.dify)*(hnav.zoom/zoomBefore);
}
////////////::::::::::::::::::::::::::::::::::::///////////////////
#define camspeed 20.0f
#define scrollcammult 0.92
void keydown(EventArgs *e)
{
	if(e->mk==KEY_UP)
	{
		hnav.dify+=(-camspeed)/(float)hrend.height;
	}
	if(e->mk==KEY_DOWN)
	{
		hnav.dify+=(camspeed)/(float)hrend.height;
	}
	if(e->mk==KEY_LEFT)
	{
		hnav.difx+=(camspeed)/(float)hrend.width;
	}
	if(e->mk==KEY_RIGHT)
	{
		hnav.difx+=(-camspeed)/(float)hrend.width;
	}
	if(e->mk==KEY_MINUS || e->mk=='#')
	{
		float zoomBefore=hnav.zoom;
		hnav.zoom*=scrollcammult;
		hnav.difx=(hnav.difx)*(hnav.zoom/zoomBefore);
		hnav.dify=(hnav.dify)*(hnav.zoom/zoomBefore);
	}
	if(e->mk==KEY_PLUS || e->mk=='+')
	{
		float zoomBefore=hnav.zoom;
		hnav.zoom/=scrollcammult;
		hnav.difx=(hnav.difx)*(hnav.zoom/zoomBefore);
		hnav.dify=(hnav.dify)*(hnav.zoom/zoomBefore);
	}
}
void (*RendFunc)(void);
void hnav_Transform()
{
	glTranslatef(hnav.difx+0.5f,hnav.dify+0.5f,0);
	glScalef(hnav.zoom,hnav.zoom,1.0f);
}
void rend()
{
	hnav_Transform();
	if(RendFunc!=NULL)
	{
		RendFunc();
	}
}
void hnav_SetRendFunc(void (*RenderFunc)(void))
{
	if(hrend.rendermode==1)
	{
		hrend_AddRenderRoutine(RenderFunc);
	}
	else
	{
		RendFunc=RenderFunc;
	}
}
///////////STANDARD-MODULE-FUNCTIONS///////////
void hnav_INIT()
{
	hnav.mode=MODE; 
//::
	hnav.zoom=1.0f;
	hnav.difx=0.0f;
	hnav.dify=0.0f;
	hinput_AddMouseDown(mousedown);
	hinput_AddMouseUp(mouseup);
	hinput_AddMouseMoved(mousemoved);
	hinput_AddMouseDragged(mousedragged);
	hinput_AddMouseScroll(mousescroll);
	hinput_AddKeyDown(keydown);
	hrend_AddRenderRoutine(rend);
}
void hnav_UPDATE(){}
void hnav_CHECK(){} //check if module is ready
///////////////////////////////////////////////