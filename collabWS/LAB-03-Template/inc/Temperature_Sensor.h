/*
 * Temperature_Sensor.h
 *
 *  Created on: Sep 26, 2019
 *      Author: Kyle Wilt
 *
 *  Use: A simple utility to configure an STM32F769 to measure the temperature
 *  sensor on ADC1 and output.  initTempSensor() must be called during initializations
 *  then getTemperature() during run-time.  getTemperature() returns a 12-bit
 *  right justified number of the direct ADC conversion of the sensor.
 */

#ifndef TEMPERATURE_SENSOR_H_
#define TEMPERATURE_SENSOR_H_

ADC_HandleTypeDef hADC1;

void initTempSensor(){
	__HAL_RCC_ADC1_CLK_ENABLE();
	hADC1.Instance = ADC1;
	hADC1.Init.Resolution = ADC_RESOLUTION_12B;
	hADC1.Init.NbrOfConversion = 1;
	hADC1.Init.ContinuousConvMode = DISABLE;
	hADC1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hADC1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	HAL_ADC_Init(&hADC1);

	//ADC->CCR |= ADC_CCR_TSVREFE;
	ADC_ChannelConfTypeDef adcchan = {  .Channel = ADC_CHANNEL_TEMPSENSOR,
										.Rank = ADC_REGULAR_RANK_1,
										.SamplingTime = ADC_SAMPLETIME_480CYCLES};
	HAL_ADC_ConfigChannel(&hADC1,&adcchan);
}

uint16_t getTemperature(){
	HAL_ADC_Start(&hADC1);
	HAL_StatusTypeDef res =  HAL_ADC_PollForConversion(&hADC1,100);
	if(res == HAL_OK){
		return HAL_ADC_GetValue(&hADC1);
	}
	return 0;
}

#endif /* TEMPERATURE_SENSOR_H_ */