/***************************************************************************************************
* FILE: ir_decoder_sony.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/11/21, by kevin
***************************************************************************************************/
#include "ir_sony.h"
#include "ir_cmd.h"

/* 版本信息 */
#define VER_MAJOR   1
#define VER_MINOR1  1
#define VER_MINOR2  0
#define DATE_YEAR   2019
#define DATE_MONTH  05
#define DATE_DAY    17

#define STR(s)     #s
#define VERSION(a, b, c)    STR(a) "." STR(b) "." STR(c)
#define DATE(y, m, d)       STR(y) "." STR(m) "." STR(d)

//static const char version[] = VERSION(VER_MAJOR, VER_MINOR1, VER_MINOR2);
static const char version_full[] = VERSION(VER_MAJOR, VER_MINOR1, VER_MINOR2) " " DATE(DATE_YEAR, DATE_MONTH, DATE_DAY);


/* 索尼红外码值特征值定义 */
#define SONY_IR_PIC_ADDR1           0x6A00
#define SONY_IR_PIC_ADDR2           0x6C80
#define SONY_IR_PIC_ADDR3           0x6D00
#define SONY_IR_PIC_ADDR4           0x6F00
#define SONY_IR_PIC_POWER           0x15
#define SONY_IR_PIC_ZOOM_TELE_S     0x1A
#define SONY_IR_PIC_ZOOM_WIDE_S     0x1B
#define SONY_IR_PIC_ZOOM_TELE_F     0x1C
#define SONY_IR_PIC_ZOOM_WIDE_F     0x1D
#define SONY_IR_PIC_FOCUS_AUTO      0x1E
#define SONY_IR_PIC_FOCUS_MANUAL    0x1F
#define SONY_IR_PIC_FOCUS_FAR       0x22
#define SONY_IR_PIC_FOCUS_NEAR      0x23
#define SONY_IR_PIC_BACKLIGHT       0x28
#define SONY_IR_PIC_BRIGHT_ADD      0x2E
#define SONY_IR_PIC_BRIGHT_SUB      0x2F
#define SONY_IR_PIC_F1              0x41
#define SONY_IR_PIC_F2              0x42
#define SONY_IR_PIC_F3              0x43
#define SONY_IR_PIC_F4              0x44
#define SONY_IR_PIC_F5              0x45
#define SONY_IR_PIC_F6              0x46
#define SONY_IR_PIC_F7              0x47
#define SONY_IR_PIC_F8              0x48

#define SONY_IR_PTZ_ADDR1           0x51D00
#define SONY_IR_PTZ_ADDR2           0x81D00
#define SONY_IR_PTZ_ADDR3           0x89D00
#define SONY_IR_PTZ_ADDR4           0x88D00
#define SONY_IR_PTZ_SOUT            0x14
#define SONY_IR_PTZ_MENU_OPEN       0x15
#define SONY_IR_PTZ_MENU_CLOSE      0x16
#define SONY_IR_PTZ_RESET           0x18
#define SONY_IR_PTZ_HOME            0x19
#define SONY_IR_PTZ_UP              0x1A
#define SONY_IR_PTZ_DOWN            0x1B
#define SONY_IR_PTZ_RIGHT           0x1C
#define SONY_IR_PTZ_LEFT            0x1D
#define SONY_IR_PTZ_DATASCREEN      0x68

#define SONY_IR_NUM_ADDR1           0x0A03
#define SONY_IR_NUM_ADDR2           0x0A13
#define SONY_IR_NUM_ADDR3           0x0A23
#define SONY_IR_NUM_ADDR4           0x0A33
#define SONY_IR_NUM_CALL            0x0A01
#define SONY_IR_NUM_SET             0x0A02
#define SONY_IR_NUM_RESET           0x0A03


/* 变量 */
static struct ir_decoder_ops ir_ops;

static uint8_t start_flag = 0;
static uint8_t bit_index = 0;
static uint32_t bit_buffer = 0;
static uint8_t arr_index = 0;
static uint16_t arr_buffer[7] = {0};

static uint8_t refresh = 0;
static uint32_t final_cmd = 0;


/***************************************************************************************************
* Description:  获取版本号
***************************************************************************************************/
const char* SonyIr_GetVersion(void)
{
    return version_full;
}

/***************************************************************************************************
* Description:  驱动初始化
***************************************************************************************************/
int32_t SonyIr_Init(struct ir_decoder_ops *ops)
{
    if(!ops || !ops->start_counting|| !ops->stop_counting)
    {
        return 1;
    }
    
    ir_ops.start_counting = ops->start_counting;
    ir_ops.stop_counting = ops->stop_counting;
    
    return 0;
}

