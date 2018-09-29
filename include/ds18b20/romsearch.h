/* romsearch.h - a part of avr-ds18b20 library
 *
 * Copyright (C) 2016 Jacek Wieczorek
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

/**
	\file
	\brief DS18B20 searching function
*/

#ifndef DS18B20_ROMSEARCH_H
#define DS18B20_ROMSEARCH_H

#include <inttypes.h>

/**
	\brief Performs search for connected DS18B20 sensors.
	Discovered sensors' ROM addresses are returned in an array.

	This function can be used to only discover the number of connected sensors.
	To do so, \ref roms parameter should be set to NULL. As usual, the value
	returned through \ref romcnt will be the sensor count.

	If the total size of discovered ROMs is greater than buffer size, the user shall only
	access ROM data addresses up to biggest multiple of 8 less than buffer size.
	The remaning bytes will contain garbage data and using them may cause UB.
	The value returned though \ref romcnt can be used to used allocate an array
	of proper size, before repeating the call.

	\param port A pointer to the port output register
	\param direction A pointer to the port direction register
	\param portin A pointer to the port input register
	\param mask A bit mask, determining to which pin the sensor is connected
	\param romcnt A pointer to a variable when discovered sensor count will be written
	\param roms A pointer to an array where discovered sensors' ROM addresses shall be stored
	\param buflen The length of the ROM array in bytes
	\returns \ref DS18B20_ERROR_OK on success
*/
extern uint8_t ds18b20search( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *romcnt, uint8_t *roms, uint16_t buflen );

#endif
