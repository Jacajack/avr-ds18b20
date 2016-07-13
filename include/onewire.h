#ifndef ONEWIRE_H
#define ONEWIRE_H

#include <inttypes.h>

typedef struct OnewireConf
{
    volatile uint8_t *portDirection; //Pointer to DDR register
    volatile uint8_t *port; //Pointer to port register
    volatile uint8_t *portInput; //Pointer to PIN register
    unsigned char mask; //port mask
    unsigned char flags; //Additional confuration flags for fututre use

    unsigned char rom[8]; //ROM address

} OnewireConf;

//Prototypes
extern unsigned char onewireInit( OnewireConf * ); //Init one wire bus
extern void onewireWrite( OnewireConf *, unsigned char ); //Write 1 or 0 to one wire bus
extern void onewireWriteByte( OnewireConf *, unsigned char ); //Write byte to one wire bus
extern unsigned char onewireRead( OnewireConf * ); //Read one wire data bus
extern unsigned char onewireReadByte( OnewireConf * ); //Read byte from one wire data bus


//Version history:
//  - v0.5 - 15-11-2012
//  - v1.0 - 27-12-2015

#endif
