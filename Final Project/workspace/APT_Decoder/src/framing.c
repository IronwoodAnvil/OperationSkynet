/*
 * framing.c
 *
 *  Created on: Nov 26, 2019
 *      Author: lovep
 */

#include "framing.h"
#include "sampling.h"
#include <stdio.h>
#include "uart.h"

#define GOOD_SYNC_THRESH (1<<20)

#define LINE_PIXELS 2080
#define LINE_LENGTH (OVERSAMPLING*LINE_PIXELS)
#define SYNC_INTERVAL_LENGTH (OVERSAMPLING*39)
#define IMAGE_START (OVERSAMPLING*86)
#define IMAGE_PIXELS (LINE_PIXELS-86)

#define SUBPIXEL (sample_counter&0x07)
#define PIXEL (sample_counter>>3)

typedef enum {
	STATE_LOCK_ON, // Finds sync initially over an entire line, capture max/min amplitude
	STATE_WAIT_LINE_1, // Waits for next line after first sync identified, capture max/min amplitude
	STATE_SYNC, // Finds sync within the syncing interval and adjusts sample counter
	STATE_WAIT_IMAGE, // Waits for start of image transmission with adjusted clock
	STATE_EMIT // Emits pixels for this line
} state_t;

state_t state;
uint32_t sample_counter;
// 10 bit max min since avg sum is 16+2=18 bit
// Thus we get highest resolution divisor to go direct to 8 bit
uint32_t smin;
uint32_t smax;
struct {
	uint32_t index;
	int32_t value;
} sync_max;

DAC_HandleTypeDef* hdac;


void reset_sync_max()
{
	sync_max.value = INT32_MIN;
	sync_max.index = -1;
}

void update_sync_max(int32_t sync)
{
	if(sync > sync_max.value)
	{
		sync_max.value = sync;
		sync_max.index = sample_counter;
	}
}

void update_sample_bounds()
{
	uint32_t sample = CURRENT_SAMPLE;
	if(sample > smax) smax = sample;
	if(sample < smin) smin = sample;
}

void emit_pixel(uint8_t pixel)
{
	HAL_DAC_SetValue(hdac, 0, DAC_ALIGN_8B_R, pixel);
}

void Framing_Init(DAC_HandleTypeDef* hdac_in)
{
	hdac = hdac_in;
	state = STATE_LOCK_ON;
	sample_counter = 0;
	smin = UINT32_MAX;
	smax = 0;
	reset_sync_max();
}


void Framing_Tasks()
{
	switch(state)
	{
	case STATE_LOCK_ON: // Search for sync in an entire line length, which is guaranteed to contain one
		{
			update_sync_max(sync_corr);
			update_sample_bounds();
			++sample_counter;
			if(sample_counter >= LINE_LENGTH)
			{
				if(sync_max.value > GOOD_SYNC_THRESH)
				{
					sample_counter -= sync_max.index-SYNC_LENGTH;
					state = STATE_WAIT_LINE_1;
					puts_dma("LOCKED\r\n");
				}
			}
			break;
		}
	case STATE_WAIT_LINE_1: // Wait until the next sync pulse is expected so we start the main process on a new line
		{
			++sample_counter;
			update_sample_bounds();
			if(sample_counter >= LINE_LENGTH)
			{
				sample_counter = 0;
				reset_sync_max();
				state = STATE_SYNC;
			}
			break;
		}
	case STATE_SYNC:
		{
			// Locate the sync pulse within the syncing interval and update clock
			// 39 bit interval means we can theoretically correct being 28 bits behind or 11 bits ahead!
			update_sync_max(sync_corr);
			++sample_counter;
			// This is the entire syncing interval, which is actually longer than the real sync pulse
			if(sample_counter >= SYNC_INTERVAL_LENGTH)
			{
				if(sync_max.value > GOOD_SYNC_THRESH)
				{
					int32_t drift = sync_max.index - SYNC_LENGTH; // Calculate deviation from expected sync
					sample_counter -= drift; // Fix bit counter
					printf_dma("Drifted %+ld Samples\r\n",drift);
					state = STATE_WAIT_IMAGE;
				} else {
					puts_dma("Lost sync!\r\n");
					Framing_Init(hdac); // Reset everything and start over
				}
			}
			break;
		}
	case STATE_WAIT_IMAGE:
		{
			++sample_counter;
			if(sample_counter >= IMAGE_START)
			{
				sample_counter = 0;
				state = STATE_EMIT;
			}
			break;
		}
	case STATE_EMIT:
		{
			static uint32_t accumulate;
			switch(SUBPIXEL)
			{
			case 0: // Reset averaging for new bit
				accumulate = 0;
				break;
			case 3: case 4: case 5: case 6: // Average 4 middle samples to get level
				accumulate += CURRENT_SAMPLE;
				break;
			case 7: // end of bit, calculate value and emit
				{   // accumulate is 4x16bit = 18 bit
					// smin/max are 16 bit
					// subtract smin as 18 bit
					// divide by range in 10b is 8 bit
					uint32_t remapped = (accumulate-(smin<<2))/((smax-smin)>>6);
					if(remapped>0xFF) remapped = 0xFF; // Saturate if this is out of the established range
					emit_pixel((uint8_t)remapped);
				}
			}
			++sample_counter;
			if(PIXEL >= IMAGE_PIXELS) // Finished emitting, prep for resync
			{
				sample_counter = 0;
				reset_sync_max();
				state = STATE_SYNC;
			}
			break;
		}
	}
}
