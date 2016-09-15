# AVR Dallas1820

[![The MIT License](https://img.shields.io/badge/license-MIT-orange.svg?style=flat-square)](http://opensource.org/licenses/MIT) [![Travis CI](https://img.shields.io/travis/Jacajack/avr-ds1820.svg?style=flat-square)](https://travis-ci.org/Jacajack/avr-ds1820)

This is a library for controlling DS1820 temperature sensor with AVR micro-controllers.

# Features
 - Structures for sensor configuration
 - Functions for reading temperatures and ROMs
 - Allows configuring DS1820 devices
 - Includes standalone OneWire library

# Usage

```c
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "uart.h"
#include "../include/ds1820.h"

char txt[80];

int main( )
{
	uartInit( 9600 );

	while ( 1 )
	{
		//Read temperature
		ds1820request( &PORTB, &DDRB, &PINB, ( 1 << 0 ) );
		sprintf( txt, "raw: %d\n", ds18b20read( &PORTB, &DDRB, &PINB, ( 1 << 0 ) ) );
		uartSend( txt );

		//Delay
		_delay_ms( 1000 );
	}

	return 0;
}
```

Each function takes pointers to port input, output and direction registers and mask value. Configuration above tells us, that sensors is connected to pin 0 at port B.

```c
ds1820request( &PORTB, &DDRB, &PINB, ( 1 << 0 ) );
```

This one tells sensor to start temperature conversion (it takes about 1s), and to read it use:

```c
ds18b20read( &PORTB, &DDRB, &PINB, ( 1 << 0 ) );
```

To read temperature with ROM address matching use function (`rom` is pointer to array with ROM stored inside):

```c
ds18b20mread( &PORTB, &DDRB, &PINB, ( 1 << 0 ), rom );
```

To read ROM into array (has to be allocated beforehand!) use:

```c
ds1820rom( &PORTB, &DDRB, &PINB, ( 1 << 0 ), rom );
```

Those are basic features, for more information visit [wiki](https://github.com/Jacajack/avr-dallas1820/wiki).

Library can be built with `makefile` for every MCU supported by `avr-gcc`.
Use it like this: `make F_CPU=8000000U MCU=atmega328p`. Simple, isn't it?

# Sample wiring diagram
<img src="http://i.imgur.com/hVe0f9P.png" height=400px></img>

This is sample wiring diagram. It shows basic connections between DS1820 and a micro-controller.
