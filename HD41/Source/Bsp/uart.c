/***************************************************************************************************
* FILE: uart.c

* DESCRIPTION: 串口驱动
*   USART1_TX-->DMA1CH4
*   USART1_RX-->DMA1CH5
*   USART2_TX-->DMA1CH7
*   USART2_RX-->DMA1CH6
*   USART3_TX-->DMA1CH2
*   USART3_RX-->DMA1CH3
*   UART4_TX -->DMA2CH5
*   UART4_RX -->DMA2CH3

* CREATED: 2018/10/10 by Kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/
#include "uart.h"
#include "packet_list.h"
#include "ptc.h"

/***************************************************************************************************
* LOCAL DEFINES
***************************************************************************************************/
#define UART_TX_BUFFER_SIZE     100
#define UART_RX_BUFFER_SIZE     100

#define UART1_TX_BUFFER_SIZE    UART_TX_BUFFER_SIZE
#define UART2_TX_BUFFER_SIZE    UART_TX_BUFFER_SIZE
#define UART3_TX_BUFFER_SIZE    UART_TX_BUFFER_SIZE
#define UART4_TX_BUFFER_SIZE    UART_TX_BUFFER_SIZE

#define UART1_RX_BUFFER_SIZE    UART_RX_BUFFER_SIZE
#define UART2_RX_BUFFER_SIZE    UART_RX_BUFFER_SIZE
#define UART3_RX_BUFFER_SIZE    UART_RX_BUFFER_SIZE
#define UART4_RX_BUFFER_SIZE    UART_RX_BUFFER_SIZE
#define UART5_RX_BUFFER_SIZE    UART_RX_BUFFER_SIZE

/***************************************************************************************************
* LOCAL GLOBAL VARIABLES
***************************************************************************************************/
static uint8_t UART1_TX_BUFFER[UART1_TX_BUFFER_SIZE] = {0};
static uint8_t UART2_TX_BUFFER[UART2_TX_BUFFER_SIZE] = {0};
static uint8_t UART3_TX_BUFFER[UART3_TX_BUFFER_SIZE] = {0};

static uint8_t UART1_RX_BUFFER[UART1_RX_BUFFER_SIZE] = {0};
static uint8_t UART2_RX_BUFFER[UART2_RX_BUFFER_SIZE] = {0};
static uint8_t UART3_RX_BUFFER[UART3_RX_BUFFER_SIZE] = {0};

static uint8_t UART1_TX_BUSY = 0;
static uint8_t UART2_TX_BUSY = 0;
static uint8_t UART3_TX_BUSY = 0;


extern struct packet_list plCameraRx;
extern struct packet_list plCamCache;
extern struct packet_list plInput1Rx;
extern struct packet_list plOutputRx;

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
void UART1_Init(uint32_t baud)
{
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    DMA_InitTypeDef DMA_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    USART_DeInit(USART1);
    DMA_DeInit(DMA1_Channel4);
    DMA_DeInit(DMA1_Channel5);
    
    /* 中断优先级 */
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel4_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Init(&NVIC_InitStruct);
    
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_Init(&NVIC_InitStruct);
    
    /* 串口参数 */
    USART_InitStruct.USART_BaudRate = baud;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;	
    USART_Init(USART1, &USART_InitStruct);
    
    /* DMA发送 */
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)UART1_TX_BUFFER;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST; //存储器到外设
    DMA_InitStruct.DMA_BufferSize = 0;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &DMA_InitStruct);
    
    /* DMA接收 */
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)UART1_RX_BUFFER;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;     //外设到存储器
    DMA_InitStruct.DMA_BufferSize = UART1_RX_BUFFER_SIZE;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStruct);
    
    /* 开中断 */
    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    
    DMA_Cmd(DMA1_Channel5, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    USART_Cmd(USART1, ENABLE);
    
    UART1_TX_BUSY = 0;
}

