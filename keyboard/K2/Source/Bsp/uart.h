/***************************************************************************************************
* FILE: uart.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/1, by kevin
***************************************************************************************************/
#ifndef __UART_H
#define __UART_H

#include "stm32f2xx_hal.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/



/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/
extern UART_HandleTypeDef hUart1;
extern UART_HandleTypeDef hUart4;
extern UART_HandleTypeDef hUart6;

extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;
extern DMA_HandleTypeDef hdma_usart6_rx;

extern struct packet_list UartRxList;
/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void UART1_Init(uint32_t baudrate);
void UART4_Init(uint32_t baudrate);
void UART6_Init(uint32_t baudrate);

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle);
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle);


void UART_Init(void);
void UartSentPacket(const uint8_t *pk, uint8_t length);



#endif
/****************************************** END OF FILE *******************************************/
