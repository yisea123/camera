/***************************************************************************************************
* FILE: visca_if.h

* DESCRIPTION:
  --

* CREATED: 2018/9/5, by kevin xu

* MODIFY:

* DATE: 

* DESCRIPTION:

***************************************************************************************************/
#ifndef __VISCA_IF_H_
#define __VISCA_IF_H_

#include "stdint.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/****************************************** 电源控制 **********************************************/
#define VISCA_POWER                 0x00
#define VISCA_POWER_ON              0x02
#define VISCA_POWER_OFF             0x03
#define VISCA_POWER_INQ             0x00

/******************************************** ZOOM ************************************************/
#define VISCA_ZOOM                  0x07
#define VISCA_ZOOM_STOP             0x00
#define VISCA_ZOOM_TELE_STD         0x02
#define VISCA_ZOOM_WIDE_STD         0x03
#define VISCA_ZOOM_TELE_VAR         0x20
#define VISCA_ZOOM_WIDE_VAR         0x30
#define VISCA_ZOOM_DIRECT           0x47
#define VISCA_ZOOM_POS_INQ          0x47

/******************************************** DZOOM ***********************************************/
#define VISCA_DZOOM                 0x06
#define VISCA_DZOOM_ON              0x02
#define VISCA_DZOOM_OFF             0x03
#define VISCA_DZOOM_STOP            0x00
#define VISCA_DZOOM_TELE_VAR        0x20
#define VISCA_DZOOM_WIDE_VAR        0x30
#define VISCA_DZOOM_X1              0x10

/*************************************** DZOOM MODE ***********************************************/
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
#define VISCA_FOCUS_MODE_INQ        0x38
#define VISCA_FOCUS_POS_INQ         0x48

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
#define VISCA_AF_MODE_AI_TIME       0x27 //Active/Interval Time pq:Movement Time rs:Interval Time
#define VISCA_AF_MODE_INQ           0x57
#define VISCA_AF_SENSITIVITY_INQ    0x58

/**************************************** IR Correction *******************************************/
#define VISCA_IR_COR                0x11
#define VISCA_IR_COR_STD            0x00
#define VISCA_IR_COR_LIGHT          0x01

/****************************************** ZOOM FOCUS ********************************************/
#define VISCA_CAM_ZOOM_FOCUS        //0x47

/******************************************* 机芯初始化 *******************************************/
#define VISCA_CAM_INIT              0x19
#define VISCA_CAM_INIT_LENS         0x01
#define VISCA_CAM_INIT_CAM          0x03

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
#define VISCA_WB_MODE_INQ           0x35

/********************************************* 红增益 *********************************************/
#define VISCA_RGAIN                 0x03
#define VISCA_RGAIN_RESET           0x00
#define VISCA_RGAIN_UP              0x02
#define VISCA_RGAIN_DOWN            0x03
#define VISCA_RGAIN_DIRECT          0x43
#define VISCA_RGAIN_INQ             0x43

/********************************************* 蓝增益 *********************************************/
#define VISCA_BGAIN                 0x04
#define VISCA_BGAIN_RESET           0x00
#define VISCA_BGAIN_UP              0x02
#define VISCA_BGAIN_DOWN            0x03
#define VISCA_BGAIN_DIRECT          0x44
#define VISCA_BGAIN_INQ                 0x44

/******************************************* 自动曝光 *********************************************/
#define VISCA_AE                    0x39
#define VISCA_AE_FULL_AUTO          0x00
#define VISCA_AE_MANUAL             0x03
#define VISCA_AE_SHUTTER_PRIORITY   0x0A
#define VISCA_AE_IRIS_PRIORITY      0x0B
#define VISCA_AE_BRIGHT             0x0D
#define VISCA_AE_MODE_INQ           0x39

/******************************************** 低速快门 ********************************************/
#define VISCA_ASS                   0x5A
#define VISCA_ASS_ON                0x02
#define VISCA_ASS_OFF               0x03

/********************************************** 快门 **********************************************/
#define VISCA_SHUTTER               0x0A
#define VISCA_SHUTTER_RESET         0x00
#define VISCA_SHUTTER_UP            0x02
#define VISCA_SHUTTER_DOWN          0x03
#define VISCA_SHUTTER_DIRECT        0x4A
#define VISCA_SHUTTER_POS_INQ           0x4A

