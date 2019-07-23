/***************************************************************************************************
* FILE: net.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/23, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "net.h"
#include "system.h"
#include "api.h"
#include "ui_framework.h"

#include "lwip/init.h"
#include "lwip/netif.h"

#include "ip_addr.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/
extern ETH_HandleTypeDef heth;


uint8_t LinkState = 0;

struct netif gnetif;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;



/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
int32_t NetCheckLinkState(void);


/***************************************************************************************************
* Description:  
***************************************************************************************************/




/***************************************************************************************************
* Description:  
***************************************************************************************************/
void func_NetTask(void * argument)
{
    
    while(1)
    {
        vTaskDelay(500);
        NetCheckLinkState();
    }
}



/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t NetCheckLinkState(void)
{
    uint32_t reg_value = 0;
    
    HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &reg_value);
    
    if(reg_value & PHY_LINKED_STATUS)
    {
        if(gnetif.flags & NETIF_FLAG_LINK_UP)
        {
            
        }
        else
        {
            printf("Link Up!\r\n");
            gnetif.flags |= NETIF_FLAG_LINK_UP;
            
            if(LinkState == 0)
            {
                LinkState = 1;
                HAL_ETH_PhyInit(&heth);
                HAL_ETH_Start(&heth);
                netif_set_up(&gnetif);
            }
            
            dhcp_start(&gnetif);
            
            UI_ShowNetState();
        }
        
        return 1;
    }
    else
    {
        if(gnetif.flags & NETIF_FLAG_LINK_UP)
        {
            printf("Link Down!\r\n");
            gnetif.flags &= (~NETIF_FLAG_LINK_UP);
            dhcp_stop(&gnetif);
            
            UI_ShowNetState();
        }
        else
        {
            
        }
        
        
        return 0;
    }
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
void LWIP_Init(void)
{
    ETH_RST = 0;
    HAL_Delay(100);
    ETH_RST = 1;
    HAL_Delay(200);
    /* Initilialize the LwIP stack with RTOS */
    tcpip_init( NULL, NULL );
    
    /* IP addresses initialization with DHCP (IPv4) */
    if(stSysPara.net.dhcp)
    {
        ipaddr.addr = 0;
        netmask.addr = 0;
        gw.addr = 0;
    }
    else
    {
        ipaddr.addr = SwapIp(stSysPara.net.ipaddr);
        netmask.addr = SwapIp(stSysPara.net.mask);
        gw.addr = SwapIp(stSysPara.net.gateway);
    }
    
    /* add the network interface (IPv4/IPv6) with RTOS */
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
    
    /* Registers the default network interface */
    netif_set_default(&gnetif);
    
    if (netif_is_link_up(&gnetif))
    {
        /* When the netif is fully configured this function must be called */
        netif_set_up(&gnetif);
    }
    else
    {
        /* When the netif link is down this function must be called */
        netif_set_down(&gnetif);
    }
    
    /* Start DHCP negotiation for a network interface (IPv4) */
    if(stSysPara.net.dhcp)
    {
        dhcp_start(&gnetif);
    }
    
}

/****************************************** END OF FILE *******************************************/
