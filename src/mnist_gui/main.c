#include "Hamlib.h"

#include "./../SDR.h"
#include "./../Encode.h"
#include "./../ANSNA.h"

//BEGIN PONG TEST
bool ANSNA_Pong_Left_executed = false;
void ANSNA_Pong_Left()
{
    ANSNA_Pong_Left_executed = true;
}
bool ANSNA_Pong_Right_executed = false;
void ANSNA_Pong_Right()
{
    ANSNA_Pong_Right_executed = true;
}
bool ANSNA_Pong_Stop_executed = false;
void ANSNA_Pong_Stop()
{
    ANSNA_Pong_Stop_executed = true;
}
int szX = 50;
int szY = 20;
int ballX = 25;
int ballY = 4;
int batX = 20;
int batVX = 0;
int batWidth = 4; //"radius", batWidth from middle to the left and right
int vX = 1;
int vY = 1;
int hits = 0;
int misses = 0;
void ANSNA_PongInit()
{
    OUTPUT = 0;
    ANSNA_INIT();
    puts(">>ANSNA Pong start");
    ANSNA_AddOperation(Encode_Term("op_left"), ANSNA_Pong_Left); 
    ANSNA_AddOperation(Encode_Term("op_right"), ANSNA_Pong_Right); 
}
void ANSNA_PongIter(bool useNumericEncoding)
{
    fputs("\033[1;1H\033[2J", stdout); //POSIX clear screen
    for(int i=0; i<batX-batWidth+1; i++)
    {
        fputs(" ", stdout);
    }
    for(int i=0; i<batWidth*2-1 ;i++)
    {
        fputs("@", stdout);
    }
    puts("");
    for(int i=0; i<ballY; i++)
    {
        for(int k=0; k<szX; k++)
        {
            fputs(" ", stdout);
        }
        puts("|");
    }
    for(int i=0; i<ballX; i++)
    {
        fputs(" ", stdout);
    }
    fputs("#", stdout);
    for(int i=ballX+1; i<szX; i++)
    {
        fputs(" ", stdout);
    }
    puts("|");
    for(int i=ballY+1; i<szY; i++)
    {
        for(int k=0; k<szX; k++)
        {
            fputs(" ", stdout);
        }
        puts("|");
    }
    if(useNumericEncoding)
    {
        SDR sdrX = Encode_Scalar(0, 2*szX, szX+(ballX-batX));
        //SDR_PrintWhereTrue(&sdrX);
        ANSNA_AddInputBelief(sdrX);
    }
    else
    {
        if(batX < ballX)
        {
            ANSNA_AddInputBelief(Encode_Term("ball_right"));
        }
        if(ballX < batX)
        {
            ANSNA_AddInputBelief(Encode_Term("ball_left"));
        }
    }
    ANSNA_AddInputGoal(Encode_Term("good_ansna"));
    if(ballX <= 0)
    {
        vX = 1;
    }
    if(ballX >= szX-1)
    {
        vX = -1;
    }
    if(ballY <= 0)
    {
        vY = 1;
    }
    if(ballY >= szY-1)
    {
        vY = -1;
    }
    ballX += vX;
    ballY += vY;
    if(ballY == 0)
    {
        if(abs(ballX-batX) <= batWidth)
        {
            ANSNA_AddInputBelief(Encode_Term("good_ansna"));
            puts("good");
            hits++;
        }
        else
        {
            //ANSNA_AddInput(Encode_Term("good_ansna"), EVENT_TYPE_BELIEF, (Truth) {.frequency = 0, .confidence = 0.9}, "good_ansna");
            puts("bad");
            misses++;
        }
    }
    if(ballY == 0 || ballX == 0 || ballX >= szX-1)
    {
        ballY = szY/2+rand()%(szY/2);
        ballX = rand()%szX;
        vX = rand()%2 == 0 ? 1 : -1;
    }
    if(ANSNA_Pong_Left_executed)
    {
        ANSNA_Pong_Left_executed = false;
        puts("Exec: op_left");
        batVX = -2;
    }
    if(ANSNA_Pong_Right_executed)
    {
        ANSNA_Pong_Right_executed = false;
        puts("Exec: op_right");
        batVX = 2;
    }
    batX=MAX(0,MIN(szX-1,batX+batVX*batWidth/2));
    printf("Hits=%d misses=%d ratio=%f time=%ld\n", hits, misses, (float) (((float) hits) / ((float) misses)), currentTime);
}
//END PONG TEST



