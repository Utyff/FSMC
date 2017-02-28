#ifndef __DWT_H
#define __DWT_H

#include "stm32f4xx.h"

#ifdef __cplusplus
 extern "C" {
#endif

void DWT_Init();
void DWT_Delay_ms(uint32_t ms);   // milliseconds
void DWT_Delay(uint32_t us);      // microseconds
void DWT_Delay_With_Action(uint32_t us, int (*cond)(), void (*act)()); // microseconds
uint32_t DWT_Get_Current_Tick();
uint32_t DWT_Elapsed_Tick(uint32_t t0);

#ifdef __cplusplus
}
#endif

#endif /* __DWT_H */
