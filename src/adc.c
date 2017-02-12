#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"

#define ADC_CDR_ADDRESS    ((uint32_t)0x40012308)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t ADCTripleConvertedValue[2048];

ADC_InitTypeDef       ADC_InitStructure;
ADC_CommonInitTypeDef ADC_CommonInitStructure;
DMA_InitTypeDef       DMA_InitStructure;
GPIO_InitTypeDef      GPIO_InitStructure;

void init_ADC()
{
	/******************************************************************************/
	/*               ADCs interface clock, pin and DMA configuration              */
	/******************************************************************************/

	  // Enable peripheral clocks
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 |
	                         RCC_APB2Periph_ADC3, ENABLE);

	  // Configure ADC Channel 12 pin as analog input
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOC, &GPIO_InitStructure);

	  // DMA2 Stream0 channel0 configuration
	  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_CDR_ADDRESS;
	  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCTripleConvertedValue;
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	  DMA_InitStructure.DMA_BufferSize = 3;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	  DMA_Init(DMA2_Stream0, &DMA_InitStructure);

	  // DMA2_Stream0 enable
	  DMA_Cmd(DMA2_Stream0, ENABLE);


	/******************************************************************************/
	/*  ADCs configuration: triple interleaved with 5cycles delay to reach 6Msps  */
	/******************************************************************************/

	  // ADC Common configuration *************************************************
	  ADC_CommonInitStructure.ADC_Mode = ADC_TripleMode_Interl;
	  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_2;
	  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	  ADC_CommonInit(&ADC_CommonInitStructure);

	  // ADC1 regular channel 12 configuration ************************************
	  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_NbrOfConversion = 1;
	  ADC_Init(ADC1, &ADC_InitStructure);

	  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_15Cycles);

	  // Enable ADC1 DMA
	  ADC_DMACmd(ADC1, ENABLE);

	  // ADC2 regular channel 12 configuration ************************************
	  ADC_Init(ADC2, &ADC_InitStructure);
	  // ADC2 regular channel12 configuration
	  ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 1, ADC_SampleTime_15Cycles);

	  // ADC3 regular channel 12 configuration ************************************
	  ADC_Init(ADC3, &ADC_InitStructure);

	  // ADC3 regular channel12 configuration *************************************
	  ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_15Cycles);

	  // Enable DMA request after last transfer (multi-ADC mode) ******************
	  ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);

	  // Enable ADC1 **************************************************************
	  ADC_Cmd(ADC1, ENABLE);

	  // Enable ADC2 **************************************************************
	  ADC_Cmd(ADC2, ENABLE);

	  // Enable ADC3 **************************************************************
	  ADC_Cmd(ADC3, ENABLE);

	  // Start ADC1 Software Conversion
	  ADC_SoftwareStartConv(ADC1);
}