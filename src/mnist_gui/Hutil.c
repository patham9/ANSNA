#include "Hutil.h"

//**void **array_malloc(int typesize,int nDimensions,int *szDimensions)
//**{
//**	int i;
//**	long curdim=1;
//**	void **ret;
//**	for(i=0;i<nDimensions;i++)
//**	{
//**		curdim*=szDimensions[i];	
//**	}
//**	ret=(void**)malloc(curdim*typesize);
//**	return ret;
//**}
//**int array_index(int nDimensions,int *szDimensions,int *coords)
//**{
//**	int index,i,curdim=1;
//**	index=coords[0];
//**	for(i=1;i<nDimensions;i++)
//**	{
//**		curdim*=szDimensions[i];
//**		index+=curdim*coords[i];
//**	}
//**	return index;
//**}
//**/////////////export:
//**int array_Size(array *arr)
//**{
//**	int i,curdim=1;
//**	for(i=0;i<arr->nDimensions;i++)
//**	{
//**		curdim*=arr->szDimensions[i];
//**	}
//**	return curdim;
//**}
//**
//**array *array_NEW(int nDimensions,int *szDimensions)
//**{
//**	array *ndim=(array*)malloc(sizeof(array));
//**	ndim->data=(float*)array_malloc(sizeof(float),nDimensions,szDimensions);
//**	ndim->coords=(int*)malloc(nDimensions*sizeof(int));
//**	ndim->szDimensions=szDimensions;
//**	ndim->nDimensions=nDimensions;
//**	return ndim;
//**}
//**array *array_NEW2(int nDimensions,...)
//**{
//**	int i;
//**	va_list ap;
//**	array *ndim=(array*)malloc(sizeof(array));
//**	ndim->szDimensions=(int*)malloc(nDimensions*sizeof(int));
//**	va_start(ap,nDimensions);
//**	for(i=0;i<nDimensions;i++)
//**	{
//**		ndim->szDimensions[i]=va_arg(ap,int);
//**	}
//**	va_end(ap);
//**	ndim->data=(float*)array_malloc(sizeof(float),nDimensions,ndim->szDimensions);
//**	ndim->coords=(int*)malloc(nDimensions*sizeof(int));
//**	ndim->nDimensions=nDimensions;
//**	return ndim;
//**}
//**void array_SetVal(array *arr,float value,int *coords)
//**{
//**	arr->coords=coords;
//**	arr->data[array_index(arr->nDimensions,arr->szDimensions,arr->coords)]=value;
//**}
//**float array_GetVal(array *arr,int *coords)
//**{
//**	arr->coords=coords;
//**	return arr->data[array_index(arr->nDimensions,arr->szDimensions,arr->coords)];
//**}
//**void array_SetVal2(array *arr,float value, ...)
//**{
//**	va_list ap;
//**	int j,ind;
//**	va_start(ap, value);
//**	for(j=0;j<arr->nDimensions;j++)
//**	{
//**		arr->coords[j]=va_arg(ap, int);
//**	}
//**	va_end(ap);
//**	arr->data[array_index(arr->nDimensions,arr->szDimensions,arr->coords)]=value;
//**}
//**float array_GetVal2(array *arr, ...)
//**{
//**	va_list ap;
//**	int j;
//**	va_start(ap, arr);
//**	for(j=0;j<arr->nDimensions;j++)
//**	{
//**		arr->coords[j]=va_arg(ap, int);
//**	}
//**	va_end(ap);
//**	return arr->data[array_index(arr->nDimensions,arr->szDimensions,arr->coords)];
//**}
//**void strprs(char *str,char *delim,void (*parser)(char*))
//**{
//**	strtok(str,delim);
//**	do
//**	{
//**		parse(str);
//**		str=strtok(NULL,delim);
//**	}
//**	while(str);
//**}
char * ftoa(float f)
{
	char * buf=(char*) malloc(16*sizeof(char));
    sprintf(buf, "%f", f);
	return buf;
}
char * dtoa(int d)
{
	char * buf=(char*) malloc(16*sizeof(char));
    sprintf(buf, "%d", d);
	return buf;
}
char * ctoa(char c)
{
	char * buf=(char*) malloc(16*sizeof(char));
    sprintf(buf, "%c", c);
	return buf;
}
#define PI 3.1415
#define PI_2 PI/2
#define PI_4 PI/4
float deg(float value)
{
	return value/3.1415f*180;
}
void *ToVoid(float value)
{
	void* temp=*(void**)&value;
	return temp;
}
float ToFloat(void *value)
{
	float temp=*(float*)&value;
	return temp;
}
double drnd() 
{ 
	return ((double)(rand()%1000000))/1000000;
}
float frnd()
{
	return ((float)(rand()%10000))/10000;
}
float srnd()
{
	return (((float)(rand()%10000))/10000)*2.0f-1.0f;
}
float qrt(float in)
{
	return in*in;
}
int hutil_curtime=0;
double CurTime()
{
#if OS!=9
	return glfwGetTime();
#endif
#if OS!=9
	hutil_curtime++;
	return hutil_curtime;
#endif
}
void Wait(double time)
{
#if OS!=9
	glfwSleep(time);
#endif
}
/*double qrt(double in)
{
	return in*in;
}*/
/*float abs(float in)
{
	return sqrt(in*in);
}*/

float **malloc2Df(int nRows,int nCols)
{
	float ** array2D;
	int i;
	array2D=(float**)malloc(nRows*sizeof(float*));
	for(i=0;i<nRows;i++)
	{
		array2D[i]=(float*)malloc(nCols*sizeof(float));
	}
	return array2D;
}
void memcpy2Df(float **To,float **From,int nRows,int nCols)
{
	int i,j;
	for(i=0;i<nRows;i++)
	{
		for(j=0;j<nRows;j++)
		{
			To[i][j]=From[i][j];
		}
	}
}
void free2Df(float **array2D,int nRows)
{
	int i;
	for(i=0;i<nRows;i++)
	{
		free(array2D[i]);
	}
	free(array2D);
}

//**Thread Thread_NEW(ThreadFunc fun,void *arg)
//**{
//**#if OS!=9 && OS!=1
//**	return glfwCreateThread(fun,arg);
//**#endif
//**#if OS==9
//**	return NULL;
//**#endif
//**}
//**void Thread_DEL(Thread thr)
//**{
//**#if OS!=9 && OS!=1
//**	glfwDestroyThread(thr);
//**#endif
//**}
float RadAngleRange(float ret)
{ //spuckt zwischen 0 und 2*PI aus
	if(ret>2*PI)
	{
		ret-=2*PI;
	}
	if(ret<0)
	{
		ret+=2*PI;
	}
	return ret;
}