float positionX=0,angle=0,size=0.1,clicks=0;

GLuint texture;
#define width 32 //64 //only using 46 currently but ok
#define height 32 //texture needs to be power of 2
int sdrwidth = 32; //46 //how to show the sdr
int sdrheight = 32;
void DrawSDR(int posX, int posY, int r, int g, int b, SDR *sdr, bool init, double scale)
{
    glActiveTexture(GL_TEXTURE0);
    unsigned char colors[(width)*(height)] = {0};
    int i = 0;
    int sdri = 0;
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            if(x<sdrwidth && y<sdrheight)
            {
                //SDR_WriteBit(&sdr, sdri, rand()%2);
                int sdr_bit = 0; //rand()%255; //x<width/2 ? 255: 0;
                if(sdri<SDR_SIZE && SDR_ReadBit(sdr, sdri))//SDR_ReadBit(&sdr, sdri)) //if(sdri<SDR_SIZE && sdri>400 && sdri<1700)
                {
                    sdr_bit = 255;
                }
                colors[i] = sdr_bit;      //r
                sdri++;
            }
            else
            {
                colors[i] = 0; 
            }
            i += 1;
        }
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    //if(init)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, &colors);
    }
    /*else
    {
        glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height, GL_RED, GL_UNSIGNED_BYTE, &colors);
    }*/
    glBindTexture(GL_TEXTURE_2D, texture);
    hrend_DrawObj(posX*0.1, posY*0.1, 0, scale, 1.0, texture);
}

