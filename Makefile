build:
	avr-gcc -mmcu=atmega328p -DF_CPU=1000000UL -Os -o main.elf main.c
	avr-objcopy -O ihex -R .eeprom main.elf main.hex

flash:
	avrdude -p m328p -c arduino_as_isp -P /dev/cu.usbmodem1401 -b 19200 -U flash:w:main.hex:i -F

ping:
	avrdude -p m328pb -c arduino_as_isp -P /dev/cu.usbmodem1401 -b 19200 -v
