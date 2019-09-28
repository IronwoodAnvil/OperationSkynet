//------------------------------------
// Lab 3 - Part 1: UART - Lab03_uart.c
//------------------------------------
//

#include "init.h"

// Main Execution Loop
int main(void) {
	//Initialize the system
	Sys_Init();

	printf("Your code here!\r\n");
	fflush(stdout);

	dprintf(WIRE_OUT_FD, "Test %d\r\n", 1);
	fprintf(WIRE_FOUT, "Test %d", 2);
	HAL_Delay(1000);
	fflush(WIRE_FOUT);

	while(1);
}
