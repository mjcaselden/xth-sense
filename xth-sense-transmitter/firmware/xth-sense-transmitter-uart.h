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


// WARNING: to use this library, be sure to specify F_CPU (system clock frequency [post-prescale divison])
// it's defined a few lines down from here:

#ifndef XTH_SENSE_TRANSMITTER_UART_H_
#define XTH_SENSE_TRANSMITTER_UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define nop()  __asm__ __volatile__("nop")

#define F_CPU 1000000UL
#define BAUD 9600
#define PRESCALE F_CPU/16/BAUD-1

void init_uart_transmit();

void send_byte(uint8_t outgoing_Byte);

#endif /* XTH_SENSE_TRANSMITTER_UART_H_ */




