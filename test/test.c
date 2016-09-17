#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "uart.h"
#include "../include/ds18b20.h"

char txt[80];

int main( )
{
	uartInit( 9600 );

	while ( 1 )
	{
		//Read temperature
		ds18b20request( &PORTB, &DDRB, &PINB, ( 1 << 0 ) );
		sprintf( txt, "raw: %d\n", ds18b20read( &PORTB, &DDRB, &PINB, ( 1 << 0 ) ) );
		uartSend( txt );

		//Delay
		_delay_ms( 1000 );
	}

	return 0;
}
