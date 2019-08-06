/***************************************************************************************************
* FILE: tim.c
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
TIM_HandleTypeDef hTim14;
/***************************************************************************************************
* FUNCTION DECLARATION
***************************************************************************************************/



/* TIM14 init function */
void TIM14_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC;
    
    hTim14.Instance = TIM14;
    hTim14.Init.Prescaler = 0;
    hTim14.Init.CounterMode = TIM_COUNTERMODE_UP;
    hTim14.Init.Period = 255;
    hTim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    hTim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&hTim14) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    if (HAL_TIM_PWM_Init(&hTim14) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&hTim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    HAL_TIM_MspPostInit(&hTim14);
    
    HAL_TIM_PWM_Start(&hTim14, TIM_CHANNEL_1);
    
    HAL_TIM_Base_Start(&hTim14);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
    
    if(tim_baseHandle->Instance==TIM14)
    {
        /* TIM14 clock enable */
        __HAL_RCC_TIM14_CLK_ENABLE();
    }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{
    
    GPIO_InitTypeDef GPIO_InitStruct;
    if(timHandle->Instance==TIM14)
    {  
        /**TIM14 GPIO Configuration    
        PF9     ------> TIM14_CH1 
        */
        GPIO_InitStruct.Pin = LCD_BL_PWM_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF9_TIM14;
        HAL_GPIO_Init(LCD_BL_PWM_GPIO_Port, &GPIO_InitStruct);
    }
    
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
    
    if(tim_baseHandle->Instance==TIM14)
    {
        /* Peripheral clock disable */
        __HAL_RCC_TIM14_CLK_DISABLE();
    }
}

/****************************************** END OF FILE *******************************************/
