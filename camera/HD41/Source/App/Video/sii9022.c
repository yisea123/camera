/***************************************************************************************************
* FILE: sii9022.c
*
* DESCRIPTION:  --
*
* CREATED: 2017/11/22, by kevin xu
***************************************************************************************************/

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/
#include "sii9022.h"
#include "video.h"
/***************************************************************************************************
* LOCAL DEFINES
***************************************************************************************************/
#define SII9136_ADDR            0x72    //CI2CA拉高
#define SII9022_ADDR            0x76    //CI2CA拉低
#define SII9022CPI_ADDR         0xC4

//i2c buf interface
#define SII9022WRITE(a, b)      I2C1_WriteOneByte(SII9022_ADDR, a, b)
#define SII9022READ(a, b)       I2C1_ReadOneByte(SII9022_ADDR, a, b)
#define SII9022CPI_WRITE(a, b)  I2C1_WriteOneByte(SII9022CPI_ADDR, a, b)
#define SII9022CPI_READ(a, b)   I2C1_ReadOneByte(SII9022CPI_ADDR, a, b)

//delay
#define SII9022_DELAY_MS(a)     delay_ms(a)
#define SII9022_DELAY           SII9022_DELAY_MS(20)

/***************************************************************************************************
* LOCAL GLOBAL VARIABLES
***************************************************************************************************/
uint8_t CEC_OWN_ADDR = 0x01;//CEC初始地址

/* video info */
const uint16_t SII9022_ESYNC[][6] = 
{
    //HBIT FIELD2 HWIDTH  VBIT VWIDTH
    {440,       0,   40,     5,    5},//720p50
    {110,       0,   40,     5,    5},//720p60
    {528,       0,   44,     4,    5},//1080p25
    { 88,       0,   44,     4,    5},//1080p30
    {528,       0,   44,     4,    5},//1080p50
    { 88,       0,   44,     5,    4},//1080p60
    {528,    1320,   44,     2,    5},//1080i50
    { 88,    1100,   44,     2,    5},//1080i60
};

/***************************************************************************************************
* Description:  sil9022 powerup initialization
***************************************************************************************************/
uint8_t Sii9022_Init(void)
{
    return 0;
}

/***************************************************************************************************
* Description:  设置输出分辨率
***************************************************************************************************/    
uint8_t Sii9022_SetVideoFormat(uint8_t res)
{   
    uint8_t index = 0;
    switch(res)
    {
        case VIDEO_1280x720p50: index = 0; break;
        case VIDEO_1280x720p60: index = 1; break;
        case VIDEO_1920x1080p25: index = 2; break;
        case VIDEO_1920x1080p29:
        case VIDEO_1920x1080p30: index = 3; break;
        case VIDEO_1920x1080p50: index = 4; break;
        case VIDEO_1920x1080p59:
        case VIDEO_1920x1080p60: index = 5; break;
        case VIDEO_1920x1080i50: index = 6; break;
        case VIDEO_1920x1080i59:
        case VIDEO_1920x1080i60: index = 7; break;
        default: return 3; break;
    }
    
    SII9022_RESET = 1;
    SII9022_DELAY_MS(50);
    SII9022_RESET = 0;
    SII9022_DELAY_MS(50);
    SII9022_RESET = 1;
    SII9022_DELAY_MS(100);
    
    /* Reset chip */
    if (SII9022WRITE(0xC7, 0x00)) return 1;
    SII9022_DELAY;
    if (SII9022WRITE(0x1E, 0x00)) return 1;
    SII9022_DELAY_MS(100);
    
    /* Mute */
    if (SII9022WRITE(0x1A, 0x09)) return 1;
    SII9022_DELAY;
    
    /* Configure Input Bus and Pixel Repetition */
    if (SII9022WRITE(0x08, 0x60)) return 1;
    SII9022_DELAY;
    
    /* Configure Input Format Data */
    if (SII9022WRITE(0x09, 0x02)) return 1;
    SII9022_DELAY;
    
    /* Configure Output Format Data */
    if (SII9022WRITE(0x0A, 0x10)) return 1;
    SII9022_DELAY;
    
    /* YC Input Mode Select */
    if (SII9022WRITE(0x0B, 0x00)) return 1;
    SII9022_DELAY;
    
    /* Configure Sync Format Data */
    if (SII9022WRITE(0x60, 0x84)) return 1;
    SII9022_DELAY;
    
    /*  */
    if (SII9022WRITE(0x19, 0x00)) return 1;
    SII9022_DELAY;
    
    /*  */
    if (SII9022WRITE(0x61, 0x00)) return 1;
    SII9022_DELAY;
    
    /*  */
    if (SII9022WRITE(0x1A, 0x10)) return 1;
    SII9022_DELAY;
    
    /* HBIT to HSYNC */
    if (SII9022WRITE(0x62, SII9022_ESYNC[index][0])) return 1;
    SII9022_DELAY;
    /*  */
    if (SII9022WRITE(0x63, 0x40+((SII9022_ESYNC[index][0]&0x0300)>>8))) return 1;
    SII9022_DELAY;
    /*  */
    if (SII9022WRITE(0x64, SII9022_ESYNC[index][1])) return 1;
    SII9022_DELAY;
    /*  */
    if (SII9022WRITE(0x65, (SII9022_ESYNC[index][1]&0x1F00)>>8)) return 1;
    SII9022_DELAY;
    /*  */
    if (SII9022WRITE(0x66, SII9022_ESYNC[index][2])) return 1;
    SII9022_DELAY;
    /*  */
    if (SII9022WRITE(0x67, (SII9022_ESYNC[index][2]&0x0300)>>8)) return 1;
    SII9022_DELAY;
    /*  */
    if (SII9022WRITE(0x68, SII9022_ESYNC[index][3]&0x3F)) return 1;
    SII9022_DELAY;
    /*  */
    if (SII9022WRITE(0x69, SII9022_ESYNC[index][4]&0x3F)) return 1;
    SII9022_DELAY;
    
    /*  */
    if (SII9022WRITE(0x1A, 0x00)) return 1;
    SII9022_DELAY;
    
    return 0;
}

/****************************************** END OF FILE *******************************************/
