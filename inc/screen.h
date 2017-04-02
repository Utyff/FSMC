#ifndef __SCREEN_H
#define __SCREEN_H

#include "stm32f4xx_conf.h"
#include "lcd.h"
#include "adc.h"


#define MAX_X 320
#define MAX_Y 240

extern uint8_t graph[];

#ifdef __cplusplus
 extern "C" {
#endif

void drawFrame();
void drawGraph();
void setXScale(s16 step);

#ifdef __cplusplus
 }
#endif

#endif /* __SCREEN_H */