void UART1_SendByte(uint8_t byt)
{
    while((USART1->SR&0X40)==0);    //等待上一次串口数据发送完成
    USART1->DR = byt;      			//写DR,串口1将发送数据
}

void UART1_SendString(char *str)
{
    while(*str != '\0')
    {
        UART1_SendByte(*str);
        str++;
    }
}

uint8_t UART1_SendArray(const uint8_t *arr, uint16_t length)
{
    uint8_t i;
    for(i=0; i<length; i++)
    {
        UART1_SendByte(arr[i]);
    }
    return 0;
}

int32_t UART1_DmaSend(const uint8_t *buffer, uint16_t length)
{
    while(UART1_TX_BUSY);
//        return 1;
    
    if(length > UART1_TX_BUFFER_SIZE)
        length = UART1_TX_BUFFER_SIZE;
    memcpy(UART1_TX_BUFFER, buffer, length);
    
    UART1_TX_BUSY = 1;
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA1_Channel4->CNDTR = length;
    DMA1_Channel4->CMAR = (uint32_t)UART1_TX_BUFFER;
    DMA_Cmd(DMA1_Channel4, ENABLE);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void UART2_Init(uint32_t baud)
{
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    DMA_InitTypeDef DMA_InitStruct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    USART_DeInit(USART2);
    DMA_DeInit(DMA1_Channel7);
    DMA_DeInit(DMA1_Channel6);
    
    /* 中断优先级 */
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel7_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_Init(&NVIC_InitStruct);
    
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel6_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_Init(&NVIC_InitStruct);
    
    /* 串口参数 */
    USART_InitStruct.USART_BaudRate = baud;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx;
    USART_InitStruct.USART_Mode |= USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;	
    USART_Init(USART2, &USART_InitStruct);
    
    /* DMA发送 */
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr = 0;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = 0;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &DMA_InitStruct);
    
    /* DMA接收 */
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)UART2_RX_BUFFER;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = UART2_RX_BUFFER_SIZE;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6, &DMA_InitStruct);
    
    /* 开中断 */
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
    DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    
    DMA_Cmd(DMA1_Channel6, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
    USART_Cmd(USART2, ENABLE);
    
    UART2_TX_BUSY = 0;
}

void UART2_SendByte(uint8_t byt)
{
    while((USART2->SR&0X40)==0); 
    USART2->DR = byt; 
}

void UART2_SendString(char *str)
{
    while(*str != '\0')
    {
        UART2_SendByte(*str);
        str++;
    }
}

uint8_t UART2_SendArray(uint8_t *arr, uint16_t length)
{
    uint8_t i;
    for(i=0; i<length; i++)
    {
        UART2_SendByte(arr[i]);
    }
    return 0;
}

int32_t UART2_DmaSend(const uint8_t *buffer, uint16_t length)
{
    //    if(UART2_TX_BUSY)
    //        return 1;
    while(UART2_TX_BUSY);
    
    if(length > UART2_TX_BUFFER_SIZE)
        length = UART2_TX_BUFFER_SIZE;
    memcpy(UART2_TX_BUFFER, buffer, length);
    
    UART2_TX_BUSY = 1;
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA1_Channel7->CNDTR = length;
    DMA1_Channel7->CMAR = (uint32_t)UART2_TX_BUFFER;
    DMA_Cmd(DMA1_Channel7, ENABLE);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UART3_Init(uint32_t baud)
{
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    DMA_InitTypeDef DMA_InitStruct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    USART_DeInit(USART3);
    DMA_DeInit(DMA1_Channel2);
    DMA_DeInit(DMA1_Channel3);
    
    /* 中断优先级 */
    NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init(&NVIC_InitStruct);
    
    NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel3_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init(&NVIC_InitStruct);
    
    /* 串口参数 */
    USART_InitStruct.USART_BaudRate = baud;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx;
    USART_InitStruct.USART_Mode |= USART_Mode_Rx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;	
    USART_Init(USART3, &USART_InitStruct);
    
    /* DMA发送 */
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr = 0;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = 0;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStruct);
    
    /* DMA接收 */
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(USART3->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)UART3_RX_BUFFER;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStruct.DMA_BufferSize = UART3_RX_BUFFER_SIZE;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStruct);
    
    /* 开中断 */
    DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
    USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
    
    DMA_Cmd(DMA1_Channel3, ENABLE);
    USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
    USART_Cmd(USART3, ENABLE);
    
    UART3_TX_BUSY = 0;
}

