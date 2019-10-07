/*
 * main.c
 *
 *  Created on: Oct 7, 2019
 *      Author: lovep
 */

#include "init.h"
#include <stdio.h>
#include <stdbool.h>


#define REG_COUNT 10
#define TRANSACT_TIMEOUT 1

static SPI_HandleTypeDef SPI;

uint8_t NO_RESPONSE() { return 0xAA; }
void IGNORE_RESULT(uint8_t x) {}

const static void(*result_table[REG_COUNT])(uint8_t) =
{
		IGNORE_RESULT,IGNORE_RESULT
};
const static uint8_t(*response_table[REG_COUNT])(void) =
{
		NO_RESPONSE,NO_RESPONSE
};

bool read_cs()
{
	return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11);
}

int main()
{
	Sys_Init();

	SPI_Init(&SPI);
//	__HAL_RCC_SYSCFG_CLK_ENABLE();
//	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
//	SYSCFG->EXTICR[11/4] |= SYSCFG_EXTICR3_EXTI11_PA;
//	EXTI->IMR |= EXTI_IMR_IM11;
//	// Rising and falling
//	EXTI->RTSR |= EXTI_RTSR_TR11;
//	EXTI->FTSR |= EXTI_FTSR_TR11;

	while(1)
	{

		while(read_cs());  // Wait for request
		uint8_t reg;
		HAL_StatusTypeDef status = HAL_SPI_Receive(&SPI, &reg, 1, TRANSACT_TIMEOUT);

		printf("R%d", reg);

		uint8_t response = 0x55; // Compute response (should have 10 us)
		if(status==HAL_OK && reg<REG_COUNT)
		{
			response = response_table[reg]();
		} else {
			printf("E");
		}

		uint8_t incoming;
		HAL_SPI_TransmitReceive(&SPI, &response, &incoming, 1, TRANSACT_TIMEOUT);

		printf("S");

		if(status==HAL_OK && reg<REG_COUNT)
		{
			result_table[reg](incoming);
		} else {
			printf("E");
		}

		printf(" %d\r\n",reg);
		while(!read_cs()); // Wait for end of transaction
	}
}

//void EXTI15_10_IRQHandler()
//{
//	if(EXTI->PR & EXTI_PR_PR11) //This is EXTI 11
//	{
//		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11))
//		{ // Rising edge (deassert)
//			if(HAL_SPI_GetState(&SPI) != HAL_SPI_STATE_READY) // If it is doing something, abort
//			{
//				HAL_SPI_Abort(&SPI);
//			}
//			request = false;
//		}
//		else
//		{ // Falling edge (assert)
//			request = true;
//		}
//
//		EXTI->PR = EXTI_PR_PR11; // Clear pending flag
//	}
//}

