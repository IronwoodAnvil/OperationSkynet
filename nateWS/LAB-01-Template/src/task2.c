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
#include "stm32f769xx.h"
#include "hello.h"

#include<stdint.h>

#define E_CHAR 27


//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------

int main(void)
{

	int mid = 80/2;
    Sys_Init(); // This always goes at the top of main (defined in init.c)

    char choice;
    printf("\033[;H\033[33;44m\033[=3h");
    printf("\r\n"); //has putty make whole terminal blue
    printf("\033[2J");
    fflush(stdout);
    printf("\033[%dCPress <ESC> or the key combination ^[ to quit\033[12;24r\033[12;0H\033[s\r\n", mid-(45/2));
    while(1)
    {

		choice = getchar();
    	if (choice == E_CHAR) break; //handle
    	else if (choice < 31){

    		printf("\033[uThe keyboard character $%02x is \033[5;4m\a'not printable'\033[24;25m\r\n\033[s", choice);
    	}
    	else printf("\033[6;0HThe keyboard character is \033[31;44m%c\033[33;44m. \r\n", choice);

	}
    printf("\n\nDone!\r\n");
}
