/***************************************************************************************************
* FILE: lcd.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/1, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "bsp.h"
#include "lcd_x.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define LCD_DelayMs(n)  HAL_Delay(n)
/***************************************************************************************************
* VARIABLES
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void LCD_SetPosi(uint16_t x, uint16_t y);
void LCD_SetArea(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);


/***************************************************************************************************
* Description:  LCD寄存器初始化
***************************************************************************************************/
void LCD_Init(void)
{
    TIM14_Init();
    
    LCD_RST = 0;
    LCD_DelayMs(20);
    LCD_RST = 1;
    LCD_DelayMs(20);
    
    LCD_WR_CMD(0xE9);
    LCD_WR_RAM(0x20);
    
    LCD_WR_CMD(0x11);// Exit_sleep_mode
    LCD_DelayMs(20);
    
    LCD_WR_CMD(0xD1);
    LCD_WR_RAM(0x00);
    LCD_WR_RAM(0x6B);
    LCD_WR_RAM(0x15);
    
    LCD_WR_CMD(0xD0);
    LCD_WR_RAM(0x07);
    LCD_WR_RAM(0x05);
    LCD_WR_RAM(0x04);
    
    //反色显示
    //    LCD_WR_CMD(0x21);
    //不反色
    LCD_WR_CMD(0x20);
    
    LCD_WR_CMD(0x36);   //设置屏幕方向
//    LCD_WR_RAM(0x28);
    LCD_WR_RAM(0x00);   
    
    LCD_WR_CMD(0x3A);
    LCD_WR_RAM(0x55);
    
    LCD_WR_CMD(0xC1);
    LCD_WR_RAM(0x10);
    LCD_WR_RAM(0x10);
    LCD_WR_RAM(0x02);
    LCD_WR_RAM(0x02);
    
    LCD_WR_CMD(0xC0); //Set Default Gamma
    LCD_WR_RAM(0x00);
    LCD_WR_RAM(0x35);
    LCD_WR_RAM(0x00);
    LCD_WR_RAM(0x00);
    LCD_WR_RAM(0x01);
    LCD_WR_RAM(0x02);
    
    LCD_WR_CMD(0xC5); //Set frame rate
    LCD_WR_RAM(0x07); //0--96 4-72 7-60
    
    LCD_WR_CMD(0xD2); //power setting
    LCD_WR_RAM(0x01);
    LCD_WR_RAM(0x33);
    
    LCD_WR_CMD(0xC8); //Set Gamma
    LCD_WR_RAM(0x02);
    LCD_WR_RAM(0x77);
    LCD_WR_RAM(0x47);
    LCD_WR_RAM(0x09);
    LCD_WR_RAM(0x09);
    LCD_WR_RAM(0x00);
    LCD_WR_RAM(0x03);
    LCD_WR_RAM(0x00);
    LCD_WR_RAM(0x57);
    LCD_WR_RAM(0x50);
    LCD_WR_RAM(0x00);
    LCD_WR_RAM(0x10);
    LCD_WR_RAM(0x08);
    LCD_WR_RAM(0x80);
    LCD_WR_RAM(0x00);
    
    LCD_WR_CMD(0xEA); //Enable 3 Gamma
    LCD_WR_RAM(0x00); 
    
    LCD_WR_CMD(0x2A); //水平 0~399
    LCD_WR_RAM(0x00);
    LCD_WR_RAM(0x00);
//    LCD_WR_RAM(0x01);
//    LCD_WR_RAM(0x8f);
    LCD_WR_RAM(0x00);
    LCD_WR_RAM(0xEF);
    
    LCD_WR_CMD(0x2B); //垂直 0~239
    LCD_WR_RAM(0x00);
    LCD_WR_RAM(0x00);
//    LCD_WR_RAM(0x00);
//    LCD_WR_RAM(0xEF);
    LCD_WR_RAM(0x01);
    LCD_WR_RAM(0x8F);
    
    LCD_WR_CMD(0x29);//打开显示
    LCD_WR_CMD(0x2C);//开始写数据
    
    LCD_Clear(COLOR_BLUE);
    
}

