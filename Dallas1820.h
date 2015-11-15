#ifdef _DALLAS_1820
#error Dallas 1820 library included more than once
#else
#define _DALLAS_1820 1.0
#endif

#ifndef _ONE_WIRE
#include "OneWire.h"
#endif

//Prototypes
extern void Dallas1820Request( OneWireConfiguration * ); //Send request to Dallas1820 on one wire bus
extern int Dallas18B20Read( OneWireConfiguration * ); //Read Dallas1820 temperature from one wire bus
extern int Dallas18B20MatchRead( OneWireConfiguration * ); //Read Dallas1820 temperature from one wire bus
extern int Dallas18B20ArrayMatchRead( OneWireConfiguration *, const unsigned char * ); //Read Dallas1820 temperature from one wire bus
extern void Dallas1820ReadROM( OneWireConfiguration * );
extern void Dallas1820ReadROMArray( OneWireConfiguration *, unsigned char * );
extern float Dallas18B20ToCelcius( int ); //Convert from Dallas18B20 response to temperature in Celcius degrees