/********************************************** 光圈 **********************************************/
#define VISCA_IRIS                  0x0B
#define VISCA_IRIS_RESET            0x00
#define VISCA_IRIS_UP               0x02
#define VISCA_IRIS_DOWN             0x03
#define VISCA_IRIS_DIRECT           0x4B
#define VIACA_IRIS_POS_INQ          0x4B

/********************************************** 曝光 **********************************************/
#define VISCA_GAIN                  0x0C
#define VISCA_GAIN_RESET            0x00
#define VISCA_GAIN_UP               0x02
#define VISCA_GAIN_DOWN             0x03
#define VISCA_GAIN_DIRECT           0x4C
#define VISCA_GAIN_LIMIT            0x2C
#define VISCA_GAIN_POS_INQ          0x4C

/********************************************** 亮度 **********************************************/
#define VISCA_BRIGHT                0x0D
#define VISCA_BRIGHT_RESET          0x00
#define VISCA_BRIGHT_UP             0x02
#define VISCA_BRIGHT_DOWN           0x03
#define VISCA_BRIGHT_DIRECT         0x4D
#define VISVA_BRIGHT_POS_INQ        0X4D
//CAM_ExpComp
#define VISCA_EXPCOMP_POWER         0x3E
#define VISCA_EXPCOMP_POWR_ON       0x02
#define VISCA_EXPCOMP_POWR_OFF      0x03
#define VISCA_EXPCOMP               0x0E
#define VISCA_EXPCOMP_RESET         0x00
#define VISCA_EXPCOMP_UP            0x02
#define VISCA_EXPCOMP_DOWN          0x03
#define VISCA_EXPCOMP_DIRECT        0x4E

/********************************************** 背光 **********************************************/
#define VISCA_BACKLIGHT             0x33
#define VISCA_BACKLIGHT_ON          0x02
#define VISCA_BACKLIGHT_OFF         0x03
#define VISCA_BACKLIGHT_MODE_INQ    0x33

/********************************************** 锐度***********************************************/
#define VISCA_APERTURE              0x02
#define VISCA_APERTURE_RESET        0x00
#define VISCA_APERTURE_UP           0x02
#define VISCA_APERTURE_DOWN         0x03
#define VISCA_APERTURE_DIRECT       0x42
#define VISCA_APERTURE_INQ          0x42

/******************************************** 夜晚模式 ********************************************/
#define VISCA_AUTOICR               0x51
#define VISCA_AUTOICR_ON            0x02
#define VISCA_AUTOICR_OFF           0x03
#define VISCA_AUTOICR_INQ           0x51

#define VISCA_ICR                   0x01
#define VISCA_ICR_ON                0x02
#define VISCA_ICR_OFF               0x03
#define VISCA_ICR_INQ               0x01

/******************************************** 图像翻转 ********************************************/
#define VISCA_PIC_FLIP              0x66
#define VISCA_PIC_ON                0x02
#define VISCA_PIC_OFF               0x03

/******************************************** 图像屏蔽 ********************************************/
#define VISCA_MUTE                  0x75
#define VISCA_MUTE_ON               0x02
#define VISCA_MUTE_OFF              0x03
#define VISCA_MUTE_ON_OFF           0x10

/******************************************** 信息显示 ********************************************/
#define VISCA_DISPLAY               0x15
#define VISCA_DISPLAY_ON            0x02
#define VISCA_DISPLAY_OFF           0x03
#define VISCA_DISPALY_MODE_INQ      0x15

/********************************************* 预置位 *********************************************/
#define VISCA_PRESET                0x3F
#define VISCA_PRESET_DELETE         0x00
#define VISCA_PRESET_SET            0x01
#define VISCA_PRESET_CALL           0x02

/********************************************* 寄存器 *********************************************/
#define VISCA_REG                   0x24
#define VISCA_REG_RES               0x72
#define VISVA_RES_INQ               0x72        //分辨率

#define VISCA_PRESET_DELETE         0x00
#define VISCA_PRESET_SET            0x01
#define VISCA_PRESET_CALL           0x02
/****************************************** Wide Dynamic *******************************************/
#define VISCA_WD                    0x3D
#define VISCA_WD_LEVEL              0xD3


/************************************** 区别于标准VISCA的指令 *************************************/
#define VISCA_GAMMA                 0x5B
#define VISCA_GAMMA_INQ             0x5B

#define VISCA_SATURATION            0xA1
#define VISCA_SATURATION_INQ        0xA1

