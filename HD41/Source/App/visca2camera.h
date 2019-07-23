/***************************************************************************************************
* FILE: visca_camera.h

* DESCRIPTION:
  --

* CREATED: 2017/9/25, by kevin xu
****************************************************************************************************
* MODIFY:  kevinxu

* DATE: 2018/03/16

* DESCRIPTION:

***************************************************************************************************/
#ifndef __VISCA_CAMERA_H_
#define __VISCA_CAMERA_H_
#include "stdint.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/********************************************** 回码 **********************************************/
#define VISCA_ACK                   0x41
#define VISCA_COMPLETE              0x51
#define VISCA_ERROR                 0x60
#define VISCA_ERROR_LEN             0x01
#define VISCA_ERROR_SYNTAX          0x02
#define VISVA_ERROR_BUF_FULL        0x03
#define VISCA_ERROR_CMD_CANCEL      0x04
#define VISCA_ERROR_NO_SOCKET       0x05
#define VISCA_ERROR_NOT_EXE         0x41


/****************************************** 电源控制 **********************************************/
#define VISCA_POWER                 0x00
#define VISCA_POWER_ON              0x02
#define VISCA_POWER_OFF             0x03

/******************************************* 机芯初始化 *******************************************/
#define VISCA_CAM_INIT              0x19
#define VISCA_CAM_INIT_LENS         0x01
#define VISCA_CAM_INIT_CAM          0x03

/******************************************** ZOOM ************************************************/
#define VISCA_ZOOM                  0x07
#define VISCA_ZOOM_STOP             0x00
#define VISCA_ZOOM_TELE_STD         0x02
#define VISCA_ZOOM_WIDE_STD         0x03
#define VISCA_ZOOM_TELE_VAR         0x20
#define VISCA_ZOOM_WIDE_VAR         0x30
#define VISCA_ZOOM_DIRECT           0x47

/******************************************** DZOOM ***********************************************/
#define VISCA_DZOOM                 0x06
#define VISCA_DZOOM_ON              0x02
#define VISCA_DZOOM_OFF             0x03
#define VISCA_DZOOM_STOP            0x00
#define VISCA_DZOOM_TELE_VAR        0x20
#define VISCA_DZOOM_WIDE_VAR        0x30
#define VISCA_DZOOM_X1              0x10

/***************************************** DZOOM MODE *********************************************/
#define VISCA_DZOOM_MODE            0x36
#define VISCA_DZOOM_MODE_CMB        0x00        //Optical/Digital Zoom Combined
#define VISCA_DZOOM_MODE_SPR        0x01        //Optical/Digital Zoom Separate
#define VISCA_DZOOM_DIRECT          0x46
#define VISCA_DZOOM_MODE_INQ        0x06
#define VISCA_DZOOM_POS_INQ         0x46

/******************************************** AF  *************************************************/
#define VISCA_FOCUS                 0x08
#define VISCA_FOCUS_STOP            0x00
#define VISCA_FOCUS_FAR_STD         0x02
#define VISCA_FOCUS_NEAR_STD        0x03
#define VISCA_FOCUS_FAR_VAR         0x20
#define VISCA_FOCUS_NEAR_VAR        0x30
#define VISCA_FOCUS_DIRECT          0x48

/***************************************** Focus Mode *********************************************/
#define VISCA_FOCUS_MODE            0x38
#define VISCA_FOCUS_AUTO            0x02
#define VISCA_FOCUS_MANUAL          0x03
#define VISCA_FOCUS_AM              0x10
#define VISCA_FOCUS_ONE_PUSH_TRIG   0x18
#define VISCA_FOCUS_NEAR_LIMIT      0x28

/****************************************** AF敏感度 **********************************************/
#define VISCA_AFS                   0x58 //AF Sensitivity
#define VISCA_AFS_LOW               0x03
#define VISCA_AFS_NORMAL            0x02
#define VISCA_AFS_HIGH              0x01

/******************************************* AF 模式 **********************************************/
#define VISCA_AF_MODE               0x57
#define VISCA_AF_MODE_NORMAL        0x00
#define VISCA_AF_MODE_INTERVAL      0x01
#define VISCA_AF_MODE_ZOOM_TRIGER   0x02

/********************************************* 白平衡 *********************************************/
#define VISCA_WB                    0x35
#define VISCA_WB_AUTO               0x00
#define VISCA_WB_INDOOR             0x01
#define VISCA_WB_OUTDOOR            0x02
#define VISCA_WB_ONE_PUSH_WB        0x03
#define VISCA_WB_ATW                0x04
#define VISCA_WB_MANUAL             0x05
#define VISCA_WB_ONE_PUSH_TRIG      0x10
#define VISCA_WB_OUTDOOR_AUTO       0x06
#define VISCA_WB_SL_AUTO            0x07
#define VISCA_WB_SL                 0x08
#define VISCA_WB_SL_OUTDOOR_AUTO    0x09

