/***************************************************************************************************
* FILE: ccp.h
*
* DESCRIPTION: camera control protocol
*
* CREATED: 2018/11/29, by kevin
***************************************************************************************************/
#ifndef __CCP_H__
#define __CCP_H__

#include "stdint.h"

#ifdef __LIST_H_
#include "list.h"
#else
#include "llist.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
    
/* 版本信息 */
#define VER_MAJOR   1
#define VER_MINOR1  3
#define VER_MINOR2  02
#define DATE_YEAR   2019
#define DATE_MONTH  05
#define DATE_DAY    31
   
/* 返回代码 */
#define CCP_OK      0
#define CCP_ERROR   1


#ifndef H4BIT
#define H4BIT(x)    ((x&0xF0)>>4)
#endif
#ifndef L4BIT
#define L4BIT(x)    (x&0x0F)
#endif
    

enum ccp_type
{
    CCP_AUTO,       //自动识别VISCA PELCO_D PELCO_P
    CCP_VISCA,      //只有VISCA控制指令，不包括回码
    CCP_PELCO_D,
    CCP_PELCO_P,
    CCP_VISCA_INQ,  //VISCA回码
    CCP_VISCA_ALL,  //VISCA控制指令和回码
    CCP_BUTT,
};

enum baudrate
{
    BAUD_2400,
    BAUD_4800,
    BAUD_9600,
    BAUD_19200,
    BAUD_38400,
    BAUD_115200,
    BUAD_BUTT,        
};


/* 协议栈会根据接收到的指令回调以下函数，按需实现 */
struct camera_operations
{
    /* 系统控制 */
    int32_t (*ctl_power)(uint8_t);
    int32_t (*inq_power)(uint8_t*);
    int32_t (*ctl_init)(uint8_t);
    
    /* 变倍 */
    int32_t (*ctl_zoom)(uint8_t);
    int32_t (*ctl_zoom_direct)(uint16_t);
    int32_t (*ctl_zoom_focus_direct)(uint16_t, uint16_t);
    int32_t (*inq_zoom)(uint16_t*);
    
    /* 数字变倍 */
    int32_t (*ctl_dzoom_mode)(uint8_t);
    int32_t (*ctl_dzoom)(uint8_t);
    int32_t (*ctl_dzoom_direct)(uint16_t);
    int32_t (*inq_dzoom_mode)(uint8_t*);
    int32_t (*inq_dzoom)(uint16_t*);
    
    /* 聚焦 */
    int32_t (*ctl_focus_mode)(uint8_t);
    int32_t (*ctl_focus)(uint8_t);
    int32_t (*ctl_focus_direct)(uint16_t);
    int32_t (*ctl_af_mode)(uint8_t);
    int32_t (*ctl_af_sensitivity)(uint8_t);
    int32_t (*inq_focus_mode)(uint8_t*);
    int32_t (*inq_focus)(uint16_t*);
    int32_t (*inq_af_mode)(uint8_t*);
    int32_t (*inq_af_sensitivity)(uint8_t*);
    
    /* 白平衡 */
    int32_t (*ctl_wb_mode)(uint8_t);
    int32_t (*ctl_rgain)(uint8_t);
    int32_t (*ctl_rgain_direct)(uint8_t);
    int32_t (*ctl_bgain)(uint8_t);
    int32_t (*ctl_bgain_direct)(uint8_t);
    int32_t (*inq_wb_mode)(uint8_t*);
    int32_t (*inq_rgain)(uint8_t*);
    int32_t (*inq_bgain)(uint8_t*);
    
    /* 曝光 */
    int32_t (*ctl_exp_mode)(uint8_t);
    int32_t (*ctl_exp_shutter)(uint8_t);
    int32_t (*ctl_exp_shutter_direct)(uint8_t);
    int32_t (*ctl_exp_slow_shutter)(uint8_t);
    int32_t (*ctl_exp_iris)(uint8_t);
    int32_t (*ctl_exp_iris_direct)(uint8_t);
    int32_t (*ctl_exp_gain)(uint8_t);
    int32_t (*ctl_exp_gain_direct)(uint8_t);
    int32_t (*ctl_exp_bright)(uint8_t);
    int32_t (*ctl_exp_bright_direct)(uint8_t);
    int32_t (*inq_exp_mode)(uint8_t*);
    int32_t (*inq_exp_shutter)(uint8_t*);
    int32_t (*inq_exp_slow_shutter)(uint8_t*);
    int32_t (*inq_exp_iris)(uint8_t*);
    int32_t (*inq_exp_gain)(uint8_t*);
    int32_t (*inq_exp_bright)(uint8_t*);
    
    /* 曝光补偿 */
    int32_t (*ctl_exp_comp_onoff)(uint8_t);
    int32_t (*ctl_exp_comp)(uint8_t);
    int32_t (*ctl_exp_comp_direct)(uint8_t);
    int32_t (*inq_exp_comp_onoff)(uint8_t*);
    int32_t (*inq_exp_comp)(uint8_t*);
    
