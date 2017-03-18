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
  for( u16 i=0; i<4; i++ )
  {
    LCD_ShowString(i*40, 0, 40,14, 12, menu[i]);
  }
}