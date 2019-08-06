/***************************************************************************************************
* FILE: spi.c

* DESCRIPTION:  SPI��������

* CREATED: 2018/02/09, by kevin xu
***************************************************************************************************/
#include "spi.h"

/***************************************************************************************************
* Description:  SPI1��ʼ��
***************************************************************************************************/
void SPI1_Init(void)
{
    SPI_InitTypeDef SPI_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
    SPI_I2S_DeInit(SPI1);
    
     /* ˫�� */
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    /* ��ģʽ */
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    /* Ƭѡ������� */
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    /* ʱ�� 96M/2=48MHz */
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2 ;
    /* ��λ��ǰ */
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_LSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    
//    SPI_Init(SPI3, &SPI_InitStruct);
    
    SPI_Init(SPI1, &SPI_InitStruct);
    SPI_Cmd(SPI1, ENABLE);
}

/***************************************************************************************************
* Description:  SPI1��дһ���ֽ�
***************************************************************************************************/
uint8_t SPI1_WriteReadByte(uint8_t arg)
{
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, arg);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
}

/***************************************************************************************************
* Description:  SPI2 ��ʼ�� ����FPGA����
***************************************************************************************************/
void SPI2_Init(void)
{
    SPI_InitTypeDef SPI_InitStruct;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    
    SPI_I2S_DeInit(SPI2);
    
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    
    SPI_Init(SPI2, &SPI_InitStruct);
    
    SPI_Cmd(SPI2, ENABLE);
}

/***************************************************************************************************
* Description:  SPI2��дһ���ֽ�
***************************************************************************************************/
uint8_t SPI2_WriteReadByte(uint8_t arg)
{
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, arg);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI2);
}
/***************************************************************************************************
* Description:  SPI3��ʼ��
***************************************************************************************************/
void SPI3_Init(void)
{
    SPI_InitTypeDef SPI_InitStruct;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    
    SPI_I2S_DeInit(SPI3);
    
     /* ˫�� */
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    /* ��ģʽ */
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    /* Ƭѡ������� */
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    /* ʱ�� 96M/2=48MHz */
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2 ;
    /* ��λ��ǰ */
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_LSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    
    SPI_Init(SPI3, &SPI_InitStruct);
    
    SPI_Cmd(SPI3, ENABLE);
}
/***************************************************************************************************
* Description:  SPI3��дһ���ֽ�
***************************************************************************************************/
uint8_t SPI3_WriteReadByte(uint8_t arg)
{
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI3, arg);
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI3);
}
/****************************************** END OF FILE *******************************************/
