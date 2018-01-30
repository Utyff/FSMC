#ifndef _DAC_H
#define _DAC_H

#include "stm32f4xx_conf.h"
#include "pins.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Sin = 0x00,
    Tri = 0x01,
    Sqe = 0x02
} DAC_SIGNAL_FORM;

void DAC_init();
void DAC_Ch2_Config(const uint16_t *table, uint32_t tableSize);
void DAC_Ch1_EscalatorConfig();
void DAC_Ch1_NoiseConfig();
void DAC_Ch1_TriangleConfig();
void DAC_step(s16 step);
void DAC_SetGeneratorSignal(DAC_SIGNAL_FORM form);
void DAC_NextGeneratorSignal();

extern DAC_SIGNAL_FORM DAC_SignalForm;

#ifdef __cplusplus
}
#endif

#endif //_DAC_H
