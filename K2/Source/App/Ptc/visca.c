/***************************************************************************************************
* FILE: visca.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/25, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "visca.h"
#include "visca_if.h"
#include "ptc.h"
#include "stm32f2xx_hal_uart.h"
#include "stm32f207xx.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define VISCA_CUR_ADDR()    (stSysPara.ptc.visca.cam_addr)

#define VISCA_COMPATIBLE_MODE() (stSysPara.ptc.visca.compatible_mode)


/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void UartSentCmd(const uint8_t *pk, uint8_t length);
void UartSentInq(const uint8_t *pk, uint8_t length);
void ViscaClearReturnBuffer(void);
int32_t ViscaGetReturnCode(uint8_t *buffer);

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/
const static struct visca_if visca_if_serial = 
{
    vTaskDelay,
    UartSentCmd,
    UartSentInq,
    ViscaClearReturnBuffer,
    ViscaGetReturnCode,
};

uint32_t visca_inq_failure_count = 0;

static uint32_t osd_timer = 0;
#define UPDATE_ACT_TIMER()  do { visca_cam_status.act_timer = HAL_GetTick(); } while(0)

static struct visca_camera_status visca_cam_status = {0};

/***************************************************************************************************
* Description:  接口驱动
***************************************************************************************************/
void UartSentCmd(const uint8_t *pk, uint8_t length)
{
    UartSentPacket(pk, length);
    UPDATE_ACT_TIMER();
}
void UartSentInq(const uint8_t *pk, uint8_t length)
{
    UartSentPacket(pk, length);
}

/***************************************************************************************************
* Description:  清除回码缓存
***************************************************************************************************/
void ViscaClearReturnBuffer(void)
{
    PacketList_Destroy(&UartRxList);
}
  

/***************************************************************************************************
* Description:  读取一个回码包
***************************************************************************************************/
int32_t ViscaGetReturnCode(uint8_t *buffer)
{
    uint8_t *pk;
    uint32_t length;
    
    if(PacketList_Get(&UartRxList, &pk, &length))
    {
        return 0;
    }
    
    memcpy(buffer, pk, length);
    //    if(length < 16)
    //    memset(buffer[length]);
    
    PacketList_DelFirst(&UartRxList);
    
    return length;
}

/***************************************************************************************************
* Description:  获取当前地址
***************************************************************************************************/
uint32_t Visca_GetCurAddr(void)
{
    return stSysPara.ptc.visca.cam_addr;
}

