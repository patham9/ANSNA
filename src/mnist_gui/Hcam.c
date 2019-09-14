#include "Hcam.h"

void hcam_keydown(EventArgs *e) 
{
	int key=e->mk;
	if(key=='Q')
	{
		hcam.xrot+=1;
		if(hcam.xrot>360)
		{
			hcam.xrot-=360;
		}
	}
	if(key=='E')
	{
		hcam.xrot-=1;
		if(hcam.xrot<-360)
		{
			hcam.xrot+=360;
		}
	}
	if(key=='W')
	{
		float xrotrad, yrotrad;
		yrotrad=(float)(hcam.yrot/180*PI);
		xrotrad=(float)(hcam.xrot/180*PI);
		hcam.xpos+=(float)(sin(yrotrad));
		hcam.zpos-=(float)(cos(yrotrad));
		hcam.ypos-=(float)(sin(xrotrad));
	}
	if(key=='S')
	{
		float xrotrad,yrotrad;
		yrotrad=(float)(hcam.yrot/180*PI);
		xrotrad=(float)(hcam.xrot/180*PI);
		hcam.xpos-=(float)(sin(yrotrad));
		hcam.zpos+=(float)(cos(yrotrad)) ;
		hcam.ypos+=(float)(sin(xrotrad));
	}
	if(key=='D')
	{
		hcam.yrot+=1;
		if(hcam.yrot>360)
		{
			hcam.yrot-=360;
		}
	}
	if(key=='A')
	{
		hcam.yrot-=1;
		if(hcam.yrot<-360)
		{
			hcam.yrot += 360;
		}
	}
	if(key==27)
	{
		exit(0);
	}
}
int lastx=512,lasty=384;
void hcam_mousedragged(EventArgs *e)
{
	if(e->mk==3)
	{
		float difx=(float)lastx-e->mx;
		float dify=(float)lasty-e->my;

		hcam.xrot-=dify*hcam.camzahigkeit;
		if(hcam.xrot<-360)
		{
			hcam.xrot+=360;
		}
		hcam.yrot-=difx*hcam.camzahigkeit;
		if(hcam.yrot<-360)
		{
			hcam.yrot += 360;
		}
		lastx=e->mx;
		lasty=e->my;
	}
}
void hcam_mousemoved(EventArgs *e) 
{
	lastx=e->mx;
	lasty=e->my;
}

float hcam_saved_xpos,hcam_saved_ypos,hcam_saved_zpos,hcam_saved_xrot,hcam_saved_yrot;
void hcam_SaveCamPos()
{
	hcam_saved_xpos=hcam.xpos;hcam_saved_ypos=hcam.ypos;hcam_saved_zpos=hcam.zpos,hcam_saved_xrot=hcam.xrot,hcam_saved_yrot=hcam.yrot;
}
void hcam_SetCamPos(float x,float y,float z,float xrot,float yrot)
{
	hcam.xpos=x;hcam.ypos=y;hcam.zpos=z;hcam.xrot=xrot;hcam.yrot=yrot;
}
void hcam_LoadCamPos()
{
	hcam.xpos=hcam_saved_xpos;hcam.ypos=hcam_saved_ypos;hcam.zpos=hcam_saved_zpos,hcam.xrot=hcam_saved_xrot,hcam.yrot=hcam_saved_yrot;
}

///////////STANDARD-MODULE-FUNCTIONS///////////
void hcam_CHECK(){}

void hcam_INIT()
{
	hcam.mode=MODE;
	hcam.init=1;
	hcam.xpos=0.0f;
	hcam.ypos=0.0f;
	hcam.zpos=0.0f;
	hcam.xrot=0.0f;
	hcam.yrot=0.0f;
	hcam.angle=0.0f;
	hcam.camzahigkeit=0.3f;

	hinput_AddMouseMoved((EventHandler)hcam_mousemoved);
	hinput_AddMouseDragged((EventHandler)hcam_mousedragged);
	hinput_AddKeyDown((EventHandler)hcam_keydown);
}
void hcam_UPDATE(void)
{
	if(hcam.init)
	{
		glRotatef(hcam.xrot,1.0,0.0,0.0);
		glRotatef(hcam.yrot,0.0,1.0,0.0);
		glTranslated(-hcam.xpos,-hcam.ypos,-hcam.zpos);
	}
}
///////////////////////////////////////////////
