/*
 * interrupt.c
 *
 *  Created on: Sep 16, 2019
 *      Author: lovep
 */

#include "stm32f7xx_hal.h"
#include <stdio.h>

void EXTI0_IRQHandler()
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

