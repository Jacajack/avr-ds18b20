#define _DALLAS_1820_C
#include "Dallas1820.h"

void Dallas1820Request( OneWireConfiguration *Config ) //Send request to Dallas1820 on one wire bus
{
    OneWireInit( Config );
    OneWireWriteByte( Config, 0xCC );
    OneWireWriteByte( Config, 0x44 );
}

int Dallas18B20Read( OneWireConfiguration *Config ) //Read Dallas1820 temperature from one wire bus
{
    unsigned char Response[9];
    int Temperature;

    OneWireInit( Config );

    OneWireWriteByte( Config, 0xCC );
    OneWireWriteByte( Config, 0xBE );

    for ( unsigned char i = 0; i < 9; i++ )
        Response[i] = OneWireReadByte( Config );

    Temperature = (int)( Response[1] << 8 ) + ( Response[0] & 0xFF ); //Get temperature from received data
    //Temperature *= ( Response[1] & ( 1 << 5 ) ? -1 : 1 );

    Temperature = Dallas1820CRC8( Response, 8 ) != Response[8] ? 16000 : Temperature; //If CRC is bad, returns exactly 16000 (1000 degrees)

    return Temperature;
}

int Dallas18B20MatchRead( OneWireConfiguration *Config ) //Read Dallas1820 temperature from one wire bus
{
    unsigned char Response[9];
    int Temperature;

    OneWireInit( Config );

    OneWireWriteByte( Config, 0x55 );

    for ( unsigned char i = 0; i < 8; i++ )
        OneWireWriteByte( Config, ( *Config ).ROM[i] );

    OneWireWriteByte( Config, 0xBE );

    for ( unsigned char i = 0; i < 9; i++ )
        Response[i] = OneWireReadByte( Config );

    Temperature = (int)( Response[1] << 8 ) + ( Response[0] & 0xFF ); //Get temperature from received data
    //Temperature *= ( Response[1] & ( 1 << 5 ) ? -1 : 1 );

    Temperature = Dallas1820CRC8( Response, 8 ) != Response[8] ? 16000 : Temperature; //If CRC is bad, returns exactly 16000 (1000 degrees)

    return Temperature;
}

int Dallas18B20ArrayMatchRead( OneWireConfiguration *Config, const unsigned char *ROM ) //Read Dallas1820 temperature from one wire bus
{
    unsigned char Response[9];
    int Temperature;

    OneWireInit( Config );

    OneWireWriteByte( Config, 0x55 );

    for ( unsigned char i = 0; i < 8; i++ )
        OneWireWriteByte( Config, ROM[i] );

    OneWireWriteByte( Config, 0xBE );

    for ( unsigned char i = 0; i < 9; i++ )
        Response[i] = OneWireReadByte( Config );

    Temperature = (int)( Response[1] << 8 ) + ( Response[0] & 0xFF ); //Get temperature from received data
    //Temperature *= ( Response[1] & ( 1 << 5 ) ? -1 : 1 );

    Temperature = Dallas1820CRC8( Response, 8 ) != Response[8] ? 16000 : Temperature; //If CRC is bad, returns exactly 16000 (1000 degrees)

    return Temperature;
}

char Dallas1820ReadROM( OneWireConfiguration *Config ) //Read ROM to configuration structure
{
    if( OneWireInit( Config ) ) return 2;

    OneWireWriteByte( Config, 0x33 );

    for ( unsigned char i = 0; i < 8; i++ )
        ( *Config ).ROM[i] = OneWireReadByte( Config );

    return Dallas1820CRC8( ( *Config ).ROM, 7 ) == ( *Config ).ROM[7] ? 0 : 1; //Returned values: ( 0 - OK, 1 - CRC error, 2 - communication error )
}

char Dallas1820ReadROMArray( OneWireConfiguration *Config, unsigned char *ROM ) //Read ROM to array
{
    if ( OneWireInit( Config ) ) return 2;

    OneWireWriteByte( Config, 0x33 );

    for ( unsigned char i = 0; i < 8; i++ )
        ROM[i] = OneWireReadByte( Config );

    return Dallas1820CRC8( ROM, 7 ) == ROM[7] ? 0 : 1; //Returned values: ( 0 - OK, 1 - CRC error, 2 - communication error )
}

float Dallas18B20ToCelcius( int Temperature ) //Convert from Dallas18B20 response to temperature in Celcius degrees
{
    return (float) Temperature / 16.0f;
}

void Dallas1820Config( OneWireConfiguration *Config, unsigned char TH, unsigned char TL, unsigned char Configuration ) //Set up Dallas1820 internal configuration
{
    OneWireInit( Config );
    OneWireWriteByte( Config, 0xCC ); //Command - SKIP ROM
    OneWireWriteByte( Config, 0x4E ); //Command - Write Scratchpad

    OneWireWriteByte( Config, TH );
    OneWireWriteByte( Config, TL );
    OneWireWriteByte( Config, Configuration );
}

unsigned char Dallas1820CRC8( unsigned char *Data, unsigned char Length ) //Generate 8bit CRC for given data
{
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
