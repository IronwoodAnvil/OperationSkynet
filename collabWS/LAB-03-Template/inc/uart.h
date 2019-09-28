#ifndef UART_H_
#define UART_H_

#include "stm32f769xx.h"
#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define USB_OUT_FD 1
#define USB_IN_FD 0
#define WIRE_OUT_FD 4
#define WIRE_IN_FD 3

void initUart(UART_HandleTypeDef* Uhand, uint32_t Baud, USART_TypeDef* Tgt);
void initUartFileIO();
void uart_print(UART_HandleTypeDef *huart, char* string);

int uart_getline(UART_HandleTypeDef *huart, char* input, int max_size);
char uart_getchar(UART_HandleTypeDef *huart, uint8_t echo);

void uart_putchar(UART_HandleTypeDef *huart, char * input);
//void Error_Handler(UART_HandleTypeDef *UartHandle);

//Global Handles
extern UART_HandleTypeDef USB_UART;
extern UART_HandleTypeDef WIRE_UART;
extern FILE* USB_FOUT;
extern FILE* USB_FIN;
extern FILE* WIRE_FOUT;
extern FILE* WIRE_FIN;

#endif /* UART_H_ */
