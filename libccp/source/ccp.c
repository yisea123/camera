/***************************************************************************************************
* FILE: ccp.c
*
* DESCRIPTION:  camera control protocol
*
* CREATED: 2018/11/29, by kevin
***************************************************************************************************/
#include "ccp.h"
#include "ccp_visca.h"
#include "ccp_pelco.h"
#include "string.h"

/* 版本定义 */
#define STR(s)     #s
#define VERSION(a, b, c)    STR(a) "." STR(b) "." STR(c)
#define DATE(y, m, d)       STR(y) "." STR(m) "." STR(d)
//static const char version[] = VERSION(VER_MAJOR, VER_MINOR1, VER_MINOR2);
static const char version_full[] = VERSION(VER_MAJOR, VER_MINOR1, VER_MINOR2) " " DATE(DATE_YEAR, DATE_MONTH, DATE_DAY);


/* 通道链表 */
static LIST_HEAD(ccp_list);     //用于存放通道结构体的链表头

struct ccp_ops *ccp_comm_ops;   //公共操作函数

struct ccp_chn *ccp_cam_p=NULL, *ccp_cache_p=NULL, *ccp_cam2_p=NULL, *ccp_cache2_p=NULL, *ccp_in1_p=NULL, *ccp_out1_p=NULL;

/***************************************************************************************************
* Description:  获取版本号
***************************************************************************************************/
const char* CCP_GetVersion(void)
{
    return version_full;
}

/***************************************************************************************************
* Description:  初始化
***************************************************************************************************/
int32_t Ccp_Init(struct ccp_ops *ops)
{
    if(ops == NULL)
        return 1;
    
    ccp_comm_ops = ops;
    return 0;
}

/***************************************************************************************************
* Description:  添加通道
***************************************************************************************************/
int32_t Ccp_AddChannel(struct ccp_chn *ccp)
{
    if(ccp == NULL)
        return 1;
    //    struct ccp_chn *temp_ccp; = (struct ccp_chn *)malloc(sizeof(struct ccp_chn));
    //    memcpy(temp_ccp, ccp, sizeof(struct ccp_chn));
    //    list_add(&temp_ccp->list, &ccp_list);
    list_add(&ccp->list, &ccp_list);
    
    if(ccp->path_id == 1)
        ccp_in1_p = ccp;
    else if(ccp->path_id == -1)
        ccp_out1_p = ccp;
    return 0;
}

int32_t Ccp_AddCamChannel(struct ccp_chn *cam, struct ccp_chn *cache)
{
    if(cam == NULL)
        return 1;
    
    list_add(&cam->list, &ccp_list);
    ccp_cam_p = cam;
    
    if(cache != NULL)
    {
        list_add(&cache->list, &ccp_list);
        ccp_cache_p = cache;
    }
    
    visca_select_camera(1);
    
    return 0;
}

int32_t Ccp_AddCam2Channel(struct ccp_chn *cam, struct ccp_chn *cache)
{
    if(cam == NULL)
        return 1;
    
    list_add(&cam->list, &ccp_list);
    ccp_cam2_p = cam;
    
    if(cache != NULL)
    {
        list_add(&cache->list, &ccp_list);
        ccp_cache2_p = cache;
    }
    
    return 0;
}

int32_t Ccp_RemoveChannel(int8_t id)
{
    struct list_head *temp;
    struct ccp_chn *ccp;
    
    list_for_each(temp, &ccp_list)
    {
        ccp = list_entry(temp, struct ccp_chn, list);
        if(ccp->path_id == id)
        {
            list_del(temp);
            return 0;
        }
    }
    return 1;
}

int32_t Ccp_RemoveAllChannel(void)
{
    struct list_head *temp;
    
    list_for_each(temp, &ccp_list)
    {
        list_del(temp);
    }
    return 0;
}

/***************************************************************************************************
* Description:  查找通道
***************************************************************************************************/
struct ccp_chn * Ccp_FindChannel(int8_t id)
{
    struct list_head *temp;
    struct ccp_chn *ccp;
    
    list_for_each(temp, &ccp_list)
    {
        ccp = list_entry(temp, struct ccp_chn, list);
        if(ccp->path_id == id)
        {
            return ccp;
        }
    }
    
