#include "stm32f4xx_conf.h"
#include <dac.h>


//#define DAC_DHR12R2_ADDRESS    0x40007414
//#define DAC_DHR8R1_ADDRESS     0x40007410

const uint16_t aSine12bit[32] = {
        2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
        3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
        599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};
const uint8_t aEscalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};

static void TIM6_Config();


void DAC_init()
{
    DAC_DeInit();

    // Preconfiguration before using DAC----------------------------------
    GPIO_InitTypeDef GPIO_InitStructure;

    // DMA1 clock enable
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    // GPIOA clock enable (to be used with DAC)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    // DAC Periph clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    // DAC channel 1 & 2 (DAC_OUT1 = PA.4)(DAC_OUT2 = PA.5) configuration
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // TIM6 Configuration ------------------------------------------------
    TIM6_Config();

    DAC_Ch1_EscalatorConfig();
    // Sine Wave generator -----------------------------------------------
    DAC_Ch2_SineWaveConfig();
    // Noise Wave generator ----------------------------------------------
//    DAC_Ch1_NoiseConfig();
    // Triangle Wave generator -------------------------------------------
//    DAC_Ch2_TriangleConfig();
}

/**
  * @brief  TIM6 Configuration
  * @note   TIM6 configuration is based on APB1 frequency
  * @note   TIM6 Update event occurs each TIM6CLK/256
  * @param  None
  * @retval None
  */
static void TIM6_Config()
{
    TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
    // TIM6 Periph clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    /* --------------------------------------------------------
    TIM3 input clock (TIM6CLK) is set to 2 * APB1 clock (PCLK1),
    since APB1 prescaler is different from 1.
      TIM6CLK = 2 * PCLK1
      TIM6CLK = HCLK / 2 = SystemCoreClock /2

    TIM6 Update event occurs each TIM6CLK/256

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.

    ----------------------------------------------------------- */
    // Time base configuration
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 0x17F;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    // TIM6 TRGO selection
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

    // TIM6 enable counter
    TIM_Cmd(TIM6, ENABLE);
}

/**
  * @brief  DAC  Channel2 SineWave Configuration
  * @param  None
  * @retval None
  */
void DAC_Ch2_SineWaveConfig()
{
    DMA_InitTypeDef DMA_InitStructure;
    DAC_InitTypeDef  DAC_InitStructure;

    // DAC channel2 Configuration
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);

    // DMA1_Stream6 channel7 configuration **************************************
    DMA_DeInit(DMA1_Stream6);
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR12R2; //DAC_DHR12R2_ADDRESS; //
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&aSine12bit;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = 32;
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
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);

    // Enable DMA1_Stream6
    DMA_Cmd(DMA1_Stream6, ENABLE);

    // Enable DAC Channel2
    DAC_Cmd(DAC_Channel_2, ENABLE);

    // Enable DMA for DAC Channel2
    DAC_DMACmd(DAC_Channel_2, ENABLE);
}

/**
  * @brief  DAC Channel1 Escalator Configuration
  * @param  None
  * @retval None
  */
void DAC_Ch1_EscalatorConfig()
{
    DMA_InitTypeDef DMA_InitStructure;
    DAC_InitTypeDef  DAC_InitStructure;

    // DAC channel1 Configuration
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    // DMA1_Stream5 channel7 configuration **************************************
    DMA_DeInit(DMA1_Stream5);
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&DAC->DHR8R1; // DAC_DHR8R1_ADDRESS;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&aEscalator8bit;
    DMA_InitStructure.DMA_BufferSize = 6;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);

    // Enable DMA1_Stream5
    DMA_Cmd(DMA1_Stream5, ENABLE);

    // Enable DAC Channel1
    DAC_Cmd(DAC_Channel_1, ENABLE);

    // Enable DMA for DAC Channel1
    DAC_DMACmd(DAC_Channel_1, ENABLE);
}

/**
  * @brief  DAC Channel2 Triangle Configuration
  * @param  None
  * @retval None
  */
void DAC_Ch2_TriangleConfig()
{
    DAC_InitTypeDef  DAC_InitStructure;

    // DAC channel2 Configuration
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_4095;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);

    // Enable DAC Channel2
    DAC_Cmd(DAC_Channel_2, ENABLE);

    // Set DAC channel2 DHR12RD register
    DAC_SetChannel2Data(DAC_Align_12b_R, 0x0);
}

/**
  * @brief  DAC  Channel1 Noise Configuration
  * @param  None
  * @retval None
  */
void DAC_Ch1_NoiseConfig()
{
    DAC_InitTypeDef  DAC_InitStructure;

    // DAC channel1 Configuration
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Noise;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits11_0;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    // Enable DAC Channel1
    DAC_Cmd(DAC_Channel_1, ENABLE);

    // Set DAC Channel1 DHR12L register
    DAC_SetChannel1Data(DAC_Align_12b_L, 0x0);//7FF0);
}