void UART3_SendByte(uint8_t byt)
{
    while((USART3->SR&0X40)==0);    //等待上一次串口数据发送完成
    USART3->DR = byt;      			//写DR,串口1将发送数据
}

void UART3_SendString(char *str)
{
    while(*str != '\0')
    {
        UART3_SendByte(*str);
        str++;
    }
}

uint8_t UART3_SendArray(uint8_t *arr, uint16_t length)
{
    uint8_t i;
    for(i=0; i<length; i++)
    {
        UART3_SendByte(arr[i]);
    }
    return 0;
}

int32_t UART3_DmaSend(const uint8_t *buffer, uint16_t length)
{
    while(UART3_TX_BUSY);
    
    if(length > UART3_TX_BUFFER_SIZE)
        length = UART3_TX_BUFFER_SIZE;
    memcpy(UART3_TX_BUFFER, buffer, length);
    
    UART3_TX_BUSY = 1;
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA1_Channel2->CNDTR = length;
    DMA1_Channel2->CMAR = (uint32_t)UART3_TX_BUFFER;
    DMA_Cmd(DMA1_Channel2, ENABLE);

    return 0;
}

#ifdef __cplusplus
extern "C" {
#endif
/***************************************************************************************************
* Description:  USART1 相关中断
***************************************************************************************************/
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        USART_ReceiveData(USART1);
    }
    
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART_ReceiveData(USART1);      //串口1收到了一帧数据
        USART_ClearITPendingBit(USART1, USART_IT_IDLE);
        
        uint16_t num;
        num = DMA_GetCurrDataCounter(DMA1_Channel5);        //将串口1收到的数据发送给USART1_RX_BUFFER
        num = UART1_RX_BUFFER_SIZE - num;
        while(PackList_GetSize(plInput1Rx) > INPUT1_RX_BUFFER_MAX_SIZE)     //串口1收到的数据链表总大小大于BUFF空间时
        {
            PacketList_DelFirst(&plInput1Rx);               //那就删除第一个节点
        }
        PacketList_AddTail(&plInput1Rx, UART1_RX_BUFFER, num);      //加到最后一个节点里面
        
        DMA_Cmd(DMA1_Channel5, DISABLE);
        DMA1_Channel5->CNDTR = UART1_RX_BUFFER_SIZE;
        DMA_Cmd(DMA1_Channel5, ENABLE);
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_GL4) != RESET)
    {
        if(DMA_GetITStatus(DMA1_IT_TC4) != RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TC4);
            UART1_TX_BUSY = 0;
        }
        
        if(DMA_GetITStatus(DMA1_IT_TE4) != RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TE4);
        }

        DMA_ClearITPendingBit(DMA1_IT_GL4);
    }
}

void DMA1_Channel5_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_GL5) != RESET)
    {
        if(DMA_GetITStatus(DMA1_IT_TC5) != RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TC5);
        }
        
        if(DMA_GetITStatus(DMA1_IT_TE5) != RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TE5);
        }
        
        DMA_Cmd(DMA1_Channel5, DISABLE);
        DMA1_Channel5->CNDTR = UART1_RX_BUFFER_SIZE;
        DMA_Cmd(DMA1_Channel5, ENABLE);
        
        DMA_ClearITPendingBit(DMA1_IT_GL5);
    }
}