    return NULL;
}

/***************************************************************************************************
* Description:  提取并解析指令
***************************************************************************************************/
uint8_t cmd_buf[128] = {0};
static inline int32_t Ccp_CmdParser(struct ccp_chn *ccp)
{
    int32_t len;
    
    int8_t visca_flag = -1;
    int8_t visca_inq_flag = -1;
    int8_t pelco_d_flag = -1;
    int8_t pelco_p_flag = -1;
    int8_t pelco_p_flag2 = -1;
    
    uint8_t high4bit, low4bit;
    uint8_t checkout;
    
    /* 从缓存中取指令 */
    len = ccp->get_packet(cmd_buf);     //从USART1_RX和USART3_RX中取数据    
    if(len <= 0)
        return 0;
    
    if(ccp_comm_ops->cmd_debug && ccp_comm_ops->cmd_debug_show != NULL)
    {
        ccp_comm_ops->cmd_debug_show(cmd_buf, len);
    }
    
    if(ccp_comm_ops->direct_link_cam2in1)       //工厂模式操作
    {
        if(ccp->path_id == 0)                   //取的数据是机芯发过来的数据
        {
            ccp_in1_p->send_packet(cmd_buf, len);       //直接发给外部串口
        }
        else if(ccp->path_id == 1)              //取的数据是外部串口发过来的数据
        {
            ccp_cam_p->send_packet(cmd_buf, len);  //不进行判断处理直接发给机芯
        }
        return 0;
    }
    
    /* 根据此通道的协议提取指令并解析 */
    switch(ccp->type)
    {
        case CCP_AUTO:      //菜单中选择为AUTO,这边进行判断是哪一种协议类型
        {
            for(uint32_t i=0; i<len; i++)
            {
                high4bit = H4BIT(cmd_buf[i]);
                low4bit = L4BIT(cmd_buf[i]);
                /* VISCA标志 */
                if((high4bit == 8) && (low4bit > 0) && (low4bit < 9))
                {
                    visca_flag = i;
                }
                /* Pelco-P标志 */
                if(cmd_buf[i] == 0xA0)
                {
                    pelco_p_flag = i; 
                }
                
                if(cmd_buf[i] == 0xAF)
                {
                    if((pelco_p_flag != -1) && ((i-pelco_p_flag)==6))
                    {
                        pelco_p_flag2 = i;      //为6时是Pelco-P
                    }
                }
                
                if(cmd_buf[i] == 0xFF)
                {
                    /* VISCA */
                    if(visca_flag != -1)
                    {
                        Ccp_ViscaCmdParser(ccp, &cmd_buf[visca_flag], i-visca_flag+1);  //VISCA指令开始解析
                        visca_flag = -1;
                    }
                    else if((pelco_d_flag == -1) || (((i-pelco_d_flag) != 4) && ((i-pelco_d_flag) != 5)))   //不是VISCA的查询指令
                    {
                        pelco_d_flag = i;
                    }
                }
                
                /* PELCO D */
                if((pelco_d_flag != -1) && ((i-pelco_d_flag) == 6))
                {
                    checkout = (uint8_t)(cmd_buf[pelco_d_flag+1]+cmd_buf[pelco_d_flag+2]+cmd_buf[pelco_d_flag+3]+cmd_buf[pelco_d_flag+4]+cmd_buf[pelco_d_flag+5]);
                    if((cmd_buf[pelco_d_flag] == 0xFF) && (cmd_buf[pelco_d_flag+6] == checkout))
                    {
                        Ccp_PelcoDCmdParser(ccp, &cmd_buf[pelco_d_flag], i-pelco_d_flag+1);     //Pelco-D指令解析
                        pelco_d_flag = -1;
                    }
                }
                /* PELCO P */
                else if((pelco_p_flag != -1) && (pelco_p_flag2 != -1))
                {
                    
                    //pelco p 协议校验字节有的包括了第0和第6字节的异或，这里为提高兼容性两种方式都可以
                    checkout = cmd_buf[pelco_p_flag+1]^ cmd_buf[pelco_p_flag+2]^cmd_buf[pelco_p_flag+3]^cmd_buf[pelco_p_flag+4]^ cmd_buf[pelco_p_flag+5];
                    if((cmd_buf[pelco_p_flag+7] == checkout) || (cmd_buf[pelco_p_flag+7] == checkout ^ cmd_buf[pelco_p_flag+0] ^ cmd_buf[pelco_p_flag+6]))
                    {
                        Ccp_PelcoPCmdParser(ccp, &cmd_buf[pelco_p_flag], i-pelco_p_flag+1);     //Pelco-P指令解析
                        pelco_p_flag = -1;
                    }
                }
            }
        } break;
        
        case CCP_VISCA:     //VISCA控制指令或查询指令,只能是外面进来的值,对应的发送函数是发给串口3的
        {
            for(uint32_t i=0; i<len; i++)
            {
                high4bit = H4BIT(cmd_buf[i]);
                low4bit = L4BIT(cmd_buf[i]);
                
                if((high4bit == 8) && (low4bit > 0) && (low4bit < 9))
                {
                    visca_flag = i;
                }
                else if(cmd_buf[i] == 0xFF)
                {
                    /* VISCA */
                    if(visca_flag != -1)
                    {
                        Ccp_ViscaCmdParser(ccp, &cmd_buf[visca_flag], i-visca_flag+1);
                        visca_flag = -1;
                    }
                }
            }
        } break;
        
        case CCP_VISCA_INQ:     //数据为回码
        {
            for(uint32_t i=0; i<len; i++)
            {
                high4bit = H4BIT(cmd_buf[i]);
                low4bit = L4BIT(cmd_buf[i]);
                
                if((low4bit == 0) && (high4bit >= 9) && (low4bit <= 0xF))
                {
                    visca_inq_flag = i;
                }
                else if(cmd_buf[i] == 0xFF)
                {
                    /* VISCA RETURN*/
                    if(visca_inq_flag != -1)
                    {
                        Ccp_ViscaRetCmdParser(ccp, &cmd_buf[visca_inq_flag], i-visca_inq_flag+1);       //VISCA回码指令解析
                        visca_inq_flag = -1;
                    }
                }
            }
        } break;
        
        case CCP_VISCA_ALL:
        {
            for(uint32_t i=0; i<len; i++)
            {
                high4bit = H4BIT(cmd_buf[i]);
                low4bit = L4BIT(cmd_buf[i]);
                
                if((high4bit == 8) && (low4bit > 0) && (low4bit < 9))
                {
                    visca_flag = i;
                }
                else if((low4bit == 0) && (high4bit >= 9) && (low4bit <= 0xF))
                {
                    visca_inq_flag = i;
                }
                else if(cmd_buf[i] == 0xFF)
                {
                    /* VISCA RETURN*/
                    if(visca_flag != -1)
                    {
                        Ccp_ViscaRetCmdParser(ccp, &cmd_buf[visca_flag], i-visca_flag+1);
                        visca_flag = -1;
                    }
                    
                    if(visca_inq_flag != -1)
                    {
                        Ccp_ViscaRetCmdParser(ccp, &cmd_buf[visca_inq_flag], i-visca_inq_flag+1);
                        visca_inq_flag = -1;
                    }
                }
            }
        } break;
        
        case CCP_PELCO_D:
        {
            for(uint32_t i=0; i<len; i++)
            {
                if(cmd_buf[i] == 0xFF)
                {
                    if((pelco_d_flag == -1) || (((i-pelco_d_flag) != 4) && ((i-pelco_d_flag) != 5)))
                    {
                        pelco_d_flag = i;
                    }
                }
                else if((pelco_d_flag != -1) && ((i-pelco_d_flag) == 6))
                {
                    checkout = (uint8_t)(cmd_buf[pelco_d_flag+1]+cmd_buf[pelco_d_flag+2]+cmd_buf[pelco_d_flag+3]+cmd_buf[pelco_d_flag+4]+cmd_buf[pelco_d_flag+5]);
                    if((cmd_buf[pelco_d_flag] == 0xFF) && (cmd_buf[pelco_d_flag+6] == checkout))
                    {
                        Ccp_PelcoDCmdParser(ccp, &cmd_buf[pelco_d_flag], i-pelco_d_flag+1);
                        pelco_d_flag = -1;
                    }
                }
            }
        } break;
        
        case CCP_PELCO_P:
        {
            for(uint32_t i=0; i<len; i++)
            {
                if(cmd_buf[i] == 0xA0)
                {
                    pelco_p_flag = i;
                }
                else if(cmd_buf[i] == 0xAF)
                {
                    if((pelco_p_flag != -1) && ((i-pelco_p_flag) == 6))
                    {
                        pelco_p_flag2 = i;
                    }
                }
                else if((pelco_p_flag != -1) && (pelco_p_flag2 != -1))
                {
                    //pelco p 协议校验字节有的包括了第0和第6字节的异或，这里为提高兼容性两种方式都可以
                    checkout = cmd_buf[pelco_p_flag+1]^ cmd_buf[pelco_p_flag+2]^cmd_buf[pelco_p_flag+3]^cmd_buf[pelco_p_flag+4]^ cmd_buf[pelco_p_flag+5];
                    if((cmd_buf[pelco_p_flag+7] == checkout) || (cmd_buf[pelco_p_flag+7] == checkout ^ cmd_buf[pelco_p_flag+0] ^ cmd_buf[pelco_p_flag+6]))
                    {
                        Ccp_PelcoPCmdParser(ccp, &cmd_buf[pelco_p_flag], i-pelco_p_flag+1);
                        pelco_p_flag = -1;
                    }
                }
            }
        } break;
        
        default:
        break;
    }
    
    return 0;
}

