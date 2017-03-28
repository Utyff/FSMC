#ifndef _ENCODER_H
#define _ENCODER_H

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C" {
#endif

void Encoder_init();
s16  Encoder_get();

#ifdef __cplusplus
}
#endif


#endif //_ENCODER_H
