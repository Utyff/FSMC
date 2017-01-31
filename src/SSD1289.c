#include "SSD1289.h"

#define LCD_REG      (*((volatile unsigned short *) 0x60000000))
#define LCD_RAM      (*((volatile unsigned short *) 0x60020000))

/**
  * @brief  Reads the selected LCD Register.
  * @param  LCD_Reg: address of the selected register.
  * @retval LCD Register Value.
  */
inline uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
  /* Write 16-bit Index (then Read Reg) */
  LCD_REG = LCD_Reg;
  /* Read 16-bit Reg */
  return (LCD_RAM);
}

inline void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  LCD_REG = LCD_Reg;
  LCD_RAM = LCD_RegValue;
}

inline void LCD_WriteRAM(uint16_t RGB_Code)
{
  LCD_RAM = RGB_Code;
}

void LCD_Init_ili9341(void);
void LCD_Init_H8347(void);
void LCD_Init_1289(void);
void LCD_Init_kumle(void);


void LCD_GPIO_Conf(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

  GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);		// D2
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);		// D3
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);		// NOE -> RD
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);		// NWE -> WR
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);		// NE1 -> CS
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);		// D13
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);		// D14
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);		// D15
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);		// A16 -> RS
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);		// D0
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);		// D1

  GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);		// D4
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);		// D5
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);		// D6
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);		// D7
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);		// D8
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);		// D9
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);		// D10
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);		// D11
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);		// D12

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}


