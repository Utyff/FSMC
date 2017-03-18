#include "lcd.h"
//#include "stdlib.h"
#include "font.h"
//#include "usart.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////

//2.4 Inch /2.8 inch/3.5 inch/4.3 inch TFT LCD driver
// Support driver IC models include:ILI9341/ILI9325/RM68042/RM68021/ILI9320/ILI9328/LGDP4531/LGDP4535/
//                  SPFD5408/1505/B505/C505/NT35310/NT35510 etc.
// STM32F4 Project - library function version
// Taobao shop: http://mcudev.taobao.com
//********************************************************************************
//V1.2 Modify the description
// Support SPFD5408 drive, in addition to the liquid crystal ID printed directly into HEX format for easy viewing LCD driver IC.
//V1.3
// Added support for fast IO
// Modify the backlight control polarity (for V1.8 and later development board version)
// For the previous version 1.8 (not including 1.8) LCD modules, please modify LCD_Init function LCD_LED=1; is LCD_LED=1;
//V1.4
// Modified LCD_ShowChar function, use the function draw point Videos characters.
// Adds support for horizontal and vertical screen display
//V1.5 20110730
//1, modify the B505 read color LCD incorrect bug.
//2, modify the fast IO and horizontal and vertical screen set up.
//V1.6 20111116
//1, adding the liquid crystal driver support for LGDP4535
//V1.7 20120713
//1, increase LCD_RD_DATA function
//2, adding support for the ILI9341
//3, the increase in independent driver code ILI9325
//4, increase LCD_Scan_Dir function (use caution)
//6, in addition to modify some of the original function, to accommodate the 9341 operation
//V1.8 20120905
//1, added LCD important parameter settings structural body lcddev
//2, Add LCD_Display_Dir function, switch the screen anyway support online
//V1.9 20120911
//1, the new RM68042 drive (ID: 6804), but the 6804 does not support cross-screen display! . Reason: change the scanning mode,
// 6804 led to the failure to set the coordinates, tried many methods will not work, temporarily no solution.
//V2.0 20120924
// Without hardware reset, ILI9341 ID reader will be misread as 9300, modify LCD_Init, will not be recognized
// The situation (read ID 9300 / illegal ID), force the designated driver IC is ILI9341, perform the initialization 9341.
//V2.1 20120930
// Read color correction ILI9325 bug.
//V2.2 20121007
// Correction LCD_Scan_Dir the bug.
//V2.3 20130120
// Add 6804 to support cross-screen display
//V2.4 20131120
//1, the new NT35310 (ID: 5310) drive support
//2, the new LCD_Set_Window function is used to set the window for fast filling, more useful, but the function when the horizontal screen, 6804 is not supported.
//V2.5 20140211
//1, the new NT 35510 (ID: 5510) drive support
//V2.6 20140504
//1, the new 24 * Support ASCII 24 fonts (more fonts the user can add their own)
//2, modify some function parameters, to support MDK -O2 optimization
//3, for 9341/35310/35510, writing time is set to the fastest possible speed
//4, SSD1289 support was removed, because 1289 is too slow, the read cycle to 1us ... simply wonderful. F4 is not suitable for use
//5, bug correction such as IC 68042 and C505 color reading function.
//V2.7 20140710
//1. Fix LCD_Color_Fill function of a bug.
//2, a bug fix LCD_Scan_Dir function.
//V2.8 20140721
//1, when MDK solve using -O2 optimization LCD_ReadPoint function reads point failure.
//////////////////////////////////////////////////////////////////////////////////

// LCD brush color and background color
u16 POINT_COLOR=0x0000;	// Drawing pen color
u16 BACK_COLOR=0xFFFF;  // background color

// Management LCD important parameters
// Default to portrait
_lcd_dev lcddev;

