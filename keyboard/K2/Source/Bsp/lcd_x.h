/***************************************************************************************************
* FILE: lcd.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/1, by kevin
***************************************************************************************************/
#ifndef __LCD_X_H_
#define __LCD_X_H_

#include "stm32f2xx_hal.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define LCD_CMD_ADDR        0x64000000
#define LCD_DAT_ADDR        0x64000080

#define LCD_WR_CMD(x)       *((volatile uint16_t*)LCD_CMD_ADDR) = (x)
#define LCD_WR_RAM(x)       *((volatile uint16_t*)LCD_DAT_ADDR) = (x)
#define LCD_RD_RAM()        *((volatile uint16_t*)LCD_DAT_ADDR)



#define COLOR_RED           0xF800
#define COLOR_GREEN         0x07E0
#define COLOR_BLUE          0x001F




/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void LCD_Init(void);

void LCD_SetBacklight(uint16_t grade);

uint16_t LCD_ReadPoint(uint16_t x, uint16_t y);

void LCD_Clear(uint16_t color);

void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawLine(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);
void LCD_DrawRetangle(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);


#endif
/****************************************** END OF FILE *******************************************/
