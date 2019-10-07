/*
 * STaTS.c
 *
 *  Created on: Oct 3, 2019
 *      Author: lovep
 */

#include "STaTS.h"


struct {
	SPI_HandleTypeDef* handle;
	GPIO_TypeDef* cs_port;
	uint16_t cs_pin;
} spi_conf;


static void delay_us(uint32_t microseconds)
{
	uint32_t delay_cnt = 216*microseconds;
	while(--delay_cnt) asm("");
}

static uint8_t STaTS_readwrite(uint8_t reg, uint8_t write)
{
	HAL_GPIO_WritePin(spi_conf.cs_port, spi_conf.cs_pin, GPIO_PIN_RESET); // CS Low

	HAL_SPI_Transmit(spi_conf.handle, &reg, 1, 1);

	delay_us(10);

	uint8_t result;
	HAL_SPI_TransmitReceive(spi_conf.handle, &write, &result, 1, 1);

	HAL_GPIO_WritePin(spi_conf.cs_port, spi_conf.cs_pin, GPIO_PIN_SET); // CS High
	delay_us(10);

	return result;
}


// PUBLIC FUNCTIONS


void STATS_useSPI(SPI_HandleTypeDef* h, GPIO_TypeDef* port, uint16_t pin)
{
	spi_conf.handle = h;
	spi_conf.cs_port = port;
	spi_conf.cs_pin = pin;
}


uint8_t STATS_status()
{
	return STaTS_readwrite(3, 0);
}
void STATS_ctrl(uint8_t word)
{
	STaTS_readwrite(2, word);
}

uint32_t STATS_getTempC()
{
	uint8_t low = STaTS_readwrite(5, 0);
	uint8_t high = STaTS_readwrite(6, 0);
	uint32_t result = (((uint16_t)high) << 8) | low;
	result &= 0xFFF;

	return result;
}
void STATS_putchar(char c)
{
	STaTS_readwrite(7, c);
}
void STATS_setattr(uint8_t attr)
{
	STaTS_readwrite(8, attr);
}
char STATS_getchar()
{
	while(!STATS_available())
	{
		HAL_Delay(1);
	}
	return STaTS_readwrite(7, 0);
}
bool STATS_poll(char* c)
{
	if(STATS_available())
	{
		*c = STaTS_readwrite(7, 0);
		return true;
	}
	else{
		return false;
	}
}
uint8_t STATS_available()
{
	return (STATS_status()&STATS_STS_NHCBF) >> 5;
}
buildno_t STATS_buildno()
{
	buildno_t result;
	result.major = STaTS_readwrite(0, 0);
	result.minor = STaTS_readwrite(1, 0);
	return result;
}
uint8_t STATS_devID()
{
	return STaTS_readwrite(9, 0xEE);
}
void STATS_newDevID(uint8_t id)
{
	STATS_ctrl(STATS_CTL_ULKDID);
	STaTS_readwrite(9, id);
}
