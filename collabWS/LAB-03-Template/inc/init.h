#ifndef INIT_H_
#define INIT_H_

#include "stm32f769xx.h"
#include "stm32f7xx_hal.h"
#include "uart_fs.h"

#define SPI_CS_PORT GPIOA
#define SPI_CS_PIN GPIO_PIN_11

//Function Prototypes
void SystemClock_Config(void);
void CPU_CACHE_Enable(void);
void Sys_Init(void);

#endif