#define VISCA_CONSTRAST             0xA2
#define VISCA_CONSTRAST_INQ         0xA2

#define VISCA_NEWBRIGHT             0xA4
#define VISCA_NEWBRIGHT_INQ         0xA4

#define VISCA_WDR                   0x3D
#define VISCA_WDR_INQ               0x3D
#define VISAC_WDR_LEVEL             0xD3
#define VISAC_WDR_LEVEL_INQ         0x2D

#define VISCA_NR                    0x53
#define VISCA_NR_INQ                0x53

#define VISCA_HUE                   0x4F
#define VISCA_HUE_INQ               0x4F

#define VISCA_LR_REVERSE            0x61
#define VISCA_LR_REVERSE_INQ        0x61

#define VISCA_FLICK                 0xAA
#define VISCA_FLICK_INQ             0xAA
#define VISCA_FLICK_OFF             0x00
#define VISCA_FLICK_50HZ            0x01
#define VISCA_FLICK_60HZ            0x02

#define VISCA_CAM_MENU              0x7F
#define VISCA_CAM_MENU_INQ          0x7F 

/********************************************** 网络 **********************************************/
#define VISCA_NET_DHCP              0xAE
#define VISCA_NET_IP                0xAB
#define VISCA_NET_MASK              0xAC
#define VISCA_NET_GATE              0xAD

/********************************************** 回码 **********************************************/
#define VISCA_ACK                   0x41
#define VISCA_COMPLETION            0x51
#define VISCA_ERROR                 0x60
#define VISCA_LEN_ERROR             0x01
#define VISCA_SYNTAX_ERROR          0x02
#define VISVA_BUF_FULL              0x03
#define VISCA_CMD_CANCEL            0x04
#define VISCA_NO_SOCKET             0x05
#define VISCA_NOT_EXE               0x41

/********************************************* OSD相关 ********************************************/
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


#define VISCA_ON                    2
#define VISCA_OFF                   3
#define VISCA_AUTO                  2
#define VISCA_MANUAL                3

typedef struct NetInfo
{
    uint32_t ip;
    uint32_t mask;
    uint32_t gate;
}NetInfo;

#define VISCA_TIME_OVERFLOW             50
#define VISCA_INQ_WAIT_TIME             70

#define OSD_DELAY                       300


/********************************************* 操作接口 *******************************************/
struct visca_if
{
    void (*delay_ms)(uint32_t);
    void (*send_cmd)(const uint8_t *, uint8_t);
    void (*send_inq)(const uint8_t *, uint8_t);
    void (*clear_return_buf)(void);
    int32_t (*get_return_code)(uint8_t *);
};

/********************************************* 相机状态 *******************************************/
struct visca_camera_status
{
    /* 操作时间 */
    uint32_t act_timer;
    
    /* 菜单 */
    uint8_t osd;
    
    /* 曝光 */
    uint8_t exposure_mode;
    uint8_t iris_val;
    uint8_t shutter_val;
    uint8_t exposure_gain;
    uint8_t bright;
    uint8_t backlight;
    uint8_t spotlight;
    
    /* 色彩 */
    uint8_t wb_mode;
    uint8_t rgain;
    uint8_t bgain;
    
    /* 图像 */
    uint8_t nr;
    uint8_t gama;
    uint8_t flick;
    uint8_t mirror;
    uint8_t flip;
    uint8_t freeze;
    uint8_t icr;
    uint8_t auto_icr;
    uint8_t wd;
    uint8_t hue;                //色调
    uint8_t aperture;           //锐度
    uint8_t saturation;         //饱和度
    uint8_t constrast;          //对比度
    uint8_t defog;              //去雾
    uint8_t mute;
    uint8_t display;
    
    
    /* 聚焦 */
    uint8_t focus_mode;
    uint16_t focus_val;
    uint8_t af_mode;
    uint8_t af_sens;
    
    /* 变倍 */
    uint16_t zoom_val;
    uint8_t dzoom_mode;
    uint8_t dzoom_val;
    
