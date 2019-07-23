/***************************************************************************************************
* FILE: visca_ip.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/25, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "ptc.h"
#include "api.h"
#include "visca_if.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define VISCA_IP_CONFIG_PORT    52380
#define VISCA_IP_CTRL_PORT      52381

#define VISCAIP_CUR_ADDR()      (stSysPara.ptc.visca_ip.cam_addr)

#define VISCA_IP_RETRY          3

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/

/* �������������� */
TaskHandle_t ViscaIpRcvTask;

/* ����ӿھ�� */
static struct netconn *ncViscaIp;

/* ����ӿڴ������ */
err_t err;

/* ���ջ��� */
static PACKET_LIST_HEAD(ViscaIpRxList);

/* ָ����� */
static uint32_t visca_ip_cmd_index;

/* ��ѯʧ�ܼ��� */
uint8_t viscaip_inq_failure_count = 0;



/* ���״̬ */
static struct visca_camera_status visca_ip_cam_status = {0};

static uint32_t rcv_count = 0;

/* ��ַ���� */
uint32_t ViscaIpAddressArr[255] = {0};
struct address_arr ViscaIpAddr = 
{
    0,
    ViscaIpAddressArr,
};

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
int ViscaIp_SelectCustomAddress(int arg);



/***************************************************************************************************
* Description:  VISCA IP�����߳�
***************************************************************************************************/
void func_ViscaIpRcvTask(void * argument)
{
    struct netbuf  *recvbuf;
    struct pbuf *p;
    
    while(1)
    {
        /* �������� */
        if((netconn_recv(ncViscaIp, &recvbuf) == ERR_OK) && (recvbuf != NULL))
        {
            /* �ر��ж�,�����ٽ��� */
            //            taskENTER_CRITICAL();
            
            /* ���������ȡ���� */
            for(p=recvbuf->p; p!=NULL; p=p->next)
            {
                while(PackList_GetSize(ViscaIpRxList) > 1024)
                {
                    PacketList_DelFirst(&ViscaIpRxList);
                    printf("Throw away some packets!\r\n");
                }
                if(PacketList_Add(&ViscaIpRxList, p->payload, p->len))
                {
                    printf("Add packet failure!\r\n");
                }              
            }
            
            rcv_count++;
            
            /* �ͷ��ڴ�, �˳��ٽ��� */
            netbuf_delete(recvbuf);
            //            taskEXIT_CRITICAL();
        }
        else
        {
            vTaskDelay(5);
        }
    }
}

/***************************************************************************************************
* Description:  ������뻺��
***************************************************************************************************/
void ViscaIpClearReturnBuffer(void)
{
    PacketList_Destroy(&ViscaIpRxList);
}

/***************************************************************************************************
* Description:  ��ȡһ�������
***************************************************************************************************/
int32_t ViscaIpGetReply(uint8_t *buffer)
{
    uint8_t *pk;
    uint32_t length;
    
    if(PacketList_Get(&ViscaIpRxList, &pk, &length))
    {
        return 0;
    }
    
    if(length <= 8)
        return 0;
    
    if((pk[0] == 0x01) && (pk[1] == 0x11) && \
        (pk[4] == (uint8_t)(visca_ip_cmd_index>>24)) && \
            (pk[5] == (uint8_t)(visca_ip_cmd_index>>16)) && \
                (pk[6] == (uint8_t)(visca_ip_cmd_index>>8)) && \
                    (pk[7] == (uint8_t)(visca_ip_cmd_index>>0)))
    {
        length = pk[2];
        length = (length<<8) | pk[3];
        memcpy(buffer, &pk[8], length);
    }
    else
    {
        length = 0;
    }
    
    PacketList_DelFirst(&ViscaIpRxList);
    
    UI_ShowRecvPacket();
    
    return length;
}

/***************************************************************************************************
* Description:  ����ָ��
***************************************************************************************************/
void ViscaIp_SentCmd(const uint8_t *cmd, uint8_t length)
{
    uint8_t buffer[24];
    struct netbuf *sentbuf;
    
    buffer[0] = 0x01;
    buffer[1] = 0x00;
    buffer[2] = (uint8_t)(length>>8);
    buffer[3] = (uint8_t)(length>>0);
    
    ++visca_ip_cmd_index;
    buffer[4] = (uint8_t)(visca_ip_cmd_index>>24);
    buffer[5] = (uint8_t)(visca_ip_cmd_index>>16);
    buffer[6] = (uint8_t)(visca_ip_cmd_index>>8);
    buffer[7] = (uint8_t)(visca_ip_cmd_index>>0);
    
    memcpy(&buffer[8], cmd, length);
    
    length += 8;
    
    sentbuf = netbuf_new();
    netbuf_alloc(sentbuf, length);
    memcpy(sentbuf->p->payload, (void*)buffer, length);
    
    err = netconn_send(ncViscaIp, sentbuf);
    netbuf_delete(sentbuf);
    
    UI_ShowSentPacket();
}

