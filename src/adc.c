#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"

#define ADC_CDR_ADDRESS    ((uint32_t)0x40012308)
#define ADC1_DR_ADDRESS    ((uint32_t)0x4001204C)
#define SAMPLE_BUFFER_SIZE 2048

__IO uint16_t SamplesBuffer[SAMPLE_BUFFER_SIZE];


void init_ADC_GPIO()
{
    GPIO_InitTypeDef      GPIO_InitStructure;

    // Enable peripheral clocks
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 |
                           RCC_APB2Periph_ADC3, ENABLE);

    // Configure ADC Channel 12 pin as analog input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void init_ADC2()
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;

  init_ADC_GPIO();

  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; // APB2 = 84MZh | ADCCLK max = 30 | ADCCLK = 84/4 = 21
  ADC_CommonInit(&ADC_CommonInitStructure);

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;

  // ADC1 regular channel 12 configuration ************************************
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_112Cycles);
  // Enable ADC1 DMA
  ADC_DMACmd(ADC1, ENABLE);
  //ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

  // Enable ADC1 ********************************************************
  ADC_Cmd(ADC1, ENABLE);

  // Start ADC1 Software Conversion
  ADC_SoftwareStartConv(ADC1);
}


u16 adc_convert()
{
  ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART;        //Start the conversion
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); //Processing the conversion
  return ADC1->DR;                               //Return the converted data
}

void sampleCircle()
{
  int i = 0;
  while(1) {
    SamplesBuffer[i] = adc_convert();//Read the ADC converted value
    i++;
    if( i>=SAMPLE_BUFFER_SIZE ) i=0;
  }
}

void init_ADC()
{
  ADC_InitTypeDef ADC_init_structure; //Structure for adc confguration
  GPIO_InitTypeDef GPIO_initStructre; //Structure for analog input pin
  //Clock configuration
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE); //The ADC1 is connected the APB2 peripheral bus thus we will use its clock source
  RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN,ENABLE); //Clock for the ADC port!! Do not forget about this one ;)
  //Analog pin configuration
  GPIO_StructInit(&GPIO_initStructre);
  GPIO_initStructre.GPIO_Pin = GPIO_Pin_2;    //The channel 12 is connected to PC2
  GPIO_initStructre.GPIO_Mode = GPIO_Mode_AN; //The PC2 pin is configured in analog mode
  GPIO_initStructre.GPIO_PuPd = GPIO_PuPd_NOPULL; //We don't need any pull up or pull down
  GPIO_Init(GPIOC,&GPIO_initStructre);        //Affecting the port with the initialization structure configuration

  //ADC structure configuration
  ADC_DeInit();
  ADC_init_structure.ADC_Resolution = ADC_Resolution_12b; //Input voltage is converted into a 12bit number giving a maximum value of 4096
  ADC_init_structure.ADC_DataAlign = ADC_DataAlign_Left;  //data converted will be shifted to right
  ADC_init_structure.ADC_ContinuousConvMode = ENABLE;     //the conversion is continuous, the input data is converted more than once
  ADC_init_structure.ADC_ScanConvMode = DISABLE;          //The scan is configured in one channel
  ADC_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;       // conversion is synchronous with TIM1 and CC1 (actually I'm not sure about this one :/)
  ADC_init_structure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // no trigger for conversion
  ADC_init_structure.ADC_NbrOfConversion = 1;             //I think this one is clear :p
  ADC_Init(ADC1,&ADC_init_structure);                     //Initialize ADC with the previous configuration
  //Enable ADC conversion
  ADC_Cmd(ADC1,ENABLE);
  //Select the channel to be read from
  ADC_RegularChannelConfig(ADC1,ADC_Channel_12,1,ADC_SampleTime_480Cycles);

  sampleCircle();
}

void init_ADC_DMA()
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;

  /******************************************************************************/
  /*               ADCs interface clock, pin and DMA configuration              */
  /******************************************************************************/
  init_ADC_GPIO();

    // DMA2 Stream0 channel0 configuration
    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_CDR_ADDRESS;// ADC1_DR_ADDRESS;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&SamplesBuffer;
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

    /******************************************************************************/
    /*  ADCs configuration: triple interleaved with 5cycles delay to reach 6Msps  */
    /******************************************************************************/

    // ADC Common configuration *************************************************
//  ADC_CommonInitStructure.ADC_Mode = ADC_TripleMode_Interl;
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
//    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_2;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; // APB2 = 84MZh | ADCCLK max = 30 | ADCCLK = 84/4 = 21
    ADC_CommonInit(&ADC_CommonInitStructure);

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;

    // ADC1 regular channel 12 configuration ************************************
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_112Cycles);
    // Enable ADC1 DMA
    ADC_DMACmd(ADC1, ENABLE);
//    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

    // ADC2 regular channel 12 configuration ************************************
//  ADC_Init(ADC2, &ADC_InitStructure);
//  ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 1, ADC_SampleTime_112Cycles);

    // ADC3 regular channel 12 configuration ************************************
//  ADC_Init(ADC3, &ADC_InitStructure);
//  ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 1, ADC_SampleTime_112Cycles);

    // Enable DMA request after last transfer (multi-ADC mode) ******************
//  ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);

    // Enable ADC1, 2, 3 ********************************************************
    ADC_Cmd(ADC1, ENABLE);
//  ADC_Cmd(ADC2, ENABLE);
//  ADC_Cmd(ADC3, ENABLE);

    // Start ADC1 Software Conversion
    ADC_SoftwareStartConv(ADC1);
}
