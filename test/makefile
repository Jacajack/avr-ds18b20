#Simple test for ATmega8

all:
	cd .. && make F_CPU=8000000U MCU=atmega8
	avr-gcc --std=c99 -mmcu=atmega8 -DF_CPU=8000000U ../obj/ds18b20.o test.c uart.c -Os -o test.elf