// Write register function
//regval: register value
void LCD_WR_REG(vu16 regval)
{
	regval=regval;		// Use -O2 optimization, you must insert delay
	LCD->LCD_REG=regval;// write to write register number  
}
// Write LCD data
//data: value to be written
void LCD_WR_DATA(vu16 data)
{
	data=data;			// Use -O2 optimization, you must insert delay
	LCD->LCD_RAM=data;
}
// Read LCD data
// Return Value: Value read
u16 LCD_RD_DATA(void)
{
	vu16 ram;			// Prevent Optimization
	ram=LCD->LCD_RAM;
	return ram;
}
// Write register
//LCD_Reg: Register Address
//LCD_RegValue: data to be written
void LCD_WriteReg(vu16 LCD_Reg, vu16 LCD_RegValue)
{
	LCD->LCD_REG = LCD_Reg;		// Write to write register number  
	LCD->LCD_RAM = LCD_RegValue;// write data        
}
// Read register
//LCD_Reg: Register Address
// Return Value: read data
u16 LCD_ReadReg(vu16 LCD_Reg)
{
	LCD_WR_REG(LCD_Reg);		// Write the register number to be read
	delay_us(5);
	return LCD_RD_DATA();		// Return value read
}
// Start writing GRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;
}
//LCD write GRAM
//RGB_Code: color values
void LCD_WriteRAM(u16 RGB_Code)
{
	LCD->LCD_RAM = RGB_Code;// write sixteen GRAM
}
// Data is read out from the ILI93xx GBR format, and when we write to RGB format.
// This function by converting
//c: color values GBR format
// Return Value: RGB color value format
u16 LCD_BGR2RGB(u16 c)
{
	u16  r,g,b,rgb;
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;
	rgb=(b<<11)+(g<<5)+(r<<0);
	return(rgb);
}
// When mdk -O1 time optimization needs to be set
// Delay i
void opt_delay(u8 i)
{
	while(i--);
}
// Reads a color value of a point
//x,y: coordinates
// Return Value: The color of this point
u16 LCD_ReadPoint(u16 x,u16 y)
{
 	vu16 r=0,g=0,b=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	// Beyond the scope of direct return
	LCD_SetCursor(x,y);
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X2E);//9341/6804/3510 sends a read command GRAM
	else if(lcddev.id==0X5510)LCD_WR_REG (0X2E00);	// 5510 sends a read command GRAM
	else LCD_WR_REG(R34);      		 				// Other IC sends a read command GRAM
	if(lcddev.id==0X9320)opt_delay(2);				//FOR 9320, delay 2us
 	LCD_RD_DATA();									//dummy Read
	opt_delay(2);
 	r=LCD_RD_DATA();  		  						// Actual color coordinate
 	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)		//9341/NT35310/NT35510 to read 2 times
 	{
		opt_delay(2);
		b=LCD_RD_DATA();
		g=r&0XFF;		// For 9341/5310/5510, first read is the value of RG, R front, G in after each eight
		g<<=8;
	}
	if(lcddev.id==0X9325||lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0XB505||lcddev.id==0XC505)return r;	// This returns the color values of several IC directly
	else if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510 need to change if the formula
	else return LCD_BGR2RGB(r);						// Other IC
}
// LCD display is turned
void LCD_DisplayOn(void)
{
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X29);	// Turn on the display
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2900);	// Turn on the display
	else LCD_WriteReg(R7,0x0173); 				 	// Turn on the display
}
// LCD display is turned off
void LCD_DisplayOff(void)
{
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)LCD_WR_REG(0X28);	// Off the display
	else if(lcddev.id==0X5510)LCD_WR_REG(0X2800);	// Turn off the display
	else LCD_WriteReg(R7,0x0);// close the display
}
// Set the cursor position
//Xpos: abscissa
//Ypos: ordinate
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
 	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(Xpos>>8);
		LCD_WR_DATA(Xpos&0XFF);
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(Ypos>>8);
		LCD_WR_DATA(Ypos&0XFF);
	}else if(lcddev.id==0X6804)
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;// handle horizontal screen when
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(Xpos>>8);
		LCD_WR_DATA(Xpos&0XFF);
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(Ypos>>8);
		LCD_WR_DATA(Ypos&0XFF);
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(Xpos>>8);
		LCD_WR_REG(lcddev.setxcmd+1);
		LCD_WR_DATA(Xpos&0XFF);
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(Ypos>>8);
		LCD_WR_REG(lcddev.setycmd+1);
		LCD_WR_DATA(Ypos&0XFF);
	}else
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;// horizontal screen is actually turned the x, y coordinates
		LCD_WriteReg(lcddev.setxcmd, Xpos);
		LCD_WriteReg(lcddev.setycmd, Ypos);
	}
}
// Set up automatic scanning direction of the LCD
// NOTE: Additional functions may be affected (especially in 9341/6804 these two wonderful) this function set,
// So, generally set L2R_U2D can, if you set the scan mode to another may result in the display is not normal.
//dir:0~7, representatives of eight directions (specifically defined lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341/5310/5510 etc. IC has actually tested
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp;
	u16 xsize,ysize;
	if(lcddev.dir==1&&lcddev.id!=0X6804)// horizontal screen, without changing the scanning direction of 6804!
	{
		switch(dir)// direction change
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;
		}
	}
	if(lcddev.id==0x9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X5510)//9341/6804/5310/5510, very special
	{
		switch(dir)
		{
			case L2R_U2D:// from left to right, top to bottom
				regval|=(0<<7)|(0<<6)|(0<<5);
				break;
			case L2R_D2U:// from left to right, from bottom to top
				regval|=(1<<7)|(0<<6)|(0<<5);
				break;
			case R2L_U2D:// from right to left, top to bottom
				regval|=(0<<7)|(1<<6)|(0<<5);
				break;
			case R2L_D2U:// from right to left, from bottom to top
				regval|=(1<<7)|(1<<6)|(0<<5);
				break;
			case U2D_L2R:// top to bottom, left to right
				regval|=(0<<7)|(0<<6)|(1<<5);
				break;
			case U2D_R2L:// top to bottom, right to left
				regval|=(0<<7)|(1<<6)|(1<<5);
				break;
			case D2U_L2R:// from bottom to top, from left to right
				regval|=(1<<7)|(0<<6)|(1<<5);
				break;
			case D2U_R2L:// from bottom to top, right to left
				regval|=(1<<7)|(1<<6)|(1<<5);
				break;
		}
		if(lcddev.id==0X5510)dirreg=0X3600;
		else dirreg=0X36;
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510))regval|=0X08;//5310/5510 do not need to BGR
		if(lcddev.id==0X6804)regval|=0x02;// 6804 of the 9341's and anti BIT6
		LCD_WriteReg(dirreg,regval);
 		if((regval&0X20)||lcddev.dir==1)
		{
			if(lcddev.width<lcddev.height)// swap X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}else
		{
			if(lcddev.width>lcddev.height)// swap X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}
		if(lcddev.dir==1&&lcddev.id!=0X6804)
		{
			xsize=lcddev.width;
			ysize=lcddev.height;
		}else
		{
			xsize=lcddev.width;
			ysize=lcddev.height;
		}
		if(lcddev.id==0X5510)
		{
			LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(0);
			LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(0);
			LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA((xsize-1)>>8);
			LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA((xsize-1)&0XFF);
			LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(0);
			LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(0);
			LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA((ysize-1)>>8);
			LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA((ysize-1)&0XFF);
		}else
		{
			LCD_WR_REG(lcddev.setxcmd);
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((xsize-1)>>8);LCD_WR_DATA((xsize-1)&0XFF);
			LCD_WR_REG(lcddev.setycmd);
			LCD_WR_DATA(0);LCD_WR_DATA(0);
			LCD_WR_DATA((ysize-1)>>8);LCD_WR_DATA((ysize-1)&0XFF);
		}
  	}else
	{
		switch(dir)
		{
			case L2R_U2D:// from left to right, top to bottom
				regval|=(1<<5)|(1<<4)|(0<<3);
				break;
			case L2R_D2U:// from left to right, from bottom to top
				regval|=(0<<5)|(1<<4)|(0<<3);
				break;
			case R2L_U2D:// from right to left, top to bottom
				regval|=(1<<5)|(0<<4)|(0<<3);
				break;
			case R2L_D2U:// from right to left, from bottom to top
				regval|=(0<<5)|(0<<4)|(0<<3);
				break;
			case U2D_L2R:// top to bottom, left to right
				regval|=(1<<5)|(1<<4)|(1<<3);
				break;
			case U2D_R2L:// top to bottom, right to left
				regval|=(1<<5)|(0<<4)|(1<<3);
				break;
			case D2U_L2R:// from bottom to top, from left to right
				regval|=(0<<5)|(1<<4)|(1<<3);
				break;
			case D2U_R2L:// from bottom to top, right to left
				regval|=(0<<5)|(0<<4)|(1<<3);
				break;
		}
		if(lcddev.id==0x8989)//8989 IC
		{
			dirreg=0X11;
			regval|=0X6040;	//65K
	 	}else// other driver IC
		{
			dirreg=0X03;
			regval|=1<<12;
		}
		LCD_WriteReg(dirreg,regval);
	}
}
// Draw points
//x,y: coordinates
//POINT_COLOR: the color of this point
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		// Set the cursor position
	LCD_WriteRAM_Prepare();	// Start writing GRAM
	LCD->LCD_RAM=POINT_COLOR;
}
// Draw the point fast
//x,y: coordinates
//color: color
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
{
	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(x>>8);
		LCD_WR_DATA(x&0XFF);
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(y>>8);
		LCD_WR_DATA(y&0XFF);
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(x>>8);
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(x&0XFF);
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(y>>8);
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(y&0XFF);
	}else if(lcddev.id==0X6804)
	{
		if(lcddev.dir==1)x=lcddev.width-1-x;// the horizontal screen treatment
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(x>>8);
		LCD_WR_DATA(x&0XFF);
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(y>>8);
		LCD_WR_DATA(y&0XFF);
	}else
	{
 		if(lcddev.dir==1)x=lcddev.width-1-x;// horizontal screen is actually turned the x, y coordinates
		LCD_WriteReg(lcddev.setxcmd,x);
		LCD_WriteReg(lcddev.setycmd,y);
	}
	LCD->LCD_REG=lcddev.wramcmd;
	LCD->LCD_RAM=color;
}


