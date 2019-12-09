#include "init.h"
#include "sampling.h"
#include "framing.h"

void DAC1_Init(DAC_HandleTypeDef* hdac)
{
	memset(hdac, 0, sizeof(DAC_HandleTypeDef));
	hdac->Instance = DAC1;
	HAL_DAC_Init(hdac);

	DAC_ChannelConfTypeDef sConfig;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE; // Output immediately on register update
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;

	HAL_DAC_ConfigChannel(hdac, &sConfig, DAC_CHANNEL_1);

}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	if(hdac->Instance == DAC1)
	{
		// Arduino A1, PA4, DAC_OUT_1
		__HAL_RCC_DAC_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitTypeDef pin_init;
		pin_init.Pin = GPIO_PIN_4;
		pin_init.Mode = GPIO_MODE_ANALOG;
		pin_init.Pull = GPIO_NOPULL;

		HAL_GPIO_Init(GPIOA, &pin_init);
	}
}

// Main Execution Loop
int main(void)
{
	Sys_Init();
	Sampling_Init();

	DAC_HandleTypeDef hdac;
	DAC1_Init(&hdac);
	HAL_DAC_Start(&hdac, 0);

	LTDC_HandleTypeDef hltdc;
	HAL_LTDC_Init(&hltdc);


	Framing_Init(&hdac);


	Sampling_Start();
	while(1)
	{
		if(new_sample)
		{
//			int32_t sync = (sync_corr/64/64) + DC_OFFSET;
//			if(sync < 0) sync = 0;
//			if(sync > 0xFFF) sync = 0xFFF;
//			HAL_DAC_SetValue(&hdac, 0, DAC_ALIGN_12B_R, sync);//current_sample>>4);

			Framing_Tasks();
			new_sample = false;
		}
	}

}

