/* onewire.c - a part of avr-ds18b20 library
 *
 * Copyright (C) 2016 Jacek Wieczorek
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.	See the LICENSE file for details.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>

#include "../include/ds18b20/onewire.h"

uint8_t onewireInit( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask )
{
	//Init one wire bus (it's basically reset pulse)

	uint8_t response = 0;
	uint8_t sreg = SREG; //Store status register

	cli( ); //Disable interrupts

	*port |= mask; //Write 1 to output
	*direction |= mask; //Set port to output
	*port &= ~mask; //Write 0 to output

	_delay_us( 600 );

	*direction &= ~mask; //Set port to input

	_delay_us( 100 );

	response = *portin & mask; //Read input

	_delay_us( 200 );

	*port |= mask; //Write 1 to output
	*direction |= mask; //Set port to output

	_delay_us( 600 );

	SREG = sreg; //Restore status register

	return response != 0 ? ONEWIRE_ERROR_COMM : ONEWIRE_ERROR_OK;
}

inline uint8_t onewireWriteBit( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t bit )
{
	uint8_t sreg = SREG;

	cli( );

	*port |= mask; //Write 1 to output
	*direction |= mask;
	*port &= ~mask; //Write 0 to output

	if ( bit != 0 ) _delay_us( 8 );
	else _delay_us( 80 );

	*port |= mask;

	if ( bit != 0 ) _delay_us( 80 );
	else _delay_us( 2 );

	SREG = sreg;

	return bit != 0;
}

void onewireWrite( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t data )
{
	//Write byte to one wire bus

	uint8_t sreg = SREG; //Store status register
	uint8_t i = 0;

	cli( );

	for ( i = 1; i != 0; i <<= 1 ) //Write byte in 8 single bit writes
		onewireWriteBit( port, direction, portin, mask, data & i );

	SREG = sreg;
}

inline uint8_t onewireReadBit( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask )
{
	uint8_t bit = 0;
	uint8_t sreg = SREG;

	cli( );
	*port |= mask; //Write 1 to output
	*direction |= mask;
	*port &= ~mask; //Write 0 to output
	_delay_us( 2 );
	*direction &= ~mask; //Set port to input
	_delay_us( 5 );
	bit = ( ( *portin & mask ) != 0 ); //Read input
	_delay_us( 60 );
	SREG = sreg;

	return bit;
}

uint8_t onewireRead( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask )
{
	//Read byte from one wire data bus

	uint8_t sreg = SREG; //Store status register
	uint8_t data = 0;
	uint8_t i = 0;

	cli( ); //Disable interrupts

	for ( i = 1; i != 0; i <<= 1 ) //Read byte in 8 single bit reads
		data |= onewireReadBit( port, direction, portin, mask ) * i;

	SREG = sreg;

	return data;
}
