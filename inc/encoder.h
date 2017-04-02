#ifndef _ENCODER_H
#define _ENCODER_H

#include "stm32f4xx_conf.h"


#define ENCODER_TIMER 8

#if   ENCODER_TIMER == 2
#define ENCODER_TIM        TIM2
#define ENCODER_TIM_AHB    1
#define ENCODER_CLOCK      RCC_APB1Periph_TIM2
#define ENCODER_GPIO_CLOCK RCC_AHB1Periph_GPIOA
#define ENCODER_GPIO       GPIOA
#define ENCODER_PIN_A      GPIO_Pin_0
#define ENCODER_PIN_B      GPIO_Pin_1
#define ENCODER_GPIO_AF    GPIO_AF_TIM2
#define ENCODER_PIN_SRC_A  GPIO_PinSource0
#define ENCODER_PIN_SRC_B  GPIO_PinSource1
#elif ENCODER_TIMER == 8
#define ENCODER_TIM        TIM8
#define ENCODER_TIM_AHB    2
#define ENCODER_CLOCK      RCC_APB2Periph_TIM8
#define ENCODER_GPIO_CLOCK RCC_AHB1Periph_GPIOC
#define ENCODER_GPIO       GPIOC
#define ENCODER_PIN_A      GPIO_Pin_6
#define ENCODER_PIN_B      GPIO_Pin_7
#define ENCODER_GPIO_AF    GPIO_AF_TIM8
#define ENCODER_PIN_SRC_A  GPIO_PinSource6
#define ENCODER_PIN_SRC_B  GPIO_PinSource7
#endif

#ifdef __cplusplus
extern "C" {
#endif

void Encoder_init();
s16  Encoder_get();

#ifdef __cplusplus
}
#endif


#endif //_ENCODER_H
