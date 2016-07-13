#include <avr/io.h>
#include "uart.h"

char UARTbuffer[80];

//Version for atmega8

void uartSendChar( char c )
{
    //Send single byte through UART
    //c - character to send

    while ( !( UCSRA & ( 1 << UDRE ) ) );
    UDR = c;
}

char uartReadChar( )
{
    //Receive and return single byte
    //Returned value: received character

    while ( !( UCSRA & ( 1 << RXC ) ) );
    return UDR;
}

void uartSend( const char *s )
{
    //Send character word through UART
    //s - pointer to string
	unsigned int i = 0;

	while ( s[i] )
    	uartSendChar( s[i++] );
}

void uartRead( )
{
    //Receive character word and store it in buffer
    //Value returned in: UARTbuffer

    unsigned int i = 0;

    char c = 0;
    for ( i = 0; i < 80; i++ )
    {
        c = uartReadChar( );
        if ( c == 13 || c == 10 || c == 0 )
        {
            UARTbuffer[i] = 0;
            break;
        }
        UARTbuffer[i] = c;
    }
}

void uartInit( unsigned int baud )
{
    //Init UART with given BAUD rate
    //baud - baud rate

	baud = F_CPU / 16 / baud - 1;

    UBRRH = (unsigned char) ( baud >> 8 ); //Set BAUD rate
    UBRRL = (unsigned char) baud;
    UCSRB = ( 1 << RXEN ) | ( 1 << TXEN ); //Enable RX and TX
    UCSRC = ( 1 << URSEL ) | ( 0 << USBS ) | ( 3 << UCSZ0 ); //Set data format
}
