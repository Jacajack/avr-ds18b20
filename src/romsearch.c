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

//! Returns nth bit from `arr` array
static inline uint8_t arrbitr( uint8_t *arr, uint8_t n )
{
	if ( arr == NULL ) return 0;
	return ( arr[n >> 3] & ( 1 << ( n & 7 ) ) ) >> ( n & 7 );
}

//! Toggles nth bit in `arr` array
static inline void arrbitt( uint8_t *arr, uint8_t n )
{
	if ( arr == NULL ) return;
	arr[n >> 3] ^= ( 1 << ( n & 7 ) );
}

//! Writes nth bit in `arr` array
static inline void arrbitw( uint8_t *arr, uint8_t n, uint8_t val )
{
	if ( arr == NULL ) return;
	if ( val != 0 )
		arr[n >> 3] |= ( 1 << ( n & 7 ) );
	else
		arr[n >> 3] &= ~( 1 << ( n & 7 ) );
}


//! Checks if there are any bits older than `n` set in `arr` array of `len` bytes length
static inline uint8_t ckolder( uint8_t *arr, uint8_t len, uint16_t n )
{
	uint8_t i, ans = 0, buf = 0;

	if ( arr == NULL ) return 0;

	buf = n >> 3;
	for ( i = len - 1; i > buf; i-- )
		ans |= arr[i];

	ans |= arr[n >> 3] >> ( ( n & 7 ) + 1 );

	return ans != 0;
}

//! Searches for connected sensors
uint8_t ds18b20search( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *romcnt, uint8_t *roms, uint16_t buflen )
{
	uint8_t i, bit, currom = 0;
	uint64_t j;
	uint64_t *jun =  &j;
	uint8_t sreg = SREG;
	uint8_t *junction = (uint8_t*) &j;

	if ( romcnt == NULL ) return DS18B20_ERROR_OTHER;

	#ifdef DS18B20_AUTO_CLI
		cli( );
	#endif

	do
	{
		//Initiate ROM search
		if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM )
		{
			*romcnt = 0;
			SREG = sreg;
			return DS18B20_ERROR_COMM;
		}
		onewireWrite( port, direction, portin, mask, DS18B20_COMMAND_SEARCH_ROM );

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
					//Check if there are older junction bits set
					if ( ckolder( junction, 8, i ) )
					{
						//Send complement of junction bit
						bit = !arrbitr( junction, i );
					}
					else
					{
						//Send value of junction bit and toggle it afterwards
						bit = arrbitr( junction, i );
						arrbitt( junction, i );
					}
					break;
			}

			//Send response bit depending on junction status
			onewireWriteBit( port, direction, portin, mask, bit );

			//Write
			if ( roms != NULL )
			{
				//Check if ROM buffer can be written
				if ( ( currom << 3 ) + ( i >> 3 ) >= buflen )
				{
					*romcnt = 0;
					SREG = sreg;
					return DS18B20_ERROR_OTHER;
				}
				arrbitw( &roms[currom << 3], i, bit );
			}
		}
	} while ( ++currom && *jun );

	*romcnt = currom;
	SREG = sreg;
	if ( currom == 0 ) return DS18B20_ERROR_COMM; //Exit because of currom overflow (junction broken?)

	return DS18B20_ERROR_OK;
}
