#include "init.h"
#include "sampling.h"
#include "framing.h"
#include "stm32f769i_discovery_lcd.h"
#include "display.h"

// Main Execution Loop
int main(void)
{
	// Initialize Everything
	Sys_Init();
	Sampling_Init();
	Disp_Init();
	Framing_Init();

	// Start the sampling/filtering
	Sampling_Start();
	while(1)
	{
		if(g_new_sample) // Advance framing state machine for each new sample
		{
			Framing_Tasks();
			g_new_sample = false;
		}
	}

}

