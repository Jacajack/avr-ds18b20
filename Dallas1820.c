#define _DALLAS_1820_C
#include "Dallas1820.h"

/******************************************************************************/
/**                           Reading data                                   **/
/**                                                                          **/
/******************************************************************************/

void Dallas1820Request( OneWireConfiguration *Config ) //Send conversion request to Dallas1820 on one wire bus
{
    //Config - OneWire device configuration structure

    OneWireInit( Config );
    OneWireWriteByte( Config, 0xCC ); //Command - Skip ROM
    OneWireWriteByte( Config, 0x44 ); //Command - Convert
}

int Dallas18B20Read( OneWireConfiguration *Config ) //Read Dallas1820 temperature from one wire bus
{
    //Config - OneWire device configuration structure
    //Returned value: Temperature (*16)

    unsigned char Response[9];
    int Temperature;

    if ( OneWireInit( Config ) ) return 17600; //When communication error occurs, returns exactly 17600 (1100 degrees)

    OneWireWriteByte( Config, 0xCC ); //Command - Skip ROM
    OneWireWriteByte( Config, 0xBE ); //Command - Read Scratchpad

    for ( unsigned char i = 0; i < 9; i++ )
        Response[i] = OneWireReadByte( Config );

    if ( ( Response[0] | Response[1] | Response[2] | Response[3] | Response[4] | Response[5] | Response[6] | Response[7] ) == 0 )
        return 19200; //When only zeros were received returns exactly 19200 (1200 degrees)

    Temperature = (int)( Response[1] << 8 ) + ( Response[0] & 0xFF ); //Get temperature from received data
    //Temperature *= ( Response[1] & ( 1 << 5 ) ? -1 : 1 );

    Temperature = Dallas1820CRC8( Response, 8 ) != Response[8] ? 16000 : Temperature; //If CRC is bad, returns exactly 16000 (1000 degrees)

    return Temperature;
}

int Dallas18B20MatchRead( OneWireConfiguration *Config ) //Read Dallas1820 temperature from one wire bus
{
    //Config - OneWire device configuration structure
    //Returned value: Temperature (*16)

    unsigned char Response[9];
    int Temperature;

    if ( OneWireInit( Config ) ) return 17600; //When communication error occurs, returns exactly 17600 (1100 degrees)

    OneWireWriteByte( Config, 0x55 ); //Command - Match ROM

    for ( unsigned char i = 0; i < 8; i++ )
        OneWireWriteByte( Config, ( *Config ).ROM[i] );

    OneWireWriteByte( Config, 0xBE ); //Command - Read Scratchpad

    for ( unsigned char i = 0; i < 9; i++ )
        Response[i] = OneWireReadByte( Config );

    if ( ( Response[0] | Response[1] | Response[2] | Response[3] | Response[4] | Response[5] | Response[6] | Response[7] ) == 0 )
        return 19200; //When only zeros were received returns exactly 19200 (1200 degrees)

    Temperature = (int)( Response[1] << 8 ) + ( Response[0] & 0xFF ); //Get temperature from received data
    //Temperature *= ( Response[1] & ( 1 << 5 ) ? -1 : 1 );

    Temperature = Dallas1820CRC8( Response, 8 ) != Response[8] ? 16000 : Temperature; //If CRC is bad, returns exactly 16000 (1000 degrees)

    return Temperature;
}

int Dallas18B20ArrayMatchRead( OneWireConfiguration *Config, const unsigned char *ROM ) //Read Dallas1820 temperature from one wire bus
{
    //Config - OneWire device configuration structure
    //ROM - pointer to array with ROM stored
    //Returned value: Temperature (*16)

    unsigned char Response[9];
    int Temperature;

    if ( OneWireInit( Config ) ) return 17600; //When communication error occurs, returns exactly 17600 (1100 degrees)

    OneWireWriteByte( Config, 0x55 ); //Command - Match ROM

    for ( unsigned char i = 0; i < 8; i++ )
        OneWireWriteByte( Config, ROM[i] );

    OneWireWriteByte( Config, 0xBE ); //Command - Read Scratchpad

    for ( unsigned char i = 0; i < 9; i++ )
        Response[i] = OneWireReadByte( Config );

    if ( ( Response[0] | Response[1] | Response[2] | Response[3] | Response[4] | Response[5] | Response[6] | Response[7] ) == 0 )
        return 19200; //When only zeros were received returns exactly 19200 (1200 degrees)

    Temperature = (int)( Response[1] << 8 ) + ( Response[0] & 0xFF ); //Get temperature from received data
    //Temperature *= ( Response[1] & ( 1 << 5 ) ? -1 : 1 );

    Temperature = Dallas1820CRC8( Response, 8 ) != Response[8] ? 16000 : Temperature; //If CRC is bad, returns exactly 16000 (1000 degrees)

    return Temperature;
}

