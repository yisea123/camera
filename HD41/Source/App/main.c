/***************************************************************************************************
* FILE: main.c
*
* DESCRIPTION:  --
*
* CREATED: 2017/10/30, by kevin xu
***************************************************************************************************/
#include "bsp.h"
#include "system.h"
#include "ms41909.h"
#include "ccp.h"
#include "ir_remote.h"


/***************************************************************************************************
* Description:  入口函数
***************************************************************************************************/
void main()
{
    /* 单片机基础外设初始化 */
    BSP_Init();
    
    /* 系统时间初始化 */
    System_TimerInit();
    
    /* 状态灯全亮 */
    LED_POWER_ON;
    LED_STANDBY_ON;
    
    /* 等待电源稳定 */
    while(stRunInfo.time < 1000){}
    
    /* 打开Camera电源 */
    CAM_RESTART();
    
    /* 等待电源稳定 */
    while(stRunInfo.time < 2000){}
    
    /* 电机驱动初始化,云台自检 */
    PanTilt_Init();
    PanTilt_Reset(0, 0);
    
    /* 检查预置位使用情况 */
    PresetCheckUsed();
    
    /* 等待Camera启动完毕 */
    while(stRunInfo.time < 30000){}
    
    /* 第一次启动 */
    if(System_CheckVirgin())
    {
        RESET_ALL:
        memset(&stSysInfo, 0, sizeof(SYSTEM_INFO_ST));
        stSysInfo.video_format_out1 = VIDEO_1920x1080p60;
        System_ResetPara();
        cam_reset_picture();
#if defined(EN_NETWORK)
        cam_reset_net();
#endif
        System_SetVirgin();
    }
    /* 不是第一次启动 */
    else
    {
        /* 读取系统参数 */
        if(System_ReadPara())
            goto RESET_ALL;
        
        /* 控制协议栈初始化 */
        Ptc_Init();
    }
    
    /* 查询机芯参数 */
    InqCamZoomFocus();
    InqCamWb();
    InqCamExposure();
    InqCamPicture();
    InqCamNet();
    
    /* 视频模块初始化 */
    Video_Init();
    
    /* 云台参数初始化 */
    Ptz_Init();
    
    /* 遥控模块初始化 */
    IrRemoter_Init();
    
    /* 显示简要信息 */
    ShowBrief();
    
    /* 调用1号预置位 */
    if(stSysInfo.call_preset1)
    {
        cam_preset(PRESET_CALL, 1);
    }
    else
    {
        ptz_home();
    }
    
    /* 系统初始化完毕,待机状态灯关闭 */
    LED_POWER_ON;
    LED_STANDBY_OFF;
    
    while(1)
    {
        IrRemoter_CmdHandler();
        Ccp_ProcessHandler();
        InqCamInfo();
        AgingTask();
    }
}

#ifdef __cplusplus
extern "C" {
#endif
    /******************************************************************************/
    /*            Cortex-M3 Processor Exceptions Handlers                         */
    /******************************************************************************/
    
    /**
    * @brief  This function handles NMI exception.
    * @param  None
    * @retval None
    */
    void NMI_Handler(void)
    {
    }
    
    /**
    * @brief  This function handles Hard Fault exception.
    * @param  None
    * @retval None
    */
    void HardFault_Handler(void)
    {
        /* Go to infinite loop when Hard Fault exception occurs */
        while(1)
        {
        }
    }
    
    /**
    * @brief  This function handles Memory Manage exception.
    * @param  None
    * @retval None
    */
    void MemManage_Handler(void)
    {
        /* Go to infinite loop when Memory Manage exception occurs */
        while(1)
        {
        }
    }
    
    /**
    * @brief  This function handles Bus Fault exception.
    * @param  None
    * @retval None
    */
    void BusFault_Handler(void)
    {
        /* Go to infinite loop when Bus Fault exception occurs */
        while(1)
        {
        }
    }
    
    /**
    * @brief  This function handles Usage Fault exception.
    * @param  None
    * @retval None
    */
    void UsageFault_Handler(void)
    {
        /* Go to infinite loop when Usage Fault exception occurs */
        while(1)
        {
        }
    }
    
    /**
    * @brief  This function handles SVCall exception.
    * @param  None
    * @retval None
    */
    void SVC_Handler(void)
    {
    }
    
    /**
    * @brief  This function handles Debug Monitor exception.
    * @param  None
    * @retval None
    */
    void DebugMon_Handler(void)
    {
    }
    
    /**
    * @brief  This function handles PendSVC exception.
    * @param  None
    * @retval None
    */
    void PendSV_Handler(void)
    {
    }
    
    /**
    * @brief  This function handles SysTick Handler.
    * @param  None
    * @retval None
    */
    void SysTick_Handler(void)
    {
        
        
        
    }
#ifdef __cplusplus
}
#endif

/****************************************** END OF FILE *******************************************/