/***************************************************************************************************
* Description:  搜索级联的设备
***************************************************************************************************/
extern TaskHandle_t RockerRefreshTask;
int32_t Visca_SearchDevice(int32_t arg)
{
    uint8_t cam_num = 0;
    
    UI_ShowAction("Searching Visca Device...", 1);
//    vTaskSuspend(RockerRefreshTask);
    
    cam_num = visca_reset_address(3000);
//    vTaskResume(RockerRefreshTask);
    
    char str[30] = {' '};
    sprintf(str, "Find %d Device", cam_num);
    if(cam_num > 1) strcat(str, "s");
    UI_ShowAction(str, 1);
    
    return 0;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
void Visca_SetTally(int32_t num)
{
    if(num > 7) return;
    
    TALLY_1 = 0;
    TALLY_2 = 0;
    TALLY_3 = 0;
    TALLY_4 = 0;
    TALLY_5 = 0;
    TALLY_6 = 0;
    TALLY_7 = 0;
    
    if(num == 1) TALLY_1 = 1;
    else if(num == 2) TALLY_2 = 1;
    else if(num == 3) TALLY_3 = 1;
    else if(num == 4) TALLY_4 = 1;
    else if(num == 5) TALLY_5 = 1;
    else if(num == 6) TALLY_6 = 1;
    else if(num == 7) TALLY_7 = 1;
    
    visca_set_tally(num, VISCA_ON, 0);
}

/***************************************************************************************************
* Description:  选择设备
***************************************************************************************************/
int32_t Visca_SelectDevice(int32_t arg)
{
    char str[30] = {0};
    
    if(arg < 1 || arg > 8)
    {
        UI_ShowAction("Out Of Range!", 0);
        return 0;
    }
    
    visca_set_tally(VISCA_CUR_ADDR(), VISCA_OFF, 0);
    stSysPara.ptc.visca.cam_addr = arg;
    Visca_SetTally(arg);
    
    sprintf(str, "Select Camera %d", arg);
    UI_ShowAction(str, 0);
    UI_ShowCamAddr();
    System_SavePara(SYS_SAVE_VISCA_ADDR);
    
    visca_inq_failure_count = 0;
    memset(&visca_cam_status, 0, sizeof(struct visca_camera_status));
    return 0;
}

/***************************************************************************************************
* Description:  预置位
***************************************************************************************************/
int32_t Visca_Preset_Call(int32_t arg)
{
    char str[30];
    if(arg > 255)
        UI_ShowAction("Out Of Range!", 0);
    else
    {
        visca_set_memory(VISCA_CUR_ADDR(), VISCA_PRESET_CALL, arg, 0);
        sprintf(str, "Call Preset %d", arg);
        UI_ShowAction(str, 0); 
    }
    return 0;
}
int32_t Visca_Preset_Set(int32_t arg)
{
    char str[30];
    if(arg > 255)
        UI_ShowAction("Out Of Range!", 0);
    else
    {
        visca_set_memory(VISCA_CUR_ADDR(), VISCA_PRESET_SET, arg, 0);
        sprintf(str, "Set Preset %d", arg);
        UI_ShowAction(str, 0); 
    }
    return 0;
}
int32_t Visca_Preset_Delete(int32_t arg)
{
    char str[30] = {0};
    if(arg > 255)
        UI_ShowAction("Out Of Range!", 0);
    else
    {
        visca_set_memory(VISCA_CUR_ADDR(), VISCA_PRESET_DELETE, arg, 0);
        sprintf(str, "Delete Preset %d", arg);
        UI_ShowAction(str, 0);
    }
    return 0;
}

/***************************************************************************************************
* Description:  OSD
***************************************************************************************************/
int32_t Visca_Osd(int32_t arg)
{
    char str[30] = {0};

    sprintf(str, "Call Camera OSD");
    UI_ShowAction(str, 0);
    
    switch(VISCA_COMPATIBLE_MODE())
    {
        case VISCA_COMPATIBLE_SONY:
        visca_set_osd(VISCA_CUR_ADDR(), 0x10, 0);
        visca_cam_status.osd = VISCA_ON;
        vTaskDelay(50);
        visca_inq_osd(VISCA_CUR_ADDR(), &visca_cam_status.osd, 0);
        break;
        
        case VISCA_COMPATIBLE_JJTS:
        visca_set_memory(VISCA_CUR_ADDR(), VISCA_PRESET_CALL, 95, 0);
        break;

        case VISCA_COMPATIBLE_VHD:
        visca_set_memory(VISCA_CUR_ADDR(), VISCA_PRESET_CALL, 95, 0);
        visca_cam_status.osd = VISCA_ON;
        vTaskDelay(50);
        visca_inq_osd(VISCA_CUR_ADDR(), &visca_cam_status.osd, 0);
        break;
        
        case VISCA_COMPATIBLE_MINRRAY:
        visca_set_memory(VISCA_CUR_ADDR(), VISCA_PRESET_CALL, 95, 0);
        visca_cam_status.osd = VISCA_ON;
        vTaskDelay(50);
        visca_inq_osd(VISCA_CUR_ADDR(), &visca_cam_status.osd, 0);
        break;
    }
    
    return 0;
}

/***************************************************************************************************
* Description:  背光
***************************************************************************************************/
int32_t Visca_Backlight(int32_t arg)
{
    char str[30] = {0};
    if(visca_cam_status.backlight == VISCA_ON)
    {
        visca_cam_status.backlight = VISCA_OFF;
        strcat(str, "Backlight OFF");
    }
    else
    {
        visca_cam_status.backlight = VISCA_ON;
        strcat(str, "Backlight ON");
    }
    
    visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_BACKLIGHT, visca_cam_status.backlight, 0);
    UI_ShowAction(str, 0); 
    
    return 0;
}

