/***************************************************************************************************
* FILE: bsp.h

* DESCRIPTION:
  --

* CREATED: 2016/12/26 by Kevin
****************************************************************************************************
* MODIFY:  --

* DATE: --

* DESCRIPTION: --

***************************************************************************************************/
#ifndef __BSP_H
#define __BSP_H
//C Standard Library
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//STM32 Standard Peripheral Library 
#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_can.h"
#include "stm32f10x_cec.h"
#include "stm32f10x_crc.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_dbgmcu.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_fsmc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_sdio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_wwdg.h"
#include "misc.h" /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */



#include "port.h"
#include "eeprom.h"
#include "uart.h"
#include "i2c.h"
#include "spi.h"

#include "cy3014.h"





/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define RETRY(exp, times)   \
do { \
    int32_t retry = times; \
    while(exp) \
    { \
        if(retry <= 0) break; \
            --retry; \
    } \
}while(0)
 


/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void BSP_Init(void);
void delay_us(uint16_t nus);
void delay_ms(uint32_t nms);


#endif
/******************************* (C) COPYRIGHT 2016 Shenzhen JJTS ***********END OF FILE***********/