    /* 云台 */
    int16_t pan_pos;
    int16_t tilt_pos;/***************************************************************************************************
* FILE: visca_if.h

* DESCRIPTION:
  --

* CREATED: 2018/9/5, by kevin xu

* MODIFY:

* DATE: 

* DESCRIPTION:

***************************************************************************************************/
#ifndef __VISCA_IF_H_
#define __VISCA_IF_H_

#include "stdint.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/****************************************** 电源控制 **********************************************/
#define VISCA_POWER                 0x00
#define VISCA_POWER_ON              0x02
#define VISCA_POWER_OFF             0x03
#define VISCA_POWER_INQ             0x00

/******************************************** ZOOM ************************************************/
#define VISCA_ZOOM                  0x07
#define VISCA_ZOOM_STOP             0x00
#define VISCA_ZOOM_TELE_STD         0x02
#define VISCA_ZOOM_WIDE_STD         0x03
#define VISCA_ZOOM_TELE_VAR         0x20
#define VISCA_ZOOM_WIDE_VAR         0x30
#define VISCA_ZOOM_DIRECT           0x47
#define VISCA_ZOOM_POS_INQ          0x47

/******************************************** DZOOM ***********************************************/
#define VISCA_DZOOM                 0x06
#define VISCA_DZOOM_ON              0x02
#define VISCA_DZOOM_OFF             0x03
#define VISCA_DZOOM_STOP            0x00
#define VISCA_DZOOM_TELE_VAR        0x20
#define VISCA_DZOOM_WIDE_VAR        0x30
#define VISCA_DZOOM_X1              0x10

/*************************************** DZOOM MODE ***********************************************/
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
#define VISCA_FOCUS_MODE_INQ        0x38
#define VISCA_FOCUS_POS_INQ         0x48

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
#define VISCA_AF_MODE_AI_TIME       0x27 //Active/Interval Time pq:Movement Time rs:Interval Time
#define VISCA_AF_MODE_INQ           0x57
#define VISCA_AF_SENSITIVITY_INQ    0x58

/**************************************** IR Correction *******************************************/
#define VISCA_IR_COR                0x11
#define VISCA_IR_COR_STD            0x00
#define VISCA_IR_COR_LIGHT          0x01

/****************************************** ZOOM FOCUS ********************************************/
#define VISCA_CAM_ZOOM_FOCUS        //0x47

/******************************************* 机芯初始化 *******************************************/
#define VISCA_CAM_INIT              0x19
#define VISCA_CAM_INIT_LENS         0x01
#define VISCA_CAM_INIT_CAM          0x03

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
#define VISCA_WB_MODE_INQ           0x35

/********************************************* 红增益 *********************************************/
#define VISCA_RGAIN                 0x03
#define VISCA_RGAIN_RESET           0x00
#define VISCA_RGAIN_UP              0x02
#define VISCA_RGAIN_DOWN            0x03
#define VISCA_RGAIN_DIRECT          0x43
#define VISCA_RGAIN_INQ             0x43

/********************************************* 蓝增益 *********************************************/
#define VISCA_BGAIN                 0x04
#define VISCA_BGAIN_RESET           0x00
#define VISCA_BGAIN_UP              0x02
#define VISCA_BGAIN_DOWN            0x03
#define VISCA_BGAIN_DIRECT          0x44
#define VISCA_BGAIN_INQ             0x44

/******************************************* 自动曝光 *********************************************/
#define VISCA_AE                    0x39
#define VISCA_AE_FULL_AUTO          0x00
#define VISCA_AE_MANUAL             0x03
#define VISCA_AE_SHUTTER_PRIORITY   0x0A
#define VISCA_AE_IRIS_PRIORITY      0x0B
#define VISCA_AE_BRIGHT_PRIORITY    0x0D
#define VISCA_AE_MODE_INQ           0x39

/******************************************** 低速快门 ********************************************/
#define VISCA_ASS                   0x5A
#define VISCA_ASS_ON                0x02
#define VISCA_ASS_OFF               0x03

/********************************************** 快门 **********************************************/
#define VISCA_SHUTTER               0x0A
#define VISCA_SHUTTER_RESET         0x00
#define VISCA_SHUTTER_UP            0x02
#define VISCA_SHUTTER_DOWN          0x03
#define VISCA_SHUTTER_DIRECT        0x4A
#define VISCA_SHUTTER_POS_INQ           0x4A

/********************************************** 光圈 **********************************************/
#define VISCA_IRIS                  0x0B
#define VISCA_IRIS_RESET            0x00
#define VISCA_IRIS_UP               0x02
#define VISCA_IRIS_DOWN             0x03
#define VISCA_IRIS_DIRECT           0x4B
#define VIACA_IRIS_POS_INQ          0x4B

/********************************************** 曝光 **********************************************/
#define VISCA_GAIN                  0x0C
#define VISCA_GAIN_RESET            0x00
#define VISCA_GAIN_UP               0x02
#define VISCA_GAIN_DOWN             0x03
#define VISCA_GAIN_DIRECT           0x4C
#define VISCA_GAIN_LIMIT            0x2C
#define VISCA_GAIN_POS_INQ          0x4C

/********************************************** 亮度 **********************************************/
#define VISCA_BRIGHT                0x0D
#define VISCA_BRIGHT_RESET          0x00
#define VISCA_BRIGHT_UP             0x02
#define VISCA_BRIGHT_DOWN           0x03
#define VISCA_BRIGHT_DIRECT         0x4D
#define VISVA_BRIGHT_POS_INQ        0X4D
//CAM_ExpComp
#define VISCA_EXPCOMP_POWER         0x3E
#define VISCA_EXPCOMP_POWR_ON       0x02
#define VISCA_EXPCOMP_POWR_OFF      0x03
#define VISCA_EXPCOMP               0x0E
#define VISCA_EXPCOMP_RESET         0x00
#define VISCA_EXPCOMP_UP            0x02
#define VISCA_EXPCOMP_DOWN          0x03
#define VISCA_EXPCOMP_DIRECT        0x4E

/********************************************** 背光 **********************************************/
#define VISCA_BACKLIGHT             0x33
#define VISCA_BACKLIGHT_ON          0x02
#define VISCA_BACKLIGHT_OFF         0x03
#define VISCA_BACKLIGHT_MODE_INQ    0x33

/********************************************** 锐度***********************************************/
#define VISCA_APERTURE              0x02
#define VISCA_APERTURE_RESET        0x00
#define VISCA_APERTURE_UP           0x02
#define VISCA_APERTURE_DOWN         0x03
#define VISCA_APERTURE_DIRECT       0x42
#define VISCA_APERTURE_INQ          0x42

/******************************************** 夜晚模式 ********************************************/
#define VISCA_AUTOICR               0x51
#define VISCA_AUTOICR_ON            0x02
#define VISCA_AUTOICR_OFF           0x03
#define VISCA_AUTOICR_INQ           0x51

#define VISCA_ICR                   0x01
#define VISCA_ICR_ON                0x02
#define VISCA_ICR_OFF               0x03
#define VISCA_ICR_INQ               0x01

/******************************************** 图像翻转 ********************************************/
#define VISCA_PIC_FLIP              0x66
#define VISCA_PIC_ON                0x02
#define VISCA_PIC_OFF               0x03

/******************************************** 图像屏蔽 ********************************************/
#define VISCA_MUTE                  0x75
#define VISCA_MUTE_ON               0x02
#define VISCA_MUTE_OFF              0x03
#define VISCA_MUTE_ON_OFF           0x10

/******************************************** 信息显示 ********************************************/
#define VISCA_DISPLAY               0x15
#define VISCA_DISPLAY_ON            0x02
#define VISCA_DISPLAY_OFF           0x03
#define VISCA_DISPALY_MODE_INQ      0x15

/********************************************* 预置位 *********************************************/
#define VISCA_PRESET                0x3F
#define VISCA_PRESET_DELETE         0x00
#define VISCA_PRESET_SET            0x01
#define VISCA_PRESET_CALL           0x02

/********************************************* 寄存器 *********************************************/
#define VISCA_REG                   0x24
#define VISCA_REG_RES               0x72
#define VISVA_RES_INQ               0x72        //分辨率

#define VISCA_PRESET_DELETE         0x00
#define VISCA_PRESET_SET            0x01
#define VISCA_PRESET_CALL           0x02
/****************************************** Wide Dynamic *******************************************/
#define VISCA_WD                    0x3D
#define VISCA_WD_LEVEL              0xD3

/********************************************** 回码 **********************************************/
#define VISCA_ACK                   0x41
#define VISCA_COMPLETION            0x51
#define VISCA_ERROR                 0x60
#define VISCA_LEN_ERROR             0x01
#define VISCA_SYNTAX_ERROR          0x02
#define VISVA_BUF_FULL              0x03
#define VISCA_CMD_CANCEL            0x04
#define VISCA_NO_SOCKET             0x05
#define VISCA_NOT_EXE               0x41

/********************************************* OSD相关 ********************************************/
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


/************************************** 区别于标准Visca的指令 *************************************/
#define VISCA_GAMMA                 0x5B
#define VISCA_GAMMA_INQ             0x5B

#define VISCA_SATURATION            0xA1
#define VISCA_SATURATION_INQ        0xA1

#define VISCA_CONSTRAST             0xA2
#define VISCA_CONSTRAST_INQ         0xA2

#define VISCA_NEWBRIGHT             0xA4
#define VISCA_NEWBRIGHT_INQ         0xA4

#define VISCA_WDR                   0x3D
#define VISCA_WDR_INQ               0x3D
#define VISAC_WDR_LEVEL             0xD3
#define VISAC_WDR_LEVEL_INQ         0x2D

#define VISCA_NR                    0x53
#define VISCA_NR_INQ                0x53

#define VISCA_HUE                   0x4F
#define VISCA_HUE_INQ               0x4F

#define VISCA_LR_REVERSE            0x61
#define VISCA_LR_REVERSE_INQ        0x61

#define VISCA_FLICK                 0xAA
#define VISCA_FLICK_INQ             0xAA
#define VISCA_FLICK_OFF             0x00
#define VISCA_FLICK_50HZ            0x01
#define VISCA_FLICK_60HZ            0x02

#define VISCA_CAM_MENU              0x7F
#define VISCA_CAM_MENU_INQ          0x7F 

/********************************************** 网络 **********************************************/
#define VISCA_NET_DHCP              0xAE
#define VISCA_NET_IP                0xAB
#define VISCA_NET_MASK              0xAC
#define VISCA_NET_GATE              0xAD


#define VISCA_ON                    2
#define VISCA_OFF                   3
#define VISCA_AUTO                  2
#define VISCA_MANUAL                3


#define VISCA_TIME_OVERFLOW             50
#define VISCA_INQ_WAIT_TIME             70

#define OSD_DELAY                       300


/********************************************* 操作接口 *******************************************/
struct visca_if
{
    void (*delay_ms)(uint32_t);
    int32_t (*send_cmd)(const uint8_t *, uint8_t);
    int32_t (*send_inq)(const uint8_t *, uint8_t);
    void (*clear_return_buf)(void);
    int32_t (*get_return_code)(uint8_t *);
};

/********************************************* 相机状态 *******************************************/
struct visca_camera_status
{
    /* 操作时间 */
    uint32_t act_timer;

