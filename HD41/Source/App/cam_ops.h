/***************************************************************************************************
* FILE: cam_ops.h
*
* DESCRIPTION: --
*
* CREATED: 2018/12/3, by kevin
***************************************************************************************************/
#ifndef __CAM_OPS_H__
#define __CAM_OPS_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif
    
#define INQ_RETRY_TIMES                 1
#define CALL_PRESET_DELAY()     delay_ms(100)

#define MIN_SHUTTER                     0
#define MAX_SHUTTER                     0x15
#define MIN_IRIS                        0
#define MAX_IRIS                        0x0D
#define MIN_GAIN                        0
#define MAX_GAIN                        0x0F
#define MIN_BRIGHT                      0
#define MAX_BRIGHT                      0x1B
#define MIN_EXP_COMP                    0
#define MAX_EXP_COMP                    15

#define MIN_APERTURE                    1
#define MAX_APERTURE                    15
    
#define MIN_DEFOG                       0
#define MAX_DEFOG                       15

#define MIN_NR                          0
#define MAX_NR                          5

#define MIN_SATURATION                  0
#define MAX_SATURATION                  15

#define MIN_GAMMA                       0
#define MAX_GAMMA                       4
    
#define MIN_WDR                         1
#define MAX_WDR                         6
    
#define MIN_CONSTRAST                   0
#define MAX_CONSTRAST                   15
    

    
    
/* Ä¬ÈÏ²ÎÊý */
#define CAM_DEFAULT_DZOOM               VISCA_OFF
#define CAM_DEFAULT_FOCUS_MODE          VISCA_AUTO
#define CAM_DEFAULT_AF_MODE             
#define CAM_DEFAULT_AF_SENS             VISCA_AFS_NORMAL
    
#define CAM_DEFAULT_WB_MODE             0
#define CAM_DEFAULT_AE_MODE             0
#define CAM_DEFAULT_SLOWSHUTTER         VISCA_OFF
#define CAM_DEFAULT_EXPCOMP             VISCA_ON
#define CAM_DEFAULT_EXPCOMP_VAL         8
#define CAM_DEFAULT_BACKLIGHT           VISCA_OFF
#define CAM_DEFAULT_HIGHLIGHT           VISCA_OFF
#define CAM_DEFAULT_APERTURE            6
#define CAM_DEFAULT_SATURATION          7
#define CAM_DEFAULT_CONSTRAST           8
#define CAM_DEFAULT_GAMMA               0
#define CAM_DEFAULT_HUE                 10
#define CAM_DEFAULT_DEFOG               0
#define CAM_DEFAULT_2DNR                VISCA_OFF
#define CAM_DEFAULT_NR                  1
#define CAM_DEFAULT_FLICK               1
#define CAM_DEFAULT_WDR                 0
#define CAM_DEFAULT_FLIP                VISCA_OFF
#define CAM_DEFAULT_MIRROR              VISCA_OFF
#define CAM_DEFAULT_MUTE                YHW_MUTE_OFF
#define CAM_DEFAULT_FREEZE              VISCA_OFF
#define CAM_DEFAULT_DISPLAY             VISCA_OFF

#define REMOTE_FOCUS_SPEED      0
#define REMOTE_ZOOM_SLOW_SPEED  1
#define REMOTE_ZOOM_FAST_SPEED  5



int32_t sys_power(uint8_t arg);
int32_t inq_power_status(uint8_t *arg);
int32_t sys_menu(uint8_t arg);
int32_t inq_menu(uint8_t *arg);

int32_t cam_zoom(uint8_t arg);
int32_t cam_zoom_direct(uint16_t zoom_pos);
int32_t cam_zoom_focus_direct(uint16_t zoom_pos, uint16_t focus_pos);
int32_t inq_zoom_pos(uint16_t *arg);

int32_t cam_dzoom_mode(uint8_t arg);
int32_t cam_dzoom(uint8_t arg);
int32_t cam_dzoom_direct(uint16_t arg);
int32_t inq_dzoom_mode(uint8_t *arg);
int32_t inq_dzoom_pos(uint16_t *arg);

int32_t cam_focus_mode(uint8_t arg);
int32_t cam_focus(uint8_t arg);
int32_t cam_focus_direct(uint16_t focus_pos);
int32_t inq_focus_mode(uint8_t *arg);
int32_t inq_focus_pos(uint16_t *arg);

int32_t cam_af_sens(uint8_t arg);
int32_t inq_af_sens(uint8_t *arg);

int32_t cam_wb_mode(uint8_t arg);
int32_t inq_wb_mode(uint8_t *arg);
int32_t cam_rgain(uint8_t arg);
int32_t cam_rgain_direct(uint8_t arg);
int32_t inq_rgain(uint8_t *arg);
int32_t cam_bgain(uint8_t arg);
int32_t cam_bgain_direct(uint8_t arg);
int32_t inq_bgain(uint8_t *arg);
int32_t cam_wb_onepush_trigger(void);

