#include <dwt.h>
#include <encoder.h>

#define MAX_ENCODER   63  // max encoder value
#define MID_ENCODER   MAX_ENCODER/2
#define ENCODER_STEP  2   // counts per step

//static u16 lastEncoder = MAX_ENCODER / 2;


void Encoder_init()
{
  GPIO_InitTypeDef         GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef        TIM_ICInitStructure;

  // TIM2 clock source enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  // Enable GPIOA, clock
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // Encoder unit connected to TIM2, quadrature mode
  // GPIOA Pins 0 and 1 Config
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);     // set alternate function TIM2 for PA0 and PA1
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);

  // Timer configuration in Encoder mode for left encoder
  TIM_TimeBaseStructure.TIM_Prescaler = 0x00;                 // No prescaling
  TIM_TimeBaseStructure.TIM_Period = MAX_ENCODER;             // counter limit
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // divide by clock by one
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // count up
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 6;
  TIM_ICInit(TIM2, &TIM_ICInitStructure);

  TIM_Cmd(TIM2, ENABLE);   // enable encoder control
  //Reset counter
  DWT_Delay(100);          // pause 100 microseconds for first count
  TIM2->CNT = MID_ENCODER; // set initial value
}


s16 Encoder_get()
{
  s16 result=0;

  s16 step = (s16)(TIM2->CNT-MID_ENCODER);
  if( step >= ENCODER_STEP || step <= -ENCODER_STEP )
  {
    result =  step / (s16)ENCODER_STEP;

    __disable_irq();
    TIM2->CNT -= result*ENCODER_STEP;
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