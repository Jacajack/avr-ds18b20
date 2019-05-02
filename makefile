# 	makefile - a part of avr-ds18b20 library
#
#	Copyright (C) 2016 Jacek Wieczorek
#
#	This software may be modified and distributed under the terms
#	of the MIT license.  See the LICENSE file for details.


F_CPU =
MCU =
MODULES = obj/onewire.o obj/ds18b20.o
MODULE_ROMSEARCH = 1
NO_AUTO_CLI =

CC = avr-gcc
CFLAGS = -Wall -Wpedantic -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Os -Iinclude

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
MODULES += obj/romsearch.o
else
$(warning ROM search module will NOT be included!)
endif

ifeq ($(NO_AUTO_CLI),)
#Hey, if you ask me why this is the default option - it's called 'backward compatibility' and I hate it
$(warning cli() will be called automatically in library calls! Please be aware that sometimes it might cause some unwanted behavior.)
CFLAGS += -DDS18B20_AUTO_CLI -DONEWIRE_AUTO_CLI
else
$(warning cli() will not be called automatically in library calls! You now have to manage interrupts all by yourself.)
endif

endif



all: force lib/libds18b20.a end

lib/libds18b20.a: $(MODULES)
	avr-ar -cvq lib/libds18b20.a $(MODULES)
	avr-ar -t  lib/libds18b20.a

obj/onewire.o: src/onewire.c include/ds18b20/onewire.h
	$(CC) $(CFLAGS) -c src/onewire.c -o obj/onewire.o

obj/ds18b20.o: src/ds18b20.c include/ds18b20/ds18b20.h
	$(CC) $(CFLAGS) -c src/ds18b20.c -o obj/ds18b20.o

obj/romsearch.o: src/romsearch.c include/ds18b20/romsearch.h
	$(CC) $(CFLAGS) -c src/romsearch.c -o obj/romsearch.o

force: clean
	-mkdir obj
	-mkdir lib

clean:
	-rm -rf obj
	-rm -rf lib

end:
	avr-size -C --mcu=$(MCU) lib/libds18b20.a
