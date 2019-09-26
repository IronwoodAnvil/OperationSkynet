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

// Populates the array arr with a randomly generated maze with size cells on each side
// Array should have allocated size of at least MGEN_DISP_ARR_SIZE(size)
// Result is a flattened representation of a (2*size-1) area, with true indicating walls
// and false indicating paths.  The entry/exit to the maze are in random positions along
// the top and bottom of the maze.
void MGEN_Generate(bool* arr, uint32_t size);

#endif /* INC_MAZE_GEN_H_ */
