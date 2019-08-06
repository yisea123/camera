/***************************************************************************************************
* FILE: ir_remote.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/12/2, by kevin
***************************************************************************************************/
#include "bsp.h"
#include "ir_sony.h"
#include "ir_cmd.h"
#include "system.h"
#include "visca2camera.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* Description:  基础硬件初始化
***************************************************************************************************/
void IR_BspInit(void)
{
    TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 定时器 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseInitStructure.TIM_Prescaler = 95; //1MHz
    TIM_BaseInitStructure.TIM_Period = 65535;
    TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_BaseInitStructure);
    
    /* 数据脚 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
    
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/***************************************************************************************************
* Description:  设置红外地址
***************************************************************************************************/
static uint32_t IR_ADDR = 0;
void IR_SetAddr(uint8_t addr)
{
    IR_ADDR = addr << 24;
}

/***************************************************************************************************
* Description: 地址检测,三个地址
***************************************************************************************************/
void IR_AddrDetection(void)
{
    if(stSysInfo.ir_addr < 4)
        IR_SetAddr(stSysInfo.ir_addr);
    else
        IR_SetAddr(0);
}

/***************************************************************************************************
* Description: 红外中断打开
***************************************************************************************************/
inline void IR_ON(void)
{
    EXTI->IMR |= 1<<2;
}

/***************************************************************************************************
* Description: 红外中断关闭
***************************************************************************************************/
inline void IR_OFF(void)
{
    EXTI->IMR &= ~(1<<2);
}

/***************************************************************************************************
* Description: 开始计数
***************************************************************************************************/
inline void IR_Counter_Start(uint32_t val)
{
    TIM3->CNT = val; 
    TIM3->CR1 |= 0x01;
}

/***************************************************************************************************
* Description: 停止计数,返回计数值
***************************************************************************************************/
inline uint32_t IR_Counter_Stop(void)
{
    TIM3->CR1 &= ~0x01;
    return TIM3->CNT;
}

/***************************************************************************************************
* Description:  获取红外指令
***************************************************************************************************/
inline uint32_t IR_GetCmd(void)
{
    uint32_t cmd;
    cmd = SonyIr_GetCmd();
    if((IR_ADDR == 0) || ((cmd & 0xFF000000) == IR_ADDR))
        return cmd & 0x00FFFFFF;
    return 0;
}

/***************************************************************************************************
* Description: 硬件模块以及数据初始化
***************************************************************************************************/
void IrRemoter_Init(void)
{
    struct ir_decoder_ops ir_ops = 
    {
        IR_Counter_Start, 
        IR_Counter_Stop
    };
    
    
    if(SonyIr_Init(&ir_ops))
    {
        while(1);
    }
    
    IR_BspInit();
    IR_AddrDetection();
}


#define DELAY_ZOOM              200
#define DELAY_FOCUS             50
#define DELAY_PT                100

#define DELAY_POWER             1000

#define DELAY_SOUT              500
#define DELAY_PTZ_RESET         500
#define DELAY_PTZ_HOME          500
#define DELAY_DATASCREEN        500
#define DELAY_BACKLIGHT         500
#define DELAY_BRIGHT            500

static uint32_t SNAP_POWER = 0;
static uint32_t SNAP_ZOOM = 0;
static uint32_t SNAP_FOCUS = 0;
static uint32_t SNAP_BACKLIGHT = 0;
static uint32_t SNAP_BRIGHT = 0;
static uint32_t SNAP_SOUT = 0;
static uint32_t SNAP_PTZ_RESET = 0;
static uint32_t SNAP_PTZ_HOME = 0;
static uint32_t SNAP_PAN_TILT = 0;
static uint32_t SNAP_DATASCREEN = 0;

static uint8_t ZoomIsRunning = 0;
static uint8_t FocusIsRunning = 0;
static uint8_t PtIsRunning = 0;
/***************************************************************************************************
* Description:  延时任务
***************************************************************************************************/
static void _IrRemote_DelayTask(void)
{
    if(ZoomIsRunning)
    {
        if(CheckTimeDiff(SNAP_ZOOM, DELAY_ZOOM))
        {
            ZoomIsRunning = 0;
            cam_zoom(VISCA_ZOOM_STOP);
        }
    }
    
    if(FocusIsRunning)
    {
        if(CheckTimeDiff(SNAP_FOCUS, DELAY_FOCUS))
        {
            FocusIsRunning = 0;
            cam_focus(VISCA_FOCUS_STOP);
        }
    }
    
    if(PtIsRunning)
    {
        if(CheckTimeDiff(SNAP_PAN_TILT, DELAY_PT))
        {
            PtIsRunning = 0;
            pan_run(PAN_STOP, 0, 0, PAN_BRAKE_DEC);
            tilt_run(TILT_STOP, 0, 0, TILT_BRAKE_DEC);
        }
    }
}

