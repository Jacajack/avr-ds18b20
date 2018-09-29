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
#include <stddef.h>
#include <ds18b20/onewire.h>
#include <ds18b20/ds18b20.h>
#include <ds18b20/romsearch.h>

//! Searches for connected sensors
uint8_t ds18b20search( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *romcnt, uint8_t *roms, uint16_t buflen )
{
	uint8_t i, bit, currom = 0;
	uint64_t junction = 0;
	uint8_t sreg = SREG;

	//romcnt is crucial
	if ( romcnt == NULL ) return DS18B20_ERROR_OTHER;

	#ifdef DS18B20_AUTO_CLI
		cli( );
	#endif

	// 1 loop - 1 thermometer discovered
	do
	{
		// Initiate ROM search
		if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM )
		{
			*romcnt = 0;
			SREG = sreg;
			return DS18B20_ERROR_COMM;
		}
		onewireWrite( port, direction, portin, mask, DS18B20_COMMAND_SEARCH_ROM );

		// Access 64 bits of ROM
		for ( i = 0; i < 64; i++ )
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
					bit = bit & 1;
					break;

				//Received 00 - ROM bits differ
				case 0:
					// Check if there are older junction bits set
					// If there are bits older than i set, the junction value shifted
					// By i+1 bits to the right will be nonzero
					if ( junction >> ( i + 1 ) )
					{
						// Send complement of junction bit
						bit = !( junction & ( 1 << i ) );
					}
					else
					{
						// Send value of junction bit and toggle it afterwards
						bit = ( junction & ( 1 << i ) );
						if ( bit )
							junction &= ~( 1 << i );
						else
							junction |= ( 1 << i );
					}
					break;
			}

			// Send response bit depending on junction status
			// At this point bit is either 0 or 1 and corresponds
			// to the discovered ROM value
			onewireWriteBit( port, direction, portin, mask, bit );

			// Write discovered bit to the ROM array
			// Or just pretend we did just to update ROM counter
			if ( roms != NULL )
			{
				// Byte index in the ROM array
				uint16_t bytenum = ( currom << 3 ) + ( i >> 3 );

				// Check if ROM buffer can be written
				if ( bytenum < buflen )
				{
					// Write ith bit in the array
					if ( bit != 0 )
						roms[bytenum] |= ( 1 << ( i & 7 ) );
					else
						roms[bytenum] &= ~( 1 << ( i & 7 ) );
				}
			}
		}
	}
	while ( ++currom && junction ); // As long as there are junction bits set

	*romcnt = currom;
	SREG = sreg;
	if ( currom == 0 ) return DS18B20_ERROR_COMM; // Exit because of currom overflow (junction broken?)
	return DS18B20_ERROR_OK;
}