    /* 菜单 */
    uint8_t osd;

    /* 曝光 */
    uint8_t exposure_mode;
    uint8_t iris_val;
    uint8_t shutter_val;
    uint8_t exposure_gain;
    uint8_t bright;
    uint8_t backlight;
    uint8_t spotlight;
    
    /* 色彩 */
    uint8_t wb_mode;
    uint8_t rgain;
    uint8_t bgain;
    
    /* 图像 */
    uint8_t nr;
    uint8_t gama;
    uint8_t flick;
    uint8_t mirror;
    uint8_t flip;
    uint8_t freeze;
    uint8_t icr;
    uint8_t auto_icr;
    uint8_t wd;
    uint8_t hue;                //色调
    uint8_t aperture;           //锐度
    uint8_t saturation;         //饱和度
    uint8_t constrast;          //对比度
    uint8_t defog;              //去雾
    uint8_t mute;
    uint8_t display;
    
    
    /* 聚焦 */
    uint8_t focus_mode;
    uint16_t focus_val;
    uint8_t af_mode;
    uint8_t af_sens;
    
    /* 变倍 */
    uint16_t zoom_val;
    uint8_t dzoom_mode;
    uint8_t dzoom_val;
    
    /* 云台 */
    int16_t pan_pos;
    int16_t tilt_pos;
    
