#include "uart.h"

#define FD_TAB_SIZE 8
static UART_HandleTypeDef fd_tab[FD_TAB_SIZE] = {{0}};
static int stdio_fd_map = 0;

// Initialize Hardware Resources
// Peripheral's clock enable
// Peripheral's GPIO Configuration
void HAL_UART_MspInit(UART_HandleTypeDef *huart){
	GPIO_InitTypeDef  GPIO_InitStruct;
	if (huart->Instance == USART1) {
		// Enable GPIO Clocks
		__GPIOA_CLK_ENABLE();

		// Initialize TX Pin
		GPIO_InitStruct.Pin       = GPIO_PIN_9;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //TX Config

		// Initialize RX Pin
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //RX Config

		// Enable UART Clocking
		__USART1_CLK_ENABLE();

	} else if (huart->Instance == USART6) {
		// Enable GPIO Clocks
		__GPIOC_CLK_ENABLE();

		// Initialize TX Pin
		GPIO_InitStruct.Pin       = GPIO_PIN_6;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLUP;
		GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); //TX Config

		// Initialize RX Pin
		GPIO_InitStruct.Pin = GPIO_PIN_7;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); //RX Config

		// Enable UART Clocking
		__USART6_CLK_ENABLE();

		USART6->CR2 |= USART_CR2_RXINV | USART_CR2_TXINV; // Invert TX/RX for wired UART
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	if (huart->Instance == USART1) {
		__USART1_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);
		//Don't disable clock, other pins might be in use
	}
	else if(huart->Instance == USART6) {
		__USART6_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6);
		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_7);
		//Don't disable clock, other pins might be in use
	}
}

FILE* openUart(USART_TypeDef* Tgt, uint32_t Baud)
{
	int fd = 3;
	UART_HandleTypeDef* handle = fd_tab;

	while(handle->Instance) ++handle, ++fd; // Find free file descriptor

	initUart(handle, Baud, Tgt);

	return fdopen(fd,"r+");
}

void stdIOUart(FILE* uart)
{
	stdio_fd_map = fileno(uart);
}

//UART Initialization
void initUart(UART_HandleTypeDef* Uhand, uint32_t Baud, USART_TypeDef* Tgt) {
	Uhand->Instance        = Tgt;

	Uhand->Init.BaudRate   = Baud;
	Uhand->Init.WordLength = UART_WORDLENGTH_8B;
	Uhand->Init.StopBits   = UART_STOPBITS_1;
	Uhand->Init.Parity     = UART_PARITY_NONE;
	Uhand->Init.Mode       = UART_MODE_TX_RX;
	Uhand->Init.HwFlowCtl  = UART_HWCONTROL_NONE;

	HAL_UART_Init(Uhand);
}

/* ============================================================================

Reassigning _write and _read to decode file descriptor and write to the corresponding UART

Note the following for an explanation of syntax:

HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

============================================================================= */

UART_HandleTypeDef* fd_uart_decode(int file)
{
	UART_HandleTypeDef* handle = NULL;

	if(stdio_fd_map && (file==0||file==1)) // stdio remapping
	{
		file = stdio_fd_map;
	}

	file -= 3;

	if(file >= 0 && file < FD_TAB_SIZE && fd_tab[file].Instance) // File is in UART table range and has valid handle
	{
		handle = &fd_tab[file];
	}

	return handle;
}

// Make printf(), putchar(), etc. work over -̶U̶S̶B̶  BOTH UART (default USB)
int _write(int file, char *ptr, int len) {
	UART_HandleTypeDef* handle = fd_uart_decode(file);
	if(handle){
		HAL_StatusTypeDef status = HAL_UART_Transmit(handle, (uint8_t*) ptr, len, 1000);
		if(status==HAL_OK) return len;
		else return -1;
		return len;
	} else {
		return -1;
	}
}

// Make scanf(), getchar(), etc. work over -̶U̶S̶B̶  BOTH UART (default USB)
int _read(int file, char *ptr, int len) {
	UART_HandleTypeDef* handle = fd_uart_decode(file);
	if(handle)
	{
		*ptr = 0x00; // Clear the character buffer because scanf() is finicky
		len = 1; // Again because of scanf's finickiness, len must = 1
		HAL_StatusTypeDef status = HAL_UART_Receive(handle, (uint8_t*) ptr, len, HAL_MAX_DELAY);
		if(status==HAL_OK) return len;
		else if(status==HAL_TIMEOUT) return 0;
		else return -1;
	} else {
		return -1;
	}
}

int _close(int file) {
	file -= 3;
	if(file>=0 && file < FD_TAB_SIZE)
	{
		if(fd_tab[file].Instance)
		{
			HAL_UART_DeInit(&fd_tab[file]);
		}
		memset(&fd_tab[file],0,sizeof(fd_tab[file]));
		return 0;
	}
	return -1;
}

/* ============================================================================

Custom UART functions that can be mapped to any UART of your choice when called

Note the following for an explanation of syntax:

HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)

============================================================================= */

// Print a string on the specified UART. (Basically a redirectable puts)
void uart_print(UART_HandleTypeDef *huart, char* string) {
    HAL_UART_Transmit(huart, (uint8_t*) string, strlen((const char*)string), 1000);
}

// Get one character
// 'echo' means enable (1) or disable (0) echoing of characters
char uart_getchar(UART_HandleTypeDef *huart, uint8_t echo) {
	char input[1];
	HAL_UART_Receive(huart, (uint8_t *)input, 1, HAL_MAX_DELAY);
	if (echo) HAL_UART_Transmit(huart, (uint8_t*) input, 1, 1000);
	return (char)input[0];
}

// Send one character
void uart_putchar(UART_HandleTypeDef *huart, char * input) {
	HAL_UART_Transmit(huart, (uint8_t*) input, 1, 1000);
}

// Collects characters until size limit or an endline is recieved
// Returns number of characters collected
// 'max size' should match the size of the array or be smaller
int uart_getline(UART_HandleTypeDef *huart, char * input, int max_size)
{
  char single;
  int i = 0;

	while(1)
	{
		if (i > max_size)
      {
				break;
			}

		single = uart_getchar(huart, 1); // Get one character

  	if (single == '\n' || single == '\r') // Unix & co. use \n, Windows uses \r\n
			break;
  	else
			input[i] = single;
  	i++;
	}
  return i; // The way this is set up, it will always report the wrong size
}
