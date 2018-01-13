# avr-ds18b20

[![The MIT License](https://img.shields.io/badge/license-MIT-orange.svg?style=flat-square)](http://opensource.org/licenses/MIT) [![Travis CI](https://img.shields.io/travis/Jacajack/avr-ds18b20.svg?style=flat-square)](https://travis-ci.org/Jacajack/avr-ds18b20)

`avr-ds18b20` is an AVR library for controlling DS18B20 temperature sensors.
It can be easily compiled for every micro-controller supported by `avr-gcc`.

## Usage example

```c
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include <ds18b20/ds18b20.h>

int main( )
{
	int temp;

	while ( 1 )
	{
		//Start conversion (without ROM matching)
		ds18b20convert( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL );

		//Delay (sensor needs time to perform conversion)
		_delay_ms( 1000 );

		//Read temperature (without ROM matching)
		ds18b20read( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL, &temp );

		//Somehow use data stored in `temp` variable
	}

	return 0;
}

```

For more information visit [wiki](https://github.com/Jacajack/avr-dallas1820/wiki).
