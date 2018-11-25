/* ds18b20.h - a part of avr-ds18b20 library
 *
 * Copyright (C) 2016 Jacek Wieczorek
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.	See the LICENSE file for details.
 */

/**
	\file
	\brief DS18B20 sensor functions
*/

#ifndef DS18B20_H
#define DS18B20_H

#include <inttypes.h>
#include "onewire.h"

#define DS18B20_ERROR_OK       0 //!< Communication with sensor succesful
#define DS18B20_ERROR_COMM     1 //!< Communication with sensor failed
#define DS18B20_ERROR_CRC      2 //!< Data CRC check failed
#define DS18B20_ERROR_PULL     3 //!< Received only zeros - you may want to check the 1wire bus pull-up resistor
#define DS18B20_ERROR_OTHER    4 //!< Other reason (bad user-provided argument, etc.)
#define DS18B20_OK DS18B20_ERROR_OK //!< An alias for \ref DS18B20_ERROR_OK

#define DS18B20_COMMAND_READ_ROM   0x33 //!< Read sensors's ROM address
#define DS18B20_COMMAND_MATCH_ROM  0x55 //!< Request sensors to start matching ROM addresses
#define DS18B20_COMMAND_SKIP_ROM   0xCC //!< Request sensors to ignore ROM matching phase
#define DS18B20_COMMAND_CONVERT    0x44 //!< Request temperature conversion
#define DS18B20_COMMAND_WRITE_SP   0x4E //!< Request internal scratchpad to be written
#define DS18B20_COMMAND_READ_SP    0xBE //!< Read data from the internal scratchpad
#define DS18B20_COMMAND_COPY_SP    0x48 //!< Request scratchpad contents to be copied into internal EEPROM
#define DS18B20_COMMAND_SEARCH_ROM 0xF0 //!< Begin ROM discovery proccess

#define DS18B20_RES09 ( 0 << 5 ) //!< 9-bit sensor resolution
#define DS18B20_RES10 ( 1 << 5 ) //!< 10-bit sensor resolution
#define DS18B20_RES11 ( 2 << 5 ) //!< 11-bit sensor resolution
#define DS18B20_RES12 ( 3 << 5 ) //!< 12-bit sensor resolution

#define DS18B20_MUL 16 //!< Temperature multiplier (so we don't need floats)

#ifdef __cplusplus
extern "C"
{
#endif

/**
	\brief Calculate 8-bit Maxim/Dallas CRC of provided data
	\param data A pointer to the data to be processed
	\param length The length of the data in bytes
	\returns 8-bit CRC value
*/
extern uint8_t ds18b20crc8( uint8_t *data, uint8_t length );

/**
	\brief Perform a DS18B20 ROM matching operation (usually before sending a command) or explicitly skips ROM matching stage
	\param port A pointer to the port output register
	\param direction A pointer to the port direction register
	\param portin A pointer to the port input register
	\param mask A bit mask, determining to which pin the sensor is connected
	\param rom The ROM address used for matching. If NULL, ROM matching will be skipped.

	\note If provided ROM address pointer is NULL, ROM matching operation is skipped.
*/
extern void ds18b20match( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom );

/**
	\brief Requests temperature conversion on DS18B20 sensor(s)
	\param port A pointer to the port output register
	\param direction A pointer to the port direction register
	\param portin A pointer to the port input register
	\param mask A bit mask, determining to which pin the sensor is connected
	\param rom The ROM address used for matching. If NULL, ROM matching will be skipped.
	\returns \ref DS18B20_ERROR_OK on success

	\note If provided ROM address pointer is NULL, ROM matching operation is skipped.
*/
extern uint8_t ds18b20convert( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom );

/**
	\brief Reads scratchpad from DS18B20 sensor
	\param port A pointer to the port output register
	\param direction A pointer to the port direction register
	\param portin A pointer to the port input register
	\param mask A bit mask, determining to which pin the sensor is connected
	\param rom The ROM address used for matching. If NULL, ROM matching will be skipped.
	\param sp A pointer to the memory location where scratchpad contents should be returned
	\returns \ref DS18B20_ERROR_OK on success

	\note If provided ROM address pointer is NULL, ROM matching operation is skipped.
*/
extern uint8_t ds18b20rsp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, uint8_t *sp );

/**
	\brief Writes DS18B20 sensor's scratchpad
	\param port A pointer to the port output register
	\param direction A pointer to the port direction register
	\param portin A pointer to the port input register
	\param mask A bit mask, determining to which pin the sensor is connected
	\param rom The ROM address used for matching. If NULL, ROM matching will be skipped.
	\param th Thermostat 'high' temperature
	\param tl Thermostat 'low' temperature
	\param conf Sensor configuration byte
	\returns \ref DS18B20_ERROR_OK on success

	\note If provided ROM address pointer is NULL, ROM matching operation is skipped.
*/
extern uint8_t ds18b20wsp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, uint8_t th, uint8_t tl, uint8_t conf );

/**
	\brief Copies DS18B20 sensor's scratchpad contents to its internal EEPROM memory
	\param port A pointer to the port output register
	\param direction A pointer to the port direction register
	\param portin A pointer to the port input register
	\param mask A bit mask, determining to which pin the sensor is connected
	\param rom The ROM address used for matching. If NULL, ROM matching will be skipped.
	\returns \ref DS18B20_ERROR_OK on success

	\note If provided ROM address pointer is NULL, ROM matching operation is skipped.
*/
extern uint8_t ds18b20csp( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom );

/**
	\brief Reads temperature from DS18B20 sensors
	\param port A pointer to the port output register
	\param direction A pointer to the port direction register
	\param portin A pointer to the port input register
	\param mask A bit mask, determining to which pin the sensor is connected
	\param rom The ROM address used for matching. If NULL, ROM matching will be skipped.
	\param temperature A pointer to a 16-bit integer variable where temperature value should be returned
	\returns \ref DS18B20_ERROR_OK on success

	\note If provided ROM address pointer is NULL, ROM matching operation is skipped.
*/
extern uint8_t ds18b20read( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom, int16_t *temperature ) ;

/**
	\brief Reads DS18B20 sensor's ROM address
	\param port A pointer to the port output register
	\param direction A pointer to the port direction register
	\param portin A pointer to the port input register
	\param mask A bit mask, determining to which pin the sensor is connected
	\param rom A pointer to the memory location where ROM address should be returned
	\param temperature A pointer to a 16-bit integer variable where temperature value should be returned
	\returns \ref DS18B20_ERROR_OK on success

	\warning This function works if only one sensors is connected. If more sensors are used, please see \ref ds18b20romsearch
*/
extern uint8_t ds18b20rom( volatile uint8_t *port, volatile uint8_t *direction, volatile uint8_t *portin, uint8_t mask, uint8_t *rom );

#ifdef __cplusplus
}
#endif
	
#endif