/***************************************************************************************************
* Description:  ���Ͳ�ѯָ��
***************************************************************************************************/
void ViscaIp_SentInq(const uint8_t *cmd, uint8_t length)
{
    uint8_t buffer[24];
    struct netbuf *sentbuf;
    
    buffer[0] = 0x01;
    buffer[1] = 0x00;
    buffer[2] = (uint8_t)(length>>8);
    buffer[3] = (uint8_t)(length>>0);
    
    ++visca_ip_cmd_index;
    buffer[4] = (uint8_t)(visca_ip_cmd_index>>24);
    buffer[5] = (uint8_t)(visca_ip_cmd_index>>16);
    buffer[6] = (uint8_t)(visca_ip_cmd_index>>8);
    buffer[7] = (uint8_t)(visca_ip_cmd_index>>0);
    
    memcpy(&buffer[8], cmd, length);
    
    length += 8;
    
    sentbuf = netbuf_new();
    netbuf_alloc(sentbuf, length);
    memcpy(sentbuf->p->payload, (void*)buffer, length);
    
    err = netconn_send(ncViscaIp, sentbuf);
    netbuf_delete(sentbuf);  
}

/***************************************************************************************************
* Description:  �����豸
***************************************************************************************************/
const uint8_t VISCA_IP_FIND_DEVICE[] = {0x02, 0x45, 0x4e, 0x51, 0x3a, 0x6e, 0x65, 0x74, 0x77, 0x6f, 0x72, 0x6b, 0xff, 0x03};
int32_t ViscaIp_SearchDevice(int32_t arg)
{
    struct netbuf *sentbuf;
    uint8_t *buffer;
    uint32_t length;
    char str[30] = {' '};
    
    UI_ShowAction("Searching Visca IP Device...", 1);
    
    /* ��ս��ջ��� */
    PacketList_Destroy(&ViscaIpRxList);
    rcv_count = 0;
    ViscaIpAddr.length = 0;
    
    /* ����netcon */
    err = netconn_bind(ncViscaIp, IP_ADDR_ANY, VISCA_IP_CONFIG_PORT);
    netconn_disconnect(ncViscaIp);
    
    /* ���������豸���ݰ� */
    sentbuf = netbuf_new();
    netbuf_alloc(sentbuf, sizeof(VISCA_IP_FIND_DEVICE));
    memcpy(sentbuf->p->payload, (void*)VISCA_IP_FIND_DEVICE, sizeof(VISCA_IP_FIND_DEVICE));
    err = netconn_sendto(ncViscaIp, sentbuf, IP_ADDR_BROADCAST, VISCA_IP_CONFIG_PORT);
    
    /* �ȴ��豸�������ݰ� */
    vTaskDelay(500);
    
    /* �������ջ��� */
    while(!PacketList_Get(&ViscaIpRxList, &buffer, &length))
    {
        /* ƥ��ؼ��� */
        char *pos1 = strstr((const char *)buffer, "IPADR:");
        char *pos2 = strstr((const char *)buffer, "MASK:");
        
        uint32_t ipaddr;
        
        if(pos1 != NULL && pos2 != NULL)
        {
            int32_t flag = 0;
            StringToIpAddr(pos1+6, &ipaddr);
            for(int i=0; i<ViscaIpAddr.length; i++)
            {
                if(ipaddr == ViscaIpAddr.arr[i])
                {
                    flag = 1;
                    break;
                }
            }
            if(flag == 0)
            {
                ViscaIpAddr.arr[ViscaIpAddr.length] = ipaddr;
                ViscaIpAddr.length++;
            }
        }
        
        //        if(pos1 != NULL && pos2 != NULL)
        //        {
        //            StringToIpAddr(pos1+6, &ViscaIpAddr.arr[ViscaIpAddr.length]);
        //            ViscaIpAddr.length++;
        //        }
        
        /* ɾ����һ���� */
        PacketList_DelFirst(&ViscaIpRxList);
    }
    
    /* ��ʾ������� */
    sprintf(str, "Find %d Device", ViscaIpAddr.length);
    if(ViscaIpAddr.length > 1) 
        strcat(str, "s");
    UI_ShowAction(str, 1);
    
    /* �ͷ���Դ */
    netbuf_delete(sentbuf);
    err = netconn_bind(ncViscaIp, IP_ADDR_ANY, VISCA_IP_CTRL_PORT);
    ip_addr_t ipaddr = {SwapIp(VISCAIP_CUR_ADDR())};
    netconn_connect(ncViscaIp, &ipaddr, VISCA_IP_CTRL_PORT);
    
    return 0;
}

