/*
 * msp_paint.c
 *
 *  Created on: Nov 18, 2019
 *      Author: lovep
 */

#include <ansi_util.h>
#include "mps_paint.h"
#include "stm32f7xx_hal.h"
#include "uart.h"

// Colors.  Underlying representation is offset in ANSI code order
typedef enum {
	COLOR_BLACK = 0,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_BLUE,
	COLOR_MAGENTA,
	COLOR_CYAN,
	COLOR_WHITE
} color_t;
// Converts generic colors above to ANSI codes for fore and background
#define ANSI_BACKGROUND(color) ((uint8_t)(color)+40)
#define ANSI_FOREGROUND(color) ((uint8_t)(color)+30)
// Scale mouse update numbers to screen movement
#define MOUSE_SENSITIVITY (0.01f)

// Current draw color
color_t draw_color;

// Current cursor position
float cursor_x;
float cursor_y;


// Gets contrasting text color for a given background
// Color order is super convenient to have this work as a bitmask
// Green, yellow, cyan and white have black test, rest have white
#define CONTRAST(c)  (((c)&0x02)?COLOR_BLACK:COLOR_WHITE)

// Emits the ANSI sequence to set the draw color to the provided arg
// and the text color to the correct contrast color
static void set_term_color(color_t color)
{
	ANSI_TextFormat(2, ANSI_BACKGROUND(color), ANSI_FOREGROUND(CONTRAST(color)));
}

// Cycles the active draw color to the next one
static void cycle_color()
{
	draw_color += 1;
	draw_color &= 0x07;
	set_term_color(draw_color);
}

// Writes the 'MPS Paint' header and then returns the cursor to the draw position
void write_title()
{
	ANSI_SaveCursor();
	ANSI_WriteCenter(1, "  MPS Paint  ");
	ANSI_RestoreCursor();
}

// Receive decoded mouse events and perform actions
void MPS_Paint_OnMouse(int8_t x, int8_t y, bool lclick, bool rclick)
{
	static bool last_rclick = false;

	if(rclick && !last_rclick){ // Right click press
		cycle_color();
		write_title(); // Current color is indicated as color of title block
	}

	// Move cursor in accordance with mouse
	cursor_x += 2.5f * x * MOUSE_SENSITIVITY; // Increase x speed as the columns are ~2.5x thinner
	cursor_y += y * MOUSE_SENSITIVITY;

	// Keep cursor in terminal area
	if(cursor_x < 0) cursor_x = 0.0f;
	else if(cursor_x > 80) cursor_x = 80.0f;

	if(cursor_y < 0) cursor_y = 0.0f;
	else if(cursor_y > 25) cursor_y = 25.0f;

	// Update cursor
	ANSI_SetCursor((uint32_t)cursor_y, (uint32_t)cursor_x);
	if(lclick) // If they are drawing, draw
	{
		putchar(' '); // Space with background color
		putchar('\b'); // Return cursor on top of painted square
	}

	ANSI_flush(); // Actually send stuff out the UART

	last_rclick = rclick;
}

void MPS_Paint_Init()
{
	draw_color = COLOR_WHITE;
	// Start at center of screen
	cursor_x = 40.0;
	cursor_y = 12.5;

	// Reset terminal format and blank screen
	ANSI_TextFormat(2, ANSI_CLEAR_FMT, ANSI_BACK_BLACK);
	ANSI_ClearScreen();
	// Set the starting draw color
	set_term_color(draw_color);
	// Move the cursor to the start position
	ANSI_SetCursor(12, 40);
	// Write the title
	write_title();
	ANSI_flush(); // Actually send stuff out the UART

}

void MPS_Paint_End()
{
	// Don't leave the terminal printing everything with weird backgrounds
	ANSI_TextFormat(1, ANSI_CLEAR_FMT);
	ANSI_ClearScreen(); // blank the terminal
	ANSI_print("\rThanks for using MPS Paint.  To relaunch, plug in a mouse again\r\n");
}

#define MODE_NONE 0
#define MODE_CONTROL 1
#define MODE_TEXT 2

#define KEYBOARD_MODE MODE_TEXT

void MPS_Paint_Tasks()
{
	// Actually nothing to do with the mouse controls here
	// Instead we added some keyboard support modes, either does nothing,
	// writes text at current cursor, or implements a full control scheme (used for testing)
#if KEYBOARD_MODE != MODE_NONE
	uint8_t in;
	if(HAL_UART_Receive(&USB_UART, &in, 1, 0) == HAL_OK) // Poll for typed character
	{
#if KEYBOARD_MODE == MODE_CONTROL
		static bool keydraw = false;
		static bool textmode = false;
		if(!textmode)
		{
			// Keyboard controls.  Mimic mouse inputs + a few mode switches
			if(in=='t')
			{
				textmode = true;
			}
			else if(in=='p') {
				keydraw = !keydraw;
				MPS_Paint_OnMouse(0, 0, keydraw, false);
			}
			else if(in=='w') MPS_Paint_OnMouse(0, -1/MOUSE_SENSITIVITY, keydraw, false);
			else if(in=='s') MPS_Paint_OnMouse(0, 1/MOUSE_SENSITIVITY, keydraw, false);
			else if(in=='a') MPS_Paint_OnMouse(-1/(2.5f*MOUSE_SENSITIVITY), 0, keydraw, false);
			else if(in=='d') MPS_Paint_OnMouse(1/(2.5f*MOUSE_SENSITIVITY), 0, keydraw, false);
			else if(in=='c')
			{ // Needs full press to detect and reset
				MPS_Paint_OnMouse(0, 0, keydraw, true);
				MPS_Paint_OnMouse(0, 0, keydraw, false);
			}
		}
		else {
#endif
		if(ANSI_IsPrintable(in)){ // Print printable characters
			putchar(in);
			cursor_x += 1; // update internal cursor representation to match
			if(cursor_x > 80) cursor_x=80.0;
			ANSI_flush();
		}
#if KEYBOARD_MODE == MODE_CONTROL
			else if(in=='\r'){ // End text entry mode on enter when using keyboard controls
				textmode = false;
			}
		}

#endif
	}
#endif
}


