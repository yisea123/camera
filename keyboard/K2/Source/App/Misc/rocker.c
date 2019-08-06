/***************************************************************************************************
* FILE: rocker.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/21, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "bsp.h"
#include "system.h"
#include "rocker.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define Channel 3

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/
extern uint16_t AdcBuffer[4];

static int8_t ADC_Value[Channel] = {0};     //计算缓存
static uint8_t DiffCount[Channel] = {0};    //偏差计数
static int8_t ADC_Result[Channel] = {0};    //结果

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* Description:  刷新摇杆数据
***************************************************************************************************/
void Rocker_Refresh(void)
{
    for(int i=0; i<Channel; i++)
    {
        ADC_Value[i] = (AdcBuffer[i] >> 4) - 127;//0~255
        
        /* 偏差计数消抖 */
        (ADC_Value[i]==ADC_Result[i]) ? (DiffCount[i] = 0) : (DiffCount[i]++);		//消抖滤波
        
        if(DiffCount[i] > 7)
        {
            //和有效数据不一致的数据个数超过7个则将此数据作为新的有效数据
            ADC_Result[i] = (int8_t)ADC_Value[i];
        }
    }
}

/***************************************************************************************************
* Description:  获取摇杆位置,返回1表示数据有效,0表示无效
***************************************************************************************************/
int Rocker_GetPanVal(int8_t *val, uint8_t mode)
{
    static uint32_t timer;
    static uint8_t refresh = 0;
    static uint8_t reentry = 0;
    static int8_t backup_val;
    int8_t new_val;
    int8_t abs_val;
    
    int ret = 0;
    
    abs_val = abs(ADC_Result[ROCKER_PAN]);
    switch(mode)
    {
        /* 摇杆只要大于触发阈值就一直触发,适用于操作云台 */
        case 0:
        if(abs_val > 5)
        {
            new_val = (ADC_Result[ROCKER_PAN]>0) ? (ADC_Result[ROCKER_PAN]-5) : (ADC_Result[ROCKER_PAN]+5);
            *val = new_val;
            ret = 1;
        }
        break;
        
        /* 在方式0的基础上,摇杆大于触发阈值若位置并且等于上一次读取的值则超过间隔时间才触发,
        适用于操作云台 */
        case 1:
        if(abs_val > 5)
        {
            new_val = (ADC_Result[ROCKER_PAN]>0) ? (ADC_Result[ROCKER_PAN]-5) : (ADC_Result[ROCKER_PAN]+5);
            
            if(new_val == backup_val)
            {
                if(CheckInterval(&timer, 500))
                {
                    *val = backup_val;
                    ret = 1;
                }
                else
                {
                    ret = 2;
                }
            }
            else
            {
                *val = new_val;
                backup_val = new_val;
                ret = 1;
            }
        }
        break;
        
        /* 摇杆回位并触发一次后摇杆位置保持大于阈值一定时间后连续触发,适用于滚动列表 */
        case 2:
        if(abs_val > 15)
        {
            if(ADC_Result[ROCKER_PAN] > 0) *val = 1;
            else *val = -1;
            
            if(refresh == 0)
            {
                refresh = 1;
                timer = HAL_GetTick();
                ret = 1;
            }
            else
            {
                if(reentry == 1)
                {
                    if(CheckInterval(&timer, 100))
                        ret = 1;
                }
                
                if(CheckInterval(&timer, 1000))
                {
                    reentry = 1;
                    ret = 1;
                }
            }
        }
        else
        {
            reentry = 0;
            if(abs_val < 7)
            {
                refresh = 0;
            }
        }
        break;
        
        /* 摇杆回位后只能触发一次,适用于操作横向菜单 */
        case 3:
        if(abs_val > 15)
        {
            if(refresh == 0)
            {
                refresh = 1;
                if(ADC_Result[ROCKER_PAN] > 0) *val = 1;
                else *val = -1;
                ret = 1;
            }
        }
        else
        {
            if(abs_val < 7)
                refresh = 0;
        }
        break;
        
        default:
        break;       
    }
    
    if(ret)
        System_UpdateIdleTimer();
    return ret;
}

