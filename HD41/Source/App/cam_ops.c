/***************************************************************************************************
* FILE: cam_ops.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/12/2, by kevin
***************************************************************************************************/
#include "cam_ops.h"
#include "ccp.h"
#include "visca2camera.h"
#include "system.h"
/***************************************************************************************************
* Description:  系统
***************************************************************************************************/

/* 电源 */
int32_t sys_power(uint8_t arg)
{
    /* 退出待机模式 */
    if(arg == 2)
    {
        if(stRunInfo.standby == ON)
        {
            stRunInfo.standby = OFF;
            NVIC_SystemReset();
        }
    }
    else if(arg == 3)
    {
        stRunInfo.standby = ON;
        SII9022_RESET = 0;
        CAM_POWER_OFF();
        LED_STANDBY_ON;
        LED_SetStatus(OFF);
    }
    else return 1;
    return 0;
}

int32_t inq_power_status(uint8_t *arg)
{
    if(stRunInfo.standby != ON)
        *arg = 3;
    else
        *arg = 2;
    return 0;
}

/* 菜单 */
int32_t sys_menu(uint8_t arg)
{
    if(arg == VISCA_MENU_ON_OFF || arg == VISCA_MENU_ON)
    {
        if(stRunInfo.menu != MENU_OFF)
            MENU_Back();
        else
            MENU_Open();
    }
    else if(arg == VISCA_MENU_OFF)
    {
        while(stRunInfo.menu != MENU_OFF)
            MENU_Back();
    }
    else
        return 1;
    return 0;
}

int32_t sys_menu_enter(uint8_t arg)
{
    if(stRunInfo.menu != MENU_OFF)
    {
        MENU_Enter();
        return 0;
    }
    return 1;
}

int32_t inq_menu(uint8_t *arg)
{
    if(stRunInfo.menu != MENU_OFF)
        *arg = VISCA_ON;
    else
        *arg = VISCA_OFF;
    return 0;
}

