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


/**********************************************
 *
 *   Lab-specific initialization functions
 *
 **********************************************/

// Configure PJ0 (D4) as an interrupt via EXTI0
// EXTI0 Must be a pin 0 (from ports A-K)
void EXTI0_HAL_Init()
{
	__HAL_RCC_GPIOJ_CLK_ENABLE();

	GPIO_InitTypeDef init;
	init.Pin = GPIO_PIN_0;
	init.Pull = GPIO_PULLDOWN;
	init.Mode = GPIO_MODE_IT_RISING_FALLING; //magic
	init.Speed = GPIO_SPEED_LOW;
	init.Alternate = 0;

	HAL_GPIO_Init(GPIOJ, &init);

	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

// Configure PB8 (D15) as an interrupt via EXTI8
// EXTI8 Must be a pin 8 (from ports A-K)
void EXTI8_Reg_Init()/***********/
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //clock enable
	asm ("nop");
	asm ("nop");

	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR8_1;  // set to pull down
	SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PB; // in [2] b/c 0-3 4-7 [8-11] 12-15


	EXTI->IMR |= EXTI_IMR_IM8; //mask for pin 8
	EXTI->RTSR |= EXTI_RTSR_TR8; //select rising edge
	// EXTI 9-5 has position 23, so lives in iser 0.  bit in iser 0 is 23
	NVIC->ISER[EXTI9_5_IRQn/32] |= (uint32_t) 1 << (EXTI9_5_IRQn % 32);

}

void TIM6_Reg_Init()
{
	// Enable timer RCC clock and wait for it to start
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	asm("nop");
	asm("nop");

	TIM6->DIER |= TIM_DIER_UIE; //Enable update interrupt
	TIM6->PSC = 540-1;
	TIM6->ARR = 20000-1;
	// 20000*(539+1) = 10800000 -> 0.1s period

	NVIC->ISER[TIM6_DAC_IRQn/32] |= (1 << (TIM6_DAC_IRQn%32));
}

void TIM7_HAL_Init(TIM_HandleTypeDef* handle)
{
	__HAL_RCC_TIM7_CLK_ENABLE();

	TIM_Base_InitTypeDef init;
	init.Period = 40000-1;
	init.Prescaler = 270-1;
	// Performs Prescaler+1 counts, period times, that is 270*40000 = 10800000
	// Time is counts / (counts/s) = 10800000/108000000 = 0.1s
	init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	init.CounterMode = TIM_COUNTERMODE_UP;
	init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	handle->Instance = TIM7;
	handle->Init = init;

	HAL_TIM_Base_Init(handle);

	HAL_NVIC_EnableIRQ(TIM7_IRQn);
}