/***************************************************************************************************
* Description:  ���ָ������
***************************************************************************************************/
int32_t ViscaIp_ClearCmdIndex(void)
{
    const uint8_t buffer[9] = {0x02, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01};
    struct netbuf *sentbuf;
    
    if(VISCAIP_CUR_ADDR() == 0)
        return 0;
    
    sentbuf = netbuf_new();
    netbuf_alloc(sentbuf, sizeof(buffer));
    memcpy(sentbuf->p->payload, (void*)buffer, sizeof(buffer));
    err = netconn_send(ncViscaIp, sentbuf);
    netbuf_delete(sentbuf);
    
    visca_ip_cmd_index = 0;
    viscaip_inq_failure_count = 0;
    
    memset(&visca_ip_cam_status, 0, sizeof(visca_ip_cam_status));
    
    return 0;
}

/***************************************************************************************************
* Description:  ����Ŀ���ַ
***************************************************************************************************/
void ViscaIp_SetTargetAddress(void)
{
    netconn_disconnect(ncViscaIp);
    ip_addr_t ipaddr = {SwapIp(VISCAIP_CUR_ADDR())};
    netconn_connect(ncViscaIp, &ipaddr, VISCA_IP_CTRL_PORT);
    
    ViscaIp_ClearCmdIndex();
}

/***************************************************************************************************
* Description:  ѡ���豸
***************************************************************************************************/
int32_t ViscaIp_SelectDevice(int32_t arg)
{
    struct ulist_handle *ViscaIpDeviceList;
    struct uitem tmp_uitem;
    
    /* �豸�б�Ϊ�� */
    if(ViscaIpAddr.length == 0 && ViscaIpCustomAddr.length == 0)
    {
        goto EMPTY;
    }
    
    /* ��ʼ���б� */
    UList_Init(&ViscaIpDeviceList, "Select Camera", ON);
    
    /* ����Զ����ַ�б���� */
    if(ViscaIpCustomAddr.length > 0)
    {
        memset(&tmp_uitem, 0, sizeof(struct uitem));
        tmp_uitem.type = uitem_func;
        tmp_uitem.genius = (int32_t)ViscaIp_SelectCustomAddress;
        memcpy(tmp_uitem.text, "Custom Addresses List", sizeof("Custom Addresses List"));
        Ulist_AppendItem(ViscaIpDeviceList, &tmp_uitem, 1);
    }
    
    /* ����������ĵ�ַ */
    for(int32_t i=0; i<ViscaIpAddr.length; i++)
    {
        memset(&tmp_uitem, 0, sizeof(struct uitem));
        tmp_uitem.type = uitem_sel_no_sign;
        tmp_uitem.genius = NULL;
        IpAddrToString(tmp_uitem.text, ViscaIpAddr.arr[i]);
        Ulist_AppendItem(ViscaIpDeviceList, &tmp_uitem, 1);
    }
    
    UList_Show(ViscaIpDeviceList);
    stRunInfo.menu = 2;
    int8_t rocker_val;
    
    while(1)
    {
        UList_Roll(ViscaIpDeviceList, e_on);
        
        if(Rocker_GetPanVal(&rocker_val, 3))
        {
            if(rocker_val > 0)
            {
                struct list_item *tmp = list_entry(ViscaIpDeviceList->cur_item, struct list_item, list);
                
                if(tmp->type == uitem_sel_no_sign)
                {
                    StringToIpAddr(tmp->text, &VISCAIP_CUR_ADDR());
                }
                else if(tmp->type == uitem_func)
                {
                    if(((int32_t (*)(int32_t))tmp->genius)(0))
                    {
                        goto EMPTY;
                    }
                }
                else
                {
                    break;
                }
                ViscaIp_SetTargetAddress();
                System_SavePara(SYS_SAVE_VISCAIP_ADDR);
                break;
            }
            else if(rocker_val < 0)
            {
                break;
            }
        }
        
        if(GetKey(KEY_ROCKER, 0))
        {
            struct list_item *tmp = list_entry(ViscaIpDeviceList->cur_item, struct list_item, list);
            
            if(tmp->type == uitem_sel_no_sign)
            {
                StringToIpAddr(tmp->text, &VISCAIP_CUR_ADDR());
            }
            else if(tmp->type == uitem_func)
            {
                if(((int32_t (*)(int32_t))tmp->genius)(0))
                {
                    goto EMPTY;
                }
            }
            else
            {
                break;
            }
            ViscaIp_SetTargetAddress();
            System_SavePara(SYS_SAVE_VISCAIP_ADDR);
            break;
        }
        
        if(GetKey(KEY_BACK, 0))
        {
            break;
        }
        vTaskDelay(20);
    }
    
    UI_DrawMainFrame();
    
    return 0;
    
    EMPTY:UI_ShowAction("The List Is Empty", 0);
    return 0;
}

