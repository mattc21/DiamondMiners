/*
 * timer0.c
 *
 * Author: Peter Sutton
 *
 * We setup timer0 to generate an interrupt every 1ms
 * We update a global clock tick variable - whose value
 * can be retrieved using the get_clock_ticks() function.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer0.h"
#include "game.h"
#include "timer1.h"

#define NO_SOUND_OFF UINT32_MAX

/* Our internal clock tick count - incremented every 
 * millisecond. Will overflow every ~49 days. */
static volatile uint32_t clockTicks;
/* Seven segment display values */
uint8_t seven_seg[10] = { 63,6,91,79,102,109,125,7,127,111};
	
uint32_t sound_off_time = NO_SOUND_OFF;

// A struct for a jingle of up to 8 notes
struct Jingle {
	int notes[8];
	uint8_t play_jingle;
	uint8_t count;
	uint32_t interval;
	uint32_t next_play;
} jingle;



/* Set up timer 0 to generate an interrupt every 1ms. 
 * We will divide the clock by 64 and count up to 124.
 * We will therefore get an interrupt every 64 x 125
 * clock cycles, i.e. every 1 milliseconds with an 8MHz
 * clock. 
 * The counter will be reset to 0 when it reaches it's
 * output compare value.
 */
void init_timer0(void) {
	/* Reset clock tick count. L indicates a long (32 bit) 
	 * constant. 
	 */
	clockTicks = 0L;
	
	/* Clear the timer */
	TCNT0 = 0;

	/* Set the output compare value to be 124 */
	OCR0A = 124;
	
	/* Set the timer to clear on compare match (CTC mode)
	 * and to divide the clock by 64. This starts the timer
	 * running.
	 */
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS01)|(1<<CS00);

	/* Enable an interrupt on output compare match. 
	 * Note that interrupts have to be enabled globally
	 * before the interrupts will fire.
	 */
	TIMSK0 |= (1<<OCIE0A);
	
	/* Make sure the interrupt flag is cleared by writing a 
	 * 1 to it.
	 */
	TIFR0 &= (1<<OCF0A);
	

}

uint32_t get_current_time(void) {
	uint32_t returnValue;

	/* Disable interrupts so we can be sure that the interrupt
	 * doesn't fire when we've copied just a couple of bytes
	 * of the value. Interrupts are re-enabled if they were
	 * enabled at the start.
	 */
	uint8_t interruptsOn = bit_is_set(SREG, SREG_I);
	cli();
	returnValue = clockTicks;
	if(interruptsOn) {
		sei();
	}
	return returnValue;
}

ISR(TIMER0_COMPA_vect) {
	/* Increment our clock tick count */
	clockTicks++;
	
	
	
	// if state is 1, we are going to change the rightmost digit and vice versa (basically aligning change to cc bit)
	uint8_t state = (PORTA & (1 << PORTA6)) >> PORTA6;

	
	if(get_game_initialised()) {
		/* Display a digit */
		if(state == 1) {
			/* Display rightmost digit - tenths of seconds */
			PORTC = seven_seg[(get_steps())%10];
		} else {
			/* Display leftmost digit - seconds */
			PORTC = seven_seg[(get_steps())/10];
		}
		/* Output the digit selection (CC) bit */
		PORTA ^= (1 << PORTA6);	
		
		/* For individual sounds */
		if (clockTicks > sound_off_time) {
			sound_off_time = NO_SOUND_OFF;
			sound_off();
		}
		
		/* For playing jingles */
		if (jingle.play_jingle == 1) {
			if (clockTicks > jingle.next_play) {
				if (jingle.notes[jingle.count] == -1 || jingle.count > 7) {
					jingle.play_jingle = 0;
					sound_off();
				} else {
					jingle.next_play = clockTicks + jingle.interval;	
					switch (jingle.notes[jingle.count]) {
						case NOTE_A:
							play_A();
							break;
						case NOTE_B:
							play_B();
							break;
						case NOTE_C:
							play_C();
							break;
						case NOTE_D:
							play_D();
							break;
						case NOTE_E:
							play_E();
							break;
						case NOTE_F:
							play_F();
							break;
						case NOTE_G:
							play_G();
							break;
						case NO_NOTE:
							sound_off();
					}
					jingle.count ++;
				}
			}
		}
	} else {
		/* No digits displayed -  display is blank */
		PORTC = 0;
	}
	
}

/* 
 * Sets time at which to switch off sound.
 * Parameters:
 *		time: time in milliseconds after current time to switch sound off.
 */
void time_till_sound_off(uint32_t time) {
	sound_off_time = clockTicks + time;
}


/*
 * Sets jingle to C E and G and calls for it to be played.
 */
void play_found_diamond() {
	uint8_t count = 0;
	uint8_t play_jingle = 1;
	uint32_t interval = 200;
	uint32_t next_play = 0;
	jingle = (struct Jingle) {{NOTE_C, NOTE_E, NOTE_G, -1, -1, -1, -1, -1}, play_jingle, count, interval, next_play};
	
}

/*
 * Plays jingle for starting game
 */
void play_start_game() {
	uint8_t count = 0;
	uint8_t play_jingle = 1;
	uint32_t interval = 200;
	uint32_t next_play = 0;
	jingle = (struct Jingle) {{NOTE_C, NOTE_D, NOTE_E, NOTE_F, NOTE_G, NO_NOTE, NOTE_C, -1}, play_jingle, count, interval, next_play};
	
}

/*
 * Plays game over jingle.
 */
void play_game_over() {
	uint8_t count = 0;
	uint8_t play_jingle = 1;
	uint32_t interval = 150;
	uint32_t next_play = 0;
	jingle = (struct Jingle) {{NOTE_F, NOTE_E, NO_NOTE, NOTE_A, NOTE_A, NOTE_A, NOTE_A, -1}, play_jingle, count, interval, next_play};
}

/*
 * Plays blow_bomb jingle.
 *
 */
void play_blow_bomb() {
	uint8_t count = 0;
	uint8_t play_jingle = 1;
	uint32_t interval = 120;
	uint32_t next_play = 0;
	jingle = (struct Jingle) {{NOTE_A, NOTE_G, NOTE_F, NOTE_E, NOTE_D, -1, -1, -1}, play_jingle, count, interval, next_play};
}
