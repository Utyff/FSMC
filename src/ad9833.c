#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"


#define AD_F_MCLK	25000000
#define AD_2POW28	268435456

#define AD_FREQ_CALC(freq) (uint32_t)(((double)AD_2POW28/(double)AD_F_MCLK*freq)*4)
#define AD_PHASE_CALC(phase_deg) (uint16_t)((512*phase_deg)/45)


#define AD_B28      	13
#define AD_HLB      	12
#define AD_FSELECT  	11
#define AD_PSELECT  	10
#define AD_RESET    	8
#define AD_SLEEP1   	7
#define AD_SLEEP12  	6
#define AD_OPBITEN  	5
#define AD_DIV2     	3
#define AD_MODE     	1

#define AD_OFF      	0
#define AD_TRIANGLE 	1
#define AD_SQUARE   	2
#define AD_SINE     	3

#define AD_FREQ0    	(1<<14)
#define AD_FREQ1    	(1<<15)
#define AD_PHASE0   	(3<<14)
#define AD_PHASE1   	((3<<14)|(1<<13))

typedef struct {
uint8_t  freq_out;
uint8_t  phase_out;
uint8_t  mode;
uint16_t command_reg;
} ad9833_settings_t;

typedef struct {
    float    freq[2];
    float    phase[2];
    float    mod_freq;
} ad9833_settings_f;


ad9833_settings_t ad_settings;
ad9833_settings_f ad_settings2;


static void delay9833(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}


static void AD9833_word(uint16_t data)
{
	SPI1->DR = (data>>8);        // High 8 bits, MSB first
	while(!(SPI1->SR & SPI_I2S_FLAG_TXE));
	while(  SPI1->SR & SPI_I2S_FLAG_BSY );
	SPI1->DR = (uint8_t) data;   // Low 8 bits
	while(!(SPI1->SR & SPI_I2S_FLAG_TXE));
	while(  SPI1->SR & SPI_I2S_FLAG_BSY );
}

static void AD9833_write(uint16_t data)
{
	GPIO_ResetBits(GPIOE, GPIO_Pin_7);
	delay9833(0xFF);
	AD9833_word(data);
	delay9833(0xFF);
	GPIO_SetBits(GPIOE, GPIO_Pin_7);
}


void AD9833_setFreq(uint8_t reg, double freq){
    uint32_t freq_reg;
    uint16_t reg_reg; //probably should be renamed...
    freq_reg = AD_FREQ_CALC(freq);
    ad_settings2.freq[reg] = freq;

    if (reg==1)
        reg_reg = AD_FREQ1;
    else
        reg_reg = AD_FREQ0;

    GPIO_ResetBits(GPIOE, GPIO_Pin_7);
    delay9833(0xFF);
    AD9833_word(reg_reg | (0x3FFF&(uint16_t)(freq_reg>>2 )));
    AD9833_word(reg_reg | (0x3FFF&(uint16_t)(freq_reg>>16)));
    delay9833(0xFF);
    GPIO_SetBits(GPIOE, GPIO_Pin_7);
}

void    AD9833_setFreqOut(uint8_t freq_out){
    ad_settings.freq_out = freq_out;
    switch (freq_out){
        case 0:
            ad_settings.command_reg &= ~(1<<AD_FSELECT);
            break;
        case 1:
            ad_settings.command_reg |= (1<<AD_FSELECT);
            break;
        case 2:
            //TODO
            break;
    }

    AD9833_write(ad_settings.command_reg);
}

double AD9833_getFreq(uint8_t reg){
    return ad_settings2.freq[reg];
}


void AD9833_setPhase(uint8_t reg, double phase){
    uint16_t reg_reg; //probably should be renamed...
    if (reg==1)
        reg_reg = AD_PHASE1;
    else
        reg_reg = AD_PHASE0;

    ad_settings2.phase[reg] = phase;


    AD9833_write(reg_reg | AD_PHASE_CALC(ad_settings2.phase[reg]));

}

void AD9833_setPhaseOut(uint8_t phase_out){
    ad_settings.phase_out = phase_out;
    switch (phase_out){
        case 0:
            ad_settings.command_reg &= ~(1<<AD_PSELECT);
            break;
        case 1:
            ad_settings.command_reg |= (1<<AD_PSELECT);
            break;
        case 2:
            //TODO
            break;
    }

    AD9833_write(ad_settings.command_reg);

}




void AD9833_setMode(uint8_t mode)
{
	ad_settings.mode = mode;
	switch(mode)
	{
			case AD_OFF:
	            ad_settings.command_reg |= (1<<AD_SLEEP12);
	            ad_settings.command_reg |= (1<<AD_SLEEP1);
	            break;
			case AD_TRIANGLE:
	            ad_settings.command_reg &= ~(1<<AD_OPBITEN);
	            ad_settings.command_reg |=  (1<<AD_MODE);
	            ad_settings.command_reg &= ~(1<<AD_SLEEP12);
	            ad_settings.command_reg &= ~(1<<AD_SLEEP1);
	            break;
	        case AD_SQUARE:
	            ad_settings.command_reg |=  (1<<AD_OPBITEN);
	            ad_settings.command_reg &= ~(1<<AD_MODE);
	            ad_settings.command_reg |=  (1<<AD_DIV2);
	            ad_settings.command_reg &= ~(1<<AD_SLEEP12);
	            ad_settings.command_reg &= ~(1<<AD_SLEEP1);
	            break;
	        case AD_SINE:
	            ad_settings.command_reg &= ~(1<<AD_OPBITEN);
	            ad_settings.command_reg &= ~(1<<AD_MODE);
	            ad_settings.command_reg &= ~(1<<AD_SLEEP12);
	            ad_settings.command_reg &= ~(1<<AD_SLEEP1);
	            break;
	}

	AD9833_write(ad_settings.command_reg); // Send selected MODE to AD9833
}


void SPI1_init(void)
  {

	    GPIO_InitTypeDef GPIO_InitStruct;
	    SPI_InitTypeDef SPI_InitStruct;


	    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	    /* configure pins used by SPI1
	     * PA5 = SCK
	     * PA6 = MISO
	     * PA7 = MOSI
	     */
	    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_7 | GPIO_Pin_5;
	    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	    GPIO_Init(GPIOA, &GPIO_InitStruct);

	    // connect SPI1 pins to SPI alternate function
	    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	    // enable clock for used IO pins
	    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	    // Configure the chip select pin
	    // in this case we will use PE7
	    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	    GPIO_Init(GPIOE, &GPIO_InitStruct);

	    GPIOE->BSRRL |= GPIO_Pin_7; // set PE7 high

	    // enable peripheral clock
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	    /* configure SPI1 in Mode 2
	     * CPOL = 1 --> clock is high when idle
	     * CPHA = 0 --> data is sampled at the first edge
	     */
	    SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
	    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;      // transmit in master mode, NSS pin has to be always high
	    SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b; // one packet of data is 8 bits wide
	    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;        // clock is low when idle
	    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;       // data sampled at first edge
	    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
	    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // SPI frequency is APB2 frequency / 4
	    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB; // data is transmitted MSB first
	    SPI_Init(SPI1, &SPI_InitStruct);

	    SPI_Cmd(SPI1, ENABLE); // enable SPI1
	}

void AD9833_init() {

    SPI1_init();

    AD9833_setFreq(0, 100);
    AD9833_setFreqOut(0);
    AD9833_setMode(AD_SQUARE);
}
