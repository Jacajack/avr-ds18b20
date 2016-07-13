#This is just a very basic, sample makefile to show how to compile this library
#Of course, you can use your own setup, compiler warnings should be clear enough
#Atmega8 is well-known AVR mcu, so it's used here as example

FCPU = 16000000U
MCU = atmega8

CC = avr-gcc
CFLAGS = -DF_CPU=$(FCPU) -mmcu=$(MCU) -Os

LD = avr-ld
LDFLAGS =

all: obj/ds1820.o
	rm -rf obj/tmp
	avr-size -C --mcu=$(MCU) obj/ds1820.o

obj/ds1820.o: obj/onewire.o obj/tmp/ds1820.o FORCE
	$(LD) $(LDFLAGS) -r obj/onewire.o obj/tmp/ds1820.o -o obj/ds1820.o

obj/onewire.o: src/onewire.c include/onewire.h FORCE
	$(CC) $(CFLAGS) -c src/onewire.c -o obj/onewire.o

obj/tmp/ds1820.o: src/ds1820.c include/ds1820.h FORCE
	$(CC) $(CFLAGS) -c src/ds1820.c -o obj/tmp/ds1820.o

FORCE: clean
	mkdir obj
	mkdir obj/tmp

clean:
	rm -rf obj
