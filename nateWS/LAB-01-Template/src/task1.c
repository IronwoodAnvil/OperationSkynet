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


    Sys_Init(); // This always goes at the top of main (defined in init.c)


    char choice;
    printf("\033[2J\r\n");
    fflush(stdout);
    printf("Press <ESC> or the key combination ^[ to quit \r\n");
    while(1)
    {
				choice = getchar();
				if (choice == E_CHAR) break; //handle
				printf("The keyboard character is %c. \r\n", choice);
	}
    printf("\n\nDone!\r\n");
}

//------------------------------------------------------------------------------------
//Extra thing to consider...
//----------------------------------------------------------------------------------
