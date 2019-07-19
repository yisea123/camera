/***************************************************************************************************
* FILE: ccp_fac.h
*
* DESCRIPTION: --
*
* CREATED: 2018/11/29, by kevin
***************************************************************************************************/
#ifndef __CCP_FAC_H__
#define __CCP_FAC_H__

#include "ccp.h"

#ifdef __cplusplus
extern "C" {
#endif
    


void Ccp_FacCmdParser(struct ccp_chn *chn, uint8_t *buf, uint8_t length);

#ifdef __cplusplus
}
#endif

#endif
/****************************************** END OF FILE *******************************************/
