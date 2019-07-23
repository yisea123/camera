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
* Description:  ��ں���
***************************************************************************************************/
void main()
{
    /* ��Ƭ�����������ʼ�� */
    BSP_Init();
    
    /* ϵͳʱ���ʼ�� */
    System_TimerInit();
    
    /* ״̬��ȫ�� */
    LED_POWER_ON;
    LED_STANDBY_ON;
    
    /* �ȴ���Դ�ȶ� */
    while(stRunInfo.time < 1000){}
    
    /* ��Camera��Դ */
    CAM_RESTART();
    
    /* �ȴ���Դ�ȶ� */
    while(stRunInfo.time < 2000){}
    
    /* ���������ʼ��,��̨�Լ� */
    PanTilt_Init();
    PanTilt_Reset(0, 0);
    
    /* ���Ԥ��λʹ����� */
    PresetCheckUsed();
    
    /* �ȴ�Camera������� */
    while(stRunInfo.time < 30000){}
    
    /* ��һ������ */
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
    /* ���ǵ�һ������ */
    else
    {
        /* ��ȡϵͳ���� */
        if(System_ReadPara())
            goto RESET_ALL;
        
        /* ����Э��ջ��ʼ�� */
        Ptc_Init();
    }
    
    /* ��ѯ��о���� */
    InqCamZoomFocus();
    InqCamWb();
    InqCamExposure();
    InqCamPicture();
    InqCamNet();
    
    /* ��Ƶģ���ʼ�� */
    Video_Init();
    
    /* ��̨������ʼ�� */
    Ptz_Init();
    
    /* ң��ģ���ʼ�� */
    IrRemoter_Init();
    
    /* ��ʾ��Ҫ��Ϣ */
    ShowBrief();
    
    /* ����1��Ԥ��λ */
    if(stSysInfo.call_preset1)
    {
        cam_preset(PRESET_CALL, 1);
    }
    else
    {
        ptz_home();
    }
    
    /* ϵͳ��ʼ�����,����״̬�ƹر� */
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