/***************************************************************************************************
* Description:  曝光控制
***************************************************************************************************/
int32_t Visca_Ae_Mode(int32_t arg)
{
    char str[30] = {0};
    if(visca_cam_status.exposure_mode == VISCA_AE_FULL_AUTO)
    {
        visca_cam_status.exposure_mode = VISCA_AE_SHUTTER_PRIORITY;
        sprintf(str, "Set to Shutter Priority");
    }
    else if(visca_cam_status.exposure_mode == VISCA_AE_SHUTTER_PRIORITY)
    {
        visca_cam_status.exposure_mode = VISCA_AE_IRIS_PRIORITY;
        sprintf(str, "Set to Iris Priority");
    }
    else
    {
        visca_cam_status.exposure_mode = VISCA_AE_FULL_AUTO;
        sprintf(str, "Set to Full Auto");
    }
    
    visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_AE, visca_cam_status.exposure_mode, 0);
    UI_ShowAction(str, 0); 
    
    return 0;
}
int32_t Visca_Ae_Add(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_cam_status.exposure_mode == VISCA_AE_SHUTTER_PRIORITY)
    {
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_SHUTTER, VISCA_SHUTTER_UP, 0);
        sprintf(str, "Shutter Up");
    }
    else if(visca_cam_status.exposure_mode == VISCA_AE_IRIS_PRIORITY)
    {
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_IRIS, VISCA_IRIS_UP, 0);
        sprintf(str, "Iris Up");
    }
    else
    {
        sprintf(str, "Not Available In Automatic Mode");
    }
    UI_ShowAction(str, 0); 
    
    return 0;
}
int32_t Visca_Ae_Sub(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_cam_status.exposure_mode == VISCA_AE_SHUTTER_PRIORITY)
    {
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_SHUTTER, VISCA_SHUTTER_DOWN, 0);
        sprintf(str, "Shutter Down");
    }
    else if(visca_cam_status.exposure_mode == VISCA_AE_IRIS_PRIORITY)
    {
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_IRIS, VISCA_IRIS_DOWN, 0);
        sprintf(str, "Iris Down");
    }
    else
    {
        sprintf(str, "Not Available In Automatic Mode");
    }
    UI_ShowAction(str, 0); 
    
    return 0;
}

/***************************************************************************************************
* Description:  白平衡
***************************************************************************************************/
const char textViscaWbMode[][30] = 
{
    "Auto White Balance",
    "Indoor Mode",
    "Outdoor Mode",
    "One Push Mode",
    "ATW Mode",
    "Manual White Balance",
};
int32_t Visca_Wb_Mode(int32_t arg)
{
    char str[30] = {0};
    
    ++visca_cam_status.wb_mode;
    if(visca_cam_status.wb_mode > VISCA_WB_MANUAL)
    {
        visca_cam_status.wb_mode = VISCA_WB_AUTO;
    }
    
    visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_WB, visca_cam_status.wb_mode, 0);
    strcat(str, textViscaWbMode[visca_cam_status.wb_mode]);
    
    UI_ShowAction(str, 0);
    return 0;
}
int32_t Visca_Rgain_Add(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_cam_status.wb_mode == VISCA_WB_ONE_PUSH_WB)
    {
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_WB_ONE_PUSH_TRIG, 0x05, 0);
        sprintf(str, "Trigger A White Balance");
    }
    else
    {
        if(visca_cam_status.wb_mode != VISCA_WB_MANUAL)
        {
            visca_cam_status.wb_mode = VISCA_WB_MANUAL;
            visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_WB, visca_cam_status.wb_mode, 0);
            vTaskDelay(50);
        }
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_RGAIN, VISCA_RGAIN_UP, 0);
        sprintf(str, "R-GAIN Up");
    }
    
    UI_ShowAction(str, 0); 
    
    return 0;
}
int32_t Visca_Rgain_Sub(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_cam_status.wb_mode == VISCA_WB_ONE_PUSH_WB)
    {
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_WB_ONE_PUSH_TRIG, 0x05, 0);
        sprintf(str, "Trigger A White Balance");
    }
    else
    {
        if(visca_cam_status.wb_mode != VISCA_WB_MANUAL)
        {
            visca_cam_status.wb_mode = VISCA_WB_MANUAL;
            visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_WB, visca_cam_status.wb_mode, 0);
            vTaskDelay(50);
        }
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_RGAIN, VISCA_RGAIN_DOWN, 0);
        sprintf(str, "R-GAIN Down");
    }
    
    UI_ShowAction(str, 0);
    return 0;
}

