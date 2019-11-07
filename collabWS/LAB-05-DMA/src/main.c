
#include <ansi_util.h>
#include <init.h>
#include <stdbool.h>

#define LAB_TASK 3


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

#elif LAB_TASK == 3

#define ADC_DMA_BATCH_SIZE 16

DMA_HandleTypeDef hdma;
ADC_HandleTypeDef hadc;
DAC_HandleTypeDef hdac;

uint32_t values[2+ADC_DMA_BATCH_SIZE] = {0};
volatile bool data_ready = false;

void DMA2_Stream0_IRQHandler()
{
	HAL_DMA_IRQHandler(&hdma);
}
void ADC_IRQHandler()
{
	HAL_ADC_IRQHandler(&hadc);
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc_local)
{
	if(hadc_local == &hadc)
	{
		data_ready = true;
	}
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

#elif LAB_TASK == 3

	//inits
	__HAL_RCC_DMA2_CLK_ENABLE();

	hdma.Instance = DMA2_Stream0;
	hdma.Init.Channel = DMA_CHANNEL_0;
	hdma.Init.Mode = DMA_CIRCULAR;
	hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	hdma.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma.Init.MemInc = DMA_MINC_ENABLE;
	hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	__HAL_LINKDMA(&hadc,DMA_Handle,hdma);
	HAL_DMA_Init(&hdma);

	ADC_Init(ADC1, &hadc);
	HAL_NVIC_EnableIRQ(ADC_IRQn);
	HAL_ADC_Start_DMA(&hadc,values+2,ADC_DMA_BATCH_SIZE);

	DAC1_Init(&hdac);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);

	uint16_t filter = 0; //history for equation h , h-1, h-2 ...
	volatile uint32_t  batch_count = 0;

	while(1)
	{
		data_ready = false;
		// Actually get data w/ DMA, values + 2 to keep history in a reasonable place

		while(!data_ready); //wait for conversion batch


		// Filter the block
		for(uint32_t t = 2; t < ADC_DMA_BATCH_SIZE; ++t)
		{
			filter = 0.312500f*values[t] + 0.240385f*values[t-1] + 0.312500f*values[t-2] + 0.296875f*filter;
			if(filter >= 1<<12) filter = (1<<12) - 1; // Saturate to 12 bits
			HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, filter); //set dac
		}
		// Store history values between batches
		values[0] = values[ADC_DMA_BATCH_SIZE];
		values[1] = values[ADC_DMA_BATCH_SIZE+1];
		++batch_count;

		HAL_ADC_Start_DMA(&hadc,values+2,ADC_DMA_BATCH_SIZE);
	}
#endif
}
