#include "Hrend.h"
#include "Htext.def"

const static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
void hrend_Shadow(void(*RenderFunc)(void*),void* data,float colR,float colG,float colB,float colA)
{
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
    glMultMatrixf(ShadowMat);
    glDisable(GL_LIGHTING);
    glColor4f(colR,colG,colB,colA);
	RenderFunc(data);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_LIGHTING);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}
void hrend_LightShadow(float ground[4],float light[4],void(*RenderFunc)(void*),void* data,float colR,float colG,float colB,float colA)
{
    float  dot;
    float  shadowMat[4][4];

    dot = ground[0] * light[0] +
          ground[1] * light[1] +
          ground[2] * light[2] +
          ground[3] * light[3];
    
    shadowMat[0][0] = (float)dot - light[0] * ground[0];
    shadowMat[1][0] = (float)0.0 - light[0] * ground[1];
    shadowMat[2][0] = (float)0.0 - light[0] * ground[2];
    shadowMat[3][0] = (float)0.0 - light[0] * ground[3];
    
    shadowMat[0][1] = (float)0.0 - light[1] * ground[0];
    shadowMat[1][1] = (float)dot - light[1] * ground[1];
    shadowMat[2][1] = (float)0.0 - light[1] * ground[2];
    shadowMat[3][1] = (float)0.0 - light[1] * ground[3];
    
    shadowMat[0][2] = (float)0.0 - light[2] * ground[0];
    shadowMat[1][2] = (float)0.0 - light[2] * ground[1];
    shadowMat[2][2] = (float)dot - light[2] * ground[2];
    shadowMat[3][2] = (float)0.0 - light[2] * ground[3];
    
    shadowMat[0][3] = (float)0.0 - light[3] * ground[0];
    shadowMat[1][3] = (float)0.0 - light[3] * ground[1];
    shadowMat[2][3] = (float)0.0 - light[3] * ground[2];
    shadowMat[3][3] = (float)dot - light[3] * ground[3];

    glDisable(GL_TEXTURE_2D);
	glPushMatrix();
    glMultMatrixf(shadowMat);
    glDisable(GL_LIGHTING);
    glColor4f(colR,colG,colB,colA);
	RenderFunc(data);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_LIGHTING);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}
void set_VGApixel(unsigned char x, unsigned char y, unsigned char color);
unsigned char RGBcolor(unsigned char red, unsigned char green, unsigned char blue);

void hrend_drawpolygon(float px0,float py0,float pz0,float px1,float py1,float pz1,float px2,float py2,float pz2)
{
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(px0, py0, pz0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(px1, py1, pz1);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(px2, py2, pz2);
	glEnd();
}
GLuint IconDL;
///////////////////////////////////////////////////////
void hrend_SetCamera(float px, float py, float pz, float roll, float pitch)
{
	hrend.px=px; hrend.py=py; hrend.pz=pz;
	hrend.roll=roll; hrend.pitch=pitch;
}
///////////////////////////////////////////////////////
void hrend_SelectColor(float r,float g,float b,float a)
{
	glColor3f(r,g,b);
}
///////////////////////////////////////////////////////
void hrend_Translate(float x,float y,float z)
{
	glTranslatef(x,y,z);
}
///////////////////////////////////////////////////////
void hrend_Rotate(float x,float y,float z)
{
	glRotatef(x,1,0,0);
	glRotatef(y,0,1,0);
	glRotatef(z,0,0,1);
}
///////////////////////////////////////////////////////
void hrend_EnableTex()
{
		glEnable(GL_TEXTURE_2D);
}
///////////////////////////////////////////////////////
void hrend_DisableTex()
{
		glDisable(GL_TEXTURE_2D);
}
///////////////////////////////////////////////////////
void hrend_Scale(float x,float y,float z)
{
	glScalef(x,y,z);
}
void hrend_ResetMatrix()
{
	glLoadIdentity();
}
///////////////////////////////////////////////////////
void hrend_EnableSmooth()
{
	glEnable(GL_LINE_SMOOTH);
	//smoothshading und so auch noch
}
///////////////////////////////////////////////////////
void hrend_DisableSmooth()
{
	glDisable(GL_LINE_SMOOTH);
}
///////////////////////////////////////////////////////
void hrend_DrawLine(float x1,float y1,float z1,float x2,float y2,float z2)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glVertex3f(x1,y1,z1);
	glVertex3f(x2,y2,z2);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}
