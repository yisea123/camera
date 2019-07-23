/***************************************************************************************************
* FILE: ptc.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/25, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "ptc.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/
 event_handler_t const *pevent_handler = NULL;


/***************************************************************************************************
* VARIABLES
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/



/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t Ptc_DeInit(int32_t type)
{
    int32_t ret;
    switch(type)
    {
        case VISCA_IP:
        ret = ViscaIp_DeInit();
        break;
        case ONVIF:
        ret = Onvif_DeInit();
        break;
        case VISCA:
        ret = Visca_DeInit();
        break;
        case PELCO_D:
        ret = PelcoD_DeInit();
        break;
        case PELCO_P:
        ret = PelcoP_DeInit();
        break;
        case PANASONIC:
        ret = Panasonic_DeInit();
        break;
        case SAMSUNG:
        ret = Samsung_DeInit();
        break;
        default:
        
        break;
    }
    
    return ret;
}


/***************************************************************************************************
* Description:  
***************************************************************************************************/
int32_t Ptc_Init(void)
{
    static int32_t type_backup = -1;
    int32_t ret;
    
    if(type_backup >= 0)
        if(Ptc_DeInit(type_backup))
            return 1;
    
    switch(stSysPara.ptc.type)
    {
        case VISCA_IP:
        ret = ViscaIp_Init();
        break;
        case ONVIF:
        ret = Onvif_Init();
        break;
        case VISCA:
        ret = Visca_Init();
        break;
        case PELCO_D:
        ret = PelcoD_Init();
        break;
        case PELCO_P:
        ret = PelcoP_Init();
        break;
        case PANASONIC:
        ret = Panasonic_Init();
        break;
        case SAMSUNG:
        ret = Samsung_Init();
        break;
        default:
        
        break;
    }
    
    if(ret == 0)
    {
        type_backup = stSysPara.ptc.type;
        return 0;
    }
    else
        return 1;
}

/***************************************************************************************************
* Description:  获取当前协议的当前相机地址
***************************************************************************************************/
uint32_t GetCurAddr(void)
{
    if(pevent_handler == NULL || pevent_handler->get_current_addr == NULL)
    {
        return 0;
    }
    return pevent_handler->get_current_addr();
}



/****************************************** END OF FILE *******************************************/
