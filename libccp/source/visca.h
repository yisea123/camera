/***************************************************************************************************
* FILE: visca.h
*
* DESCRIPTION: --
*
* CREATED: 2018/12/2, by kevin
***************************************************************************************************/
#ifndef __VISCA_H__
#define __VISCA_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VISCA_ON                    2
#define VISCA_OFF                   3
#define VISCA_AUTO                  2
#define VISCA_MANUAL                3

/******************************************** TITLE ***********************************************/
#define TITLE_OPEN                  0
#define TITLE_CLOSE                 1
#define TITLE_CLEAR                 2

#define TITLE_WHITE                 0x00
#define TITLE_YELLOW                0x01
#define TITLE_VIOLET                0x02
#define TITLE_RED                   0x03
#define TITLE_CYAN                  0x04
#define TITLE_GREEN                 0x05
#define TITLE_BLUE                  0x06
    
/******************************************** ���� ************************************************/
#define VISCA_ACK                   0x41
#define VISCA_COMPLETE              0x51
#define VISCA_ERROR                 0x60
#define VISCA_ERROR_LEN             0x01
#define VISCA_ERROR_SYNTAX          0x02
#define VISVA_ERROR_BUF_FULL        0x03
#define VISCA_ERROR_CMD_CANCEL      0x04
#define VISCA_ERROR_NO_SOCKET       0x05
#define VISCA_ERROR_NOT_EXE         0x41
    
/****************************************** ��Դ���� **********************************************/
#define VISCA_POWER                 0x00
#define VISCA_POWER_ON              0x02
#define VISCA_POWER_OFF             0x03

/***************************************** ��о��ʼ�� *********************************************/
#define VISCA_CAM_INIT              0x19
#define VISCA_CAM_INIT_LENS         0x01
#define VISCA_CAM_INIT_CAM          0x03

/******************************************* ZOOM *************************************************/
#define VISCA_ZOOM                  0x07
#define VISCA_ZOOM_STOP             0x00
#define VISCA_ZOOM_TELE_STD         0x02
#define VISCA_ZOOM_WIDE_STD         0x03
#define VISCA_ZOOM_TELE_VAR         0x20
#define VISCA_ZOOM_WIDE_VAR         0x30
#define VISCA_ZOOM_DIRECT           0x47

/****************************************** DZOOM *************************************************/
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

/****************************************** AF���ж� **********************************************/
#define VISCA_AFS                   0x58 //AF Sensitivity
#define VISCA_AFS_LOW               0x03
#define VISCA_AFS_NORMAL            0x02
#define VISCA_AFS_HIGH              0x01

/******************************************* AF ģʽ **********************************************/
#define VISCA_AF_MODE               0x57
#define VISCA_AF_MODE_NORMAL        0x00
#define VISCA_AF_MODE_INTERVAL      0x01
#define VISCA_AF_MODE_ZOOM_TRIGER   0x02

/******************************************* ��ƽ�� ***********************************************/
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

/******************************************* ������ ***********************************************/
#define VISCA_RGAIN                 0x03
#define VISCA_RGAIN_RESET           0x00
#define VISCA_RGAIN_UP              0x02
#define VISCA_RGAIN_DOWN            0x03
#define VISCA_RGAIN_DIRECT          0x43

/******************************************* ������ ***********************************************/
#define VISCA_BGAIN                 0x04
#define VISCA_BGAIN_RESET           0x00
#define VISCA_BGAIN_UP              0x02
#define VISCA_BGAIN_DOWN            0x03
#define VISCA_BGAIN_DIRECT          0x44

/****************************************** �Զ��ع� **********************************************/
#define VISCA_EXP_MODE                    0x39
#define VISCA_EXP_MODE_FULL_AUTO          0x00
#define VISCA_EXP_MODE_MANUAL             0x03
#define VISCA_EXP_MODE_SHUTTER_PRIORITY   0x0A
#define VISCA_EXP_MODE_IRIS_PRIORITY      0x0B
#define VISCA_EXP_MODE_BRIGHT             0x0D
#define VISCA_EXP_MODE_MODE_INQ           0x39

