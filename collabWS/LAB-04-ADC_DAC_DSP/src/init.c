#include "init.h"

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 216000000
  *            HCLK(Hz)                       = 216000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 432
  *            PLL_P                          = 2
  *            PLL_Q                          = 9
  *            PLL_R                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
void SystemClock_Config(void) {
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 7;

  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK) {
    while(1) { ; }
  }

  /* Activate the OverDrive to reach the 216 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK) {
    while(1) { ; }
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK) {
    while(1) { ; }
  }
}

// Enables CPU Instruction and Data Caches
void CPU_CACHE_Enable(void) {
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

// Unified System Initialization (equivalent of current MPS Sys_Init())
void Sys_Init(void) {
	//Initialize the system
	CPU_CACHE_Enable();		// Enable CPU Caching
	HAL_Init();				// Initialize HAL
	SystemClock_Config(); 	// Configure the system clock to 216 MHz

	/* UART configured as follows:
		- Word Length = 8 Bits
		- Stop Bit = No Stop bits
		- Parity = None
		- BaudRate = 115200 baud
		- Hardware flow control disabled (RTS and CTS signals)
	*/
	initUart(&USB_UART, 115200, USART1);

	setbuf(stdout,NULL); // Disable buffering of stdout - printf prints everything immediately
}

// This function is what makes everything work
// Don't touch it...
// (Increments the system clock)
void SysTick_Handler(void) {
  HAL_IncTick();
}

// LAB TASK INITIALIZATIONS

#include "task.h"

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{

	if(hadc->Instance == ADC1)
	{
		// Arduino A0, PA6, ADC1_IN6
		__HAL_RCC_ADC1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitTypeDef pin_init;
		pin_init.Pin = GPIO_PIN_6;
		pin_init.Mode = GPIO_MODE_ANALOG;

		HAL_GPIO_Init(GPIOA, &pin_init);
	}
}

void ADC1_Init(ADC_HandleTypeDef* handle)
{
	memset(handle, 0, sizeof(ADC_HandleTypeDef));
	handle->Instance = ADC1;
#if LAB_TASK != 5
	handle->Init.Resolution = ADC_RESOLUTION_12B;
#else
	handle->Init.Resolution = ADC_RESOLUTION_6B; // Lowest resolution = MAX SPEED
#endif

	handle->Init.DataAlign = ADC_DATAALIGN_RIGHT;
	handle->Init.ScanConvMode = ADC_SCAN_DISABLE;
#if LAB_TASK != 1
	handle->Init.ContinuousConvMode = ENABLE;
#endif

	HAL_ADC_Init(handle);

	ADC_ChannelConfTypeDef channel;
	channel.Channel = ADC_CHANNEL_6;
	channel.Rank = 1;
#if LAB_TASK == 1
	channel.SamplingTime = ADC_SAMPLETIME_480CYCLES;
#else
	channel.SamplingTime = ADC_SAMPLETIME_3CYCLES; // Maximum Speed!!! (We should have a strong signal from generator so okay)
#endif

	HAL_ADC_ConfigChannel(handle, &channel);
}



void DAC1_Init(DAC_HandleTypeDef* hdac)
{
	memset(hdac, 0, sizeof(DAC_HandleTypeDef));
	hdac->Instance = DAC1;
	HAL_DAC_Init(hdac);

	DAC_ChannelConfTypeDef sConfig;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;

	HAL_DAC_ConfigChannel(hdac, &sConfig, DAC_CHANNEL_1);

}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	if(hdac->Instance == DAC1)
	{
		// Arduino A, PA4, DAC_OUT_1
		__HAL_RCC_DAC_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitTypeDef pin_init;
		pin_init.Pin = GPIO_PIN_4;
		pin_init.Mode = GPIO_MODE_ANALOG;
		pin_init.Pull = GPIO_NOPULL;

		HAL_GPIO_Init(GPIOA, &pin_init);
	}
}


#if 0
void configureADC()
{
	// Enable the ADC Clock.

	HAL_ADC_Init(...); // Initialize the ADC

	/* Available sampling times:

		ADC_SAMPLETIME_3CYCLES
	  ADC_SAMPLETIME_15CYCLES
		ADC_SAMPLETIME_28CYCLES
		ADC_SAMPLETIME_56CYCLES
		ADC_SAMPLETIME_84CYCLES
		ADC_SAMPLETIME_112CYCLES
		ADC_SAMPLETIME_144CYCLES
		ADC_SAMPLETIME_480CYCLES

	*/

	// Configure the ADC channel

	HAL_ADC_ConfigChannel(...,...);
}


#endif
