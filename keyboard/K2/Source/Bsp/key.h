/***************************************************************************************************
* FILE: key.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/7, by kevin
***************************************************************************************************/
#ifndef __KEY_H_
#define __KEY_H_


#include "gpio.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define K1  0
#define K2  1
#define K3  2
#define K4  3
#define K5  4
#define K6  5
#define K7  6
#define K8  7
#define K9  8
#define K10 9
#define K11 10
#define K12 11
#define K13 12
#define K14 13
#define K15 14
#define K16 15
#define K17 16
#define K18 17
#define K19 18
#define K20 19
#define K21 20
#define K22 21
#define K23 22
#define K24 23
#define K25 24
#define K26 25
#define K27 26
#define K28 27
#define K29 28
#define K30 29
#define K31 30
#define K32 31
#define K33 32
#define K34 33


#define KEY_ROCKER          K34

#define KEY_MENU            K14
#define KEY_BACK            K17
#define KEY_LOCK            K30
#define KEY_SP_ADD          K33
#define KEY_SP_SUB          K32

#define KEY_SEARCH          K9
#define KEY_CAMERA          K10
#define KEY_PRESET_CALL     K13
#define KEY_PRESET_SET      K12
#define KEY_PRESET_DELETE   K11

#define KEY_OSD             K20
#define KEY_BACKLIGHT       K25
#define KEY_AE_MODE         K28
#define KEY_AE_ADD          K31
#define KEY_AE_SUB          K29
#define KEY_WB_MODE         K3
#define KEY_RGAIN_ADD       K2
#define KEY_RGAIN_SUB       K1
#define KEY_BGAIN_ADD       K5
#define KEY_BGAIN_SUB       K4
#define KEY_FOCUS_AUTO      K6
#define KEY_FOCUS_FAR       K8
#define KEY_FOCUS_NEAR      K7

#define KEY_NUM1            K15
#define KEY_NUM2            K18
#define KEY_NUM3            K21
#define KEY_NUM4            K23
#define KEY_NUM5            K26
#define KEY_NUM6            K16
#define KEY_NUM7            K19
#define KEY_NUM8            K22
#define KEY_NUM9            K24
#define KEY_NUM0            K27



/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void Key_Init(void);
void KeyScan(void);
uint8_t GetKey(uint8_t n, uint8_t mode);
int8_t GetKeyNum(void);

#endif
/****************************************** END OF FILE *******************************************/