/***************************************************************************************************
* Description:  获取接收到的码值
***************************************************************************************************/
uint32_t SonyIr_GetCmd(void)
{
    if(refresh)
    {
        refresh = 0;
        return final_cmd;
    }
    else
        return 0;
}

/***************************************************************************************************
* Description:  初始化参数
***************************************************************************************************/
inline static void InitPara(void)
{
    start_flag = 1;
    bit_buffer = 0;
    bit_index = 0;
}

/***************************************************************************************************
* Description:  重置参数
***************************************************************************************************/
inline static void ResetPara(void)
{
    start_flag = 0;
    bit_buffer = 0;
    bit_index = 0;
}

/***************************************************************************************************
* Description:  翻译码值
***************************************************************************************************/
inline static void TransPicCmd(uint32_t cmd)
{
    uint32_t cmd_t;
    switch(cmd & 0xFF80)
    {
        case SONY_IR_PIC_ADDR1: cmd_t = 0x01000000; break;
        case SONY_IR_PIC_ADDR2: cmd_t = 0x02000000; break;
        case SONY_IR_PIC_ADDR3: cmd_t = 0x03000000; break;
        case SONY_IR_PIC_ADDR4: cmd_t = 0x04000000; break;
        default:                cmd_t = 0;          break;
    }
    
    switch(cmd & 0x7F)
    {
        case SONY_IR_PIC_POWER:         cmd_t |= IR_CMD_POWER;          break;
        case SONY_IR_PIC_ZOOM_TELE_S:   cmd_t |= IR_CMD_ZOOM_TELE_SLOW; break;
        case SONY_IR_PIC_ZOOM_WIDE_S:   cmd_t |= IR_CMD_ZOOM_WIDE_SLOW; break;
        case SONY_IR_PIC_ZOOM_TELE_F:   cmd_t |= IR_CMD_ZOOM_TELE_FAST; break;
        case SONY_IR_PIC_ZOOM_WIDE_F:   cmd_t |= IR_CMD_ZOOM_WIDE_FAST; break;
        case SONY_IR_PIC_FOCUS_AUTO:    cmd_t |= IR_CMD_FOCUS_AUTO;     break;
        case SONY_IR_PIC_FOCUS_MANUAL:  cmd_t |= IR_CMD_FOCUS_MANUAL;   break;
        case SONY_IR_PIC_FOCUS_FAR:     cmd_t |= IR_CMD_FOCUS_FAR;      break;
        case SONY_IR_PIC_FOCUS_NEAR:    cmd_t |= IR_CMD_FOCUS_NEAR;     break;
        case SONY_IR_PIC_BACKLIGHT:     cmd_t |= IR_CMD_BACKLIGHT;      break;
        case SONY_IR_PIC_BRIGHT_ADD:    cmd_t |= IR_CMD_BRIGHT_UP;      break;
        case SONY_IR_PIC_BRIGHT_SUB:    cmd_t |= IR_CMD_BRIGHT_DOWN;    break;
        case SONY_IR_PIC_F1:            cmd_t |= IR_CMD_F1;             break;
        case SONY_IR_PIC_F2:            cmd_t |= IR_CMD_F2;             break;
        case SONY_IR_PIC_F3:            cmd_t |= IR_CMD_F3;             break;
        case SONY_IR_PIC_F4:            cmd_t |= IR_CMD_F4;             break;
        case SONY_IR_PIC_F5:            cmd_t |= IR_CMD_F5;             break;
        case SONY_IR_PIC_F6:            cmd_t |= IR_CMD_F6;             break;
        case SONY_IR_PIC_F7:            cmd_t |= IR_CMD_F7;             break;
        case SONY_IR_PIC_F8:            cmd_t |= IR_CMD_F8;             break;
        default:                        cmd_t = 0;                      break;
    }
    
    if(cmd_t)
    {
        final_cmd = cmd_t;
        refresh = 1;
    }
}

