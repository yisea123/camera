/***************************************************************************************************
* FILE: i2c.c
*
* DESCRIPTION:  --
*
* CREATED: 2017/9/4, by kevin xu
***************************************************************************************************/
#include "i2c.h"
#include "bsp.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"

/***************************************************************************************************
* Description:  ��ʼ��I2Cģ��
***************************************************************************************************/
void I2C1_Init(void)
{
    I2C_InitTypeDef I2C1_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    I2C_DeInit(I2C1);
    
    I2C1_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C1_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C1_InitStruct.I2C_ClockSpeed = 100000;
    I2C1_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;//�ߵ͵�ƽʱ��ƽ��
    I2C1_InitStruct.I2C_Mode = I2C_Mode_I2C;        //I2Cģʽ
    I2C1_InitStruct.I2C_OwnAddress1 = 0;           //��ַ
    I2C_Init(I2C1, &I2C1_InitStruct);
}

/***************************************************************************************************
* Description:  --
***************************************************************************************************/
uint8_t I2C1_WriteOneByte(uint8_t slave, uint8_t offset, uint8_t dat)
{
    //I2C ��ʼ�ź�
    I2C_GenerateSTART(I2C1, ENABLE);        
    RETRY(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT), 1000);
    delay_us(10);
    //���͵�ַ��дģʽ
    I2C_Send7bitAddress(I2C1, slave, I2C_Direction_Transmitter);
    RETRY(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED), 1000);
    //����Ƭ�ڵ�ַ
    I2C_SendData(I2C1, offset);     
    RETRY(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED), 1000);
    //��������
    I2C_SendData(I2C1, dat);     
    RETRY(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED), 1000);
    //ֹͣ
    I2C_GenerateSTOP(I2C1, ENABLE);
    return 0;
}

/***************************************************************************************************
* Description:  ��һ���ֽ�
***************************************************************************************************/
uint8_t I2C1_ReadOneByte(uint8_t slave, uint8_t offset, uint8_t *dat)
{
    //��ʼ����
    I2C_GenerateSTART(I2C1, ENABLE);        
    RETRY(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT), 1000);
    delay_us(10);
    //����slave��ַ
    I2C_Send7bitAddress(I2C1, slave, I2C_Direction_Transmitter);     //���͵�ַ������д
    RETRY(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED), 1000);
    //����Ƭ�ڵ�ַ
    I2C_SendData(I2C1, offset);     
    RETRY(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED), 1000);
    //���²�����ʼ����
    I2C_GenerateSTART(I2C1, ENABLE);        //I2C ���¿�ʼ�Զ�ȡ����
    RETRY(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT), 1000);
    //���ʹӻ���ַ����ģʽ
    I2C_Send7bitAddress(I2C1, slave, I2C_Direction_Receiver);     //���͵�ַ��������
    RETRY(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED), 1000);
    //��������
    *dat = 0;
    I2C_AcknowledgeConfig(I2C1, DISABLE);   //���ý��յ����ݲ�����ACK�� ֹͣ��������
    RETRY(!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE), 1000);
    *dat = I2C_ReceiveData(I2C1);      
    //ֹͣ����
    I2C_GenerateSTOP(I2C1, ENABLE);         //I2C ����ֹͣ�ź�
    
    return 0;
}

/****************************************** END OF FILE *******************************************/
