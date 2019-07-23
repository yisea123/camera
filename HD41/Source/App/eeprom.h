/***************************************************************************************************
* FILE: eeprom.h
*
* DESCRIPTION: --
*
* CREATED: 2017/2/24 by Kevin
***************************************************************************************************/
#ifndef __EEPROM_H
#define __EEPROM_H
#include "bsp.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void EEPROM_Init(void);
uint8_t EEPROM_ReadOneByte(uint16_t addr, uint8_t *dat);
uint8_t EEPROM_WriteOneByte(uint16_t addr, uint8_t byt);
uint8_t EEPROM_Read(uint16_t addr, uint8_t *buf, uint16_t length);
uint8_t EEPROM_Write(uint16_t addr, const uint8_t *buf,uint16_t length);

#endif

/****************************************** END OF FILE *******************************************/



