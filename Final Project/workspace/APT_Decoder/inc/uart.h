#ifndef UART_H_
#define UART_H_

#include "stm32f769xx.h"
#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


void initUart(UART_HandleTypeDef* Uhand, uint32_t Baud, USART_TypeDef* Tgt);
void puts_dma(const char* string);
void printf_dma(const char* fmt, ...);

//Global Handles
UART_HandleTypeDef USB_UART;

#endif /* UART_H_ */
