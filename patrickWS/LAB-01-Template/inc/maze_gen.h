/*
 * maze_gen.h
 *
 *  Created on: Sep 4, 2019
 *      Author: lovep
 */

#ifndef INC_MAZE_GEN_H_
#define INC_MAZE_GEN_H_

#include <stdint.h>
#include <stdbool.h>
#include "coord.h"

#define MGEN_DISP_ROW_SIZE(size) (2*(size)+1)
#define MGEN_DISP_ARR_SIZE(size) (MGEN_DISP_ROW_SIZE(size)*MGEN_DISP_ROW_SIZE(size))

void MGEN_Generate(bool* arr, uint32_t size);

#endif /* INC_MAZE_GEN_H_ */
