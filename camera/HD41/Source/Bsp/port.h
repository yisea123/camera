/***************************************************************************************************
* FILE: port.h
*
* DESCRIPTION: 92U3 MCU IO口配置
*
* CREATED: 2018/02/09, by kevin xu
***************************************************************************************************/
#ifndef PORT_H
#define PORT_H

/***************************************************************************************************
* DEFINES
***************************************************************************************************/
//IO口操作宏定义
#define BIT_DATA(va , n)		((va & (1<<n)) >> n)
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
//IO口地址映射
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

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入


/* SPI 片选信号 */
#define SPI1_CS                         PAout(4)
#define SPI2_CS                         PBout(12)
#define SPI3_CS                         PAout(15)

/* 云台限位检测点 */
#define PAN_SENSE                       PBin(3)
#define TILT_SENSE                      PBin(4)
#define IS_PAN_SENSE                    (!PAN_SENSE)
#define IS_TILT_SENSE                   (!TILT_SENSE)

/* MS41909 */
#define MS41909_PULSE                   PBout(5)


/* 拨码开关 */
#define DIAL_1                          PAin(15)
#define DIAL_2                          PAin(12)
#define DIAL_4                          PAin(11)
#define DIAL_5                          PCin(15)


/* 遥控器 */
#define IR_ADDR_1                       (!DIAL_1)  
#define IR_ADDR_2                       (!DIAL_2)  
#define IR_DATA_IN                      PAin(0)
#define WL_DATA_IN                      PAin(1)

/* LED 状态灯 */
#define LED_POWER 		                PAout(8)        //绿色指示灯
#define LED_STANDBY			            PBout(15)       //红色指示灯
#define LED_POWER_TOGGLE                do { LED_POWER = !LED_POWER; } while(0)
#define LED_STANDBY_TOGGLE              do { LED_STANDBY = !LED_STANDBY; } while(0)
#define LED_STANDBY_ON                  do { LED_STANDBY = 1; } while(0)   //待机指示灯点亮
#define LED_STANDBY_OFF                 do { LED_STANDBY = 0; } while(0)   //待机指示灯关闭
#define LED_POWER_ON                    do { LED_POWER = 1; } while(0) //电源指示灯点亮
#define LED_POWER_OFF                   do { LED_POWER = 0; } while(0) //电源指示灯关闭

/* 机芯部分 */
#define CAM_RESET                       PBout(14)
#define CAM_POWER                       PCout(13)           //主ISP电源
#define CAM_POWER_ON()                  do { CAM_RESET = 1; CAM_POWER = 1; } while(0)  //主ISP电源打开
#define CAM_POWER_OFF()                 do { CAM_RESET = 0; CAM_POWER = 0; } while(0)  //主ISP电源关闭
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