// Set the LCD display direction
//dir:0, vertical screen; 1, horizontal screen
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			// Vertical screen
	{
		lcddev.dir=0;	// Vertical screen
		lcddev.width=240;
		lcddev.height=320;
		if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;
			if(lcddev.id==0X6804||lcddev.id==0X5310)
			{
				lcddev.width=320;
				lcddev.height=480;
			}
		}else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00;
			lcddev.width=480;
			lcddev.height=800;
		}else
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=R32;
			lcddev.setycmd=R33;
		}
	}else 				// Horizontal screen
	{
		lcddev.dir=1;	// Horizontal screen
		lcddev.width=320;
		lcddev.height=240;
		if(lcddev.id==0X9341||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;
		}else if(lcddev.id==0X6804)
		{
 			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2B;
			lcddev.setycmd=0X2A;
		}else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00;
			lcddev.width=800;
			lcddev.height=480;
		}else
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=R33;
			lcddev.setycmd=R32;
		}
		if(lcddev.id==0X6804||lcddev.id==0X5310)
		{
			lcddev.width=480;
			lcddev.height=320;
		}
	}
	LCD_Scan_Dir(DFT_SCAN_DIR);	// Default scan direction
}
// Set the window, and automatically sets the upper left corner of the window to draw point coordinates (sx,sy).
//sx,sy: window start coordinate (upper left corner)
//width,height: width and height of the window, must be greater than 0!!
// Form size:width*height.
//68042, does not support horizontal screen window settings!!
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{
	u8 hsareg,heareg,vsareg,veareg;
	u16 hsaval,heaval,vsaval,veaval;
	width=sx+width-1;
	height=sy+height-1;
	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X6804)//6804 does not support horizontal screen
	{
		LCD_WR_REG(lcddev.setxcmd);
		LCD_WR_DATA(sx>>8);
		LCD_WR_DATA(sx&0XFF);
		LCD_WR_DATA(width>>8);
		LCD_WR_DATA(width&0XFF);
		LCD_WR_REG(lcddev.setycmd);
		LCD_WR_DATA(sy>>8);
		LCD_WR_DATA(sy&0XFF);
		LCD_WR_DATA(height>>8);
		LCD_WR_DATA(height&0XFF);
	}else if(lcddev.id==0X5510)
	{
		LCD_WR_REG(lcddev.setxcmd);LCD_WR_DATA(sx>>8);
		LCD_WR_REG(lcddev.setxcmd+1);LCD_WR_DATA(sx&0XFF);
		LCD_WR_REG(lcddev.setxcmd+2);LCD_WR_DATA(width>>8);
		LCD_WR_REG(lcddev.setxcmd+3);LCD_WR_DATA(width&0XFF);
		LCD_WR_REG(lcddev.setycmd);LCD_WR_DATA(sy>>8);
		LCD_WR_REG(lcddev.setycmd+1);LCD_WR_DATA(sy&0XFF);
		LCD_WR_REG(lcddev.setycmd+2);LCD_WR_DATA(height>>8);
		LCD_WR_REG(lcddev.setycmd+3);LCD_WR_DATA(height&0XFF);
	}else	// Other driver IC
	{
		if(lcddev.dir==1)// horizontal screen
		{
			// Window value
			hsaval=sy;
			heaval=height;
			vsaval=lcddev.width-width-1;
			veaval=lcddev.width-sx-1;
		}else
		{
			hsaval=sx;
			heaval=width;
			vsaval=sy;
			veaval=height;
		}
		hsareg=0X50;heareg=0X51;// horizontal window register
		vsareg=0X52;veareg=0X53;// vertical window register
		// Set the register values
		LCD_WriteReg(hsareg,hsaval);
		LCD_WriteReg(heareg,heaval);
		LCD_WriteReg(vsareg,vsaval);
		LCD_WriteReg(veareg,veaval);
		LCD_SetCursor(sx,sy);	// Set the cursor position
	}
}
// Initialize lcd
// This initialization function can initialize the various ILI93XX LCD, but the other function is based ILI9320!!!
// Not been tested on other types of driver chip!
void LCD_Init(void)
{
	vu32 i=0;

  GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);// Enable IO clock
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);// Enable FSMC clock


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;// PB1 push-pull output, control the backlight
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;// normal output mode
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;// push-pull output
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;// Pull up
  GPIO_Init(GPIOB, &GPIO_InitStructure);// initialization // PB15 push-pull output, control the backlight

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_8
																|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PD0,1,4,5,8,9,10,14,15 AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;// alternate output
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;// push-pull output
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;// Pull up
  GPIO_Init(GPIOD, &GPIO_InitStructure);// initialize

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12
																|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PE7~15,AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;// alternate output
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;// push-pull output
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;// Pull up
  GPIO_Init(GPIOE, &GPIO_InitStructure);// initialize

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//PF12,FSMC_A18
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;// alternate output
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;// push-pull output
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;// pull up
//  GPIO_Init(GPIOD, &GPIO_InitStructure);// initialize

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//PF12,FSMC_A6
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;// alternate output
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;// push-pull output
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;// pull up
//  GPIO_Init(GPIOG, &GPIO_InitStructure);// initialize

  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);//
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);//
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC); // and ZET6 chip difference
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_FSMC);// and ZET6 chip difference
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);//

  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);//PE7,AF12
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);//PE15,AF12

