/***************************************************************************************************
* FILE: visca2camera.c
*
* DESCRIPTION:  --
*
* CREATED: 2017/11/15, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/
#include "visca_if.h"

/***************************************************************************************************
* LOCAL DEFINES
***************************************************************************************************/
#define BUILD_ADDR(addr)          (0x80 + addr)
#define GET_RETURN_ADDR(addr)   (0x80 + (addr<<4))
/***************************************************************************************************
* LOCAL GLOBAL VARIABLES
***************************************************************************************************/
//static uint8_t visca_inq_index = 0;//查询索引
static uint8_t visca_inq_buffer[16] = {0};//查询数组

static void (*visca_delay_ms)(uint32_t nms);
static void (*visca_send_cmd)(const uint8_t *cmd, uint8_t length);
static void (*visca_send_inq)(const uint8_t *cmd, uint8_t length);
static void (*visca_clear_return_buf)(void);
static int32_t (*visca_get_return_code)(uint8_t *cmd);



/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/


/***************************************************************************************************
* Description:  设置操作接口
***************************************************************************************************/
void visca_if_set_ops(const struct visca_if *vif)
{
    visca_delay_ms = vif->delay_ms;
    visca_send_cmd = vif->send_cmd;
    visca_send_inq = vif->send_inq;
    visca_clear_return_buf = vif->clear_return_buf;
    visca_get_return_code = vif->get_return_code;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t visca_parse_return_code(uint8_t addr, uint8_t length)
{
    uint32_t buffer_len;
    
    buffer_len = visca_get_return_code(visca_inq_buffer);
    
    if(buffer_len == 0)
        return 0;
    
    if((visca_inq_buffer[0] == GET_RETURN_ADDR(addr)) && \
        (visca_inq_buffer[1] == 0x50) && \
            (visca_inq_buffer[length-1] == 0xFF))
    {
        return length;
    }
    else
    {
        return 0;
    }
}

/***************************************************************************************************
* Description:  发送指令后检查目标设备返回的指令
***************************************************************************************************/
int32_t visca_reply_confirm(uint8_t waitTime)
{
    if(waitTime == 0) return 0x41;
    uint8_t ack = 0;
    
    waitTime *= 100;
    
    while(waitTime--)
    {
        if(visca_get_return_code(visca_inq_buffer) > 2)
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
        visca_delay_ms(1);
    }
    
    return ack;
}

/***************************************************************************************************
* Description:  设置相机8位参数
***************************************************************************************************/
int32_t visca_set_8bits_value(uint8_t addr, uint8_t item, uint8_t value, uint8_t retry)
{
    uint8_t set_value_buf[] = {0x81, 0x01, 0x04, 0x00, 0x00, 0xFF};
    
    set_value_buf[0] = BUILD_ADDR(addr);
    set_value_buf[3] = item;
    set_value_buf[4] = value;
    
    visca_send_cmd(set_value_buf, sizeof(set_value_buf));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(set_value_buf, sizeof(set_value_buf));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    
    return 0;
}

/***************************************************************************************************
* Description:  设置相机16位参数
***************************************************************************************************/
int32_t visca_set_16bits_value(uint8_t addr, uint8_t item, uint16_t value, uint8_t retry)
{
    uint8_t direct_buf[] = {0x81, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    uint16_t tmp = value;
    
    direct_buf[0] = BUILD_ADDR(addr);
    direct_buf[3] = item;
    direct_buf[4] = (tmp >> 12) & 0x0F;
    direct_buf[5] = (tmp >> 8) & 0x0F;
    direct_buf[6] = (tmp >> 4) & 0x0F;
    direct_buf[7] = (tmp >> 0) & 0x0F;
    
    visca_send_cmd(direct_buf, sizeof(direct_buf));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(direct_buf, sizeof(direct_buf));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  设置相机寄存器参数
***************************************************************************************************/
int32_t visca_set_reg_value(uint8_t addr, uint8_t reg, uint8_t value, uint8_t retry)
{
    uint8_t set_reg_buf[] = {0x81, 0x01, 0x04, 0x24, 0x00, 0x00, 0x00, 0xFF};
    
    set_reg_buf[0] = BUILD_ADDR(addr);
    set_reg_buf[4] = reg;
    set_reg_buf[5] = (value >> 4) & 0x0F;
    set_reg_buf[6] = (value >> 0) & 0x0F;
    
    visca_send_cmd(set_reg_buf, sizeof(set_reg_buf));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(set_reg_buf, sizeof(set_reg_buf));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  设置相机储存
***************************************************************************************************/
int32_t visca_set_memory(uint8_t addr, uint8_t type, uint8_t value, uint8_t retry)
{
    uint8_t set_mem_buf[] = {0x81, 0x01, 0x04, 0x3F, 0x00, 0x00, 0xFF};
    
    set_mem_buf[0] = BUILD_ADDR(addr);
    set_mem_buf[4] = type;
    set_mem_buf[5] = value;
    
    visca_send_cmd(set_mem_buf, sizeof(set_mem_buf));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(set_mem_buf, sizeof(set_mem_buf));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  TALLY控制
***************************************************************************************************/
int32_t visca_set_tally(uint8_t addr, uint8_t onoff, uint8_t retry)
{
    uint8_t set_tally_buf[] = {0x81, 0x01, 0x7E, 0x01, 0x0A, 0x00, 0x00, 0xFF};
    set_tally_buf[0] = BUILD_ADDR(addr);
    set_tally_buf[6] = onoff;
    
    visca_send_cmd(set_tally_buf, sizeof(set_tally_buf));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(set_tally_buf, sizeof(set_tally_buf));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  云台上下左右
***************************************************************************************************/
int32_t visca_set_pan_tilt(uint8_t addr, uint8_t pan_sp, uint8_t tilt_sp, uint8_t pan_dir, uint8_t tilt_dir, uint8_t retry)
{
    uint8_t set_pan_tilt[] = {0x81, 0x01, 0x06, 0x01, 0x00, 0x00, 0x03, 0x03, 0xFF};
    
    set_pan_tilt[0] = BUILD_ADDR(addr);
    set_pan_tilt[4] = pan_sp;
    set_pan_tilt[5] = tilt_sp;
    set_pan_tilt[6] = pan_dir;
    set_pan_tilt[7] = tilt_dir;
    
    visca_send_cmd(set_pan_tilt, sizeof(set_pan_tilt));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(set_pan_tilt, sizeof(set_pan_tilt));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  云台绝对位置
***************************************************************************************************/
int32_t visca_set_pan_tilt_abs(uint8_t addr, uint8_t sp, int16_t pan_pos, int16_t tilt_pos, uint8_t retry)
{
    uint8_t visca_pt_abs[] = {0x81, 0x01, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    
    visca_pt_abs[0] = BUILD_ADDR(addr);
    visca_pt_abs[4] = sp;
    visca_pt_abs[6] = (pan_pos >> 12) & 0x0F;
    visca_pt_abs[7] = (pan_pos >> 8) & 0x0F;
    visca_pt_abs[8] = (pan_pos >> 4) & 0x0F;
    visca_pt_abs[9] = (pan_pos >> 0) & 0x0F;
    visca_pt_abs[10] = (pan_pos >> 12) & 0x0F;
    visca_pt_abs[11] = (pan_pos >> 8) & 0x0F;
    visca_pt_abs[12] = (pan_pos >> 4) & 0x0F;
    visca_pt_abs[13] = (pan_pos >> 0) & 0x0F;
    
    visca_send_cmd(visca_pt_abs, sizeof(visca_pt_abs));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(visca_pt_abs, sizeof(visca_pt_abs));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  云台相对位置
***************************************************************************************************/
int32_t visca_set_pan_tilt_rel(uint8_t addr, uint8_t sp, int16_t pan_pos, int16_t tilt_pos, uint8_t retry)
{
    uint8_t visca_pt_abs[] = {0x81, 0x01, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    
    visca_pt_abs[0] = BUILD_ADDR(addr);
    visca_pt_abs[4] = sp;
    visca_pt_abs[6] = (pan_pos >> 12) & 0x0F;
    visca_pt_abs[7] = (pan_pos >> 8) & 0x0F;
    visca_pt_abs[8] = (pan_pos >> 4) & 0x0F;
    visca_pt_abs[9] = (pan_pos >> 0) & 0x0F;
    visca_pt_abs[10] = (pan_pos >> 12) & 0x0F;
    visca_pt_abs[11] = (pan_pos >> 8) & 0x0F;
    visca_pt_abs[12] = (pan_pos >> 4) & 0x0F;
    visca_pt_abs[13] = (pan_pos >> 0) & 0x0F;
    
    visca_send_cmd(visca_pt_abs, sizeof(visca_pt_abs));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(visca_pt_abs, sizeof(visca_pt_abs));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  云台回到中心点
***************************************************************************************************/
int32_t visca_set_pan_tilt_home(uint8_t addr, uint8_t retry)
{
    uint8_t set_pan_tilt_home[] = {0x81, 0x01, 0x06, 0x04, 0xFF};
    
    set_pan_tilt_home[0] = BUILD_ADDR(addr);
    
    visca_send_cmd(set_pan_tilt_home, sizeof(set_pan_tilt_home));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(set_pan_tilt_home, sizeof(set_pan_tilt_home));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  云台自检
***************************************************************************************************/
int32_t visca_set_pan_tilt_reset(uint8_t addr, uint8_t retry)
{
    uint8_t set_pan_tilt_reset[] = {0x81, 0x01, 0x06, 0x05, 0xFF};
    
    visca_send_cmd(set_pan_tilt_reset, sizeof(set_pan_tilt_reset));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(set_pan_tilt_reset, sizeof(set_pan_tilt_reset));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  设置菜单
***************************************************************************************************/
int32_t visca_set_osd(uint8_t addr, uint8_t act, uint8_t retry)
{
    uint8_t set_osd_buf[] = {0x81, 0x01, 0x06, 0x06, 0x00, 0xFF};
    
    set_osd_buf[0] = BUILD_ADDR(addr);
    set_osd_buf[4] = act;
    
    visca_send_cmd(set_osd_buf, sizeof(set_osd_buf));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(set_osd_buf, sizeof(set_osd_buf));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  查询菜单状态
***************************************************************************************************/
int32_t visca_inq_osd(uint8_t addr, uint8_t *value, uint8_t retry)
{
    int16_t wait_time;
    uint8_t inq_osd_buf[] = {0x81, 0x09, 0x06, 0x06, 0xFF};
    
    inq_osd_buf[0] = BUILD_ADDR(addr);

    do
    {
        wait_time = VISCA_INQ_WAIT_TIME * addr;
        visca_clear_return_buf();
        visca_send_inq(inq_osd_buf, sizeof(inq_osd_buf));
        
        while(--wait_time > 0)
        {
            if(visca_parse_return_code(addr, 4))
            {
                *value = visca_inq_buffer[2];
                return 1;
            }
            visca_delay_ms(1);
        }
    }while(retry--);
    return 0;
}

/***************************************************************************************************
* Description:  JVC相机OSDEnter
***************************************************************************************************/
int32_t visca_osd_enter(uint8_t addr, uint8_t retry)
{
    uint8_t jvc_osd[] = {0x81, 0x01, 0x7E, 0x01, 0x02, 0x00, 0x01, 0xFF};
    
    jvc_osd[0] = BUILD_ADDR(addr);

    visca_send_cmd(jvc_osd, sizeof(jvc_osd));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(jvc_osd, sizeof(jvc_osd));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  设置网络参数
***************************************************************************************************/
int32_t visca_set_net(uint8_t addr, uint8_t item, uint32_t value, uint8_t retry)
{
    uint8_t set_net_buf[] = {0x81, 0x01, 0x04, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    
    set_net_buf[0] = BUILD_ADDR(addr);
    set_net_buf[3] = item;
    set_net_buf[4] = (uint8_t)((value >> 28) & 0x0F);
    set_net_buf[5] = (uint8_t)((value >> 24) & 0x0F);
    set_net_buf[6] = (uint8_t)((value >> 20) & 0x0F);
    set_net_buf[7] = (uint8_t)((value >> 16) & 0x0F);
    set_net_buf[8] = (uint8_t)((value >> 12) & 0x0F);
    set_net_buf[9] = (uint8_t)((value >> 8 ) & 0x0F);
    set_net_buf[10] = (uint8_t)((value >> 4) & 0x0F);
    set_net_buf[11] = (uint8_t)((value >> 0) & 0x0F);
    
    visca_send_cmd(set_net_buf, sizeof(set_net_buf));
    
    if(retry == 0)
    {
        return 1;
    }
    
    if(visca_reply_confirm(addr))
        return 1;
    while(--retry)
    {
        visca_send_cmd(set_net_buf, sizeof(set_net_buf));
        if(visca_reply_confirm(addr))
        {
            return 1;
        }
    }
    return 0;
}

/***************************************************************************************************
* Description:  查询8位参数
***************************************************************************************************/
int32_t visca_inq_8bits_value(uint8_t addr, uint8_t item, uint8_t *value, uint8_t retry)
{
    int16_t wait_time;
    uint8_t inq_value_buf[] = {0x81, 0x09, 0x04, 0x00, 0xFF};
    
    inq_value_buf[0] = BUILD_ADDR(addr);
    inq_value_buf[3] = item;
    do
    {
        wait_time = VISCA_INQ_WAIT_TIME * addr;
        visca_clear_return_buf();
        visca_send_inq(inq_value_buf, sizeof(inq_value_buf));
        
        while(--wait_time > 0)
        {
            if(visca_parse_return_code(addr, 4))
            {
                *value = visca_inq_buffer[2];
                return 1;
            }
            visca_delay_ms(1);
        }
    }while(retry--);
    return 0;
}

/***************************************************************************************************
* Description:  查询相机16位参数
***************************************************************************************************/
int32_t visca_inq_halfword_value(uint8_t addr, uint8_t item, uint16_t *value, uint8_t retry)
{
    int16_t wait_time;
    uint8_t inq_direct_value_buf[] = {0x81, 0x09, 0x04, 0x00, 0xFF};
    uint16_t u16Temp = 0;
    
    inq_direct_value_buf[0] = BUILD_ADDR(addr);
    inq_direct_value_buf[3] = item;
    
    do
    {
        wait_time = VISCA_INQ_WAIT_TIME * addr;
        visca_clear_return_buf();
        visca_send_inq(inq_direct_value_buf, sizeof(inq_direct_value_buf));
        
        while(--wait_time > 0)
        {
            if(visca_parse_return_code(addr, 7))
            {
                u16Temp = visca_inq_buffer[2];
                u16Temp <<= 4;
                u16Temp |= visca_inq_buffer[3];
                u16Temp <<= 4;
                u16Temp |= visca_inq_buffer[4];
                u16Temp <<= 4;
                u16Temp |= visca_inq_buffer[5];
                *value = u16Temp;
                return 1;
            }
            visca_delay_ms(1);
        }
    }while(retry--);
    return 0;
}

/***************************************************************************************************
* Description:  查询相机寄存器参数
***************************************************************************************************/
int32_t visca_inq_reg_value(uint8_t addr, uint8_t reg, uint8_t *value, uint8_t retry)
{
    int16_t wait_time;
    uint8_t inq_reg_buf[] = {0x81, 0x09, 0x04, 0x24, 0x00, 0xFF};
    
    inq_reg_buf[0] = BUILD_ADDR(addr);
    inq_reg_buf[4] = reg;
    do
    {
        wait_time = VISCA_INQ_WAIT_TIME * addr;
        visca_clear_return_buf();
        visca_send_inq(inq_reg_buf, sizeof(inq_reg_buf));
        
        while(--wait_time > 0)
        {
            if(visca_parse_return_code(addr, 6))
            {
                *value = visca_inq_buffer[3];
                *value <<= 4;
                *value |= visca_inq_buffer[4];
                return 1;
            }
            visca_delay_ms(1);
        }
    }while(retry--);
    return 0;
}

/***************************************************************************************************
* Description:  设置相机网络参数
***************************************************************************************************/
int32_t visca_inq_net(uint8_t addr, uint8_t item, uint32_t *value, uint8_t retry)
{
    int16_t wait_time;
    uint8_t inq_net_buf[] = {0x81, 0x09, 0x04, 0xAB, 0xFF};
    
    *value = 0;
    inq_net_buf[0] = BUILD_ADDR(addr);
    inq_net_buf[3] = item;
    do
    {
        wait_time = VISCA_INQ_WAIT_TIME * addr;
        visca_clear_return_buf();
        visca_send_inq(inq_net_buf, sizeof(inq_net_buf));
        
        while(wait_time--)
        {
            if(visca_parse_return_code(addr, 11))
            {
                uint8_t i;
                for(i=2; i<10; i++)
                {
                    *value <<= 4;
                    *value |= visca_inq_buffer[i];
                }
                return 1;
            }
            visca_delay_ms(1);
        }
    }while(retry--);
    return 0;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
const uint8_t cmdIfClear[] = {0x88, 0x01, 0x00, 0x01, 0xFF};
const uint8_t cmdRresetAddr[] = {0x88, 0x30, 0x01, 0xFF};
int32_t visca_reset_address(uint32_t waittime)
{
    uint32_t overtime = 0;
    /* 发送级联指令 */
    visca_send_cmd(cmdIfClear, sizeof(cmdIfClear));
    visca_send_cmd(cmdRresetAddr, sizeof(cmdRresetAddr));
    
    while(1)
    {
        visca_delay_ms(1);
        if(++overtime > waittime)
        {
            return 0;
        }
        
        if(visca_get_return_code(visca_inq_buffer))
        {
            if((visca_inq_buffer[0] == 0x88) && \
                (visca_inq_buffer[1] == 0x30) && \
                    (visca_inq_buffer[3] == 0xFF))
            {
                return (visca_inq_buffer[2] - 1);
            }
        }
    }
}

/***************************************************************************************************
* Description: 将字符准换为适合在机芯显示的格式
* Arguments:   ch 标准ascii码
* Returns:     0x1b(空格):不支持的ascii码， 转换之后的适合在机芯显示的字符值
***************************************************************************************************/
static uint8_t visca_ascii2visca(char ch)
{
    uint8_t ret = 0x1b;//默认空格
    if(ch >= '1' && ch <= '9') ret =  ch - 0x13;
    else if(ch >='A' && ch <= 'Z') ret = ch - 0x41;
    else
        switch(ch)
        {
            case '0': ret = 0x27; break;
            case ' ': ret = 0x1b; break;
            case '?': ret = 0x1C; break;
            case '!': ret = 0x1D; break;
            case ':': ret = 0x4A; break;
            case '.': ret = 0x4C; break;
            case ',': ret = 0x4D; break;
            case '/': ret = 0x4E; break;
            case '-': ret = 0x4F; break;
            default: break;
        }
    return ret;
}

/***************************************************************************************************
* Description: 机芯tilte控制命令
* Arguments:   cmd：0打开，1关闭，2清除
*              line：要操作的行号， F表示所有的行
* Returns:     0成功 1失败
***************************************************************************************************/
uint8_t visca_title_cmd(uint8_t line, uint8_t cmd)
{
    static uint8_t visca_title_cmd_buf[6] = {0x81, 0x01, 0x04, 0x74, 0x00, 0xFF};
    if(cmd == TITLE_OPEN) //打开
    {
        visca_title_cmd_buf[4] = 0x20 | line;
    }
    else if(cmd == TITLE_CLOSE)  //关闭
    {
        visca_title_cmd_buf[4] = 0x30 | line;
    }
    else if(cmd == TITLE_CLEAR)  //清除
    {
        visca_title_cmd_buf[4] = 0x10 | line;
    }
    else
    {
        return 1;   //未知命令
    }
    visca_send_cmd(visca_title_cmd_buf, sizeof(visca_title_cmd_buf));
    if(visca_reply_confirm(100) != VISCA_COMPLETION)
    {
        //没有接收到指令完成的回码
        return 1;
    }
    return 0;
}

/***************************************************************************************************
* Description: 设置指定行号的水平开始位置，颜色，闪烁效果
* Arguments:   x 水平位置
*              y 行号
*              color 颜色
*              blink 闪烁效果
* Returns:     none
***************************************************************************************************/
uint8_t visca_title_set(uint8_t line, uint8_t x, uint8_t color, uint8_t blink)
{
    static uint8_t visca_title_set1[16] = {0x81, 0x01, 0x04, 0x73, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    
    visca_title_set1[4] = 0x10 | line;
    visca_title_set1[6] = x;
    visca_title_set1[7] = color;
    visca_title_set1[8] = blink;
    visca_send_cmd(visca_title_set1, sizeof(visca_title_set1));
    
    if(visca_reply_confirm(100) != VISCA_COMPLETION)
    {
        //没有接收到指令完成的回码
        return 1;
    }
    return 0;
}

/***************************************************************************************************
* Description: 显示字符串
* Arguments:   line 行号
*              *str 字符串地址
* Returns:     1失败 0成功
***************************************************************************************************/
uint8_t visca_title_show(uint8_t line, uint8_t *str)
{
    static uint8_t visca_title_set2[16] = {0x81, 0x01, 0x04, 0x73, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    static uint8_t visca_title_set3[16] = {0x81, 0x01, 0x04, 0x73, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
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
    visca_send_cmd(visca_title_set2, 16);  //发送前十个字符
    
    //    if(visca_reply_confirm(100) != VISCA_COMPLETION)
    //    {
    //        //没有接收到指令完成的回码
    //        return 1;
    //    }
    //    visca_delay_ms(17);
    visca_send_cmd(visca_title_set3, 16);  //发送后十个字符
    
    //    if(visca_reply_confirm(100) != VISCA_COMPLETION)
    //    {
    //        //没有接收到指令完成的回码
    //        return 1;
    //    }
    //    visca_delay_ms(17);
    return 0;
}

void visca_custom_reset(void)
{
    static uint8_t BLT_CUSTOM_RESET_CMD[7] = {0x81, 0x01, 0x04, 0x3F, 0x00, 0x7F, 0xFF};
    visca_send_cmd(BLT_CUSTOM_RESET_CMD, 7);
}
void visca_custom_set(void)
{
    static uint8_t BLT_CUSTOM_SET_CMD[7] = {0x81, 0x01, 0x04, 0x3F, 0x01, 0x7F, 0xFF};
    visca_send_cmd(BLT_CUSTOM_SET_CMD, 7);
}


/****************************************** END OF FILE *******************************************/
