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
  //------------------������������� TIM6------------------
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
  //�� ��������� ������� ���� 24 ��� ��� ������������� ������ 8 ���
  TIM6->PSC = 24000 - 1;         //��������� �������� �� 1000 "�����" � �������
  TIM6->ARR = 300;               //��������� ���������� ��� � �������
  TIM6->DIER |= TIM_DIER_UIE;    //���������� ���������� �� �������
  TIM6->CR1 |= TIM_CR1_CEN;      //������ �������
  NVIC_EnableIRQ(TIM6_DAC_IRQn); //���������� TIM6_DAC_IRQn ����������
  //------------------------------------------------------

  //------------------������������� TIM7------------------
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
  //�� ��������� ������� ���� 24 ��� ��� ������������� ������ 8 ���
  TIM7->PSC = 24000 - 1;      //��������� �������� �� 1000 "�����" � �������
  TIM7->ARR = 500;            //��������� ���������� ��� � �������
  TIM7->DIER |= TIM_DIER_UIE; //���������� ���������� �� �������
  TIM7->CR1 |= TIM_CR1_CEN;   //������ �������
  NVIC_EnableIRQ(TIM7_IRQn);  //���������� TIM6_DAC_IRQn ����������
  //------------------------------------------------------

  //------------------������������� ������ �����������------------------
  GPIO_InitTypeDef GPIO_InitStructure;                     //��������� ���������� ��������� �����
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);    //�������� ������������ ����� D
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13; //�������� ������ ������ | GPIO_Pin_14| GPIO_Pin_15
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            //�������� ����� ������
  GPIO_Init(GPIOD, &GPIO_InitStructure);                   //����� ������� �������������
  //--------------------------------------------------------------------
}
