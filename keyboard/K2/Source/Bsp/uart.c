/***************************************************************************************************
* FILE: uart.c

* DESCRIPTION:    ���ڳ�ʼ��,DMA���ͽ�������
*               ����ʱ:
*                 ����DMA�������ݵ���������,����ʱ���ڴ��������ж�,����DMA������ɴ����ж�,
*                 ���ж��а����ݸ��Ƶ�����������,����ѭ�����������ȡ����������һ������
*               ����ʱ:
*                 ��ѭ���е�ǰЭ������Ҫ���͵����ݰ�����ӵ����͵�������,���DMA������ֱ����������
*                 �����ǰDMA����������DMA��������ж�����������
*               485�շ�����:
*                 ƽʱһֱ������422���͵�485оƬ����Ϊ����ģʽ,������DMA����ʱ��485оƬ����Ϊ����
*                 ģʽ,DMA������ɺ������ûط���ģʽ

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
/* DMA�����С */
#define UART_BUFFER_SIZE    128

/* ���������������,���������ֵ�Ļ�������ǰ�˵Ľڵ�ᱻ����,֪�������е�������С�ڸ�ֵ */
#define UART_LIST_MAX_SIZE  256

#define UART_CHECK_READY_TX(n)  (n.gState == HAL_UART_STATE_READY)

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/

/* HAL��� */
UART_HandleTypeDef hUart1;
UART_HandleTypeDef hUart4;
UART_HandleTypeDef hUart6;

DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart6_tx;
DMA_HandleTypeDef hdma_usart6_rx;

/* DMA��������*/
static uint8_t UART1_TX_BUFFER[UART_BUFFER_SIZE] = {0};
static uint8_t UART1_RX_BUFFER[UART_BUFFER_SIZE] = {0};
static uint8_t UART6_TX_BUFFER[UART_BUFFER_SIZE] = {0};
static uint8_t UART6_RX_BUFFER[UART_BUFFER_SIZE] = {0};

/* ���ڷ��ͻ������� */
PACKET_LIST_HEAD(UartTxList);

/* ���ڽ��ջ������� */
PACKET_LIST_HEAD(UartRxList);

/* ����ָ��ת�����ͻ������� */
PACKET_LIST_HEAD(UartTransTxList);

/* ����ָ��ת�����ջ������� */
//PACKET_LIST_HEAD(UartTransRxList);

/* ��������ָ�� */
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
* Description:  ����1��ʼ��
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
* Description:  ����4��ʼ��
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
* Description:  ����6��ʼ��
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
* Description:  ����IO��ʼ��
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
        
        /* ���Ͷ�����Ϊ����ģʽ������485 */
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_RESET);
        /* ���ն�����Ϊ����ģʽ */
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
* Description:  ����IO��ȥ��ʼ��
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
* Description:  �жϴ���
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
* Description:  stdout�ض���
***************************************************************************************************/
int fputc(int ch, FILE *f)
{
    while((UART4->SR & 0X40) == 0);//ѭ������,ֱ���������
    UART4->DR = (uint8_t) ch;
    return ch;
}

/***************************************************************************************************
* Description: ���ڽ��տ��У���ɣ��ж� 
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
        
        /* �����ݰ���ӵ��������� */
        while(PackList_GetSize((*Uart1RxList)) > UART_LIST_MAX_SIZE)
        {
            PacketList_DelFirst(Uart1RxList);
//            printf("UART1 Throw away some packets!\r\n");
        }
        PacketList_Add(Uart1RxList, UART1_RX_BUFFER, UART1_RX_NUM);     //������1�յ���������ӵ�����
        
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
        
        /* ������ʾ���յ����ݱ�־ */
        UI_ShowRecvPacket();

        /* ��ջ��棬�������� */
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
        
        /* �����ݰ���ӵ��������� */
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
        
        /* ������ʾ���յ����ݱ�־ */
        UI_ShowRecvPacket();
        
        /* ��ջ��棬�������� */
        memset(UART6_RX_BUFFER, 0, UART6_RX_NUM);
        if(HAL_UART_Receive_DMA(&hUart6, UART6_RX_BUFFER, UART_BUFFER_SIZE) != HAL_OK) 
        {
            _Error_Handler(__FILE__, __LINE__);
        }
    }
}