/***************************************************************************************************
* Description:  USART2 相关中断
***************************************************************************************************/
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        USART_ReceiveData(USART2);
    }
    
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
    {
        USART_ReceiveData(USART2);
        USART_ClearITPendingBit(USART2, USART_IT_IDLE);
        
        uint16_t num;
        num = DMA_GetCurrDataCounter(DMA1_Channel6);
        num = UART2_RX_BUFFER_SIZE - num;
        while(PackList_GetSize(plOutputRx) > OUTPUT_RX_BUFFER_MAX_SIZE)
        {
            PacketList_DelFirst(&plOutputRx);
        }
        PacketList_AddTail(&plOutputRx, UART2_RX_BUFFER, num);
        
        DMA_Cmd(DMA1_Channel6, DISABLE);
        DMA1_Channel6->CNDTR = UART2_RX_BUFFER_SIZE;
        DMA_Cmd(DMA1_Channel6, ENABLE);
    }
}

void DMA1_Channel7_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_GL7) != RESET)
    {
        if(DMA_GetITStatus(DMA1_IT_TC7) != RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TC7);
            UART2_TX_BUSY = 0;
        }
        
        if(DMA_GetITStatus(DMA1_IT_TE7) != RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TE7);
        }

        DMA_ClearITPendingBit(DMA1_IT_GL7);
    }
}

void DMA1_Channel6_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_GL6) != RESET)
    {
        if(DMA_GetITStatus(DMA1_IT_TC6) != RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TC6);
        }
        
        if(DMA_GetITStatus(DMA1_IT_TE6) != RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TE6);
        }
        
        DMA_Cmd(DMA1_Channel6, DISABLE);
        DMA1_Channel6->CNDTR = UART2_RX_BUFFER_SIZE;
        DMA_Cmd(DMA1_Channel6, ENABLE);
        
        DMA_ClearITPendingBit(DMA1_IT_GL6);
    }
}

/***************************************************************************************************
* Description:  USART3 相关中断
***************************************************************************************************/
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        USART_ReceiveData(USART3);
    }
    
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        USART_ReceiveData(USART3);
        USART_ClearITPendingBit(USART3, USART_IT_IDLE);
        
        uint16_t num;
        num = DMA_GetCurrDataCounter(DMA1_Channel3);
        num = UART3_RX_BUFFER_SIZE - num;
        while(PackList_GetSize(plCameraRx) > CAMERA_RX_BUFFER_MAX_SIZE)
        {
            PacketList_DelFirst(&plCameraRx);
        }
        PacketList_AddTail(&plCameraRx, UART3_RX_BUFFER, num);

        DMA_Cmd(DMA1_Channel3, DISABLE);
        DMA1_Channel3->CNDTR = UART3_RX_BUFFER_SIZE;
        DMA_Cmd(DMA1_Channel3, ENABLE);
    }
}

void DMA1_Channel2_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_GL2) != RESET)
    {
        if(DMA_GetITStatus(DMA1_IT_TC2) != RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TC2);
            UART3_TX_BUSY = 0;
        }
        
        if(DMA_GetITStatus(DMA1_IT_TE2) != RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TE2);
        }

        DMA_ClearITPendingBit(DMA1_IT_GL2);
    }
}

void DMA1_Channel3_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_GL3) != RESET)
    {
        if(DMA_GetITStatus(DMA1_IT_TC3) != RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TC3);
        }
        
        if(DMA_GetITStatus(DMA1_IT_TE3) != RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TE3);
        }
        
        DMA_Cmd(DMA1_Channel3, DISABLE);
        DMA1_Channel3->CNDTR = UART3_RX_BUFFER_SIZE;
        DMA_Cmd(DMA1_Channel3, ENABLE);
        
        DMA_ClearITPendingBit(DMA1_IT_GL3);
    }
}

#ifdef __cplusplus
}
#endif

int fputc(int ch, FILE *f)
{
    while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
    USART1->DR = (uint8_t) ch;
    return ch;
}

/****************************************** END OF FILE *******************************************/

