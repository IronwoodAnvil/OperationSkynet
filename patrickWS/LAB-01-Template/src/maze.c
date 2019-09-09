/*
 * maze.c
 *
 *  Created on: Sep 3, 2019
 *      Author: lovep
 */

#include "maze.h"
#include "maze_gen.h"
#include "coord.h"
#include "ansi_util.h"
#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>
#include "uart.h"

#define ESCAPE_CHARACTER '\x1B'

typedef enum {
	STATE_MENU,
	STATE_PLAY
} state_t;

#define PLAYER_STR "()"
#define MAZE_SIZE 9
#define MAZE_DRAW_SIZE MGEN_DISP_ROW_SIZE(MAZE_SIZE)
#define MAZE_ARR_SIZE MGEN_DISP_ARR_SIZE(MAZE_SIZE)
#define MAZE_ROW_OFF 5
#define MAZE_COL_OFF ((ANSI_MAX_COL-2*MAZE_DRAW_SIZE)/2 + 1)
#define WIN_BOX_W 15

#define MAZE_WALL_COLOR ANSI_BACK_GREEN
#define MAZE_PATH_COLOR ANSI_BACK_YELLOW

#define SECONDS 1000000
#define MILLIS 1000

typedef struct {
	coord_t player;
	bool walls[MAZE_ARR_SIZE];
} maze_t;


void delay_us(uint32_t us)
{
	uint32_t delay = us * 216; // Apparently the loop can be a single instruction, so 216 instruction/us at 216MHz
	while(--delay) asm(""); // asm required to prevent loop from optimizing out
}

void wait_debounce_release()
{
	delay_us(10*MILLIS);
	while(SW4Val);
}

bool has_player_won(const maze_t* state)
{
	return state->player.row==(MAZE_DRAW_SIZE-1);// Player is in bottom row (only non-wall is exit)
}


void create_maze(maze_t* val)
{
	MGEN_Generate(val->walls, MAZE_SIZE);
	val->player.row = 0;
	val->player.col = 1;
	// Find the open start position
	while(val->walls[val->player.col]) {
		val->player.col+=2;
	}
}

void draw_maze(const maze_t* state)
{
	ANSI_TextFormat(1,MAZE_WALL_COLOR);
	bool isWallColor = true;
	for(uint32_t i=0; i < MAZE_DRAW_SIZE; ++i)
	{
		const bool* row = state->walls + i*MAZE_DRAW_SIZE;
		ANSI_SetCursor(MAZE_ROW_OFF+i,MAZE_COL_OFF);
		for(uint32_t j=0; j < MAZE_DRAW_SIZE; ++j)
		{
			bool wall = row[j];
			if(wall && !isWallColor) ANSI_TextFormat(1,MAZE_WALL_COLOR);
			else if(!wall && isWallColor) ANSI_TextFormat(1,MAZE_PATH_COLOR);
			isWallColor = wall;
			putchar(' '); putchar(' ');
		}
	}

	ANSI_TextFormat(2, MAZE_PATH_COLOR, ANSI_FORE_BLACK);
	ANSI_SetCursor(MAZE_ROW_OFF+state->player.row, MAZE_COL_OFF+2*state->player.col);
	ANSI_SaveCursor();
	ANSI_print(PLAYER_STR);

	ANSI_flush();
}

void on_win()
{
	LD1On();
	putchar('\a');
	ANSI_TextFormat(4, ANSI_BOLD, ANSI_BACK_CYAN, ANSI_FORE_RED, ANSI_BLINK);

	uint32_t mid_row = MAZE_ROW_OFF+MAZE_DRAW_SIZE/2;
	for(uint32_t i = 0; i < 3; ++i)
	{
		ANSI_SetCursor(mid_row-1+i,(ANSI_MAX_COL-WIN_BOX_W)/2+1);
		for(uint32_t j = 0; j < WIN_BOX_W; ++j)
		{
			putchar(' ');
		}
	}
	ANSI_WriteCenter(mid_row,"YOU WIN!!!");
	ANSI_TextFormat(4, ANSI_NORMAL_WEIGHT, ANSI_BACK_MAGENTA, ANSI_FORE_CYAN, ANSI_NO_BLINK);
}


