#include "Hinput.h"

int lastkey=-1;
void GLFWCALL hinput_keypressed(int key, int state) //nix exporten
{
	if(state==GLFW_PRESS)
	{
		lastkey=key;
	}
	if(state==GLFW_RELEASE)
	{
		if(key==lastkey)
		{
			lastkey=-1;
		}
	}
}
void hinput_SetGUIMouseDown(EventHandler eh)
{
	hinput.MouseDowns[0]=eh;
}
void hinput_SetGUIMouseDragged(EventHandler eh)
{
	hinput.MouseDraggeds[0]=eh;
}

void hinput_AddMouseDown(EventHandler eh)
{
	hinput.MouseDowns[hinput.nMouseDowns]=eh;
	hinput.nMouseDowns++;
}
void hinput_AddKeyDown(EventHandler eh)
{
	hinput.KeyDowns[hinput.nKeyDowns]=eh;
	hinput.nKeyDowns++;
}
void hinput_AddKeyUp(EventHandler eh)
{
	hinput.KeyUps[hinput.nKeyUps]=eh;
	hinput.nKeyUps++;
}
void hinput_AddMouseUp(EventHandler eh)
{
	hinput.MouseUps[hinput.nMouseUps]=eh;
	hinput.nMouseUps++;
}
void hinput_AddMouseMoved(EventHandler eh)
{
	hinput.MouseMoveds[hinput.nMouseMoveds]=eh;
	hinput.nMouseMoveds++;
}
void hinput_AddMouseDragged(EventHandler eh)
{
	hinput.MouseDraggeds[hinput.nMouseDraggeds]=eh;
	hinput.nMouseDraggeds++;
}
void hinput_AddMouseScroll(EventHandler eh)
{
	hinput.MouseScrolls[hinput.nMouseScrolls]=eh;
	hinput.nMouseScrolls++;
}
///////::
void EventMouseScroll(int pos)
{
	int i=0;
	for(i=0;i<hinput.nMouseScrolls;i++)
	{
		EventArgs e;
		e.mk=pos;
		hinput.MouseScrolls[i](&e);
	}
}

///////////STANDARD-MODULE-FUNCTIONS///////////
void hinput_INIT()
{
	hinput.mode=MODE; 
	hinput.nMouseDraggeds=0;
	hinput.MouseDraggeds[0]=NULL;
	hinput.nMouseMoveds=0;
	hinput.nMouseUps=0;
	hinput.nMouseDowns=1;
	hinput.MouseDowns[0]=NULL;
	hinput.nMouseScrolls=0;
	hinput.nKeyDowns=0;
	hinput.nKeyUps=0;
	hinput.init=1;
	glfwSetKeyCallback(&hinput_keypressed);
}
int lastmx,lastmy;
int mousewasdown;
int lastpressedkey;
int lastmousewheel;
int lastevent=-1;
int hinput_GetLastMousePosX()
{
	int temp=lastmx;
	lastmx=-1;
	return temp;
}
int hinput_GetLastMousePosY()
{
	int temp=lastmy;
	lastmy=-1;
	return temp;
}
int hinput_GetLastMouseEvent()
{
	int temp=lastevent;
	lastevent=-1;
	return temp;
}
int upkey=0,wasKeyDown=0;
void hinput_UPDATE(){
	if(hinput.init)
	{
		EventArgs e;
		int n=0;
		int key=0;
		int mx,my;
		////////MOUSE-HANDLING/////////
		//WHEEL
		if(lastmousewheel!=glfwGetMouseWheel())
		{
			lastmousewheel=glfwGetMouseWheel();
			EventMouseScroll(lastmousewheel);
			lastevent=4;
		}

		//MOUSE

		glfwGetMousePos(&mx,&my);

		if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)>0){key=1;}
		if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)>0){key=2;}
		if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)>0){key=3;}

		if(key>0)
		{
			if(mousewasdown==0){

				e.mx=mx;
				e.my=my;
				e.mk=key;
				if(hinput.MouseDowns[0]==NULL || hinput.MouseDowns[0](&e))
				{
					for(n=1;n<hinput.nMouseDowns;n++)
					{
						hinput.MouseDowns[n](&e);
					}
					lastevent=0;
				}
			}
			mousewasdown=1;

			if(lastmx!=mx || lastmy!=my)
			{
				e.mx=mx;
				e.my=my;
				e.mk=key;
				if(hinput.MouseDraggeds[0]==NULL || hinput.MouseDraggeds[0](&e))
				{
					for(n=1;n<hinput.nMouseDraggeds;n++)
					{
						hinput.MouseDraggeds[n](&e);
					}
					lastevent=3;
				}
			}
		}
		else
		{
			if(mousewasdown==1)
			{
				e.mx=mx;
				e.my=my;
				e.mk=key;
				for(n=0;n<hinput.nMouseUps;n++)
				{
					hinput.MouseUps[n](&e);
				}
				lastevent=1;
				mousewasdown=0;
			}
		}
		if(lastmx!=mx || lastmy!=my)
		{
			e.mx=mx;
			e.my=my;
			for(n=0;n<hinput.nMouseMoveds;n++)
			{
				hinput.MouseMoveds[n](&e);
			}
			lastevent=2;
		}
		lastmx=mx;
		lastmy=my;
		////////////////////////////////////////
		if(lastkey!=-1)
		{
			EventArgs e;
			int i=0;
			e.mk=lastkey;
			for(i=0;i<hinput.nKeyDowns;i++)
			{
				hinput.KeyDowns[i](&e);
			}
			upkey=lastkey;
			wasKeyDown=1;
		}
		else
		if(wasKeyDown==1)
		{
			EventArgs e;
			int i=0;
			wasKeyDown=0;
			e.mk=upkey;
			for(i=0;i<hinput.nKeyUps;i++)
			{
				
				hinput.KeyUps[i](&e);
			}
		}
	}
}
void hinput_CHECK(){}
///////////////////////////////////////////////

