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

    Temperature = ( (unsigned int) Response[1] << 8 ) + ( Response[0] & 0xFF ); //Get temperature from received data
    Temperature *= ( Response[1] & ( 1 << 5 ) ? -1 : 1 );

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

    Temperature = ( (unsigned int) Response[1] << 8 ) + ( Response[0] & 0xFF ); //Get temperature from received data
    Temperature *= ( Response[1] & ( 1 << 5 ) ? -1 : 1 );

    return Temperature;
}

void Dallas1820ReadROM( OneWireConfiguration *Config ) //Read ROM to configuration struct
{
    OneWireInit( Config );

    OneWireWriteByte( Config, 0x33 );

    for ( unsigned char i = 0; i < 8; i++ )
        ( *Config ).ROM[i] = OneWireReadByte( Config );
}

void Dallas1820ReadROMArray( OneWireConfiguration *Config, unsigned char *ROM ) //Read ROM to array
{
    OneWireInit( Config );

    OneWireWriteByte( Config, 0x33 );

    for ( unsigned char i = 0; i < 8; i++ )
        ROM[i] = OneWireReadByte( Config );
}

float Dallas18B20ToCelcius( int Temperature ) //Convert from Dallas18B20 response to temperature in Celcius degrees
{
    return (float) Temperature / 16.0f;
}
