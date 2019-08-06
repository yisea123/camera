/***************************************************************************************************
* FILE: ptc.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/25, by kevin
***************************************************************************************************/
#ifndef __PTC_H_
#define __PTC_H_

#include "system.h"

#include "visca_ip.h"
#include "visca.h"
#include "onvif.h"
#include "pelco.h"
#include "panasonic.h"
#include "samsung.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/
typedef struct 
{
    /* 获取当前目标地址 */
    uint32_t (*get_current_addr)(void);
    
    /* 按键 */
    int32_t (*key_search_handler)(int32_t);
    int32_t (*key_camera_handler)(int32_t);
    int32_t (*key_preset_call_handler)(int32_t);
    int32_t (*key_preset_set_handler)(int32_t);
    int32_t (*key_preset_delete_handler)(int32_t);
    
    int32_t (*key_exit_handler)(int32_t);
    int32_t (*key_osd_handler)(int32_t);
    int32_t (*key_backlight_handler)(int32_t);
    int32_t (*key_ae_mode_handler)(int32_t);
    int32_t (*key_ae_add_handler)(int32_t);
    int32_t (*key_ae_sub_handler)(int32_t);
    
    int32_t (*key_wb_mode_handler)(int32_t);
    int32_t (*key_rgain_add_handler)(int32_t);
    int32_t (*key_rgain_sub_handler)(int32_t);
    int32_t (*key_bgain_add_handler)(int32_t);
    int32_t (*key_bgain_sub_handler)(int32_t);
    int32_t (*key_focus_auto_handler)(int32_t);
    int32_t (*key_focus_far_handler)(int32_t);
    int32_t (*key_focus_near_handler)(int32_t);
    
    /* 摇杆按键 */
    int32_t (*key_rocker_handler)(int32_t);
    
    /* 摇杆电位器 */
    int32_t (*rocker_pan_tilt_handler)(int32_t, int32_t, int32_t);
    int32_t (*rocker_pan_handler)(int32_t);
    int32_t (*rocker_tilt_handler)(int32_t);
    int32_t (*rocker_zoom_handler)(int32_t, int32_t);
    
    /* 间隔执行任务 */
    void (*timing_task_handler)(void);
    
}event_handler_t;

/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/
 extern event_handler_t const * pevent_handler;


/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
int32_t Ptc_Init(void);
uint32_t GetCurAddr(void);

#endif
/****************************************** END OF FILE *******************************************/
