/***************************************************************************************************
* FILE: cam.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/12/3, by kevin
***************************************************************************************************/
#include "ccp.h"
#include "visca.h"
#include "string.h"

#define CAM_ADDR    1
#define VISCA_INQ_WAIT_TIME 100
#define CMD_ADDR(n) (0x80 + (n))
#define RET_ADDR(n) (0x80 + ((n)<<4))

/* 查询缓存 */
static uint8_t visca_inq_buffer[16] = {0};
struct ccp_chn *cam=NULL, *cache=NULL;

extern struct ccp_ops *ccp_comm_ops;
extern struct ccp_chn *ccp_cam_p, *ccp_cache_p, *ccp_cam2_p, *ccp_cache2_p;
int32_t Ccp_GetViscaInqCmd(struct ccp_chn *ccp, struct ccp_chn *cache, uint8_t *buffer);


/***************************************************************************************************
* Description:  选择API控制的相机
***************************************************************************************************/
int32_t visca_select_camera(int32_t n)
{
    if(n == 1)
    {
        if(ccp_cam_p == NULL || ccp_cache_p == NULL)
            return 1;
        cam = ccp_cam_p;
        cache = ccp_cache_p;
    }
    else if(n == 2)
    {
        if(ccp_cam2_p == NULL || ccp_cache2_p == NULL)
            return 1;
        cam = ccp_cam2_p;
        cache = ccp_cache2_p;
    }
    else
        return 1;
    return 0;
}

/***************************************************************************************************
* Description:  获取回码
***************************************************************************************************/
int32_t visca_get_return_code(uint8_t addr)
{
    uint32_t len = Ccp_GetViscaInqCmd(cam, cache, visca_inq_buffer);
    
    if(len == 0)
        return 0;
    
    if((visca_inq_buffer[0] == RET_ADDR(addr)) && \
        //        (visca_inq_buffer[1] == 0x50) && 
        (visca_inq_buffer[len-1] == 0xFF))
    {
        return len;
    }
    else
    {
        return 0;
    }
}

/***************************************************************************************************
* Description:  清空缓存
***************************************************************************************************/
void visca_clear_return_buf(void)
{
    while(Ccp_GetViscaInqCmd(cam, cache, visca_inq_buffer));
}

/***************************************************************************************************
* Description:  发送指令后检查目标设备返回的指令
***************************************************************************************************/
int32_t visca_reply_confirm(uint8_t addr)
{
    uint8_t ack = 0;
    uint16_t  waitTime = addr * 100;
    
    while(waitTime--)
    {
        if(visca_get_return_code(addr) > 2)
        {
            if(visca_inq_buffer[0] == RET_ADDR(addr))
            {
                if(visca_inq_buffer[1] == 0x41)
                {//已接收
                    ack = 0x41;
                    break;
                }
                else if(visca_inq_buffer[1] == 0x51)
                {//已执行
                    ack = 0x51;
                    break;
                }
                else if(visca_inq_buffer[1] == 0x60)
                {//错误
                    ack = 0x60 + visca_inq_buffer[2];
                    break;
                }
                else
                {//未接收到标志码,继续循环等待
                    
                }
            }
        }

        ccp_comm_ops->delay_ms(1);
    }
    
    return ack;
}

