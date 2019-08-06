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


/* �汾�� */
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


/* Ԥ��λ�������� 12�ֽ� */
struct preset
{
    /* �������� */
    int32_t pan_pos;
    int32_t tilt_pos;
    uint16_t zoom_pos;
    uint16_t focus_pos;
    
    uint32_t checksum;
};

/* Ԥ��λ��ȫ���� 36�ֽ� */
struct preset_complete
{
    /* �������� */
    int32_t pan_pos;
    int32_t tilt_pos;
    uint16_t zoom_pos;
    uint16_t focus_pos;
    
    /* ͼ����� */
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

//����ʱ����
typedef struct
{
    uint32_t time;              //ϵͳʱ��
    uint8_t standby;
    uint8_t menu;                //OSD�˵� 0:�ر� 1:����OSD 2:��оOSD
    uint8_t factory_pattern;    //����ģʽ
    uint8_t refresh_mask;       //ˢ�¿���
    uint32_t inq_mask;          //����ָ������
    uint8_t inq_zoom;           //zoom����״̬
    uint8_t pan_speed;          //ˮƽ�����˶��ٶ�
    uint8_t tilt_speed;         //��ֱ�����˶��ٶ�
    uint8_t ptz_inv;            //��̨��װ
}RUNNING_INFO_ST;

//ϵͳ����
typedef struct
{
    struct ptc_info ptc;        //Э�����
    uint8_t video_format_out1;  //����ֱ���1
    uint8_t video_format_out2;  //����ֱ���2
    uint8_t language;           //����
    uint8_t ptz_inv_backup;     //��װ����
    uint8_t pan_inversion;      //ˮƽ����
    uint8_t tilt_inversion;     //��ֱ����
    uint8_t remote_speed;       //����ң���ٶ�1~16
    uint8_t remote_acc;         //ң�ؼ��ٶ�1~16
    uint8_t remote_speedlimit;  //���ݾ�ͷ����������̨�ٶ�
    uint8_t preset_speed;       //Ԥ��λ�����ٶ�1~16
    uint8_t preset_mode;        //ң��������Ԥ��λģʽ
    uint8_t preset_save_pic;    //Ԥ��λ����ͼ�����
    uint8_t call_preset1;       //��������1��Ԥ��λ
    uint8_t usb_mode;           //USBģʽ��1-��3014,2-����о������-������
    uint8_t aging_test;         //�ϻ����
    uint8_t ir_addr;            //����ң�ص�ַ
    uint8_t show_brief;         //������ʾ��Ҫ��Ϣ
    uint8_t rs485;              //RS485ģʽ
    
    uint32_t checksum;          //У���
}SYSTEM_INFO_ST;

//��о����
typedef struct
{
    uint16_t zoom_pos;          //zoomλ��
    uint8_t dzoom;              //���ֱ佹
    uint8_t dzoom_mode;         //���ֱ䱶ģʽ
    uint8_t dzoom_pos;
    
    uint16_t focus_pos;         //focusλ��
    uint8_t focus_mode;         //�۽�ģʽ
    uint8_t af_mode;
    uint8_t af_sens;            //�۽�������
    
    uint8_t wb_mode;            //��ƽ��
    uint8_t rgain;              //RGAIN
    uint8_t bgain;              //BGAIN
    
    uint8_t ae_mode;            //AEģʽ
    uint8_t slow_shutter;       //���ٿ���
    uint8_t shutter;            //����
    uint8_t iris;               //��Ȧ
    uint8_t bright;             //����
    uint8_t exp_gain;           //�ع�����
    uint8_t exp_comp_onoff;     //�عⲹ������
    uint8_t exp_comp;           //�عⲹ��
    uint8_t backlight;          //���ⲹ��
    uint8_t highlight;          //�߹�����
    
    uint8_t wdr;                //��̬
    uint8_t wdr_level;          //��̬�ȼ�

    uint8_t saturation;         //���Ͷ�
    uint8_t constrast;          //�Աȶ�
    uint8_t sharpness;          //���
    uint8_t gamma;              //٤��
    uint8_t hue;                //ɫ��
    uint8_t nr2d;               //2D����
    uint8_t nr3d;               //3D����
    uint8_t defog;              //ȥ��
    
    uint8_t flick;              //����˸
    uint8_t mute;               //ͼ������
    uint8_t freeze;             //ͼ�񶳽�
    uint8_t mirror;             //����
    uint8_t flip;               //ͼ��ת
    uint8_t datascreen;         //��Ϣ��ʾ
    
    uint8_t reg_res;            //�Ĵ��������ֱ���
    
    uint8_t dhcp;               //DHCP����
    uint32_t ip;                //IP��ַ
    uint32_t mask;              //��������
    uint32_t gateway;           //����
    
    uint32_t checksum;          //У���
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

#define PRESET_SAVE_PIC (stSysInfo.preset_save_pic)     //Ԥ��λ����ͼ�����
#define IS_RS485_ENABLE (stSysInfo.rs485)


/* ���������ַ */
#define SYS_INFO_SAVE_ADDR          (96)
#define CAM_INFO_SAVE_ADDR          (SYS_INFO_SAVE_ADDR + sizeof(SYSTEM_INFO_ST) + 4)
#define PRESET_SAVE_ADDR            (CAM_INFO_SAVE_ADDR + sizeof(CAMERA_INFO_ST) + 4)
#define PRESET_INV_SAVE_ADDR        (PRESET_SAVE_ADDR + sizeof(struct preset_complete) * 16)
#define PRESET_EXT_SAVE_ADDR        (PRESET_INV_SAVE_ADDR + sizeof(struct preset_complete) * 16)    //1��Ԥ��λռ��12���ֽڣ�256��Ԥ��λռ��2048���ֽ�
#define PRESET_EXT_INV_SAVE_ADDR    (PRESET_EXT_SAVE_ADDR + sizeof(struct preset) * 240)


#define IS_PTZ_INV                  (stRunInfo.ptz_inv == ON)
#define IS_PAN_INV                  (stRunInfo.ptz_inv ^ stSysInfo.pan_inversion)
#define IS_TILT_INV                 (stRunInfo.ptz_inv ^ stSysInfo.tilt_inversion) 

/* ����ϵͳ�������� */
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


//��ɫpower����˸
void LED_Handler(void);
void LED_Blink(void);
void LED_SetStatus(int32_t onoff);

/* ����У��� */
uint32_t CheckSum(const uint8_t *buf, uint32_t size);

//ʱ�����ж�
int32_t CheckInterval(uint32_t *timer, uint32_t interval);
int32_t CheckTimeDiff(uint32_t val, uint32_t diff);

/* Ԥ��λ״̬ */
void PresetStatusSetBit(uint32_t num);
void PresetStatusResetBit(uint32_t num);
void PresetStatusFillViscaBuffer(uint8_t *buffer);
void PresetCheckUsed(void);

/* �ϻ����� */
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
