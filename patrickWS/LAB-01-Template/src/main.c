//------------------------------------------------------------------------------------
// Hello.c
//------------------------------------------------------------------------------------
//
// Test program to demonstrate serial port I/O.  This program writes a message on
// the console using the printf() function, and reads characters using the getchar()
// function.  An ANSI escape sequence is used to clear the screen if a '2' is typed.
// A '1' repeats the message and the program responds to other input characters with
// an appropriate message.
//
// Any valid keystroke turns on the green LED on the board; invalid entries turn it off
//


//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <main.h>
#include "stm32f769xx.h"
#include "maze.h"
#include "gpio.h"
#include "ansi_util.h"

#include <stdlib.h>
#include <stdint.h>

#define LAB_TASK 4

#define ESCAPE_CHARACTER '\x1B'

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)
#if LAB_TASK == 1
    printf("Terminate Program using <ESC> or ^[\r\n\n");
    char val;
    while(1)
    {
    	val = getchar();
    	if(val == ESCAPE_CHARACTER) break;
    	printf("The keyboard character is %c.\r\n",val);
    }
    printf("\nDone!\r\n");
    while(1);
#elif LAB_TASK == 2
    ANSI_TextFormat(2, ANSI_FORE_YELLOW, ANSI_BACK_BLUE);
    ANSI_ClearScreen();
    ANSI_ScrollSection(12, ANSI_MAX_ROW);
    ANSI_WriteCenter(2,"Terminate Program using <ESC> or ^[");
    ANSI_SetCursor(11,0);
    ANSI_SaveCursor();
    ANSI_flush();

    char val;
    while(1)
    {
    	val = getchar();
    	if(val == ESCAPE_CHARACTER) break;

		ANSI_WriteAt(6,1,"The keyboard character is ");
		ANSI_TextFormat(1, ANSI_FORE_RED);
		putchar(val);
		ANSI_TextFormat(1, ANSI_FORE_YELLOW);
		putchar('.');
		ANSI_ClearToEnd();

		if(!ANSI_IsPrintable(val)) {
			ANSI_RestoreCursor();
    		ANSI_TextFormat(1, ANSI_BLINK);
    		printf("\nThe keyboard character $%02X is ", val);
			ANSI_TextFormat(1, ANSI_UNDERLINE);
			ANSI_print("'not printable'");
			ANSI_TextFormat(1, ANSI_NO_UNDERLINE);
			putchar('.');
    		ANSI_TextFormat(1, ANSI_NO_BLINK);
			ANSI_print("\a\r");
			ANSI_SaveCursor();
    	}
		ANSI_flush();
    }
    ANSI_TextFormat(1, ANSI_CLEAR_FMT);
    ANSI_ClearScreen();
    ANSI_flush();

    while(1);
#elif LAB_TASK == 3
    GPIO_Init();
    while(1)
    {
    	LD1Set(SW1Val);
    	LD2Set(SW2Val);
    	LD3Set(SW3Val);
    	LD4Set(SW4Val);
    }
#elif LAB_TASK == 4
    /*******************************************************************************************************************
     *
     * NOTE: For this task you will need to modify the build options to compile stm32f7xx_hal_rng.c
     * Also make sure you use the modified stm32f7xx_hal_conf.h or uncomment #define HAL_RNG_MODULE_ENABLED yourself
     *
     ******************************************************************************************************************/


    GPIO_Init();
    LD1Off();LD2Off();LD3Off();LD4Off(); //Turn off all LEDs at startup

    RNG_HandleTypeDef rng_handle;
    rng_handle.Instance = RNG;
    uint32_t seed = 0;

    // Spin up random number generator just to seed normal PRNG
    // We need a lot of random-ish numbers for the maze gen, and that is faster if we are not waiting on a peripheral for all of them
    __HAL_RCC_RNG_CLK_ENABLE();
    HAL_RNG_Init(&rng_handle);
    HAL_RNG_GenerateRandomNumber(&rng_handle, &seed);
    HAL_RNG_DeInit(&rng_handle); // Disable and turn off for those sweet power savings...
    __HAL_RCC_RNG_CLK_DISABLE();

    srand(seed);

    MAZE_RunGame();

#endif
}
