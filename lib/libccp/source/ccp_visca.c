/***************************************************************************************************
* FILE: ccp_visca.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/11/29, by kevin
***************************************************************************************************/
#include "string.h"
#include "ccp_visca.h"
#include "visca.h"

#define RET_ADDR(addr)  (0x80 + ((addr)<<4))
#define GET4BIT(val, n)  (((val) >> (n)) & 0x0F)
#define ARR_GET_32BIT_DATA(arr, n)  ((arr[n] << 28) | (arr[n+1] << 24) | (arr[n+2] << 20) | (arr[n+3] << 16) | \
                                     (arr[n+4] << 12) | (arr[n+5] << 8) | (arr[n+6] << 4) | arr[n+7])
#define ARR_GET_16BIT_DATA(arr, n)  ((arr[n] << 12) | (arr[n+1] << 8) | (arr[n+2] << 4) | arr[n+3])
#define ARR_GET_8BIT_DATA(arr, n)   ((arr[n] << 4) | arr[n+1])

extern struct ccp_ops *ccp_comm_ops;        //公共操作函数结构体
extern struct ccp_chn *ccp_cam_p, *ccp_cache_p, *ccp_in1_p, *ccp_out1_p;        //通道描述结构体

/***************************************************************************************************
* Description:  发送回码
***************************************************************************************************/
void Ccp_ViscaSendRetCmd(struct ccp_chn *chn, uint8_t type)
{
    static uint8_t visca_ack[] = {0x90, 0x41, 0xff};
    static uint8_t visca_cmp[] = {0x90, 0x51, 0xff};
    static uint8_t visca_error[] = {0x90, 0x60, 0x02, 0xff};
    switch(type)
    {
        case VISCA_ACK:
        visca_ack[0] = RET_ADDR(chn->addr);
        chn->send_packet(visca_ack, 3);         
        break;
        
        case VISCA_COMPLETE:
        visca_cmp[0] = RET_ADDR(chn->addr);
        chn->send_packet(visca_cmp, 3);
        break;
        
        case VISCA_ERROR_LEN:
        visca_error[0] = RET_ADDR(chn->addr);
        visca_error[2] = VISCA_ERROR_LEN;
        chn->send_packet(visca_error, 4);
        break;
        
        case VISCA_ERROR_SYNTAX:
        visca_error[0] = RET_ADDR(chn->addr);
        visca_error[2] = VISCA_ERROR_SYNTAX;
        chn->send_packet(visca_error, 4);
        break;
        
        case VISVA_ERROR_BUF_FULL:
        visca_error[0] = RET_ADDR(chn->addr);
        visca_error[2] = VISVA_ERROR_BUF_FULL;
        chn->send_packet(visca_error, 4);
        break;
        
        case VISCA_ERROR_CMD_CANCEL:
        visca_error[0] = RET_ADDR(chn->addr);
        visca_error[2] = VISCA_ERROR_CMD_CANCEL;
        chn->send_packet(visca_error, 4);
        break;
        
        case VISCA_ERROR_NO_SOCKET:
        visca_error[0] = RET_ADDR(chn->addr);
        visca_error[2] = VISCA_ERROR_NO_SOCKET;
        chn->send_packet(visca_error, 4);
        break;
        
        default:
        break;
    }
}

