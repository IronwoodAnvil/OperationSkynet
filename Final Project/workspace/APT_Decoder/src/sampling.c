/*
 * sampling.c
 *
 *  Created on: Nov 21, 2019
 *      Author: lovep
 */

#include "sampling.h"
#include "stm32f7xx_hal.h"
#include <stdlib.h>

// 8th order elliptic LPF for AM demodulation
// Passband ripple 0.5 dB to 2100 Hz
// Stopband attnuation -68 dB past 2700 Hz
const float FILT_NEXT_SAMP_COEFF = 0.000678838494336f;
const float FILT_SAMP_COEFF[] = {
		  -0.003534196288993f,
		   0.008994777919394f,
		  -0.014666976055880f,
		   0.017088053114672f,
		  -0.014666976055880f,
		   0.008994777919394f,
		  -0.003534196288993f,
		   0.000678838494336f
};
const float FILT_FILT_COEFF[] = {
		   7.200781848522088f,
		 -23.028289310289551f,
		  42.687438666850078f,
		 -50.139405828371544f,
		  38.197599092024909f,
		 -18.427485269037746f,
		   5.146330495211290f,
		  -0.637004588053818f
};
// History buffers for filter
float samp_hist[8] = {0};
float filt_hist[8] = {0};
uint32_t hist_id = 0;
#define HIST_ID_MASK (0x07)

// Sample history buffer for sync detect filter
uint16_t samples[256] = {0};
uint8_t sid = 0;

// Current sync correlation value, sample, and new sample indication flag
volatile int32_t sync_corr = 0;
volatile uint16_t current_sample = 0;
volatile bool new_sample = false;



ADC_HandleTypeDef hadc;
TIM_HandleTypeDef htim;

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// Arduino A0, PA6, ADC1_IN6
	GPIO_InitTypeDef pin_init;
	pin_init.Pin = GPIO_PIN_6;
	pin_init.Mode = GPIO_MODE_ANALOG;

	HAL_GPIO_Init(GPIOA, &pin_init);

	HAL_NVIC_EnableIRQ(ADC_IRQn);
}

static void SamplingTimerInit()
{
	__HAL_RCC_TIM6_CLK_ENABLE();

	htim.Instance = TIM6;
	htim.Init.Prescaler = 0;
//	htim.Init.Period = -1;
	htim.Init.Period = 3245-1; // Sample at 108 MHz/3245 = 33,282 Hz, very close to target 33,280 = 8*4160

	HAL_TIM_Base_Init(&htim);

	// Set it in update event trigger mode
	TIM6->CR1 &= ~TIM_CR1_UDIS;
	TIM6->CR2 |= TIM_CR2_MMS_1;
}

static void ADC_Init()
{
	hadc.Instance = ADC1;
	hadc.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4; // 216 MHz/4/4 = 27 MHz < 36 MHz
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT; // 12 bit, right aligned
	hadc.Init.Resolution = ADC_RESOLUTION_12B;
	hadc.Init.ScanConvMode = ADC_SCAN_DISABLE; // Only reading one channel
	hadc.Init.ExternalTrigConv = ADC_EXTERNALTRIG0_T6_TRGO; // Trigger on Timer 6
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;

	HAL_ADC_Init(&hadc);

	ADC_ChannelConfTypeDef channel;
	channel.Channel = ADC_CHANNEL_6;
	channel.Rank = 1;
	channel.SamplingTime = ADC_SAMPLETIME_480CYCLES; // We have time for it, this gives more time for sample cap to voltage to converge
	HAL_ADC_ConfigChannel(&hadc, &channel);

}

void Sampling_Init()
{
	SamplingTimerInit();
	ADC_Init();
}

void Sampling_Start()
{
	HAL_ADC_Start_IT(&hadc);
	HAL_TIM_Base_Start(&htim);
}

// Updates the sync correlation value for the latest sample
// Make more effcient by only adding/subtracting at edges
static void update_sync()
{
	// Use local accumulator to avoid excessive references to volatile sync_corr variable which will go unoptimized
	int32_t new_val = sync_corr;
	new_val += samples[sid]; // Add next sample (so double subtract later is correct)
	new_val -= samples[(uint8_t)(sid-SYNC_LENGTH)]; // Remove oldest sample
	uint8_t index = sid;
	for(uint8_t i=0; i<7; ++i) // Update all edges going back
	{
		new_val -= 2*samples[index];
		new_val += 2*samples[(uint8_t)(index-(SYNC_PERIOD/2))];
		index -= SYNC_PERIOD; // Wraparound of 256 element buffer is automatic due to byte size
	}
	sync_corr = new_val; // Assign computed val back to sync_corr
}

static void demodulate(int16_t next_adc)
{
	// Scale so samples use full uint16_t range
	// abs(x) 11 bit, filter extracts half amplitude for difference frequency so 10 bit.
	// for 16 bit input shift 6 (multiply by 64)
	float sample = (float)(abs(next_adc)<<6);
	float f_next = FILT_NEXT_SAMP_COEFF*sample;

	for(uint32_t i = 0; i < 8; ++i) // Add historical terms
	{
		uint32_t id = (hist_id-i)&HIST_ID_MASK;
		f_next += FILT_SAMP_COEFF[i]*samp_hist[id];
		f_next += FILT_FILT_COEFF[i]*filt_hist[id];
	}

	hist_id = (hist_id+1)&HIST_ID_MASK; // Increment history buffer index
	samp_hist[hist_id] = sample; // Add sample to history
	filt_hist[hist_id] = f_next; // Add filter value to history
	// Saturate filter value to 16 bit
	if(f_next < 0) f_next = 0.0;
	if(f_next > 0xFFFF) f_next = 0xFFFF;
	// Save filtered sample to next sample array position
	// Wraparound is handles since sid is a byte type and samples is a 256 element buffer
	samples[++sid] = (uint16_t)f_next;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	int16_t adc_val = HAL_ADC_GetValue(hadc) - DC_OFFSET;
	demodulate(adc_val);
	update_sync();
//	if(new_sample) puts("Framing Overrun!\r");
	current_sample = samples[sid];
	new_sample = true;
}

void ADC_IRQHandler()
{
	HAL_ADC_IRQHandler(&hadc);
}