    /* 背光补偿 */
    int32_t (*ctl_backlight)(uint8_t);
    int32_t (*inq_backlight)(uint8_t*);
    
    /* 锐度 */
    int32_t (*ctl_aperture)(uint8_t);
    int32_t (*ctl_aperture_direct)(uint8_t);
    int32_t (*inq_aperture)(uint8_t*);
    
    /* 伽马 */
    int32_t (*ctl_gamma)(uint8_t);
    int32_t (*ctl_gamma_offset)(uint8_t, uint8_t);
    int32_t (*inq_gamma)(uint8_t*);
    int32_t (*inq_gamma_offset)(uint8_t*, uint8_t*);
    
    /* 色彩 */
    int32_t (*ctl_color_gain)(uint8_t);
    int32_t (*inq_color_gain)(uint8_t*);
   
    /* 色调 */
    int32_t (*ctl_hue)(uint8_t);
    int32_t (*inq_hue)(uint8_t*);
    
    /* 去雾 */
    int32_t (*ctl_defog)(uint8_t, uint8_t);
    int32_t (*inq_defog)(uint8_t*, uint8_t*);
    
    /* 降噪 */
    int32_t (*ctl_nr)(uint8_t);
    int32_t (*inq_nr)(uint8_t*);
    
    /* 频闪 */
    int32_t (*ctl_flick)(uint8_t);
    int32_t (*inq_flick)(uint8_t*);
    
    /* 宽动态范围 */
    int32_t (*ctl_wdr)(uint8_t);
    int32_t (*inq_wdr)(uint8_t*);
    
    /* 翻转 */
    int32_t (*ctl_flip)(uint8_t);
    int32_t (*inq_flip)(uint8_t*);
    
    /* 镜像 */
    int32_t (*ctl_mirror)(uint8_t);
    int32_t (*inq_mirror)(uint8_t*);
    
    /* 屏蔽 */
    int32_t (*ctl_mute)(uint8_t);
    int32_t (*inq_mute)(uint8_t*);
    
    /* 冻结 */
    int32_t (*ctl_freeze)(uint8_t);
    int32_t (*inq_freeze)(uint8_t*);
    
    /* 信息显示 */
    int32_t (*ctl_display)(uint8_t);
    int32_t (*inq_display)(uint8_t*);
    
    /* 防抖 */
    int32_t (*ctl_stabilizer)(uint8_t);
    int32_t (*inq_stabilizer)(uint8_t*);
    
    /* 饱和度 */
    int32_t (*ctl_saturation)(uint8_t);
    int32_t (*inq_saturation)(uint8_t*);
    
    /* 对比度 */
    int32_t (*ctl_contrast)(uint8_t);
    int32_t (*inq_contrast)(uint8_t*);
    
    /* 亮度 */
    int32_t (*ctl_bright)(uint8_t);
    int32_t (*inq_bright)(uint8_t*);
    
    /* 预置位，内存 */
    int32_t (*ctl_preset)(uint8_t, uint8_t);
    int32_t (*inq_preset_call)(uint8_t*);
    int32_t (*inq_preset)(uint8_t, uint8_t*);
    int32_t (*inq_preset_ext)(uint8_t **, uint8_t*);
    
    /* 寄存器 */
    int32_t (*ctl_reg)(uint8_t, uint8_t);
    int32_t (*inq_reg)(uint8_t, uint8_t*);
    
    /* 菜单 */
    int32_t (*ctl_menu)(uint8_t);
    int32_t (*ctl_menu_enter)(uint8_t);
    int32_t (*inq_menu)(uint8_t*);
    
    /* 云台上下左右、绝对位置、相对位置、回位 */
    int32_t (*pt_run)(uint8_t, uint8_t, uint8_t, uint8_t);
    int32_t (*pt_abs_run)(uint8_t, uint8_t, int16_t, int16_t);
    int32_t (*pt_rel_run)(uint8_t, uint8_t, int16_t, int16_t);
    int32_t (*ptz_home)(void);
    int32_t (*inq_pt_pos)(int16_t*, int16_t*);
    
    /* 云台重置 */
    int32_t (*ptz_reset)(void);
    
    /* 版本信息 */
    int32_t (*inq_version)(uint16_t*);
    
    /* 电池信息 */
    int32_t (*inq_battery)(uint8_t*);
    int32_t (*ctl_battery_display)(uint8_t);
    int32_t (*inq_battery_display)(uint8_t*);

    /* 云台水平垂直反向控制 */
    int32_t (*pan_reverse)(uint8_t);
    int32_t (*tilt_reverse)(uint8_t);
    int32_t (*inq_pan_reverse)(uint8_t*);
    int32_t (*inq_tilt_reverse)(uint8_t*);
    
