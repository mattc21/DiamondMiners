/*
 * game.c
 *
 * Contains functions relating to the play of Diamond Miners
 *
 * Author: Luke Kamols
 */ 

#include "game.h"
#include "display.h"
#include "timer0.h"
#include <stdlib.h>

#define PLAYER_START_X  0
#define PLAYER_START_Y  0
#define FACING_START_X  1
#define FACING_START_Y  0
#define NO_BOMB			UINT8_MAX

// the initial game layout
// the values 0, 3, 4 and 5 are defined in display.h
// note that this is not laid out in such a way that starting_layout[x][y]
// does not correspond to an (x,y) coordinate but is a better visual
// representation
static const uint16_t starting_layout[HEIGHT][WIDTH] = 
		{
			{0, 3, 0, 3, 0, 0, 0, 4, 4, 0, 0, 4, 0, 4, 0, 4},
			{0, 4, 0, 4, 0, 0, 0, 3, 4, 4, 3, 4, 0, 3, 0, 4},
			{0, 4, 0, 4, 4, 4, 4, 0, 3, 0, 0, 0, 0, 4, 0, 4},
			{5, 4, 0, 4, 0, 0, 3, 0, 0, 4, 0, 0, 0, 4, 0, 0},
			{4, 4, 3, 4, 5, 0, 4, 0, 0, 4, 3, 4, 0, 0, 4, 4},
			{0, 0, 0, 4, 4, 4, 4, 0, 4, 0, 0, 0, 4, 3, 0, 4},
			{0, 0, 0, 3, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 4},
			{0, 0, 0, 4, 0, 0, 3, 0, 4, 0, 0, 3, 3, 0, 5, 4} 
		};
		
static const uint16_t alternate_layout[HEIGHT][WIDTH] =
{
	{0, 3, 0, 3, 0, 0, 0, 4, 5, 0, 0, 4, 0, 4, 0, 4},
	{3, 3, 3, 4, 0, 0, 0, 3, 4, 4, 3, 4, 0, 3, 0, 4},
	{0, 4, 0, 4, 4, 0, 4, 0, 3, 0, 0, 0, 0, 4, 0, 3},
	{5, 4, 0, 4, 0, 4, 3, 3, 3, 4, 0, 0, 0, 4, 0, 4},
	{4, 4, 3, 4, 5, 0, 4, 0, 0, 4, 3, 4, 0, 4, 4, 4},
	{0, 0, 0, 3, 3, 3, 4, 0, 4, 0, 0, 0, 4, 3, 5, 4},
	{0, 4, 0, 4, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 4},
	{0, 4, 0, 4, 0, 0, 3, 0, 4, 0, 0, 3, 3, 0, 0, 4}
};
		
#define NUM_DIRECTIONS 8
static const uint8_t directions[NUM_DIRECTIONS][2] = { {0,1}, {0,-1}, {1,0}, {-1,0}};

// variables for the current state of the game
uint16_t playing_field[WIDTH][HEIGHT]; // what is currently located at each square
uint8_t visible[WIDTH][HEIGHT]; // whether each square is currently visible
uint8_t discovered[WIDTH][HEIGHT]; // This is a record of which square has been discovered or not (regardless of if visible or not)
uint8_t player_x;
uint8_t player_y;
uint8_t facing_x;
uint8_t facing_y;
uint8_t facing_visible;
uint8_t bomb_x;		// x position of bomb on map
uint8_t bomb_y;		// y position of bomb on map
uint8_t game_over;
uint8_t steps; //steps taken in game
uint8_t game_initialised; // if game has started or not
uint8_t vision_field_on; // if field of vision is on 
uint8_t bomb_visible;
// function prototypes for this file
void discoverable_dfs(uint8_t x, uint8_t y);
void initialise_game_display(void);
void initialise_game_state(void);

/*
 * initialise the game state, sets up the playing field, visibility
 * the player and the player direction indicator
 */
