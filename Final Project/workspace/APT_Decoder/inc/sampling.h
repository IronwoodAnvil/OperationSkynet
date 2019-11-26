/*
 * sampling.h
 *
 *  Created on: Nov 21, 2019
 *      Author: lovep
 */

#ifndef SAMPLING_H_
#define SAMPLING_H_

#include <stdbool.h>
#include <stdint.h>

#define OVERSAMPLING (8)
#define SYNC_PERIOD (4*OVERSAMPLING)
#define SYNC_LENGTH (7*SYNC_PERIOD)
#define DC_OFFSET (1<<11)

extern volatile uint16_t samples[];
extern volatile uint8_t sid;
extern volatile int32_t sync_corr;
extern volatile bool new_sample;

#define CURRENT_SAMPLE (samples[sid])

void Sampling_Init();
void Sampling_Start();

#endif /* SAMPLING_H_ */
