/***************************************************************************************************
* FILE: rocker.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/21, by kevin
***************************************************************************************************/
#ifndef __ROCKER_H_
#define __ROCKER_H_

#include "stdint.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define ROCKER_PAN 0
#define ROCKER_TILT 1
#define ROCKER_ZOOM 2

struct rocker_para
{
    int32_t pan_reverse;
    int32_t tilt_reverse;
    int32_t pan_pos_min;
    int32_t pan_pos_max;
    int32_t pan_neg_min;
    int32_t pan_neg_max;
    int32_t tilt_pos_min;
    int32_t tilt_pos_max;
    int32_t tilt_neg_min;
    int32_t tilt_neg_max;
};

/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void Rocker_Refresh(void);
int Rocker_GetPanVal(int8_t *val, uint8_t mode);
int Rocker_GetTiltVal(int8_t *val, uint8_t mode);
int Rocker_GetZoomVal(int8_t *val, uint8_t mode);
int Rocker_GetPanTiltVal(int8_t *pan_val, int8_t *tilt_val);
#endif
/****************************************** END OF FILE *******************************************/