int32_t Visca_Bgain_Add(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_cam_status.wb_mode == VISCA_WB_ONE_PUSH_WB)
    {
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_WB_ONE_PUSH_TRIG, 0x05, 0);
        sprintf(str, "Trigger A White Balance");
    }
    else
    {
        if(visca_cam_status.wb_mode != VISCA_WB_MANUAL)
        {
            visca_cam_status.wb_mode = VISCA_WB_MANUAL;
            visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_WB, visca_cam_status.wb_mode, 0);
            vTaskDelay(50);
        }
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_BGAIN, VISCA_BGAIN_UP, 0);
        sprintf(str, "B-GAIN Up");
    }
    
    UI_ShowAction(str, 0); 
    return 0;
}

int32_t Visca_Bgain_Sub(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_cam_status.wb_mode == VISCA_WB_ONE_PUSH_WB)
    {
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_WB_ONE_PUSH_TRIG, 0x05, 0);
        sprintf(str, "Trigger A White Balance");
    }
    else
    {
        if(visca_cam_status.wb_mode != VISCA_WB_MANUAL)
        {
            visca_cam_status.wb_mode = VISCA_WB_MANUAL;
            visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_WB, visca_cam_status.wb_mode, 0);
            vTaskDelay(50);
        }
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_BGAIN, VISCA_BGAIN_DOWN, 0);
        sprintf(str, "B-GAIN Down");
    }
    
    UI_ShowAction(str, 0); 
    
    return 0;
}

/***************************************************************************************************
* Description:  聚焦模式
***************************************************************************************************/
int32_t Visca_Focus_Auto(int32_t arg)
{
    char str[30] = {0};
    
    visca_cam_status.focus_mode = VISCA_FOCUS_AUTO;
    visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_FOCUS_MODE, VISCA_FOCUS_AUTO, 0);
    sprintf(str, "Set To Auto Focus");
    
    UI_ShowAction(str, 0); 
    
    return 0;
}
int32_t Visca_Focus_Far(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_cam_status.focus_mode != VISCA_FOCUS_MANUAL)
    {
        visca_cam_status.focus_mode = VISCA_FOCUS_MANUAL;
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_FOCUS_MODE, VISCA_FOCUS_MANUAL, 0);
    }
    
    sprintf(str, "Focus Far");
    UI_ShowAction(str, 0); 
    visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_FOCUS, VISCA_FOCUS_FAR_STD, 0);
    HAL_Delay(30);
    visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_FOCUS, VISCA_FOCUS_STOP, 0);
    visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_FOCUS, VISCA_FOCUS_STOP, 0);
    return 0;
}
int32_t Visca_Focus_Near(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_cam_status.focus_mode != VISCA_FOCUS_MANUAL)
    {
        visca_cam_status.focus_mode = VISCA_FOCUS_MANUAL;
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_FOCUS_MODE, VISCA_FOCUS_MANUAL, 0);
    }
    
    sprintf(str, "Focus Near");
    UI_ShowAction(str, 0);
    visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_FOCUS, VISCA_FOCUS_NEAR_STD, 0);
    HAL_Delay(30);
    visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_FOCUS, VISCA_FOCUS_STOP, 0);
    visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_FOCUS, VISCA_FOCUS_STOP, 0);
    
    return 0;
}

