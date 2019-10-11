/*
 * uart_fs.c
 *
 *  Created on: Sep 30, 2019
 *      Author: lovep
 */

#include "uart_fs.h"

#if UART_FS_ENABLED

#define FD_TAB_SIZE 8
#define POLL_INTERVAL 0

static UART_HandleTypeDef* fd_tab[FD_TAB_SIZE] = {0};
static int stdio_fd_map = 0;

const UART_InitTypeDef UART_CONF_DEFAULT = {
		9600, //BaudRate
		UART_WORDLENGTH_8B, //WordLength
		UART_STOPBITS_1, //StopBits
		UART_PARITY_NONE,//Parity
		UART_MODE_TX_RX,//Mode
		UART_HWCONTROL_NONE,//HwFlowCtl
		UART_OVERSAMPLING_16,//OverSampling
		UART_ONE_BIT_SAMPLE_DISABLE //OneBitSampling
};

#if UART_FS_TX_USE_INTERRUPT

	char* volatile tx_buf[FD_TAB_SIZE] = {0};

#endif

#if UART_FS_INTERRUPT_MODE_ENABLED

static dr_callback_t callback_tab[FD_TAB_SIZE] = {0};

static int USART_IRQn(USART_TypeDef* instance)
{
	if(instance==USART1) return USART1_IRQn;
	if(instance==USART2) return USART2_IRQn;
	if(instance==USART3) return USART3_IRQn;
	if(instance==UART4)  return UART4_IRQn;
	if(instance==UART5)  return UART5_IRQn;
	if(instance==USART6) return USART6_IRQn;
	if(instance==UART7)  return UART7_IRQn;
	if(instance==UART8)  return UART8_IRQn;

	while(1);
}

static void uart_fs_IRQHandler(USART_TypeDef* instance)
{
	UART_HandleTypeDef** handle_pos = fd_tab;
	while(handle_pos < fd_tab+FD_TAB_SIZE && (*handle_pos)->Instance!=instance) ++handle_pos;
	if(handle_pos >= fd_tab+FD_TAB_SIZE)
	{
		while(1);
	}
	HAL_UART_IRQHandler(*handle_pos);
}

#endif

static int findHandle(UART_HandleTypeDef* handle)
{
	UART_HandleTypeDef** handle_pos = fd_tab;
	while(handle_pos < fd_tab+FD_TAB_SIZE && *handle_pos!=handle) ++handle_pos;
	if(handle_pos >= fd_tab+FD_TAB_SIZE)
	{
		while(1);
	}
	return handle_pos-fd_tab;
}

static int fd_to_id(int file)
{
	if(stdio_fd_map && (file==0||file==1)) // stdio remapping
	{
		file = stdio_fd_map;
	}

	file -= 3;

	return file;
}

static UART_HandleTypeDef* fd_to_handle(int file)
{
	UART_HandleTypeDef* handle = NULL;

	file = fd_to_id(file);

	if(file >= 0 && file < FD_TAB_SIZE) // File is in UART table range
	{
		return fd_tab[file];
	}

	return handle;
}

// Externally defined function implementations

// Make printf(), putchar(), etc. work over -̶U̶S̶B̶  BOTH UART (default USB)
int _write(int file, char *ptr, int len) {
	UART_HandleTypeDef* handle = fd_to_handle(file); // Turn the file descriptor into its associated HAL UART handle
	if(handle){
#if UART_FS_TX_USE_INTERRUPT
		int id = fd_to_id(file);
		while(tx_buf[id]); //Wait for any running tx to complete (currently no support for queued transmit buffers)
		tx_buf[id] = malloc(len*sizeof(char)); // Need persistent buffer for transfer to read from
		memcpy(tx_buf[id],ptr,len*sizeof(char));
		HAL_UART_Transmit_IT(handle, (uint8_t*)tx_buf[id], len);
		return len;
#else
		// Pretty much the basic transmission implementation
		HAL_StatusTypeDef status = HAL_UART_Transmit(handle, (uint8_t*) ptr, len, 1000);
		if(status==HAL_OK) return len;
		else return -1;
		return len;
#endif
	} else {
		return -1;
	}
}

