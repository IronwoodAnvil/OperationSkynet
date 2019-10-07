/*
 * STaTS.h
 *
 *  Created on: Oct 3, 2019
 *      Author: lovep
 */

#ifndef STATS_H_
#define STATS_H_

#include "stm32f7xx_hal.h"
#include <stdint.h>
#include "stdbool.h"

#define STATS_STS_RDY 0x01
#define STATS_STS_FLT 0x02
#define STATS_STS_TBUSY 0x04
#define STATS_STS_TRDY 0x08
#define STATS_STS_TOVRN 0x10
#define STATS_STS_NHCBF 0x60
#define STATS_STS_CHBOV 0x80

#define STATS_CTL_RST 0x01
#define STATS_CTL_RDTMP 0x02
#define STATS_CTL_TRMCLR 0x04
#define STATS_CTL_TRMRST 0x08
#define STATS_CTL_CHBCLR 0x10
#define STATS_CTL_ULKDID 0x80

typedef struct {
	uint8_t major;
	uint8_t minor;
} buildno_t;


void STATS_useSPI(SPI_HandleTypeDef* h, GPIO_TypeDef* port, uint16_t pin);

uint8_t STATS_status();
void STATS_ctrl(uint8_t word);

uint32_t STATS_getTempC();
void STATS_putchar(char c);
void STATS_setattr(uint8_t attr);
char STATS_getchar();
bool STATS_poll(char* c);
uint8_t STATS_available();

buildno_t STATS_buildno();
uint8_t STATS_devID();
void STATS_newDevID(uint8_t id);

#endif /* STATS_H_ */
