/*

	Copyright © 2014 M. J. Caselden and littleBits Electronics Inc.

	This file is part of Xth Sense Receiver.

	Xth Sense Receiver is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Xth Sense Receiver is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Xth Sense Receiver.  If not, see <http://www.gnu.org/licenses/>.

	You can contact M. J. Caselden at this email address: mjcaselden@gmail.com

*/


//	For ATmega328 
//	Sysclock = 1 MHz (internal 8 MHz RC oscillator, divided by 8 in fuses) 


/*	
	Quick overview of the receiving serial code: We basically have a single-byte receiving buffer going on behind the scenes (it's titled 
	"byte_From_UART", and it's defined in xth-sense-receiver-uart.h)
	
	We automatically fill this byte using the ISR(USART_RX_vect) handler, which is called every time that the UART has successfully
	received a byte. 
	
	In main(), we use read_byte() to pull the value out of "byte_From_UART", and send it directly out through our PWM analog output by 
	assigning it to the output comparison value of timer0.
*/


/* A way that we watched for packet timeouts:

	Have a timeout counter (timer1) running in the background all of the time. The timeout counter will be reset 
	every time that a UART packet is received (see UART interrupt handler routine). If the timeout counter 
	reaches its max value (determined by packet timeout duration), then do whatever (turn on LED to show, 
	later on implement some algorithm to replace this value with a linear interpolation)

	Recall that we interrupt on every UART-received byte. Inside interrupt: reset the timeout counter to zero
	
	Please note that we have some version of this code written out in xth-sense-receiver-uart.c, but its timing probably
	doesn't match the sample/transmit rates that we're using now
	
*/


#include <stdint.h>
#include <avr/interrupt.h>

#define nop()  __asm__ __volatile__("nop")

// These static volatile variables are still left in here from some debugging we've been doing
// They're just used as watch variables
static volatile uint8_t received_Byte = 0;
static volatile uint8_t	read_Error = 0;
static volatile uint32_t bytes_Read = 0;



// Set up Timer2 to generate interrupts at a rate of roughly 960 Hz
// This timer will pull samples out of our buffer, and feed them into OCR0A to control PWM output
init_timer2();


int main(void)
{	
	// Recall on the output signal we have a 3rd-order LPF with cutoff freq at approximately 180 Hz
	// Set 8-bit PWM up for 3.9 KHz output:
	init_pwm(1,'a');
	init_uart_receive();
	init_timer1();
	init_timer2();
	
	DDRD |= (1 << PD3);
	

    while(1)
    {	
		PORTD &= ~(1 << PD3);
    } // end while() 
	
}


// This interrupt will occur at a rate of 960ish Hz
ISR (TIMER2_COMPA_vect) 
{	
	// Pull in the new byte from UART
	read_byte(&received_Byte);
	bytes_Read++;
	
	// Set that received byte to output in the PWM comparison register
	OCR0A = received_Byte;
}


// Set up Timer2 to generate interrupts at a rate of 960 Hz
init_timer2()
{
	// Temporarily disable global interrupts while we're setting up the timer
	cli();

	// Set to Clear Timer on Compare Mode (CTC Mode)
	// Once this timer has counted up to the value stored in OCR2A, it will reset to zero
	TCCR2A |= (1 << WGM21);

	// Drive this timer with:  Sysclk/8 = 125 kHz clock
	TCCR2B |= (1 << CS21);
	
	// Timer will count from 0-129, for a total of 130 clocks. That means the timer will
	// max out and cause an interrupt at a rate of roughly 960 Hz (because 125 kHz clock/156 = 960 Hz)
	OCR2A = 129;
	
	// Enable interrupt ISR(TIMER2_COMPA_vect) when timer2 value == OCR2A value
	TIMSK2 |= (1 << OCIE2A);
	
	// Re-enable global interrupts
	sei();

}