// Make scanf(), getchar(), etc. work over -̶U̶S̶B̶  BOTH UART (default USB)
int _read(int file, char *ptr, int len) {
	UART_HandleTypeDef* handle = fd_to_handle(file);
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
		if(fd_tab[file]->Instance)
		{
#if UART_FS_INTERRUPT_MODE_ENABLED
			NVIC_DisableIRQ(USART_IRQn(fd_tab[file]->Instance));
#endif
			HAL_UART_DeInit(fd_tab[file]);
		}
		free(fd_tab[file]);
		fd_tab[file] = NULL;
		return 0;
	}
	return -1;
}


#if UART_FS_INTERRUPT_MODE_ENABLED

// Support for all the UART peripherals cause why not
void USART1_IRQHandler()
{
	uart_fs_IRQHandler(USART1); // This simply looks up the right handle to pass to the HAL UART IRQHandler
}
void USART2_IRQHandler()
{
	uart_fs_IRQHandler(USART2);
}
void USART3_IRQHandler()
{
	uart_fs_IRQHandler(USART3);
}
void UART4_IRQHandler()
{
	uart_fs_IRQHandler(UART4);
}
void UART5_IRQHandler()
{
	uart_fs_IRQHandler(UART5);
}
void USART6_IRQHandler()
{
	uart_fs_IRQHandler(USART6);
}
void UART7_IRQHandler()
{
	uart_fs_IRQHandler(UART7);
}
void UART8_IRQHandler()
{
	uart_fs_IRQHandler(UART8);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* handle)
{
	int id = findHandle(handle); // Lookup which uart this is
	if(id < 0 || id >= FD_TAB_SIZE)
	{
		while(1);
	}
	if(callback_tab[id])
	{
		callback_tab[id](); // Pass to callback
	}
}

#if UART_FS_TX_USE_INTERRUPT
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* handle)
{
	int id = findHandle(handle); // Lookup which uart this is
	if(id < 0 || id >= FD_TAB_SIZE)
	{
		while(1);
	}
	// Free the transmit buffer and null it out (inform a pending transmit that this one is done)
	free(tx_buf[id]);
	tx_buf[id] = NULL;
}
#endif

#endif

// Public Functions


void uart_stdiod(int fd)
{
	stdio_fd_map = fd;
}

void uart_stdiof(FILE* file){
	uart_stdiod(fileno(file));
}

int uart_fdopen(USART_TypeDef* reg, UART_InitTypeDef* config)
{
	int fd = 3;
	UART_HandleTypeDef** handle_pos = fd_tab;

	while(*handle_pos) ++handle_pos, ++fd; // Find free file descriptor

	UART_HandleTypeDef* handle = malloc(sizeof(UART_HandleTypeDef)); // Create handle for this file descriptor
	memset(handle,0,sizeof(UART_HandleTypeDef)); // malloced structs are not initialized unlike global structs
	*handle_pos = handle;

	// Configure handle for init
	handle->Instance = reg;
	memcpy(&handle->Init,config,sizeof(UART_InitTypeDef));

	HAL_UART_Init(handle); // Init the UART

#if UART_FS_INTERRUPT_MODE_ENABLED
	NVIC_EnableIRQ(USART_IRQn(reg)); // Enable the appropriate NVIC interrupt vector if in interrupt mode
#endif

	return fd;
}

FILE* uart_fopen(USART_TypeDef* reg, UART_InitTypeDef* config)
{
	return fdopen(uart_fdopen(reg, config),"r+");
}

_Bool dpoll(int fd, char* c)
{
	UART_HandleTypeDef* handle = fd_to_handle(fd);
	HAL_StatusTypeDef status = HAL_UART_Receive(handle, (uint8_t*)c, 1, POLL_INTERVAL);
	return status==HAL_OK;
}

_Bool fpoll(FILE* uart, char* c)
{
	 return dpoll(fileno(uart), c);
}


#if UART_FS_INTERRUPT_MODE_ENABLED

void read_async(int fd, char* val)
{
	UART_HandleTypeDef* handle = fd_to_handle(fd);
	HAL_UART_Receive_IT(handle, (uint8_t*)val, 1);
}
void fgetc_async(FILE* file, char* val)
{
	read_async(fileno(file), val);
}

void uart_fdDataCallback(int fd, dr_callback_t cb)
{
	callback_tab[fd_to_id(fd)] = cb;
}

void uart_fDataCallback(FILE* file, dr_callback_t cb)
{
	uart_fdDataCallback(fileno(file), cb);
}

#endif

#endif