int Rocker_GetTiltVal(int8_t *val, uint8_t mode)
{
    static uint32_t timer;
    static uint8_t refresh = 0;
    static uint8_t reentry = 0;
    static int8_t backup_val;
    int8_t new_val;
    int8_t abs_val;
    
    int ret = 0;
    
    abs_val = abs(ADC_Result[ROCKER_TILT]);
    switch(mode)
    {
        /* 摇杆只要大于触发阈值就一直触发,适用于操作云台 */
        case 0:
        if(abs_val > 5)
        {
            new_val = (ADC_Result[ROCKER_TILT]>0) ? (ADC_Result[ROCKER_TILT]-5) : (ADC_Result[ROCKER_TILT]+5);
            *val = new_val;
            ret = 1;
        }
        break;
        
        /* 在方式0的基础上,摇杆大于触发阈值若位置并且等于上一次读取的值则超过间隔时间才触发,
        适用于操作云台 */
        case 1:
        if(abs_val > 5)
        {
            new_val = (ADC_Result[ROCKER_TILT]>0) ? (ADC_Result[ROCKER_TILT]-5) : (ADC_Result[ROCKER_TILT]+5);
            
            if(new_val == backup_val)
            {
                if(CheckInterval(&timer, 500))
                {
                    *val = backup_val;
                    ret = 1;
                }
                else
                {
                    ret = 2;
                }
            }
            else
            {
                *val = new_val;
                backup_val = new_val;
                ret = 1;
            }
        }
        break;
        
        /* 摇杆回位并触发一次后摇杆位置保持大于阈值一定时间后连续触发,适用于滚动列表 */
        case 2:
        if(abs_val > 15)
        {
            if(ADC_Result[ROCKER_TILT] > 0) *val = 1;
            else *val = -1;
            
            if(refresh == 0)
            {
                refresh = 1;
                timer = HAL_GetTick();
                ret = 1;
            }
            else
            {
                if(reentry == 1)
                {
                    if(CheckInterval(&timer, 100))
                        ret = 1;
                }
                
                if(CheckInterval(&timer, 1000))
                {
                    reentry = 1;
                    ret = 1;
                }
            }
        }
        else
        {
            reentry = 0;
            if(abs_val < 7)
            {
                refresh = 0;
            }
        }
        break;
        
        /* 摇杆回位后只能触发一次,适用于操作横向菜单 */
        case 3:
        if(abs_val > 15)
        {
            if(refresh == 0)
            {
                refresh = 1;
                if(ADC_Result[ROCKER_TILT] > 0) *val = 1;
                else *val = -1;
                ret = 1;
            }
        }
        else
        {
            if(abs_val < 7)
                refresh = 0;
        }
        break;
        
        default:
        break;
    }
    
    if(ret)
        System_UpdateIdleTimer();
    return ret;
}

/***************************************************************************************************
* Description:  摇杆大于触发阈值若位置并且等于上一次读取的值则超过间隔时间才触发, 适用于操作云台
***************************************************************************************************/
int Rocker_GetPanTiltVal(int8_t *pan_val, int8_t *tilt_val)
{
    static uint32_t pan_timer, tilt_timer;
    static int8_t pan_old, tilt_old;
    
    int8_t pan_new, tilt_new;
    uint8_t flag = 0;
    
    if(abs(ADC_Result[ROCKER_PAN]) > 5)
    {
        pan_new = (ADC_Result[ROCKER_PAN]>0) ? (ADC_Result[ROCKER_PAN]-5) : (ADC_Result[ROCKER_PAN]+5);
        
        /* 反向打开 */
        if(stSysPara.rocker.pan_reverse)
        {
            pan_new *= -1;
        }
        
        if(pan_new == pan_old)
        {
            if(CheckInterval(&pan_timer, 500))
            {
                *pan_val = pan_old;
                flag |= 0x01;
            }
            else
            {
                flag |= 0x02;
            }
        }
        else
        {
            *pan_val = pan_new;
            pan_old = pan_new;
            flag |= 0x01;
        }
    }
    else
    {
        *pan_val = 0;
    }
    
    if(abs(ADC_Result[ROCKER_TILT]) > 5)
    {
        tilt_new = (ADC_Result[ROCKER_TILT]>0) ? (ADC_Result[ROCKER_TILT]-5) : (ADC_Result[ROCKER_TILT]+5);
        
        /* 反向打开 */
        if(stSysPara.rocker.tilt_reverse)
        {
            tilt_new *= -1;
        }
        
        if(tilt_new == tilt_old)
        {
            if(CheckInterval(&tilt_timer, 500))
            {
                *tilt_val = tilt_old;
                flag |= 0x10;
            }
            else
            {
                flag |= 0x20;
            }
        }
        else
        {
            *tilt_val = tilt_new;
            tilt_old = tilt_new;
            flag |= 0x10;
        }
    }
    else
    {
        *tilt_val = 0;
    }
    
    if(flag == 0)
    {
        return 0;
    }
    else 
    {
        System_UpdateIdleTimer();
        if(flag & 0x11)
            return 1;
        else
            return 2;    
    }
}


