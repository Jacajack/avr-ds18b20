/* ds1820.c
 *
 * Copyright (C) 2016 Jacek Wieczorek
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdlib.h>
#include "../include/ds18b20.h"

unsigned char ds1820request( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask )
{
	//Send conversion request to DS1820 on one wire bus

    if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM ) return DS1820_ERROR_COMM;
    onewireWrite( port, direction, portin, mask, 0xCC ); //Command - Skip ROM
    onewireWrite( port, direction, portin, mask, 0x44 ); //Command - Convert

	return DS1820_ERROR_OK;
}

int ds18b20read( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask )
{
	//Read temperature from DS1820
    //Note: returns actual temperature * 16

    unsigned char response[9];
    unsigned char i = 0;
    int temperature;

    if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM ) return DS1820_ERROR_COMM_TEMP; //Check communication

    onewireWrite( port, direction, portin, mask, 0xCC ); //Command - Skip ROM
    onewireWrite( port, direction, portin, mask, 0xBE ); //Command - Read Scratchpad

    for ( i = 0; i < 9; i++ )
        response[i] = onewireRead( port, direction, portin, mask );

    if ( ( response[0] | response[1] | response[2] | response[3] | response[4] | response[5] | response[6] | response[7] ) == 0 )
        return DS1820_ERROR_PULL_TEMP; //Pull-up check

    temperature = (int)( response[1] << 8 ) + ( response[0] & 0xFF ); //Get temperature from received data
    temperature = ds1820crc8( response, 8 ) != response[8] ? DS1820_ERROR_CRC_TEMP : temperature; //CRC check

    return temperature;
}


int ds18b20mread( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom )
{
	//Read temperature from DS1820
    //Note: returns actual temperature * 16

    unsigned char response[9];
    unsigned char i = 0;
    int temperature;

	if ( rom == NULL ) return DS1820_ERROR_OTHER_TEMP; //Check for pointer error
    if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM ) return DS1820_ERROR_COMM_TEMP; //Communication check

    onewireWrite( port, direction, portin, mask, 0x55 ); //Command - Match ROM

    for ( i = 0; i < 8; i++ )
        onewireWrite( port, direction, portin, mask, rom[i] );

    onewireWrite( port, direction, portin, mask, 0xBE ); //Command - Read Scratchpad

    for ( i = 0; i < 9; i++ )
        response[i] = onewireRead( port, direction, portin, mask );

    if ( ( response[0] | response[1] | response[2] | response[3] | response[4] | response[5] | response[6] | response[7] ) == 0 )
        return DS1820_ERROR_PULL_TEMP; //Check pull-up

    temperature = (int)( response[1] << 8 ) + ( response[0] & 0xFF ); //Get temperature from received data
    temperature = ds1820crc8( response, 8 ) != response[8] ? DS1820_ERROR_CRC_TEMP : temperature; //CRC check

    return temperature;
}

unsigned char ds1820rom( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom )
{
	//Read DS1820 rom

    unsigned char i = 0;

	if ( rom == NULL ) return DS1820_ERROR_OTHER;
    if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM )
	{
		for ( i = 0; i < 8; i++ ) rom[i] = 0;
		return DS1820_ERROR_COMM;
	}

    onewireWrite( port, direction, portin, mask, 0x33 ); //Command - Read ROM

    for ( i = 0; i < 8; i++ )
        rom[i] = onewireRead( port, direction, portin, mask );

    if ( ( rom[0] | rom[1] | rom[2] | rom[3] | rom[4] | rom[5] | rom[6] | rom[7] ) == 0 ) return DS1820_ERROR_PULL;

	//Check CRC
	if ( ds1820crc8( rom, 7 ) != rom[7] )
	{
		for ( i = 0; i < 8; i++ ) rom[i] = 0;
		return DS1820_ERROR_CRC;
	}

    return DS1820_ERROR_OK;
}


unsigned char ds1820verify( int temperature )
{
	//Decode errors from functions for reading temperatures

	switch ( temperature )
	{
		case DS1820_ERROR_COMM_TEMP:
			return DS1820_ERROR_COMM;
			break;

		case DS1820_ERROR_CRC_TEMP:
			return DS1820_ERROR_CRC;
			break;

		case DS1820_ERROR_PULL_TEMP:
			return DS1820_ERROR_PULL;
			break;

		case DS1820_ERROR_OTHER_TEMP:
			return DS1820_ERROR_OTHER;
			break;

		default:
			return DS1820_ERROR_OK;
			break;
	}

    return DS1820_ERROR_OK;
}

unsigned char ds1820conf( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t th, uint8_t tl, uint8_t dsconf )
{
	//Set up DS1820 internal confuration
    //th - thermostat high temperature
    //tl - thermostat low temperature
    //dsconf - DS1820 confuration data

    if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM ) return DS1820_ERROR_COMM;
    onewireWrite( port, direction, portin, mask, 0xCC ); //Command - SKIP ROM
    onewireWrite( port, direction, portin, mask, 0x4E ); //Command - Write Scratchpad

    onewireWrite( port, direction, portin, mask, th );
    onewireWrite( port, direction, portin, mask, tl );
    onewireWrite( port, direction, portin, mask, dsconf );

    return DS1820_ERROR_OK;
}

unsigned char ds1820crc8( unsigned char *data, unsigned char length )
{
	//Generate 8bit CRC for given data (Maxim/Dallas)
    //data - pointer to data
    //length - length of data to use

    unsigned char i = 0;
    unsigned char j = 0;
	unsigned char mix;
    unsigned char crc = 0;

    for ( i = 0; i < length; i++ )
    {
        unsigned char byte = data[i];

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
