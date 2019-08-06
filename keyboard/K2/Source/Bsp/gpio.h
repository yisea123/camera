/***************************************************************************************************
* FILE: gpio.h
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/1, by kevin
***************************************************************************************************/
#ifndef __GPIO_H_
#define __GPIO_H_

#include "stm32f2xx_hal.h"

/***************************************************************************************************
* DEFINES
***************************************************************************************************/
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)



#define LCD_SDO_Pin GPIO_PIN_6
#define LCD_SDO_GPIO_Port GPIOE
#define KEY34_Pin GPIO_PIN_14
#define KEY34_GPIO_Port GPIOC
#define KEY33_Pin GPIO_PIN_15
#define KEY33_GPIO_Port GPIOC
#define LAN8720A_RESET_Pin GPIO_PIN_8
#define LAN8720A_RESET_GPIO_Port GPIOF
#define LCD_BL_PWM_Pin GPIO_PIN_9
#define LCD_BL_PWM_GPIO_Port GPIOF
#define LCD_RST_Pin GPIO_PIN_10
#define LCD_RST_GPIO_Port GPIOF
#define DIAL1_Pin GPIO_PIN_2
#define DIAL1_GPIO_Port GPIOC
#define DIAL2_Pin GPIO_PIN_3
#define DIAL2_GPIO_Port GPIOC
#define DIAL3_Pin GPIO_PIN_0
#define DIAL3_GPIO_Port GPIOA
#define DIAL4_Pin GPIO_PIN_3
#define DIAL4_GPIO_Port GPIOA
#define ADC_PAN_Pin GPIO_PIN_4
#define ADC_PAN_GPIO_Port GPIOA
#define ADC_TILT_Pin GPIO_PIN_5
#define ADC_TILT_GPIO_Port GPIOA
#define ADC_ZOOM_Pin GPIO_PIN_6
#define ADC_ZOOM_GPIO_Port GPIOA
#define DIAL5_Pin GPIO_PIN_0
#define DIAL5_GPIO_Port GPIOB
#define DIAL6_Pin GPIO_PIN_1
#define DIAL6_GPIO_Port GPIOB
#define DIAL7_Pin GPIO_PIN_2
#define DIAL7_GPIO_Port GPIOB
#define Buzzer_Pin GPIO_PIN_11
#define Buzzer_GPIO_Port GPIOF
#define USB_ENABLE_Pin GPIO_PIN_10
#define USB_ENABLE_GPIO_Port GPIOB
#define KEY_DATA_Pin GPIO_PIN_6
#define KEY_DATA_GPIO_Port GPIOG
#define KEY_CLK_Pin GPIO_PIN_7
#define KEY_CLK_GPIO_Port GPIOG
#define KEY_PL_Pin GPIO_PIN_8
#define KEY_PL_GPIO_Port GPIOG
#define LED_TX_Pin GPIO_PIN_11
#define LED_TX_GPIO_Port GPIOA
#define LED_RX_Pin GPIO_PIN_12
#define LED_RX_GPIO_Port GPIOA

/* SPI */
#define SPI1_CS             PAout(15)
#define SPI1_CS_DISABLE()   SPI1_CS = 1
#define SPI1_CS_ENABLE()    SPI1_CS = 0

/* ²¦Âë */
#define DIAL7           PCin(2)
#define DIAL6           PCin(3)
#define DIAL5           PAin(0)
#define DIAL4           PAin(3)
#define DIAL3           PBin(0)
#define DIAL2           PBin(1)
#define DIAL1           PBin(2)

/* LED×´Ì¬µÆ */
#define LED_TX          PAout(11)
#define LED_TX_ON       LED_TX = 1
#define LED_TX_OFF      LED_TX = 0
#define LED_TX_TOGGLE   LED_TX = !LED_TX
#define LED_RX          PAout(12)
#define LED_RX_ON       LED_RX = 1
#define LED_RX_OFF      LED_RX = 0
#define LED_RX_TOGGLE   LED_RX = !LED_RX

/* ·äÃùÆ÷ */
#define BUZZER          PFout(11)
#define BUZZER_ON       BUZZER = 1
#define BUZZER_OFF      BUZZER = 0

/* °´¼ü */
#define KEY33           PCin(15)
#define KEY34           PCin(14)
#define KEY_PL          PGout(8)
#define KEY_CLK         PGout(7)
#define KEY_DATA        PGin(6)

/* USB */
#define USB_ENABLE      PAout(10) = 1
#define USB_DISABLE     PAout(10) = 0


/* LCD */
#define LCD_RST         PFout(10)
//#define 

/* ETH */
#define ETH_RST         PFout(8)

/* RS485 */
#define RS485_CTRL      PGout(12)
#define RS485_TX_MODE() RS485_CTRL = 1
#define RS485_RX_MODE() RS485_CTRL = 0

/* Tally */
#define TALLY_1         PEout(2)
#define TALLY_2         PEout(3)
#define TALLY_3         PEout(4)
#define TALLY_4         PEout(5)
#define TALLY_5         PCout(13)
#define TALLY_6         PFout(6)
#define TALLY_7         PFout(7)


/***************************************************************************************************
* VARIABLES DECLARATION
***************************************************************************************************/

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
void GPIO_Init(void);

#endif
/****************************************** END OF FILE *******************************************/
