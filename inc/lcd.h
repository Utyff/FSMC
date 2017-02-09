#ifndef __LCD_H
#define __LCD_H
#include "stm32f4xx.h"
//#include "sys.h"
//#include "stdlib.h"
//////////////////////////////////////////////////////////////////////////////////
// STM32F4 Project Template - library function version
// Taobao shop: http://mcudev.taobao.com
//********************************************************************************
//V1.2 modify the description
// Support SPFD5408 driver, in addition to the LCD ID printed directly into HEX format. Easy to view LCD driver IC.
//V1.3
// Added support for fast IO
// Modified the polarity of the backlight control (for V1.8 and later development board version)
// For LCD modules prior to 1.8 (not including 1.8), modify LCD_LED = 1 for LCD_Init function; for LCD_LED =1;
//V1.4
// modify the LCD_ShowChar function, use the draw function to draw characters.
// Added support for horizontal and vertical screen display
//V1.5 20110730
//1, modified the B505 LCD read wrong color bug.
//2, modify the fast IO and horizontal and vertical screen settings.
//V1.6 20111116
//1, joined the LGDP4535 LCD driver support
//V1.7 20120713
//1, increase LCD_RD_DATA function
//2, increase the support of ILI9341
//3, increase the ILI9325 independent driver code
//4, increase LCD_Scan_Dir function (use caution)
//6, also modified part of the original function to adapt to the operation of 9341
//V1.8 20120905
//1, add LCD important parameter setting structure lcddev
//2, by adding LCD_Display_Dir function, support for online vertical and horizontal screen switch
//V1.9 20120911
//1, the new RM68042 driver (ID: 6804), but the 6804 does not support horizontal display! . Cause: Changing the scan mode,
// 6804 coordinates set to invalidate, tried a lot of methods are not, for the time being no solution.
//V2.0 20120924
// In the case of no hardware reset, ILI9341's ID reading will be misread as 9300, modify LCD_Init, will not be recognized
// (Read ID 9300 / illegal ID), forcing the specified driver IC to ILI9341, the implementation of 9341 initialization.
//V2.1 20120930
// Fix ILI9325 bug reading color.
//V2.2 20121007
// Fix bug for LCD_Scan_Dir.
//V2.3 20130120
// Added 6804 support for horizontal display
//V2.4 20131120
//1, the new NT35310 (ID: 5310) drive support
//2, the new LCD_Set_Window function, used to set the window, the fast fill, more useful, but the function in the horizontal screen, does not support 6804.
//V2.5 20140211
//1, the new NT35510 (ID: 5510) drive support
//V2.6 20140504
//1, the new ASCII 24* 24 font support (more fonts users can add their own)
//2, modify some function parameters to support MDK-O2 optimization
//3, for 9341/35310/35510, write time set to the fastest, as much as possible to improve the speed
//4, removed the support of SSD1289, because 1289 is too slow, the read cycle to 1us ... really wonderful. Not suitable for F4 use
//5, amended 68042 and C505 IC read color function of the bug.
//V2.7 20140514
//1, fix LCD_Color_Fill function of a bug.
//////////////////////////////////////////////////////////////////////////////////

// LCD important parameter set
typedef struct
{
	u16 width;			//LCD width
	u16 height;			//LCD height
	u16 id;				//LCD ID
	u8  dir;			// horizontal screen or vertical screen control: 0, vertical screen; 1, horizontal screen.
	u16	wramcmd;		// began to write the gram command
	u16  setxcmd;		// set the x coordinate instruction
	u16  setycmd;		// Set the y coordinate instruction
}_lcd_dev;

// LCD parameters
extern _lcd_dev lcddev;	// management LCD important parameters
// The brush color and background color of the LCD
extern u16  POINT_COLOR;// Default red
extern u16  BACK_COLOR; // Background color. Default is white


//////////////////////////////////////////////////////////////////////////////////
//----------------- LCD port definition ----------------
//#define	LCD_LED PBout(1)  		// LCD Backlight PB1
// LCD address structure
typedef struct
{
	u16 LCD_REG;
	u16 LCD_RAM;
} LCD_TypeDef;
// use Bank1.sector1 of NOR / SRAM, address bits HADDR [27,26]=00   A18 as command line for data command
// Note that the STM32 will shift to the right one bit when set!
#define LCD_BASE        ((u32)(0x60000000 | 0x00007FFFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
//////////////////////////////////////////////////////////////////////////////////

// Scan direction definition
#define L2R_U2D  0 // From left to right, top to bottom
#define L2R_D2U  1 // from left to right, from bottom to top
#define R2L_U2D  2 // Right to left, top to bottom
#define R2L_D2U  3 // Right to left, bottom to top