///////////////////////////////////////////////////////
#define ElemSize 0.005f
void hrend_DrawIcon(float px, float py,float ang, float size, float lenght,GLuint icon)
{
	glBindTexture(GL_TEXTURE_2D,icon);
	glPushMatrix();
	glTranslatef(px,py,0);
	//glScalef(lenght,0,0);
	glRotatef(ang,0,0,1);
	glScalef(size,size,size);
	glCallList(IconDL);
	glPopMatrix();
}
void hrend_DrawObj(float px, float py,float ang, float size, float lenght,GLuint icon)
{ //size is not fixed
	hrend_DrawIcon(px,py,ang,size/ElemSize,lenght,icon);
}
///////////////////////////////////////////////////////
/////////////////DISPLAY LIST FOR FAST DRAWING OF ELEMS////////
void generateDrawDisplayList()
{
	float px=-ElemSize;
	float py=-ElemSize;
	float px2=ElemSize;
	float py2=ElemSize;
	//glLoadIdentity();
	IconDL=glGenLists(1);
	glNewList(IconDL,GL_COMPILE);

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
}
///////////////////////////////////////////////////////////////
struct point
{
	float px;
	float py;
};
struct buchstabe
{
	float * points;
};
void hrend_Begin()
{
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
}
void hrend_End()
{
	glfwSwapBuffers();
}
void hrend_frame()
{
	int i=0;
	if(hrend.rendermode==0)
	{
		glLoadIdentity();
	}
	glClearColor(hrend.clrR, hrend.clrG, hrend.clrB, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);

	if(hrend.rendermode==1)
	{
		hcam_UPDATE();
	}
	glLightfv(GL_LIGHT0,GL_POSITION,hrend.sunpos);
	for(i=1;i<hrend.nRenderRoutines;i++)
	{
		hrend.RenderRoutines[i]();
	}
	if(hrend.RenderRoutines[0]!=NULL)
	{
		glLoadIdentity();
		if(hrend.rendermode==1)
		{
			glPushMatrix();
			glTranslatef(-0.55,-0.54,-1); //only for now
			glScalef(1.09,1.09,0);
		}
		hrend.RenderRoutines[0]();
		if(hrend.rendermode==1)
		{
			glPopMatrix();
		}
	}
	glfwSwapBuffers();
}
void hrend_AddRenderRoutine(RenderRoutine rend)
{
	hrend.RenderRoutines[hrend.nRenderRoutines]=rend;
	hrend.nRenderRoutines++;
}
void hrend_SetGUIRenderRoutine(RenderRoutine rend) //0=max prio
{
	hrend.RenderRoutines[0]=rend;
}

float hw=0.0f;
float wh=0.0f;
void GLFWCALL WindowResize(int width,int height)
{
	if(width!=0 && height!=0)
	{
		hrend.width=width;
		hrend.height=height;
		glViewport(0,0,hrend.width,hrend.height);
		hw=(float)hrend.height/(float)hrend.width;
		wh=(float)hrend.width/(float)hrend.height;

		if(hrend.rendermode==1)
		{
			glMatrixMode(GL_PROJECTION); 
			glLoadIdentity();
			gluPerspective(60,(GLfloat)w/(GLfloat)h,1.0,1000.0);
			glMatrixMode(GL_MODELVIEW);
		}
	}
}
///////////STANDARD-MODULE-FUNCTIONS///////////
void hrend_2Dmode(float r,float g,float b)
{
	glfwSetWindowTitle("Hamlib 2D mode");
	hrend.rendermode=0;
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	glDisable(GL_LIGHT0);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0,1,0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(r, g, b, 1.0f);	
	hrend.clrR=r;
	hrend.clrG=g;
	hrend.clrB=b;
	WindowResize(hrend.width,hrend.height);
	hnav_INIT();
}
void hrend_3Dmode(float r,float g,float b)
{
	glfwSetWindowTitle("Hamlib 3D mode");
	hrend.rendermode=1;
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_LIGHT0);

	glMatrixMode(GL_PROJECTION);
	glOrtho(0,1,0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(r, g, b, 1.0f);
	hrend.clrR=r;
	hrend.clrG=g;
	hrend.clrB=b;

	hcam_INIT();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);

	hrend.sunpos[0]=0; hrend.sunpos[1]=2000; hrend.sunpos[2]=0; hrend.sunpos[3]=0;
	WindowResize(hrend.width,hrend.height);
}

void hrend_TakeImage(char *image,void (*RenderFunc)(void*),void *data,int resolutionX,int resolutionY,float posx,float posy,float posz,float rotx,float roty)
{
	hcam_SaveCamPos();
	glLoadIdentity();
	hcam_SetCamPos(posx,posy,posz,rotx,roty);
	glViewport(0,0,resolutionX,resolutionY);
	hcam_UPDATE();
	hcam_LoadCamPos();
	glDrawBuffer(GL_BACK);
	RenderFunc(data);
	glReadBuffer(GL_BACK);
	glReadPixels(0,0,resolutionX,resolutionY,GL_BGR,GL_UNSIGNED_BYTE,image);
	glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glViewport(0,0,hrend.width,hrend.height);
	hcam_UPDATE();
}

