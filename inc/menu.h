#ifndef _MENU_H
#define _MENU_H

#include "stm32f4xx.h"

#define MENU1_MAX 4

extern const char menu[5][8];
extern u8 menu1;
extern u8 menu2;

#ifdef __cplusplus
extern "C" {
#endif

void menu1Next();
void menu1Prev();
void menu1Step(s16 step);
void drawMenu1();

#ifdef __cplusplus
 }
#endif

#endif //_MENU_H