/******************************************** ���ٿ��� ********************************************/
#define VISCA_EXP_SLOW_SHUTTER          0x5A
#define VISCA_EXP_SLOW_SHUTTER_ON       0x02
#define VISCA_EXP_SLOW_SHUTTER_OFF      0x03

/********************************************** ���� **********************************************/
#define VISCA_EXP_SHUTTER               0x0A
#define VISCA_EXP_SHUTTER_RESET         0x00
#define VISCA_EXP_SHUTTER_UP            0x02
#define VISCA_EXP_SHUTTER_DOWN          0x03
#define VISCA_EXP_SHUTTER_DIRECT        0x4A

/********************************************** ��Ȧ **********************************************/
#define VISCA_EXP_IRIS                  0x0B
#define VISCA_EXP_IRIS_RESET            0x00
#define VISCA_EXP_IRIS_UP               0x02
#define VISCA_EXP_IRIS_DOWM             0x03
#define VISCA_EXP_IRIS_DIRECT           0x4B

/******************************************* �ع����� *********************************************/
#define VISCA_EXP_GAIN                  0x0C
#define VISCA_EXP_GAIN_RESET            0x00
#define VISCA_EXP_GAIN_UP               0x02
#define VISCA_EXP_GAIN_DOWN             0x03
#define VISCA_EXP_GAIN_DIRECT           0x4C
#define VISCA_EXP_GAIN_LIMIT            0x2C

/********************************************** ���� **********************************************/
#define VISCA_EXP_BRIGHT                0x0D
#define VISCA_EXP_BRIGHT_RESET          0x00
#define VISCA_EXP_BRIGHT_UP             0x02
#define VISCA_EXP_BRIGHT_DOWN           0x03
#define VISCA_EXP_BRIGHT_DIRECT         0x4D

/******************************************* �عⲹ�� *********************************************/
#define VISCA_EXP_COMP_POWER         0x3E
#define VISCA_EXP_COMP_ON            0x02
#define VISCA_EXP_COMP_OFF           0x03
#define VISCA_EXP_COMP               0x0E
#define VISCA_EXP_COMP_RESET         0x00
#define VISCA_EXP_COMP_UP            0x02
#define VISCA_EXP_COMP_DOWN          0x03
#define VISCA_EXP_COMP_DIRECT        0x4E

/******************************************* ���ⲹ�� *********************************************/
#define VISCA_BACKLIGHT             0x33
#define VISCA_BACKLIGHT_ON          0x02
#define VISCA_BACKLIGHT_OFF         0x03

/********************************************** ���***********************************************/
#define VISCA_SHARPNESS              0x02
#define VISCA_SHARPNESS_RESET        0x00
#define VISCA_SHARPNESS_UP           0x02
#define VISCA_SHARPNESS_DOWN         0x03
#define VISCA_SHARPNESS_DIRECT       0x42

/******************************************** ҹ��ģʽ ********************************************/
#define VISCA_AUTOICR               0x51
#define VISCA_AUTOICR_ON            0x02
#define VISCA_AUTOICR_OFF           0x03

#define VISCA_ICR                   0x01
#define VISCA_ICR_ON                0x02
#define VISCA_ICR_OFF               0x03

/******************************************** ͼ��ת ********************************************/
#define VISCA_PIC_FLIP              0x66
#define VISCA_PIC_ON                0x02
#define VISCA_PIC_OFF               0x03

/******************************************** ͼ���� ********************************************/
#define VISCA_LR_REVERSE            0x61

/******************************************** ͼ������ ********************************************/
#define VISCA_MUTE                  0x75
#define VISCA_MUTE_ON               0x02
#define VISCA_MUTE_OFF              0x03
#define VISCA_MUTE_ON_OFF           0x10

/******************************************** ͼ�񶳽� ********************************************/
#define VISCA_FREEZE                0x62
#define VISCA_FREEZE_ON             0x02
#define VISCA_FREEZE_OFF            0x03

