//--------------------------------
// Lab 4 - Sample - Lab04_sample.c
//--------------------------------
//
//

#include "init.h"

#include <stdint.h>
#include "task.h"

// Main Execution Loop
int main(void)
{
	//Initialize the system
	Sys_Init();
	// Code goes here

#if LAB_TASK == 1

#elif LAB_TASK == 2

#elif LAB_TASK == 3

#if SUB_TASK == 1

	asm("mov r9, #2718		\r\n"
		"mov r10, #31415	\r\n"
		"add r9, r9, r10");
	int32_t result;
	asm("str r9, %0"
			: "=m" (result));
	printf("2718+31415 = %lu\r\n", result);

	int16_t v1 = 2718;
	int16_t v2 = 31415;
	asm("smulbb %0, %1, %2"
			: "=r" (result) : "r" (v1), "r" (v2));
	printf("2718*31415 = %lu\r\n", result);

	int32_t x = 42;
	asm("mov %0, %1			\r\n" // x
		"mul %0, %3			\r\n" // Multiply by 2
		"sdiv %0, %2		\r\n" // Divide by 3
		"add %0, #5			\r\n" // Add 5
			: "=r" (result)
			: "r" (x), "r" (3), "r" (2));
	printf("[MUL] 42*2/3 + 5 = %lu\r\n", result);

	asm("mov %0, #15		\r\n"
		"mla %0, %1, %3, %0	\r\n" // 2*x + 15
		"sdiv %0, %2		\r\n" // Divide by 3
			: "=r" (result)
			: "r" (x), "r" (3), "r" (2));
	printf("[MAC] 42*2/3 + 5 = %lu\r\n", result);

#elif SUB_TASK == 2

#endif

#elif LAB_TASK == 4

#elif LAB_TASK == 5

#endif
}
