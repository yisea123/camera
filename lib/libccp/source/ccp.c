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

/* �汾���� */
#define STR(s)     #s
#define VERSION(a, b, c)    STR(a) "." STR(b) "." STR(c)
#define DATE(y, m, d)       STR(y) "." STR(m) "." STR(d)
//static const char version[] = VERSION(VER_MAJOR, VER_MINOR1, VER_MINOR2);
static const char version_full[] = VERSION(VER_MAJOR, VER_MINOR1, VER_MINOR2) " " DATE(DATE_YEAR, DATE_MONTH, DATE_DAY);


/* ͨ������ */
static LIST_HEAD(ccp_list);     //���ڴ��ͨ���ṹ�������ͷ

struct ccp_ops *ccp_comm_ops;   //������������

struct ccp_chn *ccp_cam_p=NULL, *ccp_cache_p=NULL, *ccp_cam2_p=NULL, *ccp_cache2_p=NULL, *ccp_in1_p=NULL, *ccp_out1_p=NULL;

/***************************************************************************************************
* Description:  ��ȡ�汾��
***************************************************************************************************/
const char* CCP_GetVersion(void)
{
    return version_full;
}

/***************************************************************************************************
* Description:  ��ʼ��
***************************************************************************************************/
int32_t Ccp_Init(struct ccp_ops *ops)
{
    if(ops == NULL)
        return 1;
    
    ccp_comm_ops = ops;
    return 0;
}

/***************************************************************************************************
* Description:  ���ͨ��
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
* Description:  ����ͨ��
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
* Description:  ��ȡ������ָ��
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
    
    /* �ӻ�����ȡָ�� */
    len = ccp->get_packet(cmd_buf);     //��USART1_RX��USART3_RX��ȡ����    
    if(len <= 0)
        return 0;
    
    if(ccp_comm_ops->cmd_debug && ccp_comm_ops->cmd_debug_show != NULL)
    {
        ccp_comm_ops->cmd_debug_show(cmd_buf, len);
    }
    
    if(ccp_comm_ops->direct_link_cam2in1)       //����ģʽ����
    {
        if(ccp->path_id == 0)                   //ȡ�������ǻ�о������������
        {
            ccp_in1_p->send_packet(cmd_buf, len);       //ֱ�ӷ����ⲿ����
        }
        else if(ccp->path_id == 1)              //ȡ���������ⲿ���ڷ�����������
        {
            ccp_cam_p->send_packet(cmd_buf, len);  //�������жϴ���ֱ�ӷ�����о
        }
        return 0;
    }
    
    /* ���ݴ�ͨ����Э����ȡָ����� */
    switch(ccp->type)
    {
        case CCP_AUTO:      //�˵���ѡ��ΪAUTO,��߽����ж�����һ��Э������
        {
            for(uint32_t i=0; i<len; i++)
            {
                high4bit = H4BIT(cmd_buf[i]);
                low4bit = L4BIT(cmd_buf[i]);
                /* VISCA��־ */
                if((high4bit == 8) && (low4bit > 0) && (low4bit < 9))
                {
                    visca_flag = i;
                }
                /* Pelco-P��־ */
                if(cmd_buf[i] == 0xA0)
                {
                    pelco_p_flag = i; 
                }
                
                if(cmd_buf[i] == 0xAF)
                {
                    if((pelco_p_flag != -1) && ((i-pelco_p_flag)==6))
                    {
                        pelco_p_flag2 = i;      //Ϊ6ʱ��Pelco-P
                    }
                }
                
                if(cmd_buf[i] == 0xFF)
                {
                    /* VISCA */
                    if(visca_flag != -1)
                    {
                        Ccp_ViscaCmdParser(ccp, &cmd_buf[visca_flag], i-visca_flag+1);  //VISCAָ�ʼ����
                        visca_flag = -1;
                    }
                    else if((pelco_d_flag == -1) || (((i-pelco_d_flag) != 4) && ((i-pelco_d_flag) != 5)))   //����VISCA�Ĳ�ѯָ��
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
                        Ccp_PelcoDCmdParser(ccp, &cmd_buf[pelco_d_flag], i-pelco_d_flag+1);     //Pelco-Dָ�����
                        pelco_d_flag = -1;
                    }
                }
                /* PELCO P */
                else if((pelco_p_flag != -1) && (pelco_p_flag2 != -1))
                {
                    
                    //pelco p Э��У���ֽ��еİ����˵�0�͵�6�ֽڵ��������Ϊ��߼��������ַ�ʽ������
                    checkout = cmd_buf[pelco_p_flag+1]^ cmd_buf[pelco_p_flag+2]^cmd_buf[pelco_p_flag+3]^cmd_buf[pelco_p_flag+4]^ cmd_buf[pelco_p_flag+5];
                    if((cmd_buf[pelco_p_flag+7] == checkout) || (cmd_buf[pelco_p_flag+7] == checkout ^ cmd_buf[pelco_p_flag+0] ^ cmd_buf[pelco_p_flag+6]))
                    {
                        Ccp_PelcoPCmdParser(ccp, &cmd_buf[pelco_p_flag], i-pelco_p_flag+1);     //Pelco-Pָ�����
                        pelco_p_flag = -1;
                    }
                }
            }
        } break;
        
        case CCP_VISCA:     //VISCA����ָ����ѯָ��,ֻ�������������ֵ,��Ӧ�ķ��ͺ����Ƿ�������3��
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
        
        case CCP_VISCA_INQ:     //����Ϊ����
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
                        Ccp_ViscaRetCmdParser(ccp, &cmd_buf[visca_inq_flag], i-visca_inq_flag+1);       //VISCA����ָ�����
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
                    //pelco p Э��У���ֽ��еİ����˵�0�͵�6�ֽڵ��������Ϊ��߼��������ַ�ʽ������
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
* Description:  �ӽ��ջ�������ȡ���룬�м���ȡ���Ŀ���ָ��浽������
***************************************************************************************************/
int32_t Ccp_GetViscaInqCmd(struct ccp_chn *ccp, struct ccp_chn *cache, uint8_t *buffer)
{
    static uint8_t inq_cmd_buf[128];
    int32_t len;
    
    int8_t visca_flag = -1;
    int8_t visca_inq_flag = -1;

    uint8_t high4bit, low4bit;

    /* �ӻ�����ȡָ�� */
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
                    memcpy(buffer, &inq_cmd_buf[visca_inq_flag], size);     //����ǻ���Ļ��ͽ����븴����
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
                /* ��ӵ������� */
                cache->add_packet(&inq_cmd_buf[visca_flag], i-visca_flag+1);
                memset(&inq_cmd_buf[visca_inq_flag], 0, i-visca_flag+1);
                visca_flag = -1;
            }
        }
    }
    
    return 1;
}

/***************************************************************************************************
* Description:  Э���������
***************************************************************************************************/
int32_t Ccp_ProcessHandler(void)
{
    struct list_head *temp;
    struct ccp_chn *ccp;
    
    list_for_each(temp, &ccp_list)
    {
        ccp = list_entry(temp, struct ccp_chn, list);
        Ccp_CmdParser(ccp);     //��ѯÿһ��ͨ�����������ݴ���
    }
    
    return 0;
}







/****************************************** END OF FILE *******************************************/