int ViscaIp_SelectCustomAddress(int arg)
{
    struct uitem tmp_uitem; 
    struct ulist_handle *CustomAddress;
    
    if(arg == arg_getbrief)
        return 0;
    
    UList_Init(&CustomAddress, "Custom Address List", ON);
    
    for(int32_t i=0; i<ViscaIpCustomAddr.length; i++)
    {
        memset(&tmp_uitem, 0, sizeof(struct uitem));
        tmp_uitem.type = uitem_sel_no_sign;
        tmp_uitem.genius = NULL;
        IpAddrToString(tmp_uitem.text, ViscaIpCustomAddr.arr[i]);
        Ulist_AppendItem(CustomAddress, &tmp_uitem, 1);
    }
    
    UList_Show(CustomAddress);
    int8_t rocker_val;
    
    while(1)
    {
        UList_Roll(CustomAddress, e_on);
        
        if(Rocker_GetPanVal(&rocker_val, 3))
        {
            if(rocker_val > 0)
            {
                struct list_item *tmp = list_entry(CustomAddress->cur_item, struct list_item, list);
                
                if(tmp->type == uitem_sel_no_sign)
                {
                    StringToIpAddr(tmp->text, &VISCAIP_CUR_ADDR());
                }
                else if(tmp->type == uitem_func)
                {
                    ((int32_t (*)(int32_t))tmp->genius)(0);
                }
                else
                {
                    break;
                }
                ViscaIp_SetTargetAddress();
                System_SavePara(SYS_SAVE_VISCAIP_ADDR);
                break;
            }
            else if(rocker_val < 0)
            {
                break;
            }
        }
        
        if(GetKey(KEY_ROCKER, 0))
        {
            struct list_item *tmp = list_entry(CustomAddress->cur_item, struct list_item, list);
            
            if(tmp->type == uitem_sel_no_sign)
            {
                StringToIpAddr(tmp->text, &VISCAIP_CUR_ADDR());
            }
            else if(tmp->type == uitem_func)
            {
                ((int32_t (*)(int32_t))tmp->genius)(0);
            }
            else
            {
                break;
            }
            ViscaIp_SetTargetAddress();
            System_SavePara(SYS_SAVE_VISCAIP_ADDR);
            break;
        }
        
        if(GetKey(KEY_BACK, 0))
        {
            break;
        }
        vTaskDelay(20);
    }
    
    UList_Destroy(CustomAddress);
    return 0;
}

/***************************************************************************************************
* Description:  ��ȡ��ǰ��ַ
***************************************************************************************************/
uint32_t  ViscaIp_GetCurAddr(void)
{
    return VISCAIP_CUR_ADDR();
}

/***************************************************************************************************
* Description:  ����Ԥ��λ
***************************************************************************************************/
int32_t ViscaIp_Preset_Call(int32_t arg)
{
    char str[30];
    if(arg > 255)
        UI_ShowAction("Out Of Range!", 0);
    else
    {
        visca_set_memory(1, VISCA_PRESET_CALL, arg, VISCA_IP_RETRY);
        sprintf(str, "Call Preset %d", arg);
        UI_ShowAction(str, 0); 
    }
    return 0;
}

/***************************************************************************************************
* Description:  ����Ԥ��λ
***************************************************************************************************/
int32_t ViscaIp_Preset_Set(int32_t arg)
{
    char str[30];
    if(arg > 255)
        UI_ShowAction("Out Of Range!", 0);
    else
    {
        visca_set_memory(1, VISCA_PRESET_SET, arg, VISCA_IP_RETRY);
        sprintf(str, "Set Preset %d", arg);
        UI_ShowAction(str, 0); 
    }
    return 0;
}

