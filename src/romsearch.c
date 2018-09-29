/* romsearch.c - a part of avr-ds18b20 library
 *
 * Copyright (C) 2016 Jacek Wieczorek
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

/**
	\file
	\brief Implements functions for searching for connected sensors
	\todo Optimizations and improvements
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <string.h>
#include <stddef.h>
#include <ds18b20/onewire.h>
#include <ds18b20/ds18b20.h>
#include <ds18b20/romsearch.h>

//! Searches for connected sensors
uint8_t ds18b20search( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *romcnt, uint8_t *roms, uint16_t buflen )
{
	uint8_t bit, currom = 0;
	uint64_t i, junction = 0, rom;
	uint8_t sreg = SREG;

	//romcnt is crucial
	if ( romcnt == NULL ) return DS18B20_ERROR_OTHER;

	#ifdef DS18B20_AUTO_CLI
		cli( );
	#endif

	// 1 loop - 1 thermometer discovered
	do
	{
		// Reset current ROM buffer
		rom = 0;

		// Initiate ROM search
		if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM )
		{
			*romcnt = 0;
			SREG = sreg;
			return DS18B20_ERROR_COMM;
		}
		onewireWrite( port, direction, portin, mask, DS18B20_COMMAND_SEARCH_ROM );

		// Access 64 bits of ROM
		for ( i = 1; i; i <<= 1 )
		{
			//Request two complementary bits from sensors
			bit = 0;
			bit |= onewireReadBit( port, direction, portin, mask );
			bit |= onewireReadBit( port, direction, portin, mask ) << 1;

			switch ( bit )
			{
				//Received 11 - no sensors connected
				case 3:
					*romcnt = 0; //Null pointer check is at the begining
					SREG = sreg;
					return DS18B20_ERROR_COMM;
					break;

				//Received 10 or 01 - ROM bits match
				case 1:
				case 2:
					bit &= 1;
					break;

				//Received 00 - ROM bits differ
				case 0:
					// Check if there are older junction bits set
					// If there are older bits set, junction value
					// shall be at least equal to i*2
					if ( junction >= ( i << 1 ) )
					// if ( junction >= ( 2 << i ) )
					{
						// Send complement of junction bit
						bit = !( junction & i );
					}
					else
					{
						// Send value of junction bit and toggle it afterwards
						bit = ( junction & i ) != 0;
						junction ^= i;
					}
					break;
			}

			// Send response bit depending on junction status
			// At this point bit is either 0 or 1 and corresponds
			// to the discovered ROM value
			onewireWriteBit( port, direction, portin, mask, bit );

			// Set bit in temporary ROM buffer
			rom |= bit ? i : 0;
		}

		// Copy prepared ROM to its destination
		if ( roms != NULL && ( currom + 1 ) << 3 <= buflen )
			memcpy( roms + ( currom << 3 ), &rom, 8 );
	}
	while ( ++currom && junction ); // As long as there are junction bits set

	*romcnt = currom;
	SREG = sreg;
	if ( currom == 0 ) return DS18B20_ERROR_COMM; // Exit because of currom overflow (junction broken?)
	return DS18B20_ERROR_OK;
}
