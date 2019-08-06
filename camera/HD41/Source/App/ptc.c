/***************************************************************************************************
* FILE: ptc.c
*
* DESCRIPTION:  --
*
* CREATED: 2017/9/22, by kevin xu
***************************************************************************************************/
#include "ptc.h"
#include "packet_list.h"
#include "ccp.h"
#include "cam_ops.h"
#include "system.h"

/* 串口缓存链表 */
PACKET_LIST(plCameraRx);    //用
PACKET_LIST(plCamCache);    //
PACKET_LIST(plInput1Rx);    //
PACKET_LIST(plOutputRx);

/* CCP公共属性句柄 */
struct ccp_ops ccp_o;

/* CCP通道句柄 */
struct ccp_chn ccp_cam;
struct ccp_chn ccp_cache;
struct ccp_chn ccp_in1;
struct ccp_chn ccp_out1;


/* 系统控制接口 */
extern struct camera_operations camera_ops;
int32_t camera_ops_init(void);

/***************************************************************************************************
* Description:  CCP各通道数据接口定义
***************************************************************************************************/
/* 相机1 */
int32_t ccp_cam1_clear_packet_buffer()
{
    PacketList_Clear(&plCameraRx);
    return 0;
}
int32_t ccp_cam1_get_packet(uint8_t *buf)
{
    uint32_t length;
    if(PacketList_GetFirst(&plCameraRx, buf, &length))
        return 0;
    return length;
}

/* 缓存1 */
int32_t ccp_cache1_clear_packet_buffer()
{
    PacketList_Clear(&plCamCache);
    return 0;
}
int32_t ccp_cache1_get_packet(uint8_t *buf)
{
    uint32_t length;
    if(PacketList_GetFirst(&plCamCache, buf, &length))
        return 0;
    return length;
}
int32_t ccp_cache1_add_packet(uint8_t *buf, uint8_t size)
{
    return PacketList_AddTail(&plCamCache, buf, size);
}

/* 输入1 */
int32_t ccp_in1_clear_packet_buffer()
{
    PacketList_Clear(&plInput1Rx);
    return 0;
}
int32_t ccp_in1_get_packet(uint8_t *buf)
{
    uint32_t length;
    if(PacketList_GetFirst(&plInput1Rx, buf, &length))
        return 0;
    return length;
}

int32_t ccp_in1_send(const uint8_t *buffer, uint16_t length)
{
    if(IS_RS485_ENABLE)
    {
        RS485_TX_MODE();
        UART1_SendArray(buffer, length);
        delay_ms(1);
        RS485_RX_MODE();
    }
    else
    {
        UART1_DmaSend(buffer, length);
    }
    return 0;
}

/* 输出1 */
int32_t ccp_out1_clear_packet_buffer()
{
    PacketList_Clear(&plOutputRx);
    return 0;
}
int32_t ccp_out1_get_packet(uint8_t *buf)
{
    uint32_t length;
    if(PacketList_GetFirst(&plOutputRx, buf, &length))
        return 0;
    return length;
}

int32_t ShowDebugCmd(uint8_t *buffer, int32_t length)
{
    //    while(length--)
    //    {
    //        FPGA_OsdShowHex(*buffer);
    //        buffer++;
    //    }
    return 0;
}

/***************************************************************************************************
* Description: 串口初始化
***************************************************************************************************/
void Ptc_UartInit(void)
{
    uint32_t baud[6] = {2400, 4800, 9600, 19200, 38400, 115200};
    static uint8_t Initialized = 0;
    
    if(Initialized == 0)
    {
        Initialized = 1;
        UART3_Init(115200);
    }
    
    UART1_Init(baud[PTC_BAUDRATE]);
    //    UART2_Init(baud[PTC_BAUDRATE]);
}

