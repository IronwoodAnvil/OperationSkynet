#include "init.h"
#include "sampling.h"
#include "framing.h"
#include "stm32f769i_discovery_lcd.h"
#include "display.h"

// Main Execution Loop
int main(void)
{
	Sys_Init();
	Sampling_Init();
	DISP_Init();
	Framing_Init();

	Sampling_Start();
	while(1)
	{
		if(new_sample)
		{
			Framing_Tasks();
			new_sample = false;
		}
	}

}

