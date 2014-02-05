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


#include "xth-sense-receiver-uart.h"

void init_uart_receive()
{

	// Temporarily disable global interrupts while we're setting up here
	cli();
 
	// Turn on the reception circuitry
	UCSR0B |= (1 << RXEN0);  

	// Enable the USART Receive Complete interrupt (USART_RXC)
	UCSR0B |= (1 << RXCIE0);

	// Asynchronous, N-8-1 (No parity bits, 8 bits per character, 1 stop bit)
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

	// Set BAUD rate by filling high and low bytes of the baud rate register
	UBRR0H = (PRESCALE >> 8); 
	UBRR0L = PRESCALE; 

	// Re-enable global interrupts
	sei(); 
}


// This interrupt handler is called whenever USART has received a byte. We're basically using
// byte_From_UART as a 1-byte long circular buffer here... the read_byte() function will pass
// the value out to the rest of functions in main()
ISR(USART_RX_vect)
{	
	// Stash recently received byte from our UART register
	byte_From_UART = UDR0;

	// Reset this variable to zero. This variable is used in timer1's interrupt
	// to watch how much time has passed between received UART packets. IE a way of watching for packet drops
	tim1_Interrupts_Since_Packet_In = 0;
}


// This function just returns the byte that was most recently received from UART
void read_byte(uint8_t *incoming_Byte)
{	
	*incoming_Byte = byte_From_UART;
}


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


// This interrupt will occur at a rate of 658ish Hz (so we know that if it occurs 3X without
// tim1_Interrupts_Since_Packet_In being reset to zero by a read, a packet has been dropped)
// This was used for debuggign in the past and may need to be updated before re-use
ISR (TIMER1_COMPA_vect)
{	
	tim1_Interrupts_Since_Packet_In++;
	if(tim1_Interrupts_Since_Packet_In >= 3)
		// Timeout Error: a packet has been dropped!!
		packets_Dropped++;

	// This line just to reset the variable if it overflows
	if(packets_Dropped > 10000000)
		packets_Dropped = 0;
}


init_timer1()
{

	// Drive this timer with:  Sysclk/8 = 125 kHz clock
	TCCR1B |= (1 << CS11);

	// Set to CTC Mode (clear counter when timer reaches OCR1A)
	TCCR1B |= (1 << WGM12);

	// initialize counter
	TCNT1 = 0;

	// Timer1 compare value = 189, so counts 0-189
	// So interrupts occur at 125 kHz/190 = 658 Hz
	OCR1A = 189;

	// enable overflow interrupt
	TIMSK1 |= (1 << OCIE1A);

	// enable global interrupts
	sei();
}
