#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#include "delay.h"
#include "SSD1289.h"

void init_TIM4();
void init_TIM7();
void init_LED();


int main(void)
{
//  RCC_ClocksTypeDef RCC_Clocks;
//  RCC_GetClocksFreq(&RCC_Clocks);

  DWT_Init();
//  init_TIM4(); // Orange LED timer PWM-blink
//  init_TIM7(); // Green LED interrupt blink
  init_LED();

  Delay(0x300);
  LCD_Init();
  Delay(0x300);

  while(1)
  {
    DWT_Delay(250000); // 250ms / 4 times per second
    GPIO_ToggleBits(GPIOD, GPIO_Pin_12); // Green LED toggle
  }
}

void init_TIM4()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOD clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Connect TIM4 pins to AF */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);

  /* GPIOC Configuration: TIM4 CH2 (PD13) */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN; //NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* -----------------------------------------------------------------------
    TIM4 Configuration: generate 4 PWM signals with 4 different duty cycles.

    In this example TIM4 input clock (TIM4CLK) is set to 2 * APB1 clock (PCLK1),
    since APB1 prescaler is different from 1.
      TIM4CLK = 2 * PCLK1
      PCLK1 = HCLK / 4
      => TIM4CLK = HCLK / 2 = SystemCoreClock /2

    To get TIM4 counter clock at 10 KHz, the prescaler is computed as follows:
       Prescaler = (TIM4CLK / TIM4 counter clock) - 1
       Prescaler = ((SystemCoreClock /2) /10 KHz) - 1 // 8399 (must be less then 0xFFFF)

    To get TIM4 output clock at 1 Hz, the period (ARR)) is computed as follows:
       ARR = (TIM4 counter clock / TIM4 output clock) - 1
           = 9999

    TIM4 Channel2 duty cycle = (TIM4_CCR2/ TIM4_ARR)* 100 = 37.5%

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.
  ----------------------------------------------------------------------- */

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  uint16_t CCR2_Val = 1333;
  uint16_t PrescalerValue;

  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 10000) - 1; // 8399

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 9999; // TODO why 10 000? Must be 100 000
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  /* TIM4 enable counter */
  TIM_Cmd(TIM4, ENABLE);

  /* PWM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

  TIM_OC2Init(TIM4, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM4, ENABLE);
}


void init_TIM7()
{
  //------------------������������� TIM7------------------
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
  //�� ��������� ������� ���� 24 ��� ��� ������������� ������ 8 ��� ????
  TIM7->PSC = 24000 - 1;      //��������� �������� �� 1000 "�����" � �������
  TIM7->ARR = 500;            //��������� ���������� ��� � �������
  TIM7->DIER |= TIM_DIER_UIE; //���������� ���������� �� �������
  TIM7->CR1 |= TIM_CR1_CEN;   //������ �������
  NVIC_EnableIRQ(TIM7_IRQn);  //���������� TIM7_IRQn ����������
  //------------------------------------------------------
}

void init_LED() // init GPIO PD12 for Green LED
{
  //------------------������������� ������ �����������------------------
  GPIO_InitTypeDef GPIO_InitStructure;                     //��������� ���������� ��������� �����
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);    //�������� ������������ ����� D
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;               //�������� ������ ������ | GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //�������� ����� ������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN; //NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);                   //����� ������� �������������
  //--------------------------------------------------------------------
}
