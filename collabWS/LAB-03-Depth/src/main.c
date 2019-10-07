/*
 * main.c
 *
 *  Created on: Oct 7, 2019
 *      Author: lovep
 *      		mascan
 */
#include "Temperature_Sensor.h"

uint8_t sts_reg = 0x00;
uint8_t chr_buf = 0x00;
uint8_t tmp_lo = 0;
uint8_t tmp_hi = 0;

int main()
{
	for(char i = 0; i <8 ; i++){
		sts_reg[i] = 0;
	}

	initTempSensor();
}


uint8_t V_MAG(){
		return 0x4;
}

uint8_t V_MIN(){
		return 0x4;
}



uint8_t CTL_REG(uint8_t bit){
	switch (bit){
	case 1:
		//Bit 1: RDTMP: Request a new temperature measurement to be taken.
		tempRun();
		return 0;
	default:
		return;
	}
}

unit8_t tempRun(){
	int result = 0;
	int sum = 0;
	sts_reg[2] = 1;
	for(char i = 0; i<30; i++){
		result = getTemperature();
		if (result ==0){
			sts_reg[1] = 1;
			sts_reg[3] = 1;
			return;
		}
		sum += result;
	}
	sum = sum / 30;

	tmp_lo = (uint8_t)(sum & 0xFF);
	tmp_hi = (uint8_t)(sum >> 8);

	sts_reg &= 0b111111011;
	sts_reg |= 0b00001000;
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
			sts_reg &= 0b11011111;
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
	sts_reg[3] = 0;
	return tmp_hi;
}

uint8_t DEVID(){
	return 0xAA;
}


