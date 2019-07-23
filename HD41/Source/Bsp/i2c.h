/***************************************************************************************************
* FILE: i2c.h
*
* DESCRIPTION:  MCU I2CÇý¶¯
*
* CREATED: 2017/8/15, by kevin xu
***************************************************************************************************/
#ifndef __I2C_H__
#define __I2C_H__

#include "stdint.h"


void I2C1_Init(void);
uint8_t I2C1_WriteOneByte(uint8_t slave, uint8_t offset, uint8_t dat);
uint8_t I2C1_ReadOneByte(uint8_t slave, uint8_t offset, uint8_t *dat);



#endif
/****************************************** END OF FILE *******************************************/
