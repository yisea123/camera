/***************************************************************************************************
* FILE: bsp.c

* DESCRIPTION:  --

* CREATED: 2017/10/30, by kevin xu
****************************************************************************************************
* MODIFY:  --

* DESCRIPTION: --

* DATE: --
***************************************************************************************************/

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/
#include "bsp.h"
/***************************************************************************************************
* LOCAL DEFINES
***************************************************************************************************/

/***************************************************************************************************
* LOCAL GLOBAL VARIABLES
***************************************************************************************************/
__I uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
/*!< Uncomment the following line if you need to relocate your vector Table in
Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x0 /*!< Vector Table base offset field.
This value must be a multiple of 0x200. */

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/
static void SetSysClock(void);

/***************************************************************************************************
* Description:  基础设备初始化
***************************************************************************************************/
void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    
    Port_Init();            //初始化所有IO
    I2C1_Init();
    SPI1_Init();
    EEPROM_Init();
}

/***************************************************************************************************
* Description:  
***************************************************************************************************/
void Systick_Init(void)
{
//    NVIC_SetPriority(SysTick_IRQn, 0x00);
//    SysTick_Config(150000);//10ms
//    SysTick_CKSource_Enable(SYSTICK_CKSOURCE_HCLK_DIV8);//96M/8=12MHz
}

/***************************************************************************************************
* Description:  延时微妙
***************************************************************************************************/
void delay_us(uint16_t nus)
{
    if(nus > 1000)                          //最大1ms
        nus = 1000;
    SysTick->LOAD = 12*nus;                 //计算并装入重载值
    SysTick->VAL = 0;                       //清零当前值
    SysTick->CTRL = 0x01;                   //开启SysTick
    while(!(SysTick->CTRL & 0x00010000));
    SysTick->CTRL = 0x00;                   //关闭SysTick
}

/***************************************************************************************************
* Description:  毫秒延时
***************************************************************************************************/
void delay_ms(uint32_t nms)
{
    SysTick->LOAD = 12000;  //1ms
    SysTick->VAL = 0;
    SysTick->CTRL = 0x01;   //开启SysTick
    while(nms--)
    {
        while(!(SysTick->CTRL & 0x00010000));
    }
    SysTick->CTRL = 0x00;   //关闭SysTick
}

#ifdef __cplusplus
 extern "C" {
#endif
/***************************************************************************************************
* Description:  单片机时钟初始化
***************************************************************************************************/
void SystemInit (void)
{
    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;

    /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
    RCC->CFGR &= (uint32_t)0xF0FF0000;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
    RCC->CFGR &= (uint32_t)0xFF80FFFF;

    /* Disable all interrupts and clear pending bits  */
    RCC->CIR = 0x009F0000;

#ifdef DATA_IN_ExtSRAM
    SystemInit_ExtMemCtl();
#endif /* DATA_IN_ExtSRAM */

    /* Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
    /* Configure the Flash Latency cycles and enable prefetch buffer */
    SetSysClock();

#ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
#else
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
#endif
}
/***************************************************************************************************
* Description:  设置系统时钟为96MHz，晶振频率12MHz
* Arguments:    none
* Returns:      none
***************************************************************************************************/
static void SetSysClock(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    }while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 2 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;


        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

        /*  PLL configuration: PLLCLK = HSE * 8 = 96 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                            RCC_CFGR_PLLMULL));
         RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLXTPRE_HSE | RCC_CFGR_PLLMULL8);

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock
        configuration. User can add here some code to deal with this error */
        while(1);
    }
}
#ifdef __cplusplus
 }
#endif
/****************************************** END OF FILE *******************************************/
