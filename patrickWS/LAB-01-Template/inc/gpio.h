/*
 * gpio.h
 *
 *  Created on: Sep 3, 2019
 *      Author: lovep
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#include "stm32f769xx.h"

// Configures the GPIO pins for this lab
void GPIO_Init();

// Macros to set the LEDs
#define LD1Off() (GPIOJ->BSRR = GPIO_BSRR_BR_13)
#define LD2Off() (GPIOJ->BSRR = GPIO_BSRR_BR_5)
#define LD3Off() (GPIOA->BSRR = GPIO_BSRR_BR_12)
#define LD4Off() (GPIOD->BSRR = GPIO_BSRR_BS_4)
#define LD1On() (GPIOJ->BSRR = GPIO_BSRR_BS_13)
#define LD2On() (GPIOJ->BSRR = GPIO_BSRR_BS_5)
#define LD3On() (GPIOA->BSRR = GPIO_BSRR_BS_12)
#define LD4On() (GPIOD->BSRR = GPIO_BSRR_BR_4)
#define LD1Set(v) if(v) LD1On(); else LD1Off()
#define LD2Set(v) if(v) LD2On(); else LD2Off()
#define LD3Set(v) if(v) LD3On(); else LD3Off()
#define LD4Set(v) if(v) LD4On(); else LD4Off()

// Macros to read the switches
#define SW1Val (GPIOC->IDR & GPIO_IDR_IDR_7)
#define SW2Val (GPIOC->IDR & GPIO_IDR_IDR_6)
#define SW3Val (GPIOJ->IDR & GPIO_IDR_IDR_1)
#define SW4Val (GPIOF->IDR & GPIO_IDR_IDR_6)

#endif /* INC_GPIO_H_ */
