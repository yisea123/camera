/***************************************************************************************************
* FILE: uart.c

* DESCRIPTION:    串口初始化,DMA发送接收数据
*               接收时:
*                 串口DMA接收数据到缓存数组,空闲时串口触发空闲中断,或者DMA接收完成触发中断,
*                 在中断中把数据复制到接收链表中,在主循环任务从链表取出数据作进一步处理
*               发送时:
*                 主循环中当前协议生产要发送的数据包并添加到发送到链表中,如果DMA空闲则直接启动发送
*                 如果当前DMA不可用则在DMA发送完成中断中启动发送
*               485收发处理:
*                 平时一直将用于422发送的485芯片设置为接收模式,当启动DMA发送时将485芯片设置为发送
*                 模式,DMA发送完成后将其设置回发送模式

* CREATED: 2018/8/1, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "bsp.h"

#include "system.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/
/* DMA缓存大小 */
#define UART_BUFFER_SIZE    128

/* 接收链表最大容量,超过这个数值的话链表最前端的节点会被丢弃,知道链表中的数据量小于该值 */
#define UART_LIST_MAX_SIZE  256

#define UART_CHECK_READY_TX(n)  (n.gState == HAL_UART_STATE_READY)

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/

/* HAL句柄 */
UART_HandleTypeDef hUart1;
UART_HandleTypeDef hUart4;
UART_HandleTypeDef hUart6;

DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart6_tx;
DMA_HandleTypeDef hdma_usart6_rx;

/* DMA缓存数组*/
static uint8_t UART1_TX_BUFFER[UART_BUFFER_SIZE] = {0};
static uint8_t UART1_RX_BUFFER[UART_BUFFER_SIZE] = {0};
static uint8_t UART6_TX_BUFFER[UART_BUFFER_SIZE] = {0};
static uint8_t UART6_RX_BUFFER[UART_BUFFER_SIZE] = {0};

/* 串口发送缓存链表 */
PACKET_LIST_HEAD(UartTxList);

/* 串口接收缓存链表 */
PACKET_LIST_HEAD(UartRxList);

/* 串口指令转发发送缓存链表 */
PACKET_LIST_HEAD(UartTransTxList);

/* 串口指令转发接收缓存链表 */
//PACKET_LIST_HEAD(UartTransRxList);

/* 缓存链表指针 */
//static struct packet_list *Uart1TxList;
static struct packet_list *Uart1RxList;
//static struct packet_list *Uart6TxList;
static struct packet_list *Uart6RxList;


/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void UsartReceive_IDLE(UART_HandleTypeDef *huart);
void Uart1StartDmaTx(void);
void Uart6StartDmaTx(void);


