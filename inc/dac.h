#ifndef _DAC_H
#define _DAC_H

#include "stm32f4xx.h"



#ifdef __cplusplus
extern "C" {
#endif

void DAC_init();
void DAC_Ch1_EscalatorConfig();
void DAC_Ch2_SineWaveConfig();
void DAC_Ch1_NoiseConfig();
void DAC_Ch2_TriangleConfig();

#ifdef __cplusplus
}
#endif

#endif //_DAC_H
