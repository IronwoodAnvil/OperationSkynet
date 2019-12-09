/*
 * display.c
 *
 *  Created on: Dec 9, 2019
 *      Author: lovep
 */

#include "display.h"

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
	if(pix_pos.col < BSP_LCD_GetXSize())
	{
		uint32_t color = pixel | (pixel << 8) | (pixel << 16) | 0xFF000000;
		BSP_LCD_DrawPixel(pix_pos.col++,pix_pos.row, color);
	}
}
void DISP_NewLine()
{
	pix_pos.row++;
	pix_pos.col = 0;
	if(pix_pos.row >= BSP_LCD_GetYSize()) {
		pix_pos.row = 0;
	}
}
