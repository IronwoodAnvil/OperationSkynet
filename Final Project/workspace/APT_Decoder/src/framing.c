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
#include "display.h"

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

state_t state; // Current state of state machine

// Current sample count (used for various purposes, usually the sample position within the current APT line
uint32_t sample_counter;

// Max an min signal level used for output mapping
uint32_t smin;
uint32_t smax;

// Struct used to perform sync correlation maximization
struct {
	uint32_t index;
	int32_t value;
} sync_max;

// Reset the sync correlation maximizer
void reset_sync_max()
{
	sync_max.value = INT32_MIN;
	sync_max.index = -1;
}

// Attempt to update the sync correlation max with the given value
void update_sync_max(int32_t sync)
{
	if(sync > sync_max.value)
	{
		sync_max.value = sync;
		sync_max.index = sample_counter;
	}
}

// Update signal max/min algorithm
void update_sample_bounds()
{
	uint32_t sample = current_sample;
	if(sample > smax) smax = sample;
	if(sample < smin) smin = sample;
}

// Initialize the state machine
void Framing_Init()
{
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
			update_sync_max(sync_corr); // Do maximizing
			update_sample_bounds();
			++sample_counter; // Count samples tested
			if(sample_counter >= LINE_LENGTH) // We have reached the end of one lines worth.  There should have been a sync A somewhere
			{
				if(sync_max.value > GOOD_SYNC_THRESH) // Was the max sync high enough to be considered a real signal?
				{
					sample_counter -= sync_max.index-SYNC_LENGTH; // Fix count to correct position in the line
					state = STATE_WAIT_LINE_1;
					puts_dma("LOCKED\r\n"); // Indicate successful signal lock
				}
			}
			break;
		}
	case STATE_WAIT_LINE_1: // Wait until the next sync pulse is expected so we start the main process on a new line
		{
			++sample_counter;
			update_sample_bounds();
			if(sample_counter >= LINE_LENGTH) // We reached the end of the line, move into normal operation
			{
				sample_counter = 0; // Starting new line
				reset_sync_max(); // Sync state needs cleared maximizer
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
				if(sync_max.value > GOOD_SYNC_THRESH) // Found a strong sync pulse in the interval, signal is still good
				{
					int32_t drift = sync_max.index - SYNC_LENGTH; // Calculate deviation from expected sync
					sample_counter -= drift; // Fix bit counter
					printf_dma("Drifted %+ld Samples\r\n",drift); // Indicate drift amount
					state = STATE_WAIT_IMAGE;
				} else {
					puts_dma("Lost sync!\r\n"); // Indicate loss of good signal
					Framing_Init(); // Reset everything and start search from scratch
				}
			}
			break;
		}
	case STATE_WAIT_IMAGE:
		{
			++sample_counter;
			// Wait until the end of the sync and space interval
			// Blank interval adds buffer in case sync corrected sample counter forward
			if(sample_counter >= IMAGE_START)
			{
				sample_counter = 0;
				state = STATE_EMIT;
			}
			break;
		}
	case STATE_EMIT:
		{
			static uint32_t accumulate; // Accumulator for oversample averaging
			switch(SUBPIXEL) // Current segment of the pixel sampling time
			{
			case 0: // Reset averaging for new bit
				accumulate = 0;
				break;
			case 3: case 4: case 5: case 6: // Average 4 middle samples to get level
				accumulate += current_sample;
				break;
			case 7: // end of bit, calculate value and emit
				{   // accumulate is 4x16bit = 18 bit
					// smin/max are 16 bit
					// subtract smin as 18 bit
					// divide by range in 10b is 8 bit
					uint32_t remapped = (accumulate-(smin<<2))/((smax-smin)>>6);
					if(remapped>0xFF) remapped = 0xFF; // Saturate if this is out of the established range
					DISP_EmitPixel((uint8_t)remapped);
					break;
				}
			}
			++sample_counter;
			if(PIXEL >= IMAGE_PIXELS) // Finished emitting entire line, prep for resync
			{
				DISP_NewLine();
				sample_counter = 0;
				reset_sync_max();
				state = STATE_SYNC;
			}
			break;
		}
	}
}
