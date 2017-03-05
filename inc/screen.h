
#include "lcd.h"


#define MAX_X 320
#define MAX_Y 240

extern __IO uint16_t SamplesBuffer[];
extern uint16_t graph[];

#ifdef __cplusplus
 extern "C" {
#endif

void drawFrame();
void drawGraph();


#ifdef __cplusplus
 }
#endif