int mark[CONCEPT_LAYERS] = {0};
int position = 0;
bool initflag=true;
SDR globSDR = {0};
void draw()
{
    //ANSNA_PongIter(true);
    //globSDR = Encode_Scalar(0,SDR_SIZE,position);
    position = (position+1) % SDR_SIZE;
    for(int l=0; l<CONCEPT_LAYERS; l++)
    {
        int minconcept = -1;
        for(int k=0; k<concepts[l].itemsAmount; k++)
        {
            Concept *candidate = (Concept*) concepts[l].items[k].address;
            int minconcept_candidate = candidate->id;
            //sort according to id in each layer
            /*for(int k2=0; k2<concepts[l].itemsAmount; k2++)
            {
                int id = ((Concept*) concepts[l].items[k2].address)->id;
                if(id > minconcept && id<=minconcept_candidate)
                {
                    minconcept_candidate = id;
                    candidate = (Concept*) concepts[l].items[k2].address;
                }
            }*/
            minconcept = minconcept_candidate;
            //sdr = Encode_Scalar(0,n_concepts,k);
            SDR *sdrp = &candidate->sdr;
            DrawSDR(((double)k), ((double)l), 255, l, 0, sdrp, initflag,0.03);
            if(mark[l] == candidate->id)
            {
                //hrend_DrawIcon(((double)k)*0.1,((double)l)*0.1, 0, 0.03, 1.0f, marker);
                hrend_DrawObj(((double)k)*0.1,((double)l)*0.1-0.03,  0, 0.01, 1.0f, NULL);
            }
        }
    }
    initflag=false;
    //hrend_DrawObj(positionX+((float)i)*0.1,((float)j)*0.1,angle,size,1.0f,NULL);
}
void draw_GUI()
{
    glPushMatrix();
    //glScalef(8.0,8.0,1.0);
    DrawSDR(1.0, 9.0, 255, 0, 0, &globSDR, initflag,0.06);
    glPopMatrix();
	//hrend_DrawPixelValue(positionX,0.01,0.9,2.5,0.016);
    
}
void mouse_dragged(EventArgs *e)
{
    int x = (e->mx-80)/2;
    int y = (e->my-0)/2;
    printf("%d %d\n", x, y);
    int index = x*width + y;
    printf("%d\n", index);
    if(x>=0 && y>=0 && x < sdrwidth && y < sdrheight)
    {
        int realy = y;
        int realx = sdrwidth - x;
        SDR_WriteBit(&globSDR,realy*sdrwidth + realx,1);
        for(int i=0;i<3;i++)
        {
            if(realy*sdrwidth + (realx+i) < SDR_SIZE)
            {
                SDR_WriteBit(&globSDR,realy*sdrwidth + (realx+i),1);
            }
            if((realy+1)*sdrwidth + realx < SDR_SIZE)
            {
                SDR_WriteBit(&globSDR,(realy+i)*sdrwidth + realx,1);
            }
            if((realy+1)*sdrwidth + (realx+i) < SDR_SIZE)
            {
                SDR_WriteBit(&globSDR,(realy+i)*sdrwidth + (realx+i),1);
            }
        }
    }
}
void mouse_down(EventArgs *e)
{
	if(e->mk==1)
	{
		float abstandX=(positionX-hnav_MouseToWorldCoordX(e->mx));
		float abstandY=(0        -hnav_MouseToWorldCoordY(e->my));
		if(sqrt(abstandX*abstandX+abstandY*abstandY)<size)
		{
			positionX+=0.1;
		}
	}
}
SDR svSDR = {0};
void key_used(EventArgs *e)
{
    if(e->mk=='D') //detect
    {
        for(int l=0; l<CONCEPT_LAYERS; l++)
        {
            int retInd = 0;
            Memory_getClosestConcept(l, &globSDR, SDR_Hash(&globSDR), &retInd);
            mark[l] = ((Concept*)concepts[l].items[retInd].address)->id;
        }
    }
	if(e->mk=='I') //input
    {
        ANSNA_AddInputBelief(globSDR);
    }
    if(e->mk=='S') //save
    {
        svSDR = globSDR;
    }
    if(e->mk=='L') //load
    {
        globSDR = svSDR;
    }
    if(e->mk=='C') //clear
    {
        globSDR = (SDR) {0};
    }
}
bool getimage(char* line)
{
    SDR toFill = {0};
    const char* tok;
    int skip = 1;
    int i=0;
    int x=0;
    int y=0;
    int k=0;
    for (tok = strtok(line, ",");
            tok && *tok;
            tok = strtok(NULL, ",\n"))
    {
        //if(skip && strcmp(tok,"0") && strcmp(tok,"1"))
        //{
        //    return false;
        //}
        if (!skip && strcmp(tok,"0"))
        {
            SDR_WriteBit(&toFill, y*sdrwidth+((sdrwidth-1)-x), 1);
        }
        skip = MAX(0, skip-1);
        //printf("i=%d comp=%d",i,strcmp(tok,"0"));
        k++;
        if(i%28 == 0) //next "line" in SDR
        {
            y+=1;
            x=0;
        }
        else
        {
            x++;
        }
        i++;
    }
    globSDR = toFill;
    ANSNA_AddInputBelief(globSDR);
    return true;
}

void minist_load()
{
    FILE* stream = fopen("mnist_train.csv", "r");
    char line[100000];
    int samples = 1000;
    while (samples > 0 && fgets(line, 100000, stream))
    {
        char* tmp = strdup(line);
        if(getimage(tmp))
        {
            samples--;
        }
        // NOTE strtok clobbers tmp
        free(tmp);
    }
}
void init()
{
    OUTPUT = 0;
    ANSNA_INIT();
	hnav_SetRendFunc(draw);
	hrend_SetGUIRenderRoutine(draw_GUI);
	hrend_2Dmode(0.0,0.2,0.0);
	hinput_AddMouseDown(mouse_down);
    hinput_AddMouseDragged(mouse_dragged);
	hinput_AddKeyUp(key_used);
    glGenTextures(1, &texture);
    minist_load();
    //ANSNA_PongInit();
}
int main()
{	
	Hamlib_CYCLIC(init,NULL,"1111111111111");
}

