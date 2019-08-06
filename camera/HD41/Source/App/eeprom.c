/***************************************************************************************************
* FILE: eeprom.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/12/4, by kevin
***************************************************************************************************/
#include "eeprom.h"

/* MCU IO�ڲ��� */
#define _I2C_SCL_1     PBout(6) = 1
#define _I2C_SCL_0     PBout(6) = 0
#define _I2C_SCL_DAT   PBin(6)
#define _I2C_SCL_IN    GPIOB->CRL &= 0xF0FFFFFF;  GPIOB->CRL |= 0x04000000
#define _I2C_SCL_OUT   GPIOB->CRL &= 0xFFFFFFF0;  GPIOB->CRL |= 0x03000000

#define _I2C_SDA_1     PBout(7) = 1
#define _I2C_SDA_0     PBout(7) = 0
#define _I2C_SDA_DAT   PBin(7)
#define _I2C_SDA_IN    GPIOB->CRL &= 0x0FFFFFFF;  GPIOB->CRL |= 0x40000000
#define _I2C_SDA_OUT   GPIOB->CRL &= 0x0FFFFFFF;  GPIOB->CRL |= 0x30000000

/***************************************************************************************************
* Description:  �ڲ���ʱ
***************************************************************************************************/
static void _I2C_Delay(uint8_t n)
{
    uint8_t i;
    while(n--)
        for(i=0; i<48; i++);
}

/***************************************************************************************************
* Description:  ������ʼ����
***************************************************************************************************/
static void _I2C_Start(void)
{
    _I2C_SDA_1;
    _I2C_SCL_1;
    _I2C_Delay(2);
    _I2C_SDA_0;
    _I2C_Delay(2);
    _I2C_SCL_0;
    _I2C_Delay(1);
}

/***************************************************************************************************
* Description:  ����ֹͣ����
***************************************************************************************************/
static void _I2C_Stop(void)
{
    _I2C_SDA_0;
    _I2C_Delay(1);
    _I2C_SCL_1;
    _I2C_Delay(2);
    _I2C_SDA_1;
    _I2C_Delay(2);
}

/***************************************************************************************************
* Description:  ���ack ����0�ɹ�������1ʧ��
***************************************************************************************************/
static uint8_t _I2C_WaitAck(void)
{
    uint8_t ack;
    _I2C_SDA_IN;
    _I2C_SDA_1;
    _I2C_Delay(1);
    _I2C_SCL_1;
    _I2C_Delay(2);
    if(_I2C_SDA_DAT) ack = 1;
    else ack = 0;
    _I2C_SCL_0;
    _I2C_Delay(1);
    _I2C_SDA_OUT;
    return ack;
}

/***************************************************************************************************
* Description:  ����ACK
***************************************************************************************************/
static void _I2C_SendAck(uint8_t ack)
{
    if(ack) _I2C_SDA_0;
    else _I2C_SDA_1;
    _I2C_Delay(1);
    _I2C_SCL_1;
    _I2C_Delay(2);
    _I2C_SCL_0;
    _I2C_Delay(1);
}

/***************************************************************************************************
* Description:  дһ���ֽ�
***************************************************************************************************/
static uint8_t _I2C_WriteByte(uint8_t byt, uint8_t ack)
{
    uint8_t i;
    for(i=0; i<8; i++)
    {
        if(byt & 0x80) _I2C_SDA_1;
        else _I2C_SDA_0;
        _I2C_Delay(1);
        byt <<= 1;
        _I2C_SCL_1;
        _I2C_Delay(2);
        _I2C_SCL_0;
        _I2C_Delay(1);
    }
    ack = _I2C_WaitAck();
    return ack;
}

/***************************************************************************************************
* Description:  ��һ���ֽ�
***************************************************************************************************/
static uint8_t _I2C_ReadByte(uint8_t ack)
{
    uint8_t i, data;
    data = 0;
    _I2C_SDA_IN;
    for(i=0; i<8; i++)
    {
        _I2C_Delay(1);
        _I2C_SCL_1;
        _I2C_Delay(2);
        data <<= 1;
        if(_I2C_SDA_DAT)
            data |= 0x01;
        _I2C_SCL_0;
        _I2C_Delay(1);
    }
    _I2C_SDA_OUT;
    _I2C_SendAck(ack);
    return data;
}

/***************************************************************************************************
* Description:  ��ȡ����
***************************************************************************************************/
uint8_t _I2C_Read(uint8_t dev, uint8_t addr, uint8_t *buf, uint16_t len)
{
    uint16_t i;
    
    _I2C_Start();
    if(_I2C_WriteByte(dev, 1))  //����д����
        return 1;        
    if(_I2C_WriteByte(addr, 1)) //���͵�ַ
        return 1;        
	_I2C_Start();
	if(_I2C_WriteByte(dev+1, 1))//�������ģʽ
        return 1;
    
    /* �������� */
    len -= 1;
    for(i=0; i<len; i++)
        buf[i] = _I2C_ReadByte(1);
    buf[len] = _I2C_ReadByte(0);
    
    _I2C_Stop();
	return 0;
}

/***************************************************************************************************
* Description: д����
***************************************************************************************************/
uint8_t _I2C_Write(uint8_t dev, uint16_t addr, uint8_t *buf, uint8_t len)
{
    uint16_t i;
    
    _I2C_Start();
    if(_I2C_WriteByte(dev, 1))  //���͵�ַ��д����
        return 1;        
    if(_I2C_WriteByte(addr, 1)) //���͵�ַ
        return 1;       
    
    for(i=0; i<len; i++)
    {
        if(_I2C_WriteByte(buf[i], 1))   //��������
            return 1;    
    }
    
    _I2C_Stop();
    return 0;
}

