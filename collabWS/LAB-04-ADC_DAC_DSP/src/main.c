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



#if LAB_TASK == 5

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

#define DC_OFFSET (1<<11)

// 9th bit used as the waiting flag
#define ADC_VAL_WAITING (1<<12)
volatile uint32_t adc1_val = ADC_VAL_WAITING;
volatile uint32_t adc2_val = ADC_VAL_WAITING;

void ADC_IRQHandler()
{
//	printf("0x%02X 0x%02X\r\n", hadc1.Instance->SR, hadc2.Instance->SR);
	if(hadc1.Instance->SR & ADC_SR_EOC) // ADC 1 Value
	{
		adc1_val = hadc1.Instance->DR; // Clears EOC flag
	}
	if(hadc2.Instance->SR & ADC_SR_EOC) // ADC 2 Value
	{
		adc2_val = hadc2.Instance->DR;
	}
//	printf("0x%02X 0x%02X\r\n\r\n", hadc1.Instance->SR, hadc2.Instance->SR);
}


#endif


// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	// Code goes here

#if LAB_TASK == 1
	ADC_HandleTypeDef hadc;
	ADC_Init(ADC1, &hadc);

	__HAL_RCC_GPIOA_CLK_ENABLE(); // This was actually done in the ADC MSP Init, but it isn't clear, so we do it again
	GPIO_InitTypeDef btn_conf;
	btn_conf.Pin = GPIO_PIN_0; // set read pin for adc
	btn_conf.Mode = GPIO_MODE_INPUT;
	btn_conf.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &btn_conf);

	uint16_t history[16] = {0}; //array where previous reads are stored
	uint32_t h_id = 0;
	uint32_t sum16 = 0;

	float max_v = -1; //keeps max
	float min_v = 5; // keeps min


	while(1)
	{
		while(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)); //read PA0 value when button not pressed
		uint32_t tickstart = HAL_GetTick();

		HAL_ADC_Start(&hadc);
		while(!__HAL_ADC_GET_FLAG(&hadc,ADC_SR_EOC)); //wait until conversion done
		uint16_t conv = HAL_ADC_GetValue(&hadc); //get conversion

		sum16 -= history[h_id]; // history manipulation
		sum16 += conv;
		history[h_id] = conv;
		h_id = (h_id+1)%16;

		float conv_v = ADC_TO_VOLTS(conv); //use conversion factor for adc val tovolts
		if(conv_v > max_v) max_v = conv_v;
		if(conv_v < min_v) min_v = conv_v;

		//Double casts are to suppress warnings
		printf("Raw: %4u  Volts: %f  High: %f  Low: %f  Avg: %f\r\n",conv,(double)conv_v,(double)max_v,(double)min_v,(double)ADC_TO_VOLTS(sum16>>4));

		while(HAL_GetTick() < tickstart+20); // Wait for any remaining debounce for press
		while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)); // Wait for release
		HAL_Delay(20); // Debounce release
	}

#elif LAB_TASK == 2
	DAC_HandleTypeDef hdac; //inits
	DAC1_Init(&hdac);

	ADC_HandleTypeDef hadc;
	ADC_Init(ADC1, &hadc);
	HAL_ADC_Start(&hadc);

	HAL_DAC_Start(&hdac, DAC_CHANNEL_1); //dac1

	uint32_t count = 0;
	while(1){
#if SUB_TASK == 2
		while(!__HAL_ADC_GET_FLAG(&hadc,ADC_SR_EOC)); //wait for adc conversion
		uint16_t conv = HAL_ADC_GetValue(&hadc); //get val
#elif SUB_TASK == 1
		uint16_t conv = count; //makes sawtooth
		++count;
		if(count > 4095) count = 0;
#endif
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, conv); // set dac1 to conv
	}

#elif LAB_TASK == 3

