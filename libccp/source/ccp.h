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
    
    
/* �汾��Ϣ */
#define VER_MAJOR   1
#define VER_MINOR1  3
#define VER_MINOR2  02
#define DATE_YEAR   2019
#define DATE_MONTH  05
#define DATE_DAY    31
   
/* ���ش��� */
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
    CCP_AUTO,       //�Զ�ʶ��VISCA PELCO_D PELCO_P
    CCP_VISCA,      //ֻ��VISCA����ָ�����������
    CCP_PELCO_D,
    CCP_PELCO_P,
    CCP_VISCA_INQ,  //VISCA����
    CCP_VISCA_ALL,  //VISCA����ָ��ͻ���
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


/* Э��ջ����ݽ��յ���ָ��ص����º���������ʵ�� */
struct camera_operations
{
    /* ϵͳ���� */
    int32_t (*ctl_power)(uint8_t);
    int32_t (*inq_power)(uint8_t*);
    int32_t (*ctl_init)(uint8_t);
    
    /* �䱶 */
    int32_t (*ctl_zoom)(uint8_t);
    int32_t (*ctl_zoom_direct)(uint16_t);
    int32_t (*ctl_zoom_focus_direct)(uint16_t, uint16_t);
    int32_t (*inq_zoom)(uint16_t*);
    
    /* ���ֱ䱶 */
    int32_t (*ctl_dzoom_mode)(uint8_t);
    int32_t (*ctl_dzoom)(uint8_t);
    int32_t (*ctl_dzoom_direct)(uint16_t);
    int32_t (*inq_dzoom_mode)(uint8_t*);
    int32_t (*inq_dzoom)(uint16_t*);
    
    /* �۽� */
    int32_t (*ctl_focus_mode)(uint8_t);
    int32_t (*ctl_focus)(uint8_t);
    int32_t (*ctl_focus_direct)(uint16_t);
    int32_t (*ctl_af_mode)(uint8_t);
    int32_t (*ctl_af_sensitivity)(uint8_t);
    int32_t (*inq_focus_mode)(uint8_t*);
    int32_t (*inq_focus)(uint16_t*);
    int32_t (*inq_af_mode)(uint8_t*);
    int32_t (*inq_af_sensitivity)(uint8_t*);
    
    /* ��ƽ�� */
    int32_t (*ctl_wb_mode)(uint8_t);
    int32_t (*ctl_rgain)(uint8_t);
    int32_t (*ctl_rgain_direct)(uint8_t);
    int32_t (*ctl_bgain)(uint8_t);
    int32_t (*ctl_bgain_direct)(uint8_t);
    int32_t (*inq_wb_mode)(uint8_t*);
    int32_t (*inq_rgain)(uint8_t*);
    int32_t (*inq_bgain)(uint8_t*);
    
    /* �ع� */
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
    
    /* �عⲹ�� */
    int32_t (*ctl_exp_comp_onoff)(uint8_t);
    int32_t (*ctl_exp_comp)(uint8_t);
    int32_t (*ctl_exp_comp_direct)(uint8_t);
    int32_t (*inq_exp_comp_onoff)(uint8_t*);
    int32_t (*inq_exp_comp)(uint8_t*);
    
    /* ���ⲹ�� */
    int32_t (*ctl_backlight)(uint8_t);
    int32_t (*inq_backlight)(uint8_t*);
    
    /* ��� */
    int32_t (*ctl_aperture)(uint8_t);
    int32_t (*ctl_aperture_direct)(uint8_t);
    int32_t (*inq_aperture)(uint8_t*);
    
    /* ٤�� */
    int32_t (*ctl_gamma)(uint8_t);
    int32_t (*ctl_gamma_offset)(uint8_t, uint8_t);
    int32_t (*inq_gamma)(uint8_t*);
    int32_t (*inq_gamma_offset)(uint8_t*, uint8_t*);
    
    /* ɫ�� */
    int32_t (*ctl_color_gain)(uint8_t);
    int32_t (*inq_color_gain)(uint8_t*);
   
    /* ɫ�� */
    int32_t (*ctl_hue)(uint8_t);
    int32_t (*inq_hue)(uint8_t*);
    
    /* ȥ�� */
    int32_t (*ctl_defog)(uint8_t, uint8_t);
    int32_t (*inq_defog)(uint8_t*, uint8_t*);
    
    /* ���� */
    int32_t (*ctl_nr)(uint8_t);
    int32_t (*inq_nr)(uint8_t*);
    
    /* Ƶ�� */
    int32_t (*ctl_flick)(uint8_t);
    int32_t (*inq_flick)(uint8_t*);
    
    /* ��̬��Χ */
    int32_t (*ctl_wdr)(uint8_t);
    int32_t (*inq_wdr)(uint8_t*);
    
    /* ��ת */
    int32_t (*ctl_flip)(uint8_t);
    int32_t (*inq_flip)(uint8_t*);
    
    /* ���� */
    int32_t (*ctl_mirror)(uint8_t);
    int32_t (*inq_mirror)(uint8_t*);
    
    /* ���� */
    int32_t (*ctl_mute)(uint8_t);
    int32_t (*inq_mute)(uint8_t*);
    
    /* ���� */
    int32_t (*ctl_freeze)(uint8_t);
    int32_t (*inq_freeze)(uint8_t*);
    
    /* ��Ϣ��ʾ */
    int32_t (*ctl_display)(uint8_t);
    int32_t (*inq_display)(uint8_t*);
    
    /* ���� */
    int32_t (*ctl_stabilizer)(uint8_t);
    int32_t (*inq_stabilizer)(uint8_t*);
    
