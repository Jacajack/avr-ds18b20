# 	onewire.h
#
#	Copyright (C) 2016 Jacek Wieczorek
#
#	This software may be modified and distributed under the terms
#	of the MIT license.  See the LICENSE file for details.


F_CPU = 16000000U
MCU = atmega8

CC = avr-gcc
CFLAGS = -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Os

LD = avr-ld
LDFLAGS =

all: FORCE obj/ds1820.o
	rm -rf obj/tmp
	avr-size -C --mcu=$(MCU) obj/ds1820.o

obj/ds1820.o: obj/onewire.o obj/tmp/ds1820.o
	$(LD) $(LDFLAGS) -r obj/onewire.o obj/tmp/ds1820.o -o obj/ds1820.o

obj/onewire.o: src/onewire.c include/onewire.h
	$(CC) $(CFLAGS) -c src/onewire.c -o obj/onewire.o

obj/tmp/ds1820.o: src/ds1820.c include/ds1820.h
	$(CC) $(CFLAGS) -c src/ds1820.c -o obj/tmp/ds1820.o

FORCE: clean
	mkdir obj
	mkdir obj/tmp

clean:
	rm -rf obj
