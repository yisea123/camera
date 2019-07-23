/***************************************************************************************************
* FILE: video.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/5/16, by kevin
***************************************************************************************************/
#include "video.h"
#include "system.h"
#include "ccp.h"
#include "visca2camera.h"
#include "sii9022.h"


/***************************************************************************************************
* Description:  设置主通道视频格式
***************************************************************************************************/
void SetMainVideoFormat(uint8_t res)
{
    SetCameraVideoFormat(res);
    Sii9022_SetVideoFormat(res);
    System_SavePara();
}

/***************************************************************************************************
* Description:  设置Camera分辨率
***************************************************************************************************/
void SetCameraVideoFormat(uint8_t res)
{
    uint8_t code = 0;
    switch(res)
    {
        case VIDEO_320x240p60: code = YHW_VIDEO_320x240p60; break;
        case VIDEO_640x480p60: code = YHW_VIDEO_640x480p60; break;
        case VIDEO_800x600p30: code = YHW_VIDEO_800x600p30; break;
        case VIDEO_800x600p60: code = YHW_VIDEO_800x600p60; break;
        case VIDEO_1280x720p15: code = YHW_VIDEO_1280x720p15; break;
        case VIDEO_1280x720p25: code = YHW_VIDEO_1280x720p25; break;
        case VIDEO_1280x720p29: code = YHW_VIDEO_1280x720p29; break;
        case VIDEO_1280x720p30: code = YHW_VIDEO_1280x720p30; break;
        case VIDEO_1280x720p50: code = YHW_VIDEO_1280x720p50; break;
        case VIDEO_1280x720p59: code = YHW_VIDEO_1280x720p59; break;
        case VIDEO_1280x720p60: code = YHW_VIDEO_1280x720p60; break;
        case VIDEO_1920x1080p25: code = YHW_VIDEO_1920x1080p25; break;
        case VIDEO_1920x1080p29: code = YHW_VIDEO_1920x1080p29; break;
        case VIDEO_1920x1080p30: code = YHW_VIDEO_1920x1080p30; break;
        case VIDEO_1920x1080p50: code = YHW_VIDEO_1920x1080p50; break;
        case VIDEO_1920x1080p59: code = YHW_VIDEO_1920x1080p59; break;
        case VIDEO_1920x1080p60: code = YHW_VIDEO_1920x1080p60; break;
        case VIDEO_1920x1080i50: code = YHW_VIDEO_1920x1080i50; break;
        case VIDEO_1920x1080i59: code = YHW_VIDEO_1920x1080i59; break;
        case VIDEO_1920x1080i60: code = YHW_VIDEO_1920x1080i60; break;
        case VIDEO_3840x2160p25: code = YHW_VIDEO_3840x2160p25; break;
        case VIDEO_3840x2160p29: code = YHW_VIDEO_3840x2160p29; break;
        case VIDEO_3840x2160p30: code = YHW_VIDEO_3840x2160p30; break;
        default: code = YHW_VIDEO_1920x1080p30; break;
    }
    visca_set_reg_value(0x72, code, 0);
}

/***************************************************************************************************
* Description:  外部串口指令分辨率代码转换为内部使用的分辨率代码
***************************************************************************************************/
uint8_t Cmd2VideoFormat(uint8_t cmd)
{
    uint8_t format = 0;
    switch(cmd)
    {
        /* 1280x720 */
        case SONY_VIDEO_1280x720p25: format = VIDEO_1280x720p25; break;
        case SONY_VIDEO_1280x720p29: format = VIDEO_1280x720p29; break;
        case SONY_VIDEO_1280x720p30: format = VIDEO_1280x720p30; break;
        case SONY_VIDEO_1280x720p50: format = VIDEO_1280x720p50; break;
        case SONY_VIDEO_1280x720p59: format = VIDEO_1280x720p59; break;
        case SONY_VIDEO_1280x720p60: format = VIDEO_1280x720p60; break;
        
        /* 1920x1080 */
        case SONY_VIDEO_1920x1080p25: format = VIDEO_1920x1080p25; break;
        case SONY_VIDEO_1920x1080p29: format = VIDEO_1920x1080p29; break;
        case SONY_VIDEO_1920x1080p30: format = VIDEO_1920x1080p30; break;
        case SONY_VIDEO_1920x1080p50: format = VIDEO_1920x1080p50; break;
        case SONY_VIDEO_1920x1080p59: format = VIDEO_1920x1080p59; break;
        case SONY_VIDEO_1920x1080p60: format = VIDEO_1920x1080p60; break;
        case SONY_VIDEO_1920x1080i50: format = VIDEO_1920x1080i50; break;
        case SONY_VIDEO_1920x1080i59: format = VIDEO_1920x1080i59; break;
        case SONY_VIDEO_1920x1080i60: format = VIDEO_1920x1080i60; break;
        
        /* 3840x2160 */
        case SONY_VIDEO_3840x2160p25: format = VIDEO_3840x2160p25; break;
        case SONY_VIDEO_3840x2160p29: format = VIDEO_3840x2160p29; break;
        
        default: format = 0; break;
    }
    return format;
}

/***************************************************************************************************
* Description:  内部使用的分辨率代码转换为外部指令分辨率代码
***************************************************************************************************/
uint8_t VideoFormat2Cmd(uint8_t format)
{
    uint8_t cmd = 0;
    switch(format)
    {
        /* 1280x720 */
        case VIDEO_1280x720p25: format = SONY_VIDEO_1280x720p25; break;
        case VIDEO_1280x720p29: format = SONY_VIDEO_1280x720p29; break;
        case VIDEO_1280x720p30: format = SONY_VIDEO_1280x720p30; break;
        case VIDEO_1280x720p50: format = SONY_VIDEO_1280x720p50; break;
        case VIDEO_1280x720p59: format = SONY_VIDEO_1280x720p59; break;
        case VIDEO_1280x720p60: format = SONY_VIDEO_1280x720p60; break;
        
        /* 1920x1080 */
        case VIDEO_1920x1080p25: format = SONY_VIDEO_1920x1080p25; break;
        case VIDEO_1920x1080p29: format = SONY_VIDEO_1920x1080p29; break;
        case VIDEO_1920x1080p30: format = SONY_VIDEO_1920x1080p30; break;
        case VIDEO_1920x1080p50: format = SONY_VIDEO_1920x1080p50; break;
        case VIDEO_1920x1080p59: format = SONY_VIDEO_1920x1080p59; break;
        case VIDEO_1920x1080p60: format = SONY_VIDEO_1920x1080p60; break;
        case VIDEO_1920x1080i50: format = SONY_VIDEO_1920x1080i50; break;
        case VIDEO_1920x1080i59: format = SONY_VIDEO_1920x1080i59; break;
        case VIDEO_1920x1080i60: format = SONY_VIDEO_1920x1080i60; break;
        
        /* 3840x2160 */
        case VIDEO_3840x2160p25: format = SONY_VIDEO_3840x2160p25; break;
        case VIDEO_3840x2160p29: format = SONY_VIDEO_3840x2160p29; break;
        
        default: cmd = 0; break;
    }
    return cmd;
}

/***************************************************************************************************
* Description:  视频模块初始化
***************************************************************************************************/
void Video_Init(void)
{
    SetCameraVideoFormat(stSysInfo.video_format_out1);
    Sii9022_SetVideoFormat(stSysInfo.video_format_out1);
    visca_set_8bits_value(YHW_USB, YHW_USB_ON, 1);
    
}

/****************************************** END OF FILE *******************************************/
