#include "Hgui.h"

struct Button
{
	unsigned char exist;
	float x;
	float y;
	char * text;
	float textsize;
	float textdist;
	float red;
	float green;
	float blue;
	float alpha;
	float width; 
	float height;
	unsigned char visible;
	Hgui_Event OnClick;
	Hgui_Event OnDrag;
	Hgui_Event OnReleased;
	float textoffsetX;
	int INFO; //OBJECT
};

#define maxbuttons 255
#define maxguis 16

struct GUI
{
	int exist;
	struct Button buttons[maxbuttons];
	int nButtons;
}gui[maxguis];

int activeGUI=0;	//SELECT GUI INSTANCE

unsigned int hgui_AddElem(float px,float py,float width,float height,char *text,float textsize,float textdist,float textoffsetX,Hgui_Event OnClick,unsigned char visible,int INFO)
{
	gui[activeGUI].buttons[gui[activeGUI].nButtons].x=px;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].y=py;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].width=width;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].height=height;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].text=text;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].textsize=textsize;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].textdist=textdist;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].textoffsetX=textoffsetX;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].OnClick=OnClick;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].exist=1;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].visible=visible;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].INFO=INFO;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].red=-1;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].blue=-1;
	gui[activeGUI].buttons[gui[activeGUI].nButtons].green=-1;
	return gui[activeGUI].nButtons++;
}
unsigned int hgui_AddSimpleElem(float px,float py,float width,float height,char *text,Hgui_Event OnClick,int INFO)
{
	return hgui_AddElem(px,py,width,height,text,2.0f,0.015f,0.01f,OnClick,1,INFO);
}
void hgui_SetElemColor(unsigned int button,float r,float g,float b,float a)
{
	gui[activeGUI].buttons[button].red=r;
	gui[activeGUI].buttons[button].blue=g;
	gui[activeGUI].buttons[button].green=b;
	gui[activeGUI].buttons[button].alpha=a;
}

