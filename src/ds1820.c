#include <stdlib.h>
#include "../include/ds1820.h"


/******************************************************************************/
/**                           Reading data                                   **/
/**                                                                          **/
/******************************************************************************/

void ds1820request( OnewireConf *conf ) //Send conversion request to Dallas1820 on one wire bus
{
    //conf - OneWire device confuration structure

    onewireInit( conf );
    onewireWriteByte( conf, 0xCC ); //Command - Skip ROM
    onewireWriteByte( conf, 0x44 ); //Command - Convert
}

int ds18b20read( OnewireConf *conf ) //Read Dallas1820 temperature from one wire bus
{
    //conf - OneWire device confuration structure
    //Returned value: Temperature (*16)

    unsigned char response[9];
    unsigned char i = 0;
    int temperature;

    if ( onewireInit( conf ) ) return 17600; //When communication error occurs, returns exactly 17600 (1100 degrees)

    onewireWriteByte( conf, 0xCC ); //Command - Skip ROM
    onewireWriteByte( conf, 0xBE ); //Command - Read Scratchpad

    for ( i = 0; i < 9; i++ )
        response[i] = onewireReadByte( conf );

    if ( ( response[0] | response[1] | response[2] | response[3] | response[4] | response[5] | response[6] | response[7] ) == 0 )
        return 19200; //When only zeros were received returns exactly 19200 (1200 degrees)

    temperature = (int)( response[1] << 8 ) + ( response[0] & 0xFF ); //Get temperature from received data
    temperature = ds1820crc8( response, 8 ) != response[8] ? 16000 : temperature; //If CRC is bad, returns exactly 16000 (1000 degrees)

    return temperature;
}

int ds18b20matchRead( OnewireConf *conf ) //Read Dallas1820 temperature from one wire bus
{
    //conf - OneWire device confuration structure
    //Returned value: Temperature (*16)

    unsigned char response[9];
    unsigned char i = 0;
    int temperature;

	if ( conf->rom == NULL ) return 17600;
    if ( onewireInit( conf ) ) return 17600; //When communication error occurs, returns exactly 17600 (1100 degrees)

    onewireWriteByte( conf, 0x55 ); //Command - Match ROM

    for ( i = 0; i < 8; i++ )
        onewireWriteByte( conf, conf->rom[i] );

    onewireWriteByte( conf, 0xBE ); //Command - Read Scratchpad

    for ( i = 0; i < 9; i++ )
        response[i] = onewireReadByte( conf );

    if ( ( response[0] | response[1] | response[2] | response[3] | response[4] | response[5] | response[6] | response[7] ) == 0 )
        return 19200; //When only zeros were received returns exactly 19200 (1200 degrees)

    temperature = (int)( response[1] << 8 ) + ( response[0] & 0xFF ); //Get temperature from received data
    temperature = ds1820crc8( response, 8 ) != response[8] ? 16000 : temperature; //If CRC is bad, returns exactly 16000 (1000 degrees)

    return temperature;
}

int ds18b20arrayMatchRead( OnewireConf *conf, const unsigned char *rom ) //Read Dallas1820 temperature from one wire bus
{
    //conf - OneWire device confuration structure
    //rom - pointer to array with ROM stored
    //Returned value: Temperature (*16)

    unsigned char response[9];
    unsigned char i = 0;
    int temperature;

	if ( conf->rom == NULL ) return 17600;
    if ( onewireInit( conf ) ) return 17600; //When communication error occurs, returns exactly 17600 (1100 degrees)

    onewireWriteByte( conf, 0x55 ); //Command - Match ROM

    for ( i = 0; i < 8; i++ )
        onewireWriteByte( conf, rom[i] );

    onewireWriteByte( conf, 0xBE ); //Command - Read Scratchpad

    for ( i = 0; i < 9; i++ )
        response[i] = onewireReadByte( conf );

    if ( ( response[0] | response[1] | response[2] | response[3] | response[4] | response[5] | response[6] | response[7] ) == 0 )
        return 19200; //When only zeros were received returns exactly 19200 (1200 degrees)

    temperature = (int)( response[1] << 8 ) + ( response[0] & 0xFF ); //Get temperature from received data
    temperature = ds1820crc8( response, 8 ) != response[8] ? 16000 : temperature; //If CRC is bad, returns exactly 16000 (1000 degrees)

    return temperature;
}