/***************************************************************************************************
* Description:  指令系统初始化
***************************************************************************************************/
void Ptc_Init(void)
{
    static uint8_t Initialized = 0;
    
    /* 串口初始化 */
    Ptc_UartInit();
    
    if(Initialized == 0)
    {
        Initialized = 1;
        
        /* 初始化系统控制接口 */
        camera_ops_init();
        
        /* CCP协议栈基础API注册 */
        ccp_o.delay_ms = delay_ms;
        ccp_o.action_hint = LED_Blink;
        ccp_o.get_power_status = System_GetPowerState;
        ccp_o.cmd_debug_show = ShowDebugCmd;
        Ccp_Init(&ccp_o);
        
        
        /******* 初始化并注册通道 *******/
        Ccp_RemoveAllChannel();
        
        /* 机芯通道 */
        memset(&ccp_cam, 0, sizeof(struct ccp_chn));
        ccp_cam.path_id = 0;
        ccp_cam.type = CCP_VISCA;
        ccp_cam.addr = 1;
        ccp_cam.speed_grade = 0;
        ccp_cam.clear_packet_buffer = ccp_cam1_clear_packet_buffer;
        ccp_cam.get_packet = ccp_cam1_get_packet;
        ccp_cam.send_packet = UART3_DmaSend;
        ccp_cam.camera_ops = &camera_ops;
        /* 机芯数据缓存 */
        memset(&ccp_cache, 0, sizeof(struct ccp_chn));
        ccp_cache.path_id = 0;
        ccp_cache.type = CCP_VISCA;
        ccp_cache.addr = 1;
        ccp_cache.speed_grade = 0;
        ccp_cache.clear_packet_buffer = ccp_cache1_clear_packet_buffer;
        ccp_cache.get_packet = ccp_cache1_get_packet;
        ccp_cache.add_packet = ccp_cache1_add_packet;
        ccp_cache.send_packet = UART3_DmaSend;
        ccp_cache.camera_ops = &camera_ops;
        Ccp_AddCamChannel(&ccp_cam, &ccp_cache);
        
        /* 输入通道1 */ //数据发送到外部设备
        memset(&ccp_in1, 0, sizeof(struct ccp_chn));
        ccp_in1.path_id = 1;
        ccp_in1.type = stSysInfo.ptc.type;
        ccp_in1.addr = stSysInfo.ptc.addr;
        ccp_in1.speed_grade = stSysInfo.ptc.speed_grade;
        ccp_in1.clear_packet_buffer = ccp_in1_clear_packet_buffer;
        ccp_in1.get_packet = ccp_in1_get_packet;
        ccp_in1.send_packet = ccp_in1_send;
        ccp_in1.camera_ops = &camera_ops;
        Ccp_AddChannel(&ccp_in1);
        
        //        /* 输出通道1 */
        //        memset(&ccp_out1, 0, sizeof(struct ccp_chn));
        //        ccp_out1.path_id = -1;
        //        ccp_out1.type = CCP_VISCA_INQ;
        //        ccp_out1.addr = 1;
        //        ccp_out1.speed_grade = stSysInfo.ptc.speed_grade;
        //        ccp_out1.clear_packet_buffer = ccp_out1_clear_packet_buffer;
        //        ccp_out1.get_packet = ccp_out1_get_packet;
        //        ccp_out1.send_packet = UART2_DmaSend;
        //        ccp_out1.camera_ops = &camera_ops;
        //        Ccp_AddChannel(&ccp_out1);
    }
    else
    {
        ccp_in1.type = stSysInfo.ptc.type;
        ccp_in1.addr = stSysInfo.ptc.addr;
        ccp_in1.speed_grade = stSysInfo.ptc.speed_grade;
    }
}

/***************************************************************************************************
* Description:  指令Debug控制
***************************************************************************************************/
void CmdDebug(uint8_t onoff)
{
    ccp_o.cmd_debug = onoff;
    stRunInfo.refresh_mask = onoff;
}

/***************************************************************************************************
* Description:  外部串口与机芯环通控制
***************************************************************************************************/
void LinkCam2In1(uint8_t onoff)
{
    ccp_o.direct_link_cam2in1 = onoff;
}

/****************************************** END OF FILE *******************************************/
