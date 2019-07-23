/***************************************************************************************************
* FILE: system.h
*
* DESCRIPTION: --
*
* CREATED: 2017/11/15, by kevin xu
***************************************************************************************************/
#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include "bsp.h"

#include "ptc.h"
#include "ptz.h"
#include "cam_ops.h"
#include "menu.h"
#include "video.h"


/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define STR(s)     #s
#define VERSION(a, b, c)    STR(a) "." STR(b) "." STR(c)
#define DATE(y, m, d)       STR(y) "." STR(m) "." STR(d)


/* 版本号 */
#define VER_CODE        2009
#define MENU_VERSION    "      V2009"


#define H4BIT(x)    ((x&0xF0)>>4)
#define L4BIT(x)    (x&0x0F)

#define OFF     0
#define ON      1
#define AUTO    2
#define MANUAL  3

#define PAN_LEFT        1
#define PAN_RIGHT       2
#define PAN_STOP        3
#define TILT_UP         1
#define TILT_DOWN       2
#define TILT_STOP       3

#define PRESET_CALL     2
#define PRESET_SET      1
#define PRESET_RESET    0


/* 预置位基础参数 12字节 */
struct preset
{
    /* 基础参数 */
    int32_t pan_pos;
    int32_t tilt_pos;
    uint16_t zoom_pos;
    uint16_t focus_pos;
    
    uint32_t checksum;
};

/* 预置位完全参数 36字节 */
struct preset_complete
{
    /* 基础参数 */
    int32_t pan_pos;
    int32_t tilt_pos;
    uint16_t zoom_pos;
    uint16_t focus_pos;
    
    /* 图像参数 */
    uint8_t wb_mode;
    uint8_t rgain;
    uint8_t bgain;
    
    uint8_t ae_mode;
    uint8_t shutter;
    uint8_t slow_shutter;
    uint8_t iris;
    uint8_t bright;
    uint8_t gain;
    
    uint8_t expcomp;
    uint8_t backlight;
    uint8_t highlight;
    
    uint8_t exbright;
    uint8_t constrast;
    uint8_t saturation;
    uint8_t sharpness;
    uint8_t gamma;
    uint8_t hue;
    uint8_t nr;
    uint8_t wdr;
    
    uint32_t checksum;
};

//运行时参数
typedef struct
{
    uint32_t time;              //系统时间
    uint8_t standby;
    uint8_t menu;                //OSD菜单 0:关闭 1:主板OSD 2:机芯OSD
    uint8_t factory_pattern;    //工厂模式
    uint8_t refresh_mask;       //刷新开关
    uint32_t inq_mask;          //输入指令屏蔽
    uint8_t inq_zoom;           //zoom运行状态
    uint8_t pan_speed;          //水平方向运动速度
    uint8_t tilt_speed;         //垂直方向运动速度
    uint8_t ptz_inv;            //云台倒装
}RUNNING_INFO_ST;

//系统参数
typedef struct
{
    struct ptc_info ptc;        //协议参数
    uint8_t video_format_out1;  //输出分辨率1
    uint8_t video_format_out2;  //输出分辨率2
    uint8_t language;           //语言
    uint8_t ptz_inv_backup;     //倒装记忆
    uint8_t pan_inversion;      //水平反向
    uint8_t tilt_inversion;     //垂直反向
    uint8_t remote_speed;       //红外遥控速度1~16
    uint8_t remote_acc;         //遥控加速度1~16
    uint8_t remote_speedlimit;  //根据镜头倍数限制云台速度
    uint8_t preset_speed;       //预置位调用速度1~16
    uint8_t preset_mode;        //遥控器调用预置位模式
    uint8_t preset_save_pic;    //预置位保存图像参数
    uint8_t call_preset1;       //开机调用1号预置位
    uint8_t usb_mode;           //USB模式，1-仅3014,2-仅机芯，其它-都开启
    uint8_t aging_test;         //老化标记
    uint8_t ir_addr;            //红外遥控地址
    uint8_t show_brief;         //开机显示简要信息
    uint8_t rs485;              //RS485模式
    
    uint32_t checksum;          //校验和
}SYSTEM_INFO_ST;

