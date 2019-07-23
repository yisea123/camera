/***************************************************************************************************
* FILE: sys_def.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/30, by kevin
***************************************************************************************************/
#ifndef __SYS_DEF_H_
#define __SYS_DEF_H_

#include "stdint.h"

#include "rocker.h"


/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/* 设备信息 */
#define HARDWARE_VERSION    "Hardware Version: V1.1"
#define SOFTWARE_VERSION    "Software Version: V0.3.3"
#define BUILD_TIME          "Compile Date: " __DATE__

#define ERR_SUCCESS         1
#define ERR_NO              0
#define ERR_MEM             -1
#define ERR_NULL_PTR        -2


#ifndef OFF
#define OFF  0
#endif
#ifndef ON
#define ON  1
#endif


/* VISCA Over IP 兼容性代码 */


/* VISCA 协议兼容性代码 */
#define VISCA_COMPATIBLE_SONY           0
#define VISCA_COMPATIBLE_JJTS           1
#define VISCA_COMPATIBLE_VHD            2
#define VISCA_COMPATIBLE_MINRRAY        3




/* 协议枚举 */
enum ptc_type
{
    VISCA_IP,
    //    ONVIF,
    VISCA,
    PELCO_D,
    PELCO_P,
    ONVIF,
    PANASONIC,
    SAMSUNG,   
    
};

/* 自定义地址 */
struct address_arr
{
    uint32_t length;
    uint32_t *arr;
};

//struct visca_para
//{
//    uint32_t cam_addr;
//    int32_t auto_refresh;
//    int32_t compatible_mode;
//};

struct visca_para
{
    uint32_t cam_addr;
    int32_t auto_refresh;
    int32_t compatible_mode;
};

struct onvif_para
{
    uint32_t cam_addr;   
};

struct pelco_para
{
    uint32_t cam_addr;
};

struct panasonic_para
{
    uint32_t cam_addr;
    uint32_t owm_addr;
};

struct samsung_para
{
    uint32_t cam_addr;
    uint32_t owm_addr;
};

struct ptc_para
{
    int32_t type;
    struct visca_para visca_ip;
    struct onvif_para;
    struct visca_para visca;
    struct pelco_para pelco_d;
    struct pelco_para pelco_p;
    struct panasonic_para panasonic;
    struct samsung_para samsung;
};

/* 网络参数 */
struct net_para
{
    int32_t dhcp;
    uint32_t ipaddr;
    uint32_t mask;
    uint32_t gateway;
};

/* 串口参数 */
struct serialport_para
{
    int32_t rs232_baudrate;
    int32_t rs422_baudrate;
    int32_t mode;
};

/* 系统参数 */
typedef struct 
{
    struct ptc_para ptc;
    struct net_para net;
    struct serialport_para serial;
    struct rocker_para rocker;
    int32_t buzzer;
    int32_t sleep_time;
    int32_t language;
    int32_t speed_grade;
}SysPara_t;

/* 运行状态 */
typedef struct run_info
{
    int32_t cpu_usage;
    int32_t lock;
    int32_t menu;
    int32_t op_num;
    int32_t op_num_index;
    int32_t idle;
    uint32_t sent_timer;
    uint32_t recv_timer;
}RunInfo_t;

#define GET_UART_MODE(n)        (n.serial.mode)
#define GET_RS232_BAUDRATE(n)   (n.serial.rs232_baudrate)
#define GET_RS422_BAUDRATE(n)   (n.serial.rs422_baudrate)



/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/


#endif
/****************************************** END OF FILE *******************************************/
