/*
 * project.c
 *
 * Main file for IN students
 *
 * Authors: Peter Sutton, Luke Kamols
 * Diamond Miners Inspiration: Daniel Cumming
 * Modified by <YOUR NAME HERE>
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "game.h"
#include "display.h"
#include "ledmatrix.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer0.h"
#include "timer1.h"

#define F_CPU 8000000L
#define NO_BOMB UINT32_MAX
#define NO_JOYSTICK_ACTION 512
#define JOYSTICK_LOW 300
#define JOYSTICK_HIGH 750
#include <util/delay.h>

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void start_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);
void updateInfo(uint8_t cheatMode);
void setUpPins();
void nextLevel();
uint16_t joystickDirX();
uint16_t joystickDirY();
// Global variables
uint16_t diamondCount = 0; // Count of how many diamonds
uint16_t diamondDistance = -1; // Distance to nearest diamond
uint8_t level = 0;

/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display is complete
	start_screen();
	
	// Loop forever,
	while(1) {
		new_game();
		play_game();
		if (is_game_won()) {
			nextLevel();
		} else {
			handle_game_over();
		}
	}
}

void initialise_hardware(void) {
	ledmatrix_setup();
	init_button_interrupts();
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200,0);
	
	setUpPins();
	
	init_timer0();
	init_timer1();
	// Turn on global interrupts
	sei();
	
	 
}

void start_screen(void) {
	// Clear terminal screen and output a message
	clear_terminal();
	move_terminal_cursor(10,10);
	printf_P(PSTR("Diamond Miners"));
	move_terminal_cursor(10,12);
	printf_P(PSTR("CSSE2010/7201 project by Matthew Chen 46387110"));
	
	// Output the static start screen and wait for a push button 
	// to be pushed or a serial input of 's'
	start_display();
	
	// Wait until a button is pressed, or 's' is pressed on the terminal
	while(1) {
		// First check for if a 's' is pressed
		// There are two steps to this
		// 1) collect any serial input (if available)
		// 2) check if the input is equal to the character 's'
		char serial_input = -1;
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
		}
		// If the serial input is 's', then exit the start screen
		if (serial_input == 's' || serial_input == 'S') {
			break;
		}
		// Next check for any button presses
		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED) {
			break;
		}
	}
}

void new_game(void) {
	// Clear the serial terminal
	clear_terminal();
	
	// Initialise the game and display
	initialise_game(level);
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
}

void play_game(void) {
	
	uint32_t last_flash_time, current_time, last_diamond_flash_time, bomb_time, last_joystick_time;
	uint8_t btn; //the button pushed
	uint8_t cheatMode = 0; // 1 if cheat mode is enable else 0.
	uint16_t joystickX, joystickY, bomb_flash_interval;
	uint8_t firstLoop = 1; // Whether it is the first loop of the game.
	uint8_t valid_move_made = 0;		// Whether any valid move has been made during this loop
	joystickX = NO_JOYSTICK_ACTION;
	joystickY = NO_JOYSTICK_ACTION;
	diamondCount = 0; 
	last_flash_time = get_current_time();
	last_diamond_flash_time = get_current_time();
	last_joystick_time = 0;
	bomb_time = NO_BOMB;
	bomb_flash_interval = 600;
	updateInfo(cheatMode);
	// We play the game until it's over
	while(!is_game_over()) {
		// We need to check if any button has been pushed, this will be
		// NO_BUTTON_PUSHED if no button has been pushed
		btn = button_pushed();
		valid_move_made = 0;
		// If the last joystick movement was taken greater than 0.5 seconds ago, we will take another joystick movement
		// Note we allow change in direction to be instantly registered as movements as I think this make it
		// more playable (as change in direction is likeable to change in keys pressed so should have instantaneous feedback)
		// This occurs naturally.
		if (get_current_time() - last_joystick_time > 500 || last_joystick_time == 0) {
			joystickX = joystickDirX();
			joystickY = joystickDirY();
			if (joystickX >= JOYSTICK_HIGH || joystickX <= JOYSTICK_LOW) {
				last_joystick_time = get_current_time();
			} else if (joystickY >= JOYSTICK_HIGH || joystickY <= JOYSTICK_LOW) {
				last_joystick_time = get_current_time();
			} else {
				last_joystick_time = 0;
			}
		} else {
			joystickX = NO_JOYSTICK_ACTION;
			joystickY = NO_JOYSTICK_ACTION;
		}
		
		// Get keyboard input
		char serial_input = -1;
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
		}
		if (btn == BUTTON0_PUSHED || serial_input == 'd' || serial_input == 'D' || joystickX >= JOYSTICK_HIGH) {
			// If button 0 is pushed, move right, i.e increase x by 1 and leave
			// y the same
			valid_move_made = move_player(1, 0);
			if (is_game_won()) {
				break;
			}
			if (check_diamond() == 1) {
				diamondCount ++;
				updateInfo(cheatMode);
			}
		} if ((btn == BUTTON1_PUSHED || serial_input == 's' || serial_input == 'S' || joystickY >= JOYSTICK_HIGH) && valid_move_made == 0) {
			// move down
			valid_move_made = move_player(0, -1);
			if (is_game_won()) {
				break; // CHANGE THIS TO A BREAK
			}
			if (check_diamond() == 1) {
				diamondCount ++;
				updateInfo(cheatMode);
			}
		} if ((btn == BUTTON2_PUSHED || serial_input == 'w' || serial_input == 'W' || joystickY <= JOYSTICK_LOW) && valid_move_made == 0){
			// move up
			valid_move_made = move_player(0, 1);
			if (is_game_won()) {
				break;
			}
			if (check_diamond() == 1) {
				diamondCount ++;
				updateInfo(cheatMode);
			}
		} if ((btn == BUTTON3_PUSHED || serial_input == 'a' || serial_input == 'A' || joystickX <= JOYSTICK_LOW) && valid_move_made == 0) {
			// move left
			valid_move_made = move_player(-1, 0);
			if (is_game_won()) {
				break;
			}
			if (check_diamond() == 1) {
				diamondCount ++;
				updateInfo(cheatMode);
			}
		} else if (serial_input == 'e' || serial_input == 'E') {
			// Inspect wall
			inspect_wall(cheatMode);
		} else if (serial_input == 'c' || serial_input == 'C') {
			cheatMode = !cheatMode;
			updateInfo(cheatMode);
		} else if (serial_input == ' ') {
			if (place_bomb() == 1) {
				bomb_time = get_current_time() + 2000; // set bomb time to 2 secs from now
			}
		} else if (serial_input == 'p' || serial_input == 'P') {
			uint8_t paused_bomb_time = bomb_time - get_current_time();
			uint8_t paused_last_diamond_flash_time = get_current_time() - last_diamond_flash_time;
			if (is_muted() != 1) {
				toggle_sound();
			}
			while (serial_input == 'p' || serial_input == 'P') {
				serial_input = -1;
				if (serial_input_available()) {
					serial_input = fgetc(stdin);
				}
			}
			while (serial_input != 'p' && serial_input != 'P') {
				serial_input = -1;
				if (serial_input_available()) {
					serial_input = fgetc(stdin);
				}
			}

			while (serial_input == 'p' || serial_input == 'P') {
				serial_input = -1;
				if (serial_input_available()) {
					serial_input = fgetc(stdin);
				}
			}
			bomb_time = get_current_time() + paused_bomb_time;
			last_diamond_flash_time = get_current_time() - paused_last_diamond_flash_time;
		} else if (serial_input == 'f' || serial_input == 'F') {
			toggle_field_of_vision();
		} else if (serial_input == 'm' || serial_input == 'M') {
			toggle_sound();
		}

	

		current_time = get_current_time();
		if(current_time >= last_flash_time + 500) {
			// 500ms (0.5 second) has passed since the last time we
			// flashed the cursor, so flash the cursor
			flash_facing();
			
			// Update the most recent time the cursor was flashed
			last_flash_time = current_time;
		}
		
		// Flash for diamond distance when cheat mode is on
		if(diamondDistance != -1 && cheatMode == 1) {
			// Flash speed at 250 * the diamond distance (note it is 125 here as we toggle pin on and off so full period is 250)
			if (current_time >= last_diamond_flash_time + 125 * diamond_distance()) {
				PORTA ^= (1 << PORTA7); // toggle A7 pin
				
				// Update the most recent time the cursor was flashed
				last_diamond_flash_time = current_time;
			}
			
		}
		
		// Check if there is a bomb active
		if(bomb_active()) {
			if (in_danger()) {
				PORTA |= (1 << PORTA5); // turn on A5 pin
				} else {
				PORTA &= ~(1 << PORTA5); // turn off A5 pin
			}
			if (current_time >= bomb_time) {
				blow_bomb();
				play_blow_bomb();
			}
			if (current_time >= bomb_time + 50) {
				bomb_animation_middle();
			}
			if (current_time >= bomb_time + 100) {
				bomb_animation_end();
				// reset bomb flash speed
				bomb_flash_interval = 600;
				bomb_time = NO_BOMB;
			}
			if (current_time >= bomb_time - bomb_flash_interval) {
				if (bomb_flash_interval > 75) {
					bomb_flash_interval /= 1.5;
				}
				flash_bomb();
			}
		}
		if (firstLoop) {
			play_start_game();
			firstLoop = 0;
		}

	}
	// We get here if the game is over.
}

void handle_game_over() {
	clear_terminal();
	move_terminal_cursor(10,14);
	printf_P(PSTR("GAME OVER"));
	move_terminal_cursor(10,15);
	printf_P(PSTR("Press a button to start again"));
	play_game_over();
	uint32_t current_time = get_current_time();
	uint32_t time_since_end = current_time;
	while(button_pushed() == NO_BUTTON_PUSHED) {
		current_time = get_current_time();
		if (current_time >= time_since_end + 50) {
			bomb_animation_middle();
		}
		if (current_time >= time_since_end + 100) {
			bomb_animation_end();
		}
	}
	new_game();
}

/*
 * Updates visible info (e.g. cheat mode enabled, distance, diamond count, etc)
 */