#define U2D_L2R  4 // from top to bottom, left to right
#define U2D_R2L  5 // from top to bottom, right to left
#define D2U_L2R  6 // From bottom to top, left to right
#define D2U_R2L  7 // From bottom to top, right to left

#define DFT_SCAN_DIR  L2R_U2D  // The default scan direction

// Brush color
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000
#define BLUE         	 0x001F
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 // Brown
#define BRRED 			 0XFC07 // reddish brown
#define GRAY  			 0X8430 // Gray
// GUI color

#define DARKBLUE      	 0X01CF	// dark blue
#define LIGHTBLUE      	 0X7D7C	// light blue
#define GRAYBLUE       	 0X5458 // gray-blue
// The above three colors are the color of PANEL

#define LIGHTGREEN     	 0X841F // Light green
//#define LIGHTGRAY        0XEF5B // light gray (PANNEL)
#define LGRAY 			 0XC618 // light gray (PANNEL), the form background color

#define LGRAYBLUE        0XA651 // light gray-blue (middle layer color)
#define LBBLUE           0X2B12 // light brown (select the reverse color of the entry)

#ifdef __cplusplus
 extern "C" {
#endif

void LCD_Init(void);													   	// Initialize
void LCD_DisplayOn(void);													// On to display
void LCD_DisplayOff(void);													// Off Display
void LCD_Clear(u16 Color);	 												// Clear the screen
void LCD_SetCursor(u16 Xpos, u16 Ypos);										// Set the cursor
void LCD_DrawPoint(u16 x,u16 y);											// Draw the points
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);								// Quickly draw points
u16  LCD_ReadPoint(u16 x,u16 y); 											// Read the point
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);										// Draw a circle
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);							// Draw lines
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   				// Draw the rectangle
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		   				// fill the monochrome
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);				// fill in the specified color
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);						// display a character
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  						// display a number
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				// display numbers
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);		// display a string,12/16 font

void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
void LCD_Scan_Dir(u8 dir);							// Set the screen scan direction
void LCD_Display_Dir(u8 dir);						// set the screen display direction
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);// Set the window

#ifdef __cplusplus
 }
#endif

//9320/9325 LCD registers
#define R0             0x00
#define R1             0x01
#define R2             0x02
#define R3             0x03
#define R4             0x04
#define R5             0x05
#define R6             0x06
#define R7             0x07
#define R8             0x08
#define R9             0x09
#define R10            0x0A
#define R12            0x0C
#define R13            0x0D
#define R14            0x0E
#define R15            0x0F
#define R16            0x10
#define R17            0x11
#define R18            0x12
#define R19            0x13
#define R20            0x14
#define R21            0x15
#define R22            0x16
#define R23            0x17
#define R24            0x18
#define R25            0x19
#define R26            0x1A
#define R27            0x1B
#define R28            0x1C
#define R29            0x1D
#define R30            0x1E
#define R31            0x1F
#define R32            0x20
#define R33            0x21
#define R34            0x22
#define R36            0x24
#define R37            0x25
#define R40            0x28
#define R41            0x29
#define R43            0x2B
#define R45            0x2D
#define R48            0x30
#define R49            0x31
#define R50            0x32
#define R51            0x33
#define R52            0x34
#define R53            0x35
#define R54            0x36
#define R55            0x37
#define R56            0x38
#define R57            0x39
#define R59            0x3B
#define R60            0x3C
#define R61            0x3D
#define R62            0x3E
#define R63            0x3F
#define R64            0x40
#define R65            0x41
#define R66            0x42
#define R67            0x43
#define R68            0x44
#define R69            0x45
#define R70            0x46
#define R71            0x47
#define R72            0x48
#define R73            0x49
#define R74            0x4A
#define R75            0x4B
#define R76            0x4C
#define R77            0x4D
#define R78            0x4E
#define R79            0x4F
#define R80            0x50
#define R81            0x51
#define R82            0x52
#define R83            0x53
#define R96            0x60
#define R97            0x61
#define R106           0x6A
#define R118           0x76
#define R128           0x80
#define R129           0x81
#define R130           0x82
#define R131           0x83
#define R132           0x84
#define R133           0x85
#define R134           0x86
#define R135           0x87
#define R136           0x88
#define R137           0x89
#define R139           0x8B
#define R140           0x8C
#define R141           0x8D
#define R143           0x8F
#define R144           0x90
#define R145           0x91
#define R146           0x92
#define R147           0x93
#define R148           0x94
#define R149           0x95
#define R150           0x96
#define R151           0x97
#define R152           0x98
#define R153           0x99
#define R154           0x9A
#define R157           0x9D
#define R192           0xC0
#define R193           0xC1
#define R229           0xE5
#endif
