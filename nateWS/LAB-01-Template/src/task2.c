//------------------------------------------------------------------------------------
// mascan_task2.c       mascan      9/15/19
//------------------------------------------------------------------------------------
// Takes characters typed into terminal and prints them. blue background w/ yellow txt
// if non-printable character is typed, displays error in scroll section on bottom 1/2
// of terminal.
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
    printf("\033[;H\033[33;44m\033[=3h");//sets background color, text color, size
    printf("\r\n");
    printf("\033[2J"); //clears the terminal, making the whole background blue
    fflush(stdout);
    printf("\033[%dCPress <ESC> or the key combination ^[ to quit\033[12;24r\033[12;0H\033[s\r\n", mid-(45/2));
    while(1)      //centers text                            /sets scroll area\moves cursor\saves cursor pos
    {

		choice = getchar();
    	if (choice == E_CHAR) break; //handle escape
    	else if (choice < 31){ // this means the character is non-printable
    		printf("\033[uThe keyboard character $%02x is \033[5;4m\a'not printable'\033[24;25m\r\n\033[s", choice);
    		//underline                        print as hex\ blink\                  \unblink,un-underline\save position
    	}
    	else printf("\033[6;0HThe keyboard character is \033[31;44m%c\033[33;44m. \r\n", choice);
    			//move cursor                           txt to red     txt to blue
	}
    printf("\n\nDone!\r\n");
}
