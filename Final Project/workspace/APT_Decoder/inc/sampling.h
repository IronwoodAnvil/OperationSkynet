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

// Expose the
extern volatile uint16_t current_sample;
extern volatile int32_t sync_corr;
extern volatile bool new_sample;

void Sampling_Init();
void Sampling_Start();

#endif /* SAMPLING_H_ */
