//------------------------------------
// Lab 3 - Part 1: UART - Lab03_uart.c
//------------------------------------
//


#include "conf.h"
#include "init.h"

#define SHUTDOWN_MESSAGE "It's tiring living in an echo chamber ain't it?"

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

char* print_str = " \r\n";
char wire_receive;
char usb_receive;
bool done = false;

void rxWire()
{
	if(!done)
	{
		if(wire_receive!=0x1B){
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
		if(usb_receive!=0x1B){
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
		- BaudRate = 115200 baud
		- Hardware flow control disabled (RTS and CTS signals)
	*/
	UART_InitTypeDef settings = UART_CONF_DEFAULT;
	WIRE_UART = uart_fopen(USART6, &settings);
	settings.BaudRate = 115200;
	USB_UART = uart_fopen(USART1, &settings);
	uart_stdiof(USB_UART);

#if LAB_TASK == 0

	uart_stdiof(WIRE_UART);
	fprintf(USB_UART, "USB %d\r\n", 1);
	fflush(USB_UART);

	fprintf(WIRE_UART, "Wire %d\r\n", 1);
	fflush(WIRE_UART);

	printf("WIRE %d\r\n",2);
	fflush(stdout);

	uart_stdiof(USB_UART);

	printf("USB %d\r\n",2);
	fflush(stdout);

	while(1)
	{
		char c;
		while(!fpoll(USB_UART, &c)){
			HAL_Delay(500);
			fputs("No input.\r\n",USB_UART);
			fflush(USB_UART);
		}
		if(c==0x1B) break;
		fprintf(USB_UART,"Typed '%c'\r\n",c);
		fflush(USB_UART);
	}

	fputs("Done!\r\n\r\n",stdout);

	while(1);

#elif LAB_TASK==1

	char* print_str = " \r\n";
	while(1)
	{
		if(fpoll(USB_UART, print_str))
		{
			if(print_str[0]==0x1B) break;
			puts_both(print_str);
		}
		if(fpoll(WIRE_UART, print_str))
		{
			if(print_str[0]==0x1B) break;
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
