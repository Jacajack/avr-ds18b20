/* ds18b20.h - a part of avr-ds18b20 library
 *
 * Copyright (C) 2016 Jacek Wieczorek
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.	See the LICENSE file for details.
 */

#ifndef DS18B20_H
#define DS18B20_H

#include <inttypes.h>
#include "onewire.h"

#define DS18B20_ERROR_OK	0
#define DS18B20_ERROR_COMM	1
#define DS18B20_ERROR_CRC	2
#define DS18B20_ERROR_PULL	3
#define DS18B20_ERROR_OTHER 4
#define DS18B20_OK DS18B20_ERROR_OK

#define DS18B20_COMMAND_READ_ROM 0x33
#define DS18B20_COMMAND_MATCH_ROM 0x55
#define DS18B20_COMMAND_SKIP_ROM 0xCC
#define DS18B20_COMMAND_CONVERT 0x44
#define DS18B20_COMMAND_WRITE_SP 0x4E
#define DS18B20_COMMAND_READ_SP 0xBE
#define DS18B20_COMMAND_COPY_SP 0x48

#define DS18B20_RES09 ( 0 << 5 )
#define DS18B20_RES10 ( 1 << 5 )
#define DS18B20_RES11 ( 2 << 5 )
#define DS18B20_RES12 ( 3 << 5 )

#define DS18B20_MUL 16

extern uint8_t ds18b20convert( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom );
extern uint8_t ds18b20rsp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, uint8_t *sp );
extern uint8_t ds18b20wsp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, uint8_t th, uint8_t tl, uint8_t conf );
extern uint8_t ds18b20csp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom );
extern uint8_t ds18b20read( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, int16_t *temperature ) ;
extern uint8_t ds18b20rom( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom );

#endif