void hrend_TakeTextureFromImage(GLuint *tex,char *image,int resolutionX,int resolutionY)
{
	GLuint TEXwas;
	glGetIntegerv(GL_TEXTURE_BINDING_2D,&TEXwas);
	glBindTexture(GL_TEXTURE_2D,*tex);
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,resolutionX,resolutionY,GL_BGR,GL_UNSIGNED_BYTE,(GLvoid*)image);
	glBindTexture(GL_TEXTURE_2D,TEXwas);
}
void hrend_InitTextureFromImage(GLuint *tex,char *image,int resolutionX,int resolutionY)
{
	GLuint TEXwas;
	glGetIntegerv(GL_TEXTURE_BINDING_2D,&TEXwas);
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D,*tex);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,resolutionX,resolutionY,0,GL_BGR,GL_UNSIGNED_BYTE,(GLvoid*)image);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //GL_LINEAR_MIPMAP_LINEAR
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D,TEXwas);
}

void hrend_INIT(int width, int height,int fullscreen)
{ 
	////////////////////////////////////////init process glfw glew
	//HANDLE hProcess = GetCurrentProcess(); SetPriorityClass(hProcess,IDLE_PRIORITY_CLASS); //++
	hrend.RenderRoutines[0]=NULL;
	hrend.width=width;
	hrend.height=height;
	/*GLFW*/glfwInit(); 
	if(fullscreen==0)
	{
		if(!glfwOpenWindow(hrend.width,hrend.height,8,8,8,8,24,0,GLFW_WINDOW))
		{
			glfwTerminate(); exit(0);
		}	
	}
	else
	{
		if(!glfwOpenWindow(hrend.width,hrend.height,8,8,8,8,24,0,GLFW_FULLSCREEN))
		{
			glfwTerminate(); exit(0);
		}	
	}
	///*GLEW*/GLenum err=glewInit(); if(GLEW_OK!=err){fprintf(stderr, "Error: %s\n", glewGetErrorString(err));}
	glfwSetWindowTitle("Hamlib Application");
	////////////////////////////////////////

	hrend.clrR=0.0f; hrend.clrG=0.0f; hrend.clrB=0.0f;
	hrend.mode=MODE; hrend.nRenderRoutines=1;  hrend.rendermode=-1;

	glfwSetWindowSizeCallback(WindowResize);
	generateDrawDisplayList();
	hrend.init=1;
	hrend_AddTextDefs();
	glewInit();
}
void hrend_UPDATE(){
	if(hrend.init){
		hrend_frame();
	}}
void hrend_CHECK(){}
///////////////////////////////////////////////

void hrend_CMD_DrawConsole(char **inarr,int size)
{
	int x,y;
	system("cls");
	for(x=0;x<size;x++)
	{
		for(y=0;y<size;y++)
		{
			printf("%c",inarr[x][y]);
		}
		printf("\n");
	}
}
void hrend_CMD_InsertChar(char **inarr,int px,int py,int size,char Char)
{
	px=max(0,px);
	py=max(0,py);
	px=min(px,size-1);
	py=min(py,size-1);
	inarr[px][py]=Char;
}
void hrend_CMD_ClrCharArray(char **inarr,int size)
{
	int x,y;
	for(x=0;x<size;x++)
	{
		for(y=0;y<size;y++)
		{
			inarr[x][y]=' ';
		}
	}
}
char **hrend_CMD_InitCharArray(int size)
{
	int i;
	char **inarr=(char**)malloc(size*sizeof(char*));
	for(i=0;i<size;i++)
	{
		inarr[i]=(char*)malloc(size*sizeof(char));
	}
	hrend_CMD_ClrCharArray(inarr,size);
	return inarr;
}
void hrend_DrawSegmentValue(float val,float px,float py,float size,float distance)
{
	char *ch=(char*)malloc(16*sizeof(char));
	sprintf(ch,"%f",val);
	hrend_DrawSegmentString(ch,px,py,size,distance);
	free(ch);
}
void hrend_DrawPixelValue(float val,float px,float py,float size,float distance)
{
	char *ch=(char*)malloc(16*sizeof(char));
	sprintf(ch,"%f",val);
	hrend_DrawPixelString(ch,px,py,size,distance);
	free(ch);
}
