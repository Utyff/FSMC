#include <menu.h>
#include <lcd.h>

const char menu[5][8] = {"ch 1","trig","swipe","gain", "aa555"};

u16 menu1 = 0;
u16 menu2 = 0;

void menu1Next()
{
  menu1++;
  if( menu1>MENU1_MAX ) menu1 = 0;
}

void menu1Prev()
{
  if( menu1 == 0) menu1 = MENU1_MAX;
  else menu1--;
}

void menu1Step(s16 step)
{
  menu1 += step%MENU1_MAX;
  if( (s16)menu1<0 ) menu1 += MENU1_MAX;
  if( menu1>=MENU1_MAX ) menu1 -= MENU1_MAX;
}

void drawMenu1()
{
  POINT_COLOR = BLUE;  // Drawing pen color

  for( u16 i=0; i<4; i++ )
  {
    if( i==menu1 ) BACK_COLOR=YELLOW;
    else BACK_COLOR = GRAY;
    LCD_ShowString(i*(u16)50, 0, 40,14, 16, menu[i], 0);
  }
}
