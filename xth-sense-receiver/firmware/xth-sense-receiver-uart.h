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



// WARNING: to use this library, be sure to specify F_CPU (system clock frequency [post-prescale divison])
// it's defined a few lines down from here:

#ifndef XTH_SENSE_RECEIVER_UART_H_
#define XTH_SENSE_RECEIVER_UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

/////////////////Included for Debugging Purposes/////////////////
volatile uint32_t bytes_Received = 0;
static volatile uint8_t	tim1_Interrupts_Since_Packet_In = 0;
static volatile uint8_t packets_Dropped = 0;

// Set up Timer1 to generate interrupts at a rate of roughly 1000 Hz
// This timer will be reset every time a byte is received as input to the UART
// If it maxes out, an interrupt will occur to show that a timeout has occurred.
init_timer1();

/////////////////////////////////////////////////////////////////

#define nop()  __asm__ __volatile__("nop")

#define F_CPU 1000000UL
#define BAUD 9600
#define PRESCALE F_CPU/16/BAUD-1

// this variable will receive bytes from UART and later pass the values out through read_byte();
static volatile uint8_t byte_From_UART = 0;

void init_uart_receive();
void init_uart_transmit();

void read_byte(uint8_t *incoming_Byte);
void send_byte(uint8_t outgoing_Byte);


// This function will be called automatically behind the scenes to fill the input buffer (receiving_Buffer)
ISR(USART_RX_vect);


#endif /* XTH_SENSE_RECEIVER_UART_H_ */
