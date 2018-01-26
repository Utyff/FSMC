#include <dwt.h>
#include <adc.h>
#include <screen.h>

/*
 * ADCCLK = APB2 / ADC_Prescaler =  84 MHz / 4 = 21 MHz
 * 1 Tick = 1 / 21 MHz =  47 nanoseconds
 * SampleTime = ADC_SampleTime + ADC_Resolution = 3 + 8 = 11 Tick
 * SamplePerSec = 21MHz / 11 = 1,9 MSPS
 *
*/


struct ADC_param {
    u32 ADC_Prescaler;
    u8 ADC_SampleTime;
    float SampleTime;    // microseconds
    float ScreenTime;    // microseconds
};
typedef struct ADC_param ADC_PARAM;

#define ADC_Parameters_Size 31
const ADC_PARAM ADC_Parameters[ADC_Parameters_Size] = {
        {ADC_Prescaler_Div2, ADC_SampleTime_3Cycles,   0.2619048f,  83.80952f},
        {ADC_Prescaler_Div4, ADC_SampleTime_3Cycles,   0.5238095f,  167.61905f},
        {ADC_Prescaler_Div2, ADC_SampleTime_15Cycles,  0.5476190f,  175.23810f},
        {ADC_Prescaler_Div6, ADC_SampleTime_3Cycles,   0.7857143f,  251.42857f},
        {ADC_Prescaler_Div2, ADC_SampleTime_28Cycles,  0.8571429f,  274.28571f},
        {ADC_Prescaler_Div8, ADC_SampleTime_3Cycles,   1.0476190f,  335.23810f},
        {ADC_Prescaler_Div4, ADC_SampleTime_15Cycles,  1.0952381f,  350.47619f},
        {ADC_Prescaler_Div2, ADC_SampleTime_56Cycles,  1.5238095f,  487.61905f},
        {ADC_Prescaler_Div6, ADC_SampleTime_15Cycles,  1.6428571f,  525.71429f},
        {ADC_Prescaler_Div4, ADC_SampleTime_28Cycles,  1.7142857f,  548.57143f},
        {ADC_Prescaler_Div2, ADC_SampleTime_84Cycles,  2.1904762f,  700.95238f},
        {ADC_Prescaler_Div6, ADC_SampleTime_28Cycles,  2.5714286f,  822.85714f},
        {ADC_Prescaler_Div2, ADC_SampleTime_112Cycles, 2.8571429f,  914.28571f},
        {ADC_Prescaler_Div4, ADC_SampleTime_56Cycles,  3.0476190f,  975.23810f},
        {ADC_Prescaler_Div8, ADC_SampleTime_28Cycles,  3.4285714f,  1097.14286f},
        {ADC_Prescaler_Div2, ADC_SampleTime_144Cycles, 3.6190476f,  1158.09524f},
        {ADC_Prescaler_Div4, ADC_SampleTime_84Cycles,  4.3809524f,  1401.90476f},
        {ADC_Prescaler_Div6, ADC_SampleTime_56Cycles,  4.5714286f,  1462.85714f},
        {ADC_Prescaler_Div4, ADC_SampleTime_112Cycles, 5.7142857f,  1828.57143f},
        {ADC_Prescaler_Div8, ADC_SampleTime_56Cycles,  6.0952381f,  1950.47619f},
        {ADC_Prescaler_Div6, ADC_SampleTime_84Cycles,  6.5714286f,  2102.85714f},
        {ADC_Prescaler_Div4, ADC_SampleTime_144Cycles, 7.2380952f,  2316.19048f},
        {ADC_Prescaler_Div6, ADC_SampleTime_112Cycles, 8.5714286f,  2742.85714f},
        {ADC_Prescaler_Div8, ADC_SampleTime_84Cycles,  8.7619048f,  2803.80952f},
        {ADC_Prescaler_Div6, ADC_SampleTime_144Cycles, 10.8571429f, 3474.28571f},
        {ADC_Prescaler_Div8, ADC_SampleTime_112Cycles, 11.4285714f, 3657.14286f},
        {ADC_Prescaler_Div2, ADC_SampleTime_480Cycles, 11.6190476f, 3718.09524f},
        {ADC_Prescaler_Div8, ADC_SampleTime_144Cycles, 14.4761905f, 4632.38095f},
        {ADC_Prescaler_Div4, ADC_SampleTime_480Cycles, 23.2380952f, 7436.19048f},
        {ADC_Prescaler_Div6, ADC_SampleTime_480Cycles, 34.8571429f, 11154.28571f},
        {ADC_Prescaler_Div8, ADC_SampleTime_480Cycles, 46.4761905f, 14872.38095f}
};

u32 ADC_Prescaler = ADC_Prescaler_Div4;
u8 ADC_SampleTime = ADC_SampleTime_3Cycles;

u16 ScreenTime = 0;      // index in ScreenTimes
u16 ScreenTime_adj = 0;  // 0-9 shift in ScreenTime
const float ScreenTimes[] = {100, 200, 500, 1000, 2000, 5000, 10000, 20000};  // sweep screen, microseconds