/***************************************************************************************************
* Description:  ����1DMA������ɺ�ӻ����ж�ȡ�°���������
***************************************************************************************************/
void Uart1StartDmaTx(void)
{
    uint8_t *pk;
    uint32_t length;
    
    /* ���ݴ���ת��ģʽ */
    switch(GET_UART_MODE(stSysPara))
    {
        /* RS232/USB������Ϊָ��ת��������ӿ� */
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
        
        /* RS422/485�ӿ���Ϊָ��ת��������ӿ� */
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
        
        /* ָ��ת���رգ� �������ڶ����в��������� */
        case 0:
        default:
        if(UART_CHECK_READY_TX(hUart6)) //�ܵ��ô˺���˵��UART1��OK�ģ�����ֻ��Ҫ���UART6��״̬
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
    
    /* �������ݱ�־ */
    UI_ShowSentPacket();
}

/***************************************************************************************************
* Description:  ����6DMA������ɺ�ӻ����ж�ȡ�°���������
***************************************************************************************************/
void Uart6StartDmaTx(void)
{
    uint8_t *pk;
    uint32_t length;
    
    /* ���ݴ���ת��ģʽ */
    switch(GET_UART_MODE(stSysPara))
    {
        /* RS232/USB������Ϊָ��ת��������ӿ� */
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
        
        /* RS422/485�ӿ���Ϊָ��ת��������ӿ� */
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
        
        /* ָ��ת���رգ� �������ڶ����в��������� */
        case 0:
        default:
        if(UART_CHECK_READY_TX(hUart1)) //�ܵ��ô˺���˵��UART6��OK�ģ�����ֻ��Ҫ���UART1��״̬
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
    
    /* �������ݱ�־ */
    UI_ShowSentPacket();
}

/***************************************************************************************************
* Description:  ��������
***************************************************************************************************/
void UartStartDmaTx(void)
{
    uint8_t *pk;
    uint32_t length;
    
    /* ���ݴ���ת��ģʽ�������� */
    switch(GET_UART_MODE(stSysPara))
    {
        /* RS232/USB������Ϊָ��ת��������ӿ� */
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
        
        /* RS422/485�ӿ���Ϊָ��ת��������ӿ� */
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
        
        /* ָ��ת���رգ� �������ڶ����в��������� */
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
    
    /* �������ݱ�־ */
    UI_ShowSentPacket();
}

/***************************************************************************************************
* Description:  �ⲿʹ�õķ������ݣ��ڲ������ݰ���ӵ����ͻ����У��ȴ�DMA����
***************************************************************************************************/
void UartSentPacket(const uint8_t *pk, uint8_t length)
{
    
    /* �����ݰ���ӵ��������� */
    while(PackList_GetSize(UartTxList) > UART_LIST_MAX_SIZE)
    {
        PacketList_DelFirst(&UartTxList);
        printf("Sent List Overflow! Throw away some packets!\r\n");
    }
    PacketList_Add(&UartTxList, pk, length);
    
    /* �������� */
    UartStartDmaTx();
}

/***************************************************************************************************
* Description:  ���ڳ�ʼ��
***************************************************************************************************/
void UART_Init(void)
{
    const uint32_t baudrate[] = {2400, 4800, 9600, 19200, 38400, 115200};
    
    /* ���ݴ���ת��ģʽ����ָ�� */
    switch(GET_UART_MODE(stSysPara))
    {
        /* RS232/USB������Ϊָ��ת��������ӿ� */
        case 1:
        //        Uart1TxList = &UartTransTxList;
        Uart1RxList = &UartTxList;
        //        Uart6TxList = &UartTxList;
        Uart6RxList = &UartRxList;
        
        break;
        
        /* RS422/485�ӿ���Ϊָ��ת��������ӿ� */
        case 2:
        //        Uart1TxList = &UartTxList;
        Uart1RxList = &UartRxList;
        //        Uart6TxList = &UartTransTxList;
        Uart6RxList = &UartTxList;
        
        break;
        
        /* ָ��ת���ر� */
        case 0:
        default:
        //        Uart1TxList = &UartTxList;
        Uart1RxList = &UartRxList;
        //        Uart6TxList = &UartTxList;
        Uart6RxList = &UartRxList;
        
        break;
    }
    
    /* ��ջ��� */
    PacketList_Destroy(&UartTxList);
    PacketList_Destroy(&UartRxList);
    PacketList_Destroy(&UartTransTxList);
//    PacketList_Destroy(&UartTransRxList);
    
    /* ��ʼ������ */
    UART1_Init(baudrate[GET_RS232_BAUDRATE(stSysPara)]);
    UART6_Init(baudrate[GET_RS422_BAUDRATE(stSysPara)]);
}


/****************************************** END OF FILE *******************************************/
