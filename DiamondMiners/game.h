/*
** game.h
**
** Written by Luke Kamols, Daniel Cumming
**
** Function prototypes for those functions available externally
*/

#ifndef GAME_H_
#define GAME_H_

#include <inttypes.h>

/*
 * initialise the game, creates the internal game state and updates
 * the display of this game
 * Edited by Matthew Chen (Now it takes levels!)
 */
void initialise_game(uint8_t level);

/* returns which object is located at position (x,y)
 * the value returned will be EMPTY_SQUARE, BREAKABLE, UNBREAKABLE
 * or DIAMOND
 * if the given coordinates are out of bounds UNBREAKABLE will be returned
 */
uint8_t get_object_at(uint8_t x, uint8_t y);

/*
 * returns 1 if a given (x,y) coordinate is inside the bounds of 
 * the playing field, 0 if it is out of bounds
 */
uint8_t in_bounds(uint8_t x, uint8_t y);

/* update the player direction indicator display, by changing whether
 * it is visible or not, call this function at regular intervals to
 * have the indicator flash
 */
void flash_facing(void);

/*
 * move the position of the player by (dx, dy) if they can be moved
 * the player direction indicator should also be updated by this.
 * the player should move to square (x+dx, y+dy) if there is an
 * EMPTY_SQUARE or DIAMOND at that location.
 * get_object_at(x+dx, y+dy) can be used to check what is at that position
 * returns 1 if valid move was made.
 */
uint8_t move_player(uint8_t dx, uint8_t dy);

// returns 1 if the game is over, 0 otherwise
uint8_t is_game_over(void);

/* Author: Matthew Chen 
 * Created method to inspect walls and highlight them blue if breakable.
 * Also added cheatMode functionality so breakable walls just break.
 */
void inspect_wall(uint8_t cheatMode);

/* Author: Matthew Chen 
 * Check if player is on diamond.
 * If on diamond, remove diamond and return 1, else return 0.
 */
uint8_t check_diamond();

/* Author: Matthew Chen 
 * Gives manhattan distance to closest diamond
 */
uint16_t diamond_distance();


/* BOMBS AHOY!
 * Author: Matthew Chen
 * Places bomb at player location. Can only have one active bomb at a time. Returns 1 if successful in placing bomb.
 */
uint8_t place_bomb();

/* Author: Matthew Chen
 * Unfortunately, we can't multithread. Thus, we run the clock (2 seconds) in the project.c file
 * and then we call this to blow up the bomb.
 * game over if player dies to bomb
 * I couldn't be bothered to return where the bomb is located (can't be bothered to malloc array to return it)
 * So instead I made a global variable called bomb_location LMAO.
 * Hey it's better than iterating through map in O(n^2) time right?
 */
void blow_bomb();


/* Author: Matthew Chen
 * Return number of steps taken
 */ 
uint8_t get_steps();


/* Author: Matthew Chen
 * Returns if game is initialised (started)
 */
uint8_t get_game_initialised();

/* Author: Matthew Chen
 * Check if the game is won. Return 1 if game is won, else 0.
 * Game is only won if all diamonds are collected and player is standing on a square on the right end of the map.
 */
uint8_t is_game_won();


/* Author: Matthew Chen
 * Maintains field of vision around player if field of vision is activated.
 */
void maintain_field_of_vision();

/* Author: Matthew Chen
 * Turns on and off field of vision. Called whenever field of vision is activated and deactivated.
 */
void toggle_field_of_vision();

/* Author: Matthew Chen
 * Returns if player is in danger of bomb. Returns 1 if in danger.
 */
uint8_t in_danger();

/* Author: Matthew Chen
 * Plays bomb animation start
 */
void bomb_animation_start();

/* Author: Matthew Chen
 * Plays bomb animation middle
 */
void bomb_animation_middle();

/* Author: Matthew Chen
 * Plays bomb animation middle
 */
void bomb_animation_end();

/* Author: Matthew Chen
 * Flashes bomb (basically same as flash_facing())
 */
void flash_bomb();

/* Author: Matthew Chen
 * Returns if bomb is active
 */
uint8_t bomb_active();

/* Author: Matthew Chen
 * Returns 1 if object is in field of vision, else returns 0.
 */
uint8_t in_field_of_vision(uint8_t x, uint8_t y);

#endif

/*
 * How long? That is the question Robert Cancross posits as his focus returns, 
 * breaking the reverie he had been so graciously lost in. The dappled light 
 * filters through the leaves, falling softly upon his face. The shimmering 
 * warmth reminds him of Tuscany. Tuscany. How long had it been since he was 
 * home? He knew he could not provide an honest answer. And how long till he 
 * would return? ... Would he return? The weight of these questions was heavy 
 * upon him now, and this burden was more than he should like to carry. He 
 * busies himself by picking up equipment - with each item he lifts, the burden 
 * reduces a little more. There's no point waiting any longer, he has come this
 * far and waited this long. One last look towards the hills, before he turns 
 * towards the shaft of the mine, and begins his descent...
 */