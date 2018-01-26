#include <dwt.h>
#include <encoder.h>


#define MAX_ENCODER   63  // max encoder value
#define MID_ENCODER   32
#define ENCODER_STEP  1   // counts per step


void Encoder_init() {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;

    // TIM2 clock source enable
#if   ENCODER_TIM_AHB == 1
    RCC_APB1PeriphClockCmd(ENCODER_CLOCK, ENABLE);
#elif ENCODER_TIM_AHB == 2
    RCC_APB2PeriphClockCmd(ENCODER_CLOCK, ENABLE);
#endif

    // Enable GPIOx clock
    RCC_AHB1PeriphClockCmd(ENCODER_GPIO_CLOCK, ENABLE);

    // Encoder unit connected to TIM2, quadrature mode
    // GPIOx Pins 0 and 1 Config
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = ENCODER_PIN_A | ENCODER_PIN_B;
    GPIO_Init(ENCODER_GPIO, &GPIO_InitStructure);

    GPIO_PinAFConfig(ENCODER_GPIO, ENCODER_PIN_SRC_A, ENCODER_GPIO_AF);  // set alternate function TIM2 for PA0 and PA1
    GPIO_PinAFConfig(ENCODER_GPIO, ENCODER_PIN_SRC_B, ENCODER_GPIO_AF);

    // Timer configuration in Encoder mode for left encoder
    TIM_TimeBaseStructure.TIM_Prescaler = 0x00;               // No prescaling
    TIM_TimeBaseStructure.TIM_Period = MAX_ENCODER;        // counter limit
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       // divide by clock by one
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // count up
    TIM_TimeBaseInit(ENCODER_TIM, &TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig(ENCODER_TIM, TIM_EncoderMode_TI1, TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge);

    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 6;
    TIM_ICInit(ENCODER_TIM, &TIM_ICInitStructure);

    TIM_Cmd(ENCODER_TIM, ENABLE);     // enable encoder control
    // Reset counter
    DWT_Delay(100);                   // pause 100 microseconds for first count
    ENCODER_TIM->CNT = MID_ENCODER + 1; // set initial value
}


s16 Encoder_get() {
    s16 result = 0;

    s16 step = (s16) (ENCODER_TIM->CNT - MID_ENCODER);
    if (step >= ENCODER_STEP || step <= -ENCODER_STEP) {
        result = step / (s16) ENCODER_STEP;

        __disable_irq();
        ENCODER_TIM->CNT -= result * ENCODER_STEP;
        __enable_irq();
    }

    return result;
}

/*
#include <stdio.h>

#define ENCODER_STEP 2
#define MID 32
#define MENU1_MAX 5

int main()
{
  unsigned int CNT = 20; //,  MID = 32;
  short step, result;
  unsigned short menu1 = 2;

  step = (short)(CNT - MID);
  result = (short) (step / ENCODER_STEP);
  CNT -= result*ENCODER_STEP;
  printf("step   %d | %u || %hd | %hu  \n", step, step, step, step);
  printf("result %d | %u || %hd | %hu  \n", result, result, result, result);
  printf("CNT    %d | %u || %hd | %hu  \n", CNT, CNT, CNT, CNT);

  if( step >= ENCODER_STEP || step <= -ENCODER_STEP )
  {
    printf(" -- step bigger\n");
  } else
  {
    printf(" -- step NOT bigger\n");
  }

  step = -9;
  menu1 = 2;

  menu1 +=  step%MENU1_MAX;
  printf("menu1  %d | %u || %hd | %hu  \n", menu1, menu1, menu1, menu1);

  if( (short)menu1<0 ) menu1 += MENU1_MAX;
  if( menu1>=MENU1_MAX ) menu1 -= MENU1_MAX;

  printf("menu1  %d | %u || %hhd | %hhu  \n", menu1, menu1, menu1, menu1);

  return 0;
}
*/
