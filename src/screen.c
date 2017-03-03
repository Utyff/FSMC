#include <screen.h>


void drawFrame()
{
	u16 x,y, step=40;

	LCD_Clear(BLACK);
	POINT_COLOR=GRAY;	// Drawing pen color
	BACK_COLOR=BLACK;

	for( y=step; y<MAX_Y; y+=step )
		LCD_DrawLine(0,y,MAX_X,y);

	for( x=step; x<MAX_X; x+=step )
		LCD_DrawLine(x,0,x,MAX_Y);
}
