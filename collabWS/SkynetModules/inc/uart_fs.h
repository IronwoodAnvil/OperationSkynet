/*
 * uart_fs.h
 *
 *  Created on: Sep 30, 2019
 *      Author: lovep
 */

#ifndef SKYNETMODULES_INC_UART_FS_H_
#define SKYNETMODULES_INC_UART_FS_H_

#include "stm32f769xx.h"
#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "uart_fs_conf.h"


#if UART_FS_ENABLED != 0

extern const UART_InitTypeDef UART_CONF_DEFAULT;

int uart_fdopen(USART_TypeDef* reg, UART_InitTypeDef* config);
FILE* uart_fopen(USART_TypeDef* reg, UART_InitTypeDef* config);

void uart_stdiod(int fd);
void uart_stdiof(FILE* file);

_Bool dpoll(int fd, char* c);
_Bool fpoll(FILE* uart, char* c);

#if UART_FS_INTERRUPT_MODE_ENABLED

typedef void (*dr_callback_t)(void);

void read_async(int fd, char* val);
void fgetc_async(FILE* file, char* val);

void uart_fdDataCallback(int fd, dr_callback_t cb);
void uart_fDataCallback(FILE* file, dr_callback_t cb);

#endif


#endif


#endif /* SKYNETMODULES_INC_UART_FS_H_ */
