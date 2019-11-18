/*
 * msp_paint.c
 *
 *  Created on: Nov 18, 2019
 *      Author: lovep
 */

#include <ansi_util.h>

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
#define ANSI_FOREGROUND(color) ((uint8_t)(color)+40)
#define MOUSE_SENSITIVITY 0.01;

color_t draw_color;

float cursor_x;
float cursor_y;

static color_t contrast_color(color_t c1)
{
	switch(c1)
	{
	case COLOR_WHITE:
	case COLOR_CYAN:
	case COLOR_YELLOW:
		return COLOR_BLACK;
	default:
		return COLOR_WHITE;
	}
}

static void set_term_color(color_t color)
{
	ANSI_TextFormat(1, ANSI_BACKGROUND(color), ANSI_FOREGROUND(contrast_color(color)));
}

static void cycle_color()
{
	draw_color += 1;
	draw_color &= 0x0F;
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

	if(last_rclick && !rclick){ // Right click release
		cycle_color();
		write_title();
		ANSI_flush();
	}

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

void MPS_Paint_Tasks()
{

}
