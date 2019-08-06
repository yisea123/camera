/***************************************************************************************************
* FILE: system.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/21, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "system.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/
RunInfo_t stRunInfo;
SysPara_t stSysPara;

uint32_t  ViscaIpCustomIpList[50] = {0};
struct address_arr ViscaIpCustomAddr;

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* Description:  读取芯片ID
***************************************************************************************************/
uint32_t* ReadChipID(void)
{
    static uint32_t CHIP_ID[3] = {0};

#define STM32F2
    
#if defined(STM32F1)
    CHIP_ID[0]= *(volatile uint32_t *)(0x1ffff7e8);
	CHIP_ID[1]= *(volatile uint32_t *)(0x1ffff7ec);
	CHIP_ID[2]= *(volatile uint32_t *)(0x1ffff7f0);    
#elif defined(STM32F2) || defined(STM32F4)
    CHIP_ID[0]= *(volatile uint32_t *)(0x1fff7a10);
	CHIP_ID[1]= *(volatile uint32_t *)(0x1fff7a14);
	CHIP_ID[2]= *(volatile uint32_t *)(0x1fff7a18);
#endif

    return CHIP_ID;
}

/***************************************************************************************************
* Description:  检查时间间隔，大于参数值返回1，并更新对应计数器值
***************************************************************************************************/
int32_t CheckInterval(uint32_t *timer, uint32_t interval)
{
    int32_t s32Temp = *timer - HAL_GetTick();
    if( abs(s32Temp) > interval)//比较时间
    {
        *timer = HAL_GetTick();
        return 1;
    }
    else
    {
        return 0;
    }
}

int32_t CheckTimeDiff(uint32_t val, uint32_t diff)
{
    int32_t s32Temp = val - HAL_GetTick();
    if(abs(s32Temp) > diff) return 1;
    else return 0;
}

/***************************************************************************************************
* Description:  IP地址字符串转换为16进制数
***************************************************************************************************/
int32_t StringToIpAddr(const char *str, uint32_t *addr)
{
    char str_full[16] = {'\0'};
    char substr[4][4] = {' '};
    
    int32_t i=0, j=3;
    
    uint32_t addr_tmp = 0;
    
    strncpy(str_full, str, 15);
    
    for(char *ch = str_full; (((*ch >= '0') && (*ch <= '9')) ||  *ch == '.'); ch++)
    {
        if(*ch == '.')
        {
            addr_tmp <<= 8;
            addr_tmp |= atoi(substr[j]);
            j--;
            i = 0;
        }
        else
        {
            substr[j][i++] = *ch;
        }
    }
    addr_tmp <<= 8;
    addr_tmp |= atoi(substr[j]);
    
    *addr = addr_tmp;
    
    return 1;
}

/***************************************************************************************************
* Description:  IP地址转字符串
***************************************************************************************************/
int32_t IpAddrToString(char *str, uint32_t addr)
{
    if(str == NULL)
    {
        return ERR_NULL_PTR;
    }
    
    sprintf(str, "%d.%d.%d.%d", (uint8_t)(addr>>24), (uint8_t)(addr>>16), (uint8_t)(addr>>8), (uint8_t)(addr));
    return ERR_NO;
}

/***************************************************************************************************
* Description:  转换IP地址高地字节
***************************************************************************************************/
uint32_t SwapIp(uint32_t addr)
{
    uint32_t tmp;
    
    tmp = (addr & 0x000000FF) << 24;
    tmp |= (addr & 0x0000FF00) << 8;
    tmp |= (addr & 0x00FF0000) >> 8;
    tmp |= (addr & 0xFF000000) >> 24;
    
    return tmp;
}

/***************************************************************************************************
* Description:  读取系统参数
***************************************************************************************************/
int32_t System_ReadPara(void)
{
    SysPara_t tmp;

    if(EEPROM_Read(PARA_ADDR_SYSTEM, (uint8_t *)&tmp, sizeof(SysPara_t))) 
        return 1; //读取失败,不复制数据
    else 
        memcpy(&stSysPara, &tmp, sizeof(SysPara_t));
    
    /* 读取VISCA OVER IP自定义地址 */
    ViscaIpCustomAddr.arr = ViscaIpCustomIpList;
    if(EEPROM_Read(VISCA_IP_ADDR, (uint8_t *)&ViscaIpCustomAddr, 4))
        return 1;
    if(EEPROM_Read(VISCA_IP_ADDR+4, (uint8_t *)ViscaIpCustomAddr.arr, ViscaIpCustomAddr.length*4))
        return 1;
    
    return 0;
}

