#ifdef _DALLAS_1820
#error Dallas 1820 library included more than once
#else
#define _DALLAS_1820 1.0
#endif

#ifndef _ONE_WIRE
#include "onewire.h"
#endif

//Prototypes
extern void ds1820request( OnewireConf * ); //Send conversion request to Dallas1820 on one wire bus
extern int ds18b20read( OnewireConf * ); //Read Dallas1820 temperature from one wire bus
extern int ds18b20matchRead( OnewireConf * ); //Read Dallas1820 temperature from one wire bus
extern int ds18b20arrayMatchRead( OnewireConf *, const unsigned char * ); //Read Dallas1820 temperature from one wire bus
extern unsigned char ds1820readROM( OnewireConf * );
extern unsigned char ds1820readROMArray( OnewireConf *, unsigned char * );
extern float ds18b20toCelcius( int ); //Convert from Dallas18B20 response to temperature in Celcius degrees
extern unsigned char ds1820verify( float ); //Decode errors from functions for reading temperatures
extern unsigned char ds1820config( OnewireConf *, unsigned char, unsigned char, unsigned char ); //Set up Dallas1820 internal confuration
extern unsigned char ds1820CRC8( unsigned char *, unsigned char ); //Generate 8bit CRC for given data


//Version history:
//  - v0.5 - 15-11-2012
//  - v1.0 - 27-12-2015
