
#include <ansi_util.h>
#include <init.h>

#define LAB_TASK 2


#define ESCAPE_CHARACTER '\x1B'


#if LAB_TASK == 2

SPI_HandleTypeDef hspi;
DMA_HandleTypeDef hspidma_rx;
DMA_HandleTypeDef hspidma_tx;

uint8_t rx_char;
uint8_t tx_char;

void DMA1_Stream4_IRQHandler()
{
	HAL_DMA_IRQHandler(&hspidma_tx);
}
void DMA1_Stream3_IRQHandler()
{
	HAL_DMA_IRQHandler(&hspidma_rx);
}

#endif

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
#elif LAB_TASK == 2

	SPI_Init(&hspi,&hspidma_rx,&hspidma_tx);

	printf("\033[;H\033[33;44m\033[=3h");//sets background color, text color, size
	printf("\r\n");
	printf("\033[2J"); //clears the terminal, making the whole background blue
	fflush(stdout);

	printf("\033[0;12r\033[12;24r\033[s\r\n");//sets scroll areas\moves cursor\saves cursor pos

	while(1)
	{
		tx_char =  getchar(); // if begin
		printf("\033[u\033[0;0HChar sent:%c\r\n\033[s", tx_char);

		HAL_SPI_TransmitReceive_DMA(&hspi, &tx_char, &rx_char, 1);
		while(HAL_SPI_GetState(&hspi) != HAL_SPI_STATE_READY);

		printf("\033[u\033[12BChar read:%c", rx_char);
		fflush(stdout);
	}

#endif
}
