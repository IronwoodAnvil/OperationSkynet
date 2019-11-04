
#include <ansi_util.h>
#include <init.h>

#define LAB_TASK 1


#define ESCAPE_CHARACTER '\x1B'

int main()
{
	Sys_Init();
#if LAB_TASK == 1
	ANSI_TextFormat(3, ANSI_CLEAR_FMT, ANSI_FORE_YELLOW, ANSI_BACK_BLUE);
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
#endif
}