/********************************************* 红增益 *********************************************/
#define VISCA_RGAIN                 0x03
#define VISCA_RGAIN_RESET           0x00
#define VISCA_RGAIN_UP              0x02
#define VISCA_RGAIN_DOWN            0x03
#define VISCA_RGAIN_DIRECT          0x43

/********************************************* 蓝增益 *********************************************/
#define VISCA_BGAIN                 0x04
#define VISCA_BGAIN_RESET           0x00
#define VISCA_BGAIN_UP              0x02
#define VISCA_BGAIN_DOWN            0x03
#define VISCA_BGAIN_DIRECT          0x44

/******************************************* 自动曝光 *********************************************/
#define VISCA_AE                    0x39
#define VISCA_AE_FULL_AUTO          0x00
#define VISCA_AE_MANUAL             0x03
#define VISCA_AE_SHUTTER_PRIORITY   0x0A
#define VISCA_AE_IRIS_PRIORITY      0x0B
#define VISCA_AE_BRIGHT_PRIORITY    0x0D
#define VISCA_AE_MODE_INQ           0x39

/******************************************** 低速快门 ********************************************/
#define VISCA_SLOW_SHUTTER          0x5A
#define VISCA_SLOW_SHUTTER_ON       0x02
#define VISCA_SLOW_SHUTTER_OFF      0x03

/********************************************** 快门 **********************************************/
#define VISCA_SHUTTER               0x0A
#define VISCA_SHUTTER_RESET         0x00
#define VISCA_SHUTTER_UP            0x02
#define VISCA_SHUTTER_DOWN          0x03
#define VISCA_SHUTTER_DIRECT        0x4A

/********************************************** 光圈 **********************************************/
#define VISCA_IRIS                  0x0B
#define VISCA_IRIS_RESET            0x00
#define VISCA_IRIS_UP               0x02
#define VISCA_IRIS_DOWM             0x03
#define VISCA_IRIS_DIRECT           0x4B

/******************************************* 曝光增益 *********************************************/
#define VISCA_GAIN                  0x0C
#define VISCA_GAIN_RESET            0x00
#define VISCA_GAIN_UP               0x02
#define VISCA_GAIN_DOWN             0x03
#define VISCA_GAIN_DIRECT           0x4C
#define VISCA_GAIN_LIMIT            0x2C

/********************************************** 亮度 **********************************************/
#define VISCA_BRIGHT                0x0D
#define VISCA_BRIGHT_RESET          0x00
#define VISCA_BRIGHT_UP             0x02
#define VISCA_BRIGHT_DOWN           0x03
#define VISCA_BRIGHT_DIRECT         0x4D

/******************************************* 曝光补偿 *********************************************/
#define VISCA_EXPCOMP_ONOFF         0x3E
#define VISCA_EXPCOMP_ON            0x02
#define VISCA_EXPCOMP_OFF           0x03
#define VISCA_EXPCOMP               0x0E
#define VISCA_EXPCOMP_RESET         0x00
#define VISCA_EXPCOMP_UP            0x02
#define VISCA_EXPCOMP_DOWN          0x03
#define VISCA_EXPCOMP_DIRECT        0x4E

/******************************************* 背光补偿 *********************************************/
#define VISCA_BACKLIGHT             0x33
#define VISCA_BACKLIGHT_ON          0x02
#define VISCA_BACKLIGHT_OFF         0x03

/********************************************** 锐度***********************************************/
#define VISCA_APERTURE              0x02
#define VISCA_APERTURE_RESET        0x00
#define VISCA_APERTURE_UP           0x02
#define VISCA_APERTURE_DOWN         0x03
#define VISCA_APERTURE_DIRECT       0x42

/******************************************** 夜晚模式 ********************************************/
#define VISCA_AUTOICR               0x51
#define VISCA_AUTOICR_ON            0x02
#define VISCA_AUTOICR_OFF           0x03

#define VISCA_ICR                   0x01
#define VISCA_ICR_ON                0x02
#define VISCA_ICR_OFF               0x03

/******************************************** 图像翻转 ********************************************/
#define VISCA_PIC_FLIP              0x66
#define VISCA_PIC_ON                0x02
#define VISCA_PIC_OFF               0x03

/******************************************** 图像镜像 ********************************************/
#define VISCA_LR_REVERSE            0x61

/******************************************** 图像屏蔽 ********************************************/
#define VISCA_MUTE                  0x75
#define VISCA_MUTE_ON               0x02
#define VISCA_MUTE_OFF              0x03
#define VISCA_MUTE_ON_OFF           0x10

/******************************************** 图像冻结 ********************************************/
#define VISCA_FREEZE                0x62
#define VISCA_FREEZE_ON             0x02
#define VISCA_FREEZE_OFF            0x03

/******************************************** 信息显示 ********************************************/
#define VISCA_DISPLAY               0x15
#define VISCA_DISPLAY_ON            0x02
#define VISCA_DISPLAY_OFF           0x03

