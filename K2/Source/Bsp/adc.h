/***************************************************************************************************
* FILE: adc.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/8, by kevin
***************************************************************************************************/
#ifndef __ADC_H_
#define __ADC_H_
#include "stm32f2xx_hal.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void ADC1_Init(void);


#endif
/****************************************** END OF FILE *******************************************/
