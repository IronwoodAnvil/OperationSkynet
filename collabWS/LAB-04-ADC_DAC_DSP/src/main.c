//--------------------------------
// Lab 4 - Sample - Lab04_sample.c
//--------------------------------
//
//

#include <iir_filt.h>
#include "init.h"

#include <stdint.h>
#include "task.h"

#define ADC_TO_VOLTS(adc) ((3.3/4095.0)*(adc))

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	// Code goes here

#if LAB_TASK == 1
	ADC_HandleTypeDef hadc;
	ADC1_Init(&hadc);

	__HAL_RCC_GPIOA_CLK_ENABLE(); // This was actually done in the ADC MSP Init, but it isn't clear, so we do it again
	GPIO_InitTypeDef btn_conf;
	btn_conf.Pin = GPIO_PIN_0;
	btn_conf.Mode = GPIO_MODE_INPUT;
	btn_conf.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &btn_conf);

	uint16_t history[16] = {0};
	uint32_t h_id = 0;
	uint32_t sum16 = 0;

	float max_v = -1;
	float min_v = 5;


	while(1)
	{
		while(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0));
		uint32_t tickstart = HAL_GetTick();

		HAL_ADC_Start(&hadc);
		while(!__HAL_ADC_GET_FLAG(&hadc,ADC_SR_EOC));
		uint16_t conv = HAL_ADC_GetValue(&hadc);

		sum16 -= history[h_id];
		sum16 += conv;
		history[h_id] = conv;
		h_id = (h_id+1)%16;

		float conv_v = ADC_TO_VOLTS(conv);
		if(conv_v > max_v) max_v = conv_v;
		if(conv_v < min_v) min_v = conv_v;

		//Double casts are to suppress warnings
		printf("Raw: %u\tVolts: %f\tHigh: %f\t Low: %f\tAvg: %f\r\n",conv,(double)conv_v,(double)max_v,(double)min_v,(double)ADC_TO_VOLTS(sum16>>4));

		while(HAL_GetTick() < tickstart+20); // Wait for any remaining debounce for press
		while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)); // Wait for release
		HAL_Delay(20); // Debounce release
	}

#elif LAB_TASK == 2
	DAC_HandleTypeDef hdac;
	DAC1_Init(&hdac);

	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	while(1){
		for(uint32_t x = 0; x<4096; x++){
			HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, x);

		}
	}

#elif LAB_TASK == 3

#if SUB_TASK == 1

	asm("mov r9, #2718		\r\n"
		"mov r10, #31415	\r\n"
		"add r9, r9, r10");
	int32_t result;
	asm("str r9, %0"
			: "=m" (result));
	printf("%d+%d = %ld\r\n",2718,31415,result);

	int16_t v1 = 2718;
	int16_t v2 = 31415;
	asm("smulbb %0, %1, %2"
			: "=r" (result) : "r" (v1), "r" (v2));
	printf("%d*%d = %ld\r\n",v1,v2,result);

	int32_t x = 42;
	asm("mov %[r], %[x]		\r\n" // x
		"mul %[r], %[n2]	\r\n" // Multiply by 2
		"sdiv %[r], %[n3]	\r\n" // Divide by 3
		"add %[r], #5		\r\n" // Add 5
			: [r] "=r" (result)
			: [x] "r" (x), [n3] "r" (3), [n2] "r" (2));
	printf("[MUL] %lu*2/3 + 5 = %lu\r\n", x, result);

	result = 0; // Prove that actual result is regenerated
	asm("mov %[r], #15					\r\n"
		"mla %[r], %[x], %[n2], %[r]	\r\n" // 2*x + 15
		"sdiv %[r], %[n3]				\r\n" // Divide by 3
			: [r] "=r" (result)
			: [x] "r" (x), [n3] "r" (3), [n2] "r" (2));
	printf("[MAC] %ld*2/3 + 5 = %ld\r\n", x, result);

	fflush(stdout);

	while(1);

#elif SUB_TASK == 2

#endif

#elif LAB_TASK == 4

	ADC_HandleTypeDef hadc;
	ADC1_Init(&hadc);
	ADC1->CR2 |= ADC_CR2_CONT; // Go continuous mode
    HAL_ADC_Start(&hadc);

#if SUB_TASK == 1

	uint16_t filter = 0;
	uint16_t history[4] = {0};
	uint32_t t = 0;

#define X(n) (history[(n)&0x03])

	while(1)
	{
		while(!__HAL_ADC_GET_FLAG(&hadc,ADC_SR_EOC));
		X(t) = HAL_ADC_GetValue(&hadc);
		filter = 0.312500f*X(t) + 0.240385f*X(t-1) + 0.312500f*X(t-2) + 0.296875f*filter;
		printf("%u\t%u\r\n",X(t),filter);
		++t;
	}

#elif SUB_TASK == 2

	iir_asm_filt(hadc.Instance, NULL);

#endif

#elif LAB_TASK == 5

#endif
}
