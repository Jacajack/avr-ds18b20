# 	makefile - a part of avr-ds18b20 library
#
#	Copyright (C) 2016 Jacek Wieczorek
#
#	This software may be modified and distributed under the terms
#	of the MIT license.  See the LICENSE file for details.


F_CPU =
MCU =
MODULES = obj/onewire.o obj/tmp/ds18b20.o 
MODULE_ROMSEARCH = 

CC = avr-gcc
CFLAGS = -Wall -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Os

LD = avr-ld
LDFLAGS =

ifneq ($(MAKECMDGOALS),clean)
ifndef F_CPU
$(error F_CPU is not set!)
endif
ifndef MCU
$(error MCU is not set!)
endif
ifneq ($(MODULE_ROMSEARCH),)
$(warning ROM search module will be included!)
CFLAGS += -DDS18B20_MODULE_ROMSEARCH
MODULES += obj/tmp/romsearch.o
else
$(warning ROM search module will NOT be included!)
endif
endif



all: obj/ds18b20.o lib/libds18b20.a end

lib/libds18b20.a: obj/ds18b20.o
	avr-ar -cvq lib/libds18b20.a obj/ds18b20.o
	avr-ar -t  lib/libds18b20.a

obj/ds18b20.o: force $(MODULES)
	$(LD) $(LDFLAGS) -r $(MODULES) -o obj/ds18b20.o

obj/onewire.o: src/onewire.c include/ds18b20/onewire.h
	$(CC) $(CFLAGS) -c src/onewire.c -o obj/onewire.o

obj/tmp/ds18b20.o: src/ds18b20.c include/ds18b20/ds18b20.h
	$(CC) $(CFLAGS) -c src/ds18b20.c -o obj/tmp/ds18b20.o

obj/tmp/romsearch.o: src/romsearch.c include/ds18b20/romsearch.h
	$(CC) $(CFLAGS) -c src/romsearch.c -o obj/tmp/romsearch.o

force: clean
	-mkdir obj
	-mkdir obj/tmp
	-mkdir lib

clean:
	-rm -rf obj
	-rm -rf lib

end:
	avr-size -C --mcu=$(MCU) obj/ds18b20.o