//机芯参数
typedef struct
{
    uint16_t zoom_pos;          //zoom位置
    uint8_t dzoom;              //数字变焦
    uint8_t dzoom_mode;         //数字变倍模式
    uint8_t dzoom_pos;
    
    uint16_t focus_pos;         //focus位置
    uint8_t focus_mode;         //聚焦模式
    uint8_t af_mode;
    uint8_t af_sens;            //聚焦灵敏度
    
    uint8_t wb_mode;            //白平衡
    uint8_t rgain;              //RGAIN
    uint8_t bgain;              //BGAIN
    
    uint8_t ae_mode;            //AE模式
    uint8_t slow_shutter;       //慢速快门
    uint8_t shutter;            //快门
    uint8_t iris;               //光圈
    uint8_t bright;             //亮度
    uint8_t exp_gain;           //曝光增益
    uint8_t exp_comp_onoff;     //曝光补偿开关
    uint8_t exp_comp;           //曝光补偿
    uint8_t backlight;          //背光补偿
    uint8_t highlight;          //高光抑制
    
    uint8_t wdr;                //宽动态
    uint8_t wdr_level;          //宽动态等级

    uint8_t saturation;         //饱和度
    uint8_t constrast;          //对比度
    uint8_t sharpness;          //锐度
    uint8_t gamma;              //伽马
    uint8_t hue;                //色调
    uint8_t nr2d;               //2D降噪
    uint8_t nr3d;               //3D降噪
    uint8_t defog;              //去雾
    
    uint8_t flick;              //抗闪烁
    uint8_t mute;               //图像屏蔽
    uint8_t freeze;             //图像冻结
    uint8_t mirror;             //镜像
    uint8_t flip;               //图像翻转
    uint8_t datascreen;         //信息显示
    
    uint8_t reg_res;            //寄存器——分辨率
    
    uint8_t dhcp;               //DHCP开关
    uint32_t ip;                //IP地址
    uint32_t mask;              //子网掩码
    uint32_t gateway;           //网关
    
    uint32_t checksum;          //校验和
}CAMERA_INFO_ST;


extern RUNNING_INFO_ST  stRunInfo;
extern SYSTEM_INFO_ST stSysInfo;
extern CAMERA_INFO_ST stCamInfo;


#define GET_TIME()      (stRunInfo.time)

#define CUR_PTC_TYPE    (stSysInfo.ptc.type)
#define CUR_PTC_ADDR    (stSysInfo.ptc.addr)
#define PTC_BAUDRATE    (stSysInfo.ptc.baudrate)
#define CUR_PTC_ACC     (stSysInfo.ptc.speed_acc)
#define CUR_SPEED_GRADE (stSysInfo.ptc.speed_grade)
#define PTC_SPEED_LIMIT (stSysInfo.ptc.speed_limit)

#define PRESET_SAVE_PIC (stSysInfo.preset_save_pic)     //预置位保存图像参数
#define IS_RS485_ENABLE (stSysInfo.rs485)


/* 参数保存地址 */
#define SYS_INFO_SAVE_ADDR          (96)
#define CAM_INFO_SAVE_ADDR          (SYS_INFO_SAVE_ADDR + sizeof(SYSTEM_INFO_ST) + 4)
#define PRESET_SAVE_ADDR            (CAM_INFO_SAVE_ADDR + sizeof(CAMERA_INFO_ST) + 4)
#define PRESET_INV_SAVE_ADDR        (PRESET_SAVE_ADDR + sizeof(struct preset_complete) * 16)
#define PRESET_EXT_SAVE_ADDR        (PRESET_INV_SAVE_ADDR + sizeof(struct preset_complete) * 16)    //1个预置位占用12个字节，256个预置位占用2048个字节
#define PRESET_EXT_INV_SAVE_ADDR    (PRESET_EXT_SAVE_ADDR + sizeof(struct preset) * 240)


#define IS_PTZ_INV                  (stRunInfo.ptz_inv == ON)
#define IS_PAN_INV                  (stRunInfo.ptz_inv ^ stSysInfo.pan_inversion)
#define IS_TILT_INV                 (stRunInfo.ptz_inv ^ stSysInfo.tilt_inversion) 

/* 保存系统参数代码 */
#define SYS_SAVE_VIDEOFORMAT    1
#define SYS_SAVE_PAN_REVERSE    2
#define SYS_SAVE_TILT_REVERSE   3

void System_TimerInit(void);

int32_t System_GetPowerState(void);
int32_t System_CheckVirgin(void);
int32_t System_SetVirgin(void);
int32_t System_ResetPara(void);
int32_t System_ReadPara(void);
int32_t System_SavePara(void);
int32_t System_SavePartPara(uint32_t type);


//绿色power灯闪烁
void LED_Handler(void);
void LED_Blink(void);
void LED_SetStatus(int32_t onoff);

/* 计算校验和 */
uint32_t CheckSum(const uint8_t *buf, uint32_t size);

//时间间隔判断
int32_t CheckInterval(uint32_t *timer, uint32_t interval);
int32_t CheckTimeDiff(uint32_t val, uint32_t diff);

/* 预置位状态 */
void PresetStatusSetBit(uint32_t num);
void PresetStatusResetBit(uint32_t num);
void PresetStatusFillViscaBuffer(uint8_t *buffer);
void PresetCheckUsed(void);

/* 老化任务 */
void AgingTask(void);

#define  SetInqMask() do{ stRunInfo.inq_mask = stRunInfo.time; } while(0)
void InqCamInfo(void);
int32_t InqCamZoomFocus(void);
int32_t InqCamWb(void);
int32_t InqCamExposure(void);
int32_t InqCamPicture(void);
int32_t InqCamNet(void);



#endif
/****************************************** END OF FILE *******************************************/
