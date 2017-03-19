#include <menu.h>
#include <lcd.h>

const char menu[5][8] = {"mm11","mm222","mm33","mm44"};

u8 menu1 = 0;
u8 menu2 = 0;

void menu1Next()
{
  menu1++;
  if( menu1>MENU1_MAX ) menu1 = 0;
}

void drawMenu1()
{
  POINT_COLOR = BLUE;  // Drawing pen color

  for( u16 i=0; i<4; i++ )
  {
	if( i==menu1 ) BACK_COLOR=YELLOW;
	else BACK_COLOR = GRAY;
    LCD_ShowString(i*50, 0, 40,14, 16, menu[i], 0);
  }
}
