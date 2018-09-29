/* ds18b20.c - a part of avr-ds18b20 library
 *
 * Copyright (C) 2016 Jacek Wieczorek
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.	See the LICENSE file for details.
 */

/**
	\file
	\brief Implements functions for controlling the DS18B20 sensors
*/

#include <stddef.h>
#include <util/delay.h>
#include <ds18b20/ds18b20.h>
#include <ds18b20/onewire.h>

//! Calculate CRC of provided data
uint8_t ds18b20crc8( uint8_t *data, uint8_t length )
{
	//Generate 8bit CRC for given data (Maxim/Dallas)

	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t mix = 0;
	uint8_t crc = 0;
	uint8_t byte = 0;

	for ( i = 0; i < length; i++ )
	{
		byte = data[i];

		for( j = 0; j < 8; j++ )
		{
			mix = ( crc ^ byte ) & 0x01;
			crc >>= 1;
			if ( mix ) crc ^= 0x8C;
			byte >>= 1;
		}
	}
	return crc;
}

//! Perform ROM matching
void ds18b20match( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom )
{
	//Perform ROM match operation on DS18B20 devices
	//Or skip ROM matching if ptr is NULL

	uint8_t i = 0;

	//If rom pointer is NULL then read temperature without matching.
	if ( rom == NULL )
	{
		//Skip ROM
		onewireWrite( port, direction, portin, mask, DS18B20_COMMAND_SKIP_ROM );
	}
	else
	{
		//Match ROM
		onewireWrite( port, direction, portin, mask, DS18B20_COMMAND_MATCH_ROM );
		for ( i = 0; i < 8; i++ )
			onewireWrite( port, direction, portin, mask, rom[i] );
	}
}

//! Request temperature conversion
uint8_t ds18b20convert( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom )
{
	//Send conversion request to DS18B20 on one wire bus

	//Communication check
	if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM )
		return DS18B20_ERROR_COMM;

	//ROM match (or not)
	ds18b20match( port, direction, portin, mask, rom );

	//Convert temperature
	onewireWrite( port, direction, portin, mask, DS18B20_COMMAND_CONVERT );

	return DS18B20_ERROR_OK;
}

//! Read sensor scratchpad contents
uint8_t ds18b20rsp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, uint8_t *sp )
{
	//Read DS18B20 scratchpad

	uint8_t i = 0;

	//Communication check
	if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM )
		return DS18B20_ERROR_COMM;

	//Match (or not) ROM
	ds18b20match( port, direction, portin, mask, rom );

	//Read scratchpad
	onewireWrite( port, direction, portin, mask, DS18B20_COMMAND_READ_SP );
	for ( i = 0; i < 9; i++ )
		sp[i] = onewireRead( port, direction, portin, mask );

	//Check pull-up
	if ( ( sp[0] | sp[1] | sp[2] | sp[3] | sp[4] | sp[5] | sp[6] | sp[7] ) == 0 )
		return DS18B20_ERROR_PULL;

	//CRC check
	if ( ds18b20crc8( sp, 8 ) != sp[8] )
		return DS18B20_ERROR_CRC;

	return DS18B20_ERROR_OK;
}

//! Write sensor scratchpad
uint8_t ds18b20wsp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, uint8_t th, uint8_t tl, uint8_t conf )
{
	//Writes DS18B20 scratchpad
	//th - thermostat high temperature
	//tl - thermostat low temperature
	//conf - configuration byte

	//Communication check
	if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM )
		return DS18B20_ERROR_COMM;

	//ROM match (or not)
	ds18b20match( port, direction, portin, mask, rom );

	//Write scratchpad
	onewireWrite( port, direction, portin, mask, DS18B20_COMMAND_WRITE_SP );
	onewireWrite( port, direction, portin, mask, th );
	onewireWrite( port, direction, portin, mask, tl );
	onewireWrite( port, direction, portin, mask, conf );

	return DS18B20_ERROR_OK;
}

//! Copy scratchpad to EEPROM
uint8_t ds18b20csp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom )
{
	//Copies DS18B20 scratchpad contents to its EEPROM

	//Communication check
	if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM )
		return DS18B20_ERROR_COMM;

	//ROM match (or not)
	ds18b20match( port, direction, portin, mask, rom );

	//Copy scratchpad
	onewireWrite( port, direction, portin, mask, DS18B20_COMMAND_COPY_SP );

	//Set pin high
	//Poor DS18B20 feels better then...
	*port |= mask;
	*direction |= mask;

	return DS18B20_ERROR_OK;
}

//! Read temperature
uint8_t ds18b20read( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, int16_t *temperature )
{
	//Read temperature from DS18B20
	//Note: returns actual temperature * 16

	uint8_t sp[9];
	uint8_t ec = 0;

	//Communication, pull-up, CRC checks happen here
	ec = ds18b20rsp( port, direction, portin, mask, rom, sp );

	if ( ec != DS18B20_ERROR_OK )
	{
		*temperature = 0;
		return ec;
	}

	//Get temperature from received data
	*temperature = (int16_t)( sp[1] << 8 ) + sp[0];

	return DS18B20_ERROR_OK;
}

//! Read ROM address
uint8_t ds18b20rom( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom )
{
	//Read DS18B20 rom

	unsigned char i = 0;

	if ( rom == NULL ) return DS18B20_ERROR_OTHER;

	//Communication check
	if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM )
		return DS18B20_ERROR_COMM;

	//Read ROM
	onewireWrite( port, direction, portin, mask, DS18B20_COMMAND_READ_ROM );
	for ( i = 0; i < 8; i++ )
		rom[i] = onewireRead( port, direction, portin, mask );

	//Pull-up check
	if ( ( rom[0] | rom[1] | rom[2] | rom[3] | rom[4] | rom[5] | rom[6] | rom[7] ) == 0 ) return DS18B20_ERROR_PULL;

	//Check CRC
	if ( ds18b20crc8( rom, 7 ) != rom[7] )
	{
		for ( i = 0; i < 8; i++ ) rom[i] = 0;
		return DS18B20_ERROR_CRC;
	}

	return DS18B20_ERROR_OK;
}
