/***************************************************************************************************
* FILE: system.c
*
* DESCRIPTION:  --
*
* CREATED: 2017/11/15, by kevin xu
***************************************************************************************************/

#include "system.h"
#include "ccp.h"
#include "cam_ops.h"
#include "visca2camera.h"


/* 运行时状态，重启复位的参数 */
RUNNING_INFO_ST stRunInfo;

/* 系统控制参数 */
SYSTEM_INFO_ST stSysInfo;

/* 图像相关的参数 */
CAMERA_INFO_ST stCamInfo;


/***************************************************************************************************
* Description: 系统时钟初始化，1ms系统时间加一
***************************************************************************************************/
void System_TimerInit(void)
{
    TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseInitStructure.TIM_Prescaler = 95;   //1M
    TIM_BaseInitStructure.TIM_Period = 1000;    //1ms
    TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

/***************************************************************************************************
* Description:  电源指示灯闪烁
***************************************************************************************************/
#define LED_BLINK_TIMES     5
static int8_t LED_Counter = 0;
static int8_t LED_PSC = 0;
void LED_Blink(void)
{
    if(LED_Counter >= 0)
        LED_Counter = LED_BLINK_TIMES;
}
void LED_SetStatus(int32_t onoff)
{
    if(onoff)
    {
        LED_Counter = 0;
        LED_POWER_ON;
    }
    else
    {
        LED_Counter = -1;
        LED_POWER_OFF;
    }
}
void LED_Handler(void)
{
    if(LED_Counter > 0)
    {
        if(++LED_PSC > 50)
        {
            LED_PSC = 0;
            
            LED_Counter-- ;
            if(LED_Counter > 0)
            {
                LED_POWER_TOGGLE;
            }
            else
            {
                LED_POWER_ON;
            }
        }
    }
}

/***************************************************************************************************
* Description:  获取电源状态，待机或者开机
***************************************************************************************************/
int32_t System_GetPowerState(void)
{
    return !stRunInfo.standby;
}

/***************************************************************************************************
* Description:  检测是否初次启动
***************************************************************************************************/
const uint8_t VirginCode[] = __DATE__;
int32_t System_CheckVirgin(void)
{
    uint8_t read_buffer[sizeof(VirginCode)];
    
    /* 读取标记 */
    if(EEPROM_Read(0, read_buffer, sizeof(VirginCode)))
        return 2;
    /* 首次启动 */
    if(memcmp(VirginCode, read_buffer, sizeof(VirginCode))) 
        return 1;
    else
        return 0;
}

/***************************************************************************************************
* Description:  设置标记
***************************************************************************************************/
int32_t System_SetVirgin(void)
{
    return EEPROM_Write(0, VirginCode, sizeof(VirginCode));//写入标记
}

/***************************************************************************************************
* Description: 从内部flash读取数据
***************************************************************************************************/
int32_t System_ReadPara(void)
{
    uint32_t checksum;
    
    /* 系统参数 */
    EEPROM_Read(SYS_INFO_SAVE_ADDR, (uint8_t *)&stSysInfo, sizeof(SYSTEM_INFO_ST));
    checksum = CheckSum((const uint8_t *)&stSysInfo, sizeof(SYSTEM_INFO_ST) - 4);
    if(stSysInfo.checksum != checksum)
        return 1;
    
    /* 图像参数 */
    EEPROM_Read(CAM_INFO_SAVE_ADDR, (uint8_t *)&stCamInfo, sizeof(CAMERA_INFO_ST));
    checksum = CheckSum((const uint8_t *)&stCamInfo, sizeof(CAMERA_INFO_ST) - 4);
    if(stCamInfo.checksum != checksum)
        return 1;
    
    return 0;
}

/***************************************************************************************************
* Description:  复位系统参数
***************************************************************************************************/
int32_t System_ResetPara(void)
{
    stSysInfo.ptc.type = CCP_AUTO;
    stSysInfo.ptc.addr = 1;
    stSysInfo.ptc.baudrate = BAUD_9600;
    stSysInfo.ptc.speed_grade = 0;
    stSysInfo.ptc.speed_acc = 3;
    stSysInfo.ptc.speed_limit = OFF;
    Ptc_Init();
    
    stSysInfo.pan_inversion = 0;
    stSysInfo.tilt_inversion = 0;
    stSysInfo.remote_speed = 8;
    stSysInfo.remote_acc = 2;
    stSysInfo.preset_speed = 8;
    stSysInfo.preset_mode = 10;
    stSysInfo.call_preset1 = OFF;
    stSysInfo.usb_mode = 1;
    stSysInfo.aging_test = OFF;
    stSysInfo.rs485 = OFF;
    stSysInfo.show_brief = OFF;
    
    System_SavePara();
    
    return 0;
}

/***************************************************************************************************
* Description: 保存系统参数
***************************************************************************************************/
int32_t System_SavePara(void)
{
    uint32_t checksum;
    
    /* 发送指令让机芯保存参数 */
    visca_custom_set();
    
    /* 保存系统设置 */
    checksum = CheckSum((const uint8_t *)&stSysInfo, sizeof(SYSTEM_INFO_ST) - 4);
    stSysInfo.checksum = checksum;
    EEPROM_Write(SYS_INFO_SAVE_ADDR, (uint8_t *)&stSysInfo, sizeof(SYSTEM_INFO_ST));
    
    /* 保存图像参数 */
    checksum = CheckSum((const uint8_t *)&stCamInfo, sizeof(CAMERA_INFO_ST) - 4);
    stCamInfo.checksum = checksum;
    EEPROM_Write(CAM_INFO_SAVE_ADDR, (uint8_t *)&stCamInfo, sizeof(CAMERA_INFO_ST));
    
    return 0;
}


int32_t System_SavePartPara(uint32_t type)
{
    SYSTEM_INFO_ST *para = NULL;
    uint32_t offset = 0;
    switch(type)
    {
        case SYS_SAVE_VIDEOFORMAT:
        offset = (uint32_t)&(para->video_format_out1);
        if(EEPROM_Write(SYS_INFO_SAVE_ADDR+offset, (uint8_t *)&stSysInfo.video_format_out1, 2))
            return 1;
        break;
        
        case SYS_SAVE_PAN_REVERSE:
        offset = (uint32_t)&(para->pan_inversion);
        if(EEPROM_Write(SYS_INFO_SAVE_ADDR+offset, (uint8_t *)&stSysInfo.pan_inversion, 1))
            return 1;
        break;
        
        case SYS_SAVE_TILT_REVERSE:
        offset = (uint32_t)&(para->tilt_inversion);
        if(EEPROM_Write(SYS_INFO_SAVE_ADDR+offset, (uint8_t *)&stSysInfo.tilt_inversion, 1))
            return 1;
        break;
        
        default:
        return 1;
        break;
    }
    
    /* 更新校验数据 */
    stSysInfo.checksum = CheckSum((const uint8_t *)&stSysInfo, sizeof(SYSTEM_INFO_ST) - 4);
    offset = (uint32_t)&(para->checksum);
    if(EEPROM_Write(SYS_INFO_SAVE_ADDR+offset, (uint8_t *)&stSysInfo.checksum, 1))
        return 1;

    return 0;
}

uint32_t CheckSum(const uint8_t *buf, uint32_t size)
{
    uint32_t checksum = 0;
    
    for(uint32_t i=0; i<size; i++)
    {
        checksum += buf[i];
    }
    return checksum;
}

/***************************************************************************************************
* Description:  检查时间间隔，大于参数值返回1，并更新对应计数器值
***************************************************************************************************/
int32_t CheckInterval(uint32_t *timer, uint32_t interval)
{
    int32_t s32Temp = *timer - stRunInfo.time;
    if( abs(s32Temp) > interval)//比较时间
    {
        *timer = stRunInfo.time;
        return 1;
    }
    else
        return 0;
}

int32_t CheckTimeDiff(uint32_t val, uint32_t diff)
{
    int32_t s32Temp = val - stRunInfo.time;
    if(abs(s32Temp) > diff) return 1;
    else return 0;
}

/***************************************************************************************************
* Description:  设置、删除、检查预置位保存情况
***************************************************************************************************/
static uint8_t PresetStatus[32] = {0};
void PresetStatusSetBit(uint32_t num)
{
    PresetStatus[num / 8] |= (1 << (num % 8));
}
void PresetStatusResetBit(uint32_t num)
{
    PresetStatus[num / 8] &= ~(1 << (num % 8));
}
/* buffer:visca指令有效数据部分起始地址， size:需要的预置位个数 */
void PresetStatusFillViscaBuffer(uint8_t *buffer)
{
    uint8_t temp;
    for(uint32_t i=0; i<32; i++)
    {
        temp = PresetStatus[i] & 0x0F;
        buffer[i*2] = temp;
        temp = (PresetStatus[i] & 0xF0) >> 4;
        buffer[i*2 + 1] = temp;
    }
}
void PresetCheckUsed(void)
{
    uint32_t save_addr;
    
    memset(&PresetStatus, 0, sizeof(PresetStatus));
    
    /* 完整预置位 */
    {
        struct preset_complete preset_t;
        memset(&preset_t, 0x55, sizeof(struct preset_complete));
        for(uint32_t i=0; i<16; i++)
        {
            /* 计算地址 */
            if(IS_PTZ_INV) save_addr = PRESET_INV_SAVE_ADDR + i * sizeof(struct preset_complete);
            else save_addr = PRESET_SAVE_ADDR + i * sizeof(struct preset_complete);
            
            /* 读取数据 */
            EEPROM_Read(save_addr, (uint8_t *)&preset_t, sizeof(struct preset_complete));
            
            /* 校验数据 */
            if(preset_t.checksum == CheckSum((const uint8_t *)&preset_t, sizeof(struct preset_complete)-4))
            {
                PresetStatusSetBit(i);
            }
        }
    }
    
    /* 简略预置位 */
    {
        struct preset preset_t;
        memset(&preset_t, 0x55, sizeof(struct preset));
        
        for(uint32_t i=0; i<240; i++)
        {
            /* 计算地址 */
            if(IS_PTZ_INV) save_addr = PRESET_EXT_INV_SAVE_ADDR + i * sizeof(struct preset);
            else  save_addr = PRESET_EXT_SAVE_ADDR + i * sizeof(struct preset);
            
            /* 读取数据 */
            EEPROM_Read(save_addr, (uint8_t *)&preset_t, sizeof(struct preset));
            
            /* 校验数据 */
            if(preset_t.checksum == CheckSum((const uint8_t *)&preset_t, sizeof(struct preset)-4))
            {
                PresetStatusSetBit(i+16);
            }
        }
    }
}

/***************************************************************************************************
* Description:  老化任务
***************************************************************************************************/
void AgingTask(void)
{
    static uint32_t Timer_Aging = 0;
    static uint32_t TimerLedBlink = 0;
    if(stSysInfo.aging_test == 0xFF)
    {
        static uint8_t flag1, flag2 = 0;
        int off = abs((int)(Timer_Aging-stRunInfo.time));
        if(off < 30000)
        {
            if(flag1 == 0)
            {
                flag1 = 1;
                cam_zoom(VISCA_ZOOM_TELE_VAR+4);
                pt_run(PAN_AGING_SPEED, TILT_AGING_SPEED, PAN_RIGHT, TILT_UP);
            }
        }
        else if(off < 60000)
        {
            if(flag2 == 0)
            {
                flag2 = 1;
                cam_zoom(VISCA_ZOOM_WIDE_VAR+4);
                pt_run(PAN_AGING_SPEED, TILT_AGING_SPEED, PAN_LEFT, TILT_DOWN);
            }
        }
        else
        {
            Timer_Aging = stRunInfo.time;
            flag1 = 0;
            flag2 = 0;
        }
        
        if(CheckInterval(&TimerLedBlink, 500))
        {
            LED_POWER_TOGGLE;
        }
    }
}

/***************************************************************************************************
* Description:  查询机芯相关参数
***************************************************************************************************/
#define INQ_DELAY() do { delay_ms(50); } while(0)
int32_t InqCamZoomFocus(void)
{
    uint8_t u8Temp;
    uint16_t u16Temp;
    
    inq_zoom_pos(&u16Temp);
    INQ_DELAY();
    inq_dzoom_mode(&u8Temp);
    INQ_DELAY();
    inq_focus_mode(&u8Temp);
    INQ_DELAY();
    inq_focus_pos(&u16Temp);
    INQ_DELAY();
    //    inq_af_sens(&u8Temp);
    return 0;
}
int32_t InqCamWb(void)
{
    uint8_t u8Temp;
    inq_wb_mode(&u8Temp);
    INQ_DELAY();
    inq_rgain(&u8Temp);
    INQ_DELAY();
    inq_bgain(&u8Temp);
    INQ_DELAY();
    return 0;
}
int32_t InqCamExposure(void)
{
    uint8_t u8Temp;
    inq_ae_mode(&u8Temp);
    INQ_DELAY();
    inq_shutter(&u8Temp);
    INQ_DELAY();
    inq_iris(&u8Temp);
    INQ_DELAY();
    inq_gain(&u8Temp);
    INQ_DELAY();
    inq_expcomp(&u8Temp);
    INQ_DELAY();
    inq_backlight(&u8Temp);
    INQ_DELAY();
    inq_highlight(&u8Temp);
    INQ_DELAY();
    return 0;
}
int32_t InqCamPicture(void)
{
    uint8_t u8Temp, u8Temp1;
    inq_sharpness(&u8Temp);
    INQ_DELAY();
    inq_gamma(&u8Temp);
    INQ_DELAY();
    inq_saturation(&u8Temp);
    INQ_DELAY();
    //    inq_hue(&u8Temp);
    inq_constrast(&u8Temp);
    INQ_DELAY();
    inq_defog(&u8Temp1, &u8Temp);
    INQ_DELAY();
    inq_2dnr(&u8Temp);
    INQ_DELAY();
    inq_3dnr(&u8Temp);
    INQ_DELAY();
    inq_flick(&u8Temp);
    INQ_DELAY();
    inq_wdr(&u8Temp);
    INQ_DELAY();
    inq_mirror(&u8Temp);
    INQ_DELAY();
    inq_flip(&u8Temp);
    INQ_DELAY();
    inq_mute(&u8Temp);
    INQ_DELAY();
    inq_freeze(&u8Temp);
    INQ_DELAY();
    return 0;
}
int32_t InqCamNet(void)
{
#if defined(EN_NETWORK)
    uint8_t u8Temp;
    uint32_t u32Temp;
    inq_dhcp(&u8Temp);
    INQ_DELAY();
    inq_net(YHW_NET_IP, &u32Temp);
    INQ_DELAY();
    inq_net(YHW_NET_MASK, &u32Temp);
    INQ_DELAY();
    inq_net(YHW_NET_GATEWAY, &u32Temp);
#endif
    return 0;
}

void InqCamInfo(void)
{
    static uint32_t zoom_timer = 0;
    static uint32_t timer = 0;
    static uint8_t index = 0;
    
    uint8_t u8Temp = 0;
    uint16_t u16Temp = 0;
#if defined(EN_NETWORK)
    uint32_t u32Temp = 0;
#endif
    
    if(stRunInfo.inq_zoom)
    {
        if(CheckInterval(&zoom_timer, 333))
        {
            if(!visca_inq_16bits_value(VISCA_ZOOM_DIRECT, &u16Temp, 1))
            {
                if(stCamInfo.zoom_pos == u16Temp)
                {
                    stRunInfo.inq_zoom = 0;
                }
                stCamInfo.zoom_pos = u16Temp;
                CalPtzSpeed();
            }
        }
    }
    
    if(stRunInfo.refresh_mask)
        return;
    
    if(!CheckTimeDiff(stRunInfo.inq_mask, 200))
        return;
    
    if(!CheckInterval(&timer, 500))
        return;
    
    switch(index)
    {
        case 0: inq_zoom_pos(&u16Temp); break;
        case 1: inq_dzoom_mode(&u8Temp); break;
        case 2: inq_focus_mode(&u8Temp); break;
        case 3: inq_focus_pos(&u16Temp); break;
        case 4: inq_wb_mode(&u8Temp); break;
        case 5: inq_rgain(&u8Temp); break;
        case 6: inq_bgain(&u8Temp); break;
        case 7: inq_ae_mode(&u8Temp); break;
        case 8: inq_shutter(&u8Temp); break;
        case 9: inq_iris(&u8Temp); break;
        case 10: inq_gain(&u8Temp); break;
        case 11: inq_expcomp(&u8Temp); break;
        case 12: inq_backlight(&u8Temp); break;
        case 13: inq_highlight(&u8Temp); break;
        case 14: inq_sharpness(&u8Temp); break;
        case 15: inq_gamma(&u8Temp); break;
        case 16: inq_saturation(&u8Temp); break;
        //        case 17: inq_hue(&u8Temp); break;
        case 18: inq_constrast(&u8Temp); break;
        case 19: inq_defog(&u8Temp, &u8Temp); break;
        case 20: inq_2dnr(&u8Temp); break;
        case 21: inq_3dnr(&u8Temp); break;
        case 22: inq_flick(&u8Temp); break;
        case 23: inq_wdr(&u8Temp); break;
        case 24: inq_mirror(&u8Temp); break;
        case 25: inq_flip(&u8Temp); break;
#if defined(EN_NETWORK)
        case 26: inq_dhcp(&u8Temp); break;
        case 27: inq_net(EXT_NET_IP, &u32Temp); break;
        case 28: inq_net(EXT_NET_MASK, &u32Temp); break;
        case 29: inq_net(EXT_NET_GATEWAY, &u32Temp); break;
#endif
        
        default: break;
    }
    
    if(++index > 29) index = 0;
}

/****************************************** END OF FILE *******************************************/
