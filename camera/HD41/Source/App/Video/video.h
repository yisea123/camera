/***************************************************************************************************
* FILE: video.h
*
* DESCRIPTION: --
*
* CREATED: 2018/5/16, by kevin
***************************************************************************************************/
#ifndef __VIDEO_H_
#define __VIDEO_H_

#include "bsp.h"


/***************************************************************************************************
* DEFINES
***************************************************************************************************/
typedef enum en_video_format
{
    VIDEO_NOP=0,
    VIDEO_320x240p60,
    VIDEO_640x480p60,
    VIDEO_800x600p30,
    VIDEO_800x600p60,
    
    /* 1280x720 */
    VIDEO_1280x720p15,
    VIDEO_1280x720p25,
    VIDEO_1280x720p29,
    VIDEO_1280x720p30,
    VIDEO_1280x720p50,
    VIDEO_1280x720p59,
    VIDEO_1280x720p60,
    
    /* 1920x1080 */
    VIDEO_1920x1080p25,
    VIDEO_1920x1080p29,
    VIDEO_1920x1080p30,
    VIDEO_1920x1080p50,
    VIDEO_1920x1080p59,
    VIDEO_1920x1080p60,
    VIDEO_1920x1080i50,
    VIDEO_1920x1080i59,
    VIDEO_1920x1080i60,

    /* 3840x2160 */
    VIDEO_3840x2160p25,
    VIDEO_3840x2160p29,
    VIDEO_3840x2160p30,
    VIDEO_3840x2160p50,
    VIDEO_3840x2160p59,
    VIDEO_3840x2160p60,
    
    /* 7680x4320 */
    VIDEO_7680x4320p25,
    VIDEO_7680x4320p29,
    VIDEO_7680x4320p30,
    VIDEO_7680x4320p50,
    VIDEO_7680x4320p59,
    VIDEO_7680x4320p60,
    VIDEO_BUTT,
}EN_VIDEO_FORMAT;


/* SONY机芯视频格式代码 */
#define SONY_VIDEO_1280x720p25      0x11
#define SONY_VIDEO_1280x720p29      0x0E
#define SONY_VIDEO_1280x720p30      0x0F
#define SONY_VIDEO_1280x720p50      0x0C
#define SONY_VIDEO_1280x720p59      0x09
#define SONY_VIDEO_1280x720p60      0x0A

#define SONY_VIDEO_1920x1080p25     0x08
#define SONY_VIDEO_1920x1080p29     0x06
#define SONY_VIDEO_1920x1080p30     0x07
#define SONY_VIDEO_1920x1080p50     0x14
#define SONY_VIDEO_1920x1080p59     0x13
#define SONY_VIDEO_1920x1080p60     0x15

#define SONY_VIDEO_1920x1080i50     0x04
#define SONY_VIDEO_1920x1080i59     0x01
#define SONY_VIDEO_1920x1080i60     0x02

#define SONY_VIDEO_3840x2160p25     0x1E
#define SONY_VIDEO_3840x2160p29     0x1D
//#define SONY_VIDEO_3840x2160p30
//#define SONY_VIDEO_3840x2160p50     
//#define SONY_VIDEO_3840x2160p59     
//#define SONY_VIDEO_3840x2160p60    

/* 贝莱特、熠和微机芯视频格式代码 */
#define YHW_VIDEO_320x240p60        0xD5
#define YHW_VIDEO_640x480p60        0xD1
#define YHW_VIDEO_800x600p30        0x51
#define YHW_VIDEO_800x600p60        0x51
#define YHW_VIDEO_1280x720p15       0x43
#define YHW_VIDEO_1280x720p25       0x11
#define YHW_VIDEO_1280x720p29       0x0E
#define YHW_VIDEO_1280x720p30       0x0E
#define YHW_VIDEO_1280x720p50       0x0C
#define YHW_VIDEO_1280x720p59       0x09
#define YHW_VIDEO_1280x720p60       0x09

#define YHW_VIDEO_1920x1080p25      0x08
#define YHW_VIDEO_1920x1080p29      0x06
#define YHW_VIDEO_1920x1080p30      0x06
#define YHW_VIDEO_1920x1080p50      0x2F
#define YHW_VIDEO_1920x1080p59      0x2E
#define YHW_VIDEO_1920x1080p60      0x2E

#define YHW_VIDEO_1920x1080i50      0x04
#define YHW_VIDEO_1920x1080i59      0x01
#define YHW_VIDEO_1920x1080i60      0x01

#define YHW_VIDEO_3840x2160p25      0x1E
#define YHW_VIDEO_3840x2160p29      0x1D
#define YHW_VIDEO_3840x2160p30      0x1D
//#define YHW_VIDEO_3840x2160p50     
//#define YHW_VIDEO_3840x2160p59     
//#define YHW_VIDEO_3840x2160p60    


/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void SetMainVideoFormat(uint8_t res);
void SetCameraVideoFormat(uint8_t res);

uint8_t VideoFormat2Cmd(uint8_t format);
uint8_t Cmd2VideoFormat(uint8_t cmd);


void Video_Init(void);
#endif
/****************************************** END OF FILE *******************************************/
