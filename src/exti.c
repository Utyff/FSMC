#include <dwt.h>
#include <exti.h>


u8  button0Count = 0;
u8  button1Count = 0;
u8  button2Count = 0;
u32 encoder = 0;

void EXTI_init()
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  EXTI_InitTypeDef   EXTI_InitStructure;

  // Enable GPIOA clock
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  // Enable SYSCFG clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  // Configure PA0 pin as input floating
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


void Encoder_init()
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
  TIM_TimeBaseStructure.TIM_Period = 0x3f;                    // max count: 64 - 1
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // divide by clock by one
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // count up
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 6;    // ICx_FILTER
  TIM_ICInit(TIM2, &TIM_ICInitStructure);

  // Enable the TIM2 Update Interrupt for left encoder
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); //*/

  // Clear all pending interrupts
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

  TIM_Cmd(TIM2, ENABLE); // enable encoder control
  //Reset counter
  DWT_Delay(100);
  TIM2->CNT = 0;         // set initial value
}
