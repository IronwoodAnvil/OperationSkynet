/*
 * coord.c
 *
 *  Created on: Sep 4, 2019
 *      Author: lovep
 */

#include "coord.h"
#include <string.h>
#include <stdlib.h>

uint8_t COORD_random_dir()
{
	return (1<<(rand()%4));
}

dir_t COORD_turn_right(dir_t original)
{
	dir_t result = (original & 0xF) << 1;
	if(result&0xF0) result >>= 4;
	return result;
}

dir_t COORD_opposite_dir(dir_t original)
{
	dir_t result = (original & 0xF) << 2;
	if(result&0xF0) result >>= 4;
	return result;
}

dir_t COORD_turn_left(dir_t original)
{
	// Implemented as right 270 to use nice roll implementation with upper nibble
	dir_t result = (original & 0xF) << 3;
	if(result&0xF0) result >>= 4;
	return result;
}

coord_t COORD_move_dir(coord_t start, dir_t dir)
{
	coord_t new;
	memcpy(&new, &start, sizeof(coord_t));

	if(dir&DIR_UP) --new.row;
	if(dir&DIR_RIGHT) ++new.col;
	if(dir&DIR_DOWN) ++new.row;
	if(dir&DIR_LEFT) --new.col;

	return new;
}

bool COORD_equal(coord_t c1, coord_t c2)
{
	return (c1.row==c2.row) && (c1.col==c2.col);
}

bool COORD_in_square(coord_t c, uint8_t size)
{
	return (c.col < size) && (c.row < size);
}

uint32_t COORD_to_index(coord_t loc, uint32_t row_size)
{
	return loc.row*row_size + loc.col;
}
