/***************************************************************************************************
* FILE: led.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/9/4, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "sn3731.h"
#include "bsp.h"
#include "system.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/


/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/



/***************************************************************************************************
* Description:  LED灯打开、关闭
***************************************************************************************************/
static int8_t LED_NUM = -1;
static uint32_t LED_TIMER = 0;
void LED_Ctrl(uint8_t num, int32_t arg)
{
    /* 关掉上一次打开的灯 */
    if(LED_NUM >= 0 && num != LED_NUM)
        SN3731_LedCtrl(LED_NUM, 0);
    
    /* 关灯 */
    if(arg == 0)
    {
        SN3731_LedCtrl(num, 0);
        LED_NUM = -1;
    }
    /* 打开一定时间 */
    else if(arg > 0)
    {
        SN3731_LedCtrl(num, 1);
        LED_NUM = num;
        LED_TIMER = HAL_GetTick() + arg;
    }
    /* 常亮 */
    else
    {
        SN3731_LedCtrl(num, 1);
        LED_NUM = -1;
    } 
}

/***************************************************************************************************
* Description:  关闭超时的LED灯
***************************************************************************************************/
void LED_CheckOff(void)
{
    if(LED_NUM >= 0)
    {
        if(HAL_GetTick() > LED_TIMER)
        {
            SN3731_LedCtrl(LED_NUM, 0);
        }
    }
}

/***************************************************************************************************
* Description:  蜂鸣器鸣叫
***************************************************************************************************/
static uint32_t BuzzerTimer = 0;
void Buzzer_Beep(int32_t arg)
{
    if(stSysPara.buzzer)
    {
        BuzzerTimer = HAL_GetTick() + arg;
        BUZZER_ON;
    }
}

/***************************************************************************************************
* Description:  蜂鸣器检查关闭
***************************************************************************************************/
void Buzzer_CheckOff(void)
{
    if(HAL_GetTick() > BuzzerTimer)
    {
        BUZZER_OFF;
    }
}

/****************************************** END OF FILE *******************************************/
