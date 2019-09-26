/*
 * main.c
 *
 *  Created on: Sep 16, 2019
 *      Author: lovep
 */

#include "init.h"

#define LAB_TASK 1

#define TIM6_ENABLE() (TIM6->CR1 |= TIM_CR1_CEN)
#define TIM6_DISABLE() (TIM6->CR1 &= ~TIM_CR1_CEN)

TIM_HandleTypeDef TIM7_Handle;

int main()
{
	Sys_Init();

#if LAB_TASK == 1

	EXTI0_HAL_Init();
	EXTI8_Reg_Init();

	EXTI->SWIER |= EXTI_SWIER_SWIER8;
	while(1);

#elif LAB_TASK == 2

	TIM6_Reg_Init();

	printf("Press a key to start the timing\r\n");
	getchar();

	TIM6_ENABLE();
	while(1);

#elif LAB_TASK == 3

	TIM7_HAL_Init(&TIM7_Handle);

	printf("Press a key to start the timing\r\n");
	getchar();

	HAL_TIM_Base_Start_IT(&TIM7_Handle);
	while(1);

#elif LAB_TASK == 4

#endif

}


#define BUTTON_IRQ_DEBOUNCE 20

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	if(GPIO_Pin == GPIO_PIN_0)
	{
		static uint32_t last_change_tick = -BUTTON_IRQ_DEBOUNCE; // Ensure interrupt caught even at startup

		uint32_t cur_tick = HAL_GetTick();

		// Subtraction ensures correct function even at rollover
		if(cur_tick-last_change_tick > BUTTON_IRQ_DEBOUNCE){
			last_change_tick = cur_tick;

			if(HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_0)) // This is the rising event
			{
				printf("Pressed!\r\n");
				printf("Pressed EXTI0!\r\n");
			}
		}
	}
}

void CountAndPrintTime()
{
	static uint32_t time_ovf = 0;
	++time_ovf;
	printf("\rT+%lu.%lus", time_ovf/10, time_ovf%10);
	fflush(stdout);
}

void TIM6_DAC_IRQHandler()
{
	TIM6->SR &= ~TIM_SR_UIF;
	CountAndPrintTime();
}

void EXTI9_5_IRQHandler()
{
	EXTI->PR |= GPIO_PIN_8; //manually remove flag
	printf("Pressed EXT8!\r\n");
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* handle)
{
	if(handle == &TIM7_Handle)
	{
		CountAndPrintTime();
	}
}