    /* ���Ͷ� */
    int32_t (*ctl_saturation)(uint8_t);
    int32_t (*inq_saturation)(uint8_t*);
    
    /* �Աȶ� */
    int32_t (*ctl_contrast)(uint8_t);
    int32_t (*inq_contrast)(uint8_t*);
    
    /* ���� */
    int32_t (*ctl_bright)(uint8_t);
    int32_t (*inq_bright)(uint8_t*);
    
    /* Ԥ��λ���ڴ� */
    int32_t (*ctl_preset)(uint8_t, uint8_t);
    int32_t (*inq_preset_call)(uint8_t*);
    int32_t (*inq_preset)(uint8_t, uint8_t*);
    int32_t (*inq_preset_ext)(uint8_t **, uint8_t*);
    
    /* �Ĵ��� */
    int32_t (*ctl_reg)(uint8_t, uint8_t);
    int32_t (*inq_reg)(uint8_t, uint8_t*);
    
    /* �˵� */
    int32_t (*ctl_menu)(uint8_t);
    int32_t (*ctl_menu_enter)(uint8_t);
    int32_t (*inq_menu)(uint8_t*);
    
    /* ��̨�������ҡ�����λ�á����λ�á���λ */
    int32_t (*pt_run)(uint8_t, uint8_t, uint8_t, uint8_t);
    int32_t (*pt_abs_run)(uint8_t, uint8_t, int16_t, int16_t);
    int32_t (*pt_rel_run)(uint8_t, uint8_t, int16_t, int16_t);
    int32_t (*ptz_home)(void);
    int32_t (*inq_pt_pos)(int16_t*, int16_t*);
    
    /* ��̨���� */
    int32_t (*ptz_reset)(void);
    
    /* �汾��Ϣ */
    int32_t (*inq_version)(uint16_t*);
    
    /* �����Ϣ */
    int32_t (*inq_battery)(uint8_t*);
    int32_t (*ctl_battery_display)(uint8_t);
    int32_t (*inq_battery_display)(uint8_t*);

    /* ��̨ˮƽ��ֱ������� */
    int32_t (*pan_reverse)(uint8_t);
    int32_t (*tilt_reverse)(uint8_t);
    int32_t (*inq_pan_reverse)(uint8_t*);
    int32_t (*inq_tilt_reverse)(uint8_t*);
    
    /* ���� */
    int32_t (*net_dhcp)(uint8_t);
    int32_t (*net_ip)(uint8_t, uint32_t);
    int32_t (*inq_net_dhcp)(uint8_t*);
    int32_t (*inq_net_ip)(uint8_t, uint32_t*);

        
};


/* ������������ */
struct ccp_ops
{
    uint8_t cmd_debug;
    uint8_t direct_link_cam2in1;
    void (*delay_ms)(uint32_t); //����ʵ��
    void (*action_hint)(void);
    int32_t (*get_power_status)(void);
    int32_t (*get_menu_status)(void);
    int32_t (*get_pan_tilt_status)(void);
    
    int32_t (*cmd_debug_show)(uint8_t *buffer, int32_t len);
};

/* ͨ������ */
struct ccp_chn
{
    struct list_head list;
    int8_t path_id;         //ͨ��ID
    uint8_t type;           //Э������
    uint8_t addr;           //��ַ
    uint8_t speed_grade;    //��̨�ٶ�ϸ�ֵȼ�
    struct camera_operations *camera_ops;   //�ص�����
    int32_t (*clear_packet_buffer)(void);
    int32_t (*get_packet)(uint8_t *buf);   //��ȡһ�����ݰ�
    int32_t (*add_packet)(uint8_t *buf, uint8_t size);  //���һ�����ݰ�
    int32_t (*send_packet)(const uint8_t *buf, uint16_t size);  //�������ݰ�
};

/* ��ȡ�汾�� */
const char* CCP_GetVersion(void);
/* Э��ջ��ʼ����ע����������ӿ� */
int32_t Ccp_Init(struct ccp_ops *ops);

/* �������ͨ�� */
int32_t Ccp_AddChannel(struct ccp_chn *ccp);
int32_t Ccp_RemoveChannel(int8_t id);
int32_t Ccp_RemoveAllChannel(void);

/* ������о���ӵ�����ͨ������ѡ����ͨ����
ʹ�û���ͨ�����Ա���ִ�в�ѯ����ʱ������о���͵Ŀ���ָ�� */
int32_t Ccp_AddCamChannel(struct ccp_chn *cam, struct ccp_chn *cache);
int32_t Ccp_AddCam2Channel(struct ccp_chn *cam, struct ccp_chn *cache);

/* Э��ջ�¼�ѭ�� */
int32_t Ccp_ProcessHandler(void);



/* ��о����API */
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

/* ��о�������棬127��Ԥ��λ */
void visca_custom_reset(void);
void visca_custom_set(void);




/******* ����ָ��������ػ�о���ں�΢��о���� ********/

/* ��о�˵� */
void cam_menu_switch(void);
void cam_menu_up(void);
void cam_menu_down(void);
void cam_menu_left(void);
void cam_menu_right(void);

/* ������� */
int32_t visca_set_net(uint8_t item, uint32_t value, uint8_t retry);
int32_t visca_inq_net(uint8_t item, uint32_t *value, uint8_t retry);

/* ����ָ�� */
int32_t visca_custom_cmd(uint8_t *cmd, uint8_t size, uint8_t retry);
int32_t visca_inq(uint8_t *buffer_in, uint8_t size_in, uint8_t *buffer_out, uint8_t size_out, uint8_t retry);


#ifdef __cplusplus
}
#endif

#endif
/****************************************** END OF FILE *******************************************/
