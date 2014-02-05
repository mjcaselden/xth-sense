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


#ifndef XTH_SENSE_TRANSMITTER_IO_H_
#define XTH_SENSE_TRANSMITTER_IO_H_

#include "stdint.h"


/*
	init_pwm() uses Timer0 for Fast PWM output
*/	
	void init_pwm(uint16_t pwm_clk_prescalar, unsigned char compare_reg);



/*
	If using init_adc(), you MUST include in your main() both the avr interrupt 
	library (avr/interrupt.h), and also a definition of ISR(ADC_vect).

	The value of the ADC clock = sysclock/adc_clk_prescalar
	If you wish to use the full 10-bit range of resolution, Atmel recommends
	keeping the ADC clock between 50 kHz and 200 kHz
	
	Sample rate = ADC clock/13.5 = (sysclock/adc_clk_prescalar)/13.5
*/	
	void init_adc(uint8_t adc_clk_prescalar);



/*
	Before calling go_to_sleep(), be sure to set_sleep_mode() using the def from <avr/sleep.h>
	If we're doing pwm, use:					SLEEP_MODE_IDLE
	If we're doing only ADC, look into:		SLEEP_MODE_ADC
*/
	void go_to_sleep();




#endif  // XTH_SENSE_TRANSMITTER_IO_H_


