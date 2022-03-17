/*
 * timer1.c
 *
 * Created: 28/10/2021 10:59:12 AM
 *  Author: MatthewChen
 */ 

#include <avr/io.h>
#define F_CPU 8000000UL	// 8MHz
#define DEFAULT_DC 50
#define DEFAULT_TIME 500

#include <util/delay.h>
#include <stdint.h>
#include "timer1.h"
#include "timer0.h"

// Global variables
uint16_t freq;
float dutycycle;
uint16_t clockperiod;
uint16_t pulsewidth;
uint8_t muted;



// (FROM LAB 14-2)
uint16_t freq_to_clock_period(uint16_t freq) {
	return (1000000UL / freq);
								
}

// (FROM LAB 14-2)
uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod) {
	return (dutycycle * clockperiod) / 100;
}

void init_timer1() {
	muted = 0;
	freq = 500;	// Hz
	dutycycle = 20;	// %
	clockperiod = freq_to_clock_period(freq);
	pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);
	
	// Set the maximum count value for timer/counter 1 to be one less than the clockperiod
	OCR1A = clockperiod - 1;
	
	// Set the count compare value based on the pulse width. The value will be 1 less
	// than the pulse width - unless the pulse width is 0.
	if(pulsewidth == 0) {
		OCR1B = 0;
	} else {
		OCR1B = pulsewidth - 1;
	}
	
	// Set up timer/counter 1 for Fast PWM, counting from 0 to the value in OCR1A
	// before reseting to 0. Count at 1MHz (CLK/8).
	// Configure output OC1B to be clear on compare match and set on timer/counter
	// overflow (non-inverting mode).
	sound_off();
}


void toggle_sound() {
	if (muted == 1) {
		muted = 0;
	} else {
		muted = 1;
		sound_off();
	}
}

void sound_off() {
	// Sets port to normal operation, OC1B disconnected
	TCCR1A = 0;
	TCCR1B = 0;
	
	// Port must also be turned back to input (to prevent static sound)
	DDRD &= ~(1<<4);
}

void sound_on() {
	if (muted) {
		return;
	}
	// Sets OC1b to be clear on compare match and set on timer/counter overflow
	DDRD |= (1<<4);
	TCCR1A = (1 << COM1B1) | (0 <<COM1B0) | (1 <<WGM11) | (1 << WGM10);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (0 << CS10);
}


/*
 * Sets a sound on. Will turn it off.
 * Parameters:
 *		f: frequency (hz)
 *		dc: duty cycle (%) (how loud it is)
 *		time: length of sound (milleseconds)
 */
void set_sound(uint16_t f, uint16_t dc, uint32_t time) {
	
	freq = f;	// Hz
	dutycycle = dc;	// %
	clockperiod = freq_to_clock_period(freq);
	pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);
		
		
	// Set the maximum count value for timer/counter 1 to be one less than the clockperiod
	OCR1A = clockperiod - 1;
		
	// Set the count compare value based on the pulse width. The value will be 1 less
	// than the pulse width - unless the pulse width is 0.
	if(pulsewidth == 0) {
		OCR1B = 0;
		} else {
		OCR1B = pulsewidth - 1;
	}
	sound_on();
	time_till_sound_off(time);
}

/*
 * Plays a sound but will not turn it off. 
 * Parameters:
 *		f: frequency (hz)
 *		dc: duty cycle (%) (how loud it is)
 *		
 */
void play_sound(uint16_t f, uint16_t dc) {
	
	freq = f;	// Hz
	dutycycle = dc;	// %
	clockperiod = freq_to_clock_period(freq);
	pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);
		
		
	// Set the maximum count value for timer/counter 1 to be one less than the clockperiod
	OCR1A = clockperiod - 1;
		
	// Set the count compare value based on the pulse width. The value will be 1 less
	// than the pulse width - unless the pulse width is 0.
	if(pulsewidth == 0) {
		OCR1B = 0;
		} else {
		OCR1B = pulsewidth - 1;
	}
	sound_on();
}

uint8_t is_muted() {
	return muted;
}

void play_A() {
	play_sound(440, DEFAULT_DC);
}

void play_B() {
	play_sound(494, DEFAULT_DC);
}

void play_C(){
	play_sound(262, DEFAULT_DC);
}

void play_D(){
	play_sound(293, DEFAULT_DC);
}

void play_E(){
	play_sound(330, DEFAULT_DC);
}

void play_F(){
	play_sound(349, DEFAULT_DC);
}

void play_G(){
	play_sound(391, DEFAULT_DC);
}

