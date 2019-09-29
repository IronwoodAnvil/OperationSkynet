//------------------------------------
// Lab 3 - Part 1: UART - Lab03_uart.c
//------------------------------------
//

#include "init.h"

FILE* USB_UART;
FILE* WIRE_UART;

// Main Execution Loop
int main(void) {
	//Initialize the system
	Sys_Init();

	/* UART configured as follows:
		- Word Length = 8 Bits
		- Stop Bit = No Stop bits
		- Parity = None
		- BaudRate = 115200 baud
		- Hardware flow control disabled (RTS and CTS signals)
	*/
	USB_UART = openUart(USART1, 115200);
	WIRE_UART = openUart(USART6, 9600);
	stdIOUart(WIRE_UART);

	fprintf(USB_UART, "USB %d\r\n", 1);
	fflush(USB_UART);

	fprintf(WIRE_UART, "Wire %d\r\n", 1);
	fflush(WIRE_UART);

	printf("WIRE %d\r\n",2);
	fflush(stdout);

	stdIOUart(USB_UART);

	printf("USB %d\r\n",2);
	fflush(stdout);

	while(1)
	{
		char c = fgetc(USB_UART);
		if(c==0x1B) break;
		fprintf(USB_UART,"Typed '%c'\r\n",c);
		fflush(USB_UART);
	}

	fputs("Done!\r\n\r\n",stdout);

	while(1);
}
