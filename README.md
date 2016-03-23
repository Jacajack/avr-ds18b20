# AVR Dallas1820

[![Build Status](https://travis-ci.org/Jacajack/avr-dallas1820.svg?branch=master)](https://travis-ci.org/Jacajack/avr-dallas1820)

This is a library for controlling temperature sensor DS1820 with AVR.

# Features
 - Structures for sensor configuration
 - Functions for reading temperatures and ROMs
 - Allows configuring DS1820 devices
 - CRC support
 - Includes standalone OneWire library

# Examples

### Reading temperature

```c
#include <stdio.h>

#include <avr/io.h>
#include <util/delay.h>

#include "Dallas1820/Dallas1820.h"

int main( )
{
    float Temperature = 0.0f;

    //Sensor configuration (port direction register, port output register, port input register and mask)
    OneWireConfiguration Thermometer = { &DDRD, &PORTD, &PIND, ( 1 << 7 ) };

    Dallas1820ReadROM( &Thermometer ); //Read ROM

    while ( 1 )
    {
        //Request Dallas 1820 temperature conversion
        Dallas1820Request( &Thermometer );

        //Actually conversion takes 800ms, so delay should be included here, but it's optional

        //Read temperature
        Temperature = Dallas18B20ToCelcius( Dallas18B20Read( &Thermometer ) );

        //Use data (printf( ) is only example)
        printf( "%f\n", (double) Temperature );

        //Wait 1s
        for ( unsigned char i = 0; i < 100; i++ )
            _delay_ms( 10 );
    }
    return 0;
}

```

Above code reads temperature from DS18B20 sensor and stores it in `Temperature` variable.

To tell library where sensor is connected `OneWireConfiguation` structure is used. You can set up one like this:

```c
OneWireConfiguration Thermometer = { &DDRD, &PORTD, &PIND, ( 1 << 7 ) };

```

or with ROM address and flags (flags are not supported yet):

```c
OneWireConfiguration Thermometer = { &DDRD, &PORTD, &PIND, ( 1 << 7 ), 0, { 0x28, 0xff, 0x9c, 0xc0, 0x71, 0x14, 0x04, 0x15 } };

```

It means sensor is on connected to port D on pin 7. All pointers need to specify same port registers (direction, output, input). Last member is mask, it tells you about exact pin sensor is connected to.

All you need to do later is to pass pointer to configuration structure to functions that need it, like that:

```c
Dallas1820Request( &Thermometer );
```

This one tells sensor to start internal temperature conversion, and to read it use:

```c
Dallas18B20Read( &Thermometer );
```

To read temperature with ROM address matching use function:

```c
Dallas18B20MatchRead( &Thermometer );
```

It uses ROM stored in configuration structure. You can get it into there using:
```c
Dallas1820ReadROM( &Thermometer );
```
You can also read it to array of unsigned characters, like shown below:
```c
Dallas1820ReadROMArray( &Thermometer, Array );
```

By analogy you can also match ROM from characters array:

```c
Dallas18B20ArrayMatchRead( &Thermometer, Array );
```

This is very basic usage of this library, for more information visit [wiki](https://github.com/Jacajack/avr-dallas1820/wiki).

Library is very easy to compile, just compile each `.c` file and link all `.o` files together,
if you have any problems though, just take look at `makefile`.

# Sample wiring diagram
<img src="samplewiring.png" height=400px></img>

This is sample wiring diagram. It shows basic connections between DS1820 and microcontroller. It also works with code snippet above.

**Note: You can use whatever AVR microcontroller you want to, ATmega328 is only example.**