struct float4 hgui_GetElemColor(unsigned int button)
{
	struct float4 ret;
	ret.x=gui[activeGUI].buttons[button].red;
	ret.y=gui[activeGUI].buttons[button].green;
	ret.z=gui[activeGUI].buttons[button].blue;
	ret.w=gui[activeGUI].buttons[button].alpha;
	return ret;
}
void hgui_SetElemText(unsigned int button,char *text)
{
	gui[activeGUI].buttons[button].text=text;
}
void hgui_SetElemOnDrag(unsigned int button,Hgui_Event ev)
{
	//gui[activeGUI].buttons[button].OnDrag=ev;
}
void hgui_SetElemInfo(unsigned int button,int INFO)
{
	gui[activeGUI].buttons[button].INFO=INFO;
}
char *hgui_GetElemText(unsigned int button)
{
	return gui[activeGUI].buttons[button].text;
}
int hgui_GetElemInfo(unsigned int button)
{
	return gui[activeGUI].buttons[button].INFO;
}
void hgui_DeactivateElem(unsigned int button)
{
	gui[activeGUI].buttons[button].exist=0;
	gui[activeGUI].buttons[button].visible=0;
}
void hgui_ActivateElem(unsigned int button)
{
	gui[activeGUI].buttons[button].exist=1;
	gui[activeGUI].buttons[button].visible=1;
}
void hgui_SetActiveGUI(int gui)
{
	activeGUI=gui;
}
//**void hgui_ShowWindow(GtkWidget *Window)
//**{
//**	gtk_widget_show(Window);
//**}
struct point{float px;float py;};
unsigned char IsInArea(struct point pLeftUp,struct point pRightDown,struct point IsIn)
{
	if(IsIn.px>pLeftUp.px && IsIn.py<pLeftUp.py && IsIn.px<pRightDown.px && IsIn.py>pRightDown.py)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int lastclickedbutton=-1;
int hgui_GetLastClickedElem()
{
	int temp=lastclickedbutton;
	lastclickedbutton=-1;
	return temp;
}
int mouseClicked(EventArgs *e)
{
#ifdef Hrend //butonevents without rendering button->senseless

	int i=0;
	for(i=0;i<gui[activeGUI].nButtons;i++)
	{

		struct point leftup={gui[activeGUI].buttons[i].x,gui[activeGUI].buttons[i].y+gui[activeGUI].buttons[i].height};
		struct point rightdown={gui[activeGUI].buttons[i].x+gui[activeGUI].buttons[i].width,gui[activeGUI].buttons[i].y};
		struct point isin={(float)(e->mx)/hrend.width,1.0f+(float)(1-e->my)/hrend.height};

		if(gui[activeGUI].buttons[i].exist && IsInArea(leftup,rightdown,isin))
		{
			if(gui[activeGUI].buttons[i].OnClick!=NULL)
			{
				gui[activeGUI].buttons[i].OnClick(gui[activeGUI].buttons[i].INFO);
			}
			return 0;
		}

	}
	return 1;
#endif
}

int mouseDragged(EventArgs *e)
{
#ifdef Hrend //needs to get fixed

	int i=0;
	for(i=0;i<gui[activeGUI].nButtons;i++)
	{

		struct point leftup={gui[activeGUI].buttons[i].x,gui[activeGUI].buttons[i].y+gui[activeGUI].buttons[i].height};
		struct point rightdown={gui[activeGUI].buttons[i].x+gui[activeGUI].buttons[i].width,gui[activeGUI].buttons[i].y};
		struct point isin={(float)(e->mx)/hrend.width,1.0f+(float)(1-e->my)/hrend.height};

		if(gui[activeGUI].buttons[i].OnDrag!=NULL)
		{
			if(gui[activeGUI].buttons[i].exist && IsInArea(leftup,rightdown,isin))
			{
				gui[activeGUI].buttons[i].OnDrag(gui[activeGUI].buttons[i].INFO);
			}
			return 0;
		}
	}
	return 1;
#endif
}
///////////////RENDERING
#define ElemSize 1.0f //0.015
GLuint ControlDL=0;
void generateControlDisplayList()
{
#if EXT==1
	
	float px=-ElemSize;
	float py=-ElemSize;
	float px2=ElemSize;
	float py2=ElemSize;
	//glLoadIdentity();
	ControlDL=glGenLists(1);
	glNewList(ControlDL,GL_COMPILE);

	glBegin(GL_POLYGON);
	glTexCoord2f(1, 1);
	glVertex2f(px, py);
	glTexCoord2f(0, 1);
	glVertex2f(px2, py);
	glTexCoord2f(0, 0);
	glVertex2f(px2, py2);
	glTexCoord2f(1, 0);
	glVertex2f(px, py2);
	glEnd();

	glEndList();

#endif
}
GLuint buttonTEX;
void hgui_RENDER()
{
	float px=-ElemSize;
	float py=-ElemSize;
	float px2=ElemSize;
	float py2=ElemSize;
int i=0;

#ifdef Hrend

	for(i=0;i<gui[activeGUI].nButtons;i++)
	{
		if(gui[activeGUI].buttons[i].exist && gui[activeGUI].buttons[i].visible)
		{
			if(gui[activeGUI].buttons[i].red!=-1) //eine farbkomponente abfragen reicht aus
			{
				hrend_SelectColor(gui[activeGUI].buttons[i].red,gui[activeGUI].buttons[i].green,gui[activeGUI].buttons[i].blue,gui[activeGUI].buttons[i].alpha);
				//hrend_DisableTex();
				
			}
			else
			{
				hrend_SelectColor(1.0f,1.0f,1.0f,0.0f);
			}
			glBindTexture(GL_TEXTURE_2D,buttonTEX);
			glPushMatrix();
			glTranslatef(gui[activeGUI].buttons[i].x+gui[activeGUI].buttons[i].width/2,gui[activeGUI].buttons[i].y+gui[activeGUI].buttons[i].height/2,0);
			glScalef(gui[activeGUI].buttons[i].width/2,gui[activeGUI].buttons[i].height/2,0);
			glCallList(ControlDL);
			glPopMatrix();
			glDisable(GL_TEXTURE_2D);
			hrend_DrawPixelString(gui[activeGUI].buttons[i].text,
			gui[activeGUI].buttons[i].x+gui[activeGUI].buttons[i].textoffsetX,gui[activeGUI].buttons[i].y+gui[activeGUI].buttons[i].height/2.0f-0.005f,
			gui[activeGUI].buttons[i].textsize,gui[activeGUI].buttons[i].textdist);
			glEnable(GL_TEXTURE_2D);
		}
	}
#endif
}
void hgui_setCallBack(void (*CallBackGUIFunc)())
{
	hgui.CallbackUpdate=CallBackGUIFunc;
}
///////////STANDARD-MODULE-FUNCTIONS///////////
void hgui_INIT()
{
	int i=0;
	hgui.mode=MODE; 
	hgui.CallbackUpdate=NULL;
	hinput_SetGUIMouseDown((EventHandler)mouseClicked);
	hinput_SetGUIMouseDragged((EventHandler)mouseDragged);
	hrend_SetGUIRenderRoutine(hgui_RENDER);
	//**gtk_init(NULL, NULL);

	for(i=0;i<maxguis;i++){gui[i].nButtons=0;}
#ifdef Hrend
	hfio_LoadTex("GUI/button.tga",&buttonTEX);
	generateControlDisplayList();
#endif
	hgui.init=1;
}
void hgui_UPDATE()
{
	if(hgui.init)
	{
		//**gtk_main_iteration_do(0);
		if(hgui.CallbackUpdate!=NULL)
		{
			hgui.CallbackUpdate();
		}
	}
}
void hgui_CHECK(){}
///////////////////////////////////////////////

