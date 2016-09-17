/* ds18b20.c
 *
 * Copyright (C) 2016 Jacek Wieczorek
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdlib.h>
#include "../include/ds18b20.h"

static uint8_t ds18b20crc8( uint8_t *data, uint8_t length )
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

uint8_t ds18b20request( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask )
{
	//Send conversion request to DS18B20 on one wire bus

    if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM ) return DS18B20_ERROR_COMM;
    onewireWrite( port, direction, portin, mask, 0xCC ); //Command - Skip ROM
    onewireWrite( port, direction, portin, mask, 0x44 ); //Command - Convert

	return DS18B20_ERROR_OK;
}

uint8_t ds18b20read( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, int16_t *temperature )
{
	//Read temperature from DS18B20
    //Note: returns actual temperature * 16

    uint8_t response[9];
    uint8_t i = 0;

    //Communication check
    if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM )
        return DS18B20_ERROR_COMM;

    //If rom pointer is NULL then read temperature without matching.
	if ( rom == NULL )
    {
        //Skip ROM
        onewireWrite( port, direction, portin, mask, 0xCC );
    }
    else
    {
        //Match ROM
        onewireWrite( port, direction, portin, mask, 0x55 );
        for ( i = 0; i < 8; i++ )
            onewireWrite( port, direction, portin, mask, rom[i] );
    }

    //Read scratchpad
    onewireWrite( port, direction, portin, mask, 0xBE );

    for ( i = 0; i < 9; i++ )
        response[i] = onewireRead( port, direction, portin, mask );

    //Check pull-up
    if ( ( response[0] | response[1] | response[2] | response[3] | response[4] | response[5] | response[6] | response[7] ) == 0 )
        return DS18B20_ERROR_PULL;

    //CRC check
    if ( ds18b20crc8( response, 8 ) != response[8] )
        return DS18B20_ERROR_CRC;

    //Get temperature from received data
    *temperature = (int)( response[1] << 8 ) + ( response[0] & 0xFF );

    return DS18B20_ERROR_OK;
}

uint8_t ds18b20rom( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom )
{
	//Read DS18B20 rom

    unsigned char i = 0;

	if ( rom == NULL ) return DS18B20_ERROR_OTHER;
    if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM )
	{
		for ( i = 0; i < 8; i++ ) rom[i] = 0;
		return DS18B20_ERROR_COMM;
	}

    onewireWrite( port, direction, portin, mask, 0x33 ); //Read ROM

    for ( i = 0; i < 8; i++ )
        rom[i] = onewireRead( port, direction, portin, mask );

    if ( ( rom[0] | rom[1] | rom[2] | rom[3] | rom[4] | rom[5] | rom[6] | rom[7] ) == 0 ) return DS18B20_ERROR_PULL;

	//Check CRC
	if ( ds18b20crc8( rom, 7 ) != rom[7] )
	{
		for ( i = 0; i < 8; i++ ) rom[i] = 0;
		return DS18B20_ERROR_CRC;
	}

    return DS18B20_ERROR_OK;
}

uint8_t ds18b20wsp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t th, uint8_t tl, uint8_t conf )
{
	//Writes DS18B20 scratchpad
    //th - thermostat high temperature
    //tl - thermostat low temperature
    //conf - configuration byte

    if ( onewireInit( port, direction, portin, mask ) == ONEWIRE_ERROR_COMM ) return DS18B20_ERROR_COMM;
    onewireWrite( port, direction, portin, mask, 0xCC ); //Command - SKIP ROM
    onewireWrite( port, direction, portin, mask, 0x4E ); //Write scratchpad

    onewireWrite( port, direction, portin, mask, th );
    onewireWrite( port, direction, portin, mask, tl );
    onewireWrite( port, direction, portin, mask, conf );

    return DS18B20_ERROR_OK;
}
