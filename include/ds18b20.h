/* ds18b20.h
 *
 * Copyright (C) 2016 Jacek Wieczorek
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef DS18B20_H
#define DS18B20_H

#include "onewire.h"

#define DS18B20_ERROR_OK 	0
#define DS18B20_ERROR_CRC 	1
#define DS18B20_ERROR_COMM	2
#define DS18B20_ERROR_PULL 	3
#define DS18B20_ERROR_OTHER 	4

#define DS18B20_ERROR_CRC_TEMP 	16000
#define DS18B20_ERROR_COMM_TEMP	17600
#define DS18B20_ERROR_PULL_TEMP 	19200
#define DS18B20_ERROR_OTHER_TEMP 20800

extern unsigned char ds18b20request( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask );
extern int ds18b20read( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask );
extern int ds18b20mread( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom ) ;
extern unsigned char ds18b20rom( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom );
extern unsigned char ds18b20conf( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t th, uint8_t tl, uint8_t dsconf );

#endif
