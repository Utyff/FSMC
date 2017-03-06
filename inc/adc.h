#ifndef __ADC_H
#define __ADC_H

#include "stm32f4xx.h"

#define SAMPLES_4_BUFFER_SIZE 1024
#define SAMPLES_2_BUFFER_SIZE 2048
#define SAMPLES_1_BUFFER_SIZE 4096

typedef volatile union  {
    u8 four[SAMPLES_4_BUFFER_SIZE][4];
    u8 two [SAMPLES_2_BUFFER_SIZE][2];
    u8 one [SAMPLES_1_BUFFER_SIZE];
} SampleBuffer;
extern SampleBuffer samplesBuffer;

#ifdef __cplusplus
extern "C" {
#endif

void ADC_init();

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H */
