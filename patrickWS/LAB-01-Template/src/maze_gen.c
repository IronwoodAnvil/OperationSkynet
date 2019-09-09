/*
 * maze_gen.c
 *
 *  Created on: Sep 4, 2019
 *      Author: lovep
 */

#include "maze_gen.h"
#include <stdlib.h>
#include <string.h>

void aldous_broder_ust(dir_t* field, uint32_t size)
{
	memset(field,0,size*size*sizeof(dir_t)); //Clear maze

	coord_t currentCoord = {0,0};
	dir_t* currentDirs = field;

	uint32_t remaining = size*size-1;
	while(remaining)
	{
		dir_t dir;
		coord_t nextCoord;
		do // Chose a random valid direction and get the coord
		{
			dir = COORD_random_dir();
			nextCoord = COORD_move_dir(currentCoord, dir);
		} while (!COORD_in_square(nextCoord, size));
		dir_t* nextDirs = field + COORD_to_index(nextCoord, size);

		if(!*nextDirs) // No directions, unvisited.  Add to path and decrement remaining
		{
			--remaining;
			*currentDirs |= dir;
			*nextDirs |= COORD_opposite_dir(dir);
		}
		currentCoord = nextCoord;
		currentDirs = nextDirs;
	}

	//Always go from top to bottom of maze
	uint32_t start_id = rand() % size;
	uint32_t end_id = (size*size-1) - (rand() % size);
	field[start_id] |= DIR_UP;
	field[end_id] |= DIR_DOWN;
}

void edge_to_printable(dir_t* edge_maze, bool* print, uint32_t size)
{
	uint32_t print_size = 2*size+1;

	for(uint32_t i = 0; i < print_size; ++i) // Set all corners as walls
	{
		for(uint32_t j = 0; j < print_size; ++j)
		{
			bool row_odd = i%2;
			bool col_odd = j%2;
			bool fill = false;
			if(row_odd)
			{
				if(col_odd){ // Corridor square
					fill = false;
				}
				else // possible left/right boundary
				{
					uint32_t id = j/2;
					dir_t mask = DIR_LEFT;
					if(id==size) { // Last col, need to use right wall indicator
						id = size-1;
						mask = DIR_RIGHT;
					}
					fill = !(edge_maze[(i/2)*size+id] & mask); // Fill if can't go this way
				}
			}
			else
			{
				if(col_odd){ // possible up/down boundary
					uint32_t id = i/2;
					dir_t mask = DIR_UP;
					if(id==size) { // Last col, need to use right wall indicator
						id = size-1;
						mask = DIR_DOWN;
					}
					fill = !(edge_maze[id*size+(j/2)] & mask); // Fill if can't go this way
				}
				else // Corner, always wall
				{
					fill = true;
				}
			}
			print[i*print_size + j] = fill;
		}
	}
}



void MGEN_Generate(bool* arr, uint32_t size)
{
	dir_t edges[size*size];
	aldous_broder_ust(edges, size);
	edge_to_printable(edges, arr, size);
}