/***************************************************************************************************
* Description:  从接收缓存中提取回码，中间提取到的控制指令存到缓存中
***************************************************************************************************/
int32_t Ccp_GetViscaInqCmd(struct ccp_chn *ccp, struct ccp_chn *cache, uint8_t *buffer)
{
    static uint8_t inq_cmd_buf[128];
    int32_t len;
    
    int8_t visca_flag = -1;
    int8_t visca_inq_flag = -1;

    uint8_t high4bit, low4bit;

    /* 从缓存中取指令 */
    len = ccp->get_packet(inq_cmd_buf);
    if(len <= 0)
        return 0;
    
    for(uint32_t i=0; i<len; i++)
    {
        high4bit = H4BIT(inq_cmd_buf[i]);
        low4bit = L4BIT(inq_cmd_buf[i]);
        
        if((high4bit == 8) && (low4bit > 0) && (low4bit < 9))
        {
            visca_flag = i;
        }
        else if((low4bit == 0) && (high4bit >= 9) && (low4bit <= 0xF))
        {
            visca_inq_flag = i;
        }
        else if(inq_cmd_buf[i] == 0xFF)
        {
            /* VISCA INQUREY */
            if(visca_inq_flag != -1)
            {
                int32_t size = i-visca_inq_flag+1;
                if(size <= 16)
                    memcpy(buffer, &inq_cmd_buf[visca_inq_flag], size);     //如果是回码的话就将回码复制走
                //                memset(&inq_cmd_buf[visca_inq_flag], 0, size);
                visca_inq_flag = -1;
                if(size <= 16)
                {
                    if((i+1) < len)
                        cache->add_packet(&inq_cmd_buf[i+1], len-i-1);
                    return size;
                }
            }
            
            /* VISCA CMD */
            if(visca_flag != -1)
            {
                /* 添加到缓存中 */
                cache->add_packet(&inq_cmd_buf[visca_flag], i-visca_flag+1);
                memset(&inq_cmd_buf[visca_inq_flag], 0, i-visca_flag+1);
                visca_flag = -1;
            }
        }
    }
    
    return 1;
}

/***************************************************************************************************
* Description:  协议解析进程
***************************************************************************************************/
int32_t Ccp_ProcessHandler(void)
{
    struct list_head *temp;
    struct ccp_chn *ccp;
    
    list_for_each(temp, &ccp_list)
    {
        ccp = list_entry(temp, struct ccp_chn, list);
        Ccp_CmdParser(ccp);     //轮询每一个通道并进行数据处理
    }
    
    return 0;
}







/****************************************** END OF FILE *******************************************/