//  GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC);//PF12,AF12
//  GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC);


  readWriteTiming.FSMC_AddressSetupTime = 0XF;	 // Address setup time (ADDSET) 16 HCLK 1/168M=6ns*16=96ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;	 // Address hold time (ADDHLD) mode A unused
  readWriteTiming.FSMC_DataSetupTime = 60;			// Data retention time of 60 HCLK	=6*60=360ns
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 // Mode A


	writeTiming.FSMC_AddressSetupTime =5;	      // Address setup time (ADDSET) for the nine HCLK =54ns
  writeTiming.FSMC_AddressHoldTime = 0x00;	 // Address hold time (A
  writeTiming.FSMC_DataSetupTime = 5;		 // Data retention time 6ns *9 Ge HCLK =54ns
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 // Mode A


  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;// here we use NE1
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // not address the multiplexed data
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;// width 16bit memory data
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  Write enable memory
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // read and write using different timing
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; // read and write timing
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  // write timing

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  // initialize FSMC configuration

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  // Enable BANK1

 	delay_ms(50); // delay 50 ms
 	LCD_WriteReg(0x0000,0x0001);
	delay_ms(50); // delay 50 ms
  lcddev.id = LCD_ReadReg(0x0000);
  	if(lcddev.id<0XFF||lcddev.id==0XFFFF||lcddev.id==0X9300)// read ID is not correct, the new lcddev.id==0X9300 judgment, because in 9341 has not been reset It will be read into the case of 9300
	{
 		// Try to read the 9341 ID
		LCD_WR_REG(0XD3);
		lcddev.id=LCD_RD_DATA();	//dummy read
 		lcddev.id=LCD_RD_DATA(); 	// Read 0X00
  		lcddev.id=LCD_RD_DATA();   	// Read 93
 		lcddev.id<<=8;
		lcddev.id|=LCD_RD_DATA();  	// Read 41
 		if(lcddev.id!=0X9341)		// 9341 Non-try is not 6804
		{
 			LCD_WR_REG(0XBF);
			lcddev.id=LCD_RD_DATA();//dummy read
	 		lcddev.id=LCD_RD_DATA();// read back 0X01
	 		lcddev.id=LCD_RD_DATA();// read back 0XD0
	  		lcddev.id=LCD_RD_DATA();// read back here 0X68
			lcddev.id<<=8;
	  		lcddev.id|=LCD_RD_DATA();// read back here 0X04
			if(lcddev.id!=0X6804)	// Nor is the 6804, try to see is not NT35310
			{
				LCD_WR_REG(0XD4);
				lcddev.id=LCD_RD_DATA();	//dummy read
				lcddev.id=LCD_RD_DATA();	// Read back 0X01
				lcddev.id=LCD_RD_DATA();	// Read back 0X53
				lcddev.id<<=8;
				lcddev.id|=LCD_RD_DATA();	// Read back here 0X10
				if(lcddev.id!=0X5310)		// Nor NT35310, try to see if it is NT35510
				{
					LCD_WR_REG(0XDA00);
					lcddev.id=LCD_RD_DATA();// read back 0X00
					LCD_WR_REG(0XDB00);
					lcddev.id=LCD_RD_DATA();// read back 0X80
					lcddev.id<<=8;
					LCD_WR_REG(0XDC00);
					lcddev.id|=LCD_RD_DATA();// read back 0X00
					if(lcddev.id==0x8000)lcddev.id=0x5510;// NT35510 read back ID is 8000H, for the convenience of distinction, we are forced to 5510
				}
			}
 		}
	}
	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)// If the three IC, then set the fastest timing WR
	{
		// Reconfiguration control register write timing timing
		FSMC_Bank1E->BWTR[6]&=~(0XF<<0);// address setup time (ADDSET) cleared
		FSMC_Bank1E->BWTR[6]&=~(0XF<<8);// data retention time is cleared
		FSMC_Bank1E->BWTR[6]|=3<<0;		// Address setup time (ADDSET) for the three HCLK =18ns
		FSMC_Bank1E->BWTR[6]|=2<<8; 	// Data retention time 6ns * 3 Ge HCLK =18ns
	}else if(lcddev.id==0X6804||lcddev.id==0XC505)	//6804/C505 speed falters, was reduced
	{
		// Reconfiguration control register write timing timing
		FSMC_Bank1E->BWTR[6]&=~(0XF<<0);// address setup time (ADDSET) cleared
		FSMC_Bank1E->BWTR[6]&=~(0XF<<8);// data retention time is cleared
		FSMC_Bank1E->BWTR[6]|=10<<0;	// Address setup time (ADDSET) for the 10 HCLK =60ns
		FSMC_Bank1E->BWTR[6]|=12<<8; 	// Data retention time 6ns * 13 Ge HCLK =96ns
	}