void initialise_game_state(void) {
	// initialise the player position and the facing position
	player_x = PLAYER_START_X;
	player_y = PLAYER_START_Y;
	facing_x = FACING_START_X;
	facing_y = FACING_START_Y;
	bomb_x = NO_BOMB;
	bomb_y = NO_BOMB;
	facing_visible = 1;
	bomb_visible = 1;
	game_over = 0;
	steps = 0;
	game_initialised = 1;
	vision_field_on = 0;
	// go through and initialise the state of the playing_field
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			// initialise this square based on the starting layout
			// the indices here are to ensure the starting layout
			// could be easily visualised when declared
			playing_field[x][y] = starting_layout[HEIGHT - 1 - y][x];
			// set all squares to start not visible, this will be
			// updated once the display is initialised as well
			visible[x][y] = 0;
			discovered[x][y] = 0;
		}
	}
}

/*
 * initialise the game state, sets up the playing field, visibility
 * the player and the player direction indicator
 */
void initialise_game_state_alt(void) {
	// initialise the player position and the facing position
	player_x = PLAYER_START_X;
	player_y = PLAYER_START_Y;
	facing_x = FACING_START_X;
	facing_y = FACING_START_Y;
	bomb_x = NO_BOMB;
	bomb_y = NO_BOMB;
	facing_visible = 1;
	bomb_visible = 1;
	game_over = 0;
	steps = 0;
	game_initialised = 1;
	// go through and initialise the state of the playing_field
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			// initialise this square based on the starting layout
			// the indices here are to ensure the starting layout
			// could be easily visualised when declared
			playing_field[x][y] = alternate_layout[HEIGHT - 1 - y][x];
			// set all squares to start not visible, this will be
			// updated once the display is initialised as well
			visible[x][y] = 0;
			discovered[x][y] = 0;
		}
	}	
}



/*
 * initialise the display of the game, shows the player and the player
 * direction indicator. 
 * executes a visibility search from the player's starting location
 */
void initialise_game_display(void) {
	// initialise the display
	initialise_display();
	// make the entire playing field undiscovered to start
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			update_square_colour(x, y, UNDISCOVERED);
		}
	}
	// now explore visibility from the starting location
	discoverable_dfs(player_x, player_y);
	// make the player and facing square visible
	update_square_colour(player_x, player_y, PLAYER);
	update_square_colour(facing_x, facing_y, FACING);
}

void initialise_game(uint8_t level) {
	// to initialise the game, we need to initialise the state (variables)
	// and the display
	if (level == 0) {
		initialise_game_state();	
	} else {
		initialise_game_state_alt();
	}
	initialise_game_display();
}

