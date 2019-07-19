/***************************************************************************************************
* FILE: ccp_pelco.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/11/29, by kevin
***************************************************************************************************/
#include "ccp_pelco.h"
#include "visca.h"

extern struct ccp_ops *ccp_comm_ops;


/***************************************************************************************************
* Description:  Pelco D指令解析
***************************************************************************************************/
void Ccp_PelcoDCmdParser(struct ccp_chn *chn, uint8_t *buf, uint8_t length)
{
    uint16_t cmd_temp;
    uint16_t dat_temp;
    
    /* 检查地址 */
    if(buf[1] != chn->addr)
    {
        return;
    }
    
    cmd_temp = (buf[2] << 8) | buf[3];
    dat_temp = (buf[4] << 8) | buf[5];
    
        
    /* 待机指令处理 */
    if(ccp_comm_ops->get_power_status != NULL)
    {
        if(ccp_comm_ops->get_power_status() == 0)
        {
            if(cmd_temp == PELCO_D_POWER_ON && dat_temp == 0)
            {
                if(chn->camera_ops->ctl_power == NULL) goto error;
                if(chn->camera_ops->ctl_power(VISCA_ON) != CCP_OK) goto error;
                goto complete;
            }
            else goto error;
        }
    }
    
    switch(cmd_temp)
    {
        /* 停止，Zoom Focus Pan Tilt */
        case 0:
        if(dat_temp == 0)
        {
            /* Zoom Stop */
            if(chn->camera_ops->ctl_zoom == NULL) goto error;
            if(chn->camera_ops->ctl_zoom(VISCA_ZOOM_STOP) != CCP_OK) goto error;
            
            /* Focus Stop */
            if(chn->camera_ops->ctl_focus == NULL) goto error;
            if(chn->camera_ops->ctl_focus(VISCA_FOCUS_STOP) != CCP_OK) goto error;
            
            /* Pan Tilt Stop */
            if(chn->camera_ops->pt_run == NULL) goto error;
            if(chn->camera_ops->pt_run(0, 0, 3, 3) != CCP_OK) goto error;
            
            goto complete;
        }
        break;

        /* 电源关 */
        case PELCO_D_POWER_OFF:
        if(dat_temp != CCP_OK) goto error;
        if(chn->camera_ops->ctl_power == NULL) goto error;
        if(chn->camera_ops->ctl_power(VISCA_OFF) != CCP_OK) goto error;
        goto complete;
        break;
        
        /* 设置预置位 */
        case PELCO_D_PRESET_SET:
        if(buf[4] != CCP_OK) goto error;
        if(chn->camera_ops->ctl_preset == NULL) goto error;
        if(chn->camera_ops->ctl_preset(VISCA_MEMORY_SET, buf[5]) != CCP_OK) goto error;
        goto complete;
        break;
        
        /* 删除预置位 */
        case PELCO_D_PRESET_CLEAR:
        if(buf[4] != CCP_OK) goto error;
        if(chn->camera_ops->ctl_preset == NULL) goto error;
        if(chn->camera_ops->ctl_preset(VISCA_MEMORY_DELETE, buf[5]) != CCP_OK) goto error;
        goto complete;
        break;
        
        /* 调用预置位 */
        case PELCO_D_PRESET_CALL:
        if(buf[4] != CCP_OK) goto error;
        if(chn->camera_ops->ctl_preset == NULL) goto error;
        if(chn->camera_ops->ctl_preset(VISCA_MEMORY_CALL, buf[5]) != CCP_OK) goto error;
        goto complete;
        break;
        
        /* 聚焦模式 */
        case PELCO_D_FOCUS_MODE:
        if(chn->camera_ops->ctl_focus_mode == NULL) goto error;
        if(dat_temp == 2) 
        {
            if(chn->camera_ops->ctl_focus_mode(VISCA_MANUAL) != CCP_OK) goto error;
        }
        else if(dat_temp == 0)
        {
            if(chn->camera_ops->ctl_focus_mode(VISCA_AUTO) != CCP_OK) goto error;
        }
        else goto error;
        goto complete;
        break;    

        default:
        if(cmd_temp & PELCO_D_IRIS_ADD)         //光圈加
        {
            if(chn->camera_ops->ctl_exp_mode == NULL) goto error;
            if(chn->camera_ops->ctl_exp_mode(VISCA_EXP_MODE_IRIS_PRIORITY) != CCP_OK) goto error;
            if(chn->camera_ops->ctl_exp_iris == NULL) goto error;
            if(chn->camera_ops->ctl_exp_iris(VISCA_EXP_IRIS_UP) != CCP_OK) goto error;
            goto complete;
        }
        else if(cmd_temp & PELCO_D_IRIS_DOWN)   //光圈减
        {
            if(chn->camera_ops->ctl_exp_mode == NULL) goto error;
            if(chn->camera_ops->ctl_exp_mode(VISCA_EXP_MODE_IRIS_PRIORITY) != CCP_OK) goto error;
            if(chn->camera_ops->ctl_exp_iris == NULL) goto error;
            if(chn->camera_ops->ctl_exp_iris(VISCA_EXP_IRIS_DOWM) != CCP_OK) goto error;
            goto complete;
        }
        
        if(cmd_temp & PELCO_D_FOCUS_NEAR)       //焦距 近
        {
            if(chn->camera_ops->ctl_focus_mode == NULL) goto error;
            if(chn->camera_ops->ctl_focus_mode(VISCA_FOCUS_MANUAL) != CCP_OK) goto error;
            if(chn->camera_ops->ctl_focus == NULL) goto error;
            if(chn->camera_ops->ctl_focus(VISCA_FOCUS_NEAR_STD) != CCP_OK) goto error;
            goto complete;
        }
        else if(cmd_temp & PELCO_D_FOCUS_FAR)   //焦距 远
        {
            if(chn->camera_ops->ctl_focus_mode == NULL) goto error;
            if(chn->camera_ops->ctl_focus_mode(VISCA_FOCUS_MANUAL) != CCP_OK) goto error;
            if(chn->camera_ops->ctl_focus == NULL) goto error;
            if(chn->camera_ops->ctl_focus(VISCA_FOCUS_FAR_STD) != CCP_OK) goto error;
            goto complete;
        }
        if(cmd_temp & PELCO_D_ZOOM_TELE)        //变倍 近
        {
            if(chn->camera_ops->ctl_zoom == NULL) goto error;
            if(chn->camera_ops->ctl_zoom(VISCA_ZOOM_TELE_STD) != CCP_OK) goto error;
            goto complete;
        }
        else if(cmd_temp & PELCO_D_ZOOM_WIDE)   //变倍 远
        {
            if(chn->camera_ops->ctl_zoom == NULL) goto error;
            if(chn->camera_ops->ctl_zoom(VISCA_ZOOM_WIDE_STD) != CCP_OK) goto error;
            goto complete;
        }
        
        if(cmd_temp & 0x1E)
        {
            uint8_t pan, tilt, pan_speed, tilt_speed;
            /* 左 */
            if(cmd_temp & PELCO_D_PTZ_LEFT)
            {
                pan = VISCA_PAN_LEFT; 
            }
            /* 右 */
            else if(cmd_temp & PELCO_D_PTZ_RIGHT)
            {
                pan = VISCA_PAN_RIGHT;
            }
            /* 停 */
            else
            {
                pan = VISCA_PAN_STOP;
            }
            
            /* 上 */
            if(cmd_temp & PELCO_D_PTZ_UP)
            {
                tilt = VISCA_TILT_UP; 
            }
            /* 下 */
            else if(cmd_temp & PELCO_D_PTZ_DOWN)
            {
                tilt = VISCA_TILT_DOWN;
            }
            /* 停 */
            else
            {
                tilt = VISCA_TILT_STOP;
            }
            
            /* 速度转化，系统内部速度是1~60°每秒 */
            /* VISCA速度等级1~64 */
            if(chn->speed_grade == 0)
            {
                pan_speed = buf[4];
                tilt_speed = buf[5];
            }
            /* 扩展速度等级1~100 */
            else
            {
                pan_speed = (uint8_t)(buf[4] * 0.6f);
                tilt_speed = (uint8_t)(buf[5] * 0.6f);
            }
            
            if(pan_speed > 60) pan_speed = 60;
            if(pan_speed < 1) pan_speed = 1;
            if(tilt_speed > 60) tilt_speed = 60;
            if(tilt_speed < 1) tilt_speed = 1;
            
            if(chn->camera_ops->pt_run == NULL) goto error;
            if(chn->camera_ops->pt_run(pan_speed, tilt_speed, pan, tilt) != CCP_OK) goto error;
            goto complete;
        }
    }
    
    error:
    return;
    
    complete:
    /* 活动提示，LED灯闪烁 */
    if(ccp_comm_ops->action_hint != NULL)
        ccp_comm_ops->action_hint();
    return;
}

