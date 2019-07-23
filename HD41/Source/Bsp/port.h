/***************************************************************************************************
* FILE: port.h
*
* DESCRIPTION: 92U3 MCU IO������
*
* CREATED: 2018/02/09, by kevin xu
***************************************************************************************************/
#ifndef PORT_H
#define PORT_H

/***************************************************************************************************
* DEFINES
***************************************************************************************************/
//IO�ڲ����궨��
#define BIT_DATA(va , n)		((va & (1<<n)) >> n)
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08

//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //���
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //����

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //���
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //����

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //���
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //����

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //���
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //����

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //���
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //���
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //���
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����


/* SPI Ƭѡ�ź� */
#define SPI1_CS                         PAout(4)
#define SPI2_CS                         PBout(12)
#define SPI3_CS                         PAout(15)

/* ��̨��λ���� */
#define PAN_SENSE                       PBin(3)
#define TILT_SENSE                      PBin(4)
#define IS_PAN_SENSE                    (!PAN_SENSE)
#define IS_TILT_SENSE                   (!TILT_SENSE)

/* MS41909 */
#define MS41909_PULSE                   PBout(5)


/* ���뿪�� */
#define DIAL_1                          PAin(15)
#define DIAL_2                          PAin(12)
#define DIAL_4                          PAin(11)
#define DIAL_5                          PCin(15)


/* ң���� */
#define IR_ADDR_1                       (!DIAL_1)  
#define IR_ADDR_2                       (!DIAL_2)  
#define IR_DATA_IN                      PAin(0)
#define WL_DATA_IN                      PAin(1)

/* LED ״̬�� */
#define LED_POWER 		                PAout(8)        //��ɫָʾ��
#define LED_STANDBY			            PBout(15)       //��ɫָʾ��
#define LED_POWER_TOGGLE                do { LED_POWER = !LED_POWER; } while(0)
#define LED_STANDBY_TOGGLE              do { LED_STANDBY = !LED_STANDBY; } while(0)
#define LED_STANDBY_ON                  do { LED_STANDBY = 1; } while(0)   //����ָʾ�Ƶ���
#define LED_STANDBY_OFF                 do { LED_STANDBY = 0; } while(0)   //����ָʾ�ƹر�
#define LED_POWER_ON                    do { LED_POWER = 1; } while(0) //��Դָʾ�Ƶ���
#define LED_POWER_OFF                   do { LED_POWER = 0; } while(0) //��Դָʾ�ƹر�

/* ��о���� */
#define CAM_RESET                       PBout(14)
#define CAM_POWER                       PCout(13)           //��ISP��Դ
#define CAM_POWER_ON()                  do { CAM_RESET = 1; CAM_POWER = 1; } while(0)  //��ISP��Դ��
#define CAM_POWER_OFF()                 do { CAM_RESET = 0; CAM_POWER = 0; } while(0)  //��ISP��Դ�ر�
#define CAM_RESTART()                   do { CAM_POWER_OFF(); delay_ms(50); CAM_POWER_ON(); } while(0)

/* RS485 */
#define RS485_MODE                      PCout(14)
#define RS485_TX_MODE()                 do{ RS485_MODE = 1; } while(0)
#define RS485_RX_MODE()                 do{ RS485_MODE = 0; } while(0)

/* HDMI */
#define SII9022_RESET                   PBout(1)


/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void Port_Init(void);


#endif
/****************************************** END OF FILE *******************************************/
