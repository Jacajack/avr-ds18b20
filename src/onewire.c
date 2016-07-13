#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>


#include "../include/onewire.h"

unsigned char onewireInit( OnewireConf *conf ) //Init one wire bus
{
    //conf - OneWire device confuration structure
    //Returned values: ( 0 - OK, 1 - error )

    unsigned char response = 0;
    unsigned char sreg = SREG; //Store status register

    cli( ); //Disable interrupts

    *( ( *conf ).port ) |= ( *conf ).mask; //Write 1 to output
	*( ( *conf ).portDirection ) |= ( *conf ).mask; //Set port to output
	*( ( *conf ).port ) &= ~( *conf ).mask; //Write 0 to output

    _delay_us( 600 );

    *( ( *conf ) ).portDirection &= ~( *conf ).mask; //Set port to input

    _delay_us( 100 );

    response = *( ( *conf ).portInput ) & ( *conf ).mask; //Read input

    _delay_us( 200 );

    *( ( *conf ).port ) |= ( *conf ).mask; //Write 1 to output
    *( ( *conf ).portDirection ) |= ( *conf ).mask; //Set port to output

    _delay_us( 600 );

    SREG = sreg; //Restore status register

    return response != 0; //Return logical value ( 0 - OK, 1 - communication error )
}

void onewireWrite( OnewireConf *conf, unsigned char value ) //Write 1 or 0 to one wire bus
{
    //conf - OneWire device confuration structure
    //value - 1 or 0 to write on OneWire bus

    unsigned char sreg = SREG; //Store status register

    cli( ); //Disable interrupts

    *( ( *conf ).port ) |= ( *conf ).mask; //Write 1 to output
	*( ( *conf ).portDirection ) |= ( *conf ).mask;
	*( ( *conf ).port ) &= ~( *conf ).mask; //Write 0 to output

    if ( value != 0 )   //Change delay amounts according to given value
        _delay_us( 8 );
    else
        _delay_us( 80 );

    *( ( *conf ).port ) = ( *conf ).mask;

    if ( value != 0 )
        _delay_us( 80 );
    else
        _delay_us( 2 );

    SREG = sreg;
}

void onewireWriteByte( OnewireConf *conf, unsigned char data ) //Write byte to one wire bus
{
    //conf - OneWire device confuration structure
    //data - Byte to write on OneWire bus

    unsigned char i = 0;

    for ( i = 1; i != 0; i <<= 1 ) //Write byte in 8 single bit writes
    {
        onewireWrite( conf, data & i );
    }
}

unsigned char onewireRead( OnewireConf *conf ) //Read one wire data bus
{
    //conf - OneWire device confuration structure
    //Returned value: 1 or 0 - value from OneWire bus

    unsigned char response = 0;
    unsigned char sreg = SREG; //Store status register

    cli( ); //Disable interrupts

    *( ( *conf ).port ) |= ( *conf ).mask; //Write 1 to output
	*( ( *conf ).portDirection ) |= ( *conf ).mask;
	*( ( *conf ).port ) &= ~( *conf ).mask; //Write 0 to output

	_delay_us( 2 );

	*( ( *conf ).portDirection ) &= ~( *conf ).mask; //Set port to input

	_delay_us( 5 );

	response = *( ( *conf ).portInput ); //Read input

    SREG = sreg; //Restore status register

    _delay_us( 60 );

    return ( response & ( *conf ).mask ) != 0; //Return logical value
}

unsigned char onewireReadByte( OnewireConf *conf ) //Read byte from one wire data bus
{
    //conf - OneWire device confuration structure
    //Returned values: Byte read from OneWire bus

    unsigned char data = 0;
    unsigned char i = 0;

    for ( i = 1; i != 0; i <<= 1 ) //Read byte in 8 single bit reads
    {
        data |= onewireRead( conf ) * i;
    }

    return data;
}