/***************************************************************************************************
* Description:  Pelco P指令解析
***************************************************************************************************/
void Ccp_PelcoPCmdParser(struct ccp_chn *chn, uint8_t *buf, uint8_t length)
{
    uint16_t cmd_temp;
    uint16_t dat_temp;
    
    /* 检查地址 */
    if(((buf[1]+1)%256) != chn->addr)
        return;
    
    cmd_temp = (buf[2] << 8) | buf[3];
    dat_temp = (buf[4] << 8) | buf[5];
    
    
    /* 待机指令处理 */
    if(ccp_comm_ops->get_power_status != NULL)
    {
        if(ccp_comm_ops->get_power_status() == 0)
        {
            if(cmd_temp == PELCO_P_POWER_ON && dat_temp == 0)
            {
                if(chn->camera_ops->ctl_power == NULL) goto error;
                if(chn->camera_ops->ctl_power(VISCA_ON) != CCP_OK) goto error;
                goto complete;
            }
            else goto error;
        }
    }
    
    switch(cmd_temp)
    {
        /* 停止，Zoom Focus Pan Tilt */
        case 0:
        if(dat_temp == 0)
        {
            /* Zoom Stop */
            if(chn->camera_ops->ctl_zoom == NULL) goto error;
            if(chn->camera_ops->ctl_zoom(VISCA_ZOOM_STOP) != CCP_OK) goto error;
            
            /* Focus Stop */
            if(chn->camera_ops->ctl_focus == NULL) goto error;
            if(chn->camera_ops->ctl_focus(VISCA_FOCUS_STOP) != CCP_OK) goto error;
            
            /* Pan Tilt Stop */
            if(chn->camera_ops->pt_run == NULL) goto error;
            if(chn->camera_ops->pt_run(0, 0, 3, 3) != CCP_OK) goto error;
            
            goto complete;
        }
        break;
        
        /* 电源关 */
        case PELCO_P_POWER_OFF:
        if(dat_temp != CCP_OK) goto error;
        if(chn->camera_ops->ctl_power == NULL) goto error;
        if(chn->camera_ops->ctl_power(VISCA_OFF) != CCP_OK) goto error;
        goto complete;
        break;
        
        /* 设置预置位 */
        case PELCO_P_PRESET_SET:
        if(buf[4] != CCP_OK) goto error;
        if(chn->camera_ops->ctl_preset == NULL) goto error;
        if(chn->camera_ops->ctl_preset(VISCA_MEMORY_SET, buf[5]) != CCP_OK) goto error;
        goto complete;
        break;
        
        /* 删除预置位 */
        case PELCO_P_PRESET_CLEAR:
        if(buf[4] != CCP_OK) goto error;
        if(chn->camera_ops->ctl_preset == NULL) goto error;
        if(chn->camera_ops->ctl_preset(VISCA_MEMORY_DELETE, buf[5]) != CCP_OK) goto error;
        goto complete;
        break;
        
        /* 调用预置位 */
        case PELCO_P_PRESET_CALL:
        if(buf[4] != CCP_OK) goto error;
        if(chn->camera_ops->ctl_preset == NULL) goto error;
        if(chn->camera_ops->ctl_preset(VISCA_MEMORY_CALL, buf[5]) != CCP_OK) goto error;
        goto complete;
        break;
        
        /* 聚焦模式 */
        case PELCO_P_FOCUS_MODE:
        if(chn->camera_ops->ctl_focus_mode == NULL) goto error;
        if(dat_temp == 2) 
        {
            if(chn->camera_ops->ctl_focus_mode(VISCA_MANUAL) != CCP_OK) goto error;
        }
        else if(dat_temp == 0)
        {
            if(chn->camera_ops->ctl_focus_mode(VISCA_AUTO) != CCP_OK) goto error;
        }
        else goto error;
        goto complete;
        break;    
        
        default:
        if(cmd_temp & PELCO_P_IRIS_ADD)         //光圈加
        {
            if(chn->camera_ops->ctl_exp_mode == NULL) goto error;
            if(chn->camera_ops->ctl_exp_mode(VISCA_EXP_MODE_IRIS_PRIORITY) != CCP_OK) goto error;
            if(chn->camera_ops->ctl_exp_iris == NULL) goto error;
            if(chn->camera_ops->ctl_exp_iris(VISCA_EXP_IRIS_UP) != CCP_OK) goto error;
            goto complete;
        }
        else if(cmd_temp & PELCO_P_IRIS_DOWN)   //光圈减
        {
            if(chn->camera_ops->ctl_exp_mode == NULL) goto error;
            if(chn->camera_ops->ctl_exp_mode(VISCA_EXP_MODE_IRIS_PRIORITY) != CCP_OK) goto error;
            if(chn->camera_ops->ctl_exp_iris == NULL) goto error;
            if(chn->camera_ops->ctl_exp_iris(VISCA_EXP_IRIS_DOWM) != CCP_OK) goto error;
            goto complete;
        }
        
        if(cmd_temp & PELCO_P_FOCUS_NEAR)       //焦距 近
        {
            if(chn->camera_ops->ctl_focus_mode == NULL) goto error;
            if(chn->camera_ops->ctl_focus_mode(VISCA_FOCUS_MANUAL) != CCP_OK) goto error;
            if(chn->camera_ops->ctl_focus == NULL) goto error;
            if(chn->camera_ops->ctl_focus(VISCA_FOCUS_NEAR_STD) != CCP_OK) goto error;
            goto complete;
        }
        else if(cmd_temp & PELCO_P_FOCUS_FAR)   //焦距 远
        {
            if(chn->camera_ops->ctl_focus_mode == NULL) goto error;
            if(chn->camera_ops->ctl_focus_mode(VISCA_FOCUS_MANUAL) != CCP_OK) goto error;
            if(chn->camera_ops->ctl_focus == NULL) goto error;
            if(chn->camera_ops->ctl_focus(VISCA_FOCUS_FAR_STD) != CCP_OK) goto error;
            goto complete;
        }
        if(cmd_temp & PELCO_P_ZOOM_TELE)        //变倍 近
        {
            if(chn->camera_ops->ctl_zoom == NULL) goto error;
            if(chn->camera_ops->ctl_zoom(VISCA_ZOOM_TELE_STD) != CCP_OK) goto error;
            goto complete;
        }
        else if(cmd_temp & PELCO_P_ZOOM_WIDE)   //变倍 远
        {
            if(chn->camera_ops->ctl_zoom == NULL) goto error;
            if(chn->camera_ops->ctl_zoom(VISCA_ZOOM_WIDE_STD) != CCP_OK) goto error;
            goto complete;
        }
        
        if(cmd_temp & 0x1E)
        {
            uint8_t pan, tilt, pan_speed, tilt_speed;
            /* 左 */
            if(cmd_temp & PELCO_P_PTZ_LEFT)
            {
                pan = VISCA_PAN_LEFT; 
            }
            /* 右 */
            else if(cmd_temp & PELCO_P_PTZ_RIGHT)
            {
                pan = VISCA_PAN_RIGHT;
            }
            /* 停 */
            else
            {
                pan = VISCA_PAN_STOP;
            }
            
            /* 上 */
            if(cmd_temp & PELCO_P_PTZ_UP)
            {
                tilt = VISCA_TILT_UP; 
            }
            /* 下 */
            else if(cmd_temp & PELCO_P_PTZ_DOWN)
            {
                tilt = VISCA_TILT_DOWN;
            }
            /* 停 */
            else
            {
                tilt = VISCA_TILT_STOP;
            }
            
            /* 速度转化，系统内部速度是1~60°每秒 */
            /* VISCA速度等级1~64 */
            if(chn->speed_grade == 0)
            {
                pan_speed = buf[4];
                tilt_speed = buf[5];
            }
            /* 扩展速度等级1~100 */
            else
            {
                pan_speed = (uint8_t)(buf[4] * 0.6f);
                tilt_speed = (uint8_t)(buf[5] * 0.6f);
            }
            
            if(pan_speed > 60) pan_speed = 60;
            if(pan_speed < 1) pan_speed = 1;
            if(tilt_speed > 60) tilt_speed = 60;
            if(tilt_speed < 1) tilt_speed = 1;
            
            if(chn->camera_ops->pt_run == NULL) goto error;
            if(chn->camera_ops->pt_run(pan_speed, tilt_speed, pan, tilt) != CCP_OK) goto error;
            goto complete;
        }
    }
    
    error:
    return;
    
    complete:
    /* 活动提示，LED灯闪烁 */
    if(ccp_comm_ops->action_hint != NULL)
        ccp_comm_ops->action_hint();
    return;
}

/****************************************** END OF FILE *******************************************/
