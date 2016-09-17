# avr-ds18b20

[![The MIT License](https://img.shields.io/badge/license-MIT-orange.svg?style=flat-square)](http://opensource.org/licenses/MIT) [![Travis CI](https://img.shields.io/travis/Jacajack/avr-ds18b20.svg?style=flat-square)](https://travis-ci.org/Jacajack/avr-ds18b20)

`avr-ds18b20` is a library for controlling DS18B20 temperature sensors with AVR micro-controllers.

# Features
 - Allows reading temperatures, ROMs and scratchpads
 - Sensors can be configured
 - Includes simple standalone 1-Wire library


# Some C code

```c
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
		//Start conversion (without ROM matching)
		ds18b20convert( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL );

		//Delay (sensor needs time to perform conversion)
		_delay_ms( 1000 );

		//Read temperature (without ROM matching)
		ds18b20read( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL, &temp );

        //Somehow output or use data
		sprintf( txt, "raw: %d\n", temp );
		uartSend( txt );
	}

	return 0;
}

```

For more information visit [wiki](https://github.com/Jacajack/avr-dallas1820/wiki).
