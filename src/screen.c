#include <screen.h>
#include "dwt.h"


uint8_t graph[MAX_X];

void drawFrame()
{
	u16 x,y, step=40;

	LCD_Clear(BLACK);
	POINT_COLOR = GRAY;	//�Drawing�pen color
	BACK_COLOR  = BLACK;
return;
	for( y=step; y<MAX_Y; y+=step )
		LCD_DrawLine(0,y,MAX_X,y);

	for( x=step; x<MAX_X; x+=step )
		LCD_DrawLine(x,0,x,MAX_Y);
}


int triggerStart()
{
    int i;
    u8  trgLvl = 128;
    u8  trgRdy = 0;

    for( i=0; i<SAMPLES_2_BUFFER_SIZE/2; i++ )
    {
        if( trgRdy==0 )
        {
            if( samplesBuffer.two[i][1]<trgLvl )
                trgRdy = 1;
            continue;
        }

        if( samplesBuffer.two[i][1]>trgLvl )
            return i;
    }
    return 0;
}

// start position in buffer
// number of samples to display

uint32_t BuildGraphTick;
void buildGraph()
{
    uint32_t t0 = DWT_Get_Current_Tick();
    int    i, j;
    float  scaleX, x; //, scaleY=1;
    scaleX = 0.5 ; // (float)320 / (float)(SAMPLES_2_BUFFER_SIZE/2);

    x=0; j=-1;
    i = triggerStart();
    for( ; i<SAMPLES_2_BUFFER_SIZE/2; i++ )
    {
        if( (int)x!=j )
        {
            j = (int)x;
            if( j>MAX_X ) break;
            graph[j] = samplesBuffer.two[i][1];
        } else
        {
            graph[j] = (graph[j]+samplesBuffer.two[i][1]) >>1; // arithmetical mean
        }
        x += scaleX;
    }
    BuildGraphTick = DWT_Elapsed_Tick(t0);
}

uint32_t DrawGraphTick;
void drawGraph()
{
  buildGraph();
  uint32_t t0 = DWT_Get_Current_Tick();
  for(u16 i=0; i<MAX_X; i++)
  {
//    LCD_Fast_DrawPoint(i, samplesBuffer.two[i][1], CYAN);
    LCD_Fast_DrawPoint(i,graph[i],CYAN);
  }
  DrawGraphTick = DWT_Elapsed_Tick(t0);
}
