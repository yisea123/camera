/***************************************************************************************************
* FILE: main.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/1, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "system.h"
#include "icon.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/
TaskHandle_t IdleTask;
TaskHandle_t InitTask;
TaskHandle_t MasterTask;
TaskHandle_t RefreshTask;
TaskHandle_t RockerRefreshTask;
TaskHandle_t KeyRefreshTask;
TaskHandle_t NetTask;
extern TaskHandle_t ViscaIpRcvTask;


/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void func_IdleTask(void * argument);
void func_InitTask(void * argument);
void func_RefreshTask(void * argument);
void func_RockerRefreshTask(void * argument);
void func_KeyRefreshTask(void * argument);
void func_ViscaIpRcvTask(void * argument);
void func_NetTask(void * argument);


/***************************************************************************************************
* Description:  入口
***************************************************************************************************/
void main(void)
{
    Bsp_Init();
    
    printf("\r\n\r\nBsp Init Complete! Start Scheduler!\r\n\r\n");
    
    xTaskCreate(&func_IdleTask, "Idle Task", 512, 0, 0, &IdleTask);
    xTaskCreate(&func_InitTask, "Init Task", 512, 0, 15, &InitTask);

    vTaskStartScheduler();
}

/***************************************************************************************************
* Description:  闲置任务
***************************************************************************************************/
void func_IdleTask(void * argument)
{
    for(;;)
    {
        /* 闲置处理 */
        IDLE_Handler();     //屏幕亮度处理
    }
}

void func_InitTask(void * argument)
{
    /* 创建网络状态监测任务 */
    xTaskCreate(&func_NetTask, "Net Task", 512, 0, 1, &NetTask);    //检测网络连接状态
    
    /* 创建VISCA Over IP接收任务，先挂起 */
    xTaskCreate(&func_ViscaIpRcvTask, "VISCA IP Receive Task", 512, 0, 5, &ViscaIpRcvTask);
    vTaskSuspend(ViscaIpRcvTask);

    /* 读取启动标记，如果是第一次启动则载入默认参数，否则读取保存的参数 */
    if(System_CheckVirgin())
    {
        System_ResetPara();
    }
    else
    {
        System_ReadPara();
    }
    
    /* 按键灯光初始化 */
    SN3731_Init();
    
    /* 输入按键初始化 */
    Key_Init();
    
    /* 显示屏以及EMWIN初始化 */
    LCD_Init();
    WM_SetCreateFlags(WM_CF_MEMDEV);
    GUI_Init();
    GUI_Exec();
    GUI_SetBkColor(0xFFFFFF);
    GUI_Clear();
    
#if defined(NEUTRAL)
    GUI_SetColor(BK_COLOR_0);
	GUI_FillRect(0, 0, 399, 239);
    GUI_PNG_Draw(_acInitializing, sizeof(_acInitializing), 71, 87);
#else
    GUI_SetColor(BK_COLOR_0);
	GUI_FillRect(0, 0, 399, 239);
    GUI_DrawBitmap(&bmkatov_mini, 60, 75);
#endif
    LCD_SetBacklight(256);
    
    stRunInfo.menu = -1;
   
    /* 初始化串口模块 */
    UART_Init();
    
    /* 网络协议栈初始化 */
    LWIP_Init();
    
    /* 粗略等待DHCP以及网络协议栈稳定 */
    while(HAL_GetTick() < 10000)
    {
        vTaskDelay(100);
    }
    
    /* 初始化控制协议 */
    Ptc_Init();
    
    /* 创建主循环任务和IO刷新任务 */
    xTaskCreate(&func_MasterTask, "Master Task", 1024, 0, 1, &MasterTask);
    xTaskCreate(&func_RefreshTask, "Refresh Task", 512, 0, 2, &RefreshTask);
    xTaskCreate(&func_RockerRefreshTask, "Rocker Refresh Task", 512, 0, 3, &RockerRefreshTask);
    xTaskCreate(&func_KeyRefreshTask, "Key Refresh Task", 512, 0, 4, &KeyRefreshTask);
    
    if(stSysPara.net.dhcp)
    {
        stSysPara.net.ipaddr = SwapIp(gnetif.ip_addr.addr);
        stSysPara.net.mask = SwapIp(gnetif.netmask.addr);
        stSysPara.net.gateway = SwapIp(gnetif.gw.addr);
    }
    
    for(;;)
    {
        vTaskDelay(1000);
    }
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
void func_RefreshTask(void * argument)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 50;
    
    xLastWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        LED_CheckOff();
        Buzzer_CheckOff();
        SN3731_RefreshFrame();
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}
void func_RockerRefreshTask(void * argument)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 20;
    
    xLastWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        Rocker_Refresh();
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}
void func_KeyRefreshTask(void * argument)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 10;
    
    xLastWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        KeyScan();
//        vTaskDelay(20);
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}


/****************************************** END OF FILE *******************************************/