/***************************************************************************************************
* Description:  ɾ��Ԥ��λ
***************************************************************************************************/
int32_t ViscaIp_Preset_Delete(int32_t arg)
{
    char str[30] = {0};
    if(arg > 255)
        UI_ShowAction("Out Of Range!", 0);
    else
    {
        visca_set_memory(1, VISCA_PRESET_DELETE, arg, VISCA_IP_RETRY);
        sprintf(str, "Delete Preset %d", arg);
        UI_ShowAction(str, 0); 
    }
    return 0;
}

/***************************************************************************************************
* Description:  OSD
***************************************************************************************************/
int32_t ViscaIp_Osd(int32_t arg)
{
    char str[30] = {0};
    visca_set_memory(1, VISCA_PRESET_CALL, 95, VISCA_IP_RETRY);
    
    sprintf(str, "Call Camera OSD");
    UI_ShowAction(str, 0); 
    return 0;
}

/***************************************************************************************************
* Description:  Backlight
***************************************************************************************************/
int32_t ViscaIp_Backlight(int32_t arg)
{
    char str[30] = {0};
    if(visca_ip_cam_status.backlight == VISCA_ON)
    {
        visca_ip_cam_status.backlight = VISCA_OFF;
        strcat(str, "Backlight OFF");
    }
    else
    {
        visca_ip_cam_status.backlight = VISCA_ON;
        strcat(str, "Backlight ON");
    }
    
    visca_set_8bits_value(1, VISCA_BACKLIGHT, visca_ip_cam_status.backlight, VISCA_IP_RETRY);
    UI_ShowAction(str, 0); 
    
    return 0;
}

/***************************************************************************************************
* Description:  AE Mode
***************************************************************************************************/
int32_t ViscaIp_Ae_Mode(int32_t arg)
{
    char str[30] = {0};
    if(visca_ip_cam_status.exposure_mode == VISCA_AE_FULL_AUTO)
    {
        visca_ip_cam_status.exposure_mode = VISCA_AE_SHUTTER_PRIORITY;
        sprintf(str, "Set to Shutter Priority");
    }
    else if(visca_ip_cam_status.exposure_mode == VISCA_AE_SHUTTER_PRIORITY)
    {
        visca_ip_cam_status.exposure_mode = VISCA_AE_IRIS_PRIORITY;
        sprintf(str, "Set to Iris Priority");
    }
    else
    {
        visca_ip_cam_status.exposure_mode = VISCA_AE_FULL_AUTO;
        sprintf(str, "Set to Full Auto");
    }
    
    visca_set_8bits_value(1, VISCA_AE, visca_ip_cam_status.exposure_mode, VISCA_IP_RETRY);
    UI_ShowAction(str, 0); 
    
    return 0;
}

/***************************************************************************************************
* Description:  AE +
***************************************************************************************************/
int32_t ViscaIp_Ae_Add(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_ip_cam_status.exposure_mode == VISCA_AE_SHUTTER_PRIORITY)
    {
        visca_set_8bits_value(1, VISCA_SHUTTER, VISCA_SHUTTER_UP, VISCA_IP_RETRY);
        sprintf(str, "Shutter Up");
    }
    else if(visca_ip_cam_status.exposure_mode == VISCA_AE_IRIS_PRIORITY)
    {
        visca_set_8bits_value(1, VISCA_IRIS, VISCA_IRIS_UP, VISCA_IP_RETRY);
        sprintf(str, "Iris Up");
    }
    else
    {
        sprintf(str, "Not Available In Automatic Mode");
    }
    UI_ShowAction(str, 0); 
    
    return 0;
}

/***************************************************************************************************
* Description:  AE -
***************************************************************************************************/
int32_t ViscaIp_Ae_Sub(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_ip_cam_status.exposure_mode == VISCA_AE_SHUTTER_PRIORITY)
    {
        visca_set_8bits_value(1, VISCA_SHUTTER, VISCA_SHUTTER_DOWN, VISCA_IP_RETRY);
        sprintf(str, "Shutter Down");
    }
    else if(visca_ip_cam_status.exposure_mode == VISCA_AE_IRIS_PRIORITY)
    {
        visca_set_8bits_value(1, VISCA_IRIS, VISCA_IRIS_DOWN, VISCA_IP_RETRY);
        sprintf(str, "Iris Down");
    }
    else
    {
        sprintf(str, "Not Available In Automatic Mode");
    }
    UI_ShowAction(str, 0); 
    
    return 0;
}

