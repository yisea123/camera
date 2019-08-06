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

static int8_t ADC_Value[Channel] = {0};     //���㻺��
static uint8_t DiffCount[Channel] = {0};    //ƫ�����
static int8_t ADC_Result[Channel] = {0};    //���

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* Description:  ˢ��ҡ������
***************************************************************************************************/
void Rocker_Refresh(void)
{
    for(int i=0; i<Channel; i++)
    {
        ADC_Value[i] = (AdcBuffer[i] >> 4) - 127;//0~255
        
        /* ƫ��������� */
        (ADC_Value[i]==ADC_Result[i]) ? (DiffCount[i] = 0) : (DiffCount[i]++);		//�����˲�
        
        if(DiffCount[i] > 7)
        {
            //����Ч���ݲ�һ�µ����ݸ�������7���򽫴�������Ϊ�µ���Ч����
            ADC_Result[i] = (int8_t)ADC_Value[i];
        }
    }
}

/***************************************************************************************************
* Description:  ��ȡҡ��λ��,����1��ʾ������Ч,0��ʾ��Ч
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
        /* ҡ��ֻҪ���ڴ�����ֵ��һֱ����,�����ڲ�����̨ */
        case 0:
        if(abs_val > 5)
        {
            new_val = (ADC_Result[ROCKER_PAN]>0) ? (ADC_Result[ROCKER_PAN]-5) : (ADC_Result[ROCKER_PAN]+5);
            *val = new_val;
            ret = 1;
        }
        break;
        
        /* �ڷ�ʽ0�Ļ�����,ҡ�˴��ڴ�����ֵ��λ�ò��ҵ�����һ�ζ�ȡ��ֵ�򳬹����ʱ��Ŵ���,
        �����ڲ�����̨ */
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
        
        /* ҡ�˻�λ������һ�κ�ҡ��λ�ñ��ִ�����ֵһ��ʱ�����������,�����ڹ����б� */
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
        
        /* ҡ�˻�λ��ֻ�ܴ���һ��,�����ڲ�������˵� */
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
        /* ҡ��ֻҪ���ڴ�����ֵ��һֱ����,�����ڲ�����̨ */
        case 0:
        if(abs_val > 5)
        {
            new_val = (ADC_Result[ROCKER_TILT]>0) ? (ADC_Result[ROCKER_TILT]-5) : (ADC_Result[ROCKER_TILT]+5);
            *val = new_val;
            ret = 1;
        }
        break;
        
        /* �ڷ�ʽ0�Ļ�����,ҡ�˴��ڴ�����ֵ��λ�ò��ҵ�����һ�ζ�ȡ��ֵ�򳬹����ʱ��Ŵ���,
        �����ڲ�����̨ */
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
        
        /* ҡ�˻�λ������һ�κ�ҡ��λ�ñ��ִ�����ֵһ��ʱ�����������,�����ڹ����б� */
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
        
        /* ҡ�˻�λ��ֻ�ܴ���һ��,�����ڲ�������˵� */
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
* Description:  ҡ�˴��ڴ�����ֵ��λ�ò��ҵ�����һ�ζ�ȡ��ֵ�򳬹����ʱ��Ŵ���, �����ڲ�����̨
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
        
        /* ����� */
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
        
        /* ����� */
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
        /* ҡ��ֻҪ���ڴ�����ֵ��һֱ����,�����ڲ�����̨ */
        case 0:
        if(abs_val > 5)
        {
            new_val = (ADC_Result[ROCKER_ZOOM]>0) ? (ADC_Result[ROCKER_ZOOM]-5) : (ADC_Result[ROCKER_ZOOM]+5);
            *val = new_val;
            ret = 1;
        }
        break;
        
        /* �ڷ�ʽ0�Ļ�����,ҡ�˴��ڴ�����ֵ��λ�ò��ҵ�����һ�ζ�ȡ��ֵ�򳬹����ʱ��Ŵ���,
        �����ڲ�����̨ */
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
        
        /* ҡ�˻�λ������һ�κ�ҡ��λ�ñ��ִ�����ֵһ��ʱ�����������,�����ڹ����б� */
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
        
        /* ҡ�˻�λ��ֻ�ܴ���һ��,�����ڲ�������˵� */
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
