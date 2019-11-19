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

typedef enum {
	COLOR_BLACK,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_BLUE,
	COLOR_MAGENTA,
	COLOR_CYAN,
	COLOR_WHITE
} color_t;
#define ANSI_BACKGROUND(color) ((uint8_t)(color)+40)
#define ANSI_FOREGROUND(color) ((uint8_t)(color)+30)
#define MOUSE_SENSITIVITY (0.01f)

color_t draw_color;

float cursor_x;
float cursor_y;


// Gets contrasting text color for a given background
// Color order is super convenient to have this work as a bitmask
// Green, yellow, cyan and white have black test, rest have white
#define CONTRAST(c)  (((c)&0x02)?COLOR_BLACK:COLOR_WHITE)
static void set_term_color(color_t color)
{
	ANSI_TextFormat(2, ANSI_BACKGROUND(color), ANSI_FOREGROUND(CONTRAST(color)));
}

static void cycle_color()
{
	draw_color += 1;
	draw_color &= 0x07;
	set_term_color(draw_color);
}

void write_title()
{
	ANSI_SaveCursor();
	ANSI_WriteCenter(1, "  MPS Paint  ");
	ANSI_RestoreCursor();
}

void MPS_Paint_OnMouse(int8_t x, int8_t y, bool lclick, bool rclick)
{
	static bool last_rclick = false;

	if(rclick && !last_rclick){ // Right click press
		cycle_color();
		write_title();
	}

	cursor_x += x * MOUSE_SENSITIVITY;
	cursor_y += y * MOUSE_SENSITIVITY;

	ANSI_SetCursor((uint32_t)cursor_y, (uint32_t)cursor_x);
	if(lclick)
	{
		putchar(' ');
		putchar('\b'); // Return cursor on top of painted square
	}

	ANSI_flush();
	last_rclick = rclick;
}

void MPS_Paint_Init()
{
	draw_color = COLOR_WHITE;
	cursor_x = 40.0;
	cursor_y = 12.5;

	ANSI_TextFormat(2, ANSI_CLEAR_FMT, ANSI_BACK_BLACK);
	ANSI_ClearScreen();
	set_term_color(draw_color);
	ANSI_SetCursor(12, 40);
	write_title();
	ANSI_flush();

}

#define MODE_NONE 0
#define MODE_CONTROL 1
#define MODE_TEXT 2

#define KEYBOARD_MODE MODE_NONE

void MPS_Paint_Tasks()
{
#if KEYBOARD_MODE != MODE_NONE
	uint8_t in;
	if(HAL_UART_Receive(&USB_UART, &in, 1, 0) == HAL_OK)
	{
#if KEYBOARD_MODE == MODE_CONTROL
		static bool keydraw = false;
		if(in=='p') {
			keydraw = !keydraw;
			MPS_Paint_OnMouse(0, 0, keydraw, false);
		}
		else if(in=='w') MPS_Paint_OnMouse(0, -1/MOUSE_SENSITIVITY, keydraw, false);
		else if(in=='s') MPS_Paint_OnMouse(0, 1/MOUSE_SENSITIVITY, keydraw, false);
		else if(in=='a') MPS_Paint_OnMouse(-1/MOUSE_SENSITIVITY, 0, keydraw, false);
		else if(in=='d') MPS_Paint_OnMouse(1/MOUSE_SENSITIVITY, 0, keydraw, false);
		else if(in=='c')
		{ // Needs full press to detect and reset
			MPS_Paint_OnMouse(0, 0, keydraw, true);
			MPS_Paint_OnMouse(0, 0, keydraw, false);
		}
#elif KEYBOARD_MODE == MODE_TEXT
		if(ANSI_IsPrintable(in)){
			putchar(in);
			cursor_x += 1;
			ANSI_flush();
		}
#endif
	}
#endif
}
