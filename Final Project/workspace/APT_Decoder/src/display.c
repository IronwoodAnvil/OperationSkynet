/*
 * display.c
 *
 * Contains the code for keeping track of the current display output position and writing the
 * pixels to the display using the currently configured downsampling rate and screen orientation
 *
 */

#include "display.h"

// Select decimation
#define DOWNSAMPLE 1
// Display landscape 1, or portrait 2
#define ORIENTATION 1

// Current screen output position
struct {
	uint16_t row;
	uint16_t col;
} pix_pos;

void Disp_Init()
{
	pix_pos.row = 0;
	pix_pos.col = 0;

	// Initialize display
	BSP_LCD_Init();
	BSP_LCD_DisplayOn();

	// Configure foreground drawing layer
	BSP_LCD_LayerDefaultInit(LTDC_DEFAULT_ACTIVE_LAYER, LCD_FB_START_ADDRESS);
	BSP_LCD_SetLayerVisible(LTDC_DEFAULT_ACTIVE_LAYER, ENABLE);
	BSP_LCD_SelectLayer(LTDC_DEFAULT_ACTIVE_LAYER);

	// Clear to black so background is black and not random SRAM contents
	BSP_LCD_Clear(LCD_COLOR_BLACK);

}
void Disp_EmitPixel(uint8_t pixel) // Prints the next pixel to the screen
{
	// Check if this pixel should be displayed with configured downsampling
	if(pix_pos.row%DOWNSAMPLE==0&&pix_pos.col%DOWNSAMPLE==0)
	{
		// Form grayscale color in ARGB8888
		uint32_t color = pixel | (pixel << 8) | (pixel << 16) | 0xFF000000;
		// Find actual display position
		uint16_t disprow = pix_pos.row/DOWNSAMPLE;
		uint16_t dispcol = pix_pos.col/DOWNSAMPLE;
#if ORIENTATION == 1
		if(dispcol < BSP_LCD_GetXSize()) BSP_LCD_DrawPixel(dispcol,disprow, color);
#elif ORIENTATION == 2
		if(dispcol < BSP_LCD_GetYSize()) BSP_LCD_DrawPixel(disprow,BSP_LCD_GetYSize()-1-dispcol, color);
#endif
	}

	pix_pos.col++;
}
void Disp_NewLine() // Called to indicate the beginning of a new image line
{
	pix_pos.row++;
	pix_pos.col = 0;

#if ORIENTATION == 1 // Check vs. correct screen dimension for wraparound
	if(pix_pos.row/DOWNSAMPLE >= BSP_LCD_GetYSize()) {
#elif ORIENTATION == 2
	if(pix_pos.row/DOWNSAMPLE >= BSP_LCD_GetXSize()) {
#endif
		pix_pos.row = 0;
	}
}
