/*
 * ansi_util.h
 *
 *  Created on: Sep 3, 2019
 *      Author: lovep
 */

#ifndef INC_ANSI_UTIL_H_
#define INC_ANSI_UTIL_H_

#include <stdint.h>
#include <stdio.h>

//Terminal size definitions
#define ANSI_MAX_ROW 25
#define ANSI_MAX_COL 80

// Text attributes for ANSI_TextFormat
typedef enum {
	ANSI_CLEAR_FMT = 0,
	ANSI_BOLD = 1,
	ANSI_FAINT = 2,
	ANSI_ITALIC = 3,
	ANSI_UNDERLINE = 4,
	ANSI_BLINK = 5,
	ANSI_REVERSE_VIDEO = 7,
	ANSI_CONCEALED = 8,
	ANSI_STRIKETHROUGH = 9,

	ANSI_DOUBLE_UNDERLINE = 21,
	ANSI_NORMAL_WEIGHT = 22,
	ANSI_NO_ITALIC = 22,
	ANSI_NO_UNDERLINE = 24,
	ANSI_NO_BLINK = 25,
	ANSI_NORMAL_VIDEO = 27,
	ANSI_REVEAL = 28,
	ANSI_NO_STRIKE = 29,


	ANSI_FORE_BLACK=30,
	ANSI_FORE_RED,
	ANSI_FORE_GREEN,
	ANSI_FORE_YELLOW,
	ANSI_FORE_BLUE,
	ANSI_FORE_MAGENTA,
	ANSI_FORE_CYAN,
	ANSI_FORE_WHITE,
	ANSI_FORE_EXTENDED,
	ANSI_FORE_DEFAULT,

	ANSI_BACK_BLACK=40,
	ANSI_BACK_RED,
	ANSI_BACK_GREEN,
	ANSI_BACK_YELLOW,
	ANSI_BACK_BLUE,
	ANSI_BACK_MAGENTA,
	ANSI_BACK_CYAN,
	ANSI_BACK_WHITE,
	ANSI_BACK_EXTENDED,
	ANSI_BACK_DEFAULT,

	ANSI_FRAME=51,
	ANSI_ENCIRCLE=52,
	ANSI_OVERLINE=53,
	ANSI_NO_FRAME_CIRCLE=54,
	ANSI_NO_OVERLINE=55
} ANSI_text_format_t;

// 2 value preabmles that can be used in the ANSI_TextFormat function followed by the corresponding color byte(s)
#define ANSI_FORE_256C_PREAMBLE ANSI_FORE_EXTENDED,5
#define ANSI_FORE_RGB_PREAMBLE ANSI_FORE_EXTENDED,2
#define ANSI_BACK_256C_PREAMBLE ANSI_BACK_EXTENDED,5
#define ANSI_BACK_RGB_PREAMBLE ANSI_BACK_EXTENDED,2

// Screen Modes for ANSI_SetScreenMode and ANSI_ResetScreenMode
typedef enum {
	ANSI_SCREEN_40x25_MONO = 0,
	ANSI_SCREEN_40x25_COLOR,
	ANSI_SCREEN_80x25_MONO,
	ANSI_SCREEN_80x25_COLOR,
	ANSI_SCREEN_320x200_4C_GRAPH,
	ANSI_SCREEN_320x200_MONO_GRAPH,
	ANSI_SCREEN_640x200_MONO_GRAPH,

	ANSI_LINE_WRAP_ENABLE=7,

	ANSI_SCREEN_320x200_COLOR_GRAPH=13,
	ANSI_SCREEN_640x200_16C_GRAPH,
	ANSI_SCREEN_640x350_MONO_GRAPH,
	ANSI_SCREEN_640x350_16C_GRAPH,
	ANSI_SCREEN_640x480_MONO_GRAPH,
	ANSI_SCREEN_640x480_16C_GRAPH,
	ANSI_SCREEN_320x200_256C_GRAPH,

	ANSI_CURSOR_ENABLE=25

} ANSI_screen_mode_t;


// Print the given string without format and without newline (like puts adds)
#define ANSI_print(str) fputs(str,stdout);
// Alias for flushing the output buffer
#define ANSI_flush() fflush(stdout);
// Determines if a given character is printable
#define ANSI_IsPrintable(c) ((c)&(~31))

// Convenience for moving cursor and printing with printf.  Must be macro to support forwarding varargs
#define ANSI_printf_at(row,col,fmt,...) ANSI_SetCursor(row,col);printf(fmt,__VA_ARGS__)
// Convenience for writing static (non-formatted) string at a particular location
void ANSI_WriteAt(uint32_t row, uint32_t col, char* str);
// Convenience for writing the given string in the center of the given row
void ANSI_WriteCenter(uint32_t row, char* str);

// Outputs the ANSI escape sequence to reset the terminal to its initial state
void ANSI_ResetTerminal();
// Outputs the ANSI escape sequence to move the cursor to the given row and column
void ANSI_SetCursor(uint32_t row, uint32_t col);
// Outputs the ANSI escape sequence to move the cursor to the upper left (home) position (row 1, col 1)
void ANSI_HomeCursor();
// Outputs the ANSI escape sequence to move the cursor up the given number of rows
void ANSI_CursorUp(uint32_t dist);
// Outputs the ANSI escape sequence to move the cursor down the given number of rows
void ANSI_CursorDown(uint32_t dist);
// Outputs the ANSI escape sequence to move the cursor right the given number of columns
void ANSI_CursorRight(uint32_t dist);
// Outputs the ANSI escape sequence to move the cursor left the given number of columns
void ANSI_CursorLeft(uint32_t dist);
// Outputs the ANSI escape sequence to save the current cursor position (to be restored later)
void ANSI_SaveCursor();
// Outputs the ANSI escape sequence to restore the cursor position at the last ANSI_SaveCursor()
void ANSI_RestoreCursor();
// Outputs the ANSI escape sequence to clear the screen (will apply any current background color)
void ANSI_ClearScreen();
// Outputs the ANSI escape sequence to clear the current line from the cursor to the end
void ANSI_ClearToEnd();
// Outputs the ANSI escape sequence to clear the current line up to the cursor
void ANSI_ClearFromStart();
// Outputs the ANSI escape sequence to clear the entire current line
void ANSI_ClearLine();
// Outputs the ANSI escape sequence to configure scroll section between the specified rows
void ANSI_ScrollSection(uint32_t startRow, uint32_t endRow);
// Outputs the ANSI escape sequence to reset the scroll section to the entire screen
void ANSI_ScrollAll();
// Outputs the ANSI escape sequence to configure the output text modifiers
// Multiple modifiers can be applied at once.  The first argument should be the number of passed modifiers
// At least 1 modifier must be passed.  To clear all use the ANSI_CLEAR_FMT modifer
void ANSI_TextFormat(uint32_t len, ANSI_text_format_t first, ...);
// Outputs the ANSI escape sequence to set the screen mode
void ANSI_SetScreenMode(ANSI_screen_mode_t mode);
// Outputs the ANSI escape sequence to reset the screen mode
void ANSI_ResetScreenMode(ANSI_screen_mode_t mode);


#endif /* INC_ANSI_UTIL_H_ */