    /* 杂项 */
    uint8_t resolution;    
};

void visca_if_set_ops(const struct visca_if *vif);

int32_t visca_set_8bits_value(uint8_t addr, uint8_t item, uint8_t value, uint8_t retry);
int32_t visca_set_16bits_value(uint8_t addr, uint8_t item, uint16_t value, uint8_t retry);
int32_t visca_set_reg_value(uint8_t addr, uint8_t reg, uint8_t value, uint8_t retry);
int32_t visca_set_net(uint8_t addr, uint8_t item, uint32_t value, uint8_t retry);

int32_t visca_inq_8bits_value(uint8_t addr, uint8_t item, uint8_t *value, uint8_t retry);
int32_t visca_inq_16bits_value(uint8_t addr, uint8_t item, uint16_t *value, uint8_t retry);
int32_t visca_inq_reg_value(uint8_t addr, uint8_t reg, uint8_t *value, uint8_t retry);
int32_t visca_inq_net(uint8_t addr, uint8_t item, uint32_t *value, uint8_t retry);

int32_t visca_reset_address(uint32_t waittime);
int32_t visca_set_memory(uint8_t addr, uint8_t type, uint8_t value, uint8_t retry);
int32_t visca_set_tally(uint8_t addr, uint8_t onoff, uint8_t retry);

int32_t visca_set_pan_tilt(uint8_t addr, uint8_t pan_sp, uint8_t tilt_sp, uint8_t pan_dir, uint8_t tilt_dir, uint8_t retry);
int32_t visca_set_pan_tilt_abs(uint8_t addr, uint8_t sp, int16_t pan_pos, int16_t tilt_pos, uint8_t retry);
int32_t visca_set_pan_tilt_rel(uint8_t addr, uint8_t sp, int16_t pan_pos, int16_t tilt_pos, uint8_t retry);
int32_t visca_set_pan_tilt_home(uint8_t addr, uint8_t retry);
int32_t visca_set_pan_tilt_reset(uint8_t addr, uint8_t retry);

int32_t visca_set_osd(uint8_t addr, uint8_t act, uint8_t retry);
int32_t visca_osd_enter(uint8_t addr, uint8_t retry);
int32_t visca_inq_osd(uint8_t addr, uint8_t *value, uint8_t retry);

//uint8_t visca_title_cmd(uint8_t line, uint8_t cmd);
//uint8_t visca_title_set(uint8_t line, uint8_t x, uint8_t color, uint8_t blink);
//uint8_t visca_title_show(uint8_t line, uint8_t *str);

//void visca_custom_reset(void);
//void visca_custom_set(void);

#endif
/****************************************** END OF FILE *******************************************/

    
    /* 杂项 */
    uint8_t resolution;    
};