/***************************************************************************************************
* Description:  ZOOM
***************************************************************************************************/
int32_t cam_zoom(uint8_t arg)
{
    if(arg == 0)
        visca_set_8bits_value(VISCA_ZOOM, VISCA_ZOOM_STOP, 0);
    else if(arg == 2)
        visca_set_8bits_value(VISCA_ZOOM, VISCA_ZOOM_TELE_VAR | REMOTE_ZOOM_FAST_SPEED, 0);
    else if(arg == 3)
        visca_set_8bits_value(VISCA_ZOOM, VISCA_ZOOM_WIDE_VAR | REMOTE_ZOOM_FAST_SPEED, 0);
    else if((arg & 0xF0) == 0x20)
        visca_set_8bits_value(VISCA_ZOOM, arg, 0);
    else if((arg & 0xF0) == 0x30)
        visca_set_8bits_value(VISCA_ZOOM, arg, 0);
    else return 1;
    if(stCamInfo.focus_mode != VISCA_AUTO)
    {
        cam_focus_mode(VISCA_AUTO);
    }
    stRunInfo.inq_zoom = 1;
    return 0;
}
int32_t cam_zoom_direct(uint16_t zoom_pos)
{
    if(zoom_pos > 0x5000) return 1;
    visca_set_16bits_value(VISCA_ZOOM_DIRECT, zoom_pos, 0);
    stCamInfo.zoom_pos = zoom_pos;
    CalPtzSpeed();
    if(stCamInfo.focus_mode != VISCA_AUTO)
    {
        cam_focus_mode(VISCA_AUTO);
    }
    return 0;
}
int32_t cam_zoom_focus_direct(uint16_t zoom_pos, uint16_t focus_pos)
{
    if(zoom_pos > 0x5000) return 1;
    visca_set_16bits_value(VISCA_ZOOM_DIRECT, zoom_pos, 0);
    stCamInfo.zoom_pos = zoom_pos;
    
    cam_focus_mode(VISCA_MANUAL);
    
    visca_set_16bits_value(VISCA_FOCUS_DIRECT, focus_pos, 0);
    stCamInfo.focus_pos = focus_pos;
    return 0;
}
int32_t inq_zoom_pos(uint16_t *arg)
{
    uint16_t temp;
    if(visca_inq_16bits_value(VISCA_ZOOM_DIRECT, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.zoom_pos = temp;
    *arg = stCamInfo.zoom_pos;
    return 0;
}

/***************************************************************************************************
* Description:  DZOOM
***************************************************************************************************/
//int32_t cam_dzoom_mode(uint8_t arg)
//{
//    return 0;
//}
int32_t cam_dzoom(uint8_t arg)
{
    if(arg == VISCA_ON)
        stCamInfo.dzoom = VISCA_ON;
    else if(arg == VISCA_OFF)
        stCamInfo.dzoom = VISCA_OFF;
    else
        return 1;
    visca_set_8bits_value(VISCA_DZOOM, stCamInfo.dzoom, 0);
    /* 分离模式 */
    visca_set_8bits_value(VISCA_DZOOM_MODE, VISCA_DZOOM_MODE_CMB, 0);
    return 0;
}
//int32_t cam_dzoom_direct(uint16_t arg)
//{
//    return 0;
//}
int32_t inq_dzoom_mode(uint8_t *arg)
{
    uint8_t temp;
    if(visca_inq_8bits_value(VISCA_DZOOM, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.dzoom = temp;
    *arg = stCamInfo.dzoom;
    return 0;
}
int32_t inq_dzoom_pos(uint16_t *arg)
{
    *arg = stCamInfo.dzoom_pos;
    return 0;
}

/***************************************************************************************************
* Description:  FOCUS
***************************************************************************************************/
int32_t cam_focus_mode(uint8_t arg)
{
    if(arg == VISCA_AUTO)
        stCamInfo.focus_mode = VISCA_AUTO;
    else if(arg == VISCA_MANUAL)
        stCamInfo.focus_mode = VISCA_MANUAL;
    else
        return 1;
    visca_set_8bits_value(VISCA_FOCUS_MODE,  stCamInfo.focus_mode, 0);
    return 0;
}
int32_t cam_focus(uint8_t arg)
{
    if(stCamInfo.focus_mode != VISCA_MANUAL)
        cam_focus_mode(VISCA_MANUAL);
    if(arg == 0)
        visca_set_8bits_value(VISCA_FOCUS, VISCA_FOCUS_STOP, 0);
    else if(arg == 2)
        visca_set_8bits_value(VISCA_FOCUS, VISCA_FOCUS_FAR_STD, 0);
    else if(arg == 3)
        visca_set_8bits_value(VISCA_FOCUS, VISCA_FOCUS_NEAR_STD, 0);
    else if((arg & 0xF0) == 0x20)
        visca_set_8bits_value(VISCA_FOCUS, arg, 0);
    else if((arg & 0xF0) == 0x30)
        visca_set_8bits_value(VISCA_FOCUS, arg, 0);
    else return 1;
    return 0;
}
int32_t cam_focus_direct(uint16_t focus_pos)
{
    if(stCamInfo.focus_mode != VISCA_MANUAL)
        cam_focus_mode(VISCA_MANUAL);
    visca_set_16bits_value(VISCA_FOCUS_DIRECT, focus_pos, 0);
    stCamInfo.focus_pos = focus_pos;
    return 0;
}
int32_t inq_focus_mode(uint8_t *arg)
{
    uint8_t temp;
    if(visca_inq_8bits_value(VISCA_FOCUS_MODE, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.focus_mode = temp;
    *arg = stCamInfo.focus_mode;
    return 0;
}
int32_t inq_focus_pos(uint16_t *arg)
{
    uint16_t temp;
    if(visca_inq_16bits_value(VISCA_FOCUS_DIRECT, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.focus_pos = temp;
    *arg = stCamInfo.focus_pos;
    return 0;
}
int32_t cam_af_sens(uint8_t arg)
{
    if(arg == VISCA_AFS_NORMAL || arg == VISCA_AFS_LOW)
    {
        stCamInfo.af_sens = arg;
    }
    else return 1;
    visca_set_8bits_value(VISCA_AFS, stCamInfo.af_sens, 0);
    return 0;
}
int32_t inq_af_sens(uint8_t *arg)
{
    uint8_t temp;
    if(visca_inq_8bits_value(VISCA_AFS, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.af_sens = temp;
    *arg = stCamInfo.af_sens;
    return 0;
}

/***************************************************************************************************
* Description:  白平衡
***************************************************************************************************/

/* 白平衡模式 */
int32_t cam_wb_mode(uint8_t arg)
{
    if(arg <= VISCA_WB_SL)
        stCamInfo.wb_mode = arg;
    visca_set_8bits_value(VISCA_WB, stCamInfo.wb_mode , 0);
    return 0;
}
int32_t inq_wb_mode(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(VISCA_WB, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.wb_mode = temp;
    *arg = stCamInfo.wb_mode;
    return 0;
}

/* 红增益 */
int32_t cam_rgain(uint8_t arg)
{
    if(stCamInfo.wb_mode != VISCA_WB_MANUAL)
        return 1;
    if(arg == VISCA_UP)
    {
        if(stCamInfo.rgain < 255) stCamInfo.rgain++;
        else stCamInfo.rgain = 0;
    }
    else if(arg == VISCA_DOWN)
    {
        if(stCamInfo.rgain > 0) stCamInfo.rgain--;
        else stCamInfo.rgain = 255;
    }
    else return 1;
    visca_set_16bits_value(VISCA_RGAIN_DIRECT, stCamInfo.rgain, 0);
    return 0;
}
int32_t cam_rgain_direct(uint8_t arg)
{
    stCamInfo.rgain = arg;
    visca_set_16bits_value(VISCA_RGAIN_DIRECT, stCamInfo.rgain, 0);
    return 0;
}
int32_t inq_rgain(uint8_t *arg)
{
    uint16_t temp = 0;
    if(visca_inq_16bits_value(VISCA_RGAIN_DIRECT, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.rgain = temp;
    *arg = stCamInfo.rgain;
    return 0;
}

/* 蓝增益 */
int32_t cam_bgain(uint8_t arg)
{
    if(stCamInfo.wb_mode != VISCA_WB_MANUAL)
        return 1;
    if(arg == VISCA_UP)
    {
        if(stCamInfo.bgain < 255) stCamInfo.bgain++;
        else stCamInfo.bgain = 0;
    }
    else if(arg == VISCA_DOWN)
    {
        if(stCamInfo.bgain > 0) stCamInfo.bgain--;
        else stCamInfo.bgain = 255;
    }
    else return 1;
    visca_set_16bits_value(VISCA_BGAIN_DIRECT, stCamInfo.bgain, 0);
    return 0;
}
int32_t cam_bgain_direct(uint8_t arg)
{
    stCamInfo.bgain = arg;
    visca_set_16bits_value(VISCA_BGAIN_DIRECT, stCamInfo.bgain, 0);
    return 0;
}
int32_t inq_bgain(uint8_t *arg)
{
    uint16_t temp = 0;
    if(visca_inq_16bits_value(VISCA_BGAIN_DIRECT, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.bgain = temp;
    *arg = stCamInfo.bgain;
    return 0;
}
int32_t cam_wb_onepush_trigger(void)
{
    visca_set_8bits_value(VISCA_WB_ONE_PUSH_TRIG, 0x05 , 0);
    return 0;
}

/***************************************************************************************************
* Description:  曝光
***************************************************************************************************/
int32_t cam_ae_mode(uint8_t arg)
{
    if(arg == VISCA_AE_FULL_AUTO || \
        arg == VISCA_AE_MANUAL || \
            arg == VISCA_AE_SHUTTER_PRIORITY || \
                arg == VISCA_AE_IRIS_PRIORITY || \
                    arg == VISCA_AE_BRIGHT_PRIORITY)
    {
        stCamInfo.ae_mode = arg;
    }
    else return 1;
    visca_set_8bits_value(VISCA_AE, stCamInfo.ae_mode, 0);
    return 0;
}
int32_t inq_ae_mode(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(VISCA_AE, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.ae_mode = temp;
    *arg = stCamInfo.ae_mode;
    return 0;
}

/* 快门 */
int32_t cam_shutter(uint8_t arg)
{
    if(stCamInfo.ae_mode != VISCA_AE_SHUTTER_PRIORITY && stCamInfo.ae_mode != VISCA_AE_MANUAL)
        return 1;
    if(arg == VISCA_UP)
    {
        if(stCamInfo.shutter < MAX_SHUTTER) stCamInfo.shutter++;
        else stCamInfo.shutter = MAX_SHUTTER;
    }
    else if(arg == VISCA_DOWN)
    {
        if(stCamInfo.shutter > MIN_SHUTTER) stCamInfo.shutter--;
        else stCamInfo.shutter = MIN_SHUTTER;
    }
    else return 1;
    visca_set_16bits_value(VISCA_SHUTTER_DIRECT, stCamInfo.shutter, 0);
    return 0;
}
int32_t cam_shutter_direct(uint8_t arg)
{
    int32_t temp = arg;
    if(stCamInfo.ae_mode != VISCA_AE_SHUTTER_PRIORITY && stCamInfo.ae_mode != VISCA_AE_MANUAL)
        return 1;
    if(temp > MAX_SHUTTER || temp < MAX_SHUTTER) return 1;
    stCamInfo.shutter = temp;
    visca_set_16bits_value(VISCA_SHUTTER_DIRECT, stCamInfo.shutter, 0);
    return 0;
}
int32_t inq_shutter(uint8_t *arg)
{
    uint16_t temp = 0;
    if(visca_inq_16bits_value(VISCA_SHUTTER_DIRECT, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.shutter = temp;
    *arg = stCamInfo.shutter;
    return 0;
}

/* 光圈 */
int32_t cam_iris(uint8_t arg)
{
    if(stCamInfo.ae_mode != VISCA_AE_IRIS_PRIORITY && stCamInfo.ae_mode != VISCA_AE_MANUAL)
        return 1;
    if(arg == VISCA_UP)
    {
        if(stCamInfo.iris < MAX_IRIS) stCamInfo.iris++;
        else stCamInfo.iris = MAX_IRIS;
    }
    else if(arg == VISCA_DOWN)
    {
        if(stCamInfo.iris > MIN_IRIS) stCamInfo.iris--;
        else stCamInfo.iris = MIN_IRIS;
    }
    else return 1;
    visca_set_16bits_value(VISCA_IRIS_DIRECT, stCamInfo.iris, 0);
    return 0;
}
int32_t cam_iris_direct(uint8_t arg)
{
    int32_t temp = arg;
    if(stCamInfo.ae_mode != VISCA_AE_IRIS_PRIORITY && stCamInfo.ae_mode != VISCA_AE_MANUAL)
        return 1;
    if(temp > MAX_IRIS || temp < MIN_IRIS) return 1;
    stCamInfo.iris = temp;
    visca_set_16bits_value(VISCA_IRIS_DIRECT, stCamInfo.iris, 0);
    return 0;
}
int32_t inq_iris(uint8_t *arg)
{
    uint16_t temp = 0;
    if(visca_inq_16bits_value(VISCA_IRIS_DIRECT, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.iris = temp;
    *arg = stCamInfo.iris;
    return 0;
}

/* 亮度控制 */
int32_t cam_bright(uint8_t arg)
{
    if(stCamInfo.ae_mode != VISCA_AE_BRIGHT_PRIORITY)
        return 1;
    if(arg == VISCA_UP)
    {
        if(stCamInfo.bright < MAX_BRIGHT) stCamInfo.bright++;
        else stCamInfo.bright = MAX_BRIGHT;
    }
    else if(arg == VISCA_DOWN)
    {
        if(stCamInfo.bright > MIN_BRIGHT) stCamInfo.bright--;
        else stCamInfo.bright = MIN_BRIGHT;
    }
    else return 1;
    visca_set_16bits_value(VISCA_BRIGHT_DIRECT, stCamInfo.bright, 0);
    return 0;
}
int32_t cam_bright_direct(uint8_t arg)
{
    int32_t temp = arg;
    if(stCamInfo.ae_mode != VISCA_AE_BRIGHT_PRIORITY)
        return 1;
    if(temp > MAX_BRIGHT || temp < MIN_BRIGHT)
        return 1;
    stCamInfo.bright = temp;
    visca_set_16bits_value(VISCA_BRIGHT_DIRECT, stCamInfo.bright, 0);
    return 0;
}
int32_t inq_bright(uint8_t *arg)
{
    uint16_t temp = 0;
    if(visca_inq_16bits_value(VISCA_BRIGHT_DIRECT, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.bright = temp;
    *arg = stCamInfo.bright;
    return 0;
}

/* 增益 */
int32_t cam_gain(uint8_t arg)
{
    if(stCamInfo.ae_mode != VISCA_AE_MANUAL)
        return 1;
    if(arg == VISCA_UP)
    {
        if(stCamInfo.exp_gain < MAX_GAIN) stCamInfo.exp_gain++;
        else stCamInfo.exp_gain = MAX_GAIN;
    }
    else if(arg == VISCA_DOWN)
    {
        if(stCamInfo.exp_gain > MIN_GAIN) stCamInfo.exp_gain--;
        else stCamInfo.exp_gain = MIN_GAIN;
    }
    else return 1;
    visca_set_16bits_value(VISCA_GAIN_DIRECT, stCamInfo.exp_gain, 0);
    return 0;
}
int32_t cam_gain_direct(uint8_t arg)
{
    int32_t temp = arg;
    if(stCamInfo.ae_mode != VISCA_AE_MANUAL)
        return 1;
    if(temp > MAX_GAIN || temp < MIN_GAIN) return 1;
    stCamInfo.exp_gain = temp;
    visca_set_16bits_value(VISCA_GAIN_DIRECT, stCamInfo.exp_gain, 0);
    return 0;
}
int32_t inq_gain(uint8_t *arg)
{
    uint16_t temp = 0;
    if(visca_inq_16bits_value(VISCA_GAIN_DIRECT, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.exp_gain = temp;
    *arg = stCamInfo.exp_gain;
    return 0;
}

/* 曝光增益 */
//int32_t cam_expcomp_mode(uint8_t arg)
//{
//    if(arg == VISCA_ON)
//        stCamInfo.exp_comp_onoff = VISCA_ON;
//    else if(arg == VISCA_OFF)
//        stCamInfo.exp_comp_onoff = VISCA_OFF;
//    else
//        return 1;
//    visca_set_8bits_value(VISCA_EXPCOMP_ONOFF,  stCamInfo.exp_comp_onoff, 0);
//    return 0;
//}
int32_t cam_expcomp(uint8_t arg)
{
    //    if(stCamInfo.exp_comp_onoff != VISCA_ON)
    //        return 1;
    if(arg == VISCA_UP)
    {
        if(stCamInfo.exp_comp < MAX_EXP_COMP) stCamInfo.exp_comp++;
        else stCamInfo.exp_comp = MAX_EXP_COMP;
    }
    else if(arg == VISCA_DOWN)
    {
        if(stCamInfo.exp_comp > MIN_EXP_COMP) stCamInfo.exp_comp--;
        else stCamInfo.exp_comp = MIN_EXP_COMP;
    }
    else if(arg == VISCA_RESET)
    {
        stCamInfo.exp_comp = CAM_DEFAULT_EXPCOMP_VAL;
    }
    else return 1;
    visca_set_16bits_value(YHW_NEWBRIGHT, stCamInfo.exp_comp, 0);
    return 0;
}
int32_t cam_expcomp_direct(uint8_t arg)
{
    int32_t temp = arg;
    //    if(stCamInfo.exp_comp_onoff != VISCA_ON)
    //        return 1;
    if(temp > MAX_EXP_COMP || temp < MIN_EXP_COMP) return 1;
    stCamInfo.exp_comp = temp;
    visca_set_16bits_value(YHW_NEWBRIGHT, stCamInfo.exp_comp, 0);
    return 0;
}
int32_t inq_expcomp_mode(uint8_t *arg)
{
    *arg = VISCA_ON;
    return 0;
}
int32_t inq_expcomp(uint8_t *arg)
{
    uint16_t temp = 0;
    if(visca_inq_16bits_value(YHW_NEWBRIGHT, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.exp_comp = temp;
    *arg = stCamInfo.exp_comp;
    return 0;
}

/* 背光补偿 */
int32_t cam_backlight(uint8_t arg)
{
    if(arg == VISCA_ON)
        stCamInfo.backlight = VISCA_ON;
    else if(arg == VISCA_OFF)
        stCamInfo.backlight = VISCA_OFF;
    else
        return 1;
    visca_set_8bits_value(VISCA_BACKLIGHT, stCamInfo.backlight, 0);
    return 0;
}
int32_t inq_backlight(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(VISCA_BACKLIGHT, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.backlight = temp;
    *arg = stCamInfo.backlight;
    return 0;
}

/* 高光抑制 */
int32_t cam_highlight(uint8_t arg)
{
    if(arg == VISCA_ON)
        stCamInfo.highlight = VISCA_ON;
    else if(arg == VISCA_OFF)
        stCamInfo.highlight = VISCA_OFF;
    else
        return 1;
    visca_set_8bits_value(YHW_HIGHLIGHT, stCamInfo.highlight, 0);
    return 0;
}
int32_t inq_highlight(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(YHW_HIGHLIGHT, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.highlight = temp;
    *arg = stCamInfo.highlight;
    return 0;
}

/***************************************************************************************************
* Description:  图像效果
***************************************************************************************************/

/* 锐度 */
int32_t cam_sharpness(uint8_t arg)
{
    if(arg == VISCA_UP)
    {
        if(stCamInfo.sharpness < MAX_APERTURE) stCamInfo.sharpness++;
        else stCamInfo.sharpness = MIN_APERTURE;
    }
    else if(arg == VISCA_DOWN)
    {
        if(stCamInfo.sharpness > MIN_APERTURE) stCamInfo.sharpness--;
        else stCamInfo.sharpness = MAX_APERTURE;
    }
    else if(arg == VISCA_RESET)
    {
        stCamInfo.sharpness = CAM_DEFAULT_APERTURE;
    }
    else return 1;
    visca_set_16bits_value(VISCA_APERTURE_DIRECT, stCamInfo.sharpness, 0);
    return 0;
}
int32_t cam_sharpness_direct(uint8_t arg)
{
    int32_t temp = arg;
    if(temp > MAX_APERTURE || temp < MIN_APERTURE) return 1;
    stCamInfo.sharpness = temp;
    visca_set_16bits_value(VISCA_APERTURE_DIRECT, stCamInfo.sharpness, 0);
    return 0;
}
int32_t inq_sharpness(uint8_t *arg)
{
    uint16_t temp = 0;
    if(visca_inq_16bits_value(VISCA_APERTURE_DIRECT, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.sharpness = temp;
    *arg = stCamInfo.sharpness;
    return 0;
}

/* 伽马 */
int32_t cam_gamma(uint8_t arg)
{
    if(arg > 4)
        return 1;
    stCamInfo.gamma = arg;
    visca_set_8bits_value(VISCA_GAMMA, stCamInfo.gamma, 0);
    return 0;
}
int32_t inq_gamma(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(VISCA_GAMMA, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.gamma = temp;
    *arg = stCamInfo.gamma;
    return 0;
}

/* 饱和度 */
int32_t cam_saturation(uint8_t arg)
{
    int32_t temp = arg;
    if(temp > MAX_SATURATION || temp < MIN_SATURATION)
        return 1;
    stCamInfo.saturation = temp;
    visca_set_16bits_value(YHW_SATURATION, stCamInfo.saturation, 0);
    return 0;
}
int32_t inq_saturation(uint8_t *arg)
{
    uint16_t temp = 0;
    if(visca_inq_16bits_value(YHW_SATURATION, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.saturation = temp;
    *arg = stCamInfo.saturation;
    return 0;
}

///* 色调 */
//int32_t cam_hue(uint8_t arg)
//{
//    return 0;
//}
//int32_t inq_hue(uint8_t *arg)
//{
//    return 0;
//}

/* 对比度 */
int32_t cam_constrast_direct(uint8_t arg)
{
    int32_t temp = arg;
    if(temp > MAX_CONSTRAST || temp < MIN_CONSTRAST) return 1;
    stCamInfo.constrast = temp;
    visca_set_16bits_value(YHW_CONSTRAST, stCamInfo.constrast, 0);
    return 0;
}
int32_t inq_constrast(uint8_t *arg)
{
    uint16_t temp = 0;
    if(visca_inq_16bits_value(YHW_CONSTRAST, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.constrast = temp;
    *arg = stCamInfo.constrast;
    return 0;
}

/* 去雾 */
int32_t cam_defog(uint8_t arg1, uint8_t arg2)
{
    if(arg1 == VISCA_OFF)
    {
        stCamInfo.defog = 0;
    }
    else if(arg1 == VISCA_ON)
    {
        if(arg2 > MAX_DEFOG) return 1;
        stCamInfo.defog = arg2;
    }
    else return 1;
    
    visca_set_8bits_value(YHW_DEFOG, stCamInfo.defog, 0);
    return 0;
}
int32_t inq_defog(uint8_t *arg1, uint8_t *arg2)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(YHW_DEFOG, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.defog = temp;
    if(stCamInfo.defog == 0)
    {
        *arg1 = VISCA_OFF;
        *arg2 = 0;
    }
    else
    {
        *arg1 = VISCA_ON;
        *arg2 = stCamInfo.defog;
    }
    return 0;
}

/* 降噪 */
int32_t cam_2dnr(uint8_t arg)
{
    if(arg == VISCA_ON)
        stCamInfo.nr2d = VISCA_ON;
    else if(arg == VISCA_OFF)
        stCamInfo.nr2d = VISCA_OFF;
    else return 1;
    
    visca_set_8bits_value(YHW_2DNR, stCamInfo.nr2d == VISCA_ON ? 1 : 0, 0);
    return 0;
}
int32_t inq_2dnr(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(YHW_2DNR, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.nr2d = temp == 1 ? VISCA_ON : VISCA_OFF;
    *arg = stCamInfo.nr2d;
    return 0;
}
int32_t cam_3dnr(uint8_t arg)
{
    if(arg > MAX_NR)
        return 1;
    stCamInfo.nr3d = arg;
    visca_set_8bits_value(VISCA_NR, stCamInfo.nr3d, 0);
    return 0;
}
int32_t inq_3dnr(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(VISCA_NR, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.nr3d = temp;
    *arg = stCamInfo.nr3d;
    return 0;
}

/* 频闪 */
int32_t cam_flick(uint8_t arg)
{
    if(arg > 2)
        return 1;
    stCamInfo.flick = arg;
    visca_set_8bits_value(YHW_FLICK, stCamInfo.flick, 0);
    return 0;
}
int32_t inq_flick(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(YHW_FLICK, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.flick = temp;
    *arg = stCamInfo.flick;
    return 0;
}

/* 宽动态范围 */
int32_t cam_wdr(uint8_t arg)
{
    if(arg > 6)
        return 1;
    if(arg == 0)
    {
        stCamInfo.wdr = VISCA_OFF;
        visca_set_8bits_value(VISCA_WDR, VISCA_OFF, 0);
    }
    else
    {
        stCamInfo.wdr = VISCA_ON;
        stCamInfo.wdr_level = arg;
        visca_set_8bits_value(VISCA_WDR, VISCA_ON, 0);
        visca_set_8bits_value(VISCA_WDR_LEVEL, stCamInfo.wdr_level, 0);
    }
    return 0;
}
int32_t inq_wdr(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(VISCA_WDR, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.wdr = temp;
    if(visca_inq_8bits_value(VISCA_WDR_LEVEL, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.wdr_level = temp;
    if(stCamInfo.wdr == VISCA_OFF) *arg = 0;
    else *arg = stCamInfo.wdr_level;
    
    return 0;
}

/* 镜像 */
int32_t cam_mirror(uint8_t arg)
{
    if(arg == VISCA_ON)
    {
        stCamInfo.mirror = VISCA_ON;
        if(stCamInfo.flip == VISCA_ON)
            visca_set_8bits_value(VISCA_LR_REVERSE, VISCA_ON, 0);
        else
            visca_set_8bits_value(VISCA_LR_REVERSE, VISCA_ON, 0);
    }
    else if(arg == VISCA_OFF)
    {
        stCamInfo.mirror = VISCA_OFF;
        if(stCamInfo.flip == VISCA_ON)
            visca_set_8bits_value(VISCA_LR_REVERSE, VISCA_OFF, 0);
        else
            visca_set_8bits_value(VISCA_LR_REVERSE, VISCA_OFF, 0);
    }
    else return 1;
    return 0;
}
int32_t inq_mirror(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(VISCA_LR_REVERSE, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.mirror = temp;
    *arg = stCamInfo.mirror;       
    return 0;
}

/* 翻转 */
int32_t cam_flip(uint8_t arg)
{
    /* 图像翻转开 */
    if(arg == VISCA_ON)
    {
        stCamInfo.flip = VISCA_ON;
        visca_set_8bits_value(VISCA_PIC_FLIP, VISCA_ON, 0);      
    }
    /* 图像翻转关 */
    else if(arg == VISCA_OFF)
    {
        stCamInfo.flip = VISCA_OFF;
        visca_set_8bits_value(VISCA_PIC_FLIP, VISCA_OFF, 0);
    }
    else return 1;
    cam_mirror(stCamInfo.mirror);
    return 0;
}
int32_t inq_flip(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(VISCA_PIC_FLIP, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.flip = temp;
    if(stCamInfo.flip == VISCA_ON) *arg = VISCA_ON;
    else *arg = VISCA_OFF;        
    return 0;
}

/* 屏蔽 */
int32_t cam_mute(uint8_t arg)
{
    if(arg == VISCA_ON)
        stCamInfo.mute = VISCA_ON;
    else if(arg == VISCA_OFF)
        stCamInfo.mute = VISCA_OFF;
    else return 1;
    visca_set_8bits_value(VISCA_MUTE, stCamInfo.mute, 0);
    return 0;
}
int32_t inq_mute(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(VISCA_MUTE, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.mute = temp;
    *arg = stCamInfo.mute;
    return 0;
}

/* 冻结 */
int32_t cam_freeze(uint8_t arg)
{
    if(arg == VISCA_ON)
        stCamInfo.freeze = VISCA_ON;
    else if(arg == VISCA_OFF)
        stCamInfo.freeze = VISCA_OFF;
    else return 1;
    visca_set_8bits_value(VISCA_FREEZE, stCamInfo.freeze, 0);
    return 0;
}
int32_t inq_freeze(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(VISCA_FREEZE, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.freeze = temp;
    *arg = stCamInfo.freeze;
    return 0;
}

/* 信息显示 */
int32_t cam_datascreen(uint8_t arg)
{
    if(arg == VISCA_ON)
        stCamInfo.datascreen = VISCA_ON;
    else if(arg == VISCA_OFF)
        stCamInfo.datascreen = VISCA_OFF;
    else return 1;
    visca_set_8bits_value(VISCA_DISPLAY, stCamInfo.datascreen, 0);
    return 0;
}
int32_t inq_datascreen(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(VISCA_DISPLAY, &temp, INQ_RETRY_TIMES))
        return 1;
    stCamInfo.datascreen = temp;
    *arg = stCamInfo.datascreen;
    return 0;
}

/* 网络 */
int32_t cam_dhcp(uint8_t arg)
{
    if(arg == VISCA_ON)
        stCamInfo.dhcp = VISCA_ON;
    else if(arg == VISCA_OFF)
        stCamInfo.dhcp = VISCA_OFF;
    else return 1;
    visca_set_8bits_value(YHW_NET_DHCP, stCamInfo.dhcp ^ 0x03, 0);
    return 0;
}
int32_t inq_dhcp(uint8_t *arg)
{
    uint8_t temp = 0;
    if(visca_inq_8bits_value(YHW_NET_DHCP, &temp, INQ_RETRY_TIMES))
        return 1;
    if(temp)
        stCamInfo.dhcp = VISCA_ON;
    else
        stCamInfo.dhcp = VISCA_OFF;
    return 0;
}

int32_t cam_net(uint8_t arg, uint32_t ip)
{
    if(arg == EXT_NET_IP)
    {
        stCamInfo.ip = ip;
        visca_set_net(YHW_NET_IP, stCamInfo.ip, 0);
    }
    else if(arg == EXT_NET_MASK)
    {
        visca_set_net(YHW_NET_MASK, stCamInfo.mask, 0);
    }
    else if(arg == YHW_NET_GATEWAY)
    {
        visca_set_net(YHW_NET_GATEWAY, stCamInfo.gateway, 0);
    }
    else return 1;
    return 0;
}
int32_t inq_net(uint8_t type, uint32_t *ip)
{
    uint32_t temp = 0;
    if(type == EXT_NET_IP)
    {
        if(visca_inq_net(YHW_NET_IP, &temp, INQ_RETRY_TIMES))
            return 1;
        stCamInfo.ip = temp;
        *ip = stCamInfo.ip;
    }
    else if(type == EXT_NET_MASK)
    {
        if(visca_inq_net(YHW_NET_MASK, &temp, INQ_RETRY_TIMES))
            return 1;
        stCamInfo.mask = temp;
        *ip = stCamInfo.mask;
    }
    else if(type == EXT_NET_GATEWAY)
    {
        if(visca_inq_net(YHW_NET_GATEWAY, &temp, INQ_RETRY_TIMES))
            return 1;
        stCamInfo.gateway = temp;
        *ip = stCamInfo.gateway;
    }
    else return 1;
    return 0;
}





/***************************************************************************************************
* Description:  其他
***************************************************************************************************/

/* 预置位 */

/* 完整的预置位参数 Pan,Tilt Zoom,Focus,
WB Mode,R Gain, B Gain,
AE Mode, Shutter,Slow Shutter,Iris,Bright,Gian
Exposure Compensation,Backlight,Highlight,
exBright,Constrast,Staturation,Aperture,Gamma,Hue,Nr,Wdr*/

static int32_t cam_preset_complete(uint8_t type, uint8_t num)
{
    uint32_t save_addr;
    
    struct preset_complete preset_t;
    memset(&preset_t, 0x55, sizeof(struct preset_complete));
    
    /* 计算地址 */
    if(IS_PTZ_INV) save_addr = PRESET_INV_SAVE_ADDR + num * sizeof(struct preset_complete);
    else save_addr = PRESET_SAVE_ADDR + num * sizeof(struct preset_complete);
    
    /* 删除预置位 */
    if(type == PRESET_RESET)
    {
        EEPROM_Write(save_addr, (uint8_t *)&preset_t, sizeof(struct preset_complete));
        PresetStatusResetBit(num);
    }
    /* 设置预置位 */
    else if(type == PRESET_SET)
    {
        uint16_t tmp = 0;
        /* 查询ZOOM位置 */
        tmp = 0;
        if(inq_zoom_pos(&tmp))
        {
            return 1;
        }
        preset_t.zoom_pos = tmp;
        
        /* 查询Focus位置 */
        if(inq_focus_pos(&tmp))
        {
            return 1;
        }
        preset_t.focus_pos = tmp;
        
        /* 查询云台位置 */
        preset_t.pan_pos = MS41909_GetMotorPosition(PAN);
        preset_t.tilt_pos = MS41909_GetMotorPosition(TILT);
        
        /* 其它图像参数 */
        preset_t.wb_mode = stCamInfo.wb_mode;
        preset_t.rgain = stCamInfo.rgain;
        preset_t.bgain = stCamInfo.bgain;
        preset_t.ae_mode = stCamInfo.ae_mode;
        preset_t.shutter = stCamInfo.shutter;
        preset_t.slow_shutter = stCamInfo.slow_shutter;
        preset_t.iris = stCamInfo.iris;
        preset_t.bright = stCamInfo.bright;
        preset_t.gain = stCamInfo.exp_gain;
        preset_t.expcomp = stCamInfo.exp_comp;
        preset_t.backlight = stCamInfo.backlight;
        preset_t.highlight = stCamInfo.highlight;
        preset_t.constrast =stCamInfo.constrast;
        preset_t.saturation = stCamInfo.saturation;
        preset_t.sharpness = stCamInfo.sharpness;
        preset_t.gamma = stCamInfo.gamma;
        preset_t.hue = stCamInfo.hue;
        preset_t.nr = stCamInfo.nr3d;
        preset_t.wdr = stCamInfo.wdr;
        
        /* 校验数据 */
        preset_t.checksum = CheckSum((const uint8_t *)&preset_t, sizeof(struct preset_complete)-4);
        
        /* 写入并确认已经写入成功 */
        struct preset_complete preset_verify;
        uint8_t write_count = 0;
        do
        {
            EEPROM_Write(save_addr, (uint8_t *)&preset_t, sizeof(struct preset_complete));
            memset(&preset_verify, 0xFF, sizeof(struct preset_complete));
            EEPROM_Read(save_addr, (uint8_t *)&preset_verify, sizeof(struct preset_complete));
            if(write_count++ > 3) return 1;
        }while(memcmp(&preset_t, &preset_verify, sizeof(struct preset_complete)));
        PresetStatusSetBit(num);
    }
    /* 调用预置位 */
    else if(type == PRESET_CALL)
    {
        uint8_t speed;
        
        /* 读取数据 */
        EEPROM_Read(save_addr, (uint8_t *)&preset_t, sizeof(struct preset_complete));
        
        /* 校验数据 */
        if(preset_t.checksum != CheckSum((const uint8_t *)&preset_t, sizeof(struct preset_complete)-4))
        {
            return 1;
        }
        
        /* 调用ZOOM Focus位置 */
        cam_zoom_focus_direct(preset_t.zoom_pos, preset_t.focus_pos);
        
        /* 调用云台位置 */
        speed = GetPresetSpeed(PAN, MS41909_GetMotorPosition(PAN),  preset_t.pan_pos);
        MS41909_MotorGoto(PAN, preset_t.pan_pos, speed, PAN_PRESET_ACC, PAN_PRESET_DEC);
        speed = GetPresetSpeed(TILT, MS41909_GetMotorPosition(TILT),  preset_t.tilt_pos);
        MS41909_MotorGoto(TILT, preset_t.tilt_pos, speed, TILT_PRESET_ACC, TILT_PRESET_DEC);
        
        /* 其他图像参数 */
        if(PRESET_SAVE_PIC)
        {
            CALL_PRESET_DELAY();
            cam_wb_mode(preset_t.wb_mode);
            cam_rgain_direct(preset_t.rgain);
            cam_bgain_direct(preset_t.bgain);
            
            cam_ae_mode(preset_t.ae_mode);
            cam_shutter_direct(preset_t.shutter);
            cam_iris_direct(preset_t.iris);
            cam_bright_direct(preset_t.bright);
            cam_gain_direct(preset_t.gain);
            cam_expcomp_direct(preset_t.expcomp);
            cam_backlight(preset_t.backlight);
            
            
            cam_constrast_direct(preset_t.constrast);
            cam_saturation(preset_t.saturation);
            cam_sharpness_direct(preset_t.sharpness);
            cam_gamma(preset_t.gamma);
            cam_3dnr(preset_t.nr);
            cam_wdr(preset_t.wdr);
        }
    }
    else return 1;
    
    return 0;
}
static int32_t cam_preset_base(uint8_t type, uint8_t num)
{
    uint32_t save_addr;
    struct preset preset_t;
    memset(&preset_t, 0x55, sizeof(struct preset));
    
    /* 计算地址 */
    if(num < 16) return 1;
    num -= 16;
    if(IS_PTZ_INV) save_addr = PRESET_EXT_INV_SAVE_ADDR + num * sizeof(struct preset);
    else  save_addr = PRESET_EXT_SAVE_ADDR + num * sizeof(struct preset);
    
    /* 删除预置位 */
    if(type == PRESET_RESET)
    {
        EEPROM_Write(save_addr, (uint8_t *)&preset_t, sizeof(struct preset));
        PresetStatusResetBit(num+16);
    }
    /* 设置预置位 */
    else if(type == PRESET_SET)
    {
        uint16_t tmp = 0;
        
        /* 查询ZOOM位置 */
        if(inq_zoom_pos(&tmp))
            return 1;
        preset_t.zoom_pos = tmp;
        
        /* 查询Focus位置 */
        if(inq_focus_pos(&tmp))
            return 1;
        preset_t.focus_pos = tmp;
        
        /* 查询云台位置 */
        preset_t.pan_pos = MS41909_GetMotorPosition(PAN);
        preset_t.tilt_pos = MS41909_GetMotorPosition(TILT);
        
        /* 校验数据 */
        preset_t.checksum = CheckSum((const uint8_t *)&preset_t, sizeof(struct preset)-4);
        
        /* 写入并确认已经写入成功 */
        struct preset preset_verify;
        uint8_t write_count = 0;
        do
        {
            EEPROM_Write(save_addr, (uint8_t *)&preset_t, sizeof(struct preset));
            memset(&preset_verify, 0xFF, sizeof(struct preset));
            EEPROM_Read(save_addr, (uint8_t *)&preset_verify, sizeof(struct preset));
            if(write_count++ > 3) return 1;
        }while(memcmp(&preset_t, &preset_verify, sizeof(struct preset)));
        PresetStatusSetBit(num+16);
    }
    /* 调用预置位 */
    else if(type == PRESET_CALL)
    {
        uint8_t speed;
        
        /* 读取数据 */
        EEPROM_Read(save_addr, (uint8_t *)&preset_t, sizeof(struct preset));
        
        /* 校验数据 */
        if(preset_t.checksum != CheckSum((const uint8_t *)&preset_t, sizeof(struct preset)-4))
            return 1;
        
        /* 调用ZOOM Focus位置 */
        cam_zoom_focus_direct(preset_t.zoom_pos, preset_t.focus_pos);
        
        /* 调用云台位置 */
        speed = GetPresetSpeed(PAN, MS41909_GetMotorPosition(PAN),  preset_t.pan_pos);
        MS41909_MotorGoto(PAN, preset_t.pan_pos, speed, PAN_PRESET_ACC, PAN_PRESET_DEC);
        speed = GetPresetSpeed(TILT, MS41909_GetMotorPosition(TILT),  preset_t.tilt_pos);
        MS41909_MotorGoto(TILT, preset_t.tilt_pos, TILT_PRESET_SPEED, TILT_PRESET_ACC, TILT_PRESET_DEC);
    }
    else return 1;
    
    return 0;
}

int32_t cam_preset(uint8_t type, uint8_t num)
{
    /* 菜单操作 */
    if(stRunInfo.menu != MENU_OFF)
    {
        if(type == PRESET_CALL)
        {
            if(num == 95) MENU_Back();
            else MENU_Num(num);
        }
    }
    /* 保存参数 */
    else if(type == PRESET_SET && num == 127)
    {
        System_SavePara();
    }
    /* 打开菜单 */
    else if(type == PRESET_CALL && num == 95)
    {
        MENU_Open();
    }
    else
    {
        if(num < 16)
            return cam_preset_complete(type, num);
        else
            return cam_preset_base(type, num); 
    }
    
    return 0;
}
int32_t inq_preset_call(uint8_t *arg)
{
    *arg = 1;
    return 0;
}
int32_t inq_preset(uint8_t num, uint8_t *arg)
{
    uint32_t save_addr;
    /* 完整版预置位 */
    if(num < 16)
    {
        struct preset_complete preset_t;
        memset(&preset_t, 0x55, sizeof(struct preset));
        /* 计算地址 */
        if(IS_PTZ_INV) save_addr = PRESET_INV_SAVE_ADDR + num * sizeof(struct preset_complete);
        else save_addr = PRESET_SAVE_ADDR + num * sizeof(struct preset_complete);
        /* 读取数据 */
        EEPROM_Read(save_addr, (uint8_t *)&preset_t, sizeof(struct preset_complete));
        /* 校验数据 */
        if(preset_t.checksum != CheckSum((const uint8_t *)&preset_t, sizeof(struct preset_complete)-4))
            *arg = VISCA_OFF;
        else
            *arg = VISCA_ON;
    }
    /* 基础版预置位 */
    else
    { 
        struct preset preset_t;
        memset(&preset_t, 0x55, sizeof(struct preset));
        /* 计算地址 */
        num -= 16;
        if(IS_PTZ_INV) save_addr = PRESET_EXT_INV_SAVE_ADDR + num * sizeof(struct preset);
        else  save_addr = PRESET_EXT_SAVE_ADDR + num * sizeof(struct preset);
        /* 读取数据 */
        EEPROM_Read(save_addr, (uint8_t *)&preset_t, sizeof(struct preset));
        /* 校验数据 */
        if(preset_t.checksum != CheckSum((const uint8_t *)&preset_t, sizeof(struct preset)-4))
            *arg = VISCA_OFF;
        else
            *arg = VISCA_ON;
    }
    
    return 0;
}
int32_t inq_preset_ext(uint8_t **buffer, uint8_t *size)
{
    static uint8_t preset_status[64+3] = {0x90, 0x50, 0x0F, 0x00, 0x0F, 0x00, 0xFF};
    PresetStatusFillViscaBuffer(&preset_status[2]);
    preset_status[64+2] = 0xFF;
    *buffer = preset_status;
    *size = sizeof(preset_status);
    return 0;
}

/* 寄存器 */
int32_t cam_reg(uint8_t reg, uint8_t val)
{
    /* 分辨率 */
    if(reg == REG_VIDEO_FOMAT)
    {
        uint8_t format = Cmd2VideoFormat(val);
        if(format == 0)
            return 1;
        while(stRunInfo.menu != MENU_OFF)
        {
            MENU_Back();
        }
        stSysInfo.video_format_out1 = format;
        SetMainVideoFormat(stSysInfo.video_format_out1);
        return 0;
    }
    
    return 1;
}
int32_t inq_reg(uint8_t reg, uint8_t *val)
{
    /* 分辨率 */
    if(reg == REG_VIDEO_FOMAT)
    {
        uint8_t cmd;
        cmd = VideoFormat2Cmd(stSysInfo.video_format_out1);
        if(cmd == 0)
            return 1;
        *val = cmd;
        return 0;
    }
    
    return 1;
}

int32_t cam_usb_mode(uint8_t arg)
{
    if(stSysInfo.usb_mode > 2)
        return 1;
    stSysInfo.usb_mode = arg;
    
    if(stSysInfo.usb_mode == 1)
    {
        visca_set_8bits_value(YHW_USB, YHW_USB_OFF, 1);
    }
    else
    {
        visca_set_8bits_value(YHW_USB, YHW_USB_ON, 1);
    }
    return 0;
}

int32_t inq_usb_mode(uint8_t *arg)
{
    *arg = stSysInfo.usb_mode;
    return 0;
}

/***************************************************************************************************
* Description:  云台
***************************************************************************************************/
void pan_run(uint8_t dir, uint8_t speed, int8_t acc, int8_t dec)
{
    if(stRunInfo.menu == MENU_ON || stRunInfo.menu == MENU_CAM)
    {
        if(dir == PAN_LEFT) MENU_Left();
        else if(dir == PAN_RIGHT) MENU_Right();
        return;
    }
    
    /* 速度限制 */
    if(stSysInfo.ptc.speed_limit)
    {
        if(speed > stRunInfo.pan_speed)
            speed = stRunInfo.pan_speed;
    }
    
    if(dir == PAN_LEFT)
    {
        MS41909_MotorGoto(PAN, IS_PAN_INV ? PAN_MAX_POS : PAN_MINI_POS, TransPanSpeed(speed), acc, dec);
    }
    else if(dir == PAN_RIGHT)
    {
        MS41909_MotorGoto(PAN, IS_PAN_INV ? PAN_MINI_POS : PAN_MAX_POS, TransPanSpeed(speed), acc, dec);
    }
    else
    {
        MS41909_MotorBrake(PAN, dec);
    }
}
void tilt_run(uint8_t dir, uint8_t speed, int8_t acc, int8_t dec)
{
    if(stRunInfo.menu == MENU_ON || stRunInfo.menu == MENU_CAM)
    {
        if(dir == TILT_UP) MENU_Up();
        else if(dir == TILT_DOWN) MENU_Down();
        return;
    }
    
    /* 速度限制 */
    if(stSysInfo.ptc.speed_limit)
    {
        if(speed > stRunInfo.tilt_speed)
            speed = stRunInfo.tilt_speed;
    }
    
    if(dir == TILT_DOWN)
    {
        MS41909_MotorGoto(TILT, IS_TILT_INV ? TILT_MAX_POS : TILT_MINI_POS, TransTiltSpeed(speed), acc, dec);
    }
    else if(dir == TILT_UP)
    {
        MS41909_MotorGoto(TILT, IS_TILT_INV ? TILT_MINI_POS : TILT_MAX_POS, TransTiltSpeed(speed), acc, dec);
    }
    else
    {
        MS41909_MotorBrake(TILT, dec);
    }
}
int32_t pt_run(uint8_t pan_speed, uint8_t tilt_speed, uint8_t pan_dir, uint8_t tilt_dir)
{
    pan_run(pan_dir, pan_speed, PAN_ACC, PAN_DEC);
    tilt_run(tilt_dir, tilt_speed, TILT_ACC, TILT_DEC);
    return 0;
}
int32_t pt_abs_run(uint8_t pan_speed, uint8_t tilt_speed, int16_t pan_pos, int16_t tilt_pos)
{
    int32_t pos;
    
    /******* PAN *******/
    pos = (int32_t)(pan_pos * PAN_VISCA_STEPS);
    if(IS_PAN_INV) pos *= -1;
    if(pos > PAN_MAX_POS) pos = PAN_MAX_POS;
    if(pos < PAN_MINI_POS) pos = PAN_MINI_POS;
    
    /* 速度限制 */
    if(stSysInfo.ptc.speed_limit)
    {
        if(pan_speed > stRunInfo.pan_speed)
            pan_speed = stRunInfo.pan_speed;
    }
    
    MS41909_MotorGoto(PAN, pos, TransPanSpeed(pan_speed), PAN_ACC, PAN_DEC);
    
    /******* TILT *******/
    pos = (int32_t)(tilt_pos * TILT_VISCA_STEPS);
    if(IS_TILT_INV) pos *= -1;
    if(pos > TILT_MAX_POS) pos = TILT_MAX_POS;
    if(pos < TILT_MINI_POS) pos = TILT_MINI_POS;
    
    /* 速度限制 */
    if(stSysInfo.ptc.speed_limit)
    {
        if(tilt_speed > stRunInfo.tilt_speed)
            tilt_speed = stRunInfo.tilt_speed;
    }
    
    MS41909_MotorGoto(TILT, pos, TransTiltSpeed(tilt_speed), TILT_ACC, TILT_DEC);
    
    return 0;
}
int32_t pt_rel_run(uint8_t pan_speed, uint8_t tilt_speed, int16_t pan_pos, int16_t tilt_pos)
{   
    int32_t pos;
    
    /* PAN */
    pos = (int32_t)(pan_pos * PAN_VISCA_STEPS);
    if(IS_PAN_INV) pos *= -1;
    pos += MS41909_GetMotorPosition(PAN);
    if(pos > PAN_MAX_POS) pos = PAN_MAX_POS;
    if(pos < PAN_MINI_POS) pos = PAN_MINI_POS;
    
    /* 速度限制 */
    if(stSysInfo.ptc.speed_limit)
    {
        if(pan_speed > stRunInfo.pan_speed)
            pan_speed = stRunInfo.pan_speed;
    }
    
    MS41909_MotorGoto(PAN, pos, TransPanSpeed(pan_speed), PAN_ACC, PAN_DEC);
    
    /* TILT */
    pos = (int32_t)(tilt_pos * TILT_VISCA_STEPS);
    if(IS_TILT_INV) pos *= -1;
    pos += MS41909_GetMotorPosition(TILT);
    if(pos > TILT_MAX_POS) pos = TILT_MAX_POS;
    if(pos < TILT_MINI_POS) pos = TILT_MINI_POS;
    
    /* 速度限制 */
    if(stSysInfo.ptc.speed_limit)
    {
        if(tilt_speed > stRunInfo.tilt_speed)
            tilt_speed = stRunInfo.tilt_speed;
    }
    
    MS41909_MotorGoto(TILT, pos, TransTiltSpeed(tilt_speed), TILT_ACC, TILT_DEC);
    
    return 0;
}
int32_t ptz_home(void)
{
    if(stRunInfo.menu == MENU_ON || stRunInfo.menu == MENU_CAM)
    {
        MENU_Enter();
        return 0;
    }
    
    MS41909_MotorGoto(PAN, PAN_HOME, TransPanSpeed(PAN_GOHOME_SPEED), PAN_GOHOME_ACC, PAN_GOHOME_DEC);
    MS41909_MotorGoto(TILT, TILT_HOME, TransTiltSpeed(TILT_GOHOME_SPEED), TILT_GOHOME_ACC, TILT_GOHOME_DEC);
    
    cam_focus_mode(VISCA_AUTO);
    cam_zoom(VISCA_ZOOM_WIDE_VAR | 7);
    stCamInfo.zoom_pos = 0;
    CalPtzSpeed();
    
    return 0;
}
int32_t inq_pt_pos(int16_t *pan_pos, int16_t *tilt_pos)
{
    int32_t pos;
    
    pos = MS41909_GetMotorPosition(PAN);
    *pan_pos = (int16_t)(pos / PAN_VISCA_STEPS + (pos>0?0.5f:(-0.5f)));
    if(IS_PAN_INV) *pan_pos *= -1;
    
    pos = MS41909_GetMotorPosition(TILT);
    *tilt_pos = (int16_t)(pos / TILT_VISCA_STEPS + (pos>0?0.5f:(-0.5f)));
    if(IS_TILT_INV) *tilt_pos *= -1;
    
    return 0;
}
int32_t cam_init(uint8_t arg)
{
    if(arg == VISCA_CAM_INIT_LENS)
    {
        visca_set_8bits_value(VISCA_CAM_INIT, VISCA_CAM_INIT_LENS, 0);
        return 0;
    }
    else if(arg == VISCA_CAM_INIT_CAM)
    {
        NVIC_SystemReset();
        return 0;
    }
    else return 1;
}
int32_t ptz_reset(void)
{
    cam_focus(VISCA_AUTO);
    cam_zoom(VISCA_ZOOM_WIDE_VAR | 7);
    stCamInfo.zoom_pos = 0;
    CalPtzSpeed();
    
    if(stCamInfo.focus_mode != VISCA_AUTO)
    {
        cam_focus_mode(VISCA_AUTO);
    }
    
    PanTilt_Reset(0, 0);
    
    return 0;
}

/***************************************************************************************************
* Description:  SONY扩展指令
***************************************************************************************************/


/***************************************************************************************************
* Description:  自定义扩展指令
***************************************************************************************************/
/* 查询自定义的版本信息 */
int32_t inq_version(uint16_t *ver)
{
    *ver = (uint16_t)VER_CODE;
    return 0;
}
/* 云台水平反向控制 */
int32_t pan_reverse(uint8_t onoff)
{
    if(onoff == VISCA_ON || onoff == VISCA_OFF)
    {
        stSysInfo.pan_inversion = onoff ^ 3;
        System_SavePartPara(SYS_SAVE_PAN_REVERSE);
        return 0;
    }
    return 1;
}
int32_t inq_pan_reverse(uint8_t *onoff)
{
    *onoff = stSysInfo.pan_inversion ^ 3;
    return 0;
}
/* 云台垂直反向控制 */
int32_t tilt_reverse(uint8_t onoff)
{
    if(onoff == VISCA_ON || onoff == VISCA_OFF)
    {
        stSysInfo.tilt_inversion = onoff;
        System_SavePartPara(SYS_SAVE_TILT_REVERSE);
        return 0;
    }
    return 1;
}
int32_t inq_tilt_reverse(uint8_t *onoff)
{
    *onoff = stSysInfo.tilt_inversion ^ 3;
    return 0;
}

/***************************************************************************************************
* Description:  设置默认网络参数
***************************************************************************************************/
#define LOAD_DEFAULT_DELAY()    delay_ms(50)
#define IPV4(a, b, c, d)        (((a) << 24) | ((b) << 16) | ((c) << 8) | (d))
void cam_reset_net(void)
{
    #if defined(EN_NETWORK)
    /* 网络 */
    stCamInfo.dhcp = 0;
    stCamInfo.ip = IPV4(192, 168, 1, 118);
    stCamInfo.mask = IPV4(255, 255, 255, 0);
    stCamInfo.gateway = IPV4(192, 168, 1, 1);
    visca_set_8bits_value(YHW_NET_DHCP, stCamInfo.dhcp, 1);
    visca_set_net(YHW_NET_IP, stCamInfo.ip, 1);
    visca_set_net(YHW_NET_MASK, stCamInfo.mask, 1);
    visca_set_net(YHW_NET_GATEWAY, stCamInfo.gateway, 1);
#endif
}

/***************************************************************************************************
* Description:  载入默认图像参数
***************************************************************************************************/
void cam_reset_picture(void)
{
    memset(&stCamInfo, 0, sizeof(CAMERA_INFO_ST) - 13);
    
    /* 设置机芯正倒装 */
    visca_set_8bits_value(YHW_CAM_ROATE, YHW_CAM_ROATE_ON, 1);
    LOAD_DEFAULT_DELAY();
    
    /* 数字变倍 */
    cam_dzoom(CAM_DEFAULT_DZOOM);
    LOAD_DEFAULT_DELAY();
    
    /* 自动对焦 */
    cam_focus_mode(CAM_DEFAULT_FOCUS_MODE);
    LOAD_DEFAULT_DELAY();
    
    /* 自动聚焦模式 */
    
    /* 自动聚焦灵敏度 */
    cam_af_sens(CAM_DEFAULT_AF_SENS);
    LOAD_DEFAULT_DELAY();
    
    /* 白平衡 */
    cam_wb_mode(CAM_DEFAULT_WB_MODE);
    LOAD_DEFAULT_DELAY();
    
    /* 曝光模式 */
    cam_ae_mode(CAM_DEFAULT_AE_MODE);
    LOAD_DEFAULT_DELAY();
    
    /* 低速快门 */
    
    /* 曝光补偿 */
    cam_expcomp_direct(CAM_DEFAULT_EXPCOMP_VAL);
    LOAD_DEFAULT_DELAY();
    
    /* 背光补偿 */
    cam_backlight(CAM_DEFAULT_BACKLIGHT);
    LOAD_DEFAULT_DELAY();
    
    /* 高光抑制 */
    cam_highlight(CAM_DEFAULT_HIGHLIGHT);
    LOAD_DEFAULT_DELAY();
    
    /* 锐度 */
    cam_sharpness_direct(CAM_DEFAULT_APERTURE);
    LOAD_DEFAULT_DELAY();
    
    /* 饱和度 */
    cam_saturation(CAM_DEFAULT_SATURATION);
    LOAD_DEFAULT_DELAY();
    
    /* 对比度 */
    cam_constrast_direct(CAM_DEFAULT_CONSTRAST);
    LOAD_DEFAULT_DELAY();
    
    /* 伽马 */
    cam_gamma(CAM_DEFAULT_GAMMA);
    LOAD_DEFAULT_DELAY();
    
    /* 色调 */
    
    /* 去雾 */
    cam_defog(VISCA_ON, CAM_DEFAULT_DEFOG);
    LOAD_DEFAULT_DELAY();
    
    /* 2D降噪 */
    cam_2dnr(CAM_DEFAULT_2DNR);
    LOAD_DEFAULT_DELAY();
    
    /* 3D降噪 */
    cam_3dnr(CAM_DEFAULT_NR);
    LOAD_DEFAULT_DELAY();
    
    /* 频闪抑制 */
    cam_flick(CAM_DEFAULT_FLICK);
    LOAD_DEFAULT_DELAY();
    
    /* 宽动态范围 */
    cam_wdr(CAM_DEFAULT_WDR);
    LOAD_DEFAULT_DELAY();
    
    /* 图像翻转 */
    cam_flip(CAM_DEFAULT_FLIP);
    LOAD_DEFAULT_DELAY();
    
    /* 图像镜像 */
    cam_mirror(CAM_DEFAULT_MIRROR);
    LOAD_DEFAULT_DELAY();
    
    /* 图像屏蔽 */
    cam_mute(CAM_DEFAULT_MUTE);
    LOAD_DEFAULT_DELAY();
    
    /* 图像冻结 */
    cam_freeze(CAM_DEFAULT_FREEZE);
    LOAD_DEFAULT_DELAY();
    
    /* 信息显示 */
    cam_datascreen(CAM_DEFAULT_DISPLAY);
    LOAD_DEFAULT_DELAY();
}

/***************************************************************************************************
* Description:  相机操作接口
***************************************************************************************************/
struct camera_operations camera_ops;
int32_t camera_ops_init(void)
{
    /* 系统 */
    camera_ops.ctl_power = sys_power;
    camera_ops.inq_power = inq_power_status;
    
    /* 菜单 */
    camera_ops.ctl_menu = sys_menu;
    camera_ops.ctl_menu_enter = sys_menu_enter;
    camera_ops.inq_menu = inq_menu;
    
    /* 变倍 */
    camera_ops.ctl_zoom = cam_zoom;
    camera_ops.ctl_zoom_direct = cam_zoom_direct;
    camera_ops.ctl_zoom_focus_direct = cam_zoom_focus_direct;
    camera_ops.inq_zoom = inq_zoom_pos;
    
    /* 数字变倍 */
    //    camera_ops.ctl_dzoom_mode = cam_dzoom_mode;
    camera_ops.ctl_dzoom = cam_dzoom;
    //    camera_ops.ctl_dzoom_direct = cam_dzoom_direct;
    camera_ops.inq_dzoom_mode = inq_dzoom_mode;
    //    camera_ops.inq_dzoom = inq_dzoom_pos;
    
    /* 聚焦 */
    camera_ops.ctl_focus_mode = cam_focus_mode;
    camera_ops.ctl_focus = cam_focus;
    camera_ops.ctl_focus_direct = cam_focus_direct;
    camera_ops.inq_focus_mode = inq_focus_mode;
    camera_ops.inq_focus = inq_focus_pos;
    
    /* 白平衡 */
    camera_ops.ctl_wb_mode = cam_wb_mode;
    camera_ops.ctl_rgain = cam_rgain;
    camera_ops.ctl_rgain_direct = cam_rgain_direct;
    camera_ops.ctl_bgain = cam_bgain;
    camera_ops.ctl_bgain_direct = cam_bgain_direct;
    camera_ops.inq_wb_mode = inq_wb_mode;
    camera_ops.inq_rgain = inq_rgain;
    camera_ops.inq_bgain = inq_bgain;
    
    /* 曝光 */
    camera_ops.ctl_ae_mode = cam_ae_mode;
    camera_ops.ctl_shutter = cam_shutter;
    camera_ops.ctl_shutter_direct = cam_shutter_direct;
    camera_ops.ctl_iris = cam_iris;
    camera_ops.ctl_iris_direct = cam_iris_direct;
    camera_ops.ctl_gain = cam_gain;
    camera_ops.ctl_gain_direct = cam_gain_direct;
    camera_ops.ctl_bright = cam_bright;
    camera_ops.ctl_bright_direct = cam_bright_direct;
    camera_ops.inq_ae_mode = inq_ae_mode;
    camera_ops.inq_shutter = inq_shutter;
    camera_ops.inq_iris = inq_iris;
    camera_ops.inq_gain = inq_gain;
    camera_ops.inq_bright = inq_bright;
    
    /* 曝光补偿 */
    //    camera_ops.ctl_exp_comp_onoff = cam_expcomp_mode;
    camera_ops.ctl_exp_comp = cam_expcomp;
    camera_ops.ctl_exp_comp_direct = cam_expcomp_direct;
    //    camera_ops.inq_exp_comp_onoff = inq_expcomp_mode;
    camera_ops.inq_exp_comp = inq_expcomp;
    
    /* 背光补偿 */
    camera_ops.ctl_backlight = cam_backlight;
    camera_ops.inq_backlight = inq_backlight;
    
    /* 锐度 */
    camera_ops.ctl_aperture = cam_sharpness;
    camera_ops.ctl_aperture_direct = cam_sharpness_direct;
    camera_ops.inq_aperture = inq_sharpness;
    
    /* 伽马 */
    camera_ops.ctl_gamma = cam_gamma;
    camera_ops.inq_gamma = inq_gamma;
    
    /* 饱和度 */
    camera_ops.ctl_saturation = cam_saturation;
    camera_ops.inq_saturation = inq_saturation;
    
    /* 色调 */
    //    camera_ops.ctl_hue = cam_hue;
    //    camera_ops.inq_hue = inq_hue;
    
    /* 去雾 */
    camera_ops.ctl_defog = cam_defog;
    camera_ops.inq_defog = inq_defog;
    
    /* 降噪 */
    camera_ops.ctl_nr = cam_3dnr;
    camera_ops.inq_nr = inq_3dnr;
    
    /* 频闪 */
    camera_ops.ctl_flick = cam_flick;
    camera_ops.inq_flick = inq_flick;
    
    /* 宽动态范围 */
    camera_ops.ctl_wdr = cam_wdr;
    camera_ops.inq_wdr = inq_wdr;
    
    /* 翻转 */
    camera_ops.ctl_flip = cam_flip;
    camera_ops.inq_flip = inq_flip;
    
    /* 镜像 */
    camera_ops.ctl_mirror = cam_mirror;
    camera_ops.inq_mirror = inq_mirror;
    
    /* 屏蔽 */
    camera_ops.ctl_mute = cam_mute;
    camera_ops.inq_mute = inq_mute;
    
    /* 冻结 */
    camera_ops.ctl_freeze = cam_freeze;
    camera_ops.inq_freeze = inq_freeze;
    
    /* 预置位，内存 */
    camera_ops.ctl_preset = cam_preset;
    camera_ops.inq_preset_call = inq_preset_call;
    camera_ops.inq_preset = inq_preset;
    camera_ops.inq_preset_ext = inq_preset_ext;
    
    /* 寄存器 */
    camera_ops.ctl_reg = cam_reg;
    camera_ops.inq_reg = inq_reg;
    
    /* 云台上下左右、绝对位置、相对位置、回位 */
    camera_ops.pt_run = pt_run;
    camera_ops.pt_abs_run = pt_abs_run;
    camera_ops.pt_rel_run = pt_rel_run;
    camera_ops.ptz_home = ptz_home;
    camera_ops.inq_pt_pos = inq_pt_pos;
    
    /* 重置 */
    camera_ops.ctl_init = cam_init;
    camera_ops.ptz_reset = ptz_reset;
    
    /* 版本信息 */
    camera_ops.inq_version = inq_version;
    
    /* 水平垂直反向 */
    camera_ops.pan_reverse = pan_reverse;
    camera_ops.tilt_reverse = tilt_reverse;
    camera_ops.inq_pan_reverse = inq_pan_reverse;
    camera_ops.inq_tilt_reverse = inq_tilt_reverse;
    
    /* 网络 */
    camera_ops.net_dhcp = cam_dhcp;
    camera_ops.inq_net_dhcp = inq_dhcp;
    camera_ops.net_ip = cam_net;
    camera_ops.inq_net_ip = inq_net;
    
    return 0;
}


/****************************************** END OF FILE *******************************************/
