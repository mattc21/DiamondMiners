/*
 * timer1.h
 *
 * Created: 28/10/2021 10:59:35 AM
 *  Author: MatthewChen
 * For buzzing the buzzer.
 * Tips to using
 * Apart from initiating the timer, if you want to set a sound, use
 * set_sound(). 
 * timer1 relies on timer0 (though unless you're modifying timer1, you don't need to worry about this.)
 * I have provided some functions to make specific pitches (so you can play simple music in C major or A natural minor)
 * as well as some specific jingles. The jingles are found in timer1.c not here though...
 */ 


#ifndef TIMER1_H_
#define TIMER1_H_

#define NO_NOTE 0
#define NOTE_C 1
#define NOTE_D 2
#define NOTE_E 3
#define NOTE_F 4
#define NOTE_G 5
#define NOTE_A 6
#define NOTE_B 7

/* Set up our timer to output compare match
 * and update our time reference.
 */
void init_timer1();

void sound_off();

void sound_on();


/*
 * toggles sound on and off
 */
void toggle_sound();


/*
 * Sets a sound on. 
 * Parameters:
 *		f: frequency (hz)
 *		dc: duty cycle (%) (how loud it is)
 *		time: length of sound (milleseconds)
 */
void set_sound(uint16_t f, uint16_t dc, uint32_t time);


/*
 * Return if sound is muted.
 */
uint8_t is_muted();

void play_A();

void play_B();

void play_C();

void play_D();

void play_E();

void play_F();

void play_G();


#endif /* TIMER1_H_ */