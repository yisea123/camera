/***************************************************************************************************
* FILE: sii9022.h
*
* DESCRIPTION: --
*
* CREATED: 2018/12/31, by kevin
***************************************************************************************************/
#ifndef __SII9022_H__
#define __SII9022_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

    
uint8_t Sii9022_Init(void);
uint8_t Sii9022_SetVideoFormat(uint8_t res);


#ifdef __cplusplus
}
#endif

#endif
/****************************************** END OF FILE *******************************************/

