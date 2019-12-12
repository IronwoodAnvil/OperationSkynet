#include "init.h"
#include "sampling.h"


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
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
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

//  /* initialize LTDC LCD clock */
//   PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
//   PeriphClkInitStruct.PLLSAI.PLLSAIN = 60;
//   PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
//   PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
//   HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
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

//void LCD_Config(void)
//{
//	/* Program the required configuration through the following parameters:
//	  * the LTDC timing, the horizontal and vertical polarity,
//	  *  the pixel clock polarity, Data Enable polarity
//	  *  and the LTDC background color value*/
//	LTDC_HandleTypeDef hltdc_F;
//	HAL_LTDC_MspInit(&hltdc_F);
//	LTDC_LayerCfgTypeDef pLayerCfg;
//
//	 /* Polarity configuration */
//	   /* Initialize the horizontal synchronization polarity as active low */
//	   hltdc_F.Init.HSPolarity = LTDC_HSPOLARITY_AL;
//	   /* Initialize the vertical synchronization polarity as active low */
//	   hltdc_F.Init.VSPolarity = LTDC_VSPOLARITY_AL;
//	   /* Initialize the data enable polarity as active low */
//	   hltdc_F.Init.DEPolarity = LTDC_DEPOLARITY_AL;
//	   /* Initialize the pixel clock polarity as input pixel clock */
//	   hltdc_F.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
//
//
//   /* Timing configuration */
//	/* Horizontal synchronization width = Hsync - 1 */
//	hltdc_F.Init.HorizontalSync = HSYNC - 1;
//	/* Vertical synchronization height = Vsync - 1 */
//	hltdc_F.Init.VerticalSync = VSYNC - 1;
//	/* Accumulated horizontal back porch = Hsync + HBP - 1 */
//	hltdc_F.Init.AccumulatedHBP = HSYNC + HBP - 1;
//	/* Accumulated vertical back porch = Vsync + VBP - 1 */
//	hltdc_F.Init.AccumulatedVBP = VSYNC + VBP - 1;
//	/* Accumulated active width = Hsync + HBP + Active Width - 1 */
//	hltdc_F.Init.AccumulatedActiveH = ACTIVE_W;
//	/* Accumulated active height = Vsync + VBP + Active Heigh - 1 */
//	hltdc_F.Init.AccumulatedActiveW = ACTIVE_H ;
//	/* Total height = Vsync + VBP + Active Heigh + VFP - 1 */
//	hltdc_F.Init.TotalHeigh = TOTAL_HEIGHT;
//	/* Total width = Hsync + HBP + Active Width + HFP - 1 */
//	hltdc_F.Init.TotalWidth = TOTAL_WIDTH;
//
//	/* Configure R,G,B component values for LCD background color */
//	hltdc_F.Init.Backcolor.Blue = 10;
//	hltdc_F.Init.Backcolor.Green = 10;
//	hltdc_F.Init.Backcolor.Red = 20;
//
//	hltdc_F.Instance = LTDC;
//	 /*
//		    * Program the required configuration through the following parameters: the pixel format,
//		    * the blending factors, input alpha value, the window size and the image size using
//		    * HAL_LTDC_ConfigLayer() function for foreground or/and background layer
//		    */
//
//	/* Layer1 Configuration ------------------------------------------------------*/
//
//	  /* Windowing configuration */
//	  /* In this case all the active display area is used to display a picture then :
//	     Horizontal start = horizontal synchronization + Horizontal back porch = 43
//	     Vertical start   = vertical synchronization + vertical back porch     = 12
//	     Horizontal stop = Horizontal start + window width -1 = 43 + 480 -1
//	     Vertical stop   = Vertical start + window height -1  = 12 + 272 -1      */
//	  pLayerCfg.WindowX0 = 0;
//	  pLayerCfg.WindowX1 = 480;
//	  pLayerCfg.WindowY0 = 0;
//	  pLayerCfg.WindowY1 = 272;
//
//	  /* Pixel Format configuration*/
//	  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_L8;
//
//	  /* Alpha constant (255 totally opaque) */
//	  pLayerCfg.Alpha = 255;
//
//	  /* Default Color configuration (configure A,R,G,B component values) */
//	  pLayerCfg.Alpha0 = 0;
//	  pLayerCfg.Backcolor.Blue = 0;
//	  pLayerCfg.Backcolor.Green = 0;
//	  pLayerCfg.Backcolor.Red = 0;
//
//	  /* Configure blending factors */
//	  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
//	  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
//
//	  /* Configure the number of lines and number of pixels per line */
//	  pLayerCfg.ImageWidth = LCD_WIDTH;
//	  pLayerCfg.ImageHeight = LCD_HEIGHT;
//
//	  /* Configure the LTDC */
//	  if(HAL_LTDC_Init(&hltdc_F) != HAL_OK)
//	  {
//		  while(1){}
//	  }
//
//	  /* Configure the Layer */
//	  if(HAL_LTDC_ConfigLayer(&hltdc_F, &pLayerCfg, 1) != HAL_OK)
//	  {
//	    while(1){}
//	  }
//
//}
//
//void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc){
//
//	 /* Enable the LTDC clock */
//	  __HAL_RCC_LTDC_CLK_ENABLE();
//
//}