/***************************************************************************************************
* Description:  WB Mode
***************************************************************************************************/
extern const char textViscaWbMode[][30];
int32_t ViscaIp_Wb_Mode(int32_t arg)
{
    char str[30] = {0};
    
    ++visca_ip_cam_status.wb_mode;
    if(visca_ip_cam_status.wb_mode > VISCA_WB_MANUAL)
    {
        visca_ip_cam_status.wb_mode = VISCA_WB_AUTO;
    }
    
    visca_set_8bits_value(1, VISCA_WB, visca_ip_cam_status.wb_mode, VISCA_IP_RETRY);
    strcat(str, textViscaWbMode[visca_ip_cam_status.wb_mode]);
    
    UI_ShowAction(str, 0);
    return 0;
}

/***************************************************************************************************
* Description:  RGAIN ADD
***************************************************************************************************/
int32_t ViscaIp_Rgain_Add(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_ip_cam_status.wb_mode == VISCA_WB_ONE_PUSH_WB)
    {
        visca_set_8bits_value(1, VISCA_WB_ONE_PUSH_TRIG, 0x05, VISCA_IP_RETRY);
        sprintf(str, "Trigger A White Balance");
    }
    else
    {
        if(visca_ip_cam_status.wb_mode != VISCA_WB_MANUAL)
        {
            visca_ip_cam_status.wb_mode = VISCA_WB_MANUAL;
            visca_set_8bits_value(1, VISCA_WB, visca_ip_cam_status.wb_mode, VISCA_IP_RETRY);
            vTaskDelay(50);
        }
        visca_set_8bits_value(1, VISCA_RGAIN, VISCA_RGAIN_UP, VISCA_IP_RETRY);
        sprintf(str, "R-GAIN Up");
    }
    
    UI_ShowAction(str, 0); 
    
    return 0;
}

/***************************************************************************************************
* Description:  RGAIN SUB
***************************************************************************************************/
int32_t ViscaIp_Rgain_Sub(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_ip_cam_status.wb_mode == VISCA_WB_ONE_PUSH_WB)
    {
        visca_set_8bits_value(1, VISCA_WB_ONE_PUSH_TRIG, 0x05, VISCA_IP_RETRY);
        sprintf(str, "Trigger A White Balance");
    }
    else
    {
        if(visca_ip_cam_status.wb_mode != VISCA_WB_MANUAL)
        {
            visca_ip_cam_status.wb_mode = VISCA_WB_MANUAL;
            visca_set_8bits_value(1, VISCA_WB, visca_ip_cam_status.wb_mode, VISCA_IP_RETRY);
            vTaskDelay(50);
        }
        visca_set_8bits_value(1, VISCA_RGAIN, VISCA_RGAIN_DOWN, VISCA_IP_RETRY);
        sprintf(str, "R-GAIN Down");
    }
    
    UI_ShowAction(str, 0);
    return 0;
}

/***************************************************************************************************
* Description:  BGAIN ADD
***************************************************************************************************/
int32_t ViscaIp_Bgain_Add(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_ip_cam_status.wb_mode == VISCA_WB_ONE_PUSH_WB)
    {
        visca_set_8bits_value(1, VISCA_WB_ONE_PUSH_TRIG, 0x05, VISCA_IP_RETRY);
        sprintf(str, "Trigger A White Balance");
    }
    else
    {
        if(visca_ip_cam_status.wb_mode != VISCA_WB_MANUAL)
        {
            visca_ip_cam_status.wb_mode = VISCA_WB_MANUAL;
            visca_set_8bits_value(1, VISCA_WB, visca_ip_cam_status.wb_mode, VISCA_IP_RETRY);
            vTaskDelay(50);
        }
        visca_set_8bits_value(1, VISCA_BGAIN, VISCA_BGAIN_UP, VISCA_IP_RETRY);
        sprintf(str, "B-GAIN Up");
    }
    
    UI_ShowAction(str, 0); 
    return 0;
}

/***************************************************************************************************
* Description:  BGAIN SUB
***************************************************************************************************/
int32_t ViscaIp_Bgain_Sub(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_ip_cam_status.wb_mode == VISCA_WB_ONE_PUSH_WB)
    {
        visca_set_8bits_value(1, VISCA_WB_ONE_PUSH_TRIG, 0x05, VISCA_IP_RETRY);
        sprintf(str, "Trigger A White Balance");
    }
    else
    {
        if(visca_ip_cam_status.wb_mode != VISCA_WB_MANUAL)
        {
            visca_ip_cam_status.wb_mode = VISCA_WB_MANUAL;
            visca_set_8bits_value(1, VISCA_WB, visca_ip_cam_status.wb_mode, VISCA_IP_RETRY);
            vTaskDelay(50);
        }
        visca_set_8bits_value(1, VISCA_BGAIN, VISCA_BGAIN_DOWN, VISCA_IP_RETRY);
        sprintf(str, "B-GAIN Down");
    }
    
    UI_ShowAction(str, 0); 
    
    return 0;
}