/***************************************************************************************************
* Description:  云台操作
***************************************************************************************************/
int32_t Visca_Gohome(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_cam_status.osd == VISCA_ON)
    {
        switch(VISCA_COMPATIBLE_MODE())
        {
            case VISCA_COMPATIBLE_SONY:
            if(!CheckInterval(&osd_timer, OSD_DELAY))
                return 0;
            visca_osd_enter(VISCA_CUR_ADDR(), 0);
            break;
            
            case VISCA_COMPATIBLE_JJTS:
            visca_set_pan_tilt_home(VISCA_CUR_ADDR(), 0);
            break;
            
            case VISCA_COMPATIBLE_VHD:
            case VISCA_COMPATIBLE_MINRRAY:
            if(!CheckInterval(&osd_timer, OSD_DELAY))
                return 0;
            visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_IRIS, VISCA_IRIS_UP, 0);
            break;
        }
        sprintf(str, "OSD Enter");
    }
    else
    {
        sprintf(str, "Go Home");
        visca_set_pan_tilt_home(VISCA_CUR_ADDR(), 0);
    }
    
    UI_ShowAction(str, 0);
    return 0;
}
int32_t Visca_Rocker_PanTilt(int32_t pan_val, int32_t tilt_val, int32_t grade)
{
    uint8_t pan_sp, tilt_sp, pan_dir, tilt_dir;
    uint8_t max_sp = 24/8*grade;
    
    pan_sp = max_sp * abs(pan_val) / 30;
    if(pan_sp > 24)  pan_sp = 24;
    tilt_sp = max_sp * abs(tilt_val) / 30;
    if(tilt_sp > 24)  tilt_sp = 24;
    
    if(pan_val > 0) pan_dir = 0x02;
    else if(pan_val < 0) pan_dir = 0x01;
    else pan_dir = 0x03;
    
    if(tilt_val > 0) tilt_dir = 0x01;
    else if(tilt_val < 0) tilt_dir = 0x02;
    else tilt_dir = 0x03;
    
    /* 菜单打开时需要限制指令发送频率 */
    if(visca_cam_status.osd == VISCA_ON && (pan_dir != 3 || tilt_dir != 3))
    {
        switch(VISCA_COMPATIBLE_MODE())
        {
            case VISCA_COMPATIBLE_JJTS:
            case VISCA_COMPATIBLE_MINRRAY:
            break;
            
            case VISCA_COMPATIBLE_SONY:
            case VISCA_COMPATIBLE_VHD:
            if(!CheckInterval(&osd_timer, OSD_DELAY))
                return 0;
            break;
        }
    }
    
    visca_set_pan_tilt(VISCA_CUR_ADDR(), pan_sp, tilt_sp, pan_dir, tilt_dir, 0);
    if(pan_dir == 0x03 && tilt_dir == 0x03)
    {
        visca_set_pan_tilt(VISCA_CUR_ADDR(), pan_sp, tilt_sp, pan_dir, tilt_dir, 0);
        visca_set_pan_tilt(VISCA_CUR_ADDR(), pan_sp, tilt_sp, pan_dir, tilt_dir, 0);
    }
    return 0;
}
int32_t Visca_Rocker_Zoom(int32_t arg, int32_t grade)
{
    uint8_t speed;
    
    if(visca_cam_status.osd == VISCA_ON)
    {
        switch(VISCA_COMPATIBLE_MODE())
        {
            case VISCA_COMPATIBLE_SONY:
            case VISCA_COMPATIBLE_JJTS:
            break;
            
            case VISCA_COMPATIBLE_VHD:
            case VISCA_COMPATIBLE_MINRRAY:
            if(!CheckInterval(&osd_timer, OSD_DELAY))
                return 0;
            if(arg > 1)
            {
                UI_ShowAction("OSD Enter", 0);
                visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_IRIS, VISCA_IRIS_UP, 0);
                vTaskDelay(50);
                visca_inq_osd(1, &visca_cam_status.osd, 0);
            }
            else if(arg < (-1))
            {
                UI_ShowAction("OSD Back", 0);
                visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_IRIS, VISCA_IRIS_DOWN, 0);
                vTaskDelay(50);
                visca_inq_osd(VISCA_CUR_ADDR(), &visca_cam_status.osd, 0);
                
            }
            return 0;
            break;
        }
    }
    
    speed = abs(arg) / 5;
    if(speed > 7) speed = 7;
    if(arg > 0) speed |= 0x30;
    else if(arg < 0) speed |= 0x20;
    else speed = 0;
    
    visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_ZOOM, speed, 0);
    if(speed == 0)
    {
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_ZOOM, speed, 0);
        visca_set_8bits_value(VISCA_CUR_ADDR(), VISCA_ZOOM, speed, 0);
    }
    
    return 0;
}