/******************************************************************************/
/**                                  ROM                                     **/
/**                                                                          **/
/******************************************************************************/

unsigned char ds1820readROM( OnewireConf *conf ) //Read ROM to confuration structure
{
    //conf - OneWire device confuration structure
    //Returned values: ( 0 - OK, 1 - CRC error, 2 - communication error, 3 - only zeros )

    unsigned char i = 0;

	if ( conf->rom == NULL ) return 1;
    if( onewireInit( conf ) ) return 2;

    onewireWriteByte( conf, 0x33 ); //Command - Read ROM

    for ( i = 0; i < 8; i++ )
        conf->rom[i] = onewireReadByte( conf );

    if ( ( conf->rom[0] | conf->rom[1] | conf->rom[2] | conf->rom[3] | conf->rom[4] | conf->rom[5] | conf->rom[6] | conf->rom[7] ) == 0 )
        return 3;

    return ds1820crc8( conf->rom, 7 ) == conf->rom[7] ? 0 : 1;
}

unsigned char ds1820readROMArray( OnewireConf *conf, unsigned char *rom ) //Read ROM to array
{
    //conf - OneWire device confuration structure
    //rom - pointer to array for storing ROM
    //Returned values: ( 0 - OK, 1 - CRC error, 2 - communication error, 3 - only zeros )

    unsigned char i = 0;

	if ( conf->rom == NULL ) return 1;
    if ( onewireInit( conf ) ) return 2;

    onewireWriteByte( conf, 0x33 ); //Command - Read ROM

    for ( i = 0; i < 8; i++ )
        rom[i] = onewireReadByte( conf );

    if ( ( rom[0] | rom[1] | rom[2] | rom[3] | rom[4] | rom[5] | rom[6] | rom[7] ) == 0 )
        return 3;

    return ds1820crc8( rom, 7 ) == rom[7] ? 0 : 1;
}



/******************************************************************************/
/**                              Utilities                                   **/
/**                                                                          **/
/******************************************************************************/

float ds18b20toCelcius( int temperature ) //Convert from Dallas18B20 response to temperature in Celcius degrees
{
    //Temperature - integer temperature value
    //Returned value: Temperature in Celcius degrees

    return (float) temperature / 16.0f;
}

unsigned char ds1820verify( float temperature ) //Decode errors from functions for reading temperatures
{
    //Temperature - integer or float temperature value
    //Returned values: ( 0 - OK, 1 - CRC error, 2 - communication error, 3 - only zeros )

    if ( temperature == 1200.0f || temperature == 19200.0f ) return 3;
    if ( temperature == 1100.0f || temperature == 17600.0f ) return 2;
    if ( temperature == 1000.0f || temperature == 16000.0f ) return 1;
    return 0;
}



/******************************************************************************/
/**                            confuration                                 **/
/**                                                                          **/
/******************************************************************************/

unsigned char ds1820conf( OnewireConf *conf, unsigned char th, unsigned char tl, unsigned char dsconf ) //Set up Dallas1820 internal confuration
{
    //conf - OneWire device confuration structure
    //th - Thermostat high temperature
    //tl - Thermostat low temperature
    //dsconf - DS1820 confuration data

    if ( onewireInit( conf ) ) return 2;
    onewireWriteByte( conf, 0xCC ); //Command - SKIP ROM
    onewireWriteByte( conf, 0x4E ); //Command - Write Scratchpad

    onewireWriteByte( conf, th );
    onewireWriteByte( conf, tl );
    onewireWriteByte( conf, dsconf );

    return 0;
}



/******************************************************************************/
/**                           CRC8 calculation                               **/
/**                                                                          **/
/******************************************************************************/

unsigned char ds1820crc8( unsigned char *data, unsigned char length ) //Generate 8bit CRC for given data
{
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