/***************************************************************************************************
* Description:  串口1初始化
***************************************************************************************************/
void UART1_Init(uint32_t baudrate)
{
    hUart1.Instance = USART1;
    hUart1.Init.BaudRate = baudrate;
    hUart1.Init.WordLength = UART_WORDLENGTH_8B;
    hUart1.Init.StopBits = UART_STOPBITS_1;
    hUart1.Init.Parity = UART_PARITY_NONE;
    hUart1.Init.Mode = UART_MODE_TX_RX;
    hUart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hUart1.Init.OverSampling = UART_OVERSAMPLING_16;
    
    HAL_UART_DeInit(&hUart1);
    
    if (HAL_UART_Init(&hUart1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    if(HAL_UART_Receive_DMA(&hUart1, UART1_RX_BUFFER, sizeof(UART1_RX_BUFFER)) != HAL_OK) 
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    __HAL_UART_ENABLE_IT(&hUart1, UART_IT_IDLE);
    __HAL_UART_ENABLE_IT(&hUart1, UART_IT_TC);
}

/***************************************************************************************************
* Description:  串口4初始化
***************************************************************************************************/
void UART4_Init(uint32_t baudrate)
{
    hUart4.Instance = UART4;
    hUart4.Init.BaudRate = baudrate;
    hUart4.Init.WordLength = UART_WORDLENGTH_8B;
    hUart4.Init.StopBits = UART_STOPBITS_1;
    hUart4.Init.Parity = UART_PARITY_NONE;
    hUart4.Init.Mode = UART_MODE_TX_RX;
    hUart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hUart4.Init.OverSampling = UART_OVERSAMPLING_16;
    
    HAL_UART_DeInit(&hUart4);
    
    if (HAL_UART_Init(&hUart4) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
}

/***************************************************************************************************
* Description:  串口6初始化
***************************************************************************************************/
void UART6_Init(uint32_t baudrate)
{
    hUart6.Instance = USART6;
    hUart6.Init.BaudRate = baudrate;
    hUart6.Init.WordLength = UART_WORDLENGTH_8B;
    hUart6.Init.StopBits = UART_STOPBITS_1;
    hUart6.Init.Parity = UART_PARITY_NONE;
    hUart6.Init.Mode = UART_MODE_TX_RX;
    hUart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hUart6.Init.OverSampling = UART_OVERSAMPLING_16;
    
    HAL_UART_DeInit(&hUart6);
    
    if (HAL_UART_Init(&hUart6) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    if(HAL_UART_Receive_DMA(&hUart6, UART6_RX_BUFFER, sizeof(UART6_RX_BUFFER)) != HAL_OK) 
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    HAL_NVIC_SetPriority(USART6_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
    __HAL_UART_ENABLE_IT(&hUart6, UART_IT_IDLE);
    __HAL_UART_ENABLE_IT(&hUart6, UART_IT_TC);
}

/***************************************************************************************************
* Description:  串口IO初始化
***************************************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
    
    GPIO_InitTypeDef GPIO_InitStruct;
    if(uartHandle->Instance==UART4)
    {
        /* UART4 clock enable */
        __HAL_RCC_UART4_CLK_ENABLE();
        
        /**UART4 GPIO Configuration    
        PC10     ------> UART4_TX
        PC11     ------> UART4_RX 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    }
    else if(uartHandle->Instance==USART1)
    {
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();
        
        /**USART1 GPIO Configuration    
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        /* USART1 DMA Init */
        /* USART1_TX Init */
        hdma_usart1_tx.Instance = DMA2_Stream7;
        hdma_usart1_tx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_tx.Init.Mode = DMA_NORMAL;
        hdma_usart1_tx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_usart1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
        {
            _Error_Handler(__FILE__, __LINE__);
        }
        
        __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);
        
        /* USART1_RX Init */
        hdma_usart1_rx.Instance = DMA2_Stream2;
        hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_rx.Init.Mode = DMA_NORMAL;
        hdma_usart1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
        {
            _Error_Handler(__FILE__, __LINE__);
        }
        
        __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);
    }
    else if(uartHandle->Instance==USART6)
    {
        /* USART6 clock enable */
        __HAL_RCC_USART6_CLK_ENABLE();
        
        /**USART6 GPIO Configuration    
        PC6     ------> USART6_TX
        PC7     ------> USART6_RX
        PG12     ------> RS422TX CTRL
        PG13     ------> RS422RX CTRL 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); 
        
        GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = 0;
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
        
        /* 发送端设置为接收模式，兼容485 */
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_RESET);
        /* 接收端设置为接收模式 */
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
        
        /* USART6 DMA Init */
        /* USART6_TX Init */
        hdma_usart6_tx.Instance = DMA2_Stream6;
        hdma_usart6_tx.Init.Channel = DMA_CHANNEL_5;
        hdma_usart6_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart6_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart6_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart6_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart6_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart6_tx.Init.Mode = DMA_NORMAL;
        hdma_usart6_tx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_usart6_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart6_tx) != HAL_OK)
        {
            _Error_Handler(__FILE__, __LINE__);
        }
        
        __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart6_tx);
        
        /* USART6_RX Init */
        hdma_usart6_rx.Instance = DMA2_Stream1;
        hdma_usart6_rx.Init.Channel = DMA_CHANNEL_5;
        hdma_usart6_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart6_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart6_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart6_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart6_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart6_rx.Init.Mode = DMA_NORMAL;
        hdma_usart6_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        hdma_usart6_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart6_rx) != HAL_OK)
        {
            _Error_Handler(__FILE__, __LINE__);
        }
        
        __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart6_rx);
    }
}