/***************************************************************************************************
* Description:  保存系统参数
***************************************************************************************************/
int32_t System_SavePara(int32_t arg)
{
    SysPara_t *para = NULL;
    uint32_t offset;
    
    switch(arg)
    {
        case SYS_SAVE_ALL:     
        if(EEPROM_Write(PARA_ADDR_SYSTEM, (uint8_t *)&stSysPara, sizeof(SysPara_t)))
            return 1;
        
        break;
        case SYS_SAVE_VISCAIP_ADDR:
        offset = (uint32_t)&(para->ptc.visca_ip.cam_addr);
        if(EEPROM_Write(PARA_ADDR_SYSTEM + offset, (uint8_t *)&stSysPara.ptc.visca_ip.cam_addr, 4))
            return 1;
        break;
        
        case SYS_SAVE_VISCA_ADDR:
        offset = (uint32_t)&(para->ptc.visca.cam_addr);
        if(EEPROM_Write(PARA_ADDR_SYSTEM + offset, (uint8_t *)&stSysPara.ptc.visca.cam_addr, 4))
            return 1;
        break;
        
        case SYS_SAVE_SPEED_GRADE:
        offset = (uint32_t)&(para->speed_grade);
        if(EEPROM_Write(PARA_ADDR_SYSTEM + offset, (uint8_t *)&stSysPara.speed_grade, 4))
            return 1;
        break;
        
        case SYS_SAVE_PELCOD_ADDR:
        offset = (uint32_t)&(para->ptc.pelco_d.cam_addr);
        if(EEPROM_Write(PARA_ADDR_SYSTEM + offset, (uint8_t *)&stSysPara.ptc.pelco_d.cam_addr, 4))
            return 1;
        break;
        
        case SYS_SAVE_PELCOP_ADDR:
        offset = (uint32_t)&(para->ptc.pelco_p.cam_addr);
        if(EEPROM_Write(PARA_ADDR_SYSTEM + offset, (uint8_t *)&stSysPara.ptc.pelco_p.cam_addr, 4))
            return 1;
        break;
        
        case SYS_SAVE_VISCA_IP:
        if(EEPROM_Write(VISCA_IP_ADDR, (uint8_t *)&ViscaIpCustomAddr, 4))
            return 1;
        if(EEPROM_Write(VISCA_IP_ADDR+4, (uint8_t *)ViscaIpCustomAddr.arr, ViscaIpCustomAddr.length*4))
            return 1;
        break;
        default:
        break;
    }
    return 0;
}

/***************************************************************************************************
* Description:  检查当前版本程序是否为第一次启动, 以程序编译时期为标记
***************************************************************************************************/
int32_t System_CheckVirgin(void)
{
    uint8_t read_buffer[sizeof(BUILD_TIME)];
    uint8_t length = sizeof(BUILD_TIME);
    
    /* 从预先定义的位置读取标记 */
    EEPROM_Read(PARA_ADDR_VIRGIN, read_buffer, length);
    
    if(memcmp(BUILD_TIME, read_buffer, length))
    {
        /* 第一次启动, 写入标记 */
        EEPROM_Write(PARA_ADDR_VIRGIN, BUILD_TIME, length);
        return 1;
    }
    else
    {
        /* 不是第一次启动 */
        return 0;
    }
}

/***************************************************************************************************
* Description:  重设系统参数
***************************************************************************************************/
int32_t System_ResetPara(void)
{
    memset(&stSysPara, 0, sizeof(SysPara_t));

    stSysPara.ptc.type = VISCA_IP;
    stSysPara.ptc.visca_ip.cam_addr = 0xFFFFFFFF;
    stSysPara.ptc.visca.cam_addr = 1;
    stSysPara.ptc.pelco_d.cam_addr = 1;
    stSysPara.ptc.pelco_p.cam_addr = 1;
    
    stSysPara.net.dhcp = 1;
    
    stSysPara.serial.rs232_baudrate = 2;
    stSysPara.serial.rs422_baudrate = 2;
    
    stSysPara.sleep_time = 1;
    stSysPara.language = 0;
    stSysPara.speed_grade = 8;
    
    System_SavePara(SYS_SAVE_ALL);
    
    ViscaIpCustomAddr.length = 0;
    ViscaIpCustomAddr.arr = ViscaIpCustomIpList;
    EEPROM_Write(VISCA_IP_ADDR, (uint8_t *)&ViscaIpCustomAddr, 4);
    
    return 0;
}



/***************************************************************************************************
* Description:  用户输入信息时更新最后动作时间
***************************************************************************************************/
static uint32_t idle_timer = 0;
void System_UpdateIdleTimer(void)
{
    stRunInfo.idle = 0;
    idle_timer = HAL_GetTick();
}

/***************************************************************************************************
* Description:  检查是否超过设定的休眠时间
***************************************************************************************************/
const uint32_t SleepDuration[] = {60000, 300000, 1800000, 0};
int32_t System_CheckIdle(void)
{
    int ret = 0;
    uint32_t duration;
    
    if(stSysPara.sleep_time > 2)
    {
        ret = 0;
    }
    else
    {
        duration = SleepDuration[stSysPara.sleep_time];
        if(CheckTimeDiff(idle_timer, duration))
        {
            stRunInfo.idle = 1;
            ret = 1;
        }
    }
    
    return ret;       
}








/****************************************** END OF FILE *******************************************/
