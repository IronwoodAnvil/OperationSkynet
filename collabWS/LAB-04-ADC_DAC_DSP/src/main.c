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
	printf("%d+%d = %ld\r\n",2718,31415,result);

	int16_t v1 = 2718;
	int16_t v2 = 31415;
	asm("smulbb %0, %1, %2"
			: "=r" (result) : "r" (v1), "r" (v2));
	printf("%d*%d = %ld\r\n",v1,v2,result);

	int32_t x = 42;
	asm("mov %[r], %[x]		\r\n" // x
		"mul %[r], %[n2]	\r\n" // Multiply by 2
		"sdiv %[r], %[n3]	\r\n" // Divide by 3
		"add %[r], #5		\r\n" // Add 5
			: [r] "=r" (result)
			: [x] "r" (x), [n3] "r" (3), [n2] "r" (2));
	printf("[MUL] %lu*2/3 + 5 = %lu\r\n", x, result);

	result = 0; // Prove that actual result is regenerated
	asm("mov %[r], #15					\r\n"
		"mla %[r], %[x], %[n2], %[r]	\r\n" // 2*x + 15
		"sdiv %[r], %[n3]				\r\n" // Divide by 3
			: [r] "=r" (result)
			: [x] "r" (x), [n3] "r" (3), [n2] "r" (2));
	printf("[MAC] %ld*2/3 + 5 = %ld\r\n", x, result);

	fflush(stdout);

	while(1);

#elif SUB_TASK == 2

#endif

#elif LAB_TASK == 4

#elif LAB_TASK == 5

#endif
}
