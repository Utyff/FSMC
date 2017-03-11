#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"
#include <dwt.h>
#include <adc.h>


union SampleBuffer samplesBuffer;
u8    half=0;

uint32_t ADCStartTick;         // time when start ADC buffer fill
uint32_t ADCHalfElapsedTick;   // the last time half buffer fill
uint32_t ADCElapsedTick;       // the last time buffer fill


static void ADC_GPIO_init()  // configure PC2 as ADC CH12
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


static void ADC_DMA_init()  // with IRQ when buffer fill
{
  DMA_InitTypeDef   DMA_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

  // DMA2 Stream0 channel0 configuration
  DMA_DeInit(DMA2_Stream0);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &ADC1->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &samplesBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = SAMPLES_1_BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);

  DMA_Cmd(DMA2_Stream0, ENABLE);

  // Enable the DMA Stream IRQ Channel
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init ( &NVIC_InitStructure );
  DMA_ITConfig ( DMA2_Stream0, DMA_IT_HT | DMA_IT_TC, ENABLE ); // DMA_IT_HT |  // IRQ when transfer complete and half transfer
}

void ADC_init()  // DMA mode
{
  ADC_GPIO_init();
  ADC_DMA_init();

  ADC_InitTypeDef ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;

//  FLASH->ACR &= (~FLASH_ACR_PRFTEN);
//  FLASH_PrefetchBufferCmd(DISABLE);

  // ��������� ������������
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 | RCC_APB2Periph_ADC3, ENABLE);

  // ������� ���������
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ExternalTrigConv = 0;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;
  ADC_Init(ADC1, &ADC_InitStructure);

  // ����� ������
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_3Cycles);

  ADC_DiscModeCmd(ADC1, DISABLE);
  ADC_EOCOnEachRegularChannelCmd(ADC1, ENABLE);
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);

  // �������� ���
  ADC_Cmd(ADC1, ENABLE);
  // Start ADC1 Software Conversion
  ADC_SoftwareStartConv(ADC1);
  ADCStartTick = DWT_Get_Current_Tick();
}

// dma2 stream 0 irq handler
void DMA2_Stream0_IRQHandler ( void )
{
  // Test on DMA Stream HalfTransfer Complete interrupt
  if ( DMA_GetITStatus(DMA2_Stream0, DMA_IT_HTIF0) )
  {
    // Clear Stream0 HalfTransfer
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_HTIF0);

    // count time for half circle
    ADCHalfElapsedTick = DWT_Elapsed_Tick(ADCStartTick);
    half = 0;
  }

  // Test on DMA Stream Transfer Complete interrupt
  if ( DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0) )
  {
    // Clear Stream0 Transfer Complete
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);

    // count time for one circle
    ADCElapsedTick = DWT_Elapsed_Tick(ADCStartTick);
    ADCStartTick = DWT_Get_Current_Tick();
    half = 1;
  }
}