/******************************************************************************/
/**                                  ROM                                     **/
/**                                                                          **/
/******************************************************************************/

unsigned char Dallas1820ReadROM( OneWireConfiguration *Config ) //Read ROM to configuration structure
{
    //Config - OneWire device configuration structure
    //Returned values: ( 0 - OK, 1 - CRC error, 2 - communication error, 3 - only zeros )

    if( OneWireInit( Config ) ) return 2;

    OneWireWriteByte( Config, 0x33 ); //Command - Read ROM

    for ( unsigned char i = 0; i < 8; i++ )
        ( *Config ).ROM[i] = OneWireReadByte( Config );

    if ( ( ( *Config ).ROM[0] | ( *Config ).ROM[1] | ( *Config ).ROM[2] | ( *Config ).ROM[3] | ( *Config ).ROM[4] | ( *Config ).ROM[5] | ( *Config ).ROM[6] | ( *Config ).ROM[7] ) == 0 )
        return 3;

    return Dallas1820CRC8( ( *Config ).ROM, 7 ) == ( *Config ).ROM[7] ? 0 : 1;
}

unsigned char Dallas1820ReadROMArray( OneWireConfiguration *Config, unsigned char *ROM ) //Read ROM to array
{
    //Config - OneWire device configuration structure
    //ROM - pointer to array for storing ROM
    //Returned values: ( 0 - OK, 1 - CRC error, 2 - communication error, 3 - only zeros )

    if ( OneWireInit( Config ) ) return 2;

    OneWireWriteByte( Config, 0x33 ); //Command - Read ROM

    for ( unsigned char i = 0; i < 8; i++ )
        ROM[i] = OneWireReadByte( Config );

    if ( ( ROM[0] | ROM[1] | ROM[2] | ROM[3] | ROM[4] | ROM[5] | ROM[6] | ROM[7] ) == 0 )
        return 3;

    return Dallas1820CRC8( ROM, 7 ) == ROM[7] ? 0 : 1;
}



/******************************************************************************/
/**                              Utilities                                   **/
/**                                                                          **/
/******************************************************************************/

float Dallas18B20ToCelcius( int Temperature ) //Convert from Dallas18B20 response to temperature in Celcius degrees
{
    //Temperature - integer temperature value
    //Returned value: Temperature in Celcius degrees

    return (float) Temperature / 16.0f;
}

unsigned char Dallas1820VerifyResponse( float Temperature ) //Decode errors from functions for reading temperatures
{
    //Temperature - integer or float temperature value
    //Returned values: ( 0 - OK, 1 - CRC error, 2 - communication error, 3 - only zeros )

    if ( Temperature == 1200.0f || Temperature == 19200.0f ) return 3;
    if ( Temperature == 1100.0f || Temperature == 17600.0f ) return 2;
    if ( Temperature == 1000.0f || Temperature == 16000.0f ) return 1;
    return 0;
}



/******************************************************************************/
/**                            Configuration                                 **/
/**                                                                          **/
/******************************************************************************/

unsigned char Dallas1820Config( OneWireConfiguration *Config, unsigned char TH, unsigned char TL, unsigned char Configuration ) //Set up Dallas1820 internal configuration
{
    //Config - OneWire device configuration structure
    //TH - Thermostat high temperature
    //TL - Thermostat low temperature
    //Configuration - DS1820 configuration data

    if ( !OneWireInit( Config ) ) return 2;
    OneWireWriteByte( Config, 0xCC ); //Command - SKIP ROM
    OneWireWriteByte( Config, 0x4E ); //Command - Write Scratchpad

    OneWireWriteByte( Config, TH );
    OneWireWriteByte( Config, TL );
    OneWireWriteByte( Config, Configuration );

    return 0;
}



/******************************************************************************/
/**                           CRC8 calculation                               **/
/**                                                                          **/
/******************************************************************************/

unsigned char Dallas1820CRC8( unsigned char *Data, unsigned char Length ) //Generate 8bit CRC for given data
{
    //Data - pointer to data
    //Length - length of data to use

    unsigned char CRC = 0;

    for ( unsigned char i = 0; i < Length; i++ )
    {
        unsigned char Byte = Data[i];

        for( unsigned char j = 0; j < 8; j++ )
        {
            unsigned char Mix = ( CRC ^ Byte ) & 0x01;
            CRC >>= 1;
            if ( Mix ) CRC ^= 0x8C;
            Byte >>= 1;
        }
    }
    return CRC;
}