/******************************************** ��Ϣ��ʾ ********************************************/
#define VISCA_DISPLAY               0x15
#define VISCA_DISPLAY_ON            0x02
#define VISCA_DISPLAY_OFF           0x03
    
/********************************************** ���� **********************************************/
#define VISCA_STABILIZER            0x34
#define VISCA_STABILIZER_ETC        0x02
#define VISCA_STABILIZER_OFF        0x03
#define VISCA_STABILIZER_OIS        0x04

/**************************************** ɫ�����棨���Ͷȣ� **************************************/
#define VISCA_COLOR_GAIN            0x49

/********************************************** ɫ�� **********************************************/
#define VISCA_HUE                   0x4F

/********************************************** ȥ�� **********************************************/
#define VISCA_DEFOG                 0x37

/********************************************** ���� **********************************************/
#define VISCA_NR                    0x53

/********************************************* GAMMA **********************************************/
#define VISCA_GAMMA                 0x5B
#define VISCA_GAMMA_OFFSET          0x1E
    
/******************************************* �Ǳ�VISCA *********************************************/
#define VISCA_SATURATION            0xA1
#define VISCA_CONTRAST              0xA2
#define VISCA_BRIGHT                0xA4

/********************************************* Ԥ��λ *********************************************/
#define VISCA_MEMORY                0x3F
#define VISCA_MEMORY_DELETE         0x00
#define VISCA_MEMORY_SET            0x01
#define VISCA_MEMORY_CALL           0x02

/********************************************* �Ĵ��� *********************************************/
#define VISCA_REG                   0x24
#define VISCA_REG_RES               0x72

/************************************* ***** ��̬��Χ *******************************************/
#define VISCA_WDR                   0x3D
#define VISAC_WDR_LEVEL             0xD3
#define VISAC_WDR_LEVEL_INQ         0x2D

/********************************************* FLICK **********************************************/
#define VISCA_FLICK                 0x32
#define VISCA_FLICK_OFF             0x00
#define VISCA_FLICK_50HZ            0x01
#define VISCA_FLICK_60HZ            0x02

/********************************************* ��̨ ***********************************************/
#define VISCA_PT_RUN                0x01
#define VISCA_PT_ABS_POS            0x02
#define VISCA_PT_REL_POS            0x03
#define VISCA_PT_HOME               0x04
#define VISCA_PT_RESET              0x05
#define VISCA_PT_MENU               0x06
#define VISCA_PT_STATUS             0x10
#define VISCA_PT_MAX_SPEED          0x11
#define VISCA_PT_INQ_POS            0x12

#define VISCA_PAN_LEFT        1
#define VISCA_PAN_RIGHT       2
#define VISCA_PAN_STOP        3
#define VISCA_TILT_UP         1
#define VISCA_TILT_DOWN       2
#define VISCA_TILT_STOP       3
    
#define VISCA_MENU_ON_OFF           0x10
#define VISCA_MENU_ON               0x02
#define VISCA_MENU_OFF              0x03
    
/************************************* ����ָ�� 8x 01 07 xx ***************************************/
#define EXT_VERSION                 0x00    //�汾��
#define EXT_BATTERY                 0x10    //��ѯ��ص�����Ϣ
#define EXT_BATTERY_DISPLAY         0x11    //��ص�����ʾ����
#define EXT_PRESET                  0x20    //Ԥ��λ��
#define EXT_PAN_REVERSE             0x21    //ˮƽ����
#define EXT_TILT_REVERSE            0x22    //��ֱ����
#define EXT_NET                     0x23    //����
#define EXT_NET_DHCP                    0x00    //DHCP����
#define EXT_NET_IP                      0x01    //IP��ַ
#define EXT_NET_MASK                    0x02    //��������
#define EXT_NET_GATEWAY                 0x03    //����








#ifdef __cplusplus
}
#endif

#endif
/****************************************** END OF FILE *******************************************/
