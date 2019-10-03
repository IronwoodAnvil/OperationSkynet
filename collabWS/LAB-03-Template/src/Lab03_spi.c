//----------------------------------
// Lab 3 - Part 3: SPI - Lab03_spi.c
//----------------------------------
//

#include "init.h"
#include "STaTS.h"
#include "uart_fs.h"

// If needed:
//#include <stdio.h>
//#include <stdlib.h>



int main(void)
{
	Sys_Init();

	UART_InitTypeDef uart_conf = UART_CONF_DEFAULT;
	uart_conf.BaudRate = 115200;
	uart_stdiod(uart_fdopen(USART1, &uart_conf)); // Configure USB UART for stdout/in

	SPI_HandleTypeDef SPI_H;
	SPI_Init(&SPI_H);

#if LAB_TASK == 3

#elif LAB_TASK == 4
	STATS_useSPI(&SPI_H, SPI_CS_PORT, SPI_CS_PIN);

#endif

// See 769 Description of HAL drivers.pdf, Ch. 58.2.3 or stm32f7xx_hal_spi.c
//
//	HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
//
}