uint8_t in_bounds(uint8_t x, uint8_t y) {
	// a square is in bounds if 0 <= x < WIDTH && 0 <= y < HEIGHT
	return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

uint8_t get_object_at(uint8_t x, uint8_t y) {
	// check the bounds, anything outside the boundss
	// will be considered an unbreakable wall
	if (!in_bounds(x,y)) {
		return UNBREAKABLE;
	} else {
		//if in the bounds, just index into the array
		return playing_field[x][y];
	}
}

void flash_facing(void) {
	// only flash the facing cursor if it is in bounds
	if (in_bounds(facing_x, facing_y)) {
		if (facing_visible) {
			// we need to flash the facing cursor off, it should be replaced by
			// the colour of the piece which is at that location
			uint16_t piece_at_cursor = get_object_at(facing_x, facing_y);
			update_square_colour(facing_x, facing_y, piece_at_cursor);
		
		} else {
			// we need to flash the facing cursor on
			update_square_colour(facing_x, facing_y, FACING);
		}
		facing_visible = 1 - facing_visible; //alternate between 0 and 1
	}
}

// check the header file game.h for a description of what this function should do
// it contains a few extra hints
// Matthew Chen edit: returns 1 if a valid move is made
uint8_t move_player(uint8_t dx, uint8_t dy) {
	// YOUR CODE HERE
	// suggestions for implementation:
	// 1: remove the display of the player at the current location
	//    (and replace it with whatever else is at that location)
	// 2: determine if a move is possible
	// 3: if the player can move, update the positional knowledge of the player, 
	//    this will include variables player_x and player_y
	// 4: display the player at the new location
	
	
	uint8_t valid_move = 0;
	uint8_t object_here = get_object_at(player_x+dx, player_y+dy);
	if (object_here == EMPTY_SQUARE || object_here == DIAMOND) {
		update_square_colour(player_x, player_y, get_object_at(player_x, player_y));
		player_x += dx;
		player_y += dy;
		if (steps < 99) {
			steps ++;
		}
		if (object_here == DIAMOND) {
			play_found_diamond();
		}
		valid_move = 1;
	}
	update_square_colour(facing_x, facing_y, get_object_at(facing_x, facing_y)); // Make sure to change LED to correct colour (otherwise it may be stuck in red flash)
	facing_x = player_x + dx;
	facing_y = player_y + dy;
	flash_facing();
	update_square_colour(player_x, player_y, PLAYER);
	
	maintain_field_of_vision();
	return valid_move;
}

uint8_t is_game_over(void) {
	// initially the game never ends
	return game_over; // Note game_over = 0 if game hasn't ended and 1 otherwise
}

/*
 * given an (x,y) coordinate, perform a depth first search to make any
 * squares reachable from here visible. If a wall is broken at a position
 * (x,y), this function should be called with coordinates (x,y)
 * YOU SHOULD NOT NEED TO MODIFY THIS FUNCTION
 * Matthew Chen Edit: Spoiler. I modified it. Now it wont update square colour if its outside of field of vision and field of vision is active.
 */
void discoverable_dfs(uint8_t x, uint8_t y) {
	uint8_t x_adj, y_adj, object_here;
	// set the current square to be visible and update display
	visible[x][y] = 1;
	discovered[x][y] = 1;
	object_here = get_object_at(x, y);
	
	// Make sure that if field of vision is on, we don't update square colours that are outside of field of vision
	uint8_t 
	
	distance = abs(x - player_x) + abs(y - player_y);
	if (vision_field_on == 0 || (distance <= 2 || (distance == 3 && (abs(x - player_x) == 1 || abs(y - player_y) == 1)))) {
		update_square_colour(x, y, object_here);
	}
	
	// we can continue exploring from this square if it is empty
	if (object_here == EMPTY_SQUARE || object_here == DIAMOND) {
		// consider all 4 adjacent square
		for (int i = 0; i < NUM_DIRECTIONS; i++) {
			x_adj = x + directions[i][0];
			y_adj = y + directions[i][1];
			// if this square is not visible yet, it should be explored
			if (in_bounds(x_adj, y_adj) && !visible[x_adj][y_adj]) {
				// this recursive call implements a depth first search
				// the visible array ensures termination
				discoverable_dfs(x_adj, y_adj);
			}
		}
	}
}

/*
 * Inspects to see if it is breakable wall.
 * If it is breakable, highlights it blue.
 */
void inspect_wall(uint8_t cheatMode) {
	if (get_object_at(facing_x, facing_y) == BREAKABLE || get_object_at(facing_x, facing_y) == DISCOVERED_BREAKABLE) {
		if (cheatMode == 0) {
			playing_field[facing_x][facing_y] = DISCOVERED_BREAKABLE;
		} else {
			playing_field[facing_x][facing_y] = EMPTY_SQUARE;
			discoverable_dfs(facing_x, facing_y);
		}
	}
}

/*
 * Check if standing on diamond. Removes diamond if standing on it and returns 1, else returns 0.
 */
uint8_t check_diamond() {
	if (get_object_at(player_x, player_y) == DIAMOND) {
		playing_field[player_x][player_y] = EMPTY_SQUARE;
		return 1;
	}
	return 0;
}

/*
 * Returns the Manhattan distance to closest diamond.
 */
uint16_t diamond_distance() {
	uint16_t minDistance = UINT16_MAX;
	
	// Brute force O(n^2) solution. Constant memory though!
	for (int x = 0; x < WIDTH; x ++) {
		for (int y = 0; y < HEIGHT; y ++) {
			if (get_object_at(x, y) == DIAMOND) {
				uint16_t distance = abs(player_x - x) + abs(player_y - y);
				if (distance < minDistance ) {
					minDistance = distance;
				}
			}
		}
	}
	return minDistance;
}

/*
 * Places bomb (as long as there is no other bomb currently)
 * Return 1 if successfully places new bomb.
 */
uint8_t place_bomb() {
	if ((bomb_x == NO_BOMB) && (bomb_y == NO_BOMB)) {
		playing_field[player_x][player_y] = BOMB;
		bomb_x = player_x;
		bomb_y = player_y;
		return 1;
	}
	return 0;
}

/*
 * Blows up the bomb, destroying walls (as well as the player if they're in the range).
 * Range is any object within 1 manhattan distance from it.
 */
void blow_bomb() {
	if (bomb_x == NO_BOMB || bomb_y == NO_BOMB) {
		return;
	}
	playing_field[bomb_x][bomb_y] = EMPTY_SQUARE;
	for (int i = -1; i <= 1; i+=2) {
		uint8_t xPos = bomb_x +i;
		uint8_t yPos = bomb_y;
		uint16_t blownLocation = playing_field[xPos][yPos];
		if (blownLocation == BREAKABLE || blownLocation == DISCOVERED_BREAKABLE) {
			playing_field[xPos][yPos] = EMPTY_SQUARE;
			if (in_field_of_vision(xPos, yPos)) {
				update_square_colour(xPos, yPos, EMPTY_SQUARE);
			}
			discoverable_dfs(xPos, yPos);
		}
	}
	for (int i = -1; i <= 1; i+=2) {
		uint8_t xPos = bomb_x;
		uint8_t yPos = bomb_y+i;
		uint16_t blownLocation = playing_field[xPos][yPos];
		if (blownLocation == BREAKABLE || blownLocation == DISCOVERED_BREAKABLE) {
			playing_field[xPos][yPos] = EMPTY_SQUARE;
			if (in_field_of_vision(xPos, yPos)) {
				update_square_colour(xPos, yPos, EMPTY_SQUARE);
			}
			discoverable_dfs(xPos, yPos);
		}
	}
	
	uint8_t distance_to_bomb = abs(player_x-bomb_x) + abs(player_y-bomb_y);
	if (distance_to_bomb <= 1) {
		game_over = 1;
	}
	if (in_field_of_vision(bomb_x, bomb_y)) {
		update_square_colour(bomb_x, bomb_y, EMPTY_SQUARE);
	}
	bomb_visible = 0;
	bomb_animation_start();
}

uint8_t get_steps() {
	return steps;
}

uint8_t get_game_initialised() {
	return game_initialised;
}


/*
 * Return 0 if game is not won. Return 1 if game is won.
 * Game is won if no diamonds are left and player is standing on square on rightmost column of map.
 */
uint8_t is_game_won() {
	// Brute force O(n^2) solution. One could just store the actual number diamonds at the start...
	// but I think this makes it easier for further development (e.g. some diamonds spawn later after map creation etc)
	for (int x = 0; x < WIDTH; x ++) {
		for (int y = 0; y < HEIGHT; y ++) {
			if (get_object_at(x, y) == DIAMOND) {
				return 0;
			}
		}
	}
	if (player_x == WIDTH-1) {
		return 1;
	}
	return 0;
}

/*
 * Super hacky solution
 */
void maintain_field_of_vision() {
	if (vision_field_on) {
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				uint8_t distance = abs(x - player_x) + abs(y - player_y);
				if ((distance <= 2 || (distance == 3 && (abs(x - player_x) == 1 || abs(y - player_y) == 1)))) {
					if ((player_x != x || player_y != y) && discovered[x][y] == 1) {
						visible[x][y] = 1;
						update_square_colour(x, y, get_object_at(x, y));
					}
				} else {
					visible[x][y] = 0;
					update_square_colour(x, y, UNDISCOVERED);
				}
			}
		}
	} 
}

