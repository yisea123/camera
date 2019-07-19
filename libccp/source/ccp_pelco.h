/***************************************************************************************************
* FILE: ccp_pelco.h
*
* DESCRIPTION: --
*
* CREATED: 2018/11/29, by kevin
***************************************************************************************************/
#ifndef __CCP_PELCO_H__
#define __CCP_PELCO_H__

#include "ccp.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    
#define PELCO_D_IRIS_DOWN					(1<<10)
#define PELCO_D_IRIS_ADD					(1<<9)
#define PELCO_D_FOCUS_NEAR				    (1<<8)
#define PELCO_D_FOCUS_FAR					(1<<7)
#define PELCO_D_ZOOM_WIDE					(1<<6)
#define PELCO_D_ZOOM_TELE					(1<<5)
#define PELCO_D_PTZ_DOWN					(1<<4)
#define PELCO_D_PTZ_UP						(1<<3)
#define PELCO_D_PTZ_LEFT					(1<<2)
#define PELCO_D_PTZ_RIGHT					(1<<1)
#define PELCO_D_PRESET_SET			    	(0x0003)
#define PELCO_D_PRESET_CLEAR		    	(0x0005)
#define PELCO_D_PRESET_CALL				    (0x0007)
#define PELCO_D_POWER_ON                    (0x8800)
#define PELCO_D_POWER_OFF                   (0x0800)
#define PELCO_D_FOCUS_MODE                  (0x002B)

#define PELCO_P_CAMERA_ON					(1<<14)
#define PELCO_P_AUTOSCAN					(1<<13)
#define PELCO_P_CAMERA_OFF			    	(1<<12)
#define PELCO_P_IRIS_DOWN					(1<<11)
#define PELCO_P_IRIS_ADD					(1<<10)
#define PELCO_P_FOCUS_NEAR				    (1<<9)
#define PELCO_P_FOCUS_FAR					(1<<8)
#define PELCO_P_ZOOM_WIDE					(1<<6)
#define PELCO_P_ZOOM_TELE					(1<<5)
#define PELCO_P_PTZ_DOWN					(1<<4)
#define PELCO_P_PTZ_UP						(1<<3)
#define PELCO_P_PTZ_LEFT					(1<<2)
#define PELCO_P_PTZ_RIGHT					(1<<1)
#define PELCO_P_PRESET_SET			    	(0x0003)
#define PELCO_P_PRESET_CLEAR		    	(0x0005)
#define PELCO_P_PRESET_CALL				    (0x0007)
#define PELCO_P_POWER_ON                    (0x8800)
#define PELCO_P_POWER_OFF                   (0x0800)
#define PELCO_P_FOCUS_MODE                  (0x002B)
    
    
/* 特殊预置位
95  --  调用菜单
243 --  启动跟踪
244 --  关闭跟踪
*/

void Ccp_PelcoDCmdParser(struct ccp_chn *chn, uint8_t *buf, uint8_t length);
void Ccp_PelcoPCmdParser(struct ccp_chn *chn, uint8_t *buf, uint8_t length);

#ifdef __cplusplus
}
#endif

#endif
/****************************************** END OF FILE *******************************************/
