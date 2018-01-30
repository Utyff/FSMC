#include <stm32f4xx_conf.h>
#include <dac.h>


#define SIN_TABLE_SIZE  32
#if   SIN_TABLE_SIZE == 32
const uint16_t sinTable[32] = {
        2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
        3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
        599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};
#elif SIN_TABLE_SIZE == 128
const uint16_t sinTable[128] = {
        2000, 2093, 2186, 2278, 2370, 2461, 2551, 2640,
        2727, 2812, 2895, 2976, 3055, 3131, 3205, 3275,
        3343, 3407, 3468, 3526, 3579, 3629, 3675, 3717,
        3755, 3788, 3818, 3843, 3863, 3879, 3890, 3897,
        3900, 3897, 3890, 3879, 3863, 3843, 3818, 3788,
        3755, 3717, 3675, 3629, 3579, 3526, 3468, 3407,
        3343, 3275, 3205, 3131, 3055, 2976, 2895, 2812,
        2727, 2640, 2551, 2461, 2370, 2278, 2186, 2093,
        2000, 1906, 1813, 1721, 1629, 1538, 1448, 1359,
        1272, 1187, 1104, 1023, 944, 868, 794, 724,
        656, 592, 531, 473, 420, 370, 324, 282,
        244, 211, 181, 156, 136, 120, 109, 102,
        100, 102, 109, 120, 136, 156, 181, 211,
        244, 282, 324, 370, 420, 473, 531, 592,
        656, 724, 794, 868, 944, 1023, 1104, 1187,
        1272, 1359, 1448, 1538, 1629, 1721, 1813, 1906};
#endif

#define TRI_TABLE_SIZE 32
const uint16_t triTable[32] = {
        0, 256, 512, 768,
        1024, 1280, 1536, 1792,
        2048, 2304, 2560, 2816,
        3072, 3328, 3584, 3840,
        4096, 3840, 3584, 3328,
        3072, 2816, 2560, 2304,
        2048, 1792, 1536, 1280,
        1024, 768, 512, 256};

#define SQE_TABLE_SIZE 32
const uint16_t sqeTable[32] = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        4096, 4096, 4096, 4096,
        4096, 4096, 4096, 4096,
        4096, 4096, 4096, 4096,
        4096, 4096, 4096, 4096};

const uint8_t aEscalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};

DAC_SIGNAL_FORM DAC_SignalForm = Sin;

static void TIM6_Config();


void DAC_init() {
    DAC_DeInit();

    // Preconfiguration before using DAC----------------------------------
    GPIO_InitTypeDef GPIO_InitStructure;

    // DMA1 clock enable
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    // GPIOA clock enable (to be used with DAC)
    RCC_AHB1PeriphClockCmd(DAC_CLOCKPORT, ENABLE);
    // DAC Periph clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    // DAC channel 1 & 2 (DAC_OUT1 = PA.4)(DAC_OUT2 = PA.5) configuration
    GPIO_InitStructure.GPIO_Pin = DAC1_PIN | DAC2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(DAC_PORT, &GPIO_InitStructure);

    // TIM6 Configuration as TRGOSource_Update for DAC event
    TIM6_Config();

    DAC_SetGeneratorSignal(DAC_SignalForm);

    // Triangle signal generator -------------------------------------------
    DAC_Ch1_TriangleConfig();
}

void DAC_SetGeneratorSignal(DAC_SIGNAL_FORM form) {
    DAC_SignalForm = form;
    if (form == Sin) {
        DAC_Ch2_Config(sinTable, SIN_TABLE_SIZE);
    } else if (form == Tri) {
        DAC_Ch2_Config(triTable, TRI_TABLE_SIZE);
    } else {
        DAC_Ch2_Config(sqeTable, SQE_TABLE_SIZE);
    }
}

void DAC_NextGeneratorSignal() {
    DAC_SignalForm++;
    if (DAC_SignalForm > Sqe) {
        DAC_SignalForm = Sin;
    }
    DAC_SetGeneratorSignal(DAC_SignalForm);
}

/**
  * @note   TIM6 configuration is based on APB1 frequency
  * @note   TIM6 Update event occurs each TIM6CLK/256
  */
static void TIM6_Config() {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // TIM6 Periph clock enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    // Time base configuration
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 19; // 0x3F;  // TIM6CLK = 2 * PCLK1 = HCLK /2 = SystemCoreClock /2 = 84MHz
    TIM_TimeBaseStructure.TIM_Prescaler = 0;  // 84MHz / 10 = 8,4MHz | 5,8 in 100 us | 58 000 1s
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    // TIM6 TRGO selection
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

    // TIM6 enable counter
    TIM_Cmd(TIM6, ENABLE);
}

u16 presc = 20;

void DAC_step(s16 step) {
    if (step == 0) return;
    if (step < 0) {
        presc -= 5;
    } else {
        presc += 5;
    }

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    // Time base configuration
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = presc; // 0x3F;  // TIM6CLK = 2 * PCLK1 = HCLK /2 = SystemCoreClock /2 = 84MHz
    TIM_TimeBaseStructure.TIM_Prescaler = 0;  // 84MHz / 10 = 8,4MHz | 5,8 in 100 us | 58 000 1s
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
}


/**
  * @brief  DAC  Channel2 Configuration
  */
void DAC_Ch2_Config(const uint16_t *table, uint32_t tableSize) {
    DMA_InitTypeDef DMA_InitStructure;
    DAC_InitTypeDef DAC_InitStructure;

    // DAC channel2 Configuration
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);

    // DMA1_Stream6 channel7 configuration **************************************
    DMA_DeInit(DMA1_Stream6);
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &DAC->DHR12R2; //DAC_DHR12R2_ADDRESS; //
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) table;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = tableSize;
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
  */
void DAC_Ch1_EscalatorConfig() {
    DMA_InitTypeDef DMA_InitStructure;
    DAC_InitTypeDef DAC_InitStructure;

    // DAC channel1 Configuration
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    // DMA1_Stream5 channel7 configuration **************************************
    DMA_DeInit(DMA1_Stream5);
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &DAC->DHR8R1; // DAC_DHR8R1_ADDRESS;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &aEscalator8bit;
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
  */
void DAC_Ch1_TriangleConfig() {
    DAC_InitTypeDef DAC_InitStructure;

    // DAC channel2 Configuration
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_255;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    // Enable DAC Channel2
    DAC_Cmd(DAC_Channel_1, ENABLE);

    // Set DAC channel1 DHR12RD register
    //DAC_SetChannel1Data(DAC_Align_8b_R, 0x0);
    DAC->DHR12R1 = 4096 / 2 + 50; // shift signal to centre
}

/**
  * @brief  DAC  Channel1 Noise Configuration
  */
void DAC_Ch1_NoiseConfig() {
    DAC_InitTypeDef DAC_InitStructure;

    // DAC channel1 Configuration
    DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Noise;
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits11_0;
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitStructure);

    // Enable DAC Channel1
    DAC_Cmd(DAC_Channel_1, ENABLE);

    // Set DAC Channel1 DHR12L register
    DAC_SetChannel1Data(DAC_Align_12b_L, 0x7FF0);
}