/*
 * Toggle field of vision.
 */
void toggle_field_of_vision() {
	vision_field_on ^= 1;
	if (vision_field_on == 0) {
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				visible[x][y] = 0;
			}
		}
		discoverable_dfs(player_x, player_y);
		update_square_colour(player_x, player_y, PLAYER);
	} else {
		maintain_field_of_vision();
	}
}

/*
 * Returns 1 if player is in danger of being blown up (i.e. in bomb distance).
 */
uint8_t in_danger() {
	// technically due to implementation of NO_BOMB, we don't even need to check
	// if there is a bomb as if there isn't a bomb, the distance will be greater than
	// 1
	uint8_t distance_to_bomb = abs(player_x-bomb_x) + abs(player_y-bomb_y);
	if (distance_to_bomb <= 1) {
		return 1;
	}
	return 0;
}

void bomb_animation_start() {
	if (bomb_x == NO_BOMB || bomb_y == NO_BOMB) {
		return;
	}
	if (in_field_of_vision(bomb_x, bomb_y)) {
		update_square_colour(bomb_x, bomb_y, PLAYER);
	}
}

void bomb_animation_middle() {
	if (bomb_x == NO_BOMB || bomb_y == NO_BOMB) {
		return;
	}
	for (int i = -1; i <= 1; i+=2) {
		uint8_t xPos = bomb_x +i;
		uint8_t yPos = bomb_y;
		if (in_field_of_vision(xPos, yPos)) {
			update_square_colour(xPos, yPos, FACING);
		}
	}
	for (int i = -1; i <= 1; i+=2) {
		uint8_t xPos = bomb_x;
		uint8_t yPos = bomb_y+i;
		if (in_field_of_vision(xPos, yPos)) {
			update_square_colour(xPos, yPos, FACING);
		}
	}
}

