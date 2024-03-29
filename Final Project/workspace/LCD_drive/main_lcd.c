//=================================================================================================
// STM32F429 TFT LCD and SDRAM example
// Author : Radoslaw Kwiecien
// e-mail : radek@dxp.pl
// http://en.radzio.dxp.pl/stm32f429idiscovery/
//=================================================================================================
#include "stm32f4xx.h"
//=================================================================================================
//
//=================================================================================================
void SDRAM_Init(void);
void TFTLCD_Init(void);
//=================================================================================================
// main function
//=================================================================================================
int main(void)
{
	uint32_t i;

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN |
					RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOFEN |
					RCC_AHB1ENR_GPIOGEN;

	SDRAM_Init();
	TFTLCD_Init();

	for(i = 0xD0000000; i < 0xD0000000 + 0x50000; i += 2)
		*(uint16_t *)i = 0x0000;

	while(1);
}
//=================================================================================================
// End of file
//=================================================================================================