/***************************************************************************************************
* Description:  正常状态指令响应
***************************************************************************************************/
static void _CmdHandler_Normal(void)
{
    uint32_t cmd = IR_GetCmd();
    
    /* 老化检测 */
    if(cmd == IR_CMD_MENU_CLOSE)
    {
        if(stSysInfo.aging_test != 0xFF)
            stSysInfo.aging_test++;
    }
    else if(cmd == IR_CMD_PTZ_RESET)
    {
        if(stSysInfo.aging_test > 20)
        {
            stSysInfo.aging_test = 0xFF;
            LED_Blink();
            System_SavePara();
            return;
        }
    }
    else if(cmd != 0)
    {
        if(stSysInfo.aging_test == 0xFF)
        {
            stSysInfo.aging_test = 0;
            System_SavePara();
        }
        stSysInfo.aging_test = 0;
    }
    
    switch(cmd)
    {
        case IR_CMD_POWER:
        if(CheckInterval(&SNAP_POWER, DELAY_POWER))
        {
            sys_power(VISCA_OFF);
        }
        break;
        
        case IR_CMD_PTZ_RESET:
        if(CheckInterval(&SNAP_PTZ_RESET, DELAY_PTZ_RESET))
        {
            ptz_reset();
            IR_GetCmd();
        }
        break;
        
        case IR_CMD_MENU_OPEN:
        MENU_Open();
        break;
        
        case IR_CMD_MENU_CLOSE:
        MENU_Back();
        break;
        
        case IR_CMD_S_OUT:
        if(CheckInterval(&SNAP_SOUT, DELAY_SOUT))
        {
            cam_reg(REG_VIDEO_FOMAT, SONY_VIDEO_1280x720p50);
        }
        break;
        
        case IR_CMD_DATASCREEN:
        if(CheckInterval(&SNAP_DATASCREEN, DELAY_DATASCREEN))
        {
            if(stCamInfo.datascreen != VISCA_ON)
                cam_datascreen(VISCA_ON);
            else
                cam_datascreen(VISCA_OFF);
        }
        break;
        
        case IR_CMD_HOME:
        if(CheckInterval(&SNAP_PTZ_HOME, DELAY_PTZ_HOME))
        {
            ptz_home();
        }
        break;
        
        case IR_CMD_UP:
        tilt_run(TILT_UP, stRunInfo.tilt_speed, stSysInfo.remote_acc, TILT_REMOTE_DEC);
        SNAP_PAN_TILT = GET_TIME();
        PtIsRunning = 1;
        break;
        
        case IR_CMD_DOWN:
        tilt_run(TILT_DOWN, stRunInfo.tilt_speed, stSysInfo.remote_acc, TILT_REMOTE_DEC);
        SNAP_PAN_TILT = GET_TIME();
        PtIsRunning = 1;
        break;
        
        case IR_CMD_LEFT:
        pan_run(PAN_LEFT, stRunInfo.pan_speed, stSysInfo.remote_acc, PAN_REMOTE_DEC);
        SNAP_PAN_TILT = GET_TIME();
        PtIsRunning = 1;
        break;
        
        case IR_CMD_RIGHT:
        pan_run(PAN_RIGHT, stRunInfo.pan_speed, stSysInfo.remote_acc, PAN_REMOTE_DEC);
        SNAP_PAN_TILT = GET_TIME();
        PtIsRunning = 1;
        break;
        
        case IR_CMD_FOCUS_AUTO:
        cam_focus_mode(VISCA_AUTO);
        break;
        
        case IR_CMD_FOCUS_MANUAL:
        cam_focus_mode(VISCA_MANUAL);
        break;
        
        case IR_CMD_FOCUS_FAR:
        cam_focus(VISCA_FOCUS_FAR_VAR | REMOTE_FOCUS_SPEED);
        SNAP_FOCUS = GET_TIME();
        FocusIsRunning = 1;
        break;
        
        case IR_CMD_FOCUS_NEAR:
        cam_focus(VISCA_FOCUS_NEAR_VAR | REMOTE_FOCUS_SPEED);
        SNAP_FOCUS = GET_TIME();
        FocusIsRunning = 1;
        break;
        
        case IR_CMD_ZOOM_TELE_SLOW:
        cam_zoom(VISCA_ZOOM_TELE_VAR | REMOTE_ZOOM_SLOW_SPEED);
        SNAP_ZOOM = GET_TIME();
        ZoomIsRunning = 1;
        break;
        
        case IR_CMD_ZOOM_WIDE_SLOW:
        cam_zoom(VISCA_ZOOM_WIDE_VAR | REMOTE_ZOOM_SLOW_SPEED);
        SNAP_ZOOM = GET_TIME();
        ZoomIsRunning = 1;
        break;
        
        case IR_CMD_ZOOM_TELE_FAST:
        cam_zoom(VISCA_ZOOM_TELE_VAR | REMOTE_ZOOM_FAST_SPEED);
        SNAP_ZOOM = GET_TIME();
        ZoomIsRunning = 1;
        break;
        
        case IR_CMD_ZOOM_WIDE_FAST:
        cam_zoom(VISCA_ZOOM_WIDE_VAR | REMOTE_ZOOM_FAST_SPEED);
        SNAP_ZOOM = GET_TIME();
        ZoomIsRunning = 1;
        break;
        
        case IR_CMD_BACKLIGHT:
        if(CheckInterval(&SNAP_BACKLIGHT, DELAY_BACKLIGHT))
        {
            if(stCamInfo.backlight != VISCA_ON)
                cam_backlight(VISCA_ON);
            else
                cam_backlight(VISCA_OFF);
        }
        break;
        
        case IR_CMD_BRIGHT_UP:
        if(CheckInterval(&SNAP_BRIGHT, DELAY_BRIGHT))
        {
            cam_expcomp(VISCA_UP);
        }
        break;
        
        case IR_CMD_BRIGHT_DOWN:
        if(CheckInterval(&SNAP_BRIGHT, DELAY_BRIGHT))
        {
            cam_expcomp(VISCA_DOWN);
        }
        break;
        
        case IR_CMD_F1:
        break;
        
        case IR_CMD_F2:
        break;
        
        case IR_CMD_F3:
        break;
        
        case IR_CMD_F4:
        break;
        
        case IR_CMD_F5:
        break;
        
        case IR_CMD_F6:
        break;
        
        case IR_CMD_F7:
        break;
        
        case IR_CMD_F8:
        break;
        
        default:
        {
            uint32_t temp = cmd & 0x00000300;
            uint32_t num = cmd & 0x000000FF;
            if(temp == IR_CMD_NUM_CALL)
                cam_preset(VISCA_MEMORY_CALL, num);
            else if(temp == IR_CMD_NUM_SET)
                cam_preset(VISCA_MEMORY_SET, num);
            else if(temp == IR_CMD_NUM_RESET)
                cam_preset(VISCA_MEMORY_DELETE, num);
            else return;
        }
        break;
    }
    
    SetInqMask();
    LED_Blink();
    IR_GetCmd();
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
static void _CmdHandler_OsdOn(void)
{
    uint32_t cmd = IR_GetCmd();
    switch(cmd)
    {
        case IR_CMD_MENU_OPEN:
        MENU_Open();
        break;
        
        case IR_CMD_MENU_CLOSE:
        MENU_Back();
        break;
        
        case IR_CMD_HOME:
        if(CheckInterval(&SNAP_PTZ_HOME, DELAY_PTZ_HOME))
        {
            ptz_home();
        }
        break;
        
        case IR_CMD_UP:
        tilt_run(TILT_UP, stRunInfo.tilt_speed, stSysInfo.remote_acc, TILT_REMOTE_DEC);
        SNAP_PAN_TILT = GET_TIME();
        PtIsRunning = 1;
        break;
        
        case IR_CMD_DOWN:
        tilt_run(TILT_DOWN, stRunInfo.tilt_speed, stSysInfo.remote_acc, TILT_REMOTE_DEC);
        SNAP_PAN_TILT = GET_TIME();
        PtIsRunning = 1;
        break;
        
        case IR_CMD_LEFT:
        pan_run(PAN_LEFT, stRunInfo.pan_speed, stSysInfo.remote_acc, PAN_REMOTE_DEC);
        SNAP_PAN_TILT = GET_TIME();
        PtIsRunning = 1;
        break;
        
        case IR_CMD_RIGHT:
        pan_run(PAN_RIGHT, stRunInfo.pan_speed, stSysInfo.remote_acc, PAN_REMOTE_DEC);
        SNAP_PAN_TILT = GET_TIME();
        PtIsRunning = 1;
        break;

        default:
        {
            uint32_t temp = cmd & 0x00000300;
            uint32_t num = cmd & 0x000000FF;
            if(temp == IR_CMD_NUM_CALL)
                cam_preset(VISCA_MEMORY_CALL, num);
            else if(temp == IR_CMD_NUM_SET)
                cam_preset(VISCA_MEMORY_SET, num);
            else if(temp == IR_CMD_NUM_RESET)
                cam_preset(VISCA_MEMORY_DELETE, num);
            else return;
        }
        break;
    }

    LED_Blink();
    IR_GetCmd();
}

/***************************************************************************************************
* Description:  待机状态指令响应
***************************************************************************************************/
static void _CmdHandler_Standby(void)
{
    if(IR_GetCmd() == IR_CMD_POWER)
    {
        if(CheckInterval(&SNAP_POWER, DELAY_POWER))
        {
            LED_Blink();
            sys_power(VISCA_ON);
        }
    }
}

/***************************************************************************************************
* Description:  红外指令解码
***************************************************************************************************/
void IrRemoter_CmdHandler(void)
{
    /* 延时任务 */
    _IrRemote_DelayTask();
    
    /* 待机状态 */
    if(stRunInfo.standby != OFF)
    {
        _CmdHandler_Standby();
        return;
    }
    
    /* 菜单打开状态 */
    if(stRunInfo.menu != MENU_OFF)
    {
        _CmdHandler_OsdOn();
        return;
    }

    /* 正常状态 */
    _CmdHandler_Normal();
}

#ifdef __cplusplus
}
#endif
/****************************************** END OF FILE *******************************************/
