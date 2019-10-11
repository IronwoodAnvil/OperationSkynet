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

// Creates a file descriptor for the UART, configures the peripheral with the given init struct
// The MspInit can be  used for hardware specific initialization
int uart_fdopen(USART_TypeDef* reg, UART_InitTypeDef* config);
FILE* uart_fopen(USART_TypeDef* reg, UART_InitTypeDef* config);

// Sets a uart file to be used for stdio (stdin/stdout)
void uart_stdiod(int fd);
void uart_stdiof(FILE* file);

// Polls from a file, returning true if a character was obtained and written into c, false otherwise
_Bool dpoll(int fd, char* c);
_Bool fpoll(FILE* uart, char* c);

#if UART_FS_INTERRUPT_MODE_ENABLED

// Type for async callback functions
typedef void (*dr_callback_t)(void);

// Functions to register callback functions for async reads
void uart_fdDataCallback(int fd, dr_callback_t cb);
void uart_fDataCallback(FILE* file, dr_callback_t cb);

// Starts an asynchronous read from the file, which will eventually call
// the registered function.  The val pointer should be in scope in both
// functions
void read_async(int fd, char* val);
void fgetc_async(FILE* file, char* val);

#endif


#endif


#endif /* SKYNETMODULES_INC_UART_FS_H_ */