void bomb_animation_end() {
	if (bomb_x == NO_BOMB || bomb_y == NO_BOMB) {
		return;
	}
	for (int i = -1; i <= 1; i+=2) {
		uint8_t xPos = bomb_x +i;
		uint8_t yPos = bomb_y;
		if (in_field_of_vision(xPos, yPos)) {
			update_square_colour(xPos, yPos, get_object_at(xPos, yPos));
		}
	}
	for (int i = -1; i <= 1; i+=2) {
		uint8_t xPos = bomb_x;
		uint8_t yPos = bomb_y+i;
		if (in_field_of_vision(xPos, yPos)) {
			update_square_colour(xPos, yPos, get_object_at(xPos, yPos));
		}
	}
	if (in_field_of_vision(bomb_x, bomb_y)) {
		update_square_colour(bomb_x, bomb_y, EMPTY_SQUARE);
	}
	bomb_x = NO_BOMB;
	bomb_y = NO_BOMB;
}

/* 
 * Flashes bomb (basically same as flash_facing())
 */
void flash_bomb() {
	if (bomb_x == NO_BOMB || bomb_y == NO_BOMB) {
		return;
	}
	if (bomb_visible) {
		// we need to flash the facing cursor off, it should be replaced by
		// the colour of the piece which is at that location
		if (in_field_of_vision(bomb_x, bomb_y)) {
			update_square_colour(bomb_x, bomb_y, NO_BOMB);		
		}
	} else {
		// we need to flash the facing cursor on
		if (in_field_of_vision(bomb_x, bomb_y)) {
			update_square_colour(bomb_x, bomb_y, BOMB);
		}
	}

	bomb_visible = 1 - bomb_visible; //alternate between 0 and 1
}

/*
 * Returns if bomb is active
 */
uint8_t bomb_active() {
	return !(bomb_x == NO_BOMB || bomb_y == NO_BOMB);
}

/*
 * Returns 1 if object is in field of vision, else returns 0.
 */
uint8_t in_field_of_vision(uint8_t x, uint8_t y) {
	uint8_t distance = abs(x - player_x) + abs(y - player_y);
	if (vision_field_on) {
		if ((distance <= 2 || (distance == 3 && (abs(x - player_x) == 1 || abs(y - player_y) == 1)))) {
			return 1;			// within field of vision
		} else {
			return 0;			// outside field of vision
		}
	}
	return 1; 
}