/***************************************************************************************************
* Description:  串口IO口去初始化
***************************************************************************************************/
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
    
    if(uartHandle->Instance==UART4)
    {
        /* Peripheral clock disable */
        __HAL_RCC_UART4_CLK_DISABLE();
        
        /**UART4 GPIO Configuration    
        PC10     ------> UART4_TX
        PC11     ------> UART4_RX 
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11);
    }
    else if(uartHandle->Instance==USART1)
    {
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();
        
        /**USART1 GPIO Configuration    
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX 
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
    }
    else if(uartHandle->Instance==USART6)
    {
        /* Peripheral clock disable */
        __HAL_RCC_USART6_CLK_DISABLE();
        
        /**USART6 GPIO Configuration    
        PC6     ------> USART6_TX
        PC7     ------> USART6_RX
        PG12     ------> RS422TX CTRL
        PG13     ------> RS422RX CTRL
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7);
        HAL_GPIO_DeInit(GPIOG, GPIO_PIN_12|GPIO_PIN_13);
    }
}

/***************************************************************************************************
* Description:  中断处理
***************************************************************************************************/
void USART1_IRQHandler(void)
{
    
    if(__HAL_UART_GET_FLAG(&hUart1, UART_FLAG_TC))
    {
        hUart1.gState = HAL_UART_STATE_READY;
        Uart1StartDmaTx();
    }
    
    if(__HAL_UART_GET_FLAG(&hUart1, UART_FLAG_IDLE))
    {
        UsartReceive_IDLE(&hUart1);
    }
    
    HAL_UART_IRQHandler(&hUart1);
}

void USART6_IRQHandler(void)
{
    
    if(__HAL_UART_GET_FLAG(&hUart6, UART_FLAG_TC))
    {
        hUart6.gState = HAL_UART_STATE_READY;
        Uart6StartDmaTx();
    }
    
    if(__HAL_UART_GET_FLAG(&hUart6,UART_FLAG_IDLE))
    {
        UsartReceive_IDLE(&hUart6);
    }
    
    HAL_UART_IRQHandler(&hUart6);
}

/***************************************************************************************************
* Description:  stdout重定向
***************************************************************************************************/
int fputc(int ch, FILE *f)
{
    while((UART4->SR & 0X40) == 0);//循环发送,直到发送完毕
    UART4->DR = (uint8_t) ch;
    return ch;
}

/***************************************************************************************************
* Description: 串口接收空闲（完成）中断 
***************************************************************************************************/
void UsartReceive_IDLE(UART_HandleTypeDef *huart)  
{
    static uint16_t UART1_RX_NUM = UART_BUFFER_SIZE;
    static uint16_t UART6_RX_NUM = UART_BUFFER_SIZE;
    uint16_t num = 0;
    
    if(huart->Instance == USART1)
    {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        num = huart->Instance->SR;
        num = huart->Instance->DR;
        num = hdma_usart1_rx.Instance->NDTR; 
        HAL_UART_DMAStop(huart);

        UART1_RX_NUM = UART_BUFFER_SIZE - num;
        
        /* 将数据包添加到接收链表 */
        while(PackList_GetSize((*Uart1RxList)) > UART_LIST_MAX_SIZE)
        {
            PacketList_DelFirst(Uart1RxList);
//            printf("UART1 Throw away some packets!\r\n");
        }
        PacketList_Add(Uart1RxList, UART1_RX_BUFFER, UART1_RX_NUM);     //将串口1收到的数据添加到链表
        
        if(GET_UART_MODE(stSysPara) == 1)
        {
            if(UART_CHECK_READY_TX(hUart6))
                Uart6StartDmaTx();
        }
        else if(GET_UART_MODE(stSysPara) == 2)
        {
            while(PackList_GetSize(UartTransTxList) > UART_LIST_MAX_SIZE)
            {
                PacketList_DelFirst(&UartTransTxList);
                printf("UART Repeater Throw away some packets!\r\n");
            }
            PacketList_Add(&UartTransTxList, UART1_RX_BUFFER, UART1_RX_NUM);
            if(UART_CHECK_READY_TX(hUart6))
                Uart6StartDmaTx();
        }
        
        /* 界面显示接收到数据标志 */
        UI_ShowRecvPacket();

        /* 清空缓存，继续接收 */
        memset(UART1_RX_BUFFER, 0, UART1_RX_NUM);
        if(HAL_UART_Receive_DMA(&hUart1, UART1_RX_BUFFER, UART_BUFFER_SIZE) != HAL_OK) 
        {
            printf("UART 1 Start DMA Receive Error!\r\n");
//            _Error_Handler(__FILE__, __LINE__);
        }
    }
    else if(huart->Instance == USART6)
    {
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        num = huart->Instance->SR;
        num = huart->Instance->DR;
        num = hdma_usart6_rx.Instance->NDTR; 
        HAL_UART_DMAStop(huart);
        
        UART6_RX_NUM = UART_BUFFER_SIZE - num;
        
        /* 将数据包添加到接收链表 */
        while(PackList_GetSize((*Uart6RxList)) > UART_LIST_MAX_SIZE)
        {
            PacketList_DelFirst(Uart6RxList);
            printf("UART6 Throw away some packets!\r\n");
        }
        PacketList_Add(Uart6RxList, UART6_RX_BUFFER, UART6_RX_NUM);
        
        if(GET_UART_MODE(stSysPara) == 1)
        {
            while(PackList_GetSize(UartTransTxList) > UART_LIST_MAX_SIZE)
            {
                PacketList_DelFirst(&UartTransTxList);
                printf("UART Repeater Throw away some packets!\r\n");
            }
            PacketList_Add(&UartTransTxList, UART6_RX_BUFFER, UART6_RX_NUM);
            if(UART_CHECK_READY_TX(hUart1))
                Uart1StartDmaTx();
        }
        else if(GET_UART_MODE(stSysPara) == 2)
        {
            if(UART_CHECK_READY_TX(hUart1))
                Uart1StartDmaTx();
        }
        
        /* 界面显示接收到数据标志 */
        UI_ShowRecvPacket();
        
        /* 清空缓存，继续接收 */
        memset(UART6_RX_BUFFER, 0, UART6_RX_NUM);
        if(HAL_UART_Receive_DMA(&hUart6, UART6_RX_BUFFER, UART_BUFFER_SIZE) != HAL_OK) 
        {
            _Error_Handler(__FILE__, __LINE__);
        }
    }
}

/***************************************************************************************************
* Description:  串口1DMA发送完成后从缓存中读取新包继续发送
***************************************************************************************************/
void Uart1StartDmaTx(void)
{
    uint8_t *pk;
    uint32_t length;
    
    /* 根据串口转发模式 */
    switch(GET_UART_MODE(stSysPara))
    {
        /* RS232/USB串口作为指令转发的输入接口 */
        case 1:
        if(PacketList_Get(&UartTransTxList, &pk, &length))
        {
            return;
        }
        memcpy(UART1_TX_BUFFER, pk, length);
        PacketList_DelFirst(&UartTransTxList);
        
        if(HAL_UART_Transmit_DMA(&hUart1, UART1_TX_BUFFER, length) != HAL_OK) 
        {
            
        }
        break;
        
        /* RS422/485接口作为指令转发的输入接口 */
        case 2:
        if(PacketList_Get(&UartTxList, &pk, &length))
        {
            return;
        }
        memcpy(UART1_TX_BUFFER, pk, length);
        PacketList_DelFirst(&UartTxList);
        
        if(HAL_UART_Transmit_DMA(&hUart1, UART1_TX_BUFFER, length) != HAL_OK) 
        {
            
        }
        
        break;
        
        /* 指令转发关闭， 两个串口都空闲才启动发送 */
        case 0:
        default:
        if(UART_CHECK_READY_TX(hUart6)) //能调用此函数说明UART1是OK的，所以只需要检查UART6的状态
        {
            if(PacketList_Get(&UartTxList, &pk, &length))
            {
                return;
            }
            memcpy(UART1_TX_BUFFER, pk, length);
            memcpy(UART6_TX_BUFFER, pk, length);
            PacketList_DelFirst(&UartTxList);
            
            if(HAL_UART_Transmit_DMA(&hUart1, UART1_TX_BUFFER, length) != HAL_OK) 
            {
                
            }
            
            if(HAL_UART_Transmit_DMA(&hUart6, UART6_TX_BUFFER, length) != HAL_OK) 
            {
                
            }
        }
        
        break;
    }
    
    /* 发送数据标志 */
    UI_ShowSentPacket();
}

/***************************************************************************************************
* Description:  串口6DMA发送完成后从缓存中读取新包继续发送
***************************************************************************************************/
void Uart6StartDmaTx(void)
{
    uint8_t *pk;
    uint32_t length;
    
    /* 根据串口转发模式 */
    switch(GET_UART_MODE(stSysPara))
    {
        /* RS232/USB串口作为指令转发的输入接口 */
        case 1:
        if(PacketList_Get(&UartTxList, &pk, &length))
        {
            RS485_RX_MODE();
            return;
        }
        memcpy(UART6_TX_BUFFER, pk, length);
        PacketList_DelFirst(&UartTxList);
        
        RS485_TX_MODE();
        if(HAL_UART_Transmit_DMA(&hUart6, UART6_TX_BUFFER, length) != HAL_OK) 
        {
            
        }
        
        break;
        
        /* RS422/485接口作为指令转发的输入接口 */
        case 2:
        if(PacketList_Get(&UartTransTxList, &pk, &length))
        {
            RS485_RX_MODE();
            return;
        }
        memcpy(UART6_TX_BUFFER, pk, length);
        PacketList_DelFirst(&UartTransTxList);
        
        RS485_TX_MODE();
        if(HAL_UART_Transmit_DMA(&hUart6, UART6_TX_BUFFER, length) != HAL_OK) 
        {
            
        }
        break;
        
        /* 指令转发关闭， 两个串口都空闲才启动发送 */
        case 0:
        default:
        if(UART_CHECK_READY_TX(hUart1)) //能调用此函数说明UART6是OK的，所以只需要检查UART1的状态
        {
            if(PacketList_Get(&UartTxList, &pk, &length))
            {
                RS485_RX_MODE();
                return;
            }
            memcpy(UART1_TX_BUFFER, pk, length);
            memcpy(UART6_TX_BUFFER, pk, length);
            PacketList_DelFirst(&UartTxList);
            
            if(HAL_UART_Transmit_DMA(&hUart1, UART1_TX_BUFFER, length) != HAL_OK) 
            {
                
            }
            
            RS485_TX_MODE();
            if(HAL_UART_Transmit_DMA(&hUart6, UART6_TX_BUFFER, length) != HAL_OK) 
            {
                
            }
        }
        break;
    }
    
    /* 发送数据标志 */
    UI_ShowSentPacket();
}

/***************************************************************************************************
* Description:  启动发送
***************************************************************************************************/
void UartStartDmaTx(void)
{
    uint8_t *pk;
    uint32_t length;
    
    /* 根据串口转发模式启动发送 */
    switch(GET_UART_MODE(stSysPara))
    {
        /* RS232/USB串口作为指令转发的输入接口 */
        case 1:
        if(UART_CHECK_READY_TX(hUart6))
        {
            if(PacketList_Get(&UartTxList, &pk, &length))
            {
                RS485_RX_MODE();
                return;
            }
            memcpy(UART6_TX_BUFFER, pk, length);
            PacketList_DelFirst(&UartTxList);
            
            RS485_TX_MODE();
            if(HAL_UART_Transmit_DMA(&hUart6, UART6_TX_BUFFER, length) != HAL_OK) 
            {
                
            }
        }
        break;
        
        /* RS422/485接口作为指令转发的输入接口 */
        case 2:
        if(UART_CHECK_READY_TX(hUart1))
        {
            if(PacketList_Get(&UartTxList, &pk, &length))
            {
                return;
            }
            memcpy(UART1_TX_BUFFER, pk, length);
            PacketList_DelFirst(&UartTxList);
            
            if(HAL_UART_Transmit_DMA(&hUart1, UART1_TX_BUFFER, length) != HAL_OK) 
            {
                
            }
        }
        break;
        
        /* 指令转发关闭， 两个串口都空闲才启动发送 */
        case 0:
        default:
        if(UART_CHECK_READY_TX(hUart1) && UART_CHECK_READY_TX(hUart6))
        {
            if(PacketList_Get(&UartTxList, &pk, &length))
            {
                RS485_RX_MODE();
                return;
            }
            memcpy(UART1_TX_BUFFER, pk, length);
            memcpy(UART6_TX_BUFFER, pk, length);
            PacketList_DelFirst(&UartTxList);
            
            if(HAL_UART_Transmit_DMA(&hUart1, UART1_TX_BUFFER, length) != HAL_OK) 
            {
                
            }
            
            RS485_TX_MODE();
            if(HAL_UART_Transmit_DMA(&hUart6, UART6_TX_BUFFER, length) != HAL_OK) 
            {
                
            }
        }
        break;
    }
    
    /* 发送数据标志 */
    UI_ShowSentPacket();
}

/***************************************************************************************************
* Description:  外部使用的发送数据，内部把数据包添加到发送缓存中，等待DMA发送
***************************************************************************************************/
void UartSentPacket(const uint8_t *pk, uint8_t length)
{
    
    /* 将数据包添加到接收链表 */
    while(PackList_GetSize(UartTxList) > UART_LIST_MAX_SIZE)
    {
        PacketList_DelFirst(&UartTxList);
        printf("Sent List Overflow! Throw away some packets!\r\n");
    }
    PacketList_Add(&UartTxList, pk, length);
    
    /* 启动发送 */
    UartStartDmaTx();
}

/***************************************************************************************************
* Description:  串口初始化
***************************************************************************************************/
void UART_Init(void)
{
    const uint32_t baudrate[] = {2400, 4800, 9600, 19200, 38400, 115200};
    
    /* 根据串口转发模式设置指针 */
    switch(GET_UART_MODE(stSysPara))
    {
        /* RS232/USB串口作为指令转发的输入接口 */
        case 1:
        //        Uart1TxList = &UartTransTxList;
        Uart1RxList = &UartTxList;
        //        Uart6TxList = &UartTxList;
        Uart6RxList = &UartRxList;
        
        break;
        
        /* RS422/485接口作为指令转发的输入接口 */
        case 2:
        //        Uart1TxList = &UartTxList;
        Uart1RxList = &UartRxList;
        //        Uart6TxList = &UartTransTxList;
        Uart6RxList = &UartTxList;
        
        break;
        
        /* 指令转发关闭 */
        case 0:
        default:
        //        Uart1TxList = &UartTxList;
        Uart1RxList = &UartRxList;
        //        Uart6TxList = &UartTxList;
        Uart6RxList = &UartRxList;
        
        break;
    }
    
    /* 清空缓存 */
    PacketList_Destroy(&UartTxList);
    PacketList_Destroy(&UartRxList);
    PacketList_Destroy(&UartTransTxList);
//    PacketList_Destroy(&UartTransRxList);
    
    /* 初始化串口 */
    UART1_Init(baudrate[GET_RS232_BAUDRATE(stSysPara)]);
    UART6_Init(baudrate[GET_RS422_BAUDRATE(stSysPara)]);
}


/****************************************** END OF FILE *******************************************/