/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void visca_if_set_ops(const struct visca_if *vif);

int32_t visca_set_8bits_value(uint8_t addr, uint8_t item, uint8_t value, uint8_t retry);
int32_t visca_set_16bits_value(uint8_t addr, uint8_t item, uint16_t value, uint8_t retry);
int32_t visca_set_reg_value(uint8_t addr, uint8_t reg, uint8_t value, uint8_t retry);
int32_t visca_set_net(uint8_t addr, uint8_t item, uint32_t value, uint8_t retry);

int32_t visca_inq_8bits_value(uint8_t addr, uint8_t item, uint8_t *value, uint8_t retry);
int32_t visca_inq_16bits_value(uint8_t addr, uint8_t item, uint16_t *value, uint8_t retry);
int32_t visca_inq_reg_value(uint8_t addr, uint8_t reg, uint8_t *value, uint8_t retry);
int32_t visca_inq_net(uint8_t addr, uint8_t item, uint32_t *value, uint8_t retry);

int32_t visca_reset_address(uint32_t waittime);
int32_t visca_set_memory(uint8_t addr, uint8_t type, uint8_t value, uint8_t retry);
int32_t visca_set_tally(uint8_t addr, uint8_t onoff, uint8_t retry);

int32_t visca_set_pan_tilt(uint8_t addr, uint8_t pan_sp, uint8_t tilt_sp, uint8_t pan_dir, uint8_t tilt_dir, uint8_t retry);
int32_t visca_set_pan_tilt_abs(uint8_t addr, uint8_t sp, int16_t pan_pos, int16_t tilt_pos, uint8_t retry);
int32_t visca_set_pan_tilt_rel(uint8_t addr, uint8_t sp, int16_t pan_pos, int16_t tilt_pos, uint8_t retry);
int32_t visca_set_pan_tilt_home(uint8_t addr, uint8_t retry);
int32_t visca_set_pan_tilt_reset(uint8_t addr, uint8_t retry);

int32_t visca_set_osd(uint8_t addr, uint8_t act, uint8_t retry);
int32_t visca_osd_enter(uint8_t addr, uint8_t retry);
int32_t visca_inq_osd(uint8_t addr, uint8_t *value, uint8_t retry);

//uint8_t visca_title_cmd(uint8_t line, uint8_t cmd);
//uint8_t visca_title_set(uint8_t line, uint8_t x, uint8_t color, uint8_t blink);
//uint8_t visca_title_show(uint8_t line, uint8_t *str);

//void visca_custom_reset(void);
//void visca_custom_set(void);




#endif
/****************************************** END OF FILE *******************************************/
