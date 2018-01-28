#ifndef _DAC_H
#define _DAC_H

#include "stm32f4xx_conf.h"
#include "pins.h"


#ifdef __cplusplus
extern "C" {
#endif

void DAC_init();
void DAC_Ch1_EscalatorConfig();
void DAC_Ch2_SineWaveConfig();
void DAC_Ch1_NoiseConfig();
void DAC_Ch2_TriangleConfig();
void DAC_step(s16 step);

#ifdef __cplusplus
}
#endif

#endif //_DAC_H
