MCU = atmega8
CC = avr-gcc
LD = avr-ld
FCPU = 16000000U

all: obj/Dallas1820.o
	rm -rf obj/tmp
	avr-size -C --mcu=$(MCU) obj/Dallas1820.o

obj/Dallas1820.o: obj/OneWire.o obj/tmp/Dallas1820.o FORCE
	$(LD) -r obj/OneWire.o obj/tmp/Dallas1820.o -o obj/Dallas1820.o

obj/OneWire.o: OneWire.c OneWire.h FORCE
	$(CC) -DF_CPU=$(FCPU) -mmcu=$(MCU) -Os -c OneWire.c -o obj/OneWire.o

obj/tmp/Dallas1820.o: Dallas1820.c Dallas1820.h FORCE
	$(CC) -DF_CPU=$(FCPU) -mmcu=$(MCU) -Os -c Dallas1820.c -o obj/tmp/Dallas1820.o

clean:
	rm -rf obj

FORCE: clean
	mkdir obj
	mkdir obj/tmp
