#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"


void init();

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

  init();

  while (1)
  {}
}

void init() {
  //------------------Инициализация TIM6------------------
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
  //По умолчанию частота шины 24 МГц при использовании кварца 8 МГц
  TIM6->PSC = 24000 - 1;         //Настройка делителя на 1000 "тиков" в секунду
  TIM6->ARR = 300;               //Отработка прерывания раз в секунду
  TIM6->DIER |= TIM_DIER_UIE;    //Разрешения прерывание от таймера
  TIM6->CR1 |= TIM_CR1_CEN;      //Запуск таймера
  NVIC_EnableIRQ(TIM6_DAC_IRQn); //Разрешение TIM6_DAC_IRQn прерывания
  //------------------------------------------------------

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
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13; //Выбираем нужные выводы | GPIO_Pin_14| GPIO_Pin_15
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //Включаем режим выхода
  GPIO_Init(GPIOD, &GPIO_InitStructure);                   //вызов функции инициализации
  //--------------------------------------------------------------------
}
