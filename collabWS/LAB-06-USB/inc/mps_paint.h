/*
 * msp_paint.h
 *
 *  Created on: Nov 18, 2019
 *      Author: lovep
 */

#ifndef MPS_PAINT_H_
#define MPS_PAINT_H_

#include <stdbool.h>

void MPS_Paint_Init();
void MPS_Paint_End();
void MPS_Paint_Tasks();
void MPS_Paint_OnMouse(int8_t x, int8_t y, bool lclick, bool rclick);


#endif /* MPS_PAINT_H_ */