/***************************************************************************************************
* Description: ��ָ����ַ����һ���ֽ�
***************************************************************************************************/
uint8_t _I2C_ReadOneByte(uint8_t dev, uint16_t addr)
{
    uint8_t dat;
    
    _I2C_Start();
    if(_I2C_WriteByte(dev, 1))  //����д����
        return 0;        
    if(_I2C_WriteByte(addr, 1)) //���͵�ַ
        return 0;        
	_I2C_Start();
	if(_I2C_WriteByte(dev+1, 1))//�������ģʽ
        return 0;        
    dat=_I2C_ReadByte(0);
    _I2C_Stop();
	return dat;
}

/***************************************************************************************************
* Description: ��ָ����ַд��һ���ֽ�
***************************************************************************************************/
static uint8_t _I2C_WriteOneByte(uint8_t dev, uint16_t addr, uint8_t byt)
{
    _I2C_Start();
    if(_I2C_WriteByte(dev, 1))  //���͵�ַ��д����
        return 1;        
    if(_I2C_WriteByte(addr, 1)) //���͵�ַ
        return 1;      
	if(_I2C_WriteByte(byt, 1))  //��������
        return 1;        
    _I2C_Stop();
    return 0;
}

/***************************************************************************************************
* Description:  дһ���ֽڣ�������
***************************************************************************************************/
uint8_t _I2C_WriteOneByteMask(uint8_t dev, uint16_t reg, uint8_t mask, uint8_t dat)
{
    uint8_t value = 0;
    if(mask != 0xFF)
    {
        value = _I2C_ReadOneByte(dev, reg);
    }
    
    value |= dat;
    return _I2C_WriteOneByte(dev, reg, value);
}

/***************************************************************************************************
* Description:  ģ��I2C��ʼ��
***************************************************************************************************/
static void _I2C_Init(void)
{
    _I2C_SCL_OUT;
    _I2C_SDA_OUT;
    _I2C_SCL_1;
    _I2C_SDA_1;
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
void EEPROM_Init(void)
{
    _I2C_Init();
}

/***************************************************************************************************
* Description: ��EEPROMָ����ַ����һ������
***************************************************************************************************/
uint8_t EEPROM_ReadOneByte(uint16_t addr, uint8_t *dat)
{
    _I2C_Start();
    if(_I2C_WriteByte(0XA0, 1)) return 1;         //����д����
    if(_I2C_WriteByte(addr>>8, 1)) return 1;      //���͸ߵ�ַ
    if(_I2C_WriteByte(addr&0xFF, 1)) return 1;    //���͵͵�ַ
	_I2C_Start();
	if(_I2C_WriteByte(0XA1, 1)) return 1;         //�������ģʽ
    *dat = _I2C_ReadByte(0);
    _I2C_Stop();                 //ֹͣ����
	return 0;
}

/***************************************************************************************************
* Description: ��EEPROMָ����ַд��һ������
***************************************************************************************************/
uint8_t EEPROM_WriteOneByte(uint16_t addr, uint8_t byt)
{
    _I2C_Start();
    if(_I2C_WriteByte(0XA0, 1)) return 1;             //���͵�ַ��д����
    if(_I2C_WriteByte(addr>>8, 1)) return 1;          //���͸ߵ�ַ
    if(_I2C_WriteByte(addr&0xFF, 1)) return 1;        //���͵͵�ַ
	if(_I2C_WriteByte(byt, 1)) return 1;              //�����ֽ�
    _I2C_Stop();                     //ֹͣ����
    delay_ms(5);
    return 0;
}

/***************************************************************************************************
* Description: ��EEPROM�����ָ����ַ��ʼ����ָ������������
***************************************************************************************************/
uint8_t EEPROM_Read(uint16_t addr, uint8_t *buf, uint16_t length)
{
    _I2C_Start();
    if(_I2C_WriteByte(0XA0, 1)) return 1;       //���͵�ַ��д����
    if(_I2C_WriteByte(addr>>8, 1)) return 1;    //���͸ߵ�ַ
    if(_I2C_WriteByte(addr&0xFF, 1)) return 1;  //���͵͵�ַ
    _I2C_Start();
	if(_I2C_WriteByte(0XA1, 1)) return 1;       //�������ģʽ
	while(--length)
	{
        *buf = _I2C_ReadByte(1);
        buf++;
	}
    *buf = _I2C_ReadByte(0);
    _I2C_Stop();       
    return 0;
}

/***************************************************************************************************
* Description: ��EEPROM�����ָ����ַ��ʼд��ָ������������
***************************************************************************************************/
uint8_t EEPROM_Write(uint16_t addr, const uint8_t *buf,uint16_t length)
{
    _I2C_Start();
    if(_I2C_WriteByte(0XA0, 1)) return 1;             //���͵�ַ��д����
    if(_I2C_WriteByte(addr>>8, 1)) return 1;          //���͸ߵ�ַ
    if(_I2C_WriteByte(addr&0xFF, 1)) return 1;        //���͵͵�ַ
    
	while(length--)
	{
        if(_I2C_WriteByte(*buf, 1)) return 1;         //��������
        addr++;
		buf++;
        
        /* �����ҳ��β */
        if((addr % 32) == 0)
        {
            _I2C_Stop();   
            /* ��EEPROM���д���������д�� */
            delay_ms(6);
            _I2C_Start();
            if(_I2C_WriteByte(0XA0, 1)) return 1;
            if(_I2C_WriteByte(addr>>8, 1)) return 1;
            if(_I2C_WriteByte(addr&0xFF, 1)) return 1;
        }
	}
    _I2C_Stop();
    delay_ms(6);
    return 0;
}


/****************************************** END OF FILE *******************************************/