void LCD_FSMCConfig(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructure;

  FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 16;  //0
  FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 3;    //0
  FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 17;     //3
  FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 1;        //1
  FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 7;
  FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;

  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;   //disable
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

  FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 16;   //0
  FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 3;	    //0
  FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 17;	    //3
  FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 1;         //1
  FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 7;
  FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

void LCD_Init_H8347(void) //
{
  LCD_GPIO_Conf();
  Delay(3000);
  LCD_FSMCConfig();
  Delay(3000);

//  uint16_t id10 = LCD_ReadReg(0x0000);
//  uint16_t id11 = LCD_ReadReg(0x0067);
  LCD_WriteReg(0x0007,0x0021);
  LCD_WriteReg(0x0000,0x0001);
//  uint16_t id20 = LCD_ReadReg(0x0000);
//  uint16_t id21 = LCD_ReadReg(0x0067);
  LCD_WriteReg(0x0007,0x0023);
  LCD_WriteReg(0x0010,0x0000);

  Delay(3000);
  LCD_WriteReg(0x007,0x0033);
  LCD_WriteReg(0x0011,0x6838);
  LCD_WriteReg(0x0002,0x0600);
}

void LCD_Init_1289(void) //
{
  LCD_GPIO_Conf();
  Delay(3000);
  LCD_FSMCConfig();
  Delay(3000);

//  uint16_t id10 = LCD_ReadReg(0x0000);
//  uint16_t id11 = LCD_ReadReg(0x0067);
  LCD_WriteReg(0x0007,0x0021);    Delay(50);
  LCD_WriteReg(0x0000,0x0001);    Delay(50);
//  uint16_t id20 = LCD_ReadReg(0x0000);
//  uint16_t id21 = LCD_ReadReg(0x0067);
  LCD_WriteReg(0x0007,0x0023);    Delay(50);
  LCD_WriteReg(0x0010,0x0000);    Delay(90);
  LCD_WriteReg(0x0007,0x0033);    Delay(50);
  LCD_WriteReg(0x0011,0x6830);    Delay(50);
  LCD_WriteReg(0x0002,0x0600);    Delay(50);
  LCD_WriteReg(0x0012,0x6CEB);    Delay(50);
  LCD_WriteReg(0x0003,0xA8A4);    Delay(50);
  LCD_WriteReg(0x000C,0x0000);    Delay(50);
  LCD_WriteReg(0x000D,0x080C);    Delay(50);
  LCD_WriteReg(0x000E,0x2B00);    Delay(50);
  LCD_WriteReg(0x001E,0x00B0);    Delay(50);
  LCD_WriteReg(0x0001,0x2b3F);    Delay(50);  //RGB
  LCD_WriteReg(0x0005,0x0000);    Delay(50);
  LCD_WriteReg(0x0006,0x0000);    Delay(50);
  LCD_WriteReg(0x0016,0xEF1C);    Delay(50);
  LCD_WriteReg(0x0017,0x0103);    Delay(50);
  LCD_WriteReg(0x000B,0x0000);    Delay(50);
  LCD_WriteReg(0x000F,0x0000);    Delay(50);
  LCD_WriteReg(0x0041,0x0000);    Delay(50);
  LCD_WriteReg(0x0042,0x0000);    Delay(50);
  LCD_WriteReg(0x0048,0x0000);    Delay(50);
  LCD_WriteReg(0x0049,0x013F);    Delay(50);
  LCD_WriteReg(0x004A,0x0000);    Delay(50);
  LCD_WriteReg(0x004B,0x0000);    Delay(50);
  LCD_WriteReg(0x0044,0xEF00);    Delay(50);
  LCD_WriteReg(0x0045,0x0000);    Delay(50);
  LCD_WriteReg(0x0046,0x013F);    Delay(50);
  LCD_WriteReg(0x0030,0x0707);    Delay(50);
  LCD_WriteReg(0x0031,0x0204);    Delay(50);
  LCD_WriteReg(0x0032,0x0204);    Delay(50);
  LCD_WriteReg(0x0033,0x0502);    Delay(50);
  LCD_WriteReg(0x0034,0x0507);    Delay(50);
  LCD_WriteReg(0x0035,0x0204);    Delay(50);
  LCD_WriteReg(0x0036,0x0204);    Delay(50);
  LCD_WriteReg(0x0037,0x0502);    Delay(50);
  LCD_WriteReg(0x003A,0x0302);    Delay(50);
  LCD_WriteReg(0x002F,0x12BE);    Delay(50);           
  LCD_WriteReg(0x003B,0x0302);    Delay(50);
  LCD_WriteReg(0x0023,0x0000);    Delay(50);
  LCD_WriteReg(0x0024,0x0000);    Delay(50);
  LCD_WriteReg(0x0025,0x8000);    Delay(50);
  LCD_WriteReg(0x004f,0x0000);    Delay(50);
  LCD_WriteReg(0x004e,0x0000);    Delay(50);
}

void LCD_Init_kumle() //_kumle
{
  static uint16_t DeviceCode;

  LCD_GPIO_Conf();
  Delay(3000);
  LCD_FSMCConfig();
  Delay(3000);

  DeviceCode = LCD_ReadReg(0x0000);

  //LCD_WriteReg(0x0000,0x0001);    Delay(100);
  DeviceCode = LCD_ReadReg(0x0000);

  LCD_WriteReg(0x0003,0xA8A4);    Delay(100);
  /*  LCD_WriteReg(0x000C,0x0000);    Delay(100);
  LCD_WriteReg(0x000D,0x080C);    Delay(100);
  LCD_WriteReg(0x000E,0x2B00);    Delay(100);
  LCD_WriteReg(0x001E,0x00B0);    Delay(100);
  LCD_WriteReg(0x0001,0x2B3F);    Delay(100);
  LCD_WriteReg(0x0002,0x0600);    Delay(100);
  LCD_WriteReg(0x0010,0x0000);    Delay(100);
  LCD_WriteReg(0x0011,0x6070);    Delay(100);
  LCD_WriteReg(0x0005,0x0000);    Delay(100);
  LCD_WriteReg(0x0006,0x0000);    Delay(100);
  LCD_WriteReg(0x0016,0xEF1C);    Delay(100);
  LCD_WriteReg(0x0017,0x0003);    Delay(100);
  LCD_WriteReg(0x0007,0x0233);    Delay(100);
  LCD_WriteReg(0x000B,0x0000);    Delay(100);
  LCD_WriteReg(0x000F,0x0000);    Delay(100);
  LCD_WriteReg(0x0041,0x0000);    Delay(100);
  LCD_WriteReg(0x0042,0x0000);    Delay(100);
  LCD_WriteReg(0x0048,0x0000);    Delay(100);
  LCD_WriteReg(0x0049,0x013F);    Delay(100);
  LCD_WriteReg(0x004A,0x0000);    Delay(100);
  LCD_WriteReg(0x004B,0x0000);    Delay(100);
  LCD_WriteReg(0x0044,0xEF00);    Delay(100);
  LCD_WriteReg(0x0045,0x0000);    Delay(100);
  LCD_WriteReg(0x0046,0x013F);    Delay(100);
  LCD_WriteReg(0x0030,0x0707);    Delay(100);
  LCD_WriteReg(0x0031,0x0204);    Delay(100);
  LCD_WriteReg(0x0032,0x0204);    Delay(100);
  LCD_WriteReg(0x0033,0x0502);    Delay(100);
  LCD_WriteReg(0x0034,0x0507);    Delay(100);
  LCD_WriteReg(0x0035,0x0204);    Delay(100);
  LCD_WriteReg(0x0036,0x0204);    Delay(100);
  LCD_WriteReg(0x0037,0x0502);    Delay(100);
  LCD_WriteReg(0x003A,0x0302);    Delay(100);
  LCD_WriteReg(0x003B,0x0302);    Delay(100);
  LCD_WriteReg(0x0023,0x0000);    Delay(100);
  LCD_WriteReg(0x0024,0x0000);    Delay(100);
  LCD_WriteReg(0x0025,0x8000);    Delay(100);
  LCD_WriteReg(0x004f,0);
  LCD_WriteReg(0x004e,0); //*/
  Delay(100);
}

void LCD_Init() //_ili9341
{
	LCD_WriteReg(0x0000,0x0001);
	Delay(10);

	LCD_WriteReg(0x0015,0x0030);
	LCD_WriteReg(0x0011,0x0040);
	LCD_WriteReg(0x0010,0x1628);
	LCD_WriteReg(0x0012,0x0000);
	LCD_WriteReg(0x0013,0x104d);
	Delay(10);
	LCD_WriteReg(0x0012,0x0010);
	Delay(10);
	LCD_WriteReg(0x0010,0x2620);
	LCD_WriteReg(0x0013,0x344d);
	Delay(10);

	LCD_WriteReg(0x0001,0x0100);
	LCD_WriteReg(0x0002,0x0300);
	LCD_WriteReg(0x0003,0x1030);
	LCD_WriteReg(0x0008,0x0604);
	LCD_WriteReg(0x0009,0x0000);
	LCD_WriteReg(0x000A,0x0008);

	LCD_WriteReg(0x0041,0x0002);
	LCD_WriteReg(0x0060,0x2700);
	LCD_WriteReg(0x0061,0x0001);
	LCD_WriteReg(0x0090,0x0182);
	LCD_WriteReg(0x0093,0x0001);
	LCD_WriteReg(0x00a3,0x0010);
	Delay(10);

	// Настройки гаммы
	LCD_WriteReg(0x30,0x0000);
	LCD_WriteReg(0x31,0x0502);
	LCD_WriteReg(0x32,0x0307);
	LCD_WriteReg(0x33,0x0305);
	LCD_WriteReg(0x34,0x0004);
	LCD_WriteReg(0x35,0x0402);
	LCD_WriteReg(0x36,0x0707);
	LCD_WriteReg(0x37,0x0503);
	LCD_WriteReg(0x38,0x1505);
	LCD_WriteReg(0x39,0x1505);
	Delay(10);

	// Включение дисплея
	LCD_WriteReg(0x0007,0x0001);
	Delay(10);
	LCD_WriteReg(0x0007,0x0021);
	LCD_WriteReg(0x0007,0x0023);
	Delay(10);
	LCD_WriteReg(0x0007,0x0033);
	Delay(10);
	LCD_WriteReg(0x0007,0x0133);
}
