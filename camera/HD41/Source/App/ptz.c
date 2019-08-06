/***************************************************************************************************
* FILE: ptz.c

* DESCRIPTION:  --

* CREATED: 2017/11/15, by kevin xu
***************************************************************************************************/

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/
#include "bsp.h"
#include "system.h"
#include "ccp.h"


/***************************************************************************************************
* Description: 根据镜头位置和设置的红外遥控速度来计算实际速度  
*              Sigmoid函数y = 1 ./ (1+exp(1-1/15*x))
***************************************************************************************************/
void CalPtzSpeed(void)
{
    static uint8_t zoom_pos = 0;
    static float ratio = 0;
    static float speed = 0;
    
    zoom_pos = 64-(uint8_t)(stCamInfo.zoom_pos >> 8);
    ratio = (float)stSysInfo.remote_speed / 27.0f;
    speed = (stSysInfo.remote_speed / 3) + zoom_pos * ratio;
    
    if(speed > 60) speed = 60;
    if(speed < 1) speed = 1;
    
    stRunInfo.pan_speed = (uint8_t)speed;
    stRunInfo.tilt_speed = (uint8_t)speed;
}

/***************************************************************************************************
* Description:  将度每秒转换为电机驱动速度
***************************************************************************************************/
uint8_t TransPanSpeed(uint8_t speed)
{
    uint8_t new_speed;
    
    new_speed = (uint8_t)(speed * PAN_ONE_DEGREE_SPEED + 0.5f);
    if(new_speed == 0) new_speed = 1;
    return new_speed;
}
uint8_t TransTiltSpeed(uint8_t speed)
{
    uint8_t new_speed;
    
    new_speed = (uint8_t)(speed * TILT_ONE_DEGREE_SPEED + 0.5f);
    if(new_speed == 0) new_speed = 1;
    return new_speed;
}

uint8_t GetPresetSpeed(int32_t motor, int32_t cur_pos, int32_t tar_pos)
{
    uint8_t speed;
    
    if(motor == PAN)
        speed = TransPanSpeed(PAN_PRESET_SPEED);
    else
        speed = TransTiltSpeed(TILT_PRESET_SPEED); 
    
    return speed;
}


/***************************************************************************************************
* Description:  
***************************************************************************************************/
void PTZ_CheckPos(void)
{

}

/***************************************************************************************************
* Description:  脉冲时钟初始化
***************************************************************************************************/
void TIM1_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    
    TIM_DeInit(TIM1);
    TIM_BaseInitStructure.TIM_ClockDivision = 1;
    TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_BaseInitStructure.TIM_Period = 20000;//20ms重装载， 50Hz
    TIM_BaseInitStructure.TIM_Prescaler = 95;//1MHz
    TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_BaseInitStructure);
    
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    
    TIM_Cmd(TIM1, ENABLE);
}
/* 配置端口 */
#define MS41909_CS                      SPI1_CS
#define MS41909SPI_WriteReadByte        SPI1_WriteReadByte
/***************************************************************************************************
* Description:  写数据到驱动芯片
***************************************************************************************************/
int32_t MS41909_WriteData(uint8_t addr, uint16_t arg)
{
    MS41909_CS = 1;
    MS41909SPI_WriteReadByte(addr);
    MS41909SPI_WriteReadByte((uint8_t)(arg&0xFF));
    MS41909SPI_WriteReadByte((uint8_t)(arg>>8));
    MS41909_CS = 0;
    return 0;
}

/***************************************************************************************************
* Description:  启动脉冲
***************************************************************************************************/
void MS41909_StartVdPulse(void)
{
    TIM1->CR1 |= TIM_CR1_CEN;
}

/***************************************************************************************************
* Description:  暂停脉冲
***************************************************************************************************/
void MS41909_StopVdPulse(void)
{
    TIM1->CR1 &= ~TIM_CR1_CEN;
}