/***************************************************************************************************
* Description:  LCD进入休眠模式
***************************************************************************************************/
void LCD_EnterStandby(void)
{
    LCD_WR_CMD(0x28);
    LCD_DelayMs(10);
    LCD_WR_CMD(0x10);
    LCD_DelayMs(25);
}

/***************************************************************************************************
* Description:  LCD退出休眠模式
***************************************************************************************************/
void LCD_ExitStandby (void)
{
    LCD_WR_CMD(0x11);// Exit Sleep/ Standby mode
    LCD_DelayMs(120); 
    LCD_WR_CMD(0x29);//DISPLAY ON
}

/***************************************************************************************************
* Description:  设置背光亮度
***************************************************************************************************/
void LCD_SetBacklight(uint16_t grade)
{
    TIM14->CCR1 = grade;
}


/***************************************************************************************************
* Description:  以指定颜色清屏
***************************************************************************************************/
void LCD_Clear(uint16_t color)
{
    int i=0;
    
    LCD_SetPosi(0, 0);
    
    LCD_WR_CMD(0x2C);   //开始写GRAM
    
    for(i=0; i<(400*240); i++)
    {
        LCD_WR_RAM(color);
    }
}

/***************************************************************************************************
* Description:  设置RAM坐标
***************************************************************************************************/
void LCD_SetPosi(uint16_t x, uint16_t y)
{
    LCD_WR_CMD(0x2A);               //水平
    LCD_WR_RAM((uint8_t)(x>>8));    //起始位置
    LCD_WR_RAM((uint8_t)x);         
    LCD_WR_RAM(0x01);               //结束位置
    LCD_WR_RAM(0x8F);
    
    LCD_WR_CMD(0x2B);               //垂直
    LCD_WR_RAM((uint8_t)(y>>8));    //起始位置
    LCD_WR_RAM((uint8_t)y);
    LCD_WR_RAM(0x00);               //结束位置
    LCD_WR_RAM(0xEF);
}

/***************************************************************************************************
* Description:  设置写入区域
***************************************************************************************************/
void LCD_SetArea(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
    LCD_WR_CMD(0x2A);               //水平
    LCD_WR_RAM((uint8_t)(sx>>8));   //起始位置
    LCD_WR_RAM((uint8_t)sx);         
    LCD_WR_RAM((uint8_t)(ex>>8));   //结束位置
    LCD_WR_RAM((uint8_t)ex);
    
    LCD_WR_CMD(0x2B);               //垂直
    LCD_WR_RAM((uint8_t)(sy>>8));   //起始位置
    LCD_WR_RAM((uint8_t)sy);
    LCD_WR_RAM((uint8_t)(ey>>8));   //结束位置
    LCD_WR_RAM((uint8_t)ey);
}

/***************************************************************************************************
* Description:  读点
***************************************************************************************************/
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y)
{
    LCD_SetPosi(x, y);
    LCD_WR_CMD(0x2E);   //开始读GRAM
    return LCD_RD_RAM();
}


/***************************************************************************************************
* Description:  画点
***************************************************************************************************/
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_SetPosi(x, y);
    LCD_WR_CMD(0x2C);   //开始写GRAM
    LCD_WR_RAM(color);
}

/***************************************************************************************************
* Description:  画线
***************************************************************************************************/
void LCD_DrawLine(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    
    
}

/***************************************************************************************************
* Description:  画矩形 
***************************************************************************************************/
void LCD_DrawRetangle(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    int i;
    
    //设置区域
    LCD_SetArea(sx, sy, ex, ey);
    LCD_WR_CMD(0x2C);               //开始写GRAM
    
    //计算点数
    i = (ex-sx+1) * (ey-sy+1);
    
    //写RAM
    while(i--)
        LCD_WR_RAM(color);
}




/****************************************** END OF FILE *******************************************/
