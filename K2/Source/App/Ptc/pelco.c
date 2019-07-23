/***************************************************************************************************
* FILE: pelco.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/25, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "pelco.h"
#include "ptc.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define PELCO_D_IRIS_DOWN					1<<10
#define PELCO_D_IRIS_UP						1<<9
#define PELCO_D_FOCUS_FAR					1<<8
#define PELCO_D_FOCUS_NEAR				    1<<7
#define PELCO_D_ZOOM_WIDE					1<<6
#define PELCO_D_ZOOM_TELE					1<<5
#define PELCO_D_PTZ_DOWN					1<<4
#define PELCO_D_PTZ_UP						1<<3
#define PELCO_D_PTZ_LEFT					1<<2
#define PELCO_D_PTZ_RIGHT					1<<1
#define PELCO_D_PRESET_SET			    	0x03
#define PELCO_D_PRESET_CLEAR		    	0x05
#define PELCO_D_PRESET_CALL				    0x07

#define PELCO_P_CAMERA_ON					1<<14
#define PELCO_P_AUTOSCAN					1<<13
#define PELCO_P_CAMERA_OFF			    	1<<12
#define PELCO_P_IRIS_DOWN					1<<11
#define PELCO_P_IRIS_UP						1<<10
#define PELCO_P_FOCUS_FAR					1<<8
#define PELCO_P_FOCUS_NEAR				    1<<9
#define PELCO_P_ZOOM_TELE					1<<5
#define PELCO_P_ZOOM_WIDE					1<<6
#define PELCO_P_PTZ_DOWN					1<<4
#define PELCO_P_PTZ_UP						1<<3
#define PELCO_P_PTZ_LEFT					1<<2
#define PELCO_P_PTZ_RIGHT					1<<1
#define PELCO_P_PRESET_SET				    0x03
#define PELCO_P_PRESET_CLEAR			    0x05
#define PELCO_P_PRESET_CALL				    0x07

static struct
{
	uint8_t head;
	uint8_t addr;
	uint8_t cmd1;
	uint8_t cmd2;
	uint8_t data1;
	uint8_t data2;
	uint8_t check;
}pelco_d_cmd;

static struct
{
	uint8_t head;
	uint8_t addr;
	uint8_t cmd1;
	uint8_t cmd2;
	uint8_t data1;
	uint8_t data2;
	uint8_t stop;
	uint8_t check;
}pelco_p_cmd;

#define PELCOD_CUR_ADDR()   (stSysPara.ptc.pelco_d.cam_addr)
#define PELCOP_CUR_ADDR()   (stSysPara.ptc.pelco_p.cam_addr)
/***************************************************************************************************
* VARIABLES
***************************************************************************************************/


/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/


