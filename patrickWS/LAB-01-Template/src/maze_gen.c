/*
 * maze_gen.c
 *
 *  Created on: Sep 4, 2019
 *      Author: lovep
 */

#include "maze_gen.h"
#include <stdlib.h>
#include <string.h>

// Implements the Aldous-Broder maze generation algorithm using the edge-based representation
// http://weblog.jamisbuck.org/2011/1/17/maze-generation-aldous-broder-algorithm
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

		if(!*nextDirs) // No directions yet so unvisited.  Add path and decrement remaining
		{
			--remaining;
			*currentDirs |= dir;
			*nextDirs |= COORD_opposite_dir(dir);
		}
		currentCoord = nextCoord;
		currentDirs = nextDirs;
	}

	// Clear out entrance and exit randomly on top and bottom row
	uint32_t start_id = rand() % size;
	uint32_t end_id = (size*size-1) - (rand() % size);
	field[start_id] |= DIR_UP;
	field[end_id] |= DIR_DOWN;
}

// Converts the edge-based representation to the printable (wall or no wall) representation
void edge_to_printable(dir_t* edge_maze, bool* print, uint32_t size)
{
	uint32_t print_size = 2*size+1;
	bool* print_pos = print;

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
					*print_pos = false;
				}
				else // possible left/right boundary
				{
					uint32_t id = j/2;
					dir_t mask = DIR_LEFT;
					if(id==size) { // Last col, need to use right wall indicator
						id = size-1;
						mask = DIR_RIGHT;
					}
					*print_pos = !(edge_maze[(i/2)*size+id] & mask); // Fill if can't go this way
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
					*print_pos = !(edge_maze[id*size+(j/2)] & mask); // Fill if can't go this way
				}
				else // Corner, always wall
				{
					*print_pos = true;
				}
			}

			++print_pos;
		}
	}
}



void MGEN_Generate(bool* arr, uint32_t size)
{
	dir_t edges[size*size];
	aldous_broder_ust(edges, size);
	edge_to_printable(edges, arr, size);
}


