/*
 * display.h
 *
 *  Created on: Dec 9, 2019
 *      Author: lovep
 */

#include <stddef.h>
#include <stm32f769i_discovery_lcd.h>

#ifndef DISPLAY_H_
#define DISPLAY_H_

void Disp_Init();
void Disp_EmitPixel(uint8_t pixel);
void Disp_NewLine();

#endif /* DISPLAY_H_ */
