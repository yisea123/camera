/***************************************************************************************************
* FILE: bsp.c
*
* DESCRIPTION:  --
*
* CREATED: 2018/8/1, by kevin
***************************************************************************************************/

/***************************************************************************************************
* INCLUDES
***************************************************************************************************/
#include "bsp.h"
/***************************************************************************************************
* DEFINES
***************************************************************************************************/

/***************************************************************************************************
* VARIABLES
***************************************************************************************************/
TIM_HandleTypeDef        htim7; 
uint32_t                 uwIncrementState = 0;

/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/


/**
* @brief  This function is executed in case of error occurrence.
* @param  file: The file name as string.
* @param  line: The line in file as a number.
* @retval None
*/
void _Error_Handler(char *file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    {
        HAL_Delay(2000);
        printf("\r\nWelcome To Error Handler! ^_^\r\n");
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{ 
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
    tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/***************************************************************************************************
* Description:  设置时钟
***************************************************************************************************/
void SystemClock_Config(void)
{
    
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    
    /**Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 20;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 5;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    /**Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
        |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    /**Configure the Systick interrupt time 
    */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    
    /**Configure the Systick 
    */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/**
* @brief  Period elapsed callback in non blocking mode
* @note   This function is called  when TIM7 interrupt took place, inside
* HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
* a global variable "uwTick" used as application time base.
* @param  htim : TIM handle
* @retval None
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* USER CODE BEGIN Callback 0 */
    
    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM7) {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */
    
    /* USER CODE END Callback 1 */
}

void HAL_MspInit(void)
{
    /* USER CODE BEGIN MspInit 0 */
    
    /* USER CODE END MspInit 0 */
    
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_CRC_CLK_ENABLE();
    
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    
    /* System interrupt init*/
    /* MemoryManagement_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    /* BusFault_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    /* UsageFault_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
    /* SVCall_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
    /* DebugMonitor_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
    /* PendSV_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
    
    
}

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    RCC_ClkInitTypeDef    clkconfig;
    uint32_t              uwTimclock = 0;
    uint32_t              uwPrescalerValue = 0;
    uint32_t              pFLatency;
    
    /*Configure the TIM7 IRQ priority */
    HAL_NVIC_SetPriority(TIM7_IRQn, TickPriority ,0); 
    
    /* Enable the TIM7 global Interrupt */
    HAL_NVIC_EnableIRQ(TIM7_IRQn); 
    
    /* Enable TIM7 clock */
    __HAL_RCC_TIM7_CLK_ENABLE();
    
    /* Get clock configuration */
    HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
    
    /* Compute TIM7 clock */
    uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
    
    /* Compute the prescaler value to have TIM7 counter clock equal to 1MHz */
    uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000) - 1);
    
    /* Initialize TIM7 */
    htim7.Instance = TIM7;
    
    /* Initialize TIMx peripheral as follow:
    + Period = [(TIM7CLK/1000) - 1]. to have a (1/1000) s time base.
    + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
    + ClockDivision = 0
    + Counter direction = Up
    */
    htim7.Init.Period = (1000000 / 1000) - 1;
    htim7.Init.Prescaler = uwPrescalerValue;
    htim7.Init.ClockDivision = 0;
    htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
    if(HAL_TIM_Base_Init(&htim7) == HAL_OK)
    {
        /* Start the TIM time Base generation in interrupt mode */
        return HAL_TIM_Base_Start_IT(&htim7);
    }
    
    /* Return function status */
    return HAL_ERROR;
}

void HAL_SuspendTick(void)
{
    /* Disable TIM7 update Interrupt */
    __HAL_TIM_DISABLE_IT(&htim7, TIM_IT_UPDATE);                                                  
}

void HAL_ResumeTick(void)
{
    /* Enable TIM7 Update interrupt */
    __HAL_TIM_ENABLE_IT(&htim7, TIM_IT_UPDATE);
}

void NMI_Handler(void)
{
    
}
void HardFault_Handler(void)
{
    while (1)
    {
    }
}
void MemManage_Handler(void)
{
    while (1)
    {
    }
}
void BusFault_Handler(void)
{
    while (1)
    {
        
    }
    
}

void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

void DebugMon_Handler(void)
{
}

//void SysTick_Handler(void)
//{
//
//  osSystickHandler();
//
//}

void TIM7_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim7);
}

void DMA2_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc1);
}

void DMA2_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart6_rx);
}

void DMA2_Stream2_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

void DMA2_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart6_tx);
}

void DMA2_Stream7_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

extern ETH_HandleTypeDef heth;
void ETH_IRQHandler(void)
{
    HAL_ETH_IRQHandler(&heth);
}

//void OTG_HS_IRQHandler(void)
//{
//  HAL_PCD_IRQHandler(&hpcd_USB_OTG_HS);
//}

/***************************************************************************************************
* Description:  基础环境初始化
***************************************************************************************************/
void Bsp_Init(void)
{
    HAL_Init();
    
    SystemClock_Config();
    
    /* 基本IO口初始化 */
    GPIO_Init();
    
    /* DMA UART等*/
    DMA_Init();
    
    /* SRAM和LCD屏接口 */
    FSMC_Init();
    
    /* 调试串口 */
    UART4_Init(1500000);

    /* I2C，SN3731、EEPROM */
//    I2C1_Init();
    I2C3_Init();
    
    EEPROM_Init();
    
    /* SPIFLASH */
    SPI1_Init();
    
    /* 摇杆 */
    ADC1_Init();
}




/****************************************** END OF FILE *******************************************/
