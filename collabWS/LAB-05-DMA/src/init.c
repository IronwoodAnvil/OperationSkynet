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
}

// This function is what makes everything work
// Don't touch it...
// (Increments the system clock)
void SysTick_Handler(void) {
  HAL_IncTick();
}



/*
 * For convenience, configure the SPI handler here
 */
// See 769 Description of HAL drivers.pdf, Ch. 58.1 or stm32f7xx_hal_spi.c
void SPI_Init(SPI_HandleTypeDef* handle, DMA_HandleTypeDef* hdma_rx, DMA_HandleTypeDef* hdma_tx)
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
	handle->Init.NSS = SPI_NSS_HARD_OUTPUT;

	__HAL_RCC_DMA1_CLK_ENABLE();

	// Common Configuration
	hdma_tx->Init.Mode = DMA_NORMAL;
	hdma_tx->Init.Channel = DMA_CHANNEL_0;
	hdma_tx->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	hdma_tx->Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_tx->Init.MemInc = DMA_MINC_ENABLE;
	hdma_tx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_tx->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;

	memcpy(hdma_rx,hdma_tx,sizeof(DMA_HandleTypeDef)); // Copy common config

	hdma_tx->Instance = DMA1_Stream4;
	hdma_tx->Init.Direction = DMA_MEMORY_TO_PERIPH;
	HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	__HAL_LINKDMA(handle,hdmatx,*hdma_tx);

	hdma_rx->Instance = DMA1_Stream3;
	hdma_rx->Init.Direction = DMA_PERIPH_TO_MEMORY;
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	__HAL_LINKDMA(handle,hdmarx,*hdma_rx);

	HAL_DMA_Init(hdma_tx);
	HAL_DMA_Init(hdma_rx);
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

		// PA12 -> D13 -> SCK
		pinInit.Pin = GPIO_PIN_12;
		HAL_GPIO_Init(GPIOA, &pinInit);

		// PB14 -> D12 -> MISO
		pinInit.Pin = GPIO_PIN_14;
		HAL_GPIO_Init(GPIOB, &pinInit);

		// PB15 -> D11 -> MOSI
		pinInit.Pin = GPIO_PIN_15;
		HAL_GPIO_Init(GPIOB, &pinInit);

		// No need to configure CS for loopback

        //HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET);
	}
}
