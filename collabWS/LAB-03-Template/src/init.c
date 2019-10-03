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
    while(1);
  }

  /* Activate the OverDrive to reach the 216 MHz Frequency */
  ret = HAL_PWREx_EnableOverDrive();
  if(ret != HAL_OK) {
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
  if(ret != HAL_OK) {
    while(1);
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

}

// This function is what makes everything work
// Don't touch it...
// (Increments the system clock)
void SysTick_Handler(void) {
  HAL_IncTick();
}

// Initialize Hardware Resources
// Peripheral's clock enable
// Peripheral's GPIO Configuration
void HAL_UART_MspInit(UART_HandleTypeDef *huart){
	GPIO_InitTypeDef  GPIO_InitStruct;
	if (huart->Instance == USART1) {
		// Enable GPIO Clocks
		__GPIOA_CLK_ENABLE();

		// Initialize TX Pin
		GPIO_InitStruct.Pin       = GPIO_PIN_9;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //TX Config

		// Initialize RX Pin
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //RX Config

		// Enable UART Clocking
		__USART1_CLK_ENABLE();

	} else if (huart->Instance == USART6) {
		// Enable GPIO Clocks
		__GPIOC_CLK_ENABLE();

		// Initialize TX Pin
		GPIO_InitStruct.Pin       = GPIO_PIN_6;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); //TX Config

		// Initialize RX Pin
		GPIO_InitStruct.Pin = GPIO_PIN_7;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); //RX Config

		// Enable UART Clocking
		__USART6_CLK_ENABLE();

		huart->AdvancedInit.RxPinLevelInvert = UART_ADVFEATURE_RXINV_ENABLE;
		huart->AdvancedInit.TxPinLevelInvert = UART_ADVFEATURE_TXINV_ENABLE;
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	if (huart->Instance == USART1) {
		__USART1_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);
		//Don't disable clock, other pins might be in use
	}
	else if(huart->Instance == USART6) {
		__USART6_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6);
		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_7);
		//Don't disable clock, other pins might be in use
	}
}

/*
 * For convenience, configure the SPI handler here
 */
// See 769 Description of HAL drivers.pdf, Ch. 58.1 or stm32f7xx_hal_spi.c
void SPI_Init(SPI_HandleTypeDef* handle)
{
	handle->Instance = SPI2;
	handle->Init.Mode = SPI_MODE_MASTER;
	handle->Init.TIMode = SPI_TIMODE_DISABLE;
	//216e6 / 4 (APB Prescaler) / 64 (SPI Prescaler) = 843.75 kHz ~ 1 MHz
	handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	handle->Init.Direction = SPI_DIRECTION_2LINES;
	handle->Init.DataSize = SPI_DATASIZE_8BIT;
	handle->Init.FirstBit = SPI_FIRSTBIT_LSB;
	handle->Init.CLKPhase = SPI_PHASE_2EDGE;
	handle->Init.CLKPolarity = SPI_POLARITY_HIGH;
	handle->Init.NSS = SPI_NSS_SOFT;

	HAL_SPI_Init(handle);
}


/*
 * This is called upon SPI initialization. It handles the configuration
 * of the GPIO pins for SPI.
 */
 // Do NOT change the name of an MspInit function; it needs to override a
 // __weak function of the same name. It does not need a prototype in the header.
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	if (hspi->Instance == SPI2)
	{
		// Enable SPI GPIO port clocks, set HAL GPIO init structure's values for each
		// SPI-related port pin (SPI port pin configuration), enable SPI IRQs (if applicable), etc.
		__SPI2_CLK_ENABLE();
		__GPIOA_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();

		GPIO_InitTypeDef pinInit;
		pinInit.Mode = GPIO_MODE_AF_PP;
		pinInit.Alternate = GPIO_AF5_SPI2;
		pinInit.Pull = GPIO_NOPULL;
		pinInit.Speed = GPIO_SPEED_LOW;

		// PA12 -> SCK
		pinInit.Pin = GPIO_PIN_12;
		HAL_GPIO_Init(GPIOA, &pinInit);

		// PB14 -> MISO
		pinInit.Pin = GPIO_PIN_14;
		HAL_GPIO_Init(GPIOB, &pinInit);

		// PB15 -> MOSI
		pinInit.Pin = GPIO_PIN_15;
		HAL_GPIO_Init(GPIOB, &pinInit);

		// PA11 -> CS (not using AF mode)
		pinInit.Mode = GPIO_MODE_OUTPUT_PP;
		pinInit.Pin = SPI_CS_PIN;
		HAL_GPIO_Init(SPI_CS_PORT, &pinInit);

		HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET);
	}
}
