#ifndef INIT_H_
#define INIT_H_

#include "stm32f769xx.h"
#include "stm32f7xx_hal.h"
#include "uart.h"

//Function Prototypes
void SystemClock_Config(void);
void CPU_CACHE_Enable(void);
void Sys_Init(void);
void SPI_Init(SPI_HandleTypeDef* handle, DMA_HandleTypeDef* hdma_rx, DMA_HandleTypeDef* hdma_tx);
void ADC_Init(ADC_TypeDef* instance, ADC_HandleTypeDef* handle);
void DAC1_Init(DAC_HandleTypeDef* hdac);
void ADC_DMA_Init(DMA_HandleTypeDef* hdma);
void DAC_DMA_Init(DMA_HandleTypeDef* hdma);
void SamplingTimerInit(TIM_HandleTypeDef* handle);

#endif
