/***************************************************************************************************
* FILE: ir_remote.h
*
* DESCRIPTION: --
*
* CREATED: 2018/12/2, by kevin
***************************************************************************************************/
#ifndef __IR_REMOTE_H__
#define __IR_REMOTE_H__

#include "ir_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif


void IR_AddrDetection(void);
void IrRemoter_Init(void);
void IrRemoter_CmdHandler(void);


#ifdef __cplusplus
}
#endif

#endif
/****************************************** END OF FILE *******************************************/