/***************************************************************************************************
* Description:  
***************************************************************************************************/
int Rocker_GetZoomVal(int8_t *val, uint8_t mode)
{
    static uint32_t timer;
    static uint8_t refresh = 0;
    static uint8_t reentry = 0;
    static int8_t backup_val;
    int8_t new_val;
    int8_t abs_val;
    
    int ret = 0;
    
    abs_val = abs(ADC_Result[ROCKER_ZOOM]);
    switch(mode)
    {
        /* 摇杆只要大于触发阈值就一直触发,适用于操作云台 */
        case 0:
        if(abs_val > 5)
        {
            new_val = (ADC_Result[ROCKER_ZOOM]>0) ? (ADC_Result[ROCKER_ZOOM]-5) : (ADC_Result[ROCKER_ZOOM]+5);
            *val = new_val;
            ret = 1;
        }
        break;
        
        /* 在方式0的基础上,摇杆大于触发阈值若位置并且等于上一次读取的值则超过间隔时间才触发,
        适用于操作云台 */
        case 1:
        if(abs_val > 5)
        {
            new_val = (ADC_Result[ROCKER_ZOOM]>0) ? (ADC_Result[ROCKER_ZOOM]-5) : (ADC_Result[ROCKER_ZOOM]+5);
            
            if(new_val == backup_val)
            {
                if(CheckInterval(&timer, 50000))
                {
                    *val = backup_val;
                    ret = 1;
                }
                else
                {
                    ret = 2;
                }
            }
            else
            {
                *val = new_val;
                backup_val = new_val;
                ret = 1;
            }
        }
        break;
        
        /* 摇杆回位并触发一次后摇杆位置保持大于阈值一定时间后连续触发,适用于滚动列表 */
        case 2:
        if(abs_val > 15)
        {
            if(ADC_Result[ROCKER_ZOOM] > 0) *val = 1;
            else *val = -1;
            
            if(refresh == 0)
            {
                refresh = 1;
                timer = HAL_GetTick();
                ret = 1;
            }
            else
            {
                if(reentry == 1)
                {
                    if(CheckInterval(&timer, 100))
                        ret = 1;
                }
                
                if(CheckInterval(&timer, 1000))
                {
                    reentry = 1;
                    ret = 1;
                }
            }
        }
        else
        {
            reentry = 0;
            if(abs_val < 7)
            {
                refresh = 0;
            }
        }
        break;
        
        /* 摇杆回位后只能触发一次,适用于操作横向菜单 */
        case 3:
        if(abs_val > 15)
        {
            if(refresh == 0)
            {
                refresh = 1;
                if(ADC_Result[ROCKER_ZOOM] > 0) *val = 1;
                else *val = -1;
                ret = 1;
            }
        }
        else
        {
            if(abs_val < 7)
                refresh = 0;
        }
        break;
        
        default:
        break;       
    }
    
    if(ret)
    {
        System_UpdateIdleTimer();
    }
    
    return ret;
}




/****************************************** END OF FILE *******************************************/
