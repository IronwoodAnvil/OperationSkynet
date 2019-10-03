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

SPI_HandleTypeDef SPI_H;


int main(void)
{
	Sys_Init();

	UART_InitTypeDef uart_conf = UART_CONF_DEFAULT;
	uart_conf.BaudRate = 115200;
	uart_stdiod(uart_fdopen(USART1, &uart_conf)); // Configure USB UART for stdout/in

	SPI_Init(&SPI_H);

#if LAB_TASK == 3
	{
	char a;
	uint8_t write;
	uint8_t result;


	printf("\033[;H\033[33;44m\033[=3h");//sets background color, text color, size
	printf("\r\n");
	printf("\033[2J"); //clears the terminal, making the whole background blue
	fflush(stdout);


	printf("\033[0;12r\033[12;24r\033[s\r\n");//sets scroll areas\moves cursor\saves cursor pos


	while(1)
	{
		a = getchar(); // if begin
		write = ' ';
		write = a;
		printf("\033[u\033[0;0HChar sent:%c\r\n\033[s", write);
		HAL_SPI_TransmitReceive(&SPI_H, &write, &result, 1, 1);
		printf("\033[u\033[12BChar read:%c", result);
		fflush(stdout);
	}
	}


#elif LAB_TASK == 4
	STATS_useSPI(&SPI_H, SPI_CS_PORT, SPI_CS_PIN);

#endif

// See 769 Description of HAL drivers.pdf, Ch. 58.2.3 or stm32f7xx_hal_spi.c
//
//	HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
//
}