int32_t cam_ae_mode(uint8_t arg);
int32_t inq_ae_mode(uint8_t *arg);
int32_t cam_shutter(uint8_t arg);
int32_t cam_shutter_direct(uint8_t arg);
int32_t inq_shutter(uint8_t *arg);

int32_t cam_iris(uint8_t arg);
int32_t cam_iris_direct(uint8_t arg);
int32_t inq_iris(uint8_t *arg);

int32_t cam_gain(uint8_t arg);
int32_t cam_gain_direct(uint8_t arg);
int32_t inq_gain(uint8_t *arg);

int32_t cam_expcomp_mode(uint8_t arg);
int32_t cam_expcomp(uint8_t arg);
int32_t cam_expcomp_direct(uint8_t arg);
int32_t inq_expcomp_mode(uint8_t *arg);
int32_t inq_expcomp(uint8_t *arg);

int32_t cam_bright(uint8_t arg);
int32_t cam_bright_direct(uint8_t arg);
int32_t inq_bright(uint8_t *arg);

int32_t cam_backlight(uint8_t arg);
int32_t inq_backlight(uint8_t* arg);

int32_t cam_highlight(uint8_t arg);
int32_t inq_highlight(uint8_t *arg);

int32_t cam_sharpness(uint8_t arg);
int32_t cam_sharpness_direct(uint8_t arg);
int32_t inq_sharpness(uint8_t *arg);

int32_t cam_gamma(uint8_t  arg);
int32_t inq_gamma(uint8_t *arg);

int32_t cam_saturation(uint8_t arg);
int32_t inq_saturation(uint8_t *arg);

int32_t cam_hue(uint8_t arg);
int32_t inq_hue(uint8_t *arg);

int32_t cam_constrast_direct(uint8_t arg);
int32_t inq_constrast(uint8_t *arg);

int32_t cam_defog(uint8_t arg1, uint8_t arg2);
int32_t inq_defog(uint8_t *arg1, uint8_t *arg2);

int32_t cam_2dnr(uint8_t arg);
int32_t inq_2dnr(uint8_t *arg);
int32_t cam_3dnr(uint8_t arg);
int32_t inq_3dnr(uint8_t *arg);

int32_t cam_flick(uint8_t arg);
int32_t inq_flick(uint8_t *arg);

int32_t cam_wdr(uint8_t arg);
int32_t inq_wdr(uint8_t *arg);

int32_t cam_mirror(uint8_t arg);
int32_t inq_mirror(uint8_t *arg);

int32_t cam_flip(uint8_t arg);
int32_t inq_flip(uint8_t *arg);

int32_t cam_mute(uint8_t arg);
int32_t inq_mute(uint8_t *arg);

int32_t cam_freeze(uint8_t arg);
int32_t inq_freeze(uint8_t *arg);

int32_t cam_datascreen(uint8_t arg);
int32_t inq_datascreen(uint8_t *arg);

int32_t cam_dhcp(uint8_t arg);
int32_t cam_net(uint8_t arg, uint32_t ip);
int32_t inq_dhcp(uint8_t *arg);
int32_t inq_net(uint8_t type, uint32_t *ip);

int32_t cam_preset(uint8_t type, uint8_t num);
int32_t inq_preset_call(uint8_t *arg);
int32_t inq_preset(uint8_t num, uint8_t *arg);

int32_t cam_reg(uint8_t reg, uint8_t val);
int32_t inq_reg(uint8_t reg, uint8_t *val);

int32_t cam_usb_mode(uint8_t arg);
int32_t inq_usb_mode(uint8_t *arg);

void pan_run(uint8_t dir, uint8_t speed, int8_t acc, int8_t dec);
void tilt_run(uint8_t dir, uint8_t speed, int8_t acc, int8_t dec);
int32_t pt_run(uint8_t pan_speed, uint8_t tilt_speed, uint8_t pan_dir, uint8_t tilt_dir);
int32_t pt_abs_run(uint8_t pan_speed, uint8_t tilt_speed, int16_t pan_pos, int16_t tilt_pos);
int32_t pt_rel_run(uint8_t pan_speed, uint8_t tilt_speed, int16_t pan_pos, int16_t tilt_pos);

int32_t ptz_home(void);
int32_t inq_pt_pos(int16_t *pan_pos, int16_t *tilt_pos);
int32_t cam_init(uint8_t arg);
int32_t ptz_reset(void);





void cam_reset_net(void);
void cam_reset_picture(void);

int32_t camera_ops_init(void);

#ifdef __cplusplus
}
#endif

#endif
/****************************************** END OF FILE *******************************************/
