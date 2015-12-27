#ifdef __ONE_WIRE
#error One Wire library included more than once
#else
#define _ONE_WIRE 1.1
#endif

#include <inttypes.h>

typedef struct OneWireConfiguration
{
    volatile uint8_t *PortDirection; //Pointer to DDR register
    volatile uint8_t *Port; //Pointer to PORT register
    volatile uint8_t *PortInput; //Pointer to PIN register
    unsigned char Mask; //Port mask
    unsigned char Flags; //Additional configuration flags for fututre use

    unsigned char ROM[8]; //ROM address

} OneWireConfiguration;

//Prototypes
extern unsigned char OneWireInit( OneWireConfiguration * ); //Init one wire bus
extern void OneWireWrite( OneWireConfiguration *, unsigned char ); //Write 1 or 0 to one wire bus
extern void OneWireWriteByte( OneWireConfiguration *, unsigned char ); //Write byte to one wire bus
extern unsigned char OneWireRead( OneWireConfiguration * ); //Read one wire data bus
extern unsigned char OneWireReadByte( OneWireConfiguration * ); //Read byte from one wire data bus

