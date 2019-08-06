/***************************************************************************************************
* FILE: key.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/7, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "key.h"
#include "system.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define KEY_FLITER					6

#define KEY_SCAN_DELAY()    asm( " nop " );asm( " nop " );asm( " nop " )

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/
static uint8_t Key_Counter[35] = {0};
static uint8_t Key_Fliter[35] = {0};
static uint8_t Key_Refresh[35] = {0};
static uint8_t Key_Reentry[35] = {0};
static uint32_t Key_Timer[35] = {0};

/***************************************************************************************************
* Description: 读取按键值
* Arguments:   n按键号， 
*              mode：0:不支持连按,其他:支持连按
* Returns:     按键值
***************************************************************************************************/
uint8_t GetKey(uint8_t n, uint8_t mode)
{
    uint8_t ret = 0;
    if(Key_Counter[n] >= Key_Fliter[n])
    {
        if(Key_Refresh[n] == 1)
        {
            Key_Refresh[n] = 0;
            Key_Reentry[n] = 0;
            Key_Timer[n] = HAL_GetTick();
            Buzzer_Beep(LED_100MS);
            ret = 1;
        }
        else if(mode)
        {
            if(Key_Reentry[n] == 1)
            {
                if(CheckInterval(&Key_Timer[n], 200))
                {
                    Buzzer_Beep(LED_100MS);
                    ret = 1;
                }
            }
            else if(CheckInterval(&Key_Timer[n], 1000))
            {
                Key_Reentry[n] = 1;
                Buzzer_Beep(LED_100MS);
                ret = 1;
            }
        }
    }
    
    if(ret == 1)
        System_UpdateIdleTimer();
    
    return ret;
}

/***************************************************************************************************
* Description:  读取数字按键是否触发
***************************************************************************************************/
int8_t GetKeyNum(void)
{
    int8_t s8Ret = -1;
    
    if(GetKey(KEY_NUM0, 0))
    {
        LED_Ctrl(KEY_NUM0, LED_1S);
        s8Ret = 0;
    }
    else if(GetKey(KEY_NUM1, 0))
    {
        LED_Ctrl(KEY_NUM1, LED_1S);
        s8Ret = 1;
    }
    else if(GetKey(KEY_NUM2, 0))
    {
        LED_Ctrl(KEY_NUM2, LED_1S);
        s8Ret = 2;
    }
    else if(GetKey(KEY_NUM3, 0))
    {
        LED_Ctrl(KEY_NUM3, LED_1S);
        s8Ret = 3;
    }
    else if(GetKey(KEY_NUM4, 0))
    {
        LED_Ctrl(KEY_NUM4, LED_1S);
        s8Ret = 4;
    }
    else if(GetKey(KEY_NUM5, 0))
    {
        LED_Ctrl(KEY_NUM5, LED_1S);
        s8Ret = 5;
    }
    else if(GetKey(KEY_NUM6, 0))
    {
        LED_Ctrl(KEY_NUM6, LED_1S);
        s8Ret = 6;
    }
    else if(GetKey(KEY_NUM7, 0))
    {
        LED_Ctrl(KEY_NUM7, LED_1S);
        s8Ret = 7;
    }
    else if(GetKey(KEY_NUM8, 0))
    {
        LED_Ctrl(KEY_NUM8, LED_1S);
        s8Ret = 8;
    }
    else if(GetKey(KEY_NUM9, 0))
    {
        LED_Ctrl(KEY_NUM9, LED_1S);
        s8Ret = 9;
    }
    else s8Ret = -1;
    
    return s8Ret;
}

/***************************************************************************************************
* Description: 按键扫描
***************************************************************************************************/
void KeyScan(void)
{
    int8_t i;
    
    KEY_PL = 0;
    KEY_SCAN_DELAY();
    KEY_PL = 1;
    KEY_SCAN_DELAY();
    for(i=7; i>=0; i--)
    {
        if(KEY_DATA)
        {
            Key_Counter[i] = 0;
            Key_Refresh[i] = 1;
        }
        else
        {
            if(Key_Counter[i] < Key_Fliter[i])
            {
                Key_Counter[i]++;   
            }
        }
        KEY_CLK = 0;
        KEY_SCAN_DELAY();
        KEY_CLK = 1;
        KEY_SCAN_DELAY();
    }
    
    for(i=15; i>=8; i--)
    {
        if(KEY_DATA)
        {
            Key_Counter[i] = 0;
            Key_Refresh[i] = 1;
        }
        else
        {
            if(Key_Counter[i] < Key_Fliter[i])
            {
                Key_Counter[i]++;   
            }
        }
        KEY_CLK = 0;
        KEY_SCAN_DELAY();
        KEY_CLK = 1;
        KEY_SCAN_DELAY();
    }
    
    for(i=23; i>=16; i--)
    {
        if(KEY_DATA)
        {
            Key_Counter[i] = 0;
            Key_Refresh[i] = 1;
        }
        else
        {
            if(Key_Counter[i] < Key_Fliter[i])
            {
                Key_Counter[i]++;   
            }
        }
        KEY_CLK = 0;
        KEY_SCAN_DELAY();
        KEY_CLK = 1;
        KEY_SCAN_DELAY();
    }
    
    for(i=31; i>=24; i--)
    {
        if(KEY_DATA)
        {
            Key_Counter[i] = 0;
            Key_Refresh[i] = 1;
        }
        else
        {
            if(Key_Counter[i] < Key_Fliter[i])
            {
                Key_Counter[i]++;   
            }
        }
        KEY_CLK = 0;
        KEY_SCAN_DELAY();
        KEY_CLK = 1;
        KEY_SCAN_DELAY();
    } 
    
    if(KEY33)
    {
        Key_Counter[32] = 0;
        Key_Refresh[32] = 1;
    }
    else
    {
        if(Key_Counter[32] < Key_Fliter[i])
        {
            Key_Counter[32]++;   
        }
    }
    
    if(KEY34)
    {
        Key_Counter[33] = 0;
        Key_Refresh[33] = 1;
    }
    else
    {
        if(Key_Counter[33] < Key_Fliter[i])
        {
            Key_Counter[33]++;   
        }
    }
}

/***************************************************************************************************
* Description: 按键初始化
***************************************************************************************************/
void Key_Init(void)
{
    uint8_t i;

    for(i=0; i<35; i++)
    {
        Key_Fliter[i] = KEY_FLITER;
    }
//    Key_Fliter[KEY_OSD] = 20000;
}
/****************************************** END OF FILE *******************************************/