/***************************************************************************************************
* Description:  云台初始化
***************************************************************************************************/
void PanTilt_Init(void)
{
    struct ms41909_ops ops = 
    {
        MS41909_WriteData,
        MS41909_StartVdPulse,
        MS41909_StopVdPulse,
    };
    
    MS41909_Init(&ops);
    MS41909_SetFrequency(MOTOR_DRIVER_FREQUENCY);
    MS41909_SetDirInv(PAN, OFF);
    MS41909_SetDirInv(TILT, OFF);
    MS41909_MotorEnable(PAN, ON);
    MS41909_MotorEnable(TILT, ON);
    MS41909_SetStopCutOff(PAN, OFF);
    MS41909_SetStopCutOff(TILT, OFF);
    MS41909_SetRunPower(PAN, 100);
    MS41909_SetRunPower(TILT, 100);
    MS41909_SetStopPower(PAN, 50);
    MS41909_SetStopPower(TILT, 80);
    
    TIM1_Init();
}

/***************************************************************************************************
* Description:  Ptz模块初始化
***************************************************************************************************/
void Ptz_Init(void)
{
    stRunInfo.standby = OFF;
    stRunInfo.inq_mask = 0;
    stRunInfo.menu = MENU_OFF;      //开机菜单为关闭状态
    stCamInfo.zoom_pos = 0;         //zoom位置自检后为0
    CalPtzSpeed();

    stRunInfo.ptz_inv = OFF;
}

