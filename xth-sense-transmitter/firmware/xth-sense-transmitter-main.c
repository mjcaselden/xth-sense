/*

	Copyright © 2014 M. J. Caselden and littleBits Electronics Inc.

	This file is part of Xth Sense Transmitter.

	Xth Sense Transmitter is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Xth Sense Transmitter is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Xth Sense Transmitter.  If not, see <http://www.gnu.org/licenses/>.

	You can contact M. J. Caselden at this email address: mjcaselden@gmail.com

*/


//	For ATmega328 
//	Sysclock = 1 MHz (internal 8 MHz RC oscillator, divided by 8 in fuses) 


/*	Overview:
	1)	ADC readings via interrupt at approximately 4.6 kHz sample rate
	2)	In each ADC interrupt, update the value stored in the global variable adc_val
	3)	Meanwhile, timer2 interrupts at roughly 960 Hz (same as in wireless Receiver)
		On timer2 interrupt: pass out current value adc_val to the serial port...
*/

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

#define nop()  __asm__ __volatile__("nop")

void init_timer2();

// These were just used as watch variables in debugging
static volatile uint8_t adc_val = 0;
static volatile uint8_t byte_Read = 0;
static volatile uint8_t read_Status = 0;

int main(void)
{
	init_uart_transmit();
	
	init_timer2();
	
	// Initialize ADC with clock frequency of 1/16th of sysclock (so 62.5 kHz)
	// A given ADC reading takes 13.5 cycles, so the ADC will sample at a rate of 4.6 kHz. Scope verified!
	init_adc(16);
	
	DDRD |= (1 << PD2);
	
    while(1)
    {
		// Implement go_to_sleep() here later on...
		PORTD &= ~(1 << PD2);
    }

}


// This interrupt will occur at a rate of roughly 960 Hz. Scope verified!!
ISR (TIMER2_COMPA_vect)
{
	PORTD |= (1 << PD2);
	send_byte(adc_val);
}	


ISR(ADC_vect)
{
	adc_val = (ADC >> 2);
}


// Set up Timer2 to generate interrupts at a rate of 960 Hz
void init_timer2()
{
	// Temporarily disable global interrupts while we're setting up the timer
	cli();

	// Set to Clear Timer on Compare Mode (CTC Mode)
	// Once this timer has counted up to the value stored in OCR2A, it will reset to zero
	TCCR2A |= (1 << WGM21);

	// Drive this timer with:  Sysclk/8 = 125 kHz clock
	TCCR2B |= (1 << CS21);
	
	// Timer will count from 0-129, for a total of 130 clocks. That means the timer will
	// max out and cause an interrupt at a rate of 960 Hz (because 125 kHz clock/130 = 960 Hz)
	OCR2A = 129;
	
	
	// Enable interrupt ISR(TIMER2_COMPA_vect) when timer2 value == OCR2A value
	TIMSK2 |= (1 << OCIE2A);

	
	// Re-enable global interrupts
	sei();

}