/***************************************************************************************************
* Description:  FOCUS AUTO
***************************************************************************************************/
int32_t ViscaIp_Focus_Auto(int32_t arg)
{
    char str[30] = {0};
    
    visca_ip_cam_status.focus_mode = VISCA_FOCUS_AUTO;
    visca_set_8bits_value(1, VISCA_FOCUS_MODE, VISCA_FOCUS_AUTO, VISCA_IP_RETRY);
    sprintf(str, "Set To Auto Focus");
    
    UI_ShowAction(str, 0); 
    
    return 0;
}

/***************************************************************************************************
* Description:  FOCUS FAR
***************************************************************************************************/
int32_t ViscaIp_Focus_Far(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_ip_cam_status.focus_mode != VISCA_FOCUS_MANUAL)
    {
        visca_ip_cam_status.focus_mode = VISCA_FOCUS_MANUAL;
        visca_set_8bits_value(1, VISCA_FOCUS_MODE, VISCA_FOCUS_MANUAL, VISCA_IP_RETRY);
        sprintf(str, "Set To Auto Focus");
        UI_ShowAction(str, 0); 
    }
    
    sprintf(str, "Focus Far");
    UI_ShowAction(str, 0); 
    visca_set_8bits_value(1, VISCA_FOCUS, VISCA_FOCUS_FAR_STD, VISCA_IP_RETRY);
    HAL_Delay(30);
    visca_set_8bits_value(1, VISCA_FOCUS, VISCA_FOCUS_STOP, VISCA_IP_RETRY);
    return 0;
}

/***************************************************************************************************
* Description:  FOCUS NEAR
***************************************************************************************************/
int32_t ViscaIp_Focus_Near(int32_t arg)
{
    char str[30] = {0};
    
    if(visca_ip_cam_status.focus_mode != VISCA_FOCUS_MANUAL)
    {
        visca_ip_cam_status.focus_mode = VISCA_FOCUS_MANUAL;
        visca_set_8bits_value(1, VISCA_FOCUS_MODE, VISCA_FOCUS_MANUAL, VISCA_IP_RETRY);
        sprintf(str, "Set To Manual Focus");
        UI_ShowAction(str, 0); 
    }
    
    sprintf(str, "Focus Near");
    UI_ShowAction(str, 0);
    visca_set_8bits_value(1, VISCA_FOCUS, VISCA_FOCUS_NEAR_STD, VISCA_IP_RETRY);
    HAL_Delay(30);
    visca_set_8bits_value(1, VISCA_FOCUS, VISCA_FOCUS_STOP, VISCA_IP_RETRY);
    
    return 0;
}

/***************************************************************************************************
* Description:  GO HOME
***************************************************************************************************/
int32_t ViscaIp_Gohome(int32_t arg)
{
    char str[30] = {0};
    
    sprintf(str, "Go Home");
    UI_ShowAction(str, 0);
    
    visca_set_pan_tilt_home(1, VISCA_IP_RETRY);
    return 0;
}

/***************************************************************************************************
* Description:  ROCKER PAN, grade�ķ�ΧΪ1~8
***************************************************************************************************/
int32_t ViscaIp_Rocker_PanTilt(int32_t pan_val, int32_t tilt_val, int32_t grade)
{
    uint8_t pan_sp, tilt_sp, pan_dir, tilt_dir;
    uint8_t max_sp = 24/8*grade;
    
    pan_sp = max_sp * abs(pan_val) / 30;
    if(pan_sp > 24)  pan_sp = 24;
    tilt_sp = max_sp * abs(tilt_val) / 30;
    if(tilt_sp > 24)  tilt_sp = 24;
    
    if(pan_val > 0) pan_dir = 0x02;
    else if(pan_val < 0) pan_dir = 0x01;
    else pan_dir = 0x03;
    
    if(tilt_val > 0) tilt_dir = 0x01;
    else if(tilt_val < 0) tilt_dir = 0x02;
    else tilt_dir = 0x03;
    
    visca_set_pan_tilt(1, pan_sp, tilt_sp, pan_dir, tilt_dir, VISCA_IP_RETRY);
    if(pan_dir == 0x03 && tilt_dir == 0x03)
    {
        visca_set_pan_tilt(1, pan_sp, tilt_sp, pan_dir, tilt_dir, VISCA_IP_RETRY);
    }
    
    return 0;
}

