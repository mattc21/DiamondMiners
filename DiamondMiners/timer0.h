/*
 * timer0.h
 *
 * Author: Peter Sutton
 *
 * We set up timer 0 to give us an interrupt
 * every millisecond. Tasks that have to occur
 * regularly (every millisecond or few) can be added 
 * to the interrupt handler (in timer0.c) or can
 * be added to the main event loop that checks the
 * clock tick value. This value (32 bits) can be 
 * obtained using the get_clock_ticks() function.
 * (Any tasks undertaken in the interrupt handler
 * should be kept short so that we don't run the 
 * risk of missing an interrupt in future.)
 */

#ifndef TIMER0_H_
#define TIMER0_H_

#include <stdint.h>

/* Set up our timer to give us an interrupt every millisecond
 * and update our time reference.
 */
void init_timer0(void);

/* Return the current clock tick value - milliseconds since the timer was
 * initialised.
 */
uint32_t get_current_time(void);


/* Author: Matthew Chen
 * Sets time at which to switch off sound.
 * Parameters:
 *		time: time in milleseconds after current time to switch sound off.
 */
void time_till_sound_off(uint32_t time);

/*
 * Author: Matthew Chen
 * Sets jingle to be for when you find diamond.
 */
void play_found_diamond();

/*
 * Author: Matthew Chen
 * Sets jingle to be for when you start game
 */
void play_start_game();

/*
 * Author: Matthew Chen
 * Plays game over jingle.
 */
void play_game_over();

/*
 * Author: Matthew Chen
 * Plays blow bomb jingle. (Will be covered over by game over jingle
 * if player incurs a game over due to bomb).
 */
void play_blow_bomb();

#endif