/***************************************************************************************************
* Description:  翻译码值
***************************************************************************************************/
inline static void TransPtzCmd(uint32_t cmd)
{
    uint32_t cmd_t;
    
    switch(cmd & 0xFFF00)
    {
        case SONY_IR_PTZ_ADDR1: cmd_t = 0x01000000; break;
        case SONY_IR_PTZ_ADDR2: cmd_t = 0x02000000; break;
        case SONY_IR_PTZ_ADDR3: cmd_t = 0x03000000; break;
        case SONY_IR_PTZ_ADDR4: cmd_t = 0x04000000; break;
        default:                cmd_t = 0;          break;
    }
    
    switch(cmd & 0xFF)
    {
        case SONY_IR_PTZ_SOUT:          cmd_t |= IR_CMD_S_OUT;          break;
        case SONY_IR_PTZ_MENU_OPEN:     cmd_t |= IR_CMD_MENU_OPEN;      break;
        case SONY_IR_PTZ_MENU_CLOSE:    cmd_t |= IR_CMD_MENU_CLOSE;     break;
        case SONY_IR_PTZ_RESET:         cmd_t |= IR_CMD_PTZ_RESET;      break;
        case SONY_IR_PTZ_HOME:          cmd_t |= IR_CMD_HOME;           break;
        case SONY_IR_PTZ_UP:            cmd_t |= IR_CMD_UP;             break;
        case SONY_IR_PTZ_DOWN:          cmd_t |= IR_CMD_DOWN;           break;
        case SONY_IR_PTZ_RIGHT:         cmd_t |= IR_CMD_RIGHT;          break;
        case SONY_IR_PTZ_LEFT:          cmd_t |= IR_CMD_LEFT;           break;
        case SONY_IR_PTZ_DATASCREEN:    cmd_t |= IR_CMD_DATASCREEN;     break;
        default:                        cmd_t = 0;                      break;
    }
    
    if(cmd_t)
    {
        final_cmd = cmd_t;
        refresh = 1;
    }
}

/***************************************************************************************************
* Description:  翻译码值
***************************************************************************************************/
inline static void TransNumCmd(uint32_t cmd)
{
    uint32_t cmd_t;
    
    if(cmd == 0x1203)
    {
        arr_index = 0;
        for(char i=0; i<7; i++)
            arr_buffer[i] = 0;
    }
    
    arr_buffer[arr_index] = cmd;
    
    if(++arr_index > 6)
    {
        arr_index = 0;
        
        /* 地址 */
        switch(arr_buffer[1])
        {
            case SONY_IR_NUM_ADDR1: cmd_t = 0x01000000; break;
            case SONY_IR_NUM_ADDR2: cmd_t = 0x02000000; break;
            case SONY_IR_NUM_ADDR3: cmd_t = 0x03000000; break;
            case SONY_IR_NUM_ADDR4: cmd_t = 0x04000000; break;
            default: return;
        }
        
        /* 类型 */
        switch(arr_buffer[4])
        {
            case SONY_IR_NUM_CALL:  cmd_t |= 0x0100;    break;
            case SONY_IR_NUM_SET:   cmd_t |= 0x0200;    break;
            case SONY_IR_NUM_RESET: cmd_t |= 0x0300;    break;
            default: return;
        }
        
        /* 数字 */
        cmd_t |= (arr_buffer[3] & 0x000F);
        if((cmd_t & 0xFF) < 9) 
        {
            cmd_t++;
        }
        else
        {
            cmd_t &= 0xFFFFFF00;
        }
        
        final_cmd = cmd_t;
        refresh = 1;
    }
}

/***************************************************************************************************
* Description:  译码
***************************************************************************************************/
inline static void Decode(void)
{
    if(bit_index >= 15)
    {
        if(bit_buffer > 0xD0000000)
        {
            /* 图像指令 */
            TransPicCmd(bit_buffer >> 17);
        }
        else if(bit_buffer < 0x30000000)
        {
            /* 数字指令 */
            TransNumCmd(bit_buffer>>17);
        }
        else if(bit_index >= 20)
        {
            /* 云台指令 */
            TransPtzCmd(bit_buffer >> 12);
        }
    }
}

/***************************************************************************************************
* Description:  边沿处理
***************************************************************************************************/
static uint32_t count_value;
void SonyIr_EdgeHandler(int32_t level)
{
  //  uint32_t count_value;
    
    /* 上升沿 */
    if(level)
    {
        count_value = ir_ops.stop_counting();
        
        /* 逻辑0 */
        if((start_flag == 1) && (count_value > 400) && (count_value < 800))
        {
            bit_buffer >>= 1;
            bit_index++;
            Decode();
        }
        /* 逻辑1 */
        else if((start_flag == 1) && (count_value > 1000) && (count_value < 1400))
        {
            bit_buffer >>= 1;
            bit_buffer |= 0x80000000;
            bit_index++;
            Decode();
        }
        /* 起始 */
        else if((count_value > 2000) && (count_value < 2800))
        {
            InitPara();
        }
        /* 错误 */
        else
        {
            ResetPara();
        }
    }
    /* 下降沿 */
    else
    {
        ir_ops.start_counting(0);
    }
}
/****************************************** END OF FILE *******************************************/
