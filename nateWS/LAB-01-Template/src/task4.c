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

#define LED1ON() 	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, 1)
#define LED2ON()	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, 1)
#define LED3ON()	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 1)
#define LED4ON()	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 0)

#define LED1OFF()   HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, 0)
#define LED2OFF()	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, 0)
#define LED3OFF()	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 0)
#define LED4OFF()	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 1)

#define readS1()    HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7)
#define readS2()	HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)
#define readS3()	HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_1)
#define readS4()	HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6)



//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
int main(void)
{

	//char choice;
    Sys_Init(); // This always goes at the top of main (defined in init.c)

    __HAL_RCC_GPIOJ_CLK_ENABLE(); // Need to enable clock for peripheral bus on GPIO Port J
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();

    //pj13, pj5, pa12, pd4

    /* 1. Enable the GPIO port peripheral clock
	2. Set output mode
	3. Configure output type
	4. Set output speed
	5. Set output value*/

    //OUTPUTS
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
    GPIO_InitStruct.Alternate = 0;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);



    //INPUTS
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);


    while(1){
    	if (readS1()) LED1ON(); else LED1OFF();
    	if (readS2()) LED2ON(); else LED2OFF();
    	if (readS3()) LED3ON(); else LED3OFF();
    	if (readS4()) LED4ON(); else LED4OFF();
    }




}


