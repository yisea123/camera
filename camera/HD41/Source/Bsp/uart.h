/***************************************************************************************************
* FILE: usart.h

* DESCRIPTION:
  --

* CREATED: 2016/12/26 by Kevin
****************************************************************************************************
* MODIFY:  --

* DATE: --

* DESCRIPTION: --

***************************************************************************************************/
#ifndef __UART_H
#define __UART_H
#include "bsp.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define PRINTF_BY_SERIAL				USART1


/*********************************************************************************************************
* ÉùÃ÷
*********************************************************************************************************/
void UART1_Init(uint32_t baud);
void UART1_SendByte(uint8_t byt);
void UART1_SendString(char *str);
uint8_t UART1_SendArray(const uint8_t *arr, uint16_t length);
int32_t UART1_DmaSend(const uint8_t *buffer, uint16_t length);

void UART2_Init(uint32_t baud);
void UART2_SendByte(uint8_t byt);
void UART2_SendString(char *str);
uint8_t UART2_SendArray(uint8_t *arr, uint16_t length);
int32_t UART2_DmaSend(const uint8_t *buffer, uint16_t length);

void UART3_Init(uint32_t baud);
void UART3_SendByte(uint8_t byt);
void UART3_SendString(char *str);
uint8_t UART3_SendArray(uint8_t *arr, uint16_t length);
int32_t UART3_DmaSend(const uint8_t *buffer, uint16_t length);

void UART4_Init(uint32_t baud);
void UART4_SendByte(uint8_t byt);
void UART4_SendString(char *str);
uint8_t UART4_SendArray(uint8_t *arr, uint16_t length);
int32_t UART4_DmaSend(const uint8_t *buffer, uint16_t length);

void UART5_Init(uint32_t baud);
void UART5_SendByte(uint8_t byt);
void UART5_SendString(char *str);
uint8_t UART5_SendArray(uint8_t *arr, uint16_t length);


#endif
/****************************************** END OF FILE *******************************************/

