/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Template/stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    18-January-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include <exti.h>
#include <dwt.h>
#include <menu.h>
#include <stm32f4xx.h>

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
/*  TimingDelay_Decrement(); */
}

/*int OraStat=0;
// Обработчик прерывания TIM4_DAC
void TIM4_IRQHandler(void)
{
  if( OraStat )
       GPIO_SetBits(GPIOD, GPIO_Pin_13);
  else
       GPIO_ResetBits(GPIOD, GPIO_Pin_13);
  OraStat = !OraStat;
  TIM4->SR &= ~TIM_SR_UIF; //Сбрасываем флаг прерывания
} //*/


/*/ Обработчик прерывания TIM2
void TIM2_IRQHandler()
{
  encoder = TIM2->CNT;
  TIM2->SR &= ~TIM_SR_UIF; //Сбрасываем флаг прерывания
} //*/

// Обработчик прерывания TIM7
void TIM7_IRQHandler()
{
//  GPIO_ToggleBits(GPIOD, GPIO_Pin_12); // Green toggle
  TIM7->SR &= ~TIM_SR_UIF; //Сбрасываем флаг прерывания
}

#define BOUNCING_TIME  200*(168000000/1000)  // 100 milliseconds in DWT ticks

// This function handles External line 0 interrupt request.
void EXTI0_IRQHandler()
{
  static u32 t0 = 0;

  if (EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    if ( DWT_GetDelta(t0)>BOUNCING_TIME )
    {
      t0 = DWT_Get_Current_Tick();
//      if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) != 0 )
      if( (GPIOA->IDR & GPIO_Pin_0) != 0 )
      {
        button0Count++;   // only on rising trigger (button down)
//        menu1Next();
      }
    }
    // Clear the EXTI line 0 pending bit
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

// This function handles External line 1 interrupt request.
void EXTI1_IRQHandler()
{
  static u32 t0 = 0;

  if (EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
    if ( DWT_GetDelta(t0)>BOUNCING_TIME )
    {
      t0 = DWT_Get_Current_Tick();
//      if( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) != 0 )
      if( (GPIOB->IDR & GPIO_Pin_1) == 0 )
        button1Count++;   // only on rising trigger (button down)
    }
    // Clear the EXTI line 1 pending bit
    EXTI_ClearITPendingBit(EXTI_Line1);
  }
}

// This function handles External line 2 interrupt request.
void EXTI2_IRQHandler()
{
  static u32 t0 = 0;

  if (EXTI_GetITStatus(EXTI_Line2) != RESET)
  {
//    if ( DWT_GetDelta(t0)>BOUNCING_TIME )
    {
//      t0 = DWT_Get_Current_Tick();
//      if( GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) != 0 )
      if( (GPIOE->IDR & GPIO_Pin_2) != 0 )
        button2Count++;   // only on rising trigger (button down)
    }
    // Clear the EXTI line 0 pending bit
    EXTI_ClearITPendingBit(EXTI_Line2);
  }
}

// This function handles External line 2 interrupt request.
void EXTI9_5_IRQHandler()
{
  static u32 t0 = 0;

  if (EXTI_GetITStatus(EXTI_Line5) != RESET)
  {
//    if ( DWT_GetDelta(t0)>BOUNCING_TIME )
    {
//      t0 = DWT_Get_Current_Tick();
      if( (GPIOE->IDR & GPIO_Pin_5) != 0 )
        button1Count++;   // only on rising trigger (button down)
    }
    EXTI_ClearITPendingBit(EXTI_Line5);
  }

  if (EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
//    if ( DWT_GetDelta(t0)>BOUNCING_TIME )
    {
//      t0 = DWT_Get_Current_Tick();
      if( (GPIOE->IDR & GPIO_Pin_6) != 0 )
        button2Count++;   // only on rising trigger (button down)
    }
    EXTI_ClearITPendingBit(EXTI_Line6);
  }

  if (EXTI_GetITStatus(EXTI_Line7) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line7);
  }

  if (EXTI_GetITStatus(EXTI_Line8) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line8);
  }

  if (EXTI_GetITStatus(EXTI_Line9) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line9);
  }
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
