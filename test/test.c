#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "uart.h"
#include "../include/ds1820.h"

char txt[80];
OnewireConf thermo = { &DDRB, &PORTB, &PINB, ( 1 << 0 ), 0, NULL };

int main( )
{
	uartInit( 9600 );

	while ( 1 )
	{
		//Read temperature
		ds1820request( &thermo );
		sprintf( txt, "raw: %d\n", ds18b20read( &thermo ) );
		uartSend( txt );

		//Delay
		_delay_ms( 1000 );
	}

	return 0;
}
