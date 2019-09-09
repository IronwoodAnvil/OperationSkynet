/*
 * coord.h
 *
 *  Created on: Sep 4, 2019
 *      Author: lovep
 */

#ifndef INC_COORD_H_
#define INC_COORD_H_

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t dir_t;

// Define directions as bits.  Allows them to be combined
// or used to define the valid paths out of a maze cell...
#define DIR_NONE 0
#define DIR_UP 0b1000
#define DIR_RIGHT 0b0100
#define DIR_DOWN 0b0010
#define DIR_LEFT 0b0001

// General (unsigned) 2d coordinate struct
// 16 bit vals to be retrievable as a single word
typedef struct
{
	uint16_t row;
	uint16_t col;
} coord_t;

// Determines if both coordinates are in the interval [0,row_size)
bool COORD_in_square(coord_t c, uint8_t size);
// Determines if two coordinates are equal
bool COORD_equal(coord_t c1, coord_t c2);
// Produces a random direction
uint8_t COORD_random_dir();
// Computes the direction 90 degrees right of the original
dir_t COORD_turn_right(dir_t original);
// Computes the direction 90 degrees right of the original
dir_t COORD_turn_left(dir_t original);
// Computes the opposite direction to the one given
dir_t COORD_opposite_dir(dir_t original);
// Produces the coordinate in the given direction from the start
// Can do diagonal (e.g. DIR_UP | DIR RIGHT and no movement (e.g. DIR_NONE or DIR_UP | DIR_DOWN)
coord_t COORD_move_dir(coord_t start, dir_t dir);
// Converts a coordinate to its index in a flattened array assuming the given row size
uint32_t COORD_to_index(coord_t loc, uint32_t row_size);

#endif /* INC_COORD_H_ */
