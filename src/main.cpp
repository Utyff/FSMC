#include <stm32f4xx.h>
#include <stm32f4xx_conf.h>
#include <stm32f4xx.h>
#include <delay.h>
#include <adc.h>
#include <dac.h>
#include <lcd.h>
#include <screen.h>
#include <exti.h>
#include <encoder.h>
#include <menu.h>


// for f4-disco GPIOD & GPIO_Pin_12  Green LED
// for ve-board GPIOA & GPIO_Pin_6
#define LED_CLOCKPORT RCC_AHB1Periph_GPIOD
#define LED_PORT      GPIOD
#define LED_PIN       GPIO_Pin_12

void TIM3_init(); // Configure TIM3 as square generator
void TIM4_init();
void TIM7_init();
void LED_init();

extern "C" {
  void AD9833_init();
}


void main()
{
//  RCC_ClocksTypeDef RCC_Clocks;
//  RCC_GetClocksFreq(&RCC_Clocks);

  DWT_Init();
  TIM3_init(); // Square generator
//  TIM4_init(); // Orange LED timer PWM-blink
//  TIM7_init(); // Green LED interrupt blink
//  AD9833_init();
  LED_init();
  LCD_Init();
  ADC_init();
  DAC_init();
//  EXTI_init();
  Encoder_init();

  while(1)
  {
    DWT_Delay(50000); // 50ms / 20 times per second
    GPIO_ToggleBits(LED_PORT, LED_PIN); // Green LED toggle

    drawFrame();
    drawGraph();

    POINT_COLOR = MAGENTA;
    BACK_COLOR  = BLUE;
    LCD_ShowxNum(0,   227, button0Count, 5,12, 9);
    LCD_ShowxNum(30,  227, button1Count, 5,12, 9);
    LCD_ShowxNum(60,  227, button2Count, 5,12, 9);
    LCD_ShowxNum(260, 227, TIM2->CNT,    5,12, 9);

    menu1Step(Encoder_get());
    drawMenu1();
  }
}

// Configure TIM3 as square generator
void TIM3_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // GPIOA clock enable
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  // Connect TIM3 pins to AF
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);

  // GPIOA Configuration: TIM3 CH1 (PC6)
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef        TIM_OCInitStructure;

  // TIM3 clock enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  // Time base configuration
  TIM_TimeBaseStructure.TIM_Period = 99; // 1000KHz input freq;  100 > 10KHz
  TIM_TimeBaseStructure.TIM_Prescaler = 83; //  TIM3CLK = APB1*2 = 84MHz;  84 > 1000KHz
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  // TIM3 enable counter
  TIM_Cmd(TIM3, ENABLE);

  // PWM1 Mode configuration: Channel1
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 60;  // 60/100

  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM3, ENABLE);
}


void TIM4_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOD clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Connect TIM4 pins to AF */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);

  /* GPIOD Configuration: TIM4 CH2 (PD13) */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
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

  // TIM4 clock enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  // Compute the prescaler value, TIM4CLK = APB1*2 = 84MHz
  PrescalerValue = (uint16_t) (((SystemCoreClock /2) / 10000) - 1); // 8399;  84M > 1Hz | 8,4M > 10Hz | 840K > 100Hz .. 8400 > 10KHz

  // Time base configuration
  TIM_TimeBaseStructure.TIM_Period = 9999; // 10KHz input freq;  10K > 1Hz
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  // TIM4 enable counter
  TIM_Cmd(TIM4, ENABLE);

  // PWM1 Mode configuration: Channel2
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

  TIM_OC2Init(TIM4, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM4, ENABLE);
}


void TIM7_init()
{
  // ------------------������������� TIM7------------------
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
  // �� ��������� ������� ���� 82 ���
  TIM7->PSC = 24000 - 1;      // ��������� �������� �� 4000 "�����" � �������
  TIM7->ARR = 500;            // ��������� ���������� 8 ��� � �������
  TIM7->DIER |= TIM_DIER_UIE; // ���������� ���������� �� �������
  TIM7->CR1 |= TIM_CR1_CEN;   // ������ �������
  NVIC_EnableIRQ(TIM7_IRQn);  // ���������� TIM7_IRQn ����������
  // ------------------------------------------------------
}


void LED_init() // init GPIO PD12 for Green LED
{
  // ------------------������������� ������ �����������------------------
  GPIO_InitTypeDef GPIO_InitStructure;                // ��������� ���������� ��������� �����
  RCC_AHB1PeriphClockCmd(LED_CLOCKPORT, ENABLE);      // �������� ������������ ����� D
  GPIO_InitStructure.GPIO_Pin = LED_PIN;              // �������� ������ ������ | GPIO_Pin_13 | GPIO_Pin_14| GPIO_Pin_15
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       // �������� ����� ������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_Init(LED_PORT, &GPIO_InitStructure);           // ����� ������� �������������
  // --------------------------------------------------------------------
}
