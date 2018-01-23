#include <screen.h>
#include <dwt.h>


/*
 * Draw frame and oscillogram
 */

uint8_t graph[MAX_X];
float  scaleX = 1;  // no more then 1

void drawFrame() {
  u16 x, y, step = 32;

  LCD_Clear(BLACK);
  POINT_COLOR = GRAY;  // Drawing pen color
  BACK_COLOR = BLACK;

  for (y = step; y < MAX_Y; y += step)
  {
    if (y == 128) POINT_COLOR = GRAY;  // Drawing pen color
    else POINT_COLOR = DARKGRAY;
    LCD_DrawLine(0, y, MAX_X, y);
  }

  for (x = step; x < MAX_X; x += step)
  {
    if (x == 160) POINT_COLOR = GRAY;  // Drawing pen color
    else          POINT_COLOR = DARKGRAY;
    LCD_DrawLine(x, 0, x, MAX_Y); //*/
  }
}

/*
 * Looking for trigger event position in 1 channel samples array
 */
int triggerStart1ch(u8 const *samples)
{
    int i;
    u8  trgLvl = 128;
    u8  trgRdy = 0;

    for( i=0; i<SAMPLES_1_BUFFER_SIZE/2; i++ )
    {
        if( trgRdy==0 )
        {
            if( samples[i]<trgLvl )
                trgRdy = 1;
            continue;
        }

        if( samples[i]>trgLvl )
            return i;
    }
    return 0;
}

/*
 * Looking for trigger event position in 2 channels samples array
 */
int triggerStart2ch(u8 (*samples)[2])
{
    int i;
    u8  trgLvl = 128;
    u8  trgRdy = 0;

    for( i=0; i<SAMPLES_2_BUFFER_SIZE/2; i++ )
    {
        if( trgRdy==0 )
        {
            if( samples[i][1]<trgLvl )
                trgRdy = 1;
            continue;
        }

        if( samples[i][1]>trgLvl )
            return i;
    }
    return 0;
}

// start position in buffer
// number of samples to display

uint32_t BuildGraphTick;
void buildGraph1ch()
{
    uint32_t t0 = DWT_Get_Current_Tick();
    int    i, j;
    float  x;

    u8     *samples = samplesBuffer.one;
    if( half!=0 ) samples += SAMPLES_1_BUFFER_SIZE/2;

//    scaleX = 0.5; // (float)320 / (float)(SAMPLES_2_BUFFER_SIZE/2);

    x=0; j=-1;
    i = triggerStart1ch(samples);
    for( ; i<SAMPLES_1_BUFFER_SIZE/2; i++ )
    {
        if( (int)x!=j )
        {
            j = (int)x;
            if( j>=MAX_X ) break;
            graph[j] = samples[i];
        } else
        {
            graph[j] = (graph[j]+samples[i]) >>1; // arithmetical mean
        }
        x += scaleX;
    }
    BuildGraphTick = DWT_Elapsed_Tick(t0);
}

void buildGraph2ch()
{
    uint32_t t0 = DWT_Get_Current_Tick();
    int    i, j;
    float  x;

    u8     (*samples)[2] = samplesBuffer.two;
    if( half!=0 ) samples += SAMPLES_2_BUFFER_SIZE/2;

//    scaleX = 0.5 ; // (float)320 / (float)(SAMPLES_2_BUFFER_SIZE/2);

    x=0; j=-1;
    i = triggerStart2ch(samples);
    for( ; i<SAMPLES_2_BUFFER_SIZE/2; i++ )
    {
        if( (int)x!=j )
        {
            j = (int)x;
            if( j>=MAX_X ) break;
            graph[j] = samples[i][1];
        } else
        {
            graph[j] = (graph[j]+samples[i][1]) >>1; // arithmetical mean
        }
        x += scaleX;
    }
    BuildGraphTick = DWT_Elapsed_Tick(t0);
}

uint32_t DrawGraphTick;
void drawGraph()
{
  u8 prev;

  buildGraph1ch();
  uint32_t t0 = DWT_Get_Current_Tick();

  POINT_COLOR = CYAN;
  prev = graph[0];
  for(u16 i=1; i<MAX_X; i++)
  {
//    LCD_Fast_DrawPoint(i, samplesBuffer.two[i][1], CYAN);
//    LCD_Fast_DrawPoint(i, graph[i], CYAN);
    LCD_DrawLine(i-(u16)1, prev, i, graph[i]);
    prev = graph[i];
  }
  DrawGraphTick = DWT_Elapsed_Tick(t0);
//  LCD_ShowxNum(150,227, DrawGraphTick/168,  10,12, 9);
//  LCD_ShowxNum(190,227, BuildGraphTick/168, 10,12, 9);
}

/*void setXScale(s16 step)
{
  scaleX += 0.05*step;
  if( scaleX>1 )   scaleX = 1;
  if( scaleX<0.5 ) scaleX = 0.5;
} //*/
