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

#include "xth-sense-transmitter-uart.h"


void init_uart_transmit()
{

	// Temporarily disable global interrupts while we're setting up here
	cli();

	// Turn on the transmission circuitry
	UCSR0B |= (1 << TXEN0);

	// Asynchronous, N-8-1 (No parity bits, 8 bits per character, 1 stop bit)
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

	// Set BAUD rate by filling high and low bytes of the baud rate register
	UBRR0H = (PRESCALE >> 8); 
	UBRR0L = PRESCALE; 

	// Re-enable global interrupts
	sei();
}


void send_byte(uint8_t outgoing_Byte)
{
	// Wait until the UART is ready to receive another outgoing byte
	while ((UCSR0A & (1<<UDRE0)) == 0);
		
	// Now send out the data byte to the UART
	UDR0 = outgoing_Byte;
}





