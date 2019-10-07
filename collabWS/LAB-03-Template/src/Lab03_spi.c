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
	uint32_t tempC;
	uint8_t id;
	char a;
	STATS_useSPI(&SPI_H, SPI_CS_PORT, SPI_CS_PIN);
	buildno_t temp= STATS_buildno();
	printf("Device version: %d.%d\r\n", temp.major, temp.minor);


	bool menu = false;
	while(1){

		if(menu){
			printf("Menu\r\n\nt) get temperature reading\r\n\ni) set id to 0xff\r\n\nr) reset terminal\r\n");
			a =getchar();
			if (a == 't'){
				STATS_ctrl(STATS_CTL_RDTMP);
				while (!(STATS_status() & STATS_STS_RDY)){
				}
				tempC = STATS_getTempC();
				printf("Temperature: %lu\r\n", tempC);
			}
			else if(a == 'i'){
				char b = STATS_devID();
				id = 0xA1;
				STATS_newDevID(id);
				HAL_Delay(1);
				char c = STATS_devID();

				printf("Old Device ID was: %02X\r\n", b);
				printf("New Device ID is: %02X\r\n\n",c);
			}
			else if(a == 'r'){
				STATS_ctrl(STATS_CTL_TRMRST);
			}
			else{
			}
			fflush(stdout);
			menu = false;
		} else {
			if(fpoll(stdin, &a))
			{
				if(a == 0x1B) menu = true;
				else {
					STATS_putchar(a);
				}
			}
//			if(STATS_poll(&a))
//			{
//				putchar(a);
//				fflush(stdout);
//			}
			HAL_Delay(1);
		}
	}
#endif
}


