#include "exti.h"


u8 button0Count = 0;
u8 button1Count = 0;
u8 button2Count = 0;

void EXTI_init()
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;

  /* Enable GPIOA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  // Connect EXTI Line0 to PA0 pin
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
//  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
//  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource5);
//  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource6);

  // Configure EXTI Line0
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  // Configure EXTI Line1
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_Init(&EXTI_InitStructure);

  // Configure EXTI Line2
  EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_Init(&EXTI_InitStructure);

  // Configure EXTI Line5
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  EXTI_Init(&EXTI_InitStructure);
  // Configure EXTI Line6
  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
  EXTI_Init(&EXTI_InitStructure);

  // Enable and set EXTI Line0 Interrupt to the lowest priority
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Enable and set EXTI Line1 Interrupt to the lowest priority
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_Init(&NVIC_InitStructure);

  // Enable and set EXTI Line2 Interrupt to the lowest priority
  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
  NVIC_Init(&NVIC_InitStructure);

  // Enable and set EXTI Line9-5 Interrupt to the lowest priority
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_Init(&NVIC_InitStructure);
}

void encoder()
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = 0x0FFF;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_EncoderInterfaceConfig(TIM8,TIM_EncoderMode_TI1,TIM_ICPolarity_Falling,TIM_ICPolarity_Falling);
  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
  TIM_Cmd(TIM8, ENABLE);
}

void Encoder_Init()
{
  NVIC_InitTypeDef         NVIC_InitStructure;
  GPIO_InitTypeDef         GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef        TIM_ICInitStructure;

  // TIM2 clock source enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  // Enable GPIOA, clock
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // Encoder unit connected to TIM2, quadrature mode
  // GPIOA Config
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Enable the TIM2 Update Interrupt for left encoder
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Timer configuration in Encoder mode for left encoder
  TIM_TimeBaseStructure.TIM_Prescaler = 0x00;              // No prescaling
  TIM_TimeBaseStructure.TIM_Period = 0xffffffff;           // max resolution
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // divide by clock by one
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // count up
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 6;    // ICx_FILTER;
  TIM_ICInit(TIM2, &TIM_ICInitStructure);

  // Clear all pending interrupts
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

  //Reset counter
  TIM2->CNT = 1000;     // prevent exceeding 0 when turning wheel backwards
  TIM_Cmd(TIM2, ENABLE); // enable left encoder
}

void Encoder_Configration2()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_TIM2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_TIM2);

  TIM_SetAutoreload (TIM2, 0xffffffff);
  /* Configure the timer */

  TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);
  /* TIM2 counter enable */
  TIM_Cmd(TIM2, ENABLE);
}

/*
TIM_Encoder_InitTypeDef encoder;
TIM_HandleTypeDef timer;

int mainEN(void) {

  HAL_Init();
  SystemClock_Config();
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 1);

  timer.Instance = TIM3;
  timer.Init.Period = 0xFFFF;
  timer.Init.CounterMode = TIM_COUNTERMODE_UP;
  timer.Init.Prescaler = 0;
  timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

  encoder.EncoderMode = TIM_ENCODERMODE_TI12;

  encoder.IC1Filter = 0x0F;
  encoder.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
  encoder.IC1Prescaler = TIM_ICPSC_DIV4;
  encoder.IC1Selection = TIM_ICSELECTION_DIRECTTI;

  encoder.IC2Filter = 0x0F;
  encoder.IC2Polarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  encoder.IC2Prescaler = TIM_ICPSC_DIV4;
  encoder.IC2Selection = TIM_ICSELECTION_DIRECTTI;

  if (HAL_TIM_Encoder_Init(&amp;amp;timer, &amp;amp;encoder) != HAL_OK) {
    Error_Handler();
  }

  if(HAL_TIM_Encoder_Start_IT(&amp;amp;timer,TIM_CHANNEL_1)!=HAL_OK){
    Error_Handler();
  }

  while (1) {
    uint16_t count=TIM3-&amp;gt;CNT;
  }
}*/