/**************************************** 色彩增益（饱和度） **************************************/
#define VISCA_COLOR_GAIN            0x49

/********************************************** 色调 **********************************************/
#define VISCA_HUE                   0x4F

/********************************************** 去雾 **********************************************/
#define VISCA_DEFOG                 0x37

/********************************************** 降噪 **********************************************/
#define VISCA_NR                    0x53

/********************************************* GAMMA **********************************************/
#define VISCA_GAMMA                 0x5B
#define VISCA_GAMMA_OFFSET          0x1E

/********************************************* 预置位 *********************************************/
#define VISCA_MEMORY                0x3F
#define VISCA_MEMORY_DELETE         0x00
#define VISCA_MEMORY_SET            0x01
#define VISCA_MEMORY_CALL           0x02

/********************************************* 寄存器 *********************************************/
#define VISCA_REG                   0x24
#define VISCA_REG_RES               0x72

/******************************************* 宽动态范围 *******************************************/
#define VISCA_WDR                   0x3D
#define VISCA_WDR_LEVEL             0xD3
#define VISCA_WDR_LEVEL_INQ         0xD3

/********************************************* FLICK **********************************************/
#define VISCA_FLICK                 0x32
#define VISCA_FLICK_OFF             0x00
#define VISCA_FLICK_50HZ            0x01
#define VISCA_FLICK_60HZ            0x02

/********************************************* 云台 ***********************************************/
#define VISCA_PT_RUN                0x01
#define VISCA_PT_ABS_POS            0x02
#define VISCA_PT_REL_POS            0x03
#define VISCA_PT_HOME               0x04
#define VISCA_PT_RESET              0x05
#define VISCA_PT_MENU               0x06
#define VISCA_PT_INQ_POS            0x12

/********************************************* 菜单 ***********************************************/
#define VISCA_MENU_ON_OFF           0x10
#define VISCA_MENU_ON               0x02
#define VISCA_MENU_OFF              0x03


/************************************** 区别于标准VISCA的指令 *************************************/
#define YHW_CAM_MENU                0x7F
#define YHW_CAM_MENU_INQ            0x7F 

#define YHW_PICTURE_RESET           0xA0
#define YHW_SATURATION              0xA1
#define YHW_CONSTRAST               0xA2
#define YHW_DEFOG                   0xA3
#define YHW_NEWBRIGHT               0xA4
#define YHW_2DNR                    0xA5
#define YHW_HIGHLIGHT               0xA6

#define YHW_FLICK                   0xAA
#define YHW_FLICK_OFF               0x00
#define YHW_FLICK_50HZ              0x01
#define YHW_FLICK_60HZ              0x02

#define YHW_NET_IP                  0xAB
#define YHW_NET_MASK                0xAC
#define YHW_NET_GATEWAY             0xAD
#define YHW_NET_DHCP                0xAE

#define YHW_MUTE                    0xBC
#define YHW_MUTE_OFF                0x00
#define YHW_MUTE_BLACK              0x01
#define YHW_MUTE_BLUE               0x02

#define YHW_CAM_ROATE               0xC0
#define YHW_CAM_ROATE_OFF           0x00
#define YHW_CAM_ROATE_ON            0x01

#define YHW_USB                     0xC6
#define YHW_USB_OFF                 0
#define YHW_USB_ON                  1

/************************************* 扩充指令 8x 01 07 xx ***************************************/
#define EXT_VERSION                 0x00    //版本号
#define EXT_PRESET                  0x20    //预置位块
#define EXT_PAN_REVERSE             0x21    //水平反向
#define EXT_TILT_REVERSE            0x22    //垂直反向
#define EXT_NET                     0x23    //网络
#define EXT_NET_DHCP                    0x00    //DHCP开关
#define EXT_NET_IP                      0x01    //IP地址
#define EXT_NET_MASK                    0x02    //子网掩码
#define EXT_NET_GATEWAY                 0x03    //网关


/********************************************* TITLE **********************************************/
#define TITLE_OPEN                  2
#define TITLE_CLOSE                 3
#define TITLE_CLEAR                 1

#define TITLE_WHITE                 0x00
#define TITLE_YELLOW                0x01
#define TITLE_VIOLET                0x02
#define TITLE_RED                   0x03
#define TITLE_CYAN                  0x04
#define TITLE_GREEN                 0x05
#define TITLE_BLUE                  0x06

/* 操作码 */
#define VISCA_ON                    2
#define VISCA_OFF                   3
#define VISCA_AUTO                  2
#define VISCA_MANUAL                3
#define VISCA_UP                    2
#define VISCA_DOWN                  3
#define VISCA_RESET                 0

/* 寄存器代码 */
#define REG_VIDEO_FOMAT             0x72

/* SONY视频格式代码*/
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


#endif
/****************************************** END OF FILE *******************************************/
