/*
 * interrupt.c
 *
 *  Created on: Sep 16, 2019
 *      Author: lovep
 */

#include "stm32f7xx_hal.h"

void EXTI0_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}


// Magically pull this from main.c
extern TIM_HandleTypeDef TIM7_Handle;

void TIM7_IRQHandler()
{
	HAL_TIM_IRQHandler(&TIM7_Handle);
}
