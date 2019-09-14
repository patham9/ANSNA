#include "Hfio.h"

///////////STANDARD-MODULE-FUNCTIONS///////////
void hfio_INIT()
{
	hfio.mode=MODE;
	hfio.init=1;
}
void hfio_UPDATE(){if(hfio.init){}}
void hfio_CHECK(){}
///////////////////////////////////////////////

char *hfio_textFileRead(const char *fn) 
{
	FILE *fp;
	char *content=NULL;
	int count=0;

	if(fn!=NULL)
	{
		fp=fopen(fn,"rt");
		if(fp!=NULL)
		{
			fseek(fp,0,SEEK_END);
			count=ftell(fp);
			rewind(fp);
			if(count>0)
			{
				content=(char*)malloc((count+1)*sizeof(char));
				count=fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

int hfio_textFileWrite(char *fn, char *s) 
{
	FILE *fp;
	int status=0;

	if (fn!=NULL)
	{
		fp=fopen(fn,"w");
		if(fp!=NULL)
		{
			if(fwrite(s,sizeof(char),strlen(s),fp)==strlen(s))
			{
				status = 1;
			}
			fclose(fp);
		}
	}
	return(status);
}

void hfio_LoadTex(const char *datei,GLuint *ziel)
{
#if EXT==1
	FILE *fp=fopen(datei,"r");
	if (!fp)
	{
		printf(datei);
		printf(" does not exist!\n");
	}
	if(fp!=NULL)
	{
		fclose(fp);
	}
	glGenTextures(1, ziel);
	glBindTexture(GL_TEXTURE_2D, *ziel);
	if (!glfwLoadTexture2D(datei, GLFW_BUILD_MIPMAPS_BIT))
	{
		printf("failed to load ");
		printf(datei);
		printf("\n");
	}
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //GL_LINEAR_MIPMAP_LINEAR
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
#endif
}