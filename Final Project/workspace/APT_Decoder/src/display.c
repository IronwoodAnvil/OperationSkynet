/*
 * display.c
 *
 *  Created on: Dec 9, 2019
 *      Author: lovep
 */

#include "display.h"

// Select decimation
#define DOWNSAMPLE 1
// Display landscape 1, or portrait 2
#define ORIENTATION 1

struct {
	uint16_t row;
	uint16_t col;
} pix_pos;

void DISP_Init()
{
	pix_pos.row = 0;
	pix_pos.col = 0;

	BSP_LCD_Init();
	BSP_LCD_DisplayOn();
	BSP_LCD_LayerDefaultInit(LTDC_DEFAULT_ACTIVE_LAYER, LCD_FB_START_ADDRESS);
	BSP_LCD_SetLayerVisible(LTDC_DEFAULT_ACTIVE_LAYER, ENABLE);
	BSP_LCD_SelectLayer(LTDC_DEFAULT_ACTIVE_LAYER);
	BSP_LCD_Clear(LCD_COLOR_BLACK);

}
void DISP_EmitPixel(uint8_t pixel)
{
	if(pix_pos.row%DOWNSAMPLE==0&&pix_pos.col%DOWNSAMPLE==0)
	{
		uint32_t color = pixel | (pixel << 8) | (pixel << 16) | 0xFF000000;
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
void DISP_NewLine()
{
	pix_pos.row++;
	pix_pos.col = 0;
#if ORIENTATION == 1
	if(pix_pos.row/DOWNSAMPLE >= BSP_LCD_GetYSize()) {
#elif ORIENTATION == 2
	if(pix_pos.row/DOWNSAMPLE >= BSP_LCD_GetXSize()) {
#endif
		pix_pos.row = 0;
	}
}