union SampleBuffer samplesBuffer;
u8 half = 0;  // first or second half writing

uint32_t ADCStartTick;         // time when start ADC buffer fill
uint32_t ADCHalfElapsedTick;   // the last time half buffer fill
uint32_t ADCElapsedTick;       // the last time buffer fill



void ADC_GPIO_init() { // configure PC2 as ADC CH12
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable peripheral clocks
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // Configure PC2, ADC Channel 12 pin as analog input
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


void ADC_DMA_init() { // with IRQ when buffer fill
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

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
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA2_Stream0, DMA_IT_HT | DMA_IT_TC, ENABLE); // IRQ when transfer complete and half transfer
}


void ADC_init() {  // DMA mode
//  ADC_DeInit();
    ADC_GPIO_init();
//  ADC_DMA_init();

    ADC_InitTypeDef ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;

//  FLASH->ACR &= (~FLASH_ACR_PRFTEN);
//  FLASH_PrefetchBufferCmd(DISABLE);

    // разрешаем тактирование
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 | RCC_APB2Periph_ADC3, ENABLE);

    // базовая настройка
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler;
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

    // выбор канала
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime);

    ADC_DiscModeCmd(ADC1, DISABLE);
    ADC_EOCOnEachRegularChannelCmd(ADC1, ENABLE);
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);

    // включаем АЦП
    ADC_Cmd(ADC1, ENABLE);
    // Start ADC1 Software Conversion
    ADC_SoftwareStartConv(ADC1);
    ADCStartTick = DWT_Get_Current_Tick();
}

void ADC_set_parameters() {
    DMA_Cmd(DMA2_Stream0, DISABLE);
    DMA_SetCurrDataCounter(DMA2_Stream0, 0);

    DMA_Cmd(DMA2_Stream0, ENABLE);
}

void ADC_step_up() {
    if (ScreenTime_adj < 9)
        ScreenTime_adj++;
    else if (ScreenTime < sizeof(ScreenTimes) / sizeof(ScreenTimes[0]) - 2) // last value forbidden to assign
        ScreenTime_adj = 0, ScreenTime++;
}


void ADC_step_down() {
    if (ScreenTime_adj > 0)
        ScreenTime_adj--;
    else if (ScreenTime > 0)
        ScreenTime_adj = 9, ScreenTime--;
}


float ADC_getTime() {
    float time = ScreenTimes[ScreenTime];
    // next time always exist because last forbidden to assign
    float adj = (ScreenTimes[ScreenTime + 1] - time) * ScreenTime_adj / 10;
    time += adj;
    return time;
}

s16 sStep;
float time;
int ii;

void ADC_step(s16 step) {
    if (step == 0) return;
    if (step > 0) ADC_step_up();
    else ADC_step_down();
    sStep = step;

    time = ADC_getTime(); // get screen sweep time

    // looking last parameters set with ScreenTime less than required time
    int i = 1;
    while (ADC_Parameters[i].ScreenTime < time) {
        i++;
        if (i >= ADC_Parameters_Size) break;
    }

    i--;
    ii = i;
    ADC_Prescaler = ADC_Parameters[i].ADC_Prescaler;
    ADC_SampleTime = ADC_Parameters[i].ADC_SampleTime;

    // set X scale
    scaleX = ADC_Parameters[i].ScreenTime / time;

    ADC_init();
}


/*void ADC_step_Sample(s16 step) {
  if( step==0 )
    return;

  if( step>0 )
  {
    if (ADC_SampleTime < ADC_SampleTime_480Cycles)
      ADC_SampleTime++;
  } else
  {
    if (ADC_SampleTime > ADC_SampleTime_3Cycles)
      ADC_SampleTime--;
  }
  ADC_init();
}


void ADC_step_Prescaler(s16 step) {
  if( step==0 )
    return;

  if( step>0 )
  {
    if (ADC_Prescaler < ADC_Prescaler_Div8)
      ADC_Prescaler += ADC_Prescaler_Div4;
  } else
  {
    if (ADC_Prescaler > ADC_Prescaler_Div2)
      ADC_Prescaler -= ADC_Prescaler_Div4;
  }
  ADC_init();
} //*/

u16 ICount = 0;

// dma2 stream 0 irq handler
void DMA2_Stream0_IRQHandler() {
    ICount++;
    // Test on DMA Stream HalfTransfer Complete interrupt
    if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_HTIF0)) {
        // Clear Stream0 HalfTransfer
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_HTIF0);

        // count time for half circle
        ADCHalfElapsedTick = DWT_Elapsed_Tick(ADCStartTick);
        half = 0;
    }

    // Test on DMA Stream Transfer Complete interrupt
    if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0)) {
        // Clear Stream0 Transfer Complete
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);

        // count time for one circle
        ADCElapsedTick = DWT_Elapsed_Tick(ADCStartTick);
        ADCStartTick = DWT_Get_Current_Tick();
        half = 1;
    }
}