//	printf(" LCD ID:%x\r\n",lcddev.id); // print LCD ID
	if(lcddev.id==0X9341)	// 9341 initialize
	{
		LCD_WR_REG(0xCF);
		LCD_WR_DATA(0x00);
		i++;
		LCD_WR_DATA(0xC1);
		LCD_WR_DATA(0X30);
		LCD_WR_REG(0xED);
		LCD_WR_DATA(0x64);
		LCD_WR_DATA(0x03);
		LCD_WR_DATA(0X12);
		LCD_WR_DATA(0X81);
		LCD_WR_REG(0xE8);
		LCD_WR_DATA(0x85);
		LCD_WR_DATA(0x10);
		LCD_WR_DATA(0x7A);
		LCD_WR_REG(0xCB);
		LCD_WR_DATA(0x39);
		LCD_WR_DATA(0x2C);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x34);
		LCD_WR_DATA(0x02);
		LCD_WR_REG(0xF7);
		LCD_WR_DATA(0x20);
		LCD_WR_REG(0xEA);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_REG(0xC0);    //Power control
		LCD_WR_DATA(0x1B);   //VRH[5:0]
		LCD_WR_REG(0xC1);    //Power control
		LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0]
		LCD_WR_REG(0xC5);    //VCM control
		LCD_WR_DATA(0x30); 	 //3F
		LCD_WR_DATA(0x30); 	 //3C
		LCD_WR_REG(0xC7);    //VCM control2
		LCD_WR_DATA(0XB7);
		LCD_WR_REG(0x36);    // Memory Access Control
		LCD_WR_DATA(0x48);
		LCD_WR_REG(0x3A);
		LCD_WR_DATA(0x55);
		LCD_WR_REG(0xB1);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x1A);
		LCD_WR_REG(0xB6);    // Display Function Control
		LCD_WR_DATA(0x0A);
		LCD_WR_DATA(0xA2);
		LCD_WR_REG(0xF2);    // 3Gamma Function Disable
		LCD_WR_DATA(0x00);
		LCD_WR_REG(0x26);    //Gamma curve selected
		LCD_WR_DATA(0x01);
		LCD_WR_REG(0xE0);    //Set Gamma
		LCD_WR_DATA(0x0F);
		LCD_WR_DATA(0x2A);
		LCD_WR_DATA(0x28);
		LCD_WR_DATA(0x08);
		LCD_WR_DATA(0x0E);
		LCD_WR_DATA(0x08);
		LCD_WR_DATA(0x54);
		LCD_WR_DATA(0XA9);
		LCD_WR_DATA(0x43);
		LCD_WR_DATA(0x0A);
		LCD_WR_DATA(0x0F);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_REG(0XE1);    //Set Gamma
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x15);
		LCD_WR_DATA(0x17);
		LCD_WR_DATA(0x07);
		LCD_WR_DATA(0x11);
		LCD_WR_DATA(0x06);
		LCD_WR_DATA(0x2B);
		LCD_WR_DATA(0x56);
		LCD_WR_DATA(0x3C);
		LCD_WR_DATA(0x05);
		LCD_WR_DATA(0x10);
		LCD_WR_DATA(0x0F);
		LCD_WR_DATA(0x3F);
		LCD_WR_DATA(0x3F);
		LCD_WR_DATA(0x0F);
		LCD_WR_REG(0x2B);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x3f);
		LCD_WR_REG(0x2A);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xef);
		LCD_WR_REG(0x11); //Exit Sleep
		delay_ms(120);
		LCD_WR_REG(0x29); //display on
	}else if(lcddev.id==0x6804) // 6804 initialization
	{
		LCD_WR_REG(0X11);
 	}else if(lcddev.id==0x5310)
	{
		LCD_WR_REG(0xED);
	}else if(lcddev.id==0x5510)
	{
		LCD_WriteReg(0xF000,0x55);
	}else if(lcddev.id==0x9325)//9325
	{
		LCD_WriteReg(0x00E5,0x78F0);
	}else if(lcddev.id==0x9328)//ILI9328   OK
	{
  		LCD_WriteReg(0x00EC,0x108F);// internal timeing
	}else if(lcddev.id==0x9320)// test OK.
	{
		LCD_WriteReg(0x00,0x0000);
	}else if(lcddev.id==0X9331)//OK |/|/|
	{
		LCD_WriteReg(0x00E7, 0x1014);
	}else if(lcddev.id==0x5408)
	{
		LCD_WriteReg(0x01,0x0100);
	}
	else if(lcddev.id==0x1505)//OK
	{
        LCD_WriteReg(0x0007,0x0000);
	}else if(lcddev.id==0xB505)
	{
		LCD_WriteReg(0x0000,0x0000);
	}else if(lcddev.id==0xC505)
	{
		LCD_WriteReg(0x0000,0x0000);
	}else if(lcddev.id==0x4531)//OK |/|/|
	{
		LCD_WriteReg(0X00,0X0001);
	}else if(lcddev.id==0x4535)
	{
		LCD_WriteReg(0X15,0X0030);
	}
	LCD_Display_Dir(1);		 	// default to portrait
