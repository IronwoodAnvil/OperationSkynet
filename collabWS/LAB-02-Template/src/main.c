/*
 * main.c
 *
 *  Created on: Sep 16, 2019
 *      Author: lovep
 */

#include "init.h"

#define LAB_TASK 3

TIM_HandleTypeDef TIM7_Handle;

int main()
{
	Sys_Init();

#if LAB_TASK == 1

	EXTI0_HAL_Init();

	while(1);

#elif LAB_TASK == 2

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

#if LAB_TASK == 1
				printf("Pressed!\r\n");
#endif
			}
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* handle)
{
	static uint32_t time_ovf = 0;
	if(handle == &TIM7_Handle)
	{
		++time_ovf;
		printf("\rT+%lu.%lus", time_ovf/10, time_ovf%10);
		fflush(stdout);
	}
}
