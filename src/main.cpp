#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

#include "delay.h"
#include "SSD1289.h"
//#include "touch_7846.h"
//#include "Julija.h"

void init();
void TIM4_Config(void);

extern unsigned char flag;
char stringas[8];
int xold,yold;


int main(void)
{
  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used.
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates
  *  SCB->VTOR register.
  *  E.g.  SCB->VTOR = 0x20000000;
  */
//  RCC_ClocksTypeDef RCC_Clocks;
//  RCC_GetClocksFreq(&RCC_Clocks);

  init();
  TIM4_Config();

  Delay(0x300);
  LCD_Init();
  Delay(0x300);
//  touch_init();
//  LCD_Clear(BLACK);
  //LCD_SetTextColor(BLUE);

  while(1)
  {
/*    Convert_Pos();
    Pixel(Pen_Point.X0,Pen_Point.Y0,WHITE);
    Pixel(Pen_Point.X0,Pen_Point.Y0+1,WHITE);
    Pixel(Pen_Point.X0+1,Pen_Point.Y0,WHITE);
    Pixel(Pen_Point.X0+1,Pen_Point.Y0+1,WHITE);//*/
  }
}

void TIM4_Config(void)
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
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
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
       Prescaler = ((SystemCoreClock /2) /10 KHz) - 1 // 8399 must be < 65000

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
  TIM_TimeBaseStructure.TIM_Period = 9999;
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


void init() {
  /*/------------------Инициализация TIM4------------------
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
  //По умолчанию частота шины 24 МГц при использовании кварца 8 МГц
  TIM4->PSC = 24000 - 1;         //Настройка делителя на 1000 "тиков" в секунду
  TIM4->ARR = 300;               //Отработка прерывания раз в секунду
  TIM4->DIER |= TIM_DIER_UIE;    //Разрешения прерывание от таймера
  TIM4->CR1 |= TIM_CR1_CEN;      //Запуск таймера
  NVIC_EnableIRQ(TIM4_IRQn);     //Разрешение TIM4_DAC_IRQn прерывания
  //------------------------------------------------------ */

  //------------------Инициализация TIM7------------------
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
  //По умолчанию частота шины 24 МГц при использовании кварца 8 МГц
  TIM7->PSC = 24000 - 1;      //Настройка делителя на 1000 "тиков" в секунду
  TIM7->ARR = 500;            //Отработка прерывания раз в секунду
  TIM7->DIER |= TIM_DIER_UIE; //Разрешения прерывание от таймера
  TIM7->CR1 |= TIM_CR1_CEN;   //Запуск таймера
  NVIC_EnableIRQ(TIM7_IRQn);  //Разрешение TIM6_DAC_IRQn прерывания
  //------------------------------------------------------

  //------------------Инициализация портов светодиодов------------------
  GPIO_InitTypeDef GPIO_InitStructure;                     //Структура содержащая настройки порта
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);    //Включаем тактирование порта D
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;               //Выбираем нужные выводы | GPIO_Pin_14| GPIO_Pin_15
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //Включаем режим выхода
  GPIO_Init(GPIOD, &GPIO_InitStructure);                   //вызов функции инициализации
  //--------------------------------------------------------------------
}
