#include <dwt.h>
#include <encoder.h>

#define MAX_ENCODER   63  // max encoder value
#define MID_ENCODER   MAX_ENCODER/2
#define ENCODER_STEP  2   // counts per step

//static u16 lastEncoder = MAX_ENCODER / 2;


void Encoder_init()
{
//  NVIC_InitTypeDef         NVIC_InitStructure;
  GPIO_InitTypeDef         GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef        TIM_ICInitStructure;

  // TIM2 clock source enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  // Enable GPIOA, clock
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // Encoder unit connected to TIM2, quadrature mode
  // GPIOA Config
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
  TIM_TimeBaseStructure.TIM_Period = MAX_ENCODER;             // max count
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // divide by clock by one
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // count up
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 6;    // ICx_FILTER
  TIM_ICInit(TIM2, &TIM_ICInitStructure);

  TIM_Cmd(TIM2, ENABLE); // enable encoder control
  //Reset counter
  DWT_Delay(100);
  TIM2->CNT = MID_ENCODER;         // set initial value

  /*/ Enable the TIM2 Update Interrupt for left encoder
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Clear all pending interrupts
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //*/
}


s16 Encoder_get()
{
  s16 result=0;

  s32 step = TIM2->CNT-MID_ENCODER;   // TODO fix sign
  if( step > ENCODER_STEP || step < -ENCODER_STEP )
  {
    result = (s16) (step / ENCODER_STEP);

    __disable_irq();
    TIM2->CNT -= result*ENCODER_STEP;
    __enable_irq();
  }
  return result;
}
