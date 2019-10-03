/*
 * uart_fs_conf_temp.h
 *
 *  Created on: Sep 30, 2019
 *      Author: lovep
 */

#include <task.h>

#define UART_FS_ENABLED 1


#define UART_FS_INTERRUPT_MODE_ENABLED 1
#if LAB_TASK == 2
#define UART_FS_TX_USE_INTERRUPT 1 //blocking
#else
#define UART_FS_TX_USE_INTERRUPT 0 //non-blocking
#endif