    /* 网络 */
    int32_t (*net_dhcp)(uint8_t);
    int32_t (*net_ip)(uint8_t, uint32_t);
    int32_t (*inq_net_dhcp)(uint8_t*);
    int32_t (*inq_net_ip)(uint8_t, uint32_t*);

        
};


/* 公共操作函数 */
struct ccp_ops
{
    uint8_t cmd_debug;
    uint8_t direct_link_cam2in1;
    void (*delay_ms)(uint32_t); //必须实现
    void (*action_hint)(void);
    int32_t (*get_power_status)(void);
    int32_t (*get_menu_status)(void);
    int32_t (*get_pan_tilt_status)(void);
    
    int32_t (*cmd_debug_show)(uint8_t *buffer, int32_t len);
};

/* 通道描述 */
struct ccp_chn
{
    struct list_head list;
    int8_t path_id;         //通道ID
    uint8_t type;           //协议类型
    uint8_t addr;           //地址
    uint8_t speed_grade;    //云台速度细分等级
    struct camera_operations *camera_ops;   //回调函数
    int32_t (*clear_packet_buffer)(void);
    int32_t (*get_packet)(uint8_t *buf);   //读取一个数据包
    int32_t (*add_packet)(uint8_t *buf, uint8_t size);  //添加一个数据包
    int32_t (*send_packet)(const uint8_t *buf, uint16_t size);  //发送数据包
};

/* 获取版本号 */
const char* CCP_GetVersion(void);
/* 协议栈初始化，注册基础操作接口 */
int32_t Ccp_Init(struct ccp_ops *ops);

/* 添加数据通道 */
int32_t Ccp_AddChannel(struct ccp_chn *ccp);
int32_t Ccp_RemoveChannel(int8_t id);
int32_t Ccp_RemoveAllChannel(void);

/* 添加与机芯连接的数据通道，可选缓存通道，
使用缓存通道可以避免执行查询任务时丢弃机芯发送的控制指令 */
int32_t Ccp_AddCamChannel(struct ccp_chn *cam, struct ccp_chn *cache);
int32_t Ccp_AddCam2Channel(struct ccp_chn *cam, struct ccp_chn *cache);

/* 协议栈事件循环 */
int32_t Ccp_ProcessHandler(void);



/* 机芯控制API */
int32_t visca_select_camera(int32_t n); //1~2
int32_t visca_set_8bits_value(uint8_t item, uint8_t value, uint8_t retry);
int32_t visca_set_16bits_value(uint8_t item, uint16_t value, uint8_t retry);
int32_t visca_set_reg_value(uint8_t reg, uint8_t value, uint8_t retry);
int32_t visca_set_memory(uint8_t type, uint8_t value, uint8_t retry);

int32_t visca_inq_8bits_value(uint8_t item, uint8_t *value, uint8_t retry);
int32_t visca_inq_16bits_value(uint8_t item, uint16_t *value, uint8_t retry);
int32_t visca_inq_reg_value(uint8_t reg, uint8_t *value, uint8_t retry);

int32_t visca_set_pan_tilt(uint8_t pan_sp, uint8_t tilt_sp, uint8_t pan_dir, uint8_t tilt_dir, uint8_t retry);
int32_t visca_set_pan_tilt_abs(uint8_t sp, int16_t pan_pos, int16_t tilt_pos, uint8_t retry);
int32_t visca_set_pan_tilt_rel(uint8_t sp, int16_t pan_pos, int16_t tilt_pos, uint8_t retry);
int32_t visca_set_pan_tilt_home(uint8_t retry);
int32_t visca_set_pan_tilt_reset(uint8_t retry);

/* OSD */
int32_t visca_title_cmd(uint8_t line, uint8_t cmd, uint8_t retry);
int32_t visca_title_set(uint8_t line, uint8_t x, uint8_t color, uint8_t blink, uint8_t retry);
int32_t visca_title_show(uint8_t line, uint8_t *str, uint8_t retry);

/* 机芯参数保存，127号预置位 */
void visca_custom_reset(void);
void visca_custom_set(void);




/******* 以下指令仅贝莱特机芯和熠和微机芯可用 ********/

/* 机芯菜单 */
void cam_menu_switch(void);
void cam_menu_up(void);
void cam_menu_down(void);
void cam_menu_left(void);
void cam_menu_right(void);

/* 网络参数 */
int32_t visca_set_net(uint8_t item, uint32_t value, uint8_t retry);
int32_t visca_inq_net(uint8_t item, uint32_t *value, uint8_t retry);

/* 特殊指令 */
int32_t visca_custom_cmd(uint8_t *cmd, uint8_t size, uint8_t retry);
int32_t visca_inq(uint8_t *buffer_in, uint8_t size_in, uint8_t *buffer_out, uint8_t size_out, uint8_t retry);


#ifdef __cplusplus
}
#endif

#endif
/****************************************** END OF FILE *******************************************/
