#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"

//#define ADC_CDR_ADDRESS    ((uint32_t)0x40012308)
//#define ADC1_DR_ADDRESS    ((uint32_t)0x4001204C) // &ADC1->DR
#define SAMPLE_BUFFER_SIZE 2048

__IO uint16_t SamplesBuffer[SAMPLE_BUFFER_SIZE];


static void init_ADC_GPIO()  // configure PC2 as ADC CH12
{
  GPIO_InitTypeDef      GPIO_InitStructure;

  // Enable peripheral clocks
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  // Configure PC2, ADC Channel 12 pin as analog input
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}


static void dma() // http://ziblog.ru
{
  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

  // DMA2 Stream0 channel0 configuration
  DMA_DeInit(DMA2_Stream0);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &ADC1->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &SamplesBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = SAMPLE_BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);

  // DMA2_Stream0 enable
  DMA_Cmd(DMA2_Stream0, ENABLE);
}

void init_ADC() // DMA mode  http://ziblog.ru
{
  init_ADC_GPIO();
  dma();

  ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;

  // разрешаем тактирование
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 | RCC_APB2Periph_ADC3, ENABLE);

  // базовая настройка
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
  ADC_InitStructure.ADC_ExternalTrigConv = 0;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_Init(ADC1, &ADC_InitStructure);

  // выбор канала
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_3Cycles);

  ADC_DiscModeCmd(ADC1, DISABLE);
  ADC_EOCOnEachRegularChannelCmd(ADC1, ENABLE);
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);

  // включаем АЦП
  ADC_Cmd(ADC1, ENABLE);
  // Start ADC1 Software Conversion
  ADC_SoftwareStartConv(ADC1);
}