/***************************************************************************************************
* Description:  控制指令解析，8x 01 xx
***************************************************************************************************/
static inline void _Visca_ControlCmdParser(struct ccp_chn *chn, uint8_t *cmd_buf, uint8_t length)
{
    if(chn->camera_ops == NULL) goto error;
    
    /* 待机指令处理 */
    if(ccp_comm_ops->get_power_status != NULL)
    {
        if(ccp_comm_ops->get_power_status() == 0)         //待机状态
        {
            if(cmd_buf[2] == 0x04 && cmd_buf[3] == VISCA_POWER && cmd_buf[5] == 0xFF)
            {
                if(chn->camera_ops->ctl_power == NULL) goto error;  //无注册函数会返回错误
                if(chn->camera_ops->ctl_power(cmd_buf[4]) != CCP_OK) goto error;
                goto complete;
            }
            else
            {
                goto error;
            }
        }
    }
    
    /* 开机指令处理 */
    switch(cmd_buf[2])
    {
        /* 图像指令 8x 01 04 xx*/
        case 0x04:
        switch(cmd_buf[3])
        {
            /* 电源
            8x 01 04 00 02/03 FF*/
            case VISCA_POWER:
            {
                if(chn->camera_ops->ctl_power == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_power(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 初始化
            8x 01 04 19 01/02 FF*/
            case VISCA_CAM_INIT:
            {
                if(chn->camera_ops->ctl_init == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_init(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* ZOOM
            8x 01 04 07 00/02/03/2x/3x FF*/
            case VISCA_ZOOM:
            {
                if(chn->camera_ops->ctl_zoom == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_zoom(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* ZOOM DIRECT
            8x 01 04 47 0p 0q 0r 0s FF 
            8x 01 04 47 0p 0q 0r 0s 0t 0u 0v 0w FF */
            case VISCA_ZOOM_DIRECT:
            {
                uint16_t zoom_pos, focus_pos;
                if(cmd_buf[8] == 0xFF)
                {
                    if(chn->camera_ops->ctl_zoom_direct == NULL) goto error;
                    zoom_pos = ARR_GET_16BIT_DATA(cmd_buf, 4);
                    if(chn->camera_ops->ctl_zoom_direct(zoom_pos) != CCP_OK) goto error;
                }
                else if(cmd_buf[12] == 0xFF)
                {
                    if(chn->camera_ops->ctl_zoom_focus_direct == NULL) goto error;
                    zoom_pos = ARR_GET_16BIT_DATA(cmd_buf, 4);
                    focus_pos = ARR_GET_16BIT_DATA(cmd_buf, 8);
                    if(chn->camera_ops->ctl_zoom_focus_direct(zoom_pos, focus_pos) != CCP_OK) goto error;
                }
                else
                {
                    goto error;
                }
            } break;
            
            /* DZOOM 
            8x 01 04 06 02/03/00/2p/2q/10 FF */
            case VISCA_DZOOM:
            {
                if(chn->camera_ops->ctl_dzoom == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_dzoom(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* DZOOM MODE
            8x 01 04 36 00/01 FF */
            case VISCA_DZOOM_MODE:
            {
                if(chn->camera_ops->ctl_dzoom_mode == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_dzoom_mode(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* DZOOM DIRECT
            8x 01 04 46 00 00 0p 0q FF */
            case VISCA_DZOOM_DIRECT:
            {
                uint16_t dzoom_pos;
                if(chn->camera_ops->ctl_dzoom_direct == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                dzoom_pos = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_dzoom_direct(dzoom_pos) != CCP_OK) goto error;
            } break;
            
            /* 聚焦模式
            8x 01 04 38 02/03 FF */
            case VISCA_FOCUS_MODE:
            {
                if(chn->camera_ops->ctl_focus_mode == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_focus_mode(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 自动聚焦模式
            8x 01 04 57 00/01/02 FF */
            case VISCA_AF_MODE:
            {
                if(chn->camera_ops->ctl_af_mode == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_af_mode(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 自动聚焦灵敏度
            8x 01 04 58 02/03 FF*/
            case VISCA_AFS:
            {
                if(chn->camera_ops->ctl_af_sensitivity == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_af_sensitivity(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* FOCUS 
            8x 01 04 08 00/02/03/2p/3p FF */
            case VISCA_FOCUS:
            {
                if(chn->camera_ops->ctl_focus == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_focus(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* FOCUS DIRECT
            8x 01 04 48 0p 0q 0r 0s FF */
            case VISCA_FOCUS_DIRECT:
            {
                uint16_t focus_pos;
                if(chn->camera_ops->ctl_focus_direct == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                focus_pos = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_focus_direct(focus_pos) != CCP_OK) goto error;
            } break;
            
            /* 白平衡
            8x 01 04 35 00/01/02/03/04/05 FF */
            case VISCA_WB:
            {
                if(chn->camera_ops->ctl_wb_mode == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_wb_mode(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* RGAIN
            8x 01 04 03 00/02/03 FF */
            case VISCA_RGAIN:
            {
                if(chn->camera_ops->ctl_rgain == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_rgain(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* RGAIN DIRECT 
            8x 01 04 43 00 00 0p 0q FF */
            case VISCA_RGAIN_DIRECT:
            {
                uint16_t rgain;
                if(chn->camera_ops->ctl_rgain_direct == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                rgain = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_rgain_direct(rgain) != CCP_OK) goto error;
            } break;
            
            /* BGAIN
            8x 01 04 04 00/02/03 FF */
            case VISCA_BGAIN:
            {
                if(chn->camera_ops->ctl_bgain == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_bgain(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* BGAIN DIRECT
            8x 01 04 44 00 00 0p 0q FF */
            case VISCA_BGAIN_DIRECT:
            {
                uint16_t bgain;
                if(chn->camera_ops->ctl_bgain_direct == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                bgain = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_bgain_direct(bgain) != CCP_OK) goto error;
            } break;
            
            /* 曝光模式
            8x 01 04 39 00/03/0A/0B/0D FF */
            case VISCA_EXP_MODE:
            {
                if(chn->camera_ops->ctl_exp_mode == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_exp_mode(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 快门
            8x 01 04 0A 00/02/03 FF */
            case VISCA_EXP_SHUTTER:
            {
                if(chn->camera_ops->ctl_exp_shutter == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_exp_shutter(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 快门 DIRECT
            8x 01 04 4A 00 00 0p 0q FF */
            case VISCA_EXP_SHUTTER_DIRECT:
            {
                uint16_t exp_shutter;
                if(chn->camera_ops->ctl_exp_shutter_direct == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                exp_shutter = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_exp_shutter_direct(exp_shutter) != CCP_OK) goto error;
            } break;
            
            /* 低速快门
            8x 01 04 5A 02/03 FF */
            case VISCA_EXP_SLOW_SHUTTER:
            {
                if(chn->camera_ops->ctl_exp_slow_shutter == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_exp_slow_shutter(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 光圈
            8x 01 04 0B 00/02/03 FF*/
            case VISCA_EXP_IRIS:
            {
                if(chn->camera_ops->ctl_exp_iris == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_exp_iris(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 光圈 DIRECT
            8x 01 04 4b 00 00 0p 0q FF */
            case VISCA_EXP_IRIS_DIRECT:
            {
                uint16_t iris;
                if(chn->camera_ops->ctl_exp_iris_direct == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                iris = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_exp_iris_direct(iris) != CCP_OK) goto error;
            } break;
            
            /* 增益
            8x 01 04 0C 00/02/03 FF */
            case VISCA_EXP_GAIN:
            {
                if(chn->camera_ops->ctl_exp_gain == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_exp_gain(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 增益 DIRECT
            8x 01 04 4C 00 00 0p 0q FF */
            case VISCA_EXP_GAIN_DIRECT:
            {
                uint16_t gain;
                if(chn->camera_ops->ctl_exp_gain_direct == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                gain = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_exp_gain_direct(gain) != CCP_OK) goto error;
            } break;
            
            /* 亮度
            8x 01 04 0D 00/02/03 FF */
            case VISCA_EXP_BRIGHT:
            {
                if(chn->camera_ops->ctl_exp_bright == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_exp_bright(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 亮度 DIRECT
            8x 01 04 4D 00 00 0p 0q FF */
            case VISCA_EXP_BRIGHT_DIRECT:
            {
                uint16_t bright;
                if(chn->camera_ops->ctl_exp_bright_direct == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                bright = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_exp_bright_direct(bright) != CCP_OK) goto error;
            } break;
            
            /* 曝光补偿开关
            8x 01 04 3E 02/03 FF */
            case VISCA_EXP_COMP_POWER:
            {
                if(chn->camera_ops->ctl_exp_comp_onoff == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_exp_comp_onoff(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 曝光补偿
            8x 01 04 0E 02/03 FF */
            case VISCA_EXP_COMP:
            {
                if(chn->camera_ops->ctl_exp_comp == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_exp_comp(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 曝光补偿 DIRECT
            8x 01 04 4E 00 00 0p 0q FF */
            case VISCA_EXP_COMP_DIRECT:
            {
                uint16_t temp;
                if(chn->camera_ops->ctl_exp_comp_direct == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                temp = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_exp_comp_direct(temp) != CCP_OK) goto error;
            } break;
            
            /* 背光补偿
            8x 01 04 33 02/03 FF */
            case VISCA_BACKLIGHT:
            {
                if(chn->camera_ops->ctl_backlight == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_backlight(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 锐度
            8x 01 04 02 00/02/03 FF */
            case VISCA_SHARPNESS:
            {
                if(chn->camera_ops->ctl_aperture == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_aperture(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 锐度 DIRECT
            8x 01 04 42 00 00 0p 0q FF */
            case VISCA_SHARPNESS_DIRECT:
            {
                uint16_t temp;
                if(chn->camera_ops->ctl_aperture_direct == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                temp = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_aperture_direct(temp) != CCP_OK) goto error;
            } break;
            
            /* 伽马
            8x 01 04 5B 00~04 FF */
            case VISCA_GAMMA:
            {
                if(chn->camera_ops->ctl_gamma == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_gamma(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 伽马偏移
            8x 01 04 1E 00 00 00 0s 0t 0u FF */
            case VISCA_GAMMA_OFFSET:
            {
                uint8_t temp;
                if(chn->camera_ops->ctl_gamma_offset == NULL) goto error;
                if(cmd_buf[10] != 0xFF) goto error;
                temp = ARR_GET_8BIT_DATA(cmd_buf, 8);
                if(chn->camera_ops->ctl_gamma_offset(cmd_buf[7], temp) != CCP_OK) goto error;
            } break;
            
            /* 色彩
            8x 01 04 49 00 00 00 0p FF */
            case VISCA_COLOR_GAIN:
            {
                uint16_t temp;
                if(chn->camera_ops->ctl_color_gain == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                temp = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_color_gain(temp) != CCP_OK) goto error;
            } break;
            
            /* 色调
            8x 01 04 4F 00 00 00 0p FF */
            case VISCA_HUE:
            {
                uint16_t temp;
                if(chn->camera_ops->ctl_hue == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                temp = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_hue(temp) != CCP_OK) goto error;
            } break;
            
            /* 降噪
            8x 01 04 53 0p FF */
            case VISCA_NR:
            {
                if(chn->camera_ops->ctl_nr == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_nr(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 去雾
            8x 01 04 37 02/03 00/01/02/03 FF */
            case VISCA_DEFOG:
            {
                if(chn->camera_ops->ctl_defog == NULL) goto error;
                if(cmd_buf[6] != 0xFF) goto error;
                if(chn->camera_ops->ctl_defog(cmd_buf[4], cmd_buf[5]) != CCP_OK) goto error;
            } break;
            
            /* 频闪抑制 */
            case VISCA_FLICK:
            if(chn->camera_ops->ctl_flick == NULL) goto error;
            break;
            
            /* 宽动态 */
            case VISCA_WDR:
            if(chn->camera_ops->ctl_wdr == NULL) goto error;
            break;
            
            /* 图像翻转
            8x 01 04 66 02/03 FF */
            case VISCA_PIC_FLIP:
            {
                if(chn->camera_ops->ctl_flip == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_flip(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 图像镜像
            8x 01 04 61 02/03 FF */
            case VISCA_LR_REVERSE:
            {
                if(chn->camera_ops->ctl_mirror == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_mirror(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 图像屏蔽
            8x 01 04 75 02/03 FF */
            case VISCA_MUTE:
            {
                if(chn->camera_ops->ctl_mute == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_mute(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 图像冻结
            8x 01 04 62 02/03 FF */
            case VISCA_FREEZE:
            {
                if(chn->camera_ops->ctl_freeze == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_freeze(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 信息显示
            8x 01 04 15 02/03 FF */
            case VISCA_DISPLAY:
            {
                if(chn->camera_ops->ctl_display == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_display(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            /* 防抖
            8x 01 04 34 02/03/04 FF */
            case VISCA_STABILIZER:
            {
                if(chn->camera_ops->ctl_stabilizer == NULL) goto error;
                if(cmd_buf[5] != 0xFF) goto error;
                if(chn->camera_ops->ctl_stabilizer(cmd_buf[4]) != CCP_OK) goto error;
            } break;
            
            
            /* 饱和度
            8x 01 04 A1 00 00 0p 0q FF */
            case VISCA_SATURATION:
            {
                if(chn->camera_ops->ctl_saturation == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                uint16_t temp = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_saturation(temp) != CCP_OK) goto error;
            } break;
            
            /* 对比度
            8x 01 04 A2 00 00 0p 0q FF */
            case VISCA_CONTRAST:
            {
                if(chn->camera_ops->ctl_contrast == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                uint16_t temp = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_contrast(temp) != CCP_OK) goto error;
            } break;
            
            /* 亮度
            8x 01 04 A3 00 00 0p 0q FF*/
            case VISCA_BRIGHT:
            {
                if(chn->camera_ops->ctl_bright == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                uint16_t temp = ARR_GET_16BIT_DATA(cmd_buf, 4);
                if(chn->camera_ops->ctl_bright(temp) != CCP_OK) goto error;
            } break;
            
            /* 预置位
            8x 01 04 3F 00/01/02 0p FF */
            case VISCA_MEMORY:
            {
                if(chn->camera_ops->ctl_preset == NULL) goto error;
                
                /* 0~127 */
                if(cmd_buf[6] == 0xFF)
                {
                    if(chn->camera_ops->ctl_preset(cmd_buf[4], cmd_buf[5]) != CCP_OK) goto error;
                }
                /* 0~255 */
                else if(cmd_buf[7] == 0xFF)
                {
                    uint8_t temp = (cmd_buf[5] << 4) | cmd_buf[6];
                    if(chn->camera_ops->ctl_preset(cmd_buf[4], temp) != CCP_OK) goto error;
                }
                else
                {
                    goto error;
                }
                
            } break;
            
            /* 寄存器
            8x 01 04 24 mm 0p 0q FF */
            case VISCA_REG:
            {
                uint8_t reg, value;
                if(chn->camera_ops->ctl_reg == NULL) goto error;
                if(cmd_buf[7] != 0xFF) goto error;
                reg = cmd_buf[4];
                value = ARR_GET_8BIT_DATA(cmd_buf, 5);
                if(chn->camera_ops->ctl_reg(reg, value) != CCP_OK) goto error;
            } break;
            
            default:
            goto error;
            break;
        }
        break;
        
        /* 云台指令 8x 09 06 xx*/
        case 0x06:
        switch(cmd_buf[3])
        {
            /* 云台上下左右
            8x 01 06 01 vv ww 0x 0x FF */
            case VISCA_PT_RUN:
            {
                uint8_t pan_speed, tilt_speed;
                if(chn->camera_ops->pt_run == NULL) goto error;
                if(cmd_buf[8] != 0xFF) goto error;
                
                /* 速度转化，系统内部速度是1~60°每秒 */
                /* VISCA速度等级1~24 */   //24*2.5=60
                if(chn->speed_grade == 0)
                {
                    pan_speed = (uint8_t)(cmd_buf[4] * 2.5f);
                    tilt_speed = (uint8_t)(cmd_buf[5] * 2.5f);
                }
                /* 扩展速度等级1~100 */
                else
                {
                    pan_speed = (uint8_t)(cmd_buf[4] * 0.6f);
                    tilt_speed = (uint8_t)(cmd_buf[5] * 0.6f);
                }
                
                if(pan_speed > 60) pan_speed = 60;
                if(pan_speed < 1 && cmd_buf[4] != 0) pan_speed = 1;
                if(tilt_speed > 60) tilt_speed = 60;
                if(tilt_speed < 1 && cmd_buf[5] != 0) tilt_speed = 1;

                if(chn->camera_ops->pt_run(pan_speed, tilt_speed, cmd_buf[6], cmd_buf[7]) != CCP_OK) goto error;
            } break;
            
            /* 云台绝对位置
            8x 01 06 02 vv ww 0y 0y 0y 0y 0z 0z 0z 0z FF */
            case VISCA_PT_ABS_POS:
            {
                uint8_t pan_speed, tilt_speed;
                int16_t pan_pos, tilt_pos;
                if(chn->camera_ops->pt_abs_run == NULL) goto error;
                if(cmd_buf[14] != 0xFF) goto error;
                
                /* 速度转化，系统内部速度是1~60°每秒 */
                /* VISCA速度等级1~24 */
                if(chn->speed_grade == 0)
                {
                    pan_speed = (uint8_t)(cmd_buf[4] * 2.5f);
                    tilt_speed = (uint8_t)(cmd_buf[5] * 2.5f);
                }
                /* 扩展速度等级1~100 */
                else
                {
                    pan_speed = (uint8_t)(cmd_buf[4] * 0.6f);
                    tilt_speed = (uint8_t)(cmd_buf[5] * 0.6f);
                }
                
                if(pan_speed > 60) pan_speed = 60;
                if(pan_speed < 1 && cmd_buf[4] != 0) pan_speed = 1;
                if(tilt_speed > 60) tilt_speed = 60;
                if(tilt_speed < 1 && cmd_buf[5] != 0) tilt_speed = 1;
                
                pan_pos = ARR_GET_16BIT_DATA(cmd_buf, 6);
                tilt_pos = ARR_GET_16BIT_DATA(cmd_buf, 10);
                
                if(chn->camera_ops->pt_abs_run(pan_speed, tilt_speed, pan_pos, tilt_pos) != CCP_OK) goto error;
            } break;
            
            /* 云台相对位置
            8x 01 06 02 vv ww 0y 0y 0y 0y 0z 0z 0z 0z FF */
            case VISCA_PT_REL_POS:
            {
                uint8_t pan_speed, tilt_speed;
                int16_t pan_pos, tilt_pos;
                if(chn->camera_ops->pt_rel_run == NULL) goto error;
                if(cmd_buf[14] != 0xFF) goto error;
                
                /* 速度转化，系统内部速度是1~60°每秒 */
                /* VISCA速度等级1~24 */
                if(chn->speed_grade == 0)
                {
                    pan_speed = (uint8_t)(cmd_buf[4] * 2.5f);
                    tilt_speed = (uint8_t)(cmd_buf[5] * 2.5f);
                }
                /* 扩展速度等级1~100 */
                else
                {
                    pan_speed = (uint8_t)(cmd_buf[4] * 0.6f);
                    tilt_speed = (uint8_t)(cmd_buf[5] * 0.6f);
                }
                
                if(pan_speed > 60) pan_speed = 60;
                if(pan_speed < 1 && cmd_buf[4] != 0) pan_speed = 1;
                if(tilt_speed > 60) tilt_speed = 60;
                if(tilt_speed < 1 && cmd_buf[5] != 0) tilt_speed = 1;

                pan_pos = ARR_GET_16BIT_DATA(cmd_buf, 6);
                tilt_pos = ARR_GET_16BIT_DATA(cmd_buf, 10);
                
                if(chn->camera_ops->pt_rel_run(pan_speed, tilt_speed, pan_pos, tilt_pos) != CCP_OK) goto error;
            } break;
            
            
            /* 云台回位
            8x 01 06 04 FF*/
            case VISCA_PT_HOME:
            {
                if(chn->camera_ops->ptz_home == NULL) goto error;
                if(chn->camera_ops->ptz_home() != CCP_OK) goto error;
            } break;
            
            /* 云台重置 */
            case VISCA_PT_RESET:
            {
                if(chn->camera_ops->ptz_reset == NULL) goto error;
                if(chn->camera_ops->ptz_reset() != CCP_OK) goto error;
            } break;
            
            /* 菜单 8x 01 06 06 xx*/
            case VISCA_PT_MENU:
            {
                /* 打开、返回、退出菜单 */
                switch(cmd_buf[4])
                {
                    //OSD OPEN/CLOSE    0x81, 0x01, 0x06, 0x06, 0x10, 0xFF
                    case VISCA_MENU_ON_OFF:
                    if(chn->camera_ops->ctl_menu == NULL) goto error;
                    if(chn->camera_ops->ctl_menu(VISCA_MENU_ON_OFF) != CCP_OK) goto error;
                    break;
                    
                    //OSD OPEN          0x81, 0x01, 0x06, 0x06, 0x02, 0xFF
                    case VISCA_MENU_ON:
                    if(chn->camera_ops->ctl_menu == NULL) goto error;
                    if(chn->camera_ops->ctl_menu(VISCA_MENU_ON) != CCP_OK) goto error;
                    break;
                    
                    //OSD CLOSE         0x81, 0x01, 0x06, 0x06, 0x03, 0xFF
                    case VISCA_MENU_OFF:
                    if(chn->camera_ops->ctl_menu == NULL) goto error;
                    if(chn->camera_ops->ctl_menu(VISCA_MENU_OFF) != CCP_OK) goto error;
                    break;
                    
                    default: goto error;
                    break;
                }
            } break;
            
            default: goto error;
            break;
        }
        break;
        
        /* 扩充指令 8x 01 07 xx*/
        case 0x07:
        switch(cmd_buf[3])
        {
            /* 水平反向 8x 01 07 21 02/03 FF */
            case EXT_PAN_REVERSE:
            if(chn->camera_ops->pan_reverse == NULL) goto error;
            if(cmd_buf[5] != 0xFF) goto error;
            if(chn->camera_ops->pan_reverse(cmd_buf[4]) != CCP_OK) goto error;
            break;
            
            /* 垂直反向 8x 01 07 22 02/03 FF */
            case EXT_TILT_REVERSE:
            if(chn->camera_ops->tilt_reverse == NULL) goto error;
            if(cmd_buf[5] != 0xFF) goto error;
            if(chn->camera_ops->tilt_reverse(cmd_buf[4]) != CCP_OK) goto error;
            break;
            
            /* 网络 8x 01 07 23 xx */
            case EXT_NET:
            {
                switch(cmd_buf[4])
                {
                    /* DHCP 8x 01 07 23 00 02/03 FF */
                    case EXT_NET_DHCP:
                    if(chn->camera_ops->net_dhcp == NULL) goto error;
                    if(cmd_buf[6] != 0xFF) goto error;
                    if(chn->camera_ops->net_dhcp(cmd_buf[5]) != CCP_OK) goto error;
                    break;
                    
                    /* IP地址     8x 01 07 23 01 0x 0x 0x 0x 0x 0x 0x 0x FF */
                    /* 子网掩码   8x 01 07 23 01 0x 0x 0x 0x 0x 0x 0x 0x FF */
                    /* 网关       8x 01 07 23 01 0x 0x 0x 0x 0x 0x 0x 0x FF */
                    case EXT_NET_IP:
                    case EXT_NET_MASK:
                    case EXT_NET_GATEWAY:
                    {
                        uint32_t ip;
                        if(chn->camera_ops->net_ip == NULL) goto error;
                        if(cmd_buf[13] != 0xFF) goto error;
                        ip = ARR_GET_32BIT_DATA(cmd_buf, 5);
                        if(chn->camera_ops->net_ip(cmd_buf[4], ip) != CCP_OK) goto error;
                    } break;
                    
                    default: goto error;
                }
            }break;
            
            default: goto error;
            break;
        }
        break;
        
        /* 扩充指令 8x 01 7E xx*/
        case 0x7E:
        //OSD ENTER         0x81, 0x01, 0x7E, 0x01, 0x02, 0x00, 0x01, 0xFF
        if(cmd_buf[3] == 0x01 && cmd_buf[4] == 0x02 && cmd_buf[5] == 0x00 && cmd_buf[6] == 0x01 && cmd_buf[7] == 0xFF)
        {
            if(chn->camera_ops->ctl_menu_enter == NULL) goto error;
            if(chn->camera_ops->ctl_menu_enter(0) != CCP_OK) goto error;
        }
        else goto error;
        break;
        
        default: goto error;
        break;
    }
    
    complete:
    /* 返回ACK和已执行的回码 */
    Ccp_ViscaSendRetCmd(chn, VISCA_ACK);
    Ccp_ViscaSendRetCmd(chn, VISCA_COMPLETE);
    /* 活动提示，LED灯闪烁 */
    if(ccp_comm_ops->action_hint != NULL)
        ccp_comm_ops->action_hint();
    return;
    
    error:
    Ccp_ViscaSendRetCmd(chn, VISCA_ERROR_SYNTAX);       //
    return;
}

/***************************************************************************************************
* Description:  查询指令解析，8x 09
***************************************************************************************************/
static inline void Fill16BitData(uint8_t *cmd_buf, uint16_t data)
{
    cmd_buf[0] = (data >> 12) & 0x0F;
    cmd_buf[1] = (data >> 8) & 0x0F;
    cmd_buf[2] = (data >> 4) & 0x0F;
    cmd_buf[3] = (data >> 0) & 0x0F;
}

static inline void Fill32BitData(uint8_t *cmd_buf, uint32_t data)
{
    cmd_buf[0] = (data >> 28) & 0x0F;
    cmd_buf[1] = (data >> 24) & 0x0F;
    cmd_buf[2] = (data >> 20) & 0x0F;
    cmd_buf[3] = (data >> 16) & 0x0F;
    cmd_buf[4] = (data >> 12) & 0x0F;
    cmd_buf[5] = (data >> 8) & 0x0F;
    cmd_buf[6] = (data >> 4) & 0x0F;
    cmd_buf[7] = (data >> 0) & 0x0F;
}

static inline void _Send4BytesInquiryCmd(struct ccp_chn *chn, uint8_t val)
{
    static uint8_t ret4byte[4] = {0x90, 0x50, 0x00, 0xFF};
    ret4byte[0] = RET_ADDR(chn->addr);
    ret4byte[2] = val;
    chn->send_packet(ret4byte, sizeof(ret4byte));
}

static inline void _Send7BytesInquiryCmd(struct ccp_chn *chn, uint16_t val)
{
    static uint8_t ret7byte[7] = {0x90, 0x50, 0x00, 0x00, 0x00, 0x00, 0xFF};
    ret7byte[0] = RET_ADDR(chn->addr);
    Fill16BitData(&ret7byte[2], val);
    chn->send_packet(ret7byte, sizeof(ret7byte));
}

static inline void _Send11BytesInquiryCmd(struct ccp_chn *chn, uint32_t val)
{
    static uint8_t ret11byte[11] = {0x90, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    ret11byte[0] = RET_ADDR(chn->addr);
    Fill32BitData(&ret11byte[2], val);
    chn->send_packet(ret11byte, sizeof(ret11byte));
}

static inline void _Visca_InquireCmdParser(struct ccp_chn *chn, uint8_t *cmd_buf, uint8_t length)
{
    static uint8_t ret5byte[5] = {0x90, 0x50, 0x00, 0x00, 0xFF};
    uint8_t temp8bit1, temp8bit2;
    uint16_t temp16bit1;
    uint32_t temp32bit1;
    
    if(chn->camera_ops == NULL) goto error;
    
    /* 待机指令处理 */
    if(ccp_comm_ops->get_power_status != NULL)
    {
        if(ccp_comm_ops->get_power_status() == 0)
        {
            if(cmd_buf[2] == 0x04 && cmd_buf[3] == VISCA_POWER && cmd_buf[5] == 0xFF)
            {
                if(chn->camera_ops->inq_power == NULL) goto error;
                if(chn->camera_ops->inq_power(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
                goto complete;
            }
            else
            {
                goto error;
            }
        }
    }
    
    /* 开机指令处理 */
    switch(cmd_buf[2])
    {
        /* 版本信息 8x 09 00 xx */
        case 0x00:
        switch(cmd_buf[3])
        {
            case 0x02:
            {
                uint8_t camera_version[10]={0x90, 0x50,0x00,0x01,0x05,0x04,0x01,0x04,0x02,0xff};
                camera_version[0] = RET_ADDR(chn->addr);
                chn->send_packet(camera_version, sizeof(camera_version));
            } break;
            
            default:
            goto error;
            break;
        }     
        break;
        
        /* 图像指令 8x 09 04 xx */
        case 0x04:
        switch(cmd_buf[3])
        {
            /* 电源
            y0 50 02/03 FF */
            case VISCA_POWER:
            {
                if(chn->camera_ops->inq_power == NULL) goto error;
                if(chn->camera_ops->inq_power(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;

            /* ZOOM
            y0 50 0p 0q 0r 0s FF */
            case VISCA_ZOOM_DIRECT:
            {
                if(chn->camera_ops->inq_zoom == NULL) goto error;
                if(chn->camera_ops->inq_zoom(&temp16bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp16bit1);
            } break;
            
            /* DZOOM MODE*/
            case VISCA_DZOOM_MODE:
            {
                if(chn->camera_ops->inq_dzoom_mode == NULL) goto error;
                if(chn->camera_ops->inq_dzoom_mode(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* DZOOM */
            case VISCA_DZOOM_DIRECT:
            {
                if(chn->camera_ops->inq_dzoom == NULL) goto error;
                if(chn->camera_ops->inq_dzoom(&temp16bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp16bit1);
            } break;
            
            /* 聚焦模式 */
            case VISCA_FOCUS_MODE:
            {
                if(chn->camera_ops->inq_focus_mode == NULL) goto error;
                if(chn->camera_ops->inq_focus_mode(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 自动聚焦模式 */
            case VISCA_AF_MODE:
            {
                if(chn->camera_ops->inq_af_mode == NULL) goto error;
                if(chn->camera_ops->inq_af_mode(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 自动聚焦灵敏度 */
            case VISCA_AFS:
            {
                if(chn->camera_ops->inq_af_sensitivity == NULL) goto error;
                if(chn->camera_ops->inq_af_sensitivity(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* FOCUS */
            case VISCA_FOCUS_DIRECT:
            {
                if(chn->camera_ops->inq_focus == NULL) goto error;
                if(chn->camera_ops->inq_focus(&temp16bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp16bit1);
            } break;
            
            /* 白平衡 */
            case VISCA_WB:
            {
                if(chn->camera_ops->inq_wb_mode == NULL) goto error;
                if(chn->camera_ops->inq_wb_mode(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* RGAIN */
            case VISCA_RGAIN_DIRECT:
            {
                if(chn->camera_ops->inq_rgain == NULL) goto error;
                if(chn->camera_ops->inq_rgain(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* BGAIN */
            case VISCA_BGAIN_DIRECT:
            {
                if(chn->camera_ops->inq_bgain == NULL) goto error;
                if(chn->camera_ops->inq_bgain(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 曝光模式 */
            case VISCA_EXP_MODE:
            {
                if(chn->camera_ops->inq_exp_mode == NULL) goto error;
                if(chn->camera_ops->inq_exp_mode(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 快门 */
            case VISCA_EXP_SHUTTER_DIRECT:
            {
                if(chn->camera_ops->inq_exp_shutter == NULL) goto error;
                if(chn->camera_ops->inq_exp_shutter(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 低速快门 */
            case VISCA_EXP_SLOW_SHUTTER:
            {
                if(chn->camera_ops->inq_exp_slow_shutter == NULL) goto error;
                if(chn->camera_ops->inq_exp_slow_shutter(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 光圈 */
            case VISCA_EXP_IRIS_DIRECT:
            {
                if(chn->camera_ops->inq_exp_iris == NULL) goto error;
                if(chn->camera_ops->inq_exp_iris(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 增益 */
            case VISCA_EXP_GAIN_DIRECT:
            {
                if(chn->camera_ops->inq_exp_gain == NULL) goto error;
                if(chn->camera_ops->inq_exp_gain(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 亮度 */
            case VISCA_EXP_BRIGHT_DIRECT:
            {
                if(chn->camera_ops->inq_exp_bright == NULL) goto error;
                if(chn->camera_ops->inq_exp_bright(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 曝光补偿开关 */
            case VISCA_EXP_COMP_POWER:
            {
                if(chn->camera_ops->inq_exp_comp_onoff == NULL) goto error;
                if(chn->camera_ops->inq_exp_comp_onoff(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 曝光补偿 */
            case VISCA_EXP_COMP_DIRECT:
            {
                if(chn->camera_ops->inq_exp_comp == NULL) goto error;
                if(chn->camera_ops->inq_exp_comp(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 背光补偿 */
            case VISCA_BACKLIGHT:
            {
                if(chn->camera_ops->inq_backlight == NULL) goto error;
                if(chn->camera_ops->inq_backlight(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 锐度 */
            case VISCA_SHARPNESS_DIRECT:
            {
                if(chn->camera_ops->inq_aperture == NULL) goto error;
                if(chn->camera_ops->inq_aperture(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 伽马 */
            case VISCA_GAMMA:
            {
                if(chn->camera_ops->inq_gamma == NULL) goto error;
                if(chn->camera_ops->inq_gamma(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 伽马偏移
            y0 50 00 00 00 0s 0t 0u FF */
            case VISCA_GAMMA_OFFSET:
            {
                uint8_t ret9byte[9] = {0x90, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
                if(chn->camera_ops->inq_gamma_offset == NULL) goto error;
                if(chn->camera_ops->inq_gamma_offset(&temp8bit1, &temp8bit2) != CCP_OK) goto error;
                ret9byte[0] = RET_ADDR(chn->addr);
                ret9byte[5] = temp8bit1;
                ret9byte[6] = (temp8bit2 >> 4) & 0x0F;
                ret9byte[7] = temp8bit2 & 0x0F;
                chn->send_packet(ret9byte, sizeof(ret9byte));
            } break;
            
            /* 色彩 */
            case VISCA_COLOR_GAIN:
            {
                if(chn->camera_ops->inq_color_gain == NULL) goto error;
                if(chn->camera_ops->inq_color_gain(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 色调 */
            case VISCA_HUE:
            {
                if(chn->camera_ops->inq_hue == NULL) goto error;
                if(chn->camera_ops->inq_hue(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 去雾
            y0 50 02/03 0p FF */
            case VISCA_DEFOG:
            {
                if(chn->camera_ops->inq_defog == NULL) goto error;
                if(chn->camera_ops->inq_defog(&temp8bit1, &temp8bit2) != CCP_OK) goto error;
                ret5byte[0] = RET_ADDR(chn->addr);
                ret5byte[2] = temp8bit1;
                ret5byte[3] = temp8bit2;
                chn->send_packet(ret5byte, sizeof(ret5byte));
            } break;
            
            /* 降噪 */
            case VISCA_NR:
            {
                if(chn->camera_ops->inq_nr == NULL) goto error;
                if(chn->camera_ops->inq_nr(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            //            /* 频闪抑制 */
            //            case VISCA_FLICK:
            //            if(chn->camera_ops->inq_flick == NULL) goto error;
            //            break;
            //            
            //            /* 宽动态 */
            //            case VISCA_WDR:
            //            if(chn->camera_ops->inq_wdr == NULL) goto error;
            //            break;
            
            /* 图像翻转 */
            case VISCA_PIC_FLIP:
            {
                if(chn->camera_ops->inq_flip == NULL) goto error;
                if(chn->camera_ops->inq_flip(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 图像镜像 */
            case VISCA_LR_REVERSE:
            {
                if(chn->camera_ops->inq_mirror == NULL) goto error;
                if(chn->camera_ops->inq_mirror(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 图像屏蔽 */
            case VISCA_MUTE:
            {
                if(chn->camera_ops->inq_mute == NULL) goto error;
                if(chn->camera_ops->inq_mute(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 图像冻结 */
            case VISCA_FREEZE:
            {
                if(chn->camera_ops->inq_freeze == NULL) goto error;
                if(chn->camera_ops->inq_freeze(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 信息显示 */
            case VISCA_DISPLAY:
            {
                if(chn->camera_ops->inq_display == NULL) goto error;
                if(chn->camera_ops->inq_display(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 防抖 */
            case VISCA_STABILIZER:
            {
                if(chn->camera_ops->inq_stabilizer == NULL) goto error;
                if(chn->camera_ops->inq_stabilizer(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 饱和度 */
            case VISCA_SATURATION:
            {
                if(chn->camera_ops->inq_saturation == NULL) goto error;
                if(chn->camera_ops->inq_saturation(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 对比度 */
            case VISCA_CONTRAST:
            {
                if(chn->camera_ops->inq_contrast == NULL) goto error;
                if(chn->camera_ops->inq_contrast(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 亮度 */
            case VISCA_BRIGHT:
            {
                if(chn->camera_ops->inq_bright == NULL) goto error;
                if(chn->camera_ops->inq_bright(&temp8bit1) != CCP_OK) goto error;
                _Send7BytesInquiryCmd(chn, temp8bit1);
            } break;

            /* 预置位 
            8x 09 04 3F xx*/
            case VISCA_MEMORY:
            {
                /* 最后调用的预置位 */
                if(cmd_buf[4] == 0xFF)
                {
                    if(chn->camera_ops->inq_preset_call == NULL) goto error;
                    if(chn->camera_ops->inq_preset_call(&temp8bit1) != CCP_OK) goto error;
                    _Send4BytesInquiryCmd(chn, temp8bit1);
                }
                /* 0~127 */
                else if(cmd_buf[5] == 0xFF)
                {
                    if(chn->camera_ops->inq_preset == NULL) goto error;
                    if(chn->camera_ops->inq_preset(cmd_buf[4], &temp8bit1) != CCP_OK) goto error;
                    _Send4BytesInquiryCmd(chn, temp8bit1);
                }
                /* 0~255 */
                else if(cmd_buf[6] == 0xFF)
                {
                    uint8_t temp = (cmd_buf[4]<<4) | cmd_buf[5];
                    if(chn->camera_ops->inq_preset == NULL) goto error;
                    if(chn->camera_ops->inq_preset(temp, &temp8bit1) != CCP_OK) goto error;
                    _Send4BytesInquiryCmd(chn, temp8bit1);
                }
                else
                {
                    goto error;
                }
            } break;
            
            /* 寄存器 */
            case VISCA_REG:
            {
                if(chn->camera_ops->inq_reg == NULL) goto error;
                if(chn->camera_ops->inq_reg(cmd_buf[4], &temp8bit1) != CCP_OK) goto error;
                ret5byte[0] = RET_ADDR(chn->addr);
                ret5byte[2] = temp8bit1;
                ret5byte[3] = temp8bit2;
                chn->send_packet(ret5byte, sizeof(ret5byte));
            } break;
            
            default: goto error;
            break;
        }
        break;
        
        /* 云台指令 8x 09 06 xx*/
        case 0x06:
        switch(cmd_buf[3])
        {
            /* 菜单 8x 09 06 06 FF*/
            case VISCA_PT_MENU:
            {
                if(chn->camera_ops->inq_menu == NULL) goto error;
                if(chn->camera_ops->inq_menu(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 云台状态 8x 09 06 10 FF*/
            case VISCA_PT_STATUS:
            {
                if(ccp_comm_ops->get_pan_tilt_status == NULL) goto error;
                if(ccp_comm_ops->get_pan_tilt_status() != CCP_OK) goto error;
                ret5byte[0] = RET_ADDR(chn->addr);
                ret5byte[2] = 0x28;
                ret5byte[3] = 0x00;
                chn->send_packet(ret5byte, sizeof(ret5byte));
            } break;
            
            /* 云台最大速度 8x 09 06 11 FF */
            case VISCA_PT_MAX_SPEED:
            {
                ret5byte[0] = RET_ADDR(chn->addr);
                ret5byte[2] = chn->speed_grade;
                ret5byte[3] = chn->speed_grade;
                chn->send_packet(ret5byte, sizeof(ret5byte));
            } break;
            
            /* 云台位置 
            8x 01 06 12 FF
            y0 50 0w 0w 0w 0w 0z 0z 0z 0z FF */
            case VISCA_PT_INQ_POS:
            {
                uint8_t ret11byte[11] = {0x90, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
                int16_t pan_pos, tilt_pos;
                
                if(chn->camera_ops->inq_pt_pos == NULL) goto error;
                if(chn->camera_ops->inq_pt_pos(&pan_pos, &tilt_pos) != CCP_OK) goto error;
                ret11byte[0] = RET_ADDR(chn->addr);
                Fill16BitData(&ret11byte[2], pan_pos);
                Fill16BitData(&ret11byte[6], tilt_pos);
                chn->send_packet(ret11byte, sizeof(ret11byte));
            }
            break;
            
            default: goto error;
            break;
        }
        break;
        
        /* 扩充指令 8x 09 07 xx*/
        case 0x07:
        switch(cmd_buf[3])
        {
            /* 查询版本号
            8x 09 07 00 FF */
            case EXT_VERSION:
            {
                if(cmd_buf[4] != 0xFF) goto error;
                if(chn->camera_ops->inq_version == NULL) goto error;
                if(chn->camera_ops->inq_version(&temp16bit1) != CCP_OK) goto error;
                ret5byte[0] = RET_ADDR(chn->addr);
                ret5byte[2] = (uint8_t)(temp16bit1>>8);
                ret5byte[3] = (uint8_t)(temp16bit1>>0);
                chn->send_packet(ret5byte, sizeof(ret5byte));
            } break;
            
            /* 查询电池电量
            81 09 07 10 FF
            y0 50 0p 0q FF */
            case EXT_BATTERY:
            {
                if(cmd_buf[4] != 0xFF) goto error;
                if(chn->camera_ops->inq_battery == NULL) goto error;
                if(chn->camera_ops->inq_battery(&temp8bit1) != CCP_OK) goto error;
                ret5byte[0] = RET_ADDR(chn->addr);
                ret5byte[2] = (uint8_t)((temp8bit1>>4)&0x0F);
                ret5byte[3] = (uint8_t)((temp8bit1>>0)&0x0F);
                chn->send_packet(ret5byte, sizeof(ret5byte));
            } break;
            
            /* 查询电池电量显示开关
            81 09 07 11 FF
            y0 50 02/03 FF */
            case EXT_BATTERY_DISPLAY:
            {
                if(cmd_buf[4] != 0xFF) goto error;
                if(chn->camera_ops->inq_battery_display == NULL) goto error;
                if(chn->camera_ops->inq_battery_display(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 查询所有预置位使用情况
            8x 09 07 20 FF */
            case EXT_PRESET:
            {
                uint8_t *buffer, size;
                if(cmd_buf[4] != 0xFF) goto error;
                if(chn->camera_ops->inq_preset_ext == NULL) goto error;
                if(chn->camera_ops->inq_preset_ext(&buffer, &size) != CCP_OK) goto error;
                buffer[0] = RET_ADDR(chn->addr);
                chn->send_packet(buffer, size);
            } break;
            
            /* 查询水平反向
            8x 09 07 21 FF */
            case EXT_PAN_REVERSE:
            {
                if(cmd_buf[4] != 0xFF) goto error;
                if(chn->camera_ops->inq_pan_reverse == NULL) goto error;
                if(chn->camera_ops->inq_pan_reverse(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;

            /* 查询垂直反向
            8x 09 07 22 FF */
            case EXT_TILT_REVERSE:
            {
                if(cmd_buf[4] != 0xFF) goto error;
                if(chn->camera_ops->inq_tilt_reverse == NULL) goto error;
                if(chn->camera_ops->inq_tilt_reverse(&temp8bit1) != CCP_OK) goto error;
                _Send4BytesInquiryCmd(chn, temp8bit1);
            } break;
            
            /* 查询网络
            8x 09 07 23 xx */
            case EXT_NET:
            {
                switch(cmd_buf[4])
                {
                    /* 查询DHCP 
                    8x 09 07 23 00 FF*/
                    case EXT_NET_DHCP:
                    {
                        if(cmd_buf[5] != 0xFF) goto error;
                        if(chn->camera_ops->inq_net_dhcp == NULL) goto error;
                        if(chn->camera_ops->inq_net_dhcp(&temp8bit1) != CCP_OK) goto error;
                        _Send4BytesInquiryCmd(chn, temp8bit1);
                    } break;
                    
                    /* 查询IP地址
                    8x 09 07 23 01 FF */
                    /* 查询子网掩码 
                    8x 09 07 23 02 FF */
                    /* 查询网关
                    8x 09 07 23 03 FF */
                    case EXT_NET_IP:
                    case EXT_NET_MASK:
                    case EXT_NET_GATEWAY:
                    {
                        if(cmd_buf[5] != 0xFF) goto error;
                        if(chn->camera_ops->inq_net_ip == NULL) goto error;
                        if(chn->camera_ops->inq_net_ip(cmd_buf[4], &temp32bit1) != CCP_OK) goto error;
                        _Send11BytesInquiryCmd(chn, temp32bit1);
                    } break;
                }
            } break;
            
            default: goto error;
            break;
        }
        break;
        
        /* 扩充指令 8x 09 7E xx*/
        case 0x7E:
        switch(cmd_buf[3])
        {
            /* 组合查询 */
            case 0x7E:
            {
                uint8_t block_inq_buf[16];
                memset(block_inq_buf, 0, sizeof(block_inq_buf));
                block_inq_buf[0] = RET_ADDR(chn->addr);
                block_inq_buf[1] = 0x50;
                block_inq_buf[15] = 0xFF;
                
                switch(cmd_buf[4])
                {
                    /* ZOOM, FOCUS */
                    case 0x00:
                    {
                        /* 2,3,4,5字节 ZOOM POSITION*/
                        if(chn->camera_ops->inq_zoom != NULL)
                        {
                            if(chn->camera_ops->inq_zoom(&temp16bit1) == 0)
                            {
                                block_inq_buf[2] = GET4BIT(temp16bit1, 12);
                                block_inq_buf[3] = GET4BIT(temp16bit1, 8);
                                block_inq_buf[4] = GET4BIT(temp16bit1, 4);
                                block_inq_buf[5] = GET4BIT(temp16bit1, 0);
                            }
                        }
                        
                        /* 8,9,10,11字节 ZOOM POSITION*/
                        if(chn->camera_ops->inq_focus != NULL)
                        {
                            if(chn->camera_ops->inq_focus(&temp16bit1) == 0)
                            {
                                block_inq_buf[8] = GET4BIT(temp16bit1, 12);
                                block_inq_buf[9] = GET4BIT(temp16bit1, 8);
                                block_inq_buf[10] = GET4BIT(temp16bit1, 4);
                                block_inq_buf[11] = GET4BIT(temp16bit1, 0);
                            }
                        }
                        
                        /* 13字节：B5-DZOOM MODE，B3B4-AF MODE，B2-AF SENS，B1-DZOOM，B0-Focus mode  */
                        /* DZOOM MODE*/
                        if(chn->camera_ops->inq_dzoom_mode != NULL)
                        {
                            if(chn->camera_ops->inq_dzoom_mode(&temp8bit1) == 0)
                            {
                                block_inq_buf[13] |= (temp8bit1&0x01) << 5;
                            }
                        }
                        
                        /* 自动聚焦模式 */
                        if(chn->camera_ops->inq_af_mode != NULL)
                        {
                            if(chn->camera_ops->inq_af_mode(&temp8bit1) == 0)
                            {
                                block_inq_buf[13] |= (temp8bit1&0x03) << 3;
                            }
                        }
                        
                        /* 自动聚焦灵敏度 */
                        if(chn->camera_ops->inq_af_sensitivity != NULL)
                        {
                            if(chn->camera_ops->inq_af_sensitivity(&temp8bit1) == 0)
                            {
                                if(temp8bit1 == 0x02)
                                    block_inq_buf[13] |= 1 << 1;
                            }
                        }
                        
                        /* 聚焦模式 */
                        if(chn->camera_ops->inq_focus_mode != NULL)
                        {
                            if(chn->camera_ops->inq_focus_mode(&temp8bit1) == 0)
                            {
                                if(temp8bit1 == 0x02)
                                    block_inq_buf[13] |= 1;
                            }
                        }
                        
                        chn->send_packet(block_inq_buf, sizeof(block_inq_buf));
                    } break;
                    
                    /* White Balance,Auto Focus */
                    case 0x01:
                    {
                        /* 2,3字节 RGAIN*/
                        if(chn->camera_ops->inq_rgain != NULL)
                        {
                            if(chn->camera_ops->inq_rgain(&temp8bit1) == 0)
                            {
                                block_inq_buf[2] = GET4BIT(temp8bit1, 4);
                                block_inq_buf[3] = GET4BIT(temp8bit1, 0);
                            }
                        }
                        
                        /* 4,5字节 BGAIN */
                        if(chn->camera_ops->inq_bgain != NULL)
                        {
                            if(chn->camera_ops->inq_bgain(&temp8bit1) == 0)
                            {
                                block_inq_buf[4] = GET4BIT(temp8bit1, 4);
                                block_inq_buf[5] = GET4BIT(temp8bit1, 0);
                            }
                        }
                        
                        /* 6字节 WB Mode */
                        if(chn->camera_ops->inq_wb_mode != NULL)
                        {
                            if(chn->camera_ops->inq_wb_mode(&temp8bit1) == 0)
                            {
                                block_inq_buf[6] = temp8bit1 & 0x0F;
                            }
                        }
                        
                        /* 7字节 Aperture */
                        if(chn->camera_ops->inq_aperture != NULL)
                        {
                            if(chn->camera_ops->inq_aperture(&temp8bit1) == 0)
                            {
                                block_inq_buf[7] = temp8bit1 & 0x0F;
                            }
                        }
                        
                        /* 8字节 Exposure Mode */
                        if(chn->camera_ops->inq_exp_mode != NULL)
                        {
                            if(chn->camera_ops->inq_exp_mode(&temp8bit1) == 0)
                            {
                                block_inq_buf[8] = temp8bit1 & 0x0F;
                            }
                        }
                        
                        /* 9字节 B2-Backlight B1-Exposure Compensation B0-Slow Shutter */
                        if(chn->camera_ops->inq_backlight != NULL)
                        {
                            if(chn->camera_ops->inq_backlight(&temp8bit1) == 0)
                            {
                                if(temp8bit1 == 0x02)
                                    block_inq_buf[9] |= 1<<2;
                            }
                        }
                        if(chn->camera_ops->inq_exp_comp_onoff != NULL)
                        {
                            if(chn->camera_ops->inq_exp_comp_onoff(&temp8bit1) == 0)
                            {
                                if(temp8bit1 == 0x02)
                                    block_inq_buf[9] |= 1<<1;
                            }
                        }
                        if(chn->camera_ops->inq_exp_slow_shutter != NULL)
                        {
                            if(chn->camera_ops->inq_exp_slow_shutter(&temp8bit1) == 0)
                            {
                                if(temp8bit1 == 0x02)
                                    block_inq_buf[9] |= 1;
                            }
                        }
                        
                        /* 10字节 Shutter */
                        if(chn->camera_ops->inq_exp_shutter != NULL)
                        {
                            if(chn->camera_ops->inq_exp_shutter(&temp8bit1) == 0)
                            {
                                block_inq_buf[10] = temp8bit1 & 0x0F;
                            }
                        }
                        
                        /* 11字节 Iris */
                        if(chn->camera_ops->inq_exp_iris != NULL)
                        {
                            if(chn->camera_ops->inq_exp_iris(&temp8bit1) == 0)
                            {
                                block_inq_buf[11] = temp8bit1 & 0x0F;
                            }
                        }
                        
                        /* 12字节 Gain */
                        if(chn->camera_ops->inq_exp_gain != NULL)
                        {
                            if(chn->camera_ops->inq_exp_gain(&temp8bit1) == 0)
                            {
                                block_inq_buf[12] = temp8bit1 & 0x0F;
                            }
                        }
                        
                        /* 13字节 Bright */
                        if(chn->camera_ops->inq_exp_bright != NULL)
                        {
                            if(chn->camera_ops->inq_exp_bright(&temp8bit1) == 0)
                            {
                                block_inq_buf[13] = temp8bit1 & 0x0F;
                            }
                        }
                        
                        /* 14字节 Exposure Compensation */
                        if(chn->camera_ops->inq_exp_comp != NULL)
                        {
                            if(chn->camera_ops->inq_exp_comp(&temp8bit1) == 0)
                            {
                                block_inq_buf[14] = temp8bit1 & 0x0F;
                            }
                        }
                        
                        chn->send_packet(block_inq_buf, sizeof(block_inq_buf));
                    } break;
                    
                    case 0x02:
                    {
                        /* 2字节 B0-Power */
                        if(chn->camera_ops->inq_power != NULL)
                        {
                            if(chn->camera_ops->inq_power(&temp8bit1) == 0)
                            {
                                if(temp8bit1 == 0x02)
                                    block_inq_buf[2] |= 1;
                            }
                        }
                        
                        /* 3字节 B2-Mirror B3-Freeze B4-ICR */
                        if(chn->camera_ops->inq_mirror != NULL)
                        {
                            if(chn->camera_ops->inq_mirror(&temp8bit1) == 0)
                            {
                                if(temp8bit1 == 0x02)
                                    block_inq_buf[3] |= 1<<2;
                            }
                        }
                        if(chn->camera_ops->inq_freeze != NULL)
                        {
                            if(chn->camera_ops->inq_freeze(&temp8bit1) == 0)
                            {
                                if(temp8bit1 == 0x02)
                                    block_inq_buf[3] |= 1<<3;
                            }
                        }
                        
                        /* 4字节 B2-Display B4-Mute */
                        if(chn->camera_ops->inq_display != NULL)
                        {
                            if(chn->camera_ops->inq_display(&temp8bit1) == 0)
                            {
                                if(temp8bit1 == 0x02)
                                    block_inq_buf[4] |= 1<<2;
                            }
                        }
                        if(chn->camera_ops->inq_mute != NULL)
                        {
                            if(chn->camera_ops->inq_mute(&temp8bit1) == 0)
                            {
                                if(temp8bit1 == 0x02)
                                    block_inq_buf[4] |= 1<<4;
                            }
                        }
                        
                        /* 8,9,10,11字节 Camera ID */
                        block_inq_buf[8] = GET4BIT(0, 12);
                        block_inq_buf[9] = GET4BIT(0, 8);
                        block_inq_buf[10] = GET4BIT(0, 4);
                        block_inq_buf[11] = GET4BIT(0, 0);
                        
                        chn->send_packet(block_inq_buf, sizeof(block_inq_buf));
                        
                    } break;
                    
                    case 0x03:
                    {
                        /* 2,3字节 DZOOM Position */
                        if(chn->camera_ops->inq_dzoom != NULL)
                        {
                            if(chn->camera_ops->inq_dzoom(&temp16bit1) == 0)
                            {
                                block_inq_buf[2] = GET4BIT(temp16bit1, 4);
                                block_inq_buf[3] = GET4BIT(temp16bit1, 0);
                            }
                        }
                        
                        /* 10字节 B0-Image Flip */
                        if(chn->camera_ops->inq_flip != NULL)
                        {
                            if(chn->camera_ops->inq_flip(&temp8bit1) == 0)
                            {
                                if(temp8bit1 == 0x02)
                                    block_inq_buf[10] |= 1;
                            }
                        }
                        
                        chn->send_packet(block_inq_buf, sizeof(block_inq_buf));
                    } break;
                    
                    default: goto error;
                    break;
                }
            } break;
            
            default: goto error;
            break;
            
        } break;
        
        default: goto error;
        break;
    }
    
    complete:
    /* 活动提示，LED灯闪烁 */
    if(ccp_comm_ops->action_hint != NULL)
        ccp_comm_ops->action_hint();
    return;
    
    error:
    Ccp_ViscaSendRetCmd(chn, VISCA_ERROR_SYNTAX);
    return;
}

extern uint8_t cmd_buf[32];
static inline int32_t  Visca_ResetAddress(struct ccp_chn *ccp, uint8_t addr)
{
    static uint8_t addr_reset[] = {0x88, 0x30, 0x01, 0xFF};
    uint8_t *cmd_buf;
    int32_t len;
    int32_t wait_time;
    
    /* 只有输入通道才有地址 */
    if(ccp->path_id < 1)
        return 1;
    
    /* 地址范围只能是1~7 */
    if(addr < 1 || addr > 7)
    {
        Ccp_ViscaSendRetCmd(ccp, VISCA_ERROR_SYNTAX);
        return 1;
    }
    
    /* 活动提示，LED灯闪烁 */
    if(ccp_comm_ops->action_hint != NULL)
        ccp_comm_ops->action_hint();
    
    ccp->addr = addr;
    wait_time = (7-addr) * 100;
    addr_reset[2] = addr +1;
    
    /* 没有输出通道或者已经是最后一台 */
    if(ccp_out1_p == NULL || addr == 7)
    {
        goto end;
    }
    
    /* 清空buffer */
    ccp_out1_p->clear_packet_buffer();
    
    /* 向下级发送设置地址指令 */
    ccp_out1_p->send_packet(addr_reset, sizeof(addr_reset));
    while(wait_time--)
    {
        ccp_comm_ops->delay_ms(1);
        
        len = ccp_out1_p->get_packet(cmd_buf);
        if(len < 4)
            continue;
        
        len-= 4;
        for(int32_t i=0; i<=len; i++)
        {
            if(cmd_buf[i] == 0x88 && cmd_buf[i+1] == 0x30 && cmd_buf[i+3] == 0xFF)
            {
                if(cmd_buf[i+2] >= 2 && cmd_buf[i+2] <= 8)
                {
                    addr_reset[2] = cmd_buf[i+2];
                    goto end;
                }
            }
        }
    }
    
    end: 
    ccp->send_packet(addr_reset, sizeof(addr_reset));
    if(ccp_in1_p->camera_ops->ctl_preset != NULL)
        ccp_in1_p->camera_ops->ctl_preset(1, 0x7F);
    return 0;
}

/***************************************************************************************************
* Description:  VISCA带回码指令解析
***************************************************************************************************/
void Ccp_ViscaRetCmdParser(struct ccp_chn *chn, uint8_t *cmd_buf, uint8_t length)
{
    /* 直接转发给输入端口 */
    if(ccp_in1_p != NULL)
        ccp_in1_p->send_packet(cmd_buf, length);
}

/***************************************************************************************************
* Description:  VISCA指令解析
***************************************************************************************************/
void Ccp_ViscaCmdParser(struct ccp_chn *chn, uint8_t *cmd_buf, uint8_t length)
{
    /* 地址匹配 */
    if(L4BIT(cmd_buf[0]) == chn->addr)
    {
        /* 控制指令 */
        if(cmd_buf[1] == 0x01)
        {
            _Visca_ControlCmdParser(chn, cmd_buf, length);
        }
        /* 查询指令 */
        else if(cmd_buf[1] == 0x09)
        {
            _Visca_InquireCmdParser(chn, cmd_buf, length);
        }
        else
        {
            Ccp_ViscaSendRetCmd(chn, VISCA_ERROR_SYNTAX);
        }
    }
    /* 广播 */
    else if(L4BIT(cmd_buf[0]) == 8)
    {
        if(cmd_buf[1] == 0x01 && cmd_buf[2] == 0x00 && cmd_buf[3] == 0x01 && cmd_buf[4] == 0xFF)
        {
            uint8_t clear_address[] = {0x88, 0x01, 0x00, 0x01, 0xFF};
            if(ccp_comm_ops->action_hint != NULL)
                ccp_comm_ops->action_hint();
            chn->addr = 1;
            chn->send_packet(clear_address, sizeof(clear_address));
            if(ccp_in1_p->camera_ops->ctl_preset != NULL)
                ccp_in1_p->camera_ops->ctl_preset(1, 0x7F);
            return;
        }
        else if(cmd_buf[1] == 0x30 && cmd_buf[3] == 0xFF)
        {
            Visca_ResetAddress(chn, cmd_buf[2]);
            return;
        }
        
        if(ccp_out1_p != NULL)
            ccp_out1_p->send_packet(cmd_buf, length);
        
        /* 控制指令 */
        if(cmd_buf[1] == 0x01)
        {
            _Visca_ControlCmdParser(chn, cmd_buf, length);
        }
        /* 查询指令 */
        else if(cmd_buf[1] == 0x09)
        {
            _Visca_InquireCmdParser(chn, cmd_buf, length);
        }
        else
        {
            Ccp_ViscaSendRetCmd(chn, VISCA_ERROR_SYNTAX);
        }
    }
    /* 不是本机地址 */
    else
    {
        if(ccp_out1_p != NULL)
            ccp_out1_p->send_packet(cmd_buf, length);
    }
    
}
/****************************************** END OF FILE *******************************************/
