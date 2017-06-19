# 	makefile - a part of avr-ds18b20 library
#
#	Copyright (C) 2016 Jacek Wieczorek
#
#	This software may be modified and distributed under the terms
#	of the MIT license.  See the LICENSE file for details.


F_CPU =
MCU =
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
endif

ifneq ($(MAKECMDGOALS),)
CFLAGS += -DDS18B20_MODULE_ROMSEARCH
endif

all: obj/ds18b20.o lib/libds18b20.a end

lib/libds18b20.a: obj/ds18b20.o
	avr-ar -cvq lib/libds18b20.a obj/ds18b20.o
	avr-ar -t  lib/libds18b20.a

obj/ds18b20.o: force obj/onewire.o obj/tmp/ds18b20.o obj/tmp/search.o
	$(LD) $(LDFLAGS) -r obj/onewire.o obj/tmp/ds18b20.o obj/tmp/search.o -o obj/ds18b20.o

obj/onewire.o: src/onewire.c include/ds18b20/onewire.h
	$(CC) $(CFLAGS) -c src/onewire.c -o obj/onewire.o

obj/tmp/ds18b20.o: src/ds18b20.c include/ds18b20/ds18b20.h
	$(CC) $(CFLAGS) -c src/ds18b20.c -o obj/tmp/ds18b20.o

obj/tmp/search.o: src/search.c include/ds18b20/search.h
	$(CC) $(CFLAGS) -c src/search.c -o obj/tmp/search.o

force:
	-mkdir obj
	-mkdir obj/tmp
	-mkdir lib

clean:
	-rm -rf obj
	-rm -rf lib

end:
	avr-size -C --mcu=$(MCU) obj/ds18b20.o
