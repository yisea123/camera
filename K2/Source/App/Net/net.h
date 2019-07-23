/***************************************************************************************************
* FILE: net.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/23, by kevin
***************************************************************************************************/
#ifndef __NET_H_
#define __NET_H_

#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "ethernetif.h"

#include "lwip/tcpip.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define IPV4_ADDR(a, b, c, d)   (((uint32_t)a)+((uint32_t)b<<8)+((uint32_t)c<<16)+((uint32_t)d<<24))


/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/
extern struct netif gnetif;
/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void func_NetTask(void * argument);

void LWIP_Init(void);

int ScanViscaIpDevice(void);

#endif
/****************************************** END OF FILE *******************************************/