#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "uart.h"
#include "../include/ds18b20.h"

char txt[80];

int main( )
{
	int temp;
	uartInit( 9600 );

	while ( 1 )
	{
		//Start conversion
		ds18b20convert( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL );

		//Delay
		_delay_ms( 1000 );

		//Read temperature
		ds18b20read( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL, &temp );
		sprintf( txt, "raw: %d\n", temp );
		uartSend( txt );
	}

	return 0;
}