/***************************************************************************************************
* Description:  ROCKER ZOOM
***************************************************************************************************/
int32_t ViscaIp_Rocker_Zoom(int32_t arg, int32_t grade)
{
    uint8_t speed;
    
    speed = abs(arg) / 5;
    if(speed > 7) speed = 7;
    if(arg > 0) speed |= 0x30;
    else if(arg < 0) speed |= 0x20;
    else speed = 0;
    
    visca_set_8bits_value(1, VISCA_ZOOM, speed, VISCA_IP_RETRY);
    if(speed == 0)
    {
        visca_set_8bits_value(1, VISCA_ZOOM, speed, VISCA_IP_RETRY);
    }
    return 0;
}

/***************************************************************************************************
* Description:  ��ʱ��ѯ����
***************************************************************************************************/
void ViscaIpInqCameraStatus(void)
{
    //    if(viscaip_inq_failure_count > 10)
    //        return;
    //    
    //    if(visca_inq_8bits_value(1, VISCA_FOCUS_MODE, &visca_ip_cam_status.focus_mode, 1))
    //    {
    //        viscaip_inq_failure_count = 0;
    //    }else viscaip_inq_failure_count++;
    //    
    //    if(visca_inq_8bits_value(1, VISCA_WB_MODE_INQ, &visca_ip_cam_status.wb_mode, 1))
    //    {
    //        viscaip_inq_failure_count = 0;
    //    }else viscaip_inq_failure_count++;
    //    
    //    if(visca_inq_8bits_value(1, VISCA_AE_MODE_INQ, &visca_ip_cam_status.exposure_mode, 1))
    //    {
    //        viscaip_inq_failure_count = 0;
    //    }else viscaip_inq_failure_count++;
}

/* visca Э��ӿ� */
const static struct visca_if visca_if_ip = 
{
    vTaskDelay,
    ViscaIp_SentCmd,
    ViscaIp_SentInq,
    ViscaIpClearReturnBuffer,
    ViscaIpGetReply,
};
/* �¼���Ӧ�����ṹ�� */
const event_handler_t visca_ip_event_handler = 
{
    /* ��ȡ��ǰĿ���ַ */
    ViscaIp_GetCurAddr,
    
    /* �������� */
    ViscaIp_SearchDevice,
    ViscaIp_SelectDevice,
    ViscaIp_Preset_Call,
    ViscaIp_Preset_Set,
    ViscaIp_Preset_Delete,
    
    NULL,
    ViscaIp_Osd,
    ViscaIp_Backlight,
    ViscaIp_Ae_Mode,
    ViscaIp_Ae_Add,
    ViscaIp_Ae_Sub,
    
    ViscaIp_Wb_Mode,
    ViscaIp_Rgain_Add,
    ViscaIp_Rgain_Sub,
    ViscaIp_Bgain_Add,
    ViscaIp_Bgain_Sub,
    ViscaIp_Focus_Auto,
    ViscaIp_Focus_Far,
    ViscaIp_Focus_Near,
    
    /* ҡ�˰��� */
    ViscaIp_Gohome,
    
    /* ҡ�˵�λ�� */
    ViscaIp_Rocker_PanTilt,
    NULL,
    NULL,
    ViscaIp_Rocker_Zoom,
    
    ViscaIpInqCameraStatus,
};

/***************************************************************************************************
* Description:  ��ʼ��Э��
***************************************************************************************************/
int32_t ViscaIp_Init(void)
{   
    /* ����UDP���� */
    ncViscaIp = netconn_new(NETCONN_UDP);
    
    err = netconn_bind(ncViscaIp, IP_ADDR_ANY, VISCA_IP_CTRL_PORT);
    
    vTaskResume(ViscaIpRcvTask);
    
    /* ���ýӿ� */
    visca_if_set_ops(&visca_if_ip);
    
    /* �¼���Ӧ */
    pevent_handler = &visca_ip_event_handler;
    
    ViscaIp_SetTargetAddress();
    
    return 0;
}

/***************************************************************************************************
* Description:  ȥ��ʼ��,�ͷ���Դ
***************************************************************************************************/
int32_t ViscaIp_DeInit(void)
{
    netconn_delete(ncViscaIp);
    vTaskSuspend(ViscaIpRcvTask);
    return 0;
}

/****************************************** END OF FILE *******************************************/