#if SUB_TASK == 1

	asm("mov r6, #2718		\r\n" //objective 1
		"mov r7, #31415	\r\n"
		"add r6, r6, r7");
	int32_t result;
	asm("str r6, %0"
			: "=m" (result));
	printf("%d+%d = %ld\r\n",2718,31415,result);

	int16_t v1 = 2718; //objective 2
	int16_t v2 = 31415;
	asm("smulbb %0, %1, %2"
			: "=r" (result) : "r" (v1), "r" (v2));
	printf("%d*%d = %ld\r\n",v1,v2,result);

	int32_t x = 42; // objective 3
	asm("mov %[r], %[x]		\r\n" // x
		"mul %[r], %[n2]	\r\n" // Multiply by 2
		"sdiv %[r], %[n3]	\r\n" // Divide by 3
		"add %[r], #5		\r\n" // Add 5
			: [r] "=r" (result)
			: [x] "r" (x), [n3] "r" (3), [n2] "r" (2));
	printf("[MUL] %lu*2/3 + 5 = %lu\r\n", x, result);

	//objective 4
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
	int32_t result1; //objective 1
	asm("mov r6, #8675		\r\n"
		"mov r7, #309	\r\n"
		"add r6, r6, r7");
	asm("str r6, %0"
			: "=m" (result1));
	printf("%d+%d = %ld\r\n",8675,309,result1);

	float result; //objective 2
	float v1 = 8675.309;
	float v2 = 525.600;
	asm("VMUL.F32 %0, %1, %2"
			: "=t" (result) : "t" (v1), "t" (v2));
	printf("%f*%f = %f\r\n",v1,v2,result);

	//objective 3
	//evaluate the equation 2x/3 + 5 through floating point addition (VADD), multiplication (VMUL), and division (VDIV)
	float x = 3;
	asm("vmov %[w], %[x]		\r\n" // x
		"vmul.f32 %[w], %[n2]	\r\n" // Multiply by 2
		"vdiv.f32 %[w], %[w], %[n3]	\r\n" // Divide by 3
		"vadd.f32 %[w], %[n4], %[w]	\r\n" // Add 5
			: [w] "+t" (result)
			: [x] "t" (x), [n3] "t" (3.0f), [n2] "t" (2.0f), [n4] "t" (5.0f));
	printf("[MUL] %f*2/3 + 5 = %f\r\n", x, result);

	result = 0; // Prove that actual result is regenerated
		//objective 4
		asm("vmov %[w], %[n4]		\r\n" // 15
			"vmla.f32 %[w], %[x], %[n2] \r\n" //x * 2 added to 15
			"vdiv.f32 %[w], %[w], %[n3]	\r\n" // Divide by 3
				: [w] "+t" (result)
				: [x] "t" (x), [n3] "t" (3.0f), [n2] "t" (2.0f), [n4] "t" (15.0f));
		printf("[MUL2] %f*2/3 + 5 = %f\r\n", x, result);



#endif

#elif LAB_TASK == 4

	ADC_HandleTypeDef hadc; //inits
	ADC_Init(ADC1, &hadc);
    HAL_ADC_Start(&hadc);

	DAC_HandleTypeDef hdac;
	DAC1_Init(&hdac);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

#if SUB_TASK == 1

	uint16_t filter = 0; //history for equation h , h-1, h-2 ...
	uint16_t history[4] = {0};
	uint32_t t = 0;

#define X(n) (history[(n)&0x03])

	while(1)
	{

		while(!__HAL_ADC_GET_FLAG(&hadc,ADC_SR_EOC)); //wait for conversion
		X(t) = HAL_ADC_GetValue(&hadc);
		filter = 0.312500f*X(t) + 0.240385f*X(t-1) + 0.312500f*X(t-2) + 0.296875f*filter; //our filter
		if(filter >= 1<<12) filter = (1<<12) - 1; // Saturate to 12 bits
		HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, filter); //set dac
		++t;
	}

#elif SUB_TASK == 2

	iir_asm_filt(hadc.Instance, hdac.Instance); //assembly implementaion

#endif

#elif LAB_TASK == 5

	// See globals and interrupt stuff at top

	ADC_Init(ADC1, &hadc1);
	ADC_Init(ADC3, &hadc2);

	TIM_HandleTypeDef sample_time;
	SamplingTimerInit(&sample_time);
	HAL_TIM_Base_Start(&sample_time);

	DAC_HandleTypeDef hdac;
	DAC1_Init(&hdac);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

	// Store in float to avoid need to reconvert whenever used
	float x_history[4] = {0};
	float y_history[4] = {0};
	uint32_t t = 0;

#define X(t) (x_history[(t)&0x3])
#define Y(t) (y_history[(t)&0x3])

	HAL_ADC_Start_IT(&hadc1);
	HAL_ADC_Start_IT(&hadc2);

	while(1)
	{
		// Wait for both values
		while( (adc1_val & ADC_VAL_WAITING) || (adc2_val & ADC_VAL_WAITING) );

		// Divide by offset (max amplitude) as multiply has max amplitude of DC_OFFSET^2
		float Xt = ((int32_t)(adc1_val-DC_OFFSET) * (int32_t)(adc2_val-DC_OFFSET)) / ((float)(DC_OFFSET));

		float Yt =
				0.001f*Xt - 0.002f*X(t-2) + 0.001f*X(t-4)
				+ 3.166f*Y(t-1) - 4.418f*Y(t-2) + 3.028f*Y(t-3) - 0.915f*Y(t-4);

		int32_t out = ((int32_t)Yt)+DC_OFFSET;
		// Saturation
		if(out < 0) out = 0;
		if(out > ((1<<12)-1)) out = (1<<12)-1;

		hdac.Instance->DHR12R1 = out;
		adc1_val = adc2_val = ADC_VAL_WAITING; // Set waiting flags (wait to do this so DAC is set ASAP)
		// Must do after eval since this overwrites X(t-4) which is actually the same spot in circ buff
		Y(t) = Yt;
		X(t) = Xt;
		++t;

//		__disable_irq();
//		printf("A1: %lu\t A2: %lu\tX: %f\tY: %f\r\n",adc1_val,adc2_val,(double)Xt,(double)Yt);
//		__enable_irq();
	}

#endif
}