/***************************************************************************************************
* Description:  特殊指令
***************************************************************************************************/
int32_t visca_custom_cmd(uint8_t *cmd, uint8_t size, uint8_t retry)
{
    cmd[0] = CMD_ADDR(CAM_ADDR);
    
    cam->send_packet(cmd, size);
    
    if(retry == 0) return CCP_OK;
    
    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    
    while(retry--)
    {
        cam->send_packet(cmd, size);
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  设置相机8位参数
***************************************************************************************************/
int32_t visca_set_8bits_value(uint8_t item, uint8_t value, uint8_t retry)
{
    uint8_t set_value_buf[] = {0x81, 0x01, 0x04, 0x00, 0x00, 0xFF};
    
    set_value_buf[0] = CMD_ADDR(CAM_ADDR);
    set_value_buf[3] = item;
    set_value_buf[4] = value;
    
    cam->send_packet(set_value_buf, sizeof(set_value_buf));     //发给机芯
    
    if(retry == 0) return CCP_OK;       //不重试就直接返回正确

    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    
    while(retry--)
    {
        cam->send_packet(set_value_buf, sizeof(set_value_buf));
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  设置相机16位参数
***************************************************************************************************/
int32_t visca_set_16bits_value(uint8_t item, uint16_t value, uint8_t retry)
{
    uint8_t direct_buf[] = {0x81, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    uint16_t tmp = value;
    
    direct_buf[0] = CMD_ADDR(CAM_ADDR);
    direct_buf[3] = item;
    direct_buf[4] = (tmp >> 12) & 0x0F;
    direct_buf[5] = (tmp >> 8) & 0x0F;
    direct_buf[6] = (tmp >> 4) & 0x0F;
    direct_buf[7] = (tmp >> 0) & 0x0F;
    
    cam->send_packet(direct_buf, sizeof(direct_buf));
    
    if(retry == 0) return CCP_OK;

    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    
    while(retry--)
    {
        cam->send_packet(direct_buf, sizeof(direct_buf));
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  设置相机寄存器参数
***************************************************************************************************/
int32_t visca_set_reg_value(uint8_t reg, uint8_t value, uint8_t retry)
{
    uint8_t set_reg_buf[] = {0x81, 0x01, 0x04, 0x24, 0x00, 0x00, 0x00, 0xFF};
    
    set_reg_buf[0] = CMD_ADDR(CAM_ADDR);
    set_reg_buf[4] = reg;
    set_reg_buf[5] = (value >> 4) & 0x0F;
    set_reg_buf[6] = (value >> 0) & 0x0F;
    
    cam->send_packet(set_reg_buf, sizeof(set_reg_buf));
    
    if(retry == 0) return CCP_OK;

    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    
    while(retry--)
    {
        cam->send_packet(set_reg_buf, sizeof(set_reg_buf));
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  设置相机储存
***************************************************************************************************/
int32_t visca_set_memory(uint8_t type, uint8_t value, uint8_t retry)
{
    uint8_t set_mem_buf[] = {0x81, 0x01, 0x04, 0x3F, 0x00, 0x00, 0xFF};
    
    set_mem_buf[0] = CMD_ADDR(CAM_ADDR);
    set_mem_buf[4] = type;
    set_mem_buf[5] = value;
    
    cam->send_packet(set_mem_buf, sizeof(set_mem_buf));
    
    if(retry == 0) return CCP_OK;
    
    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    
    while(retry--)
    {
        cam->send_packet(set_mem_buf, sizeof(set_mem_buf));
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}


/***************************************************************************************************
* Description:  云台上下左右
***************************************************************************************************/
int32_t visca_set_pan_tilt(uint8_t pan_sp, uint8_t tilt_sp, uint8_t pan_dir, uint8_t tilt_dir, uint8_t retry)
{
    uint8_t set_pan_tilt[] = {0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x03, 0x03, 0xFF};
    
    set_pan_tilt[0] = CMD_ADDR(CAM_ADDR);
    set_pan_tilt[4] = pan_sp;
    set_pan_tilt[5] = tilt_sp;
    set_pan_tilt[6] = pan_dir;
    set_pan_tilt[7] = tilt_dir;
    
    cam->send_packet(set_pan_tilt, sizeof(set_pan_tilt));
    
    if(retry == 0) return CCP_OK;
    
    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    
    while(retry--)
    {
        cam->send_packet(set_pan_tilt, sizeof(set_pan_tilt));
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  云台绝对位置
***************************************************************************************************/
int32_t visca_set_pan_tilt_abs(uint8_t sp, int16_t pan_pos, int16_t tilt_pos, uint8_t retry)
{
    uint8_t visca_pt_abs[] = {0x81, 0x01, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    
    visca_pt_abs[0] = CMD_ADDR(CAM_ADDR);
    visca_pt_abs[4] = sp;
    visca_pt_abs[6] = (pan_pos >> 12) & 0x0F;
    visca_pt_abs[7] = (pan_pos >> 8) & 0x0F;
    visca_pt_abs[8] = (pan_pos >> 4) & 0x0F;
    visca_pt_abs[9] = (pan_pos >> 0) & 0x0F;
    visca_pt_abs[10] = (pan_pos >> 12) & 0x0F;
    visca_pt_abs[11] = (pan_pos >> 8) & 0x0F;
    visca_pt_abs[12] = (pan_pos >> 4) & 0x0F;
    visca_pt_abs[13] = (pan_pos >> 0) & 0x0F;
    
    cam->send_packet(visca_pt_abs, sizeof(visca_pt_abs));
    
    if(retry == 0) return CCP_OK;
    
    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    
    while(retry--)
    {
        cam->send_packet(visca_pt_abs, sizeof(visca_pt_abs));
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  云台相对位置
***************************************************************************************************/
int32_t visca_set_pan_tilt_rel(uint8_t sp, int16_t pan_pos, int16_t tilt_pos, uint8_t retry)
{
    uint8_t visca_pt_abs[] = {0x81, 0x01, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    
    visca_pt_abs[0] = CMD_ADDR(CAM_ADDR);
    visca_pt_abs[4] = sp;
    visca_pt_abs[6] = (pan_pos >> 12) & 0x0F;
    visca_pt_abs[7] = (pan_pos >> 8) & 0x0F;
    visca_pt_abs[8] = (pan_pos >> 4) & 0x0F;
    visca_pt_abs[9] = (pan_pos >> 0) & 0x0F;
    visca_pt_abs[10] = (pan_pos >> 12) & 0x0F;
    visca_pt_abs[11] = (pan_pos >> 8) & 0x0F;
    visca_pt_abs[12] = (pan_pos >> 4) & 0x0F;
    visca_pt_abs[13] = (pan_pos >> 0) & 0x0F;
    
    cam->send_packet(visca_pt_abs, sizeof(visca_pt_abs));
    
    if(retry == 0) return CCP_OK;
    
    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    
    while(retry--)
    {
        cam->send_packet(visca_pt_abs, sizeof(visca_pt_abs));
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  云台回到中心点
***************************************************************************************************/
int32_t visca_set_pan_tilt_home(uint8_t retry)
{
    uint8_t set_pan_tilt_home[] = {0x81, 0x01, 0x06, 0x04, 0xFF};
    
    set_pan_tilt_home[0] = CMD_ADDR(CAM_ADDR);
    
    cam->send_packet(set_pan_tilt_home, sizeof(set_pan_tilt_home));
    
    if(retry == 0) return CCP_OK;

    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    while(retry--)
    {
        cam->send_packet(set_pan_tilt_home, sizeof(set_pan_tilt_home));
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  云台自检
***************************************************************************************************/
int32_t visca_set_pan_tilt_reset(uint8_t retry)
{
    uint8_t set_pan_tilt_reset[] = {0x81, 0x01, 0x06, 0x05, 0xFF};
    
    cam->send_packet(set_pan_tilt_reset, sizeof(set_pan_tilt_reset));
    
    if(retry == 0) return CCP_OK;
    
    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    
    while(retry--)
    {
        cam->send_packet(set_pan_tilt_reset, sizeof(set_pan_tilt_reset));
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  设置网络参数
***************************************************************************************************/
int32_t visca_set_net(uint8_t item, uint32_t value, uint8_t retry)
{
    uint8_t set_net_buf[] = {0x81, 0x01, 0x04, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    
    set_net_buf[0] = CMD_ADDR(CAM_ADDR);
    set_net_buf[3] = item;
    set_net_buf[4] = (uint8_t)((value >> 28) & 0x0F);
    set_net_buf[5] = (uint8_t)((value >> 24) & 0x0F);
    set_net_buf[6] = (uint8_t)((value >> 20) & 0x0F);
    set_net_buf[7] = (uint8_t)((value >> 16) & 0x0F);
    set_net_buf[8] = (uint8_t)((value >> 12) & 0x0F);
    set_net_buf[9] = (uint8_t)((value >> 8 ) & 0x0F);
    set_net_buf[10] = (uint8_t)((value >> 4) & 0x0F);
    set_net_buf[11] = (uint8_t)((value >> 0) & 0x0F);
    
    cam->send_packet(set_net_buf, sizeof(set_net_buf));
    
    if(retry == 0) return CCP_OK;
    
    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    
    while(retry--)
    {
        cam->send_packet(set_net_buf, sizeof(set_net_buf));
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  查询特殊指令
***************************************************************************************************/
int32_t visca_inq(uint8_t *buffer_in, uint8_t size_in, uint8_t *buffer_out, uint8_t size_out, uint8_t retry)
{
    int16_t wait_time;
    buffer_in[0] = CMD_ADDR(CAM_ADDR);
    do
    {
        wait_time = VISCA_INQ_WAIT_TIME * CAM_ADDR;
        visca_clear_return_buf();
        cam->send_packet(buffer_in, sizeof(size_in));
        
        while(--wait_time > 0)
        {
            if(visca_get_return_code(CAM_ADDR) == size_out)
            {
                memcpy(buffer_out, &visca_inq_buffer[2], size_out);
                return CCP_OK;
            }
            ccp_comm_ops->delay_ms(1);
        }
    }while(retry--);
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  查询8位参数
***************************************************************************************************/
int32_t visca_inq_8bits_value(uint8_t item, uint8_t *value, uint8_t retry)
{
    int16_t wait_time;
    uint8_t inq_value_buf[] = {0x81, 0x09, 0x04, 0x00, 0xFF};
    
    inq_value_buf[0] = CMD_ADDR(CAM_ADDR);
    inq_value_buf[3] = item;
    do
    {
        wait_time = VISCA_INQ_WAIT_TIME * CAM_ADDR;
        visca_clear_return_buf();
        cam->send_packet(inq_value_buf, sizeof(inq_value_buf));
        
        while(--wait_time > 0)
        {
            if(visca_get_return_code(CAM_ADDR) == 4)
            {
                *value = visca_inq_buffer[2];
                return CCP_OK;
            }
            ccp_comm_ops->delay_ms(1);
        }
    }while(retry--);
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  查询相机16位参数
***************************************************************************************************/
int32_t visca_inq_16bits_value(uint8_t item, uint16_t *value, uint8_t retry)
{
    int16_t wait_time;
    uint8_t inq_direct_value_buf[] = {0x81, 0x09, 0x04, 0x00, 0xFF};
    uint16_t u16Temp = 0;
    
    inq_direct_value_buf[0] = CMD_ADDR(CAM_ADDR);
    inq_direct_value_buf[3] = item;
    
    do
    {
        wait_time = VISCA_INQ_WAIT_TIME * CAM_ADDR;
        visca_clear_return_buf();
        cam->send_packet(inq_direct_value_buf, sizeof(inq_direct_value_buf));
        
        while(--wait_time > 0)
        {
            if(visca_get_return_code(CAM_ADDR) == 7)
            {
                u16Temp = visca_inq_buffer[2];
                u16Temp <<= 4;
                u16Temp |= visca_inq_buffer[3];
                u16Temp <<= 4;
                u16Temp |= visca_inq_buffer[4];
                u16Temp <<= 4;
                u16Temp |= visca_inq_buffer[5];
                *value = u16Temp;
                return CCP_OK;
            }
            ccp_comm_ops->delay_ms(1);
        }
    }while(retry--);
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  查询相机寄存器参数
***************************************************************************************************/
int32_t visca_inq_reg_value(uint8_t reg, uint8_t *value, uint8_t retry)
{
    int16_t wait_time;
    uint8_t inq_reg_buf[] = {0x81, 0x09, 0x04, 0x24, 0x00, 0xFF};
    
    inq_reg_buf[0] = CMD_ADDR(CAM_ADDR);
    inq_reg_buf[4] = reg;
    do
    {
        wait_time = VISCA_INQ_WAIT_TIME * CAM_ADDR;
        visca_clear_return_buf();
        cam->send_packet(inq_reg_buf, sizeof(inq_reg_buf));
        
        while(--wait_time > 0)
        {
            if(visca_get_return_code(CAM_ADDR) == 6)
            {
                *value = visca_inq_buffer[3];
                *value <<= 4;
                *value |= visca_inq_buffer[4];
                return CCP_OK;
            }
            ccp_comm_ops->delay_ms(1);
        }
    }while(retry--);
    return CCP_ERROR;
}

/***************************************************************************************************
* Description:  设置相机网络参数
***************************************************************************************************/
int32_t visca_inq_net(uint8_t item, uint32_t *value, uint8_t retry)
{
    int16_t wait_time;
    uint8_t inq_net_buf[] = {0x81, 0x09, 0x04, 0xAB, 0xFF};
    
    *value = 0;
    inq_net_buf[0] = CMD_ADDR(CAM_ADDR);
    inq_net_buf[3] = item;
    do
    {
        wait_time = VISCA_INQ_WAIT_TIME * CAM_ADDR;
        visca_clear_return_buf();
        cam->send_packet(inq_net_buf, sizeof(inq_net_buf));
        
        while(wait_time--)
        {
            if(visca_get_return_code(CAM_ADDR) == 11)
            {
                uint8_t i;
                for(i=2; i<10; i++)
                {
                    *value <<= 4;
                    *value |= visca_inq_buffer[i];
                }
                return CCP_OK;
            }
            ccp_comm_ops->delay_ms(1);
        }
    }while(retry--);
    return CCP_ERROR;
}

/***************************************************************************************************
* Description: 将字符转换为适合在机芯显示的格式
* Arguments:   ch 标准ascii码
* Returns:     0x1b(空格):不支持的ascii码， 转换之后的适合在机芯显示的字符值
***************************************************************************************************/
static uint8_t visca_ascii2visca(char ch)
{
    uint8_t ret = 0x1b;//默认空格
    if(ch >= '1' && ch <= '9') ret =  ch - 0x13;
    else if(ch >='A' && ch <= 'Z') ret = ch - 0x41;
    else
    {
        switch(ch)
        {
            case '0': ret = 0x27; break;
            case '&': ret = 0x1A; break;
            case ' ': ret = 0x1B; break;
            case '?': ret = 0x1C; break;
            case '!': ret = 0x1D; break;
            case ':': ret = 0x4A; break;
            case '.': ret = 0x4C; break;
            case ',': ret = 0x4D; break;
            case '/': ret = 0x4E; break;
            case '-': ret = 0x4F; break;
            default: break;
        }
    }
    return ret;
}

/***************************************************************************************************
* Description: 机芯tilte控制命令
* Arguments:   cmd：0打开，1关闭，2清除
*              line：要操作的行号， F表示所有的行
***************************************************************************************************/
int32_t visca_title_cmd(uint8_t line, uint8_t cmd, uint8_t retry)
{
    uint8_t visca_title_cmd_buf[6] = {0x81, 0x01, 0x04, 0x74, 0x00, 0xFF};
    
    visca_title_cmd_buf[4] = (cmd << 4) | line;
    
    cam->send_packet(visca_title_cmd_buf, sizeof(visca_title_cmd_buf));
    
    if(retry == 0) return CCP_OK;
    
    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    
    while(retry--)
    {
        cam->send_packet(visca_title_cmd_buf, sizeof(visca_title_cmd_buf));
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}

/***************************************************************************************************
* Description: 设置指定行号的水平开始位置，颜色，闪烁效果
* Arguments:   x 水平位置
*              y 行号
*              color 颜色
*              blink 闪烁效果
***************************************************************************************************/
int32_t visca_title_set(uint8_t line, uint8_t x, uint8_t color, uint8_t blink, uint8_t retry)
{
    uint8_t visca_title_set1[16] = {0x81, 0x01, 0x04, 0x73, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    
    visca_title_set1[4] = 0x10 | line;
    visca_title_set1[6] = x;
    visca_title_set1[7] = color;
    visca_title_set1[8] = blink;
    
    cam->send_packet(visca_title_set1, sizeof(visca_title_set1));
    
    if(retry == 0) return CCP_OK;
    
    if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    
    while(retry--)
    {
        cam->send_packet(visca_title_set1, sizeof(visca_title_set1));
        if(visca_reply_confirm(CAM_ADDR)) return CCP_OK;
    }
    return CCP_ERROR;
}

/***************************************************************************************************
* Description: 显示字符串
* Arguments:   line 行号
*              *str 字符串地址
***************************************************************************************************/
int32_t visca_title_show(uint8_t line, uint8_t *str, uint8_t retry)
{
    uint8_t visca_title_set2[16] = {0x81, 0x01, 0x04, 0x73, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    uint8_t visca_title_set3[16] = {0x81, 0x01, 0x04, 0x73, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    uint8_t i = 0;
    
    visca_title_set2[4] = 0x20 | line;//填入行数数据
    visca_title_set3[4] = 0x30 | line;
    memset(&visca_title_set2[5], 0, 10);//清空字符数据
    memset(&visca_title_set3[5], 0, 10);
    while(i < 20)  //循环填入20个字符，一行最多20个字符
    {
        if(i < 10)
        {
            visca_title_set2[i+5] = visca_ascii2visca(*str);
        }
        else if(i < 20)
        {
            visca_title_set3[i-5] = visca_ascii2visca(*str);
        }
        i++;
        str++;
    }
    
    cam->send_packet(visca_title_set2, 16);
    
    if(retry > 0) 
    {
        visca_reply_confirm(CAM_ADDR);
    }

    cam->send_packet(visca_title_set3, 16);  //发送后十个字符
    if(retry > 0) 
    {
        visca_reply_confirm(CAM_ADDR);
    }

    return CCP_OK;
}


/***************************************************************************************************
* Description:  机芯参数保存
***************************************************************************************************/
void visca_custom_reset(void)
{
    uint8_t CUSTOM_RESET_CMD[7] = {0x81, 0x01, 0x04, 0x3F, 0x00, 0x7F, 0xFF};
    CUSTOM_RESET_CMD[0] = CMD_ADDR(CAM_ADDR);
    cam->send_packet(CUSTOM_RESET_CMD, sizeof(CUSTOM_RESET_CMD));
}
void visca_custom_set(void)
{
    uint8_t CUSTOM_SET_CMD[7] = {0x81, 0x01, 0x04, 0x3F, 0x01, 0x7F, 0xFF};
    CUSTOM_SET_CMD[0] = CMD_ADDR(CAM_ADDR);
    cam->send_packet(CUSTOM_SET_CMD, sizeof(CUSTOM_SET_CMD));
}

/***************************************************************************************************
* Description: 贝莱特/海思机芯菜单操作函数
***************************************************************************************************/
static uint8_t cam_menu_buf[6] = {0x81, 0x01, 0x04, 0x7F, 0x02, 0xFF};
void cam_menu_switch(void)
{
    cam_menu_buf[0] = CMD_ADDR(CAM_ADDR);
    cam_menu_buf[3] = 0x7F;
    cam_menu_buf[4] = 0x02;
    cam->send_packet(cam_menu_buf, 6);
}
void cam_menu_up(void)
{
    cam_menu_buf[0] = CMD_ADDR(CAM_ADDR);
    cam_menu_buf[3] = 0x07;
    cam_menu_buf[4] = 0x02;
    cam->send_packet(cam_menu_buf, 6);
}
void cam_menu_down(void)
{
    cam_menu_buf[0] = CMD_ADDR(CAM_ADDR);
    cam_menu_buf[3] = 0x07;
    cam_menu_buf[4] = 0x03;
    cam->send_packet(cam_menu_buf, 6);
}
void cam_menu_left(void)
{
    cam_menu_buf[0] = CMD_ADDR(CAM_ADDR);
    cam_menu_buf[3] = 0x08;
    cam_menu_buf[4] = 0x02;
    cam->send_packet(cam_menu_buf, 6);
}
void cam_menu_right(void)
{
    cam_menu_buf[0] = CMD_ADDR(CAM_ADDR);
    cam_menu_buf[3] = 0x08;
    cam_menu_buf[4] = 0x03;
    cam->send_packet(cam_menu_buf, 6);
}


/****************************************** END OF FILE *******************************************/