//	LCD_LED=1;					// lit backlight
	LCD_Clear(GREEN);
}
// Clear screen function
//color: To clear the screen fill color
u32 LCDClearTick;
void LCD_Clear(u16 color)
{
    // get start time
    u32 t0 = DWT_Get_Current_Tick();

	u32 index=0;
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			// get the total number of points
	if((lcddev.id==0X6804)&&(lcddev.dir==1))// 6804 horizontal screen when special treatment
	{
 		lcddev.dir=0;
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;
		LCD_SetCursor(0x00,0x0000);		// set the cursor position
 		lcddev.dir=1;
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;
 	}else LCD_SetCursor(0x00,0x0000);	// set the cursor position
	LCD_WriteRAM_Prepare();     		// start writing GRAM
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;
	}

    // count time for one circle
	LCDClearTick = DWT_Elapsed_Tick(t0);
	POINT_COLOR = YELLOW;
    LCD_ShowxNum(100,227, LCDClearTick/168, 8,12, 9);
}
// Fill a single color in the designated area
//(sx,sy),(ex,ey): filled rectangle coordinates diagonal , area size:(ex-sx+1)*(ey-sy+1)
//color: To fill color
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{
	u16 i,j;
	u16 xlen=0;
	u16 temp;
	if((lcddev.id==0X6804)&&(lcddev.dir==1))	// 6804 horizontal screen when special treatment
	{
		temp=sx;
		sx=sy;
		sy=lcddev.width-ex-1;
		ex=ey;
		ey=lcddev.width-temp-1;
 		lcddev.dir=0;
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;
		LCD_Fill(sx,sy,ex,ey,color);
 		lcddev.dir=1;
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;
 	}else
	{
		xlen=ex-sx+1;
		for(i=sy;i<=ey;i++)
		{
		 	LCD_SetCursor(sx,i);      				// set the cursor position
			LCD_WriteRAM_Prepare();     			// start writing GRAM
			for(j=0;j<xlen;j++)LCD->LCD_RAM=color;	// display colors
		}
	}
}
// In the designated area to fill the specified color block
//(sx,sy),(ex,ey): filled rectangle coordinates diagonal, area size:(ex-sx+1)*(ey-sy+1)
//color: To fill color
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{
	u16 height,width;
	u16 i,j;
	width=ex-sx+1; 			// get filled width
	height=ey-sy+1;			// height
 	for(i=0;i<height;i++)
	{
 		LCD_SetCursor(sx,sy+i);   	// set the cursor position
		LCD_WriteRAM_Prepare();     // start writing GRAM
		for(j=0;j<width;j++)LCD->LCD_RAM=color[i*width+j];// write data
	}
}
// Draw a line
//x1,y1: starting point coordinates
//x2,y2: end coordinates
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; // calculate the coordinates increment
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1; // set the single-step directions
	else if(delta_x==0)incx=0;// vertical line
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;// horizontal
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; // Select the basic incremental axis
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )// draw a line output
	{
		LCD_DrawPoint(uRow,uCol);// draw points
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}
// Draw a rectangle
//(x1,y1),(x2,y2): rectangle coordinates diagonal
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
// A circle the size of the appointed position Videos
//(x,y): the center
//r    : Radius
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;
	di=3-(r<<1);             // determine the next point position sign
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b);             //5
 		LCD_DrawPoint(x0+b,y0-a);             //0
		LCD_DrawPoint(x0+b,y0+a);             //4
		LCD_DrawPoint(x0+a,y0+b);             //6
		LCD_DrawPoint(x0-a,y0+b);             //1
 		LCD_DrawPoint(x0-b,y0+a);
		LCD_DrawPoint(x0-a,y0-b);             //2
  		LCD_DrawPoint(x0-b,y0-a);             //7
		a++;
		// Use Bresenham algorithm Circle
		if(di<0)di +=4*a+6;
		else
		{
			di+=10+4*(a-b);
			b--;
		}
	}
}
// Display a character in the specified location
//x,y: Start coordinates
//num:characters to be displayed:" "--->"~"
//size: Font size 12/16/24
//mode: the superposition mode (1) or non-overlapping mode (0)
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		// get a font character set corresponding to the number of bytes occupied by a dot
	// Setup Window
	num=num-' ';// values obtained after offset
	for(t=0;t<csize;t++)
	{
		if(size==12)temp=asc2_1206[num][t]; 	 	// call 1206 font
		else if(size==16)temp=asc2_1608[num][t];	// call 1608 font
		else if(size==24)temp=asc2_2412[num][t];	// call 2412 font
		else return;								// no fonts
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		// over the region
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	// over the region
				break;
			}
		}
	}
}
//m^n function
// Return value:m^n-th power.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)result*=m;
	return result;
}
// Show figures, the high is 0, no display
//x,y : the starting point coordinates
//len : Digits
//size: Font Size
//color: color
//num: Numerical(0~4294967295);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1;

		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0);
	}
}
// Show figures, the high is 0, or show
//x,y: the starting point coordinates
//num: Numerical (0~999999999);
//len: length (ie the number of digits to be displayed)
//size: Font Size
//mode:
//[7]:0, no padding;1, filled with 0.
//[6:1]: Reserved
//[0]:0, non-superimposition display;1, superimposed display.
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);
 				continue;
			}else enshow=1;

		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01);
	}
}
// Display string
//x,y: the starting point coordinates
//width,height: size of the area
//size: Font Size
//*p: string starting address
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,const char *p)
{
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))// judgment is not illegal characters!
    {
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//Exit
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }
}