/***************************************************************************************************
* Description:  
***************************************************************************************************/
void pelco_d_send_command(uint8_t addr, uint16_t event, uint16_t value)
{
    memset(&pelco_d_cmd, 0, 7); //清空结构体
    
    pelco_d_cmd.head = 0xFF;
    pelco_d_cmd.addr = addr;
    
    pelco_d_cmd.cmd1 = event >> 8;
    pelco_d_cmd.cmd2 = event & 0xFF;
    
    pelco_d_cmd.data1 = value >> 8;
    pelco_d_cmd.data2 = value & 0xFF;
    
    pelco_d_cmd.check = pelco_d_cmd.addr + \
        pelco_d_cmd.cmd1 + pelco_d_cmd.cmd2 + \
            pelco_d_cmd.data1 + pelco_d_cmd.data2;
    
    UartSentPacket((const uint8_t *)&pelco_d_cmd, 7);
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
uint32_t PelcoD_GetCurAddr(void)
{
    return PELCOD_CUR_ADDR();
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t PelcoD_SelectDevice(int32_t arg)
{
    if(arg < 0 || arg > 255)
    {
        UI_ShowAction("Out Of Range!", 0);
        return 0;
    }
    PELCOD_CUR_ADDR() = arg;
    char str[30] = {0};
    sprintf(str, "Select Camera %d", arg);
    UI_ShowAction(str, 0);
    UI_ShowCamAddr();
    System_SavePara(SYS_SAVE_PELCOD_ADDR);
    return 0;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t PelcoD_Preset_Call(int32_t arg)
{    
    char str[30] = {0};
    if(arg > 255)
        UI_ShowAction("Out Of Range!", 0);
    else
    {
        pelco_d_send_command(PELCOD_CUR_ADDR(), PELCO_D_PRESET_CALL, (uint16_t)arg);
        sprintf(str, "Call Preset %d", arg);
        UI_ShowAction(str, 0); 
    }
    return 0;
}
int32_t PelcoD_Preset_Set(int32_t arg)
{    
    char str[30] = {0};
    if(arg > 255)
        UI_ShowAction("Out Of Range!", 0);
    else
    {
        pelco_d_send_command(PELCOD_CUR_ADDR(), PELCO_D_PRESET_SET, (uint16_t)arg);
        sprintf(str, "Set Preset %d", arg);
        UI_ShowAction(str, 0); 
    }
    return 0;
}
int32_t PelcoD_Preset_Delete(int32_t arg)
{    
    char str[30] = {0};
    if(arg > 255)
        UI_ShowAction("Out Of Range!", 0);
    else
    {
        pelco_d_send_command(PELCOD_CUR_ADDR(), PELCO_D_PRESET_CLEAR, (uint16_t)arg);
        sprintf(str, "Clear Preset %d", arg);
        UI_ShowAction(str, 0); 
    }
    return 0;
}

/***************************************************************************************************
* Description:  调用OSD
***************************************************************************************************/
int32_t PelcoD_Osd(int32_t arg)
{
    char str[30] = {0};
    pelco_d_send_command(PELCOD_CUR_ADDR(), PELCO_D_PRESET_CALL, 95);
    sprintf(str, "Call Camera OSD");
    UI_ShowAction(str, 0); 
    return 0;
}

/***************************************************************************************************
* Description:  聚焦
***************************************************************************************************/
int32_t PelcoD_Focus_Far(int32_t arg)
{
    char str[30] = {0};
    
    sprintf(str, "Focus Far");
    UI_ShowAction(str, 0); 
    pelco_d_send_command(PELCOD_CUR_ADDR(), PELCO_D_FOCUS_FAR, 0);
    HAL_Delay(30);
    pelco_d_send_command(PELCOD_CUR_ADDR(), 0, 0);
    pelco_d_send_command(PELCOD_CUR_ADDR(), 0, 0);
    return 0;
}
int32_t PelcoD_Focus_Near(int32_t arg)
{
    char str[30] = {0};
    
    sprintf(str, "Focus Near");
    UI_ShowAction(str, 0);
    pelco_d_send_command(PELCOD_CUR_ADDR(), PELCO_D_FOCUS_NEAR, 0);
    HAL_Delay(30);
    pelco_d_send_command(PELCOD_CUR_ADDR(), 0, 0);
    pelco_d_send_command(PELCOD_CUR_ADDR(), 0, 0);
    
    return 0;
}

/***************************************************************************************************
* Description:  云台
***************************************************************************************************/
static uint16_t pelco_d_pan_sp, pelco_d_tilt_sp, pelco_d_pan_dir, pelco_d_tilt_dir = 0;
static uint16_t pelco_d_zoom = 0;
int32_t PelcoD_Rocker_PanTilt(int32_t pan_val, int32_t tilt_val, int32_t grade)
{
    uint8_t max_sp = 64/8*grade;
    
    pelco_d_pan_sp = max_sp * abs(pan_val) / 30;
    if(pelco_d_pan_sp > 64)  pelco_d_pan_sp = 64;
    pelco_d_tilt_sp = max_sp * abs(tilt_val) / 30;
    if(pelco_d_tilt_sp > 64)  pelco_d_tilt_sp = 64;
    
    if(pan_val > 0) pelco_d_pan_dir = PELCO_D_PTZ_RIGHT;
    else if(pan_val < 0) pelco_d_pan_dir = PELCO_D_PTZ_LEFT;
    else pelco_d_pan_dir = 0;
    
    if(tilt_val > 0) pelco_d_tilt_dir = PELCO_D_PTZ_UP;
    else if(tilt_val < 0) pelco_d_tilt_dir = PELCO_D_PTZ_DOWN;
    else pelco_d_tilt_dir = 0;
    
    pelco_d_send_command(PELCOD_CUR_ADDR(), pelco_d_zoom|pelco_d_pan_dir|pelco_d_tilt_dir, (pelco_d_pan_sp<<8)|pelco_d_tilt_sp);
    
    return 0;
}

int32_t PelcoD_Rocker_Zoom(int32_t arg, int32_t grade)
{
    if(arg > 10) pelco_d_zoom = PELCO_D_ZOOM_WIDE;
    else if(arg < -10) pelco_d_zoom = PELCO_D_ZOOM_TELE;
    else pelco_d_zoom = 0;
    
    pelco_d_send_command(PELCOD_CUR_ADDR(), pelco_d_zoom|pelco_d_pan_dir|pelco_d_tilt_dir, (pelco_d_pan_sp<<8)|pelco_d_tilt_sp);
    
    return 0;
}


/* 事件响应函数结构体 */
event_handler_t pelcod_event_handler = 
{
    /* 获取当前目标地址 */
    PelcoD_GetCurAddr,
    
    /* 按键任务 */
    NULL,
    PelcoD_SelectDevice,
    PelcoD_Preset_Call,
    PelcoD_Preset_Set,
    PelcoD_Preset_Delete,
    
    NULL,
    PelcoD_Osd,
    NULL,
    NULL,
    NULL,
    NULL,
    
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PelcoD_Focus_Far,
    PelcoD_Focus_Near,
    
    /* 摇杆按键 */
    NULL,
    
    /* 摇杆电位器 */
    PelcoD_Rocker_PanTilt,
    NULL,
    NULL,
    PelcoD_Rocker_Zoom,
    
    NULL,
};

/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t PelcoD_Init(void)
{
    pevent_handler = &pelcod_event_handler;
    
    return 0;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t PelcoD_DeInit(void)
{
    
    
    return 0;
}


















/***************************************************************************************************
* Description:  
***************************************************************************************************/
void pelco_p_send_command(uint8_t addr, uint16_t event, uint16_t value)
{
    memset(&pelco_p_cmd, 0, 8); //清空结构体
    
    pelco_p_cmd.head = 0xA0;
    if(addr == 0) addr = 255;
    else addr--;
    pelco_p_cmd.addr = addr;
    
    pelco_p_cmd.cmd1 = event >> 8;
    pelco_p_cmd.cmd2 = event & 0xFF;
    
    pelco_p_cmd.data1 = value >> 8;
    pelco_p_cmd.data2 = value & 0xFF;
    
    pelco_p_cmd.stop = 0xAF;
    
    pelco_p_cmd.check = 0xA0 ^ addr ^ \
        (event >> 8) ^ (event & 0xFF) ^ \
            (value >> 8) ^ (value & 0xFF) ^ 0xAF;
    
    UartSentPacket((const uint8_t *)&pelco_p_cmd, 8);
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
uint32_t PelcoP_GetCurAddr(void)
{
    return PELCOP_CUR_ADDR();
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t PelcoP_SelectDevice(int32_t arg)
{
    if(arg < 0 || arg > 255)
    {
        UI_ShowAction("Out Of Range!", 0);
        return 0;
    }
    PELCOP_CUR_ADDR() = arg;
    char str[30] = {0};
    sprintf(str, "Select Camera %d", arg);
    UI_ShowAction(str, 0);
    UI_ShowCamAddr();
    System_SavePara(SYS_SAVE_PELCOP_ADDR);
    return 0;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t PelcoP_Preset_Call(int32_t arg)
{    
    char str[30] = {0};
    if(arg > 255)
        UI_ShowAction("Out Of Range!", 0);
    else
    {
        pelco_p_send_command(PELCOP_CUR_ADDR(), PELCO_P_PRESET_CALL, (uint16_t)arg);
        sprintf(str, "Call Preset %d", arg);
        UI_ShowAction(str, 0); 
    }
    return 0;
}
int32_t PelcoP_Preset_Set(int32_t arg)
{    
    char str[30] = {0};
    if(arg > 255)
        UI_ShowAction("Out Of Range!", 0);
    else
    {
        pelco_p_send_command(PELCOP_CUR_ADDR(), PELCO_P_PRESET_SET, (uint16_t)arg);
        sprintf(str, "Set Preset %d", arg);
        UI_ShowAction(str, 0); 
    }
    return 0;
}
int32_t PelcoP_Preset_Delete(int32_t arg)
{    
    char str[30] = {0};
    if(arg > 255)
        UI_ShowAction("Out Of Range!", 0);
    else
    {
        pelco_p_send_command(PELCOP_CUR_ADDR(), PELCO_P_PRESET_CLEAR, (uint16_t)arg);
        sprintf(str, "Clear Preset %d", arg);
        UI_ShowAction(str, 0); 
    }
    return 0;
}

/***************************************************************************************************
* Description:  调用OSD
***************************************************************************************************/
int32_t PelcoP_Osd(int32_t arg)
{
    char str[30] = {0};
    pelco_p_send_command(PELCOP_CUR_ADDR(), PELCO_P_PRESET_CALL, 95);
    sprintf(str, "Call Camera OSD");
    UI_ShowAction(str, 0); 
    return 0;
}

/***************************************************************************************************
* Description:  聚焦
***************************************************************************************************/
int32_t PelcoP_Focus_Far(int32_t arg)
{
    char str[30] = {0};
    
    sprintf(str, "Focus Far");
    UI_ShowAction(str, 0); 
    pelco_p_send_command(PELCOP_CUR_ADDR(), PELCO_P_FOCUS_FAR, 0);
    HAL_Delay(30);
    pelco_p_send_command(PELCOP_CUR_ADDR(), 0, 0);
    pelco_p_send_command(PELCOP_CUR_ADDR(), 0, 0);
    return 0;
}
int32_t PelcoP_Focus_Near(int32_t arg)
{
    char str[30] = {0};
    
    sprintf(str, "Focus Near");
    UI_ShowAction(str, 0);
    pelco_p_send_command(PELCOP_CUR_ADDR(), PELCO_P_FOCUS_NEAR, 0);
    HAL_Delay(30);
    pelco_p_send_command(PELCOP_CUR_ADDR(), 0, 0);
    pelco_p_send_command(PELCOP_CUR_ADDR(), 0, 0);
    
    return 0;
}

/***************************************************************************************************
* Description:  云台
***************************************************************************************************/
static uint16_t pelco_p_pan_sp, pelco_p_tilt_sp, pelco_p_pan_dir, pelco_p_tilt_dir = 0;
static uint16_t pelco_p_zoom = 0;
int32_t PelcoP_Rocker_PanTilt(int32_t pan_val, int32_t tilt_val, int32_t grade)
{
    uint8_t max_sp = 64/8*grade;
    
    pelco_p_pan_sp = max_sp * abs(pan_val) / 30;
    if(pelco_p_pan_sp > 64)  pelco_p_pan_sp = 64;
    pelco_p_tilt_sp = max_sp * abs(tilt_val) / 30;
    if(pelco_p_tilt_sp > 64)  pelco_p_tilt_sp = 64;
    
    if(pan_val > 0) pelco_p_pan_dir = PELCO_P_PTZ_RIGHT;
    else if(pan_val < 0) pelco_p_pan_dir = PELCO_P_PTZ_LEFT;
    else pelco_p_pan_dir = 0;
    
    if(tilt_val > 0) pelco_p_tilt_dir = PELCO_P_PTZ_UP;
    else if(tilt_val < 0) pelco_p_tilt_dir = PELCO_P_PTZ_DOWN;
    else pelco_p_tilt_dir = 0;
    
    pelco_p_send_command(PELCOP_CUR_ADDR(), pelco_p_zoom|pelco_p_pan_dir|pelco_p_tilt_dir, (pelco_p_pan_sp<<8)|pelco_p_tilt_sp);
    
    return 0;
}

int32_t PelcoP_Rocker_Zoom(int32_t arg, int32_t grade)
{
    if(arg > 10) pelco_p_zoom = PELCO_P_ZOOM_WIDE;
    else if(arg < -10) pelco_p_zoom = PELCO_P_ZOOM_TELE;
    else pelco_p_zoom = 0;
    
    pelco_p_send_command(PELCOP_CUR_ADDR(), pelco_p_zoom|pelco_p_pan_dir|pelco_p_tilt_dir, (pelco_p_pan_sp<<8)|pelco_p_tilt_sp);
    
    return 0;
}

/* 事件响应函数结构体 */
event_handler_t pelcop_event_handler = 
{
    /* 获取当前目标地址 */
    PelcoP_GetCurAddr,
    
    /* 按键任务 */
    NULL,
    PelcoP_SelectDevice,
    PelcoP_Preset_Call,
    PelcoP_Preset_Set,
    PelcoP_Preset_Delete,
    
    NULL,
    PelcoP_Osd,
    NULL,
    NULL,
    NULL,
    NULL,
    
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PelcoP_Focus_Far,
    PelcoP_Focus_Near,
    
    /* 摇杆按键 */
    NULL,
    
    /* 摇杆电位器 */
    PelcoP_Rocker_PanTilt,
    NULL,
    NULL,
    PelcoP_Rocker_Zoom,
    
    NULL,
};

/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t PelcoP_Init(void)
{
    pevent_handler = &pelcop_event_handler;
    
    return 0;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t PelcoP_DeInit(void)
{
    
    
    return 0;
}

/****************************************** END OF FILE *******************************************/
