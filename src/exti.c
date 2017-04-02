#include <dwt.h>
#include <exti.h>


u8  button0Count = 0;
u8  button1Count = 0;
u8  button2Count = 0;


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

  // Configure PA0 pin as input floating. DISCO button 1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6;
//  GPIO_Init(GPIOE, &GPIO_InitStructure);

  // Connect EXTI Line0 to PA0 pin
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
//  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
//  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource5);
//  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource6);

  // Configure EXTI Line0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_Init(&EXTI_InitStructure);

  // Configure EXTI Line1
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_Init(&EXTI_InitStructure);

  // Configure EXTI Line2
  EXTI_InitStructure.EXTI_Line = EXTI_Line2;
//  EXTI_Init(&EXTI_InitStructure);

  // Configure EXTI Line5
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
//  EXTI_Init(&EXTI_InitStructure);
  // Configure EXTI Line6
  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
//  EXTI_Init(&EXTI_InitStructure);

  // Enable and set EXTI Line0 Interrupt to the lowest priority
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_Init(&NVIC_InitStructure);

  // Enable and set EXTI Line1 Interrupt to the lowest priority
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
  NVIC_Init(&NVIC_InitStructure);

  // Enable and set EXTI Line2 Interrupt to the lowest priority
  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
//  NVIC_Init(&NVIC_InitStructure);

  // Enable and set EXTI Line9-5 Interrupt to the lowest priority
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//  NVIC_Init(&NVIC_InitStructure);
}
