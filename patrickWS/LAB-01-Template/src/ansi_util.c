/*
 * ansi_util.c
 *
 *  Created on: Sep 3, 2019
 *      Author: lovep
 */

#include "ansi_util.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define ANSI_ESC "\x1B["

void ANSI_WriteAt(uint32_t row, uint32_t col, char* str)
{
	ANSI_SetCursor(row, col);
	fputs(str,stdout);
}
void ANSI_WriteCenter(uint32_t row, char* str)
{
	uint32_t col = (ANSI_MAX_COL-strlen(str))/2 + 1;
	ANSI_WriteAt(row, col, str);
}
void ANSI_SetCursor(uint32_t row, uint32_t col)
{
	printf(ANSI_ESC"%lu;%luH",row,col);
}
void ANSI_HomeCursor()
{
	ANSI_print(ANSI_ESC "H");
}
void ANSI_CursorUp(uint32_t dist)
{
	printf(ANSI_ESC "%luA",dist);
}
void ANSI_CursorDown(uint32_t dist)
{
	printf(ANSI_ESC "%luB",dist);
}
void ANSI_CursorRight(uint32_t dist)
{
	printf(ANSI_ESC "%luC",dist);
}
void ANSI_CursorLeft(uint32_t dist)
{
	printf(ANSI_ESC "%luD",dist);
}
void ANSI_SaveCursor()
{
	ANSI_print(ANSI_ESC "s");
}
void ANSI_RestoreCursor()
{
	ANSI_print(ANSI_ESC "u");
}
void ANSI_ClearScreen()
{
	ANSI_print(ANSI_ESC "2J");
}
void ANSI_ClearToEnd()
{
	ANSI_print(ANSI_ESC "K");
}
void ANSI_ClearFromStart()
{
	ANSI_print(ANSI_ESC "1K");
}
void ANSI_ClearLine()
{
	ANSI_print(ANSI_ESC "2K");
}
void ANSI_ScrollSection(uint32_t startRow, uint32_t endRow)
{
	printf(ANSI_ESC "%lu;%lur", startRow, endRow);
}
void ANSI_ScrollAll()
{
	ANSI_print(ANSI_ESC "r");
}
void ANSI_TextFormat(uint32_t len, ANSI_text_format_t first, ...)
{
	va_list argp;
	va_start(argp, first);
	printf(ANSI_ESC"%u",first);
	for(uint32_t i = 1; i < len; ++i)
	{
		int val = va_arg(argp, int);
		printf(";%d", val);
	}
	va_end(argp);
	putchar('m');
}
void ANSI_SetScreenMode(ANSI_screen_mode_t mode)
{
	printf(ANSI_ESC "=%uh",mode);
}
void ANSI_ResetScreenMode(ANSI_screen_mode_t mode)
{
	printf(ANSI_ESC "=%ul",mode);
}