void MAZE_RunGame()
{
	LD1Off();
	ANSI_ResetScreenMode(ANSI_CURSOR_ENABLE);
	ANSI_TextFormat(1, ANSI_CLEAR_FMT);
	ANSI_TextFormat(4, ANSI_BACK_MAGENTA, ANSI_FORE_CYAN, ANSI_BOLD, ANSI_BLINK);
	ANSI_ClearScreen();
	ANSI_WriteCenter(2, "SUPER COOL MPS MAZE GAME");
	ANSI_TextFormat(2, ANSI_NORMAL_WEIGHT, ANSI_NO_BLINK);
	ANSI_WriteCenter(3, "Press button to (re)start, WASD to move, ESC to quit");
	ANSI_WriteAt(ANSI_MAX_ROW,ANSI_MAX_COL-44+1,"Copyright 2019 Patrick Love & Nathan Mascari");
	ANSI_HomeCursor();
	ANSI_flush();

	state_t game_state = STATE_MENU;
	uint8_t input_char;
	bool hasChar, first_draw = false;
	maze_t maze_state;

	while(1)
	{
		hasChar = uart_getchar_nw(&USB_UART, &input_char);
		if(hasChar&&input_char==ESCAPE_CHARACTER) break;

		if(SW4Val) // Starts/Restarts the game
		{
			game_state = STATE_PLAY;
			first_draw = true;
			wait_debounce_release();
		}

		switch(game_state)
		{
			case STATE_MENU:
				break;
			case STATE_PLAY:
			{
				bool did_draw = false;
				if(first_draw)
				{
					LD1Off();
					create_maze(&maze_state);
					draw_maze(&maze_state);
					first_draw = false;
					did_draw = true;
				}

				//TODO move/update player position
				if(hasChar)
				{
					dir_t move_dir = 0;
					if(input_char=='w') move_dir = DIR_UP;
					else if(input_char=='a') move_dir = DIR_LEFT;
					else if(input_char=='s') move_dir = DIR_DOWN;
					else if(input_char=='d') move_dir = DIR_RIGHT;

					if(move_dir){ //If the character was a valid control
						coord_t nextPos = COORD_move_dir(maze_state.player, move_dir);
						if(COORD_in_square(nextPos, MAZE_DRAW_SIZE) && !maze_state.walls[COORD_to_index(nextPos, MAZE_DRAW_SIZE)]) // Can move
						{
							ANSI_RestoreCursor();
							putchar(' '); putchar(' '); // Clear current player position
							ANSI_SetCursor(MAZE_ROW_OFF+nextPos.row, MAZE_COL_OFF+2*nextPos.col);
							ANSI_SaveCursor();
							ANSI_print(PLAYER_STR);
							did_draw = true;

							maze_state.player = nextPos;
						}
					}

				}

				if(has_player_won(&maze_state))
				{
					on_win();
					did_draw = true;
					game_state = STATE_MENU;
				}


				if(did_draw)
				{
					ANSI_HomeCursor(); // Keep cursor out of actual maze area
					ANSI_flush();
				}

				break;
			}
		}
	}


	ANSI_TextFormat(3, ANSI_BOLD, ANSI_BACK_MAGENTA, ANSI_FORE_CYAN);
	ANSI_ClearScreen();
	ANSI_WriteCenter(ANSI_MAX_ROW/2+1,"Thanks for playing!");
	ANSI_flush();

	delay_us(3*SECONDS);

	ANSI_TextFormat(1, ANSI_CLEAR_FMT);
	ANSI_ClearScreen();
	ANSI_SetScreenMode(ANSI_CURSOR_ENABLE);
	ANSI_HomeCursor();
	ANSI_flush();

	while(1);
}
