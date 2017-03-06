#ifndef __SCREEN_H
#define __SCREEN_H

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


#ifdef __cplusplus
 }
#endif

#endif /* __SCREEN_H */