/***************************************************************************************************
* Description:  云台自检
***************************************************************************************************/
void PanTilt_Reset(int32_t panPos, int32_t tiltPos)
{
    uint32_t ulCount;
    int32_t temp;
    static uint8_t PanSenseState=0, TiltSenseState=0;
    static int32_t s32PanOffset=0, s32TiltOffset=0;
    static int8_t s8PanIntCount=0, s8TiltIntCount=0;
    
    /* 初始化变量 */
    s32PanOffset = 0;
    s32TiltOffset = 0;
    
    /* 刹车 */
    MS41909_MotorBrake(PAN, 10);
    MS41909_MotorBrake(TILT, 10);
    while(MS41909_MotorIsRunning(PAN) || MS41909_MotorIsRunning(TILT) != 0);
    
    /* 获取初始状态 */
    do
    {
        PanSenseState = IS_PAN_SENSE;
        delay_ms(20);
    }
    while(PanSenseState != IS_PAN_SENSE);
    do
    {
        TiltSenseState = IS_TILT_SENSE;
        delay_ms(20);
    } while(TiltSenseState != IS_TILT_SENSE);
    
    if(PanSenseState)
    {
        //处于限位区，先运动到限位区之外再进入限位区，所以需要出发3次中断
        s8PanIntCount = 3;
        temp = MS41909_GetMotorPosition(PAN) - PAN_RESET_DISTANCE * PAN_RESET_DIR;
        MS41909_MotorGoto(PAN, temp, PAN_RESET_SPEED, 1, 5);
    }
    else
    {
        //在限位区之外，总共触发2次中断
        s8PanIntCount = 2;
        temp = MS41909_GetMotorPosition(PAN) + PAN_RESET_DISTANCE * PAN_RESET_DIR;
        MS41909_MotorGoto(PAN, temp, PAN_RESET_SPEED, 1, 5);
    }
    
    if(TiltSenseState)
    {
        //处于限位区，先运动到限位区之外再进入限位区，所以需要出发3次中断
        s8TiltIntCount = 3;
        temp = MS41909_GetMotorPosition(TILT) - TILT_RESET_DISTANCE * TILT_RESET_DIR;
        MS41909_MotorGoto(TILT, temp, TILT_RESET_SPEED, 5, 5);
    }
    else
    {
        //在限位区之外，总共触发2次中断
        s8TiltIntCount = 2;
        temp = MS41909_GetMotorPosition(TILT) + TILT_RESET_DISTANCE * TILT_RESET_DIR;
        MS41909_MotorGoto(TILT, temp, TILT_RESET_SPEED, 5, 5);
    }
    
    ulCount = 0;
    while(1)
    {
        /* 水平传感器状态变化 */
        if(PanSenseState != IS_PAN_SENSE)
        {
            delay_ms(10);
            if(PanSenseState != IS_PAN_SENSE)
            {
                PanSenseState = IS_PAN_SENSE;
                if(s8PanIntCount > 0)
                {
                    --s8PanIntCount;
                    if(s8PanIntCount == 0) s32PanOffset = MS41909_MotorBrake(PAN, 1);
                    else s32PanOffset = MS41909_MotorBrake(PAN, 10);
                }
            }
        }
        
        /* 垂直传感器状态变化 */
        if(TiltSenseState != IS_TILT_SENSE)
        {
            delay_ms(10);
            if(TiltSenseState != IS_TILT_SENSE)
            {
                TiltSenseState = IS_TILT_SENSE;
                if(s8TiltIntCount > 0)
                {
                    --s8TiltIntCount;
                    if(s8TiltIntCount == 0) s32TiltOffset = MS41909_MotorBrake(TILT, 1);
                    else s32TiltOffset = MS41909_MotorBrake(TILT, 5);
                }
            }
        }
        
        /* 水平已停车 */
        if(!MS41909_MotorIsRunning(PAN))
        {
            if(s8PanIntCount > 0)
            {
                if(IS_PAN_SENSE)
                {
                    temp = MS41909_GetMotorPosition(PAN) - PAN_RESET_DISTANCE * PAN_RESET_DIR;
                    MS41909_MotorGoto(PAN, temp, 1, 1, 1);
                }
                else
                {
                    temp = MS41909_GetMotorPosition(PAN) + PAN_RESET_DISTANCE * PAN_RESET_DIR;
                    MS41909_MotorGoto(PAN, temp, 1, 1, 1);
                }
            }
            else if(s8PanIntCount == 0)
            {
                s8PanIntCount = -1;
                temp = PAN_CK_POS - s32PanOffset * PAN_RESET_DIR;
                MS41909_SetMotorPosition(PAN, temp);
                MS41909_MotorGoto(PAN, panPos, TransPanSpeed(PAN_GOHOME_SPEED), PAN_GOHOME_ACC, PAN_GOHOME_DEC);
            }
        }
        
        /* 垂直已停车 */
        if(!MS41909_MotorIsRunning(TILT))
        {
            if(s8TiltIntCount > 0)
            {
                if(IS_TILT_SENSE)
                {
                    temp = MS41909_GetMotorPosition(TILT) - TILT_RESET_DISTANCE * TILT_RESET_DIR;
                    MS41909_MotorGoto(TILT, temp, 1, 1, 1);
                }
                else
                {
                    temp = MS41909_GetMotorPosition(TILT) + TILT_RESET_DISTANCE * TILT_RESET_DIR;
                    MS41909_MotorGoto(TILT, temp, 1, 1, 1);
                }
            }
            else if(s8TiltIntCount == 0)
            {
                s8TiltIntCount = -1;
                temp = TILT_CK_POS - s32TiltOffset  * TILT_RESET_DIR;
                MS41909_SetMotorPosition(TILT, temp);
                MS41909_MotorGoto(TILT, tiltPos, TransTiltSpeed(TILT_GOHOME_SPEED), TILT_GOHOME_ACC, TILT_GOHOME_DEC);
            }
        }
        
        if(s8PanIntCount==-1 && s8TiltIntCount==-1)
        {
            break;
        }
        
        if(++ulCount > PT_RESET_TIMEOUT)
        {
            MS41909_MotorBrake(PAN, 10);
            MS41909_MotorBrake(TILT, 10);
            while(MS41909_MotorIsRunning(PAN) || MS41909_MotorIsRunning(TILT) != 0);
            MS41909_SetMotorPosition(PAN, 0);
            MS41909_SetMotorPosition(TILT, 0);
            break;
        }
        
        delay_ms(1);
    }
}

/****************************************** END OF FILE *******************************************/
