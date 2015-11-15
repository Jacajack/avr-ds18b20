#define _ONE_WIRE_C

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>

#include "OneWire.h"

unsigned char OneWireInit( OneWireConfiguration *Config ) //Init one wire bus
{
    unsigned char Response = 0;
    unsigned char SReg = SREG; //Store status register

    cli( ); //Disable interrupts

    *( ( *Config ).Port ) |= ( *Config ).Mask; //Write 1 to output
	*( ( *Config ).PortDirection ) |= ( *Config ).Mask;
	*( ( *Config ).Port ) &= ~( *Config ).Mask; //Write 0 to output

    _delay_us( 600 );

    *( ( *Config ) ).PortDirection &= ~( *Config ).Mask; //Set port to input

    _delay_us( 100 );

    Response = *( (*Config ).PortInput ) & ( *Config ).Mask; //Read input

    *( ( *Config ).Port ) |= ( *Config ).Mask; //Write 1 to output

    _delay_us( 500 );

    SREG = SReg; //Restore status register

    return Response != 0; //Return logical value
}

void OneWireWrite( OneWireConfiguration *Config, unsigned char Value ) //Write 1 or 0 to one wire bus
{
    unsigned char SReg = SREG; //Store status register

    cli( ); //Disable interrupts

    *( ( *Config ).Port ) |= ( *Config ).Mask; //Write 1 to output
	*( ( *Config ).PortDirection ) |= ( *Config ).Mask;
	*( ( *Config ).Port ) &= ~( *Config ).Mask; //Write 0 to output

    if ( Value != 0 )   //Change delay amounts according to given value
        _delay_us( 10 );
    else
        _delay_us( 80 );

    *( ( *Config ).Port ) = ( *Config ).Mask;

    if ( Value != 0 )
        _delay_us( 80 );
    else
        _delay_us( 2 );

    SREG = SReg;
}

void OneWireWriteByte( OneWireConfiguration *Config, unsigned char Data ) //Write byte to one wire bus
{
    for ( unsigned char i = 1; i != 0; i <<= 1 ) //Write byte in 8 single bit writes
    {
        OneWireWrite( Config, Data & i );
    }
}

unsigned char OneWireRead( OneWireConfiguration *Config ) //Read one wire data bus
{
    unsigned char Response = 0;
    unsigned char SReg = SREG; //Store status register

    cli( ); //Disable interrupts

    *( ( *Config ).Port ) |= ( *Config ).Mask; //Write 1 to output
	*( ( *Config ).PortDirection ) |= ( *Config ).Mask;
	*( ( *Config ).Port ) &= ~( *Config ).Mask; //Write 0 to output

	_delay_us( 2 );

	*( ( *Config ).PortDirection ) &= ~( *Config ).Mask; //Set port to input

	_delay_us( 5 );

	Response = *( ( *Config ).PortInput ); //Read input

    SREG = SReg; //Restore status register

    _delay_us( 60 );

    return ( Response & ( *Config ).Mask ) != 0; //Return logical value
}

unsigned char OneWireReadByte( OneWireConfiguration *Config ) //Read byte from one wire data bus
{
    unsigned char Data = 0;

    for ( unsigned char i = 1; i != 0; i <<= 1 ) //Read byte in 8 single bit reads
    {
        Data |= OneWireRead( Config ) * i;
    }

    return Data;
}

