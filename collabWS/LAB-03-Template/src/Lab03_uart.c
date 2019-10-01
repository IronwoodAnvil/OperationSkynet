//------------------------------------
// Lab 3 - Part 1: UART - Lab03_uart.c
//------------------------------------
//


#include <task.h>
#include "init.h"

#define ESCAPE_CHAR 0x1B
#define SHUTDOWN_MESSAGE "\r\n\"To exist in an echo chamber and only talk to people with whom we agree is fruitless.\"\r\n\n"

FILE* USB_UART;
FILE* WIRE_UART;

void puts_both(char* str)
{
	fputs(str,USB_UART);
	fputs(str,WIRE_UART);
	fflush(USB_UART);
	fflush(WIRE_UART);
}


#if LAB_TASK == 2

char print_str[] = " \r\n";
char wire_receive;
char usb_receive;
bool done = false;

void rxWire()
{
	if(!done)
	{
		if(wire_receive!=ESCAPE_CHAR){
			print_str[0] = wire_receive;
			puts_both(print_str);
			fgetc_async(WIRE_UART, &wire_receive);
		} else {
			done = true;
			puts_both(SHUTDOWN_MESSAGE);
		}
	}
}

void rxUsb()
{
	if(!done)
	{
		if(usb_receive!=ESCAPE_CHAR){
			print_str[0] = usb_receive;
			puts_both(print_str);
			fgetc_async(USB_UART, &usb_receive);
		} else {
			done = true;
			puts_both(SHUTDOWN_MESSAGE);
		}
	}
}

#endif

// Main Execution Loop
int main(void) {
	//Initialize the system
	Sys_Init();

	/* UART configured as follows:
		- Word Length = 8 Bits
		- Stop Bit = No Stop bits
		- Parity = None
		- Hardware flow control disabled (RTS and CTS signals)
	*/
	UART_InitTypeDef settings = UART_CONF_DEFAULT;
	WIRE_UART = uart_fopen(USART6, &settings);
	settings.BaudRate = 115200;
	USB_UART = uart_fopen(USART1, &settings);

#if LAB_TASK==1

	char print_str[] = " \r\n";
	while(1)
	{
		if(fpoll(USB_UART, print_str))
		{
			if(print_str[0]==ESCAPE_CHAR)
			{
				break;
			}
			puts_both(print_str);
		}
		if(fpoll(WIRE_UART, print_str))
		{
			if(print_str[0]==ESCAPE_CHAR)
			{
				break;
			}
			puts_both(print_str);
		}
	}

	puts_both(SHUTDOWN_MESSAGE);

	while(1);

#elif LAB_TASK==2

	uart_fDataCallback(USB_UART, rxUsb);
	uart_fDataCallback(WIRE_UART, rxWire);

	fgetc_async(USB_UART, &usb_receive);
	fgetc_async(WIRE_UART, &wire_receive);

	while(1);

#endif
}
