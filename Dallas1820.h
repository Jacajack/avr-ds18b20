#ifdef _DALLAS_1820
#error Dallas 1820 library included more than once
#else
#define _DALLAS_1820 1.1
#endif

#ifndef _ONE_WIRE
#include "OneWire.h"
#endif

//Prototypes
extern void Dallas1820Request( OneWireConfiguration * ); //Send conversion request to Dallas1820 on one wire bus
extern int Dallas18B20Read( OneWireConfiguration * ); //Read Dallas1820 temperature from one wire bus
extern int Dallas18B20MatchRead( OneWireConfiguration * ); //Read Dallas1820 temperature from one wire bus
extern int Dallas18B20ArrayMatchRead( OneWireConfiguration *, const unsigned char * ); //Read Dallas1820 temperature from one wire bus
extern unsigned char Dallas1820ReadROM( OneWireConfiguration * );
extern unsigned char Dallas1820ReadROMArray( OneWireConfiguration *, unsigned char * );
extern float Dallas18B20ToCelcius( int ); //Convert from Dallas18B20 response to temperature in Celcius degrees
extern unsigned char Dallas1820VerifyResponse( float );
extern void Dallas1820Config( OneWireConfiguration *, unsigned char, unsigned char, unsigned char ); //Set up Dallas1820 internal configuration
extern unsigned char Dallas1820CRC8( unsigned char *, unsigned char ); //Generate 8bit CRC for given data


//Version history:
//  - v0.5 - 15-11-2012
//  - v1.1 - 27-12-2015
