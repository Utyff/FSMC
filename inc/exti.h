#ifndef _EXTI_H
#define _EXTI_H

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"

extern u8  button0Count;
extern u8  button1Count;
extern u8  button2Count;
extern u32 encoder;

#ifdef __cplusplus
extern "C" {
#endif

void EXTI_init();
void Encoder_init();

#ifdef __cplusplus
}
#endif

#endif //_EXTI_H
