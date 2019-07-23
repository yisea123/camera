/***************************************************************************************************
* FILE: system.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/21, by kevin
***************************************************************************************************/
#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include "bsp.h"

#include "sys_def.h"

#include "os.h"

#include "rocker.h"
#include "led.h"
#include "ui.h"
#include "net.h"

#include "work.h"
#include "xmenu.h"
#include "ptc.h"


/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/* 参数保存位置 */
#define PARA_ADDR_VIRGIN        1
#define PARA_ADDR_SYSTEM        1024
#define VISCA_IP_ADDR           2048

/* 参数保存范围 */
#define SYS_SAVE_ALL            0
#define SYS_SAVE_VISCAIP_ADDR   1
#define SYS_SAVE_VISCA_ADDR     2
#define SYS_SAVE_SPEED_GRADE    3
#define SYS_SAVE_PELCOD_ADDR    4 
#define SYS_SAVE_PELCOP_ADDR    5
#define SYS_SAVE_VISCA_IP       6

#define SYS_LOG_EN
#define LOG_POSITION_EN

#ifdef LOG_POSITION_EN
#define SYS_LOG_SHOW_POSITION   ;printf(" File:%s Line:%d\r\n", __FILE__, __LINE__)
#else
#define SYS_LOG_SHOW_POSITION   ;printf("\r\n")
#endif

#ifdef SYS_LOG_EN
#define SYS_LOG(content)    printf(content) SYS_LOG_SHOW_POSITION
#else
#define SYS_LOG(content)  
#endif
/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/
extern SysPara_t stSysPara;
extern RunInfo_t stRunInfo;
extern struct address_arr ViscaIpCustomAddr;
/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
uint32_t* ReadChipID(void);

int32_t CheckInterval(uint32_t *timer, uint32_t interval);
int32_t CheckTimeDiff(uint32_t val, uint32_t diff);

int32_t System_ReadPara(void);
int32_t System_SavePara(int32_t arg);
int32_t System_CheckVirgin(void);
int32_t System_ResetPara(void);
void System_UpdateIdleTimer(void);
int32_t System_CheckIdle(void);

int32_t StringToIpAddr(const char *str, uint32_t *addr);
int32_t IpAddrToString(char *str, uint32_t addr);
uint32_t SwapIp(uint32_t addr);


#endif
/****************************************** END OF FILE *******************************************/
