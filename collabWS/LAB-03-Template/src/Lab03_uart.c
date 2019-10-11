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
	// This is the magic of the uart_fs system
	// Just like the usual setup for using stdio functions
	// this eventually ends up with a call to _write
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

// Registered callback for characters read on the wire uart
void rxWire()
{
	if(!done)
	{
		if(wire_receive!=ESCAPE_CHAR){
			print_str[0] = wire_receive;
			puts_both(print_str);
			// Starts an interrupt based read
			// See read_async
			fgetc_async(WIRE_UART, &wire_receive);
		} else {
			done = true;
			puts_both(SHUTDOWN_MESSAGE);
		}
	}
}

// Registered callback for characters read on the usb uart
void rxUsb()
{
	if(!done)
	{
		if(usb_receive!=ESCAPE_CHAR){
			print_str[0] = usb_receive;
			puts_both(print_str);
			// Starts an interrupt based read
			// See read_async
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
	// The super fun setup function
	// The majority of the underlying implementation is actually in uart_fdopen where the file descriptor is originally created
	WIRE_UART = uart_fopen(USART6, &settings);
	settings.BaudRate = 115200;
	USB_UART = uart_fopen(USART1, &settings);

#if LAB_TASK==1

	char print_str[] = " \r\n"; // The received characters are read directly into character 0 of this string
	while(1)
	{
		// fpoll is a custom function that provides polling reads from a "UART File"
		// It actually avoid the usual _read which is still blocking, and is implemented directly
		// in dpoll (the raw file descriptor version of fpoll)
		if(fpoll(USB_UART, print_str)) // Look for char on USB UART
		{
			if(print_str[0]==ESCAPE_CHAR)
			{
				break;
			}
			puts_both(print_str);
		}
		if(fpoll(WIRE_UART, print_str)) // Look for char on wired UART
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

	// Register callbacks for asynchrounous reads
	uart_fDataCallback(USB_UART, rxUsb);
	uart_fDataCallback(WIRE_UART, rxWire);

	// Start first reads
	fgetc_async(USB_UART, &usb_receive);
	fgetc_async(WIRE_UART, &wire_receive);

	while(1);

#endif
}
