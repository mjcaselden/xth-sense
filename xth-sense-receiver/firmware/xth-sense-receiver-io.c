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


#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include "xth-sense-receiver-io.h"


void init_pwm(uint16_t pwm_clk_prescalar, unsigned char compare_reg)
{
	// First disable global interrupts (temporarily) while we're filling these registers:
	cli();
	
	if(compare_reg == 'b')
	{
		
		// Fast PWM Mode where TOP = 0xFF (ie the 255 max of the 8-bit counter TCNT0)
		// Set output (OC0B) to high when counter (TCNT0) hits TOP (0xFF)
		// Clear output (OC0B) to low when counter (TCNT0) equals compare (OCR0B)
		
		TCCR0A = 0x23;	// 0010	0011
		
		// Set PD5 as output pin (to send output of PWM ie OC0B)
		
		DDRD |= (1 << PD5);
		
		// Set our initial 50% duty cycle with the output comparison register (OCR0B)
		// Our 8-bit counter ranges 0-255, so we must select OCR0A values within that range:

		OCR0B = 127;

	}
		
	else // assume compare_reg == 'a' (this will also catch any invalid character values)
	{
		// Fast PWM Mode where TOP = 0xFF (ie the 255 max of the 8-bit counter TCNT0)
		// Set output (OC0A) to high when counter (TCNT0) hits TOP (0xFF)
		// Clear output (OC0A) to low when counter (TCNT0) equals compare (OCR0A)
			
		TCCR0A = 0x83;	// 1000	0011
		
		// Set PD6 as output pin (to send output of PWM ie OC0A)
				
		DDRD |= (1 << PD6);
				
		// Set our initial 50% duty cycle with the output comparison register (OCR0A)
		// Our 8-bit counter ranges 0-255, so we must select OCR0A values within that range:

		OCR0A = 127;
		
	} // end else	

			
	// Before prescaling, our PWM frequency will be equal to
	// the system clock divided by resolution of our 8-bit counter, ie:
	// pre-scalar Fpwm = sysclk/256
			
	// So final Fpwm = (sysclk/256)/pwm_clk_prescalar

	if(pwm_clk_prescalar == 1)
	TCCR0B = 0x01;	// 0000	0001	prescalar = 1
			
	else
	{ 
		if(pwm_clk_prescalar == 8)
		TCCR0B = 0x02;	// 0000	0010	prescalar = 8
		else
		{
			if(pwm_clk_prescalar == 64)
			TCCR0B = 0x03;	// 0000	0011	prescalar = 64
			else
			{
				if(pwm_clk_prescalar == 256)
				TCCR0B = 0x04;	// 0000	0100	prescalar = 256
				else
				{
					if(pwm_clk_prescalar == 1024)
					TCCR0B = 0x05;	// 0000	0101	prescalar = 1024
					else
					TCCR0B = 0x01;	// 0000	0001	prescalar = 1
				}
			}
		}
	}
	// This last one is in case a user enters an invalid parameter value;
	// we can't exit with a printf warning so this is next best solution...
	// This should really be a switch case but it runs fine so I'm leaving it for now


	// Initialize our counter to zero...

	TCNT0 = 0x00;
	
	// Lastly re-enable global interrupts:
	sei();

} // end init_pwm()


// Before calling this function, be sure to set_sleep_mode() using the def from <avr/sleep.h>
// If we're doing pwm, use:					SLEEP_MODE_IDLE
// If we're doing only ADC, look into:		SLEEP_MODE_ADC
void go_to_sleep()					
{
	sleep_enable();
	// sleep_bod_disable();					// This function does not compile for some AVR processors
	sei();
	sleep_cpu();
	sleep_disable();
}


void init_adc(uint8_t adc_clk_prescalar)
{
	// Disable all global interrupts while we're setting up:
	cli();
	
	//The first step is to set the prescalar for the ADC clock frequency:
	
	ADCSRA &= ~( (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) ) ;	// clear the values initially
	
	
	switch(adc_clk_prescalar)									// then set the proper bits high
	{
		case 2:	
			ADCSRA |= (1 << ADPS0);
			break;
		
		case 4:	
			ADCSRA |= (1 << ADPS1);
			break;
	
		case 8: 
			ADCSRA |= ( (1 << ADPS0) | (1 << ADPS1) );
			break;
		
		case 16: 
			ADCSRA |= (1 << ADPS2);
			break;
		
		case 32:
			ADCSRA |= ( (1 << ADPS2) | (1 << ADPS0) );
			break;
		
		case 64:
		   ADCSRA |= ( (1 << ADPS2) | (1 << ADPS1) );
		   break;
	   
		case 128:
			ADCSRA |= ( (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) );
			break;
	
		default:
			ADCSRA |= (1 << ADPS0);
			break;
			
	} // end switch()	

	
	// Set reference voltage to AVcc, and configure ADC0 for action!
	ADMUX = 0x40;									// ADMUX = 0100 0000;

	// ADMUX |= (1 << ADLAR); 						// Left adjust ADC result to allow easy 8 bit reading

	ADCSRA |= (1 << ADATE);  						// Set ADC to Free-Running Mode 
													// (we can do this because we just use one ADC 
													// channel consistently)
	
	ADCSRA |= (1 << ADIE); 							// Enable ADC interrupt

	ADCSRA |= (1 << ADEN);  						// Enable the ADC
	ADCSRA |= (1 << ADSC);  						// Start A2D Conversions
	
	sei();											// Enable global interrupts
	
} // end init_adc()