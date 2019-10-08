/*
 * main.c
 *
 *  Created on: Oct 7, 2019
 *      Author: lovep
 *      		mascan
 */
#include "init.h"
#include <stdio.h>
#include <stdbool.h>
#include "Temperature_Sensor.h"


#define REG_COUNT 10
#define TRANSACT_TIMEOUT 1


uint8_t NO_RESPONSE() { return 0x00; }
void IGNORE_RESULT(uint8_t x) {}

const static uint8_t(*response_table[REG_COUNT])(void);
const static void(*result_table[REG_COUNT])(uint8_t);

static SPI_HandleTypeDef SPI;
uint8_t sts_reg = 0x00;
uint8_t chr_buf = 0x00;
uint8_t tmp_lo = 0;
uint8_t tmp_hi = 0;


bool read_cs()
{
	return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11);
}

void tempTask(){
	static int sum = 0;
	static int samples = 0;
	if(sts_reg & (1<<2))
	{
		int result = getTemperature();
		if (result == 0){
			sts_reg |= 1<<1;
			sts_reg |= 1<<3;
			return;
		}
		sum += result;

		++samples;

		if(samples == 30)
		{
			uint16_t temp = sum / 30;

			tmp_lo = temp & 0xFF;
			tmp_hi = temp >> 8;

			sts_reg &= ~(1<<2);
			sts_reg |= 1<<3;

			sum = samples = 0;
		}
	}
}

int main()
{
	Sys_Init();

	SPI_Init(&SPI);
	initTempSensor();

//	printf("It Works!\r\n");
	//	__HAL_RCC_SYSCFG_CLK_ENABLE();
	//	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	//	SYSCFG->EXTICR[11/4] |= SYSCFG_EXTICR3_EXTI11_PA;
	//	EXTI->IMR |= EXTI_IMR_IM11;
	//	// Rising and falling
	//	EXTI->RTSR |= EXTI_RTSR_TR11;
	//	EXTI->FTSR |= EXTI_FTSR_TR11;

	while(1)
	{

		while(read_cs())
		{
			// Wait for request and poll uart
			if(fpoll(stdin, (char*)&chr_buf))
			{
				sts_reg |= (1<<5);
			}
			tempTask();
		}

		uint8_t reg;
		HAL_StatusTypeDef status = HAL_SPI_Receive(&SPI, &reg, 1, TRANSACT_TIMEOUT);

//		printf("R%d ", reg);

		uint8_t response = 0x55; // Compute response (should have 10 us)
		if(status==HAL_OK && reg<REG_COUNT)
		{
			response = response_table[reg]();
		} else {
//			printf("E");
		}

		uint8_t incoming;
		HAL_SPI_TransmitReceive(&SPI, &response, &incoming, 1, TRANSACT_TIMEOUT);

//		printf("S%02X R%02X ", response, incoming);

		if(status==HAL_OK && reg<REG_COUNT)
		{
			result_table[reg](incoming);
		} else {
//			printf("E");
		}

//		printf("\r\n");
		while(!read_cs()); // Wait for end of transaction
	}
}


uint8_t V_MAG(){
		return 0x4;
}

uint8_t V_MIN(){
		return 0x4;
}

void CTL_REG(uint8_t byte){
	if(byte & (1<<1)){
		//Bit 1: RDTMP: Request a new temperature measurement to be taken.
		sts_reg |= 1<<2;
	}
}


uint8_t STS_REG(){

		//Bit 1: FLT: 1 if a fault has occurred during temperature measurement.
		//Bit 2: TBUSY: 1 if a temperature measurement is currently underway
		//Bit 3: TRDY: 1 if a temperature measurement is ready for reading. This bit is automatically cleared
		//when TMP_HI is read.
		//Bit 5: 1 if characters are in the terminal receiving buffer (CH_BUF) awaiting transmit
	return sts_reg;

}

uint8_t CH_BUF_response(){
		return chr_buf;
}

void CH_BUF_receive(uint8_t bit){
	switch (bit){
	case 0:
		// clear char read flg
		sts_reg &= ~(1<<5);
		return;
	default:
		//write to terminal
		putchar(bit);
		fflush(stdout);
		return;
	}
}


uint8_t TMP_LO(){
	return tmp_lo;
}
uint8_t TMP_HI(){
	sts_reg &= ~(1<<3);
	return tmp_hi;
}

uint8_t DEVID(){
	return 0xAA;
}



const static uint8_t(*response_table[REG_COUNT])(void) =
{
		V_MAG,
		V_MIN,
		NO_RESPONSE,
		STS_REG,
		NO_RESPONSE,
		TMP_LO,
		TMP_HI,
		CH_BUF_response,
		NO_RESPONSE,
		DEVID
};
const static void(*result_table[REG_COUNT])(uint8_t) =
{
		IGNORE_RESULT,
		IGNORE_RESULT,
		CTL_REG,
		IGNORE_RESULT,
		IGNORE_RESULT,
		IGNORE_RESULT,
		IGNORE_RESULT,
		CH_BUF_receive,
		IGNORE_RESULT,
		IGNORE_RESULT
};

