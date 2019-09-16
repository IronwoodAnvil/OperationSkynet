#ifndef UART_H_
#define UART_H_

#include "stm32f769xx.h"
#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


void initUart(UART_HandleTypeDef* Uhand, uint32_t Baud, USART_TypeDef* Tgt);
void uart_print(UART_HandleTypeDef *huart, char* string);

int uart_getline(UART_HandleTypeDef *huart, char* input, int max_size);
char uart_getchar(UART_HandleTypeDef *huart, uint8_t echo);
// Attempts to get a character if available, assigning it to val
// Returns true if character was received, false otherwise
_Bool uart_getchar_nw(UART_HandleTypeDef *huart, uint8_t* val);

void uart_putchar(UART_HandleTypeDef *huart, char * input);
//void Error_Handler(UART_HandleTypeDef *UartHandle);

//Global Handles
UART_HandleTypeDef USB_UART;

#endif /* UART_H_ */