void updateInfo(uint8_t cheatMode) {
		// Update terminal info
		clear_terminal();
		move_terminal_cursor(10,10);
		if (cheatMode == 1) {
			printf_P(PSTR("CHEATMODE ENABLED"));
		} else {
			printf_P(PSTR("CHEATMODE DISABLED"));
			PORTA &= ~(1 << PORTA7); // turn A7 pin off
		}
		if (diamond_distance() == -1) {				// for case where no diamonds on map but cheat mode is on
			PORTA &= ~(1 << PORTA7); // turn A7 pin off
		}
			
		move_terminal_cursor(10,12);
		printf_P(PSTR("Diamond Count %d"), diamondCount);
		diamondDistance = diamond_distance();
}

/*
 * Sets up SSD pins
 * MAYBE CHANGE INTO SETUP ALL PINS IN HERE??
 */
void setUpPins() {
	// Set A pins to be outputs for LEDs and CC control for SSD and JOYSTICK CONTROL
	// A7 is for LED steps blinker, A6 is for CC, A5 is for bomb danger LED, A0 is for U/D, A1 is for L/R
	DDRA = (1 << DDRA5) | (1 << DDRA6) | (1 << DDRA7);
	// Set C pins to be outputs for SSD
	DDRC = 0xFF;
	
	// SETUP FOR JOYSTICK. COPY AND PASTED FROM LAB 16
	// Set up ADC - AVCC reference, right adjust
	// Input selection doesn't matter yet - we'll swap this around in the while
	// loop below.
	ADMUX = (1<<REFS0);
	// Turn on the ADC (but don't start a conversion yet). Choose a clock
	// divider of 64. (The ADC clock must be somewhere
	// between 50kHz and 200kHz. We will divide our 8MHz clock by 64
	// to give us 125kHz.)
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);
}

/*
 * Some sort of direction thing. Currently using polling. May need to use interrupts instead.
 */ 
uint16_t joystickDirX() {
	ADMUX &= ~1; //turn off first bit
	// Start the ADC conversion
	ADCSRA |= (1<<ADSC);
			
	while(ADCSRA & (1<<ADSC)) {
		; /* Wait until conversion finished */
	}
	return ADC;
}

/*
 * Some sort of direction thing. Currently using polling. May need to use interrupts instead.
 */ 
uint16_t joystickDirY() {
	ADMUX |= 1;		//turn on first bit
	// Start the ADC conversion
	ADCSRA |= (1<<ADSC);
			
	while(ADCSRA & (1<<ADSC)) {
		; /* Wait until conversion finished */
	}
	return ADC;
}



/*
 * Goes to next level
 */
void nextLevel() {
	level ^= 1;
	new_game();
}