/***************************************************************************************************
* Description:  定时查询任务
***************************************************************************************************/
void InqCameraStatus(void)
{
    static int32_t inq_index = 0;
    static uint32_t timerInq = 0;
    
    /* 判断查询失败次数 */
    if(visca_inq_failure_count == 4)
        UI_ShowAction("No return code, please check the connection!", 0);
    
    
    /* 发送控制指令后一段时间才可以开始查询 */
    if(!CheckTimeDiff(visca_cam_status.act_timer, 1000))
    {
        return;
    }
    
    /* 查询间隔控制 */
    if(!CheckInterval(&timerInq, 500))
        return;
    
    switch(inq_index)
    {
        case 0:
        if(VISCA_COMPATIBLE_MODE() != VISCA_COMPATIBLE_JJTS)
        {
            if(visca_inq_osd(VISCA_CUR_ADDR(), &visca_cam_status.osd, 0))
            {
                visca_inq_failure_count = 0;
            }else visca_inq_failure_count++;
        }
        break;
        
        case 1:
        if(visca_inq_8bits_value(VISCA_CUR_ADDR(), VISCA_FOCUS_MODE, &visca_cam_status.focus_mode, 0))
        {
            visca_inq_failure_count = 0;
        }else visca_inq_failure_count++;
        break;
        
        case 2:
        if(visca_inq_8bits_value(VISCA_CUR_ADDR(), VISCA_WB_MODE_INQ, &visca_cam_status.wb_mode, 0))
        {
            visca_inq_failure_count = 0;
        }else visca_inq_failure_count++;
        break;
        
        case 3:
        if(visca_inq_8bits_value(VISCA_CUR_ADDR(), VISCA_AE_MODE_INQ, &visca_cam_status.exposure_mode, 0))
        {
            visca_inq_failure_count = 0;
        }else visca_inq_failure_count++;
        break;
        
        default:
        break;
    }
    
    if(++inq_index > 3) inq_index = 0;
    
}

/* 事件响应函数结构体 */
const event_handler_t visca_event_handler = 
{
    /* 获取当前目标地址 */
    Visca_GetCurAddr,
    
    /* 按键任务 */
    Visca_SearchDevice,
    Visca_SelectDevice,
    Visca_Preset_Call,
    Visca_Preset_Set,
    Visca_Preset_Delete,
    
    NULL,
    Visca_Osd,
    Visca_Backlight,
    Visca_Ae_Mode,
    Visca_Ae_Add,
    Visca_Ae_Sub,
    
    Visca_Wb_Mode,
    Visca_Rgain_Add,
    Visca_Rgain_Sub,
    Visca_Bgain_Add,
    Visca_Bgain_Sub,
    Visca_Focus_Auto,
    Visca_Focus_Far,
    Visca_Focus_Near,
    
    /* 摇杆按键 */
    Visca_Gohome,
    
    /* 摇杆电位器 */
    Visca_Rocker_PanTilt,
    NULL,
    NULL,
    Visca_Rocker_Zoom,
    
    InqCameraStatus,
};

/***************************************************************************************************
* Description:  初始化协议，设置操作接口，事件处理函数
***************************************************************************************************/
int32_t Visca_Init(void)
{
    /* 设置接口 */
    visca_if_set_ops(&visca_if_serial);
    
    pevent_handler = &visca_event_handler;
    
    /* 发送级联指令 */
    visca_reset_address(0);
    
    Visca_SelectDevice(VISCA_CUR_ADDR());
    
    return 0;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t Visca_DeInit(void)
{
    Visca_SetTally(0);
    return 0;
}




/****************************************** END OF FILE *******